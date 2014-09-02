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
       
        namespace MoveDirection
        {
            enum Values
            {
                Forward,
                Backwards,
                Left,
                Right,
                Up,
                Down
            };
        }
        
        class OVREegeoCameraController : public Eegeo::Location::IInterestPointProvider, protected Eegeo::NonCopyable
        {
        public:
            OVREegeoCameraController(float screenWidth, float screenHeight)
            : m_moving(false)
            , m_screenHeight(screenHeight)
            , m_screenWidth(screenWidth)
            , m_moveDirection(0.f, 0.f, 0.f)
            , m_ecefPosition(0.0, 0.0, 0.0)
            {
                m_orientation.Identity();
            }
            
            ~OVREegeoCameraController() { };
            
            Eegeo::dv3 GetEcefInterestPoint() const;
            double GetAltitudeAboveSeaLevel() const;

            Eegeo::Camera::RenderCamera& GetCamera() { return m_renderCamera; }
            const bool IsMoving() const { return m_moving; }

            const v3 GetRight() const { return m_orientation.GetRow(0); }
            const v3 GetUp() const { return m_orientation.GetRow(1); }
            const v3 GetForward() const { return m_orientation.GetRow(2); }
            
            void UpdateFromPose(const Eegeo::m33& orientation, const Eegeo::v3& eyeOffset);
            void SetEcefPosition(const Eegeo::dv3& ecef);
            void SetStartLatLongAltitude(const Eegeo::Space::LatLongAltitude& eyePos);
            
            void GetNearFarPlaneDistances(float& out_near, float& out_far);
            void Update(float dt);
            
            void MoveStart(MoveDirection::Values direction);
            void MoveEnd();
            
            void RotateHorizontal(float angle);
        private:
            float MovementAltitudeMutlipler() const;
            
            void Move(float dt);
            void UpdateFovAndClippingPlanes();

            bool m_moving;
            v3 m_moveDirection;
            
            float m_screenWidth;
            float m_screenHeight;
            
            Eegeo::Camera::RenderCamera m_renderCamera;
            
            dv3 m_interestEcef;
            dv3 m_ecefPosition;
            m33 m_orientation;
        };
    }
}
