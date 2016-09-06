#include "winrtmidiadapter.h"

#include <memory>

#include "mididevicewatcher.h"

using namespace Windows::UI::Core;
using namespace Windows::Devices::Midi;

namespace {
    MidiDeviceWatcher ^ _midiInDeviceWatcher;
    Vector<String ^ > ^ _inPortsList;

    std::string convert(Platform::String ^ str) {
        std::wstring fooW(str->Begin());
        return std::string(fooW.begin(), fooW.end());
    }
}


WinRTMidiAdapter::WinRTMidiAdapter()
{
    _inPortsList = ref new Vector<String ^ >();
}

void WinRTMidiAdapter::init() {
    auto currentView = Windows::ApplicationModel::Core::CoreApplication::MainView;
    auto coreWindow = currentView->CoreWindow;
    auto dispatcher = coreWindow->Dispatcher;
    _midiInDeviceWatcher = ref new MidiDeviceWatcher(MidiInPort::GetDeviceSelector(), dispatcher, _inPortsList);
    _midiInDeviceWatcher->Start();

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
    return true;
}

bool WinRTMidiAdapter::OpenPort(std::string AppName) {
    return OpenPort(0, AppName);
}

int WinRTMidiAdapter::getCurrentPort() const {
    return mCurrentPort;
}
