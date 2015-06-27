#pragma once

#include "Kernel/OVR_Types.h"
#include "Kernel/OVR_Allocator.h"
#include "Kernel/OVR_RefCount.h"
#include "Kernel/OVR_Log.h"
#include "Kernel/OVR_System.h"
#include "Kernel/OVR_Nullptr.h"
#include "Kernel/OVR_String.h"
#include "Kernel/OVR_Array.h"
#include "Kernel/OVR_Timer.h"
#include "Kernel/OVR_DebugHelp.h"
#include "Extras/OVR_Math.h"

#include "../CommonSrc/Platform/Platform_Default.h"
#include "../CommonSrc/Render/Render_Device.h"
#include "../CommonSrc/Render/Render_XmlSceneLoader.h"
#include "../CommonSrc/Platform/Gamepad.h"
#include "../CommonSrc/Util/OptionMenu.h"
#include "../CommonSrc/Util/RenderProfiler.h"


#include "Player.h"

namespace Eegeo
{
    class Platform;
    
    namespace OVR
    {
        class OVREegeoCameraController;
    }
}

using namespace OVR;
using namespace OVR::OvrPlatform;
using namespace OVR::Render;

class EegeoWorldDemoApp : public Application
{
public:
    EegeoWorldDemoApp();
    ~EegeoWorldDemoApp();
    
    virtual int  OnStartup(int argc, const char** argv);
    virtual void OnIdle();
    
    virtual void OnMouseMove(int x, int y, int modifiers);
    virtual void OnKey(OVR::KeyCode key, int chr, bool down, int modifiers);
    virtual void OnResize(int width, int height);
    
    bool         SetupWindowAndRendering(int argc, const char** argv);
    
    void         PopulateOptionMenu();

    void         CalculateHmdValues();
    Sizei        EnsureRendertargetAtLeastThisBig (int rtNum, Sizei size);
    
    void         RenderEyeView(ovrEyeType eye);
    void         RenderTextInfoHud(float textHeight);
    void         RenderAnimatedBlocks(ovrEyeType eye, double appTime);
    void         RenderGrid(ovrEyeType eye);
    
    Matrix4f     CalculateViewFromPose(const Posef& pose);
    
    bool        FrameNeedsRendering(double curtime);
    void        ApplyDynamicResolutionScaling();
    void        UpdateFrameRateCounter(double curtime);
    
    void        ChangeDisplay ( bool bBackToWindowed, bool bNextFullscreen, bool bFullWindowDebugging );
    void        GamepadStateChanged(const GamepadState& pad);
    
    // Processes DeviceNotificationStatus queue to handles plug/unplug.
    void         ProcessDeviceNotificationQueue();
    
    void HmdSettingChange(OptionVar* = 0)   { HmdSettingsChanged = true; }
    void MirrorSettingChange(OptionVar* = 0)
    { HmdSettingsChanged = true; NotificationTimeout = ovr_GetTimeInSeconds() + 10.0f;}
    
    void EyeHeightChange(OptionVar* = 0)
    {
        ThePlayer.HeightScale = ScaleAffectsEyeHeight ? PositionTrackingScale : 1.0f;
        ThePlayer.BodyPos.y = ThePlayer.GetScaledEyeHeight();
    }
    
    void HmdSensorToggle(OptionVar* = 0);
    void HmdSettingChangeFreeRTs(OptionVar* = 0);
    void MultisampleChange(OptionVar* = 0);
    void CenterPupilDepthChange(OptionVar* = 0);
    void DistortionClearColorChange(OptionVar* = 0);
    
    void ResetHmdPose(OptionVar* = 0);
    
protected:
    Eegeo::Platform* pEegeoPlaform;
    Eegeo::OVR::OVREegeoCameraController* pCameraController;
    
    ExceptionHandler     OVR_ExceptionHandler;
    GUIExceptionListener OVR_GUIExceptionListener;
    
    RenderDevice*       pRender;
    RendererParams      RenderParams;
    Sizei               WindowSize;
    int                 ScreenNumber;
    int                 FirstScreenInCycle;
    bool                SupportsSrgb;
    bool                SupportsMultisampling;
    
    struct RenderTarget
    {
        Ptr<Texture>     pColorTex;
        Ptr<Texture>     pDepthTex;
        ovrTexture       OvrColorTex;
        ovrTexture       OvrDepthTex;
    };
    enum RendertargetsEnum
    {
        Rendertarget_Left,
        Rendertarget_Right,
        Rendertarget_BothEyes,    // Used when both eyes are rendered to the same target.
        Rendertarget_LAST
    };
    RenderTarget        RenderTargets[Rendertarget_LAST];
    RenderTarget        MsaaRenderTargets[Rendertarget_LAST];
    RenderTarget*       DrawEyeTargets; // the buffers we'll actually render to (could be MSAA)
    
    ovrHmd              Hmd;
    ovrEyeRenderDesc    EyeRenderDesc[2];
    Matrix4f            Projection[2];          // Projection matrix for eye.
    Matrix4f            OrthoProjection[2];     // Projection for 2D.
    ovrPosef            EyeRenderPose[2];       // Poses we used for rendering.
    ovrTexture          EyeTexture[2];
    ovrTexture          EyeDepthTexture[2];
    Sizei               EyeRenderSize[2];       // Saved render eye sizes; base for dynamic sizing.

    unsigned            StartTrackingCaps;
    bool                UsingDebugHmd;
    
    float               SecondsOfFpsMeasurement;
    int                 FrameCounter;
    int					TotalFrameCounter;
    float               SecondsPerFrame;
    float               FPS;
    double              LastFpsUpdate;

    double              LastUpdate;
    
    bool                HaveVisionTracking;
    bool                HavePositionTracker;
    bool                HaveHMDConnected;
    bool                HaveSync;
    
    double              LastSyncTime;
    unsigned int        LastCameraFrame;
    
    GamepadState        LastGamepadState;
    
    Player				ThePlayer;
    Matrix4f            ViewFromWorld[2];   // One per eye.
    
    ovrFrameTiming      HmdFrameTiming;
    unsigned            HmdStatus;
    
    double              NotificationTimeout;
    
    bool                HmdSettingsChanged;
    
    bool                RendertargetIsSharedByBothEyes;
    bool                DynamicRezScalingEnabled;
    bool                EnableSensor;
    
    enum MonoscopicMode
    {
        Mono_Off,               // Disabled.
        Mono_ZeroIpd,           // Set the player's IPD to zero (but still hve head-tracking - WARNING - UNPLEASANT FOR SOME)
        Mono_ZeroPlayerScale,   // Set the player's scale to zero (removes head-tracking)
        
        Mono_Count
    };
    MonoscopicMode      MonoscopicRenderMode;
    float               PositionTrackingScale;
    bool                ScaleAffectsEyeHeight;
    float               DesiredPixelDensity;
    float               FovSideTanMax;
    float               FovSideTanLimit; // Limit value for Fov.
    bool                FadedBorder;
    
    float               NearClip;
    float               FarClip;
    enum DepthMod
    {
        NearLessThanFar,
        FarLessThanNear,
        FarLessThanNearAndInfiniteFarClip,
    };
    DepthMod            DepthModifier;
    
    enum SceneRenderCountEnum
    {
        SceneRenderCount_FixedLow,
        SceneRenderCount_SineTenSec,
        SceneRenderCount_SquareTenSec,
        SceneRenderCount_Spikes,
        
        SceneRenderCount_LAST
    } SceneRenderCountType;
    int32_t             SceneRenderCountLow;
    int32_t             SceneRenderCountHigh;
    
    // Time-warp.
    enum TimewarpMode
    {
        Timewarp_Off,
        Timewarp_Orientation,
    };
    TimewarpMode        TimewarpOption;
    bool                TimewarpJitDelayEnabled;
    float               TimewarpRenderIntervalInSeconds;
    bool                FreezeEyeUpdate;
    bool                FreezeEyeOneFrameRendered;
    bool                ComputeShaderEnabled;
    
    // Other global settings.
    float               CenterPupilDepthMeters;
    bool                ForceZeroHeadMovement;
    bool                VsyncEnabled;
    bool                MultisampleEnabled;

    // DK2 only:
    bool                IsLowPersistence;
    bool                DynamicPrediction;
    bool                DisplaySleep;
    bool                PositionTrackingEnabled;
    bool				PixelLuminanceOverdrive;
    bool                HqAaDistortion;
    bool                MirrorToWindow;
    
    // Support toggling background color for distortion so that we can see
    // the effect on the periphery.
    int                 DistortionClearBlue;
    
    // Stereo settings adjustment state.
    bool                ShiftDown;
    bool                CtrlDown;
    
    // Logging
    bool                IsLogging;
    
    // What help screen we display, brought up by 'Spacebar'.
    enum TextScreen
    {
        Text_None,
        Text_Info,
        Text_Timing,
        Text_Help1,
        Text_Help2,
        Text_Count
    };
    TextScreen          TextScreen;
    
    // User configurable options, brought up by 'Tab' key.
    // Also handles shortcuts and pop-up overlay messages.
    OptionSelectionMenu Menu;
    bool                ShortcutChangeMessageEnable;
    
    // Profiler for rendering - displays timing stats.
    RenderProfiler      Profiler;
    
    // true if logging tracking data to file
    bool IsVisionLogging;
};
