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
                orientation.SetFromBasis(Eegeo::v3(-0.181377,-0.110160,0.977223),Eegeo::v3(0.728775,0.652148,0.208780),Eegeo::v3(0.660294,-0.750046,0.038002));
                AddPoint(Eegeo::dv3(4648172.292439,4159580.242028,1331625.804022), orientation);
                orientation.SetFromBasis(Eegeo::v3(0.061158,-0.365653,0.928737),Eegeo::v3(0.728776,0.652152,0.208768),Eegeo::v3(0.682016,-0.664078,-0.306365));
                AddPoint(Eegeo::dv3(4647918.212886,4159788.902805,1331710.577593), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.221746,-0.063747,0.973017),Eegeo::v3(0.728776,0.652142,0.208809),Eegeo::v3(0.647856,-0.755416,0.098153));
                AddPoint(Eegeo::dv3(4647929.349371,4160099.512685,1331821.951101), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.004384,-0.300435,0.953790),Eegeo::v3(0.728779,0.652151,0.208770),Eegeo::v3(0.684737,-0.696021,-0.216091));
                AddPoint(Eegeo::dv3(4647868.438146,4160170.536716,1331812.722798), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.643567,0.756417,-0.116844),Eegeo::v3(0.728666,0.652228,0.208925),Eegeo::v3(-0.234246,-0.049319,0.970928));
                AddPoint(Eegeo::dv3(4647672.783374,4160141.472382,1331674.231118), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.416373,0.664006,-0.621070),Eegeo::v3(0.728615,0.652290,0.208909),Eegeo::v3(-0.543837,0.365536,0.755400));
                AddPoint(Eegeo::dv3(4647572.540046,4160282.525131,1331583.460096), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.170106,0.467721,-0.867352),Eegeo::v3(0.728587,0.652333,0.208879),Eegeo::v3(-0.663500,0.596408,0.451739));
                AddPoint(Eegeo::dv3(4647853.716026,4160544.688893,1331627.942732), orientation);
                orientation.SetFromBasis(Eegeo::v3(0.552680,-0.379914,-0.741760),Eegeo::v3(0.728579,0.652396,0.208715),Eegeo::v3(-0.404627,0.655780,-0.637363));
                AddPoint(Eegeo::dv3(4647935.263351,4160641.274088,1331610.706218), orientation);
                orientation.SetFromBasis(Eegeo::v3(-0.077166,-0.224347,0.971445),Eegeo::v3(0.728930,0.652075,0.208491),Eegeo::v3(0.680228,-0.724208,-0.113214));
                AddPoint(Eegeo::dv3(4648349.368964,4159360.308238,1331560.434232), orientation);
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