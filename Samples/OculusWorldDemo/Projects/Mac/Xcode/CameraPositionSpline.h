// Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#pragma once

#include "Types.h"
#include "CatmullRomSpline.h"
#include "VectorMathDecl.h"

namespace Eegeo
{
    namespace OVR
    {
        class CameraPositionSpline : protected Eegeo::NonCopyable
        {
        public:
            CameraPositionSpline()
            : m_playbackSpeed(0.03)
            , m_playing(false)
            , m_time(0.0)
            {
                m33 orientation;
                orientation.SetFromBasis(Eegeo::v3(-0.003297,-0.301644,0.953413),Eegeo::v3(0.728825,0.652078,0.208825),Eegeo::v3(0.684698,-0.695559,-0.217691));
                AddPoint(Eegeo::dv3(4645941.497602,4162308.552896,1332740.424752), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.003297,-0.301644,0.953413),Eegeo::v3(0.728825,0.652078,0.208825),Eegeo::v3(0.684698,-0.695559,-0.217691));
                AddPoint(Eegeo::dv3(4645462.385839,4162795.264665,1332892.752648), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.003297,-0.301644,0.953413),Eegeo::v3(0.728825,0.652078,0.208825),Eegeo::v3(0.684698,-0.695559,-0.217691));
                AddPoint(Eegeo::dv3(4644830.434319,4163437.240650,1333093.674132), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.003297,-0.301644,0.953413),Eegeo::v3(0.728825,0.652078,0.208825),Eegeo::v3(0.684698,-0.695559,-0.217691));
                AddPoint(Eegeo::dv3(4644237.010047,4164040.078239,1333282.346334), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.003297,-0.301644,0.953413),Eegeo::v3(0.728825,0.652078,0.208825),Eegeo::v3(0.684698,-0.695559,-0.217691));
                AddPoint(Eegeo::dv3(4643674.446370,4164611.565698,1333461.206776), orientation);
                orientation.SetFromBasis(Eegeo::v3(0.622116,-0.501066,-0.601582),Eegeo::v3(0.727658,0.653615,0.208093),Eegeo::v3(-0.288928,0.567207,-0.771233));
                AddPoint(Eegeo::dv3(4643271.400814,4164790.315370,1333528.008281), orientation);
                orientation.SetFromBasis(Eegeo::v3(0.068765,0.233774,-0.969854),Eegeo::v3(0.727139,0.653852,0.209162),Eegeo::v3(-0.683033,0.719609,0.125021));
                AddPoint(Eegeo::dv3(4643039.435598,4164606.450429,1333690.827047), orientation);
            }
            
            void AddPoint(const dv3& ecefEyePosition, const m33& orientation);
            void Update(float dt);
            
            void Start();
            void Stop();
            
            double GetPlaybackSpeed() const { return m_playbackSpeed; }
            void SetPlaybackSpeed(double speed) { m_playbackSpeed = speed; }
            
            const bool IsPlaying() const { return m_playing; }
            
            void Spew();
            void Clear();
            
            void GetCurrentCameraPosition(dv3& interpolatedPositionEcef, m33& interpolatedOrientation) const;
        private:
            Geometry::CatmullRomSpline m_positionSpline;
            Geometry::CatmullRomSpline m_forwardSpline;
            Geometry::CatmullRomSpline m_upSpline;
            Geometry::CatmullRomSpline m_rightSpline;
            double m_playbackSpeed;
            double m_time;
            bool m_playing;
        };
    }
}