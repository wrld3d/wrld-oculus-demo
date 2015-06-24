#pragma once

#import <Cocoa/Cocoa.h>
#include <string>
#include "PlatformConfig.h"

namespace Eegeo
{
    namespace OSX
    {
        class OSXPlatformAbstractionModule;
        class OSXAlertBoxFactory;
        class OSXInputBoxFactory;
        class OSXKeyboardInputFactory;
        class OSXLocationService;
    }
    
    namespace Helpers
    {
        namespace Jpeg
        {
            class JpegLoader;
            class IJpegLoader;
        }
    }
    
    namespace UI
    {
        class NativeUIFactories;
        
        namespace NativeAlerts
        {
            class IAlertBoxFactory;
        }
        
        namespace NativeInput
        {
            class IInputBoxFactory;
            class IKeyboardInputFactory;
        }
    }
    
    namespace Modules
    {
        class IPlatformAbstractionModule;
    }
    
    namespace Location
    {
        class ILocationService;
    }
    
    namespace Config
    {
        class PlatformConfig;
    }
    
    namespace OVR
    {
        class EegeoOSX
        {
        public:
            EegeoOSX(NSOpenGLPixelFormat* pPixelFormat,
                     const std::string& apiKey);
            ~EegeoOSX();
            
            UI::NativeUIFactories& GetNativeUIFactories();
            Modules::IPlatformAbstractionModule& GetPlatformAbstraction();
            Helpers::Jpeg::IJpegLoader& GetJpegLoader();
            Location::ILocationService& GetLocationService();
            
            Eegeo::Config::PlatformConfig GetPlatformConfig();
        private:
            UI::NativeUIFactories* m_pNativeUIFactories;
            OSX::OSXPlatformAbstractionModule* m_pOSXPlatformAbstractionModule;
            OSX::OSXAlertBoxFactory* m_pOSXAlertBoxFactory;
            OSX::OSXInputBoxFactory* m_pOSXInputBoxFactory;
            OSX::OSXKeyboardInputFactory* m_pOSXKeyboardInputFactory;
            OSX::OSXLocationService* m_pOSXLocationService;
            Helpers::Jpeg::JpegLoader* m_pJpegLoader;
        };
    }
}