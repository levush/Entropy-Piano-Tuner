#include "mididevicewatcher.h"
#include "winrtnativehelper.h"

using namespace Platform::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

using namespace Windows::Devices::Midi;

MidiDeviceWatcher::MidiDeviceWatcher(String ^ midiSelector, CoreDispatcher ^ dispatcher,
    IVector<String ^> ^ portList)
 :  _midiSelectorString(midiSelector),
    _coreDispatcher(dispatcher),
    _portList(portList),
    _deviceWatcher(nullptr),
    _devInfoCollection(nullptr),
    _enumCompleted(false)
{
    _deviceWatcher = Windows::Devices::Enumeration::DeviceInformation::CreateWatcher(midiSelector);

    _portAddedToken = _deviceWatcher->Added += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformation ^>(this, &MidiDeviceWatcher::OnPortAdded);
    _portRemovedToken = _deviceWatcher->Removed += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformationUpdate ^>(this, &MidiDeviceWatcher::OnPortRemoved);
    _portUpdatedToken = _deviceWatcher->Updated += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformationUpdate ^>(this, &MidiDeviceWatcher::OnPortUpdated);
    _portEnumCompleteToken = _deviceWatcher->EnumerationCompleted += ref new TypedEventHandler<DeviceWatcher ^, Object ^>(this, &MidiDeviceWatcher::OnPortEnumCompleted);
}

MidiDeviceWatcher::~MidiDeviceWatcher()
{
    _deviceWatcher->Added -= _portAddedToken;
    _deviceWatcher->Removed -= _portRemovedToken;
    _deviceWatcher->Updated -= _portUpdatedToken;
    _deviceWatcher->EnumerationCompleted -= _portEnumCompleteToken;
}

void MidiDeviceWatcher::Start()
{
    _deviceWatcher->Start();
}

void MidiDeviceWatcher::Stop()
{
    _deviceWatcher->Stop();
}

DeviceInformationCollection^ MidiDeviceWatcher::GetDeviceInformationCollection()
{
    return _devInfoCollection;
}

void MidiDeviceWatcher::UpdatePorts()
{
    auto properties = ref new Vector<String^>();
    properties->Append("System.Devices.ContainerId");
    properties->Append("System.Devices.DeviceInstanceId");
    properties->Append("System.Devices.InterfaceClassGuid");
    properties->Append("System.Devices.InterfaceEnabled");
    properties->Append("System.ItemNameDisplay");

    create_task(DeviceInformation::FindAllAsync(_midiSelectorString, properties)).then(
        [this](DeviceInformationCollection^ DeviceInfoCollection)
    {
        _portList->Clear();

        if ((DeviceInfoCollection == nullptr) || (DeviceInfoCollection->Size == 0))
        {
        }
        else
        {
            _devInfoCollection = DeviceInfoCollection;

            // Enumerate through the ports and the custom properties
            for (unsigned int i = 0; i < DeviceInfoCollection->Size; i++)
            {
                _portList->Append(DeviceInfoCollection->GetAt(i)->Name);
            }
        }
    });
}

void MidiDeviceWatcher::OnPortAdded(DeviceWatcher^ deviceWatcher, DeviceInformation^ devInfo)
{
    if (_enumCompleted)
    {
        create_task(_coreDispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
            ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            UpdatePorts();
        })));
    }
}

void MidiDeviceWatcher::OnPortRemoved(DeviceWatcher^ deviceWatcher, DeviceInformationUpdate^ devInfoUpdate)
{
    if (_enumCompleted)
    {
        create_task(_coreDispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
            ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            UpdatePorts();
        })));
    }
}

void MidiDeviceWatcher::OnPortUpdated(DeviceWatcher^ deviceWatcher, DeviceInformationUpdate^ devInfoUpdate)
{
    if (_enumCompleted)
    {
        create_task(_coreDispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
            ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            UpdatePorts();
        })));
    }
}

void MidiDeviceWatcher::OnPortEnumCompleted(DeviceWatcher^ deviceWatcher, Object^ obj)
{
    _enumCompleted = true;

    create_task(_coreDispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
            ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        UpdatePorts();
    })));
}
