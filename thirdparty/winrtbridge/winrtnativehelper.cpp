#include "winrtnativehelper.h"

HRESULT __stdcall GetActivationFactoryByPCWSTR(void*,::Platform::Guid&, void**);
namespace __winRT
{
    HRESULT __stdcall __getActivationFactoryByPCWSTR(const void* str, ::Platform::Guid& pGuid, void** ppActivationFactory)
    {
        return GetActivationFactoryByPCWSTR(const_cast<void*>(str), pGuid, ppActivationFactory);
    }
}
