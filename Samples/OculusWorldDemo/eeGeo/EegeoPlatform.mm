#include "EegeoPlatform.h"

#include "GlobalLighting.h"
#include "GlobalFogging.h"
#include "EnvironmentFlatteningService.h"
#include "JpegLoader.h"
#include "OSXWebRequestService.h"
#include "OSXGlTaskContextFactory.h"
#include "OSXPlatformConfigBuilder.h"
#include "OSXLocationService.h"
#include "OSXStubUI.h"
#include "NavigationService.h"
#include "GpsGlobeCameraControllerFactory.h"
#include "GpsGlobeCameraController.h"
#include "StringHelpers.h"
#include "GpsGlobeCameraComponentConfiguration.h"
#include "GlobeCameraTouchControllerConfiguration.h"
#include "GlobeCameraControllerConfiguration.h"
#include "GlobeCameraTouchSettings.h"
#include "GlobeCameraController.h"
#include "StreamingVolumeController.h"
#include "GlobalFogging.h"
#include "CityThemesService.h"
#include "RenderCamera.h"
#include "CameraFrustumStreamingVolume.h"
#include "LodRefinementConfig.h"
#include "QuadTreeCube.h"
#include "StreamingController.h"
#include "StreamingModule.h"
#include "MapModule.h"
#include "TerrainModelModule.h"
#include "LightingModule.h"

#include "PackedRenderableFilter.h"
#include "PlaceNamesViewFilter.h"
#include "RoadNamesRenderableFilter.h"

namespace Eegeo
{
    namespace
    {
        namespace
        {
            const v3 dayClearColor(135/255.0f, 206/255.0f, 235/255.0f);
            const v3 nightClearColor(0/255.f,24/255.f,72/255.f);
        }
    }
    
    Platform::Platform(Eegeo::Rendering::ScreenProperties& screenProperties, NSOpenGLPixelFormat* pPixelFormat)
    : m_night(false)
    , m_nightTParam(0.f)
    , m_currentClearColor(dayClearColor)
    , m_startClearColor(nightClearColor)
    , m_destClearColor(dayClearColor)
    {
        Eegeo::TtyHandler::TtyEnabled = true;

        m_pJpegLoader = new Eegeo::Helpers::Jpeg::JpegLoader();
        
        Eegeo::EffectHandler::Initialise();
        m_pBlitter = new Eegeo::Blitter(1024 * 128, 1024 * 64, 1024 * 32, screenProperties.GetScreenWidth(), screenProperties.GetScreenHeight());
        m_pBlitter->Initialise();
        
        const Eegeo::EnvironmentCharacterSet::Type environmentCharacterSet = Eegeo::EnvironmentCharacterSet::JapanPlaceNames;
        
        Eegeo::Config::PlatformConfig config = (new OSX::OSXPlatformConfigBuilder())->Build();
        
        m_pOSXLocationService = new Eegeo::OSX::OSXLocationService();
        
        OSX::OSXAlertBoxFactory* alertBox = new OSX::OSXAlertBoxFactory();
        OSX::OSXInputBoxFactory* inputBox = new OSX::OSXInputBoxFactory();
        OSX::OSXKeyboardInputFactory* keyboardInputFactory = new OSX::OSXKeyboardInputFactory();
        
        m_pOSXNativeUIFactories = new UI::NativeUIFactories(*alertBox, *inputBox, *keyboardInputFactory);
        
        const std::string placeholderTextureResource = "placeholder.png";
        const std::string apiKey = "2207a928392ebe113122fce9e16c3a48";
        
        m_pOSXPlatformAbstractionModule = new Eegeo::OSX::OSXPlatformAbstractionModule(*m_pJpegLoader,
                                                                                       pPixelFormat,
                                                                                       "OculusWorldDemo.app/Contents/Resources/Assets/eeGeo/",
                                                                                       apiKey);
        
        m_pWorld = new Eegeo::EegeoWorld(apiKey,
                                         *m_pOSXPlatformAbstractionModule,
                                         *m_pJpegLoader,
                                         screenProperties,
                                         *m_pOSXLocationService,
                                         *m_pBlitter,
                                         *m_pOSXNativeUIFactories,
                                         environmentCharacterSet,
                                         config,
                                         NULL,
                                         "http://cdn1.eegeo.com/coverage-trees/v427_zdc/manifest.txt.gz",
                                         "http://cdn1.eegeo.com/test-manifests/manifest-fudged-fog.txt.gz");
        
        m_pOSXPlatformAbstractionModule->SetWebRequestServiceWorkPool(m_pWorld->GetWorkPool());
        
        Eegeo::Modules::Map::MapModule& mapModule = m_pWorld->GetMapModule();
               
        m_pStreamingVolume = Eegeo_NEW(Eegeo::Streaming::CameraFrustumStreamingVolume)(mapModule.GetResourceCeilingProvider(),
                                                                                       Eegeo::Config::LodRefinementConfig::GetLodRefinementAltitudesForDeviceSpec(config.PerformanceConfig.DeviceSpecification),
                                                                                       Eegeo::Streaming::QuadTreeCube::MAX_DEPTH_TO_VISIT,
                                                                                       mapModule.GetEnvironmentFlatteningService());
        
        // Uses altitude LOD refinement up until first level with buildings, from there it does distance based LOD selection
        m_pStreamingVolume->setDeepestLevelForAltitudeLodRefinement(12);
        
        m_pWorld->GetMapModule().GetShadowPresentationModule().GetShadowRenderableFilter().SetEnabled(false);
        m_pWorld->GetMapModule().GetPlaceNamesPresentationModule().GetPlaceNamesViewFilter().SetEnabled(false);
        m_pWorld->GetMapModule().GetTransportPresentationModule().GetRoadNamesRenderableFilter().SetEnabled(false);

    }
    
    Platform::~Platform()
    {
        m_pWorld->OnPause();
        delete m_pWorld;
        delete m_pOSXPlatformAbstractionModule;
        delete m_pOSXNativeUIFactories;
        delete m_pOSXLocationService;
        delete m_pBlitter;
        delete m_pJpegLoader;
    }

    void Platform::NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties)
    {
        m_screenProperties = screenProperties;
    }
    
    void Platform::Update(float dt, Eegeo::OVR::OVREegeoCameraController& ovrCamera)
    {
        Eegeo::EegeoWorld& eegeoWorld(*m_pWorld);
        
        eegeoWorld.EarlyUpdate(dt);
        
        Eegeo::Camera::CameraState cameraState = ovrCamera.GetCameraState();
        Eegeo::Camera::RenderCamera renderCamera = ovrCamera.GetCamera();
        
        std::vector<Eegeo::Geometry::Plane> frustumPlanes(Eegeo::Geometry::Frustum::PLANES_COUNT);
        BuildFrustumPlanesFromViewProjection(frustumPlanes, renderCamera.GetViewProjectionMatrix());
        const double d = renderCamera.GetAltitude() * Eegeo::Streaming::StreamingVolumeController::CAMERA_ALTITUDE_TO_FAR_PLANE_DISTANCE_MULTIPLIER;
        const double cameraFarPlaneD = fmin(fmax(d, Eegeo::Streaming::StreamingVolumeController::MIN_STREAMING_FAR_PLANE_DISTANCE),
                                            frustumPlanes[Eegeo::Geometry::Frustum::PLANE_FAR].d);
        frustumPlanes[Eegeo::Geometry::Frustum::PLANE_FAR].d = static_cast<float>(cameraFarPlaneD);
        
        m_pStreamingVolume->updateStreamingVolume(renderCamera.GetEcefLocation(), frustumPlanes, renderCamera.GetFOV());
        m_pStreamingVolume->ResetVolume(cameraState.InterestPointEcef());
        
        Eegeo::EegeoUpdateParameters updateParams(dt,
                                                  cameraState.LocationEcef(),
                                                  cameraState.InterestPointEcef(),
                                                  cameraState.ViewMatrix(),
                                                  cameraState.ProjectionMatrix(),
                                                  *m_pStreamingVolume,
                                                  m_screenProperties);        
        eegeoWorld.Update(updateParams);
        
        UpdateNightTParam(dt);
        UpdateFogging();
    }
    
    
    void Platform::Draw(Eegeo::OVR::OVREegeoCameraController& ovrCamera)
    {
        Eegeo::Camera::CameraState cameraState = ovrCamera.GetCameraState();
        Eegeo::EegeoDrawParameters drawParams(cameraState.LocationEcef(),
                                              ovrCamera.GetEcefInterestPoint(),
                                              cameraState.ViewMatrix(),
                                              cameraState.ProjectionMatrix(),
                                              m_screenProperties);
        m_pWorld->Draw(drawParams);
    }
    
    void Platform::UpdateNightTParam(float dt)
    {
        m_nightTParam += dt;
        m_nightTParam = Math::Clamp01(m_nightTParam);
        m_currentClearColor = v3::Lerp(m_startClearColor, m_destClearColor, m_nightTParam);
    }
    
    void Platform::UpdateFogging()
    {
        Eegeo::Lighting::GlobalFogging& fogging = m_pWorld->GetCoreModule().GetLightingModule().GetGlobalFogging();
        fogging.SetHeightFogIntensity(0.0f);
        fogging.SetDistanceFogIntensity(1.0f);
        fogging.SetDistanceFogDistances(m_foggingFar - 500.0f, m_foggingFar);
        fogging.SetFogColour(Eegeo::v4(m_currentClearColor,1.0f));
        fogging.SetFogDensity(1.0f);
    }
    
    void Platform::ToggleNight()
    {
        m_night = !m_night;
        std::stringstream themeNameBuidler;
        themeNameBuidler << (m_night ? "Night" : "Day");
        themeNameBuidler << "Default";
        m_pWorld->GetMapModule().GetCityThemesModule().GetCityThemesService().RequestTransitionToState(themeNameBuidler.str(), 1.f);
        
        if (m_night)
        {
            m_startClearColor = dayClearColor;
            m_destClearColor = nightClearColor;
        }
        else
        {
            m_startClearColor = nightClearColor;
            m_destClearColor = dayClearColor;
        }
        m_nightTParam = 0.f;
    }
    
    void Platform::ToggleStreaming()
    {
        bool streamingEnabled = m_pWorld->GetMapModule().GetStreamingModule().GetStreamingController().Enabled();
        m_pWorld->GetMapModule().GetStreamingModule().GetStreamingController().SetEnabled(!streamingEnabled);
    }
    
    void Platform::SetFoggingFar(float far)
    {
        m_foggingFar = far;
    }
}