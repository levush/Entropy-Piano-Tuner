#include "winrtmidiadapterinterface.h"
#include "core/messages/messagehandler.h"
#include "core/messages/messagemidievent.h"

WinRTMidiAdapterInterface::WinRTMidiAdapterInterface()
    : mAdapter(this)
{

}

void WinRTMidiAdapterInterface::sendMidiEvent(int byte0, int byte1, int byte2, double deltaTime)
{
    MidiAdapter::Data data = {MidiAdapter::byteToEvent(byte0), byte1, byte2, deltaTime};
    MessageHandler::send<MessageMidiEvent>(data);
}
