#ifndef WINRTNATIVEHELPER_H
#define WINRTNATIVEHELPER_H

#include <windows.h>

HRESULT __stdcall GetActivationFactoryByPCWSTR(void*, ::Platform::Guid&, void**);
namespace __winRT
{
    HRESULT __stdcall __getActivationFactoryByPCWSTR(const void* str, ::Platform::Guid& pGuid, void** ppActivationFactory);
}

#endif // WINRTNATIVEHELPER_H
