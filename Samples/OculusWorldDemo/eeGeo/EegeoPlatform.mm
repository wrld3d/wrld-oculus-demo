#include "EegeoPlatform.h"

#include "JpegLoader.h"
#include "OSXPlatformConfigBuilder.h"
#include "OSXStubUI.h"
#include "StringHelpers.h"
#include "RenderCamera.h"
#include "StreamingController.h"
#include "CityThemesService.h"
#include "MathFunc.h"
#include "RenderContext.h"
#include "OSXLocationService.h"
#include "GlobalFogging.h"
#include "StreamingVolumeController.h"

#include <Cocoa/Cocoa.h>

namespace Eegeo
{
    namespace
    {
        const v3 dayClearColor(135/255.0f, 206/255.0f, 235/255.0f);
        const v3 nightClearColor(0/255.f,24/255.f,72/255.f);
    }
    
    Platform::Platform(Location::IInterestPointProvider& interestPointProvider, float screenWidth, float screenHeight, float pixelScale, float dpi, NSOpenGLPixelFormat* pPixelFormat)
    : m_pBlitter(NULL)
    , m_pJpegLoader(NULL)
    , m_pRenderContext(NULL)
    , m_pOSXLocationService(NULL)
    , m_pOSXNativeUIFactories(NULL)
    , m_pOSXPlatformAbstractionModule(NULL)
    , m_night(false)
    , m_nightTParam(0.f)
    , m_currentClearColor(dayClearColor)
    , m_startClearColor(nightClearColor)
    , m_destClearColor(dayClearColor)
    {
        m_pRenderContext = new Eegeo::Rendering::RenderContext();
        m_pRenderContext->SetScreenDimensions(screenWidth, screenHeight, pixelScale, dpi);
        
        m_pJpegLoader = new Eegeo::Helpers::Jpeg::JpegLoader();
        
        Eegeo::EffectHandler::Initialise();
        m_pBlitter = new Eegeo::Blitter(1024 * 128, 1024 * 64, 1024 * 32, *m_pRenderContext);
        m_pBlitter->Initialise();
        
        const Eegeo::EnvironmentCharacterSet::Type environmentCharacterSet = Eegeo::EnvironmentCharacterSet::JapanPlaceNames;
        
        Eegeo::Config::PlatformConfig config = (new OSX::OSXPlatformConfigBuilder())->Build();
        
        m_pOSXLocationService = new Eegeo::OSX::OSXLocationService();
        
        OSX::OSXAlertBoxFactory* alertBox = new OSX::OSXAlertBoxFactory();
        OSX::OSXInputBoxFactory* inputBox = new OSX::OSXInputBoxFactory();
        OSX::OSXKeyboardInputFactory* keyboardInputFactory = new OSX::OSXKeyboardInputFactory();
        
        m_pOSXNativeUIFactories = new UI::NativeUIFactories(*alertBox, *inputBox, *keyboardInputFactory);

        m_pOSXPlatformAbstractionModule = new Eegeo::OSX::OSXPlatformAbstractionModule(m_pRenderContext->GetGLState(),
                                                                                       *m_pJpegLoader,
                                                                                       pPixelFormat,
                                                                                       "OculusWorldDemo.app/Contents/Resources/Assets/eeGeo/");
        const std::string apiKey = "5d251555072dbe44979bdd6613fbec76";

        m_pWorld = new Eegeo::EegeoWorld(apiKey,
                                         *m_pOSXPlatformAbstractionModule,
                                         *m_pJpegLoader,
                                         *m_pRenderContext,
                                         *m_pOSXLocationService,
                                         *m_pBlitter,
                                         interestPointProvider,
                                         *m_pOSXNativeUIFactories,
                                         environmentCharacterSet,
                                         config,
                                         NULL,
                                         "Default-Landscape@2x~ipad.png",
                                         "http://cdn1.eegeo.com/coverage-trees/v417/manifest.txt.gz",
                                         "http://d2xvsc8j92rfya.cloudfront.net/mobile-themes-new/v193/manifest.txt.gz");
        
        m_pOSXPlatformAbstractionModule->SetWebRequestServiceWorkPool(m_pWorld->GetWorkPool());
        
        // Uses altitude LOD refinement up until first level with buildings, from there it does distance based LOD selection
        m_pWorld->GetStreamingVolumeController().setDeepestLevelForAltitudeLodRefinement(12);
        
        m_pRenderContext->GetGLState().InvalidateAll();
    }
    
    Platform::~Platform()
    {
        delete m_pWorld;
        delete m_pOSXPlatformAbstractionModule;
        delete m_pOSXNativeUIFactories;
        delete m_pOSXLocationService;
        delete m_pBlitter;
        delete m_pJpegLoader;
        delete m_pRenderContext;
    }
    
    void Platform::UpdateNightTParam(float dt)
    {
        m_nightTParam += dt;
        m_nightTParam = Math::Clamp01(m_nightTParam);
        m_currentClearColor = v3::Lerp(m_startClearColor, m_destClearColor, m_nightTParam);
    }
    
    void Platform::UpdateFogging()
    {
        m_pWorld->GetGlobalFogging().SetHeightFogIntensity(0.0f);
        m_pWorld->GetGlobalFogging().SetDistanceFogIntensity(1.0f);
        m_pWorld->GetGlobalFogging().SetDistanceFogDistances(m_foggingFar - 500.0f, m_foggingFar);
        m_pWorld->GetGlobalFogging().SetFogColour(Eegeo::v4(m_currentClearColor,1.0f));
        m_pWorld->GetGlobalFogging().SetFogDensity(1.0f);
    }
    
    void Platform::ToggleNight()
    {
        m_night = !m_night;
        std::stringstream themeNameBuidler;
        themeNameBuidler << (m_night ? "Night" : "Day");
        themeNameBuidler << "Default";
        m_pWorld->GetCityThemesService().RequestTransitionToState(themeNameBuidler.str(), 1.f);
        
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
        bool streamingEnabled = m_pWorld->GetStreamingController().Enabled();
        m_pWorld->GetStreamingController().SetEnabled(!streamingEnabled);
    }
    
    void Platform::Update(float dt)
    {
        m_pWorld->EarlyUpdate(dt);
        m_pWorld->Update(dt);
        UpdateNightTParam(dt);
        UpdateFogging();
    }
    
    void Platform::SetCamera(Camera::RenderCamera* camera)
    {
        m_pWorld->SetCamera(camera);
    }
    
    void Platform::Draw(float dt, Camera::RenderCamera* camera)
    {
        m_pWorld->SetCamera(camera);
        m_pWorld->Draw(dt);
    }
    
    void Platform::SetFoggingFar(float far)
    {
        m_foggingFar = far;
    }
}
