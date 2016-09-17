#include "winrtmidiadapter.h"

#include <memory>
#include <assert.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "mididevicewatcher.h"
#include "winrtnativehelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Devices::Midi;

namespace {
    MidiDeviceWatcher ^ _midiInDeviceWatcher;
    Vector<String ^ > ^ _inPortsList;
    MidiInPort ^ _midiInPort;
    WinRTMidiAdapterCallback *_inCallback = nullptr;

    std::string convert(Platform::String ^ str) {
        std::wstring fooW(str->Begin());
        return std::string(fooW.begin(), fooW.end());

    }

    void MidiInPort_MessageReceived(MidiInPort ^ sender, MidiMessageReceivedEventArgs ^ args) {
        if (_inCallback) {
            /*auto m = args->Message;
            auto raw = m->RawData;
            auto timestamp = m->Timestamp;
            int length = raw->Length;
            std::vector<unsigned char> data(length);

            std::cout << "Midi: " << length << std::endl;
            auto reader = ::Windows::Storage::Streams::DataReader::FromBuffer(raw);
            reader->ReadBytes(::Platform::ArrayReference<unsigned char>(data.data(), data.size()));*/
            int byte0 = 0;
            int byte1 = 0;
            int byte2 = 0;

            _inCallback->sendMidiEvent(byte0, byte1, byte2, 0);
        }
    }
}


WinRTMidiAdapter::WinRTMidiAdapter(WinRTMidiAdapterCallback *cb)
{
    assert(nullptr != cb);
    _inPortsList = ref new Vector<String ^ >();
    _inCallback = cb;
}

WinRTMidiAdapter::~WinRTMidiAdapter() {
    _inCallback = nullptr;
}

void WinRTMidiAdapter::init() {
    auto currentView = Windows::ApplicationModel::Core::CoreApplication::MainView;
    auto coreWindow = currentView->CoreWindow;
    auto dispatcher = coreWindow->Dispatcher;
    _midiInDeviceWatcher = ref new MidiDeviceWatcher(MidiInPort::GetDeviceSelector(), dispatcher, _inPortsList);
    _midiInDeviceWatcher->Start();

    //Sleep(500);
    // Hack: Wait some time for connected midi devices.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    mCurrentPort = -1;
}

void WinRTMidiAdapter::exit() {
    mCurrentPort = -1;

}

int WinRTMidiAdapter::GetNumberOfPorts () {
    return _inPortsList->Size;
}

std::string WinRTMidiAdapter::GetPortName(int i) {
    return convert(_inPortsList->GetAt(i));
}

bool WinRTMidiAdapter::OpenPort(int i, std::string AppName) {
    mCurrentPort = i;

    if (mCurrentPort < 0) {
      _midiInPort = nullptr;
      return true;
    }

    DeviceInformationCollection^ devInfoCollection = _midiInDeviceWatcher->GetDeviceInformationCollection();
    if (devInfoCollection) {
        DeviceInformation^ devInfo = devInfoCollection->GetAt(mCurrentPort);
        assert(nullptr != devInfo);
        auto task = create_task(MidiInPort::FromIdAsync(devInfo->Id));

        // get results
        try
        {
            // block until port is created
            _midiInPort = task.get();
            //_midiInPort->MessageReceived += ref new TypedEventHandler<MidiInPort ^,MidiMessageReceivedEventArgs^>(&MidiInPort_MessageReceived);
        }
        catch (Platform::Exception^ ex)
        {
            return false;
        }
        //assert(nullptr != _midiInPort);
    } else {
        return false;
    }

    return true;
}

bool WinRTMidiAdapter::OpenPort(std::string AppName) {
    return OpenPort(0, AppName);
}

int WinRTMidiAdapter::getCurrentPort() const {
    return mCurrentPort;
}
