#include "EegeoPlatform.h"

#include "GlobalLighting.h"
#include "GlobalFogging.h"
#include "EnvironmentFlatteningService.h"
#include "JpegLoader.h"
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
#include "LoadingScreen.h"
#include "LocalAsyncTextureLoader.h"
#include "Model.h"
#include "NullMaterialFactory.h"
#include "RenderableFilters.h"
#include "Node.h"
#include "CityThemesModule.h"
#include "EffectHandler.h"
#include "PackedRenderableFilter.h"
#include "PlaceNamesViewFilter.h"
#include "RoadNamesRenderableFilter.h"

namespace
{
    Eegeo::Rendering::LoadingScreen* CreateLoadingScreen(const Eegeo::Modules::Core::RenderingModule& renderingModule,
                                                         Eegeo::Helpers::ITextureFileLoader& textureFileLoader,
                                                         const Eegeo::Rendering::ScreenProperties& screenProperties)
    {
        Eegeo::Rendering::LoadingScreenConfig loadingScreenConfig;
        loadingScreenConfig.loadingBarBackgroundColor = Eegeo::v4(0.45f, 0.7f, 1.0f, 1.0f);
        loadingScreenConfig.fadeOutDurationSeconds = 1.5f;
        loadingScreenConfig.screenWidth = screenProperties.GetScreenWidth();
        loadingScreenConfig.screenHeight = screenProperties.GetScreenHeight();
        loadingScreenConfig.backgroundColor = Eegeo::v4(132.f/255.f, 203.f/255.f, 235.f/255.f, 1.f);
        loadingScreenConfig.loadingBarOffset = Eegeo::v2(0.5f, 0.1f);
        loadingScreenConfig.layout = Eegeo::Rendering::LoadingScreenLayout::Centred;
        
        
        Eegeo::Rendering::LoadingScreen* pLoadingScreen = Eegeo::Rendering::LoadingScreen::Create("SplashScreen-1024x768.png",
                                                                                                  loadingScreenConfig,
                                                                                                  renderingModule.GetShaderIdGenerator(),
                                                                                                  renderingModule.GetMaterialIdGenerator(),
                                                                                                  renderingModule.GetGlBufferPool(),
                                                                                                  renderingModule.GetVertexLayoutPool(),
                                                                                                  renderingModule.GetVertexBindingPool(),
                                                                                                  textureFileLoader);
        
        return pLoadingScreen;
    }
}

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
    , m_pLoadingScreen(NULL)
    {
        Eegeo::TtyHandler::TtyEnabled = true;

        m_pJpegLoader = new Eegeo::Helpers::Jpeg::JpegLoader();
        
        Eegeo::EffectHandler::Initialise();
        
        const Eegeo::EnvironmentCharacterSet::Type environmentCharacterSet = Eegeo::EnvironmentCharacterSet::JapanPlaceNames;
        
        Eegeo::Config::PlatformConfig config = (new OSX::OSXPlatformConfigBuilder())->Build();
        config.GraphicsConfig.AlwaysUseHighFidelityWaterShader = true;
        
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
                                         *m_pOSXNativeUIFactories,
                                         environmentCharacterSet,
                                         config);
        
        Eegeo::Modules::Map::MapModule& mapModule = m_pWorld->GetMapModule();
               
        m_pStreamingVolume = Eegeo_NEW(Eegeo::Streaming::CameraFrustumStreamingVolume)(mapModule.GetResourceCeilingProvider(),
                                                                                       Eegeo::Config::LodRefinementConfig::GetLodRefinementAltitudesForDeviceSpec(config.PerformanceConfig.DeviceSpecification),
                                                                                       Eegeo::Streaming::QuadTreeCube::MAX_DEPTH_TO_VISIT,
                                                                                       mapModule.GetEnvironmentFlatteningService());

        m_pLoadingScreen = CreateLoadingScreen(m_pWorld->GetRenderingModule(), m_pOSXPlatformAbstractionModule->GetTextureFileLoader(), screenProperties);
        
        // Uses altitude LOD refinement up until first level with buildings, from there it does distance based LOD selection
        m_pStreamingVolume->setDeepestLevelForAltitudeLodRefinement(11);
        
        m_pWorld->GetMapModule().GetShadowPresentationModule().GetShadowRenderableFilter().SetEnabled(true);
        m_pWorld->GetMapModule().GetPlaceNamesPresentationModule().GetPlaceNamesViewFilter().SetEnabled(false);
        m_pWorld->GetMapModule().GetTransportPresentationModule().GetRoadNamesRenderableFilter().SetEnabled(false);

        m_pModel = Eegeo::Model::CreateFromPODFile("Test_ROBOT_ARM.pod", m_pOSXPlatformAbstractionModule->GetFileIO(), &(m_pWorld->GetAsyncLoadersModule().GetLocalAsyncTextureLoader()), "");
        Eegeo_ASSERT(m_pModel->GetRootNode());
        
        m_pNullMaterial = m_pWorld->GetRenderingModule().GetNullMaterialFactory().Create("PODAnimationExampleNullMaterial");
        
        m_pRobotArmRenderable = Eegeo_NEW (RobotArmRenderable)(*m_pModel, m_pWorld->GetLightingModule().GetGlobalFogging(), *m_pNullMaterial);
        m_pRobotArmFilter = Eegeo_NEW (RobotArmFilter)(*m_pRobotArmRenderable);
        m_pWorld->GetRenderingModule().GetRenderableFilters().AddRenderableFilter(*m_pRobotArmFilter);
    }
    
    Platform::~Platform()
    {
        m_pWorld->OnPause();
        m_pWorld->GetRenderingModule().GetRenderableFilters().RemoveRenderableFilter(*m_pRobotArmFilter);
        delete m_pRobotArmFilter;
        delete m_pRobotArmRenderable;
        delete m_pLoadingScreen;
        delete m_pWorld;
        delete m_pOSXPlatformAbstractionModule;
        delete m_pOSXNativeUIFactories;
        delete m_pOSXLocationService;
        delete m_pJpegLoader;
    }

    void Platform::NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties)
    {
        m_screenProperties = screenProperties;
        
        if (m_pLoadingScreen != NULL)
        {
            m_pLoadingScreen->NotifyScreenDimensionsChanged(m_screenProperties.GetScreenWidth(), m_screenProperties.GetScreenHeight());
        }
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
        UpdateLoadingScreen(dt);
        m_pModel->UpdateAnimator(dt*0.4f);
    }
    
    
    void Platform::Draw(Eegeo::OVR::OVREegeoCameraController& ovrCamera)
    {
        Eegeo::Camera::CameraState cameraState = ovrCamera.GetCameraState();
        Eegeo::EegeoDrawParameters drawParams(cameraState.LocationEcef(),
                                              ovrCamera.GetEcefInterestPoint(),
                                              cameraState.ViewMatrix(),
                                              cameraState.ProjectionMatrix(),
                                              m_screenProperties);
        
        m_pRobotArmRenderable->UpdateObserverLocation(cameraState.LocationEcef());
        
        m_pWorld->Draw(drawParams);

        if (m_pLoadingScreen != NULL)
        {
            m_pLoadingScreen->Draw();
        }
    }
    
    void Platform::UpdateLoadingScreen(float dt)
    {
        if (m_pLoadingScreen == NULL)
            return;
        
        if (!m_pWorld->Initialising() && !m_pLoadingScreen->IsDismissed())
        {
            m_pLoadingScreen->Dismiss();
        }
        
        m_pLoadingScreen->SetProgress(m_pWorld->GetInitialisationProgress());
        m_pLoadingScreen->Update(dt);
        
        if (!m_pLoadingScreen->IsVisible())
        {
            Eegeo_DELETE m_pLoadingScreen;
            m_pLoadingScreen = NULL;
        }
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
