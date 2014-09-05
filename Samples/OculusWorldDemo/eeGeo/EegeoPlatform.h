#pragma once

#include "Types.h"
#include "Graphics.h"
#include "TerrainHeightProvider.h"
#include "NativeUIFactories.h"
#include "TerrainHeightRepository.h"
#include "Blitter.h"
#include "SearchServiceCredentials.h"
#include "IJpegLoader.h"
#include "EegeoWorld.h"
#include "NativeUIFactories.h"
#include "OSXPlatformAbstractionModule.h"
#include "RenderCamera.h"
#include "ILocationService.h"

namespace Eegeo
{
    class Platform
    {
    public:
        Platform(Location::IInterestPointProvider&, float screenWidth, float screenHeight, float pixelScale, float dpi, NSOpenGLPixelFormat* pPixelFormat);
        ~Platform();
        
        void Update(float dt);
        void Draw(float dt, Camera::RenderCamera* camera);
        
        void SetCamera(Camera::RenderCamera* camera);
        void ToggleStreaming();
        void ToggleNight();
        
        const v3& GetClearColor() { return m_currentClearColor; }
        
        Resources::Terrain::Heights::TerrainHeightProvider& GetTerrainHeightProvider() { return m_terrainHeightProvider;}
    private:
        Eegeo::Blitter* m_pBlitter;
        Eegeo::Helpers::Jpeg::IJpegLoader* m_pJpegLoader;
        Eegeo::Rendering::RenderContext* m_pRenderContext;
        Eegeo::Location::ILocationService* m_pOSXLocationService;
        UI::NativeUIFactories* m_pOSXNativeUIFactories;
        Eegeo::OSX::OSXPlatformAbstractionModule * m_pOSXPlatformAbstractionModule;
        
        Eegeo::Resources::Terrain::Heights::TerrainHeightRepository m_terrainHeightRepository;
        Eegeo::Resources::Terrain::Heights::TerrainHeightProvider m_terrainHeightProvider;
        
        Eegeo::EegeoWorld* m_pWorld;
        
        void UpdateNightTParam(float dt);
        bool m_night;
        float m_nightTParam;
        v3 m_currentClearColor;
        v3 m_startClearColor;
        v3 m_destClearColor;
    };
}