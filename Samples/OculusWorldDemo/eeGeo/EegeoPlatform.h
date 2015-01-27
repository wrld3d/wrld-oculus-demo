#pragma once

#include "Types.h"
#include "Graphics.h"
#include "OSXFileIO.h"
#include "OSXHttpCache.h"
#include "OSXTextureFileLoader.h"
#include "OSXWebLoadRequestFactory.h"
#include "OSXUrlEncoder.h"
#include "OSXLocationService.h"
#include "GlobeCameraInterestPointProvider.h"
#include "TerrainHeightProvider.h"
#include "NativeUIFactories.h"
#include "TerrainHeightRepository.h"
#include "GlobalShadowing.h"
#include "TouchEventWrapper.h"
#include "Blitter.h"
#include "SearchServiceCredentials.h"
#include "IJpegLoader.h"
#include "EegeoWorld.h"
#include "NativeUIFactories.h"
#include "OSXPlatformAbstractionModule.h"
#include "ScreenProperties.h"
#include "CameraState.h"
#include "IStreamingVolume.h"
#include "CameraFrustumStreamingVolume.h"
#include "OVREegeoCameraController.h"

#import <OpenGL/OpenGL.h>
#import <Cocoa/Cocoa.h>

namespace Eegeo
{
    class Platform
    {
    public:
        Platform(Eegeo::Rendering::ScreenProperties& screenProperties, NSOpenGLPixelFormat* pPixelFormat);
        ~Platform();
        
        void Update(float dt, Eegeo::OVR::OVREegeoCameraController& ovrCamera);
        void Draw(Eegeo::OVR::OVREegeoCameraController& ovrCamera);
        
        void SetFoggingFar(float far);
        
        void SetCamera(Camera::RenderCamera* camera);
        void ToggleStreaming();
        void ToggleNight();
        
        const v3& GetClearColor() { return m_currentClearColor; }
        
        void NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties);
        
        Resources::Terrain::Heights::TerrainHeightProvider& GetTerrainHeightProvider() { return m_pWorld->GetMapModule().GetTerrainModelModule().GetTerrainHeightProvider();}
        
        ITouchController& GetTouchController();
    private:
        Eegeo::Blitter* m_pBlitter;
        Eegeo::Helpers::Jpeg::IJpegLoader* m_pJpegLoader;
        Eegeo::Location::ILocationService* m_pOSXLocationService;
        UI::NativeUIFactories* m_pOSXNativeUIFactories;
        Eegeo::OSX::OSXPlatformAbstractionModule * m_pOSXPlatformAbstractionModule;
        
        Eegeo::Rendering::ScreenProperties m_screenProperties;
        Eegeo::Streaming::CameraFrustumStreamingVolume* m_pStreamingVolume;
                
        Eegeo::EegeoWorld* m_pWorld;
        
        void UpdateNightTParam(float dt);
        void UpdateFogging();
        bool m_night;
        float m_nightTParam;
        float m_foggingFar;
        v3 m_currentClearColor;
        v3 m_startClearColor;
        v3 m_destClearColor;
    };
}