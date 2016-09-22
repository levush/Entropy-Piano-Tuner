#include "midiimplementation.h"
#include "core/system/log.h"

MidiImplementation::MidiImplementation()
{

}

void MidiImplementation::init() {
    auto r = midi::manager().init(true, false);
    if (r != midi::OK) {
        LogW("Midi backend could not be initialized. Error code: %d", r);
    }
}

void MidiImplementation::exit() {
    auto r = midi::manager().exit();
    if (r != midi::OK) {
        LogW("Midi backend could not be closed. Error code: %d", r);
    }
}

//-------------- Return the number of available Midi devices ------------

/// \return Always return that no Midi ports are available
int MidiImplementation::GetNumberOfPorts () {
    return midi::manager().listAvailableInputDevices().size();
}


//------------------------Get the Midi device name ----------------------

////////////////////////////////////////////////////////////////////////
/// \param i : Number of the port, ignored here
/// \return Returns a string saying that no devices are available
////////////////////////////////////////////////////////////////////////

std::string MidiImplementation::GetPortName (int i)
{
    (void)i; // Do not use this arguement
    return "Midi system not available.";
}

//------------------------ Open a particular port -----------------------

////////////////////////////////////////////////////////////////////////
/// \param i : Number of the Midi port to be opened, ignored here
/// \param AppName : Name of the application, ignored here
/// \return always false since there is no port to be opened
////////////////////////////////////////////////////////////////////////

bool MidiImplementation::OpenPort (int i, std::string AppName)
{
    (void)i; (void)AppName;  // Do not use these arguments
    return false;
}


//--------------------- Open the port with the highest number ------------

////////////////////////////////////////////////////////////////////////
/// \param AppName : Name of the application, ignored here
/// \return always false since there is no port to be opened
////////////////////////////////////////////////////////////////////////

bool MidiImplementation::OpenPort (std::string AppName)
{
    (void)AppName; // Do not use this argument
    midi::MidiResult r;
    midi::MidiInputDevicePtr device;
    std::tie(r, device) = midi::manager().createDefaultInputDevice();
    if (r != midi::OK) {
        LogW("Default input device could not be created. Error Code: %d", r);
        return false;
    }
    device->addListener(this);
    LogI("Connected to midi input device %s", device->id()->humanReadable().c_str());
    return true;
}

int MidiImplementation::getCurrentPort() const {
    return -1;
}

void MidiImplementation::receiveMessage(int cmd, int byte1, int byte2) {
    Data data = {byteToEvent(cmd), byte1, byte2, 0.0};
    send(data);
}
