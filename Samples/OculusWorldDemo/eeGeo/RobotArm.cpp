#include "RobotArm.h"
#include "RenderQueue.h"
#include "LatLongAltitude.h"
#include "NullMaterial.h"
#include "Node.h"
#include "EffectHandler.h"

namespace Eegeo
{
    RobotArmRenderable::RobotArmRenderable(Eegeo::Model& model,
                                                   Eegeo::Lighting::GlobalFogging& globalFogging,
                                                   Eegeo::Rendering::Materials::NullMaterial& nullMat)
    : Eegeo::Rendering::RenderableBase(Eegeo::Rendering::LayerIds::Buildings,
                                       Eegeo::Space::LatLongAltitude::FromDegrees(37.8109, -122.4084, 0.0f).ToECEF(),
                                       &nullMat)
    , m_model(model)
    , m_globalFogging(globalFogging)
    , m_observerLocationEcef()
    {
        
    }

    RobotArmFilter::RobotArmFilter(Eegeo::Rendering::RenderableBase& renderable)
    : m_renderable(renderable)
    {
        
    }

    void RobotArmFilter::EnqueueRenderables(const Eegeo::Rendering::RenderContext &renderContext, Eegeo::Rendering::RenderQueue &renderQueue)
    {
        renderQueue.EnqueueRenderable(m_renderable);
    }

    void RobotArmRenderable::UpdateObserverLocation(const Eegeo::dv3& observerLocationEcef)
    {
        m_observerLocationEcef = observerLocationEcef;
    }

    void RobotArmRenderable::Render(Eegeo::Rendering::GLState &glState) const
    {
        //create basis around a known location off coast of SF
        Eegeo::m44 transform;
        Eegeo::dv3 location = m_ecefPosition;
        Eegeo::v3 up(location.Norm().ToSingle());
        Eegeo::v3 forward = (location  - Eegeo::v3(0.f, 1.f, 0.f)).Norm().ToSingle();
        Eegeo::v3 right(Eegeo::v3::Cross(up, forward).Norm());
        forward = Eegeo::v3::Cross(up, right);
        Eegeo::v3 cameraRelativePos = (location - m_observerLocationEcef).ToSingle();
        
        Eegeo::m44 scaleMatrix;
        scaleMatrix.Scale(1.f);
        Eegeo::m44 cameraRelativeTransform;
        cameraRelativeTransform.SetFromBasis(right, up, forward, cameraRelativePos);
        Eegeo::m44::Mul(transform, cameraRelativeTransform, scaleMatrix);
        transform.SetRow(3, Eegeo::v4(cameraRelativePos, 1.f));
        
        glState.DepthTest.Enable();
        glState.DepthFunc(GL_LEQUAL);
        
        //loaded model faces are ccw
        glState.FrontFace(GL_CCW);
        
        m_model.GetRootNode()->SetVisible(true);
        m_model.GetRootNode()->SetLocalMatrix(transform);
        m_model.GetRootNode()->UpdateRecursive();
        m_model.GetRootNode()->UpdateSphereRecursive();
        m_model.GetRootNode()->DrawRecursive(glState, m_globalFogging, NULL, true, false);
        
        glState.FrontFace(GL_CW);
        
        Eegeo::EffectHandler::Reset();
    }
}