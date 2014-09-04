//  Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#include "CameraPositionSpline.h"

namespace Eegeo
{
    namespace OVR
    {
        void CameraPositionSpline::AddPoint(const dv3& ecefEyePoint, const m33& orientation)
        {
            m_positionSpline.AddPoint(ecefEyePoint);
            
            v3 right = orientation.GetRow(0);
            v3 up = orientation.GetRow(1);
            v3 forward = orientation.GetRow(2);

            m_rightSpline.AddPoint(dv3(right.x, right.y, right.z));
            m_upSpline.AddPoint(dv3(up.x, up.y, up.z));
            m_forwardSpline.AddPoint(dv3(forward.x, forward.y, forward.z));
        }
        
        void CameraPositionSpline::Start()
        {
            m_time = 0.0;
            m_playing = true;
        }
        
        void CameraPositionSpline::Stop()
        {
            m_playing = false;
        }
        
        void CameraPositionSpline::Update(float dt)
        {
            if(m_playing)
            {
                m_time += dt * m_playbackSpeed;
                if(m_time >= 1.0)
                {
                    Stop();
                }
                
                m_time = Math::Clamp01(m_time);
            }
        }
        
        void CameraPositionSpline::GetCurrentCameraPosition(dv3& interpolatedPositionEcef, m33& interpolatedOrientation) const
        {
            m_positionSpline.GetInterpolatedPositionInPlace(m_time, interpolatedPositionEcef);
            dv3 forward;
            dv3 up;
            dv3 right;
            m_forwardSpline.GetInterpolatedPositionInPlace(m_time, forward);
            m_upSpline.GetInterpolatedPositionInPlace(m_time, up);
            m_rightSpline.GetInterpolatedPositionInPlace(m_time, right);
            interpolatedOrientation.SetRow(0, right.ToSingle());
            interpolatedOrientation.SetRow(1, up.ToSingle());
            interpolatedOrientation.SetRow(2, forward.ToSingle());
        }
    }
}