//  Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#include "OVREegeoCameraController.h"
#include "LatLongAltitude.h"
#include "CameraHelpers.h"
#include "SpaceHelpers.h"
#include "EcefTangentBasis.h"
#include "Quaternion.h"
#include "EarthConstants.h"

namespace Eegeo
{
    namespace OVR
    {
        Eegeo::dv3 OVREegeoCameraController::GetEcefInterestPoint() const
        {
            dv3 ecefPosition = m_ecefPosition;
            return ecefPosition.Normalise() * Eegeo::Space::EarthConstants::Radius;
        }
        
        double OVREegeoCameraController::GetAltitudeAboveSeaLevel() const
        {
            return Space::SpaceHelpers::GetAltitude(m_renderCamera.GetEcefLocation());
        }
        
        void OVREegeoCameraController::UpdateFromPose(const Eegeo::m33& orientation, const Eegeo::v3& eyeOffset)
        {
            m33 orientationMatrix;
            m33::Mul(orientationMatrix, m_orientation, orientation);
            
            v3 eyeOffsetModified = eyeOffset;
            v3 rotatedEyeOffset = v3::Mul(eyeOffsetModified, m_orientation);
            
            m_renderCamera.SetOrientationMatrix(orientationMatrix);
            m_renderCamera.SetEcefLocation(dv3(m_ecefPosition.x + rotatedEyeOffset.x, m_ecefPosition.y + rotatedEyeOffset.y, m_ecefPosition.z + rotatedEyeOffset.z));
        }
        
        void OVREegeoCameraController::SetEcefPosition(const Eegeo::dv3& ecef)
        {
            m_ecefPosition = ecef;
            m_renderCamera.SetEcefLocation(m_ecefPosition);
            UpdateFovAndClippingPlanes();
        }
        
        void OVREegeoCameraController::SetStartLatLongAltitude(const Eegeo::Space::LatLongAltitude& eyePos)
        {
            m_ecefPosition = eyePos.ToECEF();
            
            Space::EcefTangentBasis tangentBasis;
            Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(m_ecefPosition, 0.f, tangentBasis);
            
            m_orientation.SetRow(0, tangentBasis.GetRight());
            m_orientation.SetRow(1, tangentBasis.GetUp());
            m_orientation.SetRow(2, -tangentBasis.GetForward());
            
            m_renderCamera.SetOrientationMatrix(m_orientation);
            m_renderCamera.SetEcefLocation(m_ecefPosition);
        }
        
        void OVREegeoCameraController::Update(float dt)
        {
            Rotate(dt);
            Move(dt);
            UpdateFovAndClippingPlanes();
        }
        
        void OVREegeoCameraController::MoveStart(MoveDirection::Values direction)
        {
            if (!m_moving)
            {
                switch (direction)
                {
                    case MoveDirection::Forward:
                    {
                        const v3 forwardDirection = -GetForward();
                        m_moveDirection = forwardDirection;
                        break;
                    }
                    case MoveDirection::Backwards:
                    {
                        const v3 backwardsDirection = GetForward();
                        m_moveDirection = backwardsDirection;
                        break;
                    }
                    case MoveDirection::Left:
                    {
                        const v3 leftDirection = -GetRight();
                        m_moveDirection = leftDirection;
                        break;
                    }
                    case MoveDirection::Right:
                    {
                        const v3 rightDirection = GetRight();
                        m_moveDirection = rightDirection;
                        break;
                    }
                    case MoveDirection::Up:
                    {
                        const v3 upDirection = GetUp();
                        m_moveDirection = upDirection;
                        break;
                    }
                    case MoveDirection::Down:
                    {
                        const v3 downDirection = -GetUp();
                        m_moveDirection = downDirection;
                        break;
                    }
                }
                m_moving = true;
            }
        }
        
        void OVREegeoCameraController::MoveEnd()
        {
            if(m_moving)
            {
                m_moving = false;
            }
        }
        
        void OVREegeoCameraController::Move(float dt)
        {
            if (m_moving)
            {
                const float mutliplier = MovementAltitudeMutlipler();
                const float& mutliplierRef = mutliplier;
                m_ecefPosition += m_moveDirection * mutliplierRef * dt;
                m_renderCamera.SetEcefLocation(m_ecefPosition);
            }
        }
        
        void OVREegeoCameraController::RotateStart(RotateDirection::Values direction)
        {
            if (!m_rotating)
            {
                m_rotateDirection = direction == RotateDirection::Left ? 1.f : -1.f;
                m_rotating = true;
            }
        }
        
        void OVREegeoCameraController::RotateEnd()
        {
            if (m_rotating)
            {
                m_rotating = false;
            }
        }

        void OVREegeoCameraController::Rotate(float dt)
        {
            
        }
        
        void OVREegeoCameraController::SetFovDegrees(float fovDegrees)
        {
            m_fovDegrees = fovDegrees;
            UpdateFovAndClippingPlanes();
        }
        
        float OVREegeoCameraController::MovementAltitudeMutlipler() const
        {
            const double minAltitude = 300.0;
            const double maxAltitude = 8000.0;
            
            const double minMultiplier = 100.f;
            const double maxMultiplier = 10000.f;
            
            double clampedAltitude = Math::Clamp(GetAltitudeAboveSeaLevel(), minAltitude, maxAltitude);
            double altitudeT = (clampedAltitude - minAltitude) / (maxAltitude - minAltitude);
            
            return minMultiplier + ((maxMultiplier - minMultiplier) * altitudeT);
        }
        
        void OVREegeoCameraController::UpdateFovAndClippingPlanes()
        {
            double cameraAltitude = GetAltitudeAboveSeaLevel();
            double approxTerrainAltitude = 100;
            double approxTerrainAltitudeDelta = approxTerrainAltitude - 100;
            
            float nearZ;
            float farZ;
            const double ClipPlaneThresholdAltitude = 15000.0;
            Camera::CameraHelpers::GetAltitudeInterpolatedNearFar(cameraAltitude, approxTerrainAltitude, approxTerrainAltitudeDelta, ClipPlaneThresholdAltitude, nearZ, farZ);
            
            m_renderCamera.SetProjection(Math::Deg2Rad(m_fovDegrees), nearZ, farZ);
        }
    }
}
