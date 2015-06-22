#pragma once

#include "RenderableBase.h"
#include "Model.h"
#include "VectorMath.h"
#include "GlobalFogging.h"
#include "IRenderableFilter.h"

namespace Eegeo
{
    class RobotArmRenderable : public Eegeo::Rendering::RenderableBase
    {
        Eegeo::Model& m_model;
        Eegeo::Lighting::GlobalFogging& m_globalFogging;
        Eegeo::dv3 m_observerLocationEcef;
        
    public:
        RobotArmRenderable(Eegeo::Model& model,
                           Eegeo::Lighting::GlobalFogging& globalFogging,
                           Eegeo::Rendering::Materials::NullMaterial& nullMaterial);
        
        void UpdateObserverLocation(const Eegeo::dv3& observerLocationEcef);
        
        void Render(Eegeo::Rendering::GLState& glState) const;
    };
    
    class RobotArmFilter : public Eegeo::Rendering::IRenderableFilter
    {
        Eegeo::Rendering::RenderableBase& m_renderable;
    public:
        RobotArmFilter(Eegeo::Rendering::RenderableBase& renderable);
        
        void EnqueueRenderables(const Eegeo::Rendering::RenderContext& renderContext, Eegeo::Rendering::RenderQueue& renderQueue);
    };
}