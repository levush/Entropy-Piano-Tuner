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

    WinRTMidiAdapterCallback * _rawCallback = nullptr;

    ref class MidiMessageCallbackWrapper sealed {
    public:
        MidiMessageCallbackWrapper()
        {
        }

        void onMessageReceived(MidiInPort^ sender, MidiMessageReceivedEventArgs^ args) {
            if (_rawCallback) {
                IMidiMessage^ midiMessage = args->Message;
                switch (midiMessage->Type) {
                case MidiMessageType::NoteOff: {
                    MidiNoteOffMessage^ noteOff = static_cast<MidiNoteOffMessage ^>(midiMessage);
                    _rawCallback->sendMidiEvent(0x80, noteOff->Note, noteOff->Velocity, 0);
                    break;
                }
                case MidiMessageType::NoteOn: {
                    MidiNoteOnMessage^ noteOn = static_cast<MidiNoteOnMessage ^>(midiMessage);
                    _rawCallback->sendMidiEvent(0x90, noteOn->Note, noteOn->Velocity, 0);
                    break;
                }
                case MidiMessageType::PolyphonicKeyPressure: {
                    MidiPolyphonicKeyPressureMessage^ polyKeyMessage = static_cast<MidiPolyphonicKeyPressureMessage ^>(midiMessage);
                    _rawCallback->sendMidiEvent(0xA0, polyKeyMessage->Note, polyKeyMessage->Pressure, 0);
                    break;
                }
                case MidiMessageType::ControlChange: {
                    MidiControlChangeMessage^ controlMessage = static_cast<MidiControlChangeMessage ^>(midiMessage);
                    _rawCallback->sendMidiEvent(0xB0, controlMessage->Controller, controlMessage->ControlValue, 0);
                    break;
                }
                case MidiMessageType::ProgramChange: {
                    MidiProgramChangeMessage^ programMessage = static_cast<MidiProgramChangeMessage ^>(midiMessage);
                    _rawCallback->sendMidiEvent(0xC0, programMessage->Program, 0, 0);
                    break;
                }
                case MidiMessageType::ChannelPressure: {
                    MidiChannelPressureMessage^ channelPressureMessage = static_cast<MidiChannelPressureMessage ^>(midiMessage);
                    _rawCallback->sendMidiEvent(0xD0, channelPressureMessage->Pressure, 0, 0);
                    break;
                }
                case MidiMessageType::PitchBendChange: {
                    MidiPitchBendChangeMessage^ pitchBendMessage = static_cast<MidiPitchBendChangeMessage ^>(midiMessage);
                    _rawCallback->sendMidiEvent(0xD0, pitchBendMessage->Channel, 0, 0);
                    break;
                }
                case MidiMessageType::SystemExclusive: {
                    MidiSystemExclusiveMessage^ sysExMessage = static_cast<MidiSystemExclusiveMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::MidiTimeCode: {
                    MidiTimeCodeMessage^ mtcMessage = static_cast<MidiTimeCodeMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::SongPositionPointer: {
                    MidiSongPositionPointerMessage^ songPositionMessage = static_cast<MidiSongPositionPointerMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::SongSelect: {
                    MidiSongSelectMessage^ songSelectMessage = static_cast<MidiSongSelectMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::TuneRequest: {
                    MidiTuneRequestMessage^ tuneRequestMessage = static_cast<MidiTuneRequestMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::TimingClock: {
                    MidiTimingClockMessage^ timingClockMessage = static_cast<MidiTimingClockMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::Start: {
                    MidiStartMessage^ startMessage = static_cast<MidiStartMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::Continue: {
                    MidiContinueMessage^ continueMessage = static_cast<MidiContinueMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::Stop: {
                    MidiStopMessage^ stopMessage = static_cast<MidiStopMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::ActiveSensing: {
                    MidiActiveSensingMessage^ activeSensingMessage = static_cast<MidiActiveSensingMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::SystemReset: {
                    MidiSystemResetMessage^ systemResetMessage = static_cast<MidiSystemResetMessage ^>(midiMessage);
                    break;
                }
                case MidiMessageType::None:
                default:
                    break;
                }
            }
        }

    private:
    };

    MidiMessageCallbackWrapper ^ _inCallback = nullptr;

    std::string convert(Platform::String ^ str) {
        std::wstring fooW(str->Begin());
        return std::string(fooW.begin(), fooW.end());

    }
}


WinRTMidiAdapter::WinRTMidiAdapter(WinRTMidiAdapterCallback *cb)
{
    assert(nullptr != cb);
    _inPortsList = ref new Vector<String ^ >();
    _inCallback = ref new MidiMessageCallbackWrapper();
    _rawCallback = cb;
}

WinRTMidiAdapter::~WinRTMidiAdapter() {
    _inCallback = nullptr;
    _rawCallback = nullptr;
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
        auto task = create_task(MidiInPort::FromIdAsync(devInfo->Id)).then([this](MidiInPort^ inPort)
        {
            _midiInPort = inPort;
            if (nullptr != inPort) {
                inPort->MessageReceived += ref new TypedEventHandler<MidiInPort ^,MidiMessageReceivedEventArgs^>(_inCallback, &MidiMessageCallbackWrapper::onMessageReceived);
            }
        });
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
