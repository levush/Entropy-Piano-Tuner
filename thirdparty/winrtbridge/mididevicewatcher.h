#ifndef MIDIDEVICEWATCHER_H
#define MIDIDEVICEWATCHER_H

#include <collection.h>
#include <ppltasks.h>

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Devices::Enumeration;

using namespace Platform::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

HRESULT __stdcall GetActivationFactoryByPCWSTR(void*,Guid&, void**);
namespace __winRT
{
    HRESULT __stdcall __getActivationFactoryByPCWSTR(const void* str, ::Platform::Guid& pGuid, void** ppActivationFactory);
}

ref class MidiDeviceWatcher sealed
{
public:
    MidiDeviceWatcher(String ^ midiSelector, Windows::UI::Core::CoreDispatcher ^ dispatcher, IVector<String ^ > ^ portList);
    virtual ~MidiDeviceWatcher();
    void Start();
    void Stop();
    DeviceInformationCollection^ GetDeviceInformationCollection();
    void UpdatePorts();

private:

    void OnPortAdded(DeviceWatcher^ deviceWatcher, DeviceInformation^ devInfo);
    void OnPortRemoved(DeviceWatcher^ deviceWatcher, DeviceInformationUpdate^ devInfoUpdate);
    void OnPortUpdated(DeviceWatcher^ deviceWatcher, DeviceInformationUpdate^ devInfoUpdate);
    void OnPortEnumCompleted(DeviceWatcher^ deviceWatcher, Object^ obj);

    Windows::Foundation::EventRegistrationToken _portAddedToken;
    Windows::Foundation::EventRegistrationToken _portRemovedToken;
    Windows::Foundation::EventRegistrationToken _portUpdatedToken;
    Windows::Foundation::EventRegistrationToken _portEnumCompleteToken;

    String ^ _midiSelectorString;
    Windows::UI::Core::CoreDispatcher^ _coreDispatcher;
    IVector<String ^> ^ _portList;
    DeviceWatcher ^ _deviceWatcher;
    DeviceInformationCollection^ _devInfoCollection;
    bool _enumCompleted;
};

#endif // MIDIDEVICEWATCHER_H
