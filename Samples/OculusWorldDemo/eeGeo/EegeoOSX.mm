#include "EegeoOSX.h"
#include "OSXPlatformAbstractionModule.h"
#include "OSXWebRequestService.h"
#include "OSXGlTaskContextFactory.h"
#include "OSXPlatformConfigBuilder.h"
#include "OSXLocationService.h"
#include "OSXStubUI.h"
#include "JpegLoader.h"
#include "IAlertBoxFactory.h"
#include "IInputBoxFactory.h"
#include "IKeyboardInputFactory.h"
#include "NativeUIFactories.h"

namespace Eegeo
{
    namespace OVR
    {
        EegeoOSX::EegeoOSX(NSOpenGLPixelFormat* pPixelFormat,
                           const std::string& apiKey)
        {
            m_pJpegLoader = new Eegeo::Helpers::Jpeg::JpegLoader();
            
            m_pOSXPlatformAbstractionModule = new Eegeo::OSX::OSXPlatformAbstractionModule(*m_pJpegLoader,
                                                                                           pPixelFormat,
                                                                                           "OculusWorldDemo.app/Contents/Resources/Assets/eeGeo/",
                                                                                           apiKey);

            m_pOSXAlertBoxFactory = new OSX::OSXAlertBoxFactory();
            m_pOSXInputBoxFactory = new OSX::OSXInputBoxFactory();
            m_pOSXKeyboardInputFactory = new OSX::OSXKeyboardInputFactory();
            
            m_pNativeUIFactories = new UI::NativeUIFactories(*m_pOSXAlertBoxFactory, *m_pOSXInputBoxFactory, *m_pOSXKeyboardInputFactory);
            m_pOSXLocationService = new OSX::OSXLocationService();
        }
        
        void EegeoOSX::SetWebRequestServiceWorkPool(Concurrency::Tasks::IWorkPool& workPool) { m_pOSXPlatformAbstractionModule->SetWebRequestServiceWorkPool(workPool); }
        
        UI::NativeUIFactories& EegeoOSX::GetNativeUIFactories() { return *m_pNativeUIFactories; }
        
        Modules::IPlatformAbstractionModule& EegeoOSX::GetPlatformAbstraction() { return *m_pOSXPlatformAbstractionModule; }
        Helpers::Jpeg::IJpegLoader& EegeoOSX::GetJpegLoader() { return *m_pJpegLoader; }
        Location::ILocationService& EegeoOSX::GetLocationService() { return *m_pOSXLocationService; }
        
        void SetWorkPool();
        
        Eegeo::Config::PlatformConfig EegeoOSX::GetPlatformConfig() { return OSX::OSXPlatformConfigBuilder().Build(); }
    }
}