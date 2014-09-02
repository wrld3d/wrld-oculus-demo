//  Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#pragma once
#include "Types.h"
#include "RenderCamera.h"
#include "VectorMathDecl.h"
#include "Space.h"
#include "IInterestPointProvider.h"

namespace Eegeo
{
    namespace OVR
    {
        namespace ZoomDirection
        {
            enum Values
            {
                In,
                Out
            };
        }
        
        namespace RotateDirection
        {
            enum Values
            {
                Left,
                Right
            };
        }
        
        class OVREegeoCameraController : public Eegeo::Location::IInterestPointProvider, protected Eegeo::NonCopyable
        {
        public:
            OVREegeoCameraController(float screenWidth, float screenHeight)
            : m_zooming(false)
            , m_rotating(false)
            , m_screenHeight(screenHeight)
            , m_screenWidth(screenWidth)
            , m_zoomDirection(0.f)
            , m_rotateDirection(0.f)
            , m_ecefPosition(0.0, 0.0, 0.0)
            , m_fovDegrees(40.0)
            {
                m_orientation.Identity();
            }
            
            ~OVREegeoCameraController() { };
            
            Eegeo::dv3 GetEcefInterestPoint() const { return m_renderCamera.GetEcefLocation(); }

            Eegeo::Camera::RenderCamera& GetCamera() { return m_renderCamera; }
            const bool IsRotating() const { return m_rotating; }
            const bool IsZooming() const { return m_zooming; }
            void SetFovDegrees(float fovDegrees);
            
            void UpdateFromPose(const Eegeo::m33& orientation, const Eegeo::v3& eyeOffset);
            void SetEcefPosition(const Eegeo::dv3& ecef);
            void SetStartLatLongAltitude(const Eegeo::Space::LatLongAltitude& eyePos);

            void Update(float dt);
            
            void MouseDown(float x, float y);
            void MouseUp(float x, float y);
            void MouseDrag(float x, float y);
            void ZoomStart(ZoomDirection::Values direction);
            void ZoomEnd();
            void RotateStart(RotateDirection::Values direction);
            void RotateEnd();
        private:
            void Zoom(float dt);
            void Rotate(float dt);
            void UpdateFovAndClippingPlanes();

            bool m_zooming;
            bool m_rotating;
            float m_fovDegrees;
            float m_rotation;
            float m_screenWidth;
            float m_screenHeight;
            float m_zoomDirection;
            float m_rotateDirection;
            
            Eegeo::Camera::RenderCamera m_renderCamera;
            
            dv3 m_interestEcef;
            dv3 m_ecefPosition;
            m33 m_orientation;
        };
    }
}
