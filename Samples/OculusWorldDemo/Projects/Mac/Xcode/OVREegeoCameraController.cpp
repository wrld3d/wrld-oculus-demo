//  Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#include "OVREegeoCameraController.h"
#include "LatLongAltitude.h"
#include "CameraHelpers.h"
#include "SpaceHelpers.h"
#include "EcefTangentBasis.h"

namespace Eegeo
{
    namespace OVR
    {
        void OVREegeoCameraController::UpdateFromPose(const Eegeo::m33& orientation, const Eegeo::v3 eyeOffset)
        {
            m33 newOrientation;
            m33::Mul(newOrientation, m_orientation, orientation);
            
            v3 eyeOffsetModified = eyeOffset;
            eyeOffsetModified.z *= -1.0; // inverted forward!?
            v3 rotatedEyeOffset = v3::Mul(eyeOffsetModified, newOrientation);
            
            m_renderCamera.SetOrientationMatrix(newOrientation);
            m_renderCamera.SetEcefLocation(dv3(m_ecefPosition.x + rotatedEyeOffset.x, m_ecefPosition.y + rotatedEyeOffset.y, m_ecefPosition.z + rotatedEyeOffset.z));
        }
        
        void OVREegeoCameraController::SetEcefPosition(const Eegeo::dv3& ecef)
        {
            m_ecefPosition = ecef;
            m_renderCamera.SetEcefLocation(m_ecefPosition);
            UpdateFovAndClippingPlanes();
        }
        
        void OVREegeoCameraController::SetStartLatLongAltitude(const Eegeo::Space::LatLongAltitude& eyePos, const Eegeo::Space::LatLongAltitude lookAt)
        {
            Space::LatLongAltitude interest = lookAt;
            m_interestEcef = interest.ToECEF();
            
            Space::EcefTangentBasis cameraInterestBasis;
            Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(interest.ToECEF(), 0, cameraInterestBasis);
            
            dv3 cameraLocationEcef;
            v3 cameraDirection;
            v3 cameraUp;
            Camera::CameraHelpers::CalculateLookAt(interest.ToECEF(), cameraInterestBasis.GetForward(), Math::Deg2Rad(70.0f), 100, cameraLocationEcef, cameraDirection, cameraUp);
            
            Camera::CameraHelpers::CalculateCameraOrientation(cameraDirection, cameraUp, m_orientation);
            
            m_renderCamera.SetOrientationMatrix(m_orientation);
            m_renderCamera.SetEcefLocation(cameraLocationEcef);
            m_ecefPosition = cameraLocationEcef;
            
/*            dv3 eyePosEcef = eyePos.ToECEF();
            dv3 lookAtEcef = lookAt.ToECEF();
            
            v3 up = eyePosEcef.Norm().ToSingle();
            v3 forward = (lookAtEcef - eyePosEcef).Norm().ToSingle();
            
            m33 orientation;
            Camera::CameraHelpers::CalculateCameraOrientation(forward, up, orientation);
            m_renderCamera.SetOrientationMatrix(orientation);
            SetEcefPosition(eyePosEcef); */
        }
        
        void OVREegeoCameraController::Update(float dt)
        {
            Zoom(dt);
            Rotate(dt);
            UpdateFovAndClippingPlanes();
        }
        
        void OVREegeoCameraController::MouseDown(float x, float y)
        {
            
        }
        
        void OVREegeoCameraController::MouseUp(float x, float y)
        {
            
        }
        
        void OVREegeoCameraController::MouseDrag(float x, float y)
        {
            
        }
        
        void OVREegeoCameraController::ZoomStart(ZoomDirection::Values direction)
        {
            if (!m_zooming)
            {
                m_zoomDirection = direction == ZoomDirection::In ? 1.f : -1.f;
                m_zooming = true;
            }
        }
        
        void OVREegeoCameraController::ZoomEnd()
        {
            if (m_zooming)
            {
                m_zooming = false;
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

        void OVREegeoCameraController::Zoom(float dt)
        {
            
        }
        
        void OVREegeoCameraController::Rotate(float dt)
        {
            
        }
        
        void OVREegeoCameraController::UpdateFovAndClippingPlanes()
        {
            double cameraAltitude = Space::SpaceHelpers::GetAltitude(m_renderCamera.GetEcefLocation());
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