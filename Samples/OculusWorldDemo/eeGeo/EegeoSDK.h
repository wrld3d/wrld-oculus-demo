#pragma once

#include "Types.h"
#include "Graphics.h"
#include "GlobeCameraInterestPointProvider.h"
#include "TerrainHeightProvider.h"
#include "NativeUIFactories.h"
#include "TerrainHeightRepository.h"
#include "GlobalShadowing.h"
#include "TouchEventWrapper.h"
#include "Blitter.h"
#include "SearchServiceCredentials.h"
#include "EegeoWorld.h"
#include "NativeUIFactories.h"
#include "ScreenProperties.h"
#include "CameraState.h"
#include "IStreamingVolume.h"
#include "CameraFrustumStreamingVolume.h"
#include "OVREegeoCameraController.h"
#include "RobotArm.h"
#include "PlatformConfig.h"

namespace Eegeo
{
    class Platform
    {
    public:
        Platform(Eegeo::Rendering::ScreenProperties& screenProperties,
                 Modules::IPlatformAbstractionModule& platformAbstraction,
                 Eegeo::Helpers::Jpeg::IJpegLoader& jpegLoader,
                 Eegeo::Location::ILocationService& locationService,
                 Eegeo::UI::NativeUIFactories& nativeInputFactories,
                 Eegeo::Config::PlatformConfig config
                 );
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
        
        Eegeo::DebugRendering::DebugRenderer& GetDebugRenderer() { return m_pWorld->GetDebugRenderingModule().GetDebugRenderer(); }
        
        ITouchController& GetTouchController();
    private:
        void UpdateLoadingScreen(float dt);
        
        Eegeo::Model* m_pModel;
        Eegeo::Rendering::Materials::NullMaterial* m_pNullMaterial;
        
        RobotArmRenderable* m_pRobotArmRenderable;
        RobotArmFilter* m_pRobotArmFilter;
        
        Eegeo::Blitter* m_pBlitter;
        
        Eegeo::Rendering::ScreenProperties m_screenProperties;
        Eegeo::Streaming::CameraFrustumStreamingVolume* m_pStreamingVolume;
        
        Eegeo::EegeoWorld* m_pWorld;
        
        Eegeo::Rendering::LoadingScreen* m_pLoadingScreen;
        
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