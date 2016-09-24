#include "winrtmidiadapterinterface.h"
#include "core/messages/messagehandler.h"
#include "core/messages/messagemidievent.h"

WinRTMidiAdapterInterface::WinRTMidiAdapterInterface()
    : mAdapter(this)
{

}

void WinRTMidiAdapterInterface::init() {
    mAdapter.init();
}

void WinRTMidiAdapterInterface::exit() {
    mAdapter.exit();
}

int WinRTMidiAdapterInterface::GetNumberOfPorts() {
    return mAdapter.GetNumberOfPorts();
}

std::string WinRTMidiAdapterInterface::GetPortName(int i) {
    return mAdapter.GetPortName(i);
}

bool WinRTMidiAdapterInterface::OpenPort (int i, std::string AppName) {
    return mAdapter.OpenPort(i, AppName);
}

bool WinRTMidiAdapterInterface::OpenPort (std::string AppName) {
    return mAdapter.OpenPort(AppName);
}

int WinRTMidiAdapterInterface::getCurrentPort() const {
    return mAdapter.getCurrentPort();
}

void WinRTMidiAdapterInterface::sendMidiEvent(int byte0, int byte1, int byte2, double deltaTime)
{
    LogI("Received midi event");
    MidiAdapter::Data data = {MidiAdapter::byteToEvent(byte0), byte1, byte2, deltaTime};
    MessageHandler::send<MessageMidiEvent>(data);
}
