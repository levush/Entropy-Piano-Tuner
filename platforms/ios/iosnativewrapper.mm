#include "iosnativewrapper.h"
#include "core/system/log.h"
#include "core/system/eptexception.h"
#include "tunerapplication.h"
#include "Qt/platformtools.h"
#include "core/messages/messagehandler.h"
#include "core/messages/messagemidievent.h"

#import <UIKit/UIKit.h>
#import "PGMidi.h"
#import "iOSVersionDetection.h"


// midi interface declaration
@interface MidiMonitorViewController : NSObject
{

}
@end

// variables
PGMidi                    *pgMidiInterface;
MidiMonitorViewController *pgMidiController;
int                        pgMidiCurrentPort = -1;

void iosInit() {
}

void iosDisableScreensaver() {
    [UIApplication sharedApplication].idleTimerDisabled = YES;
}

void iosReleaseScreensaverLock() {
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

// midi functions
// =================================================================================================
void iosMidiInit() {
    LogI("Initializing PGMidi");
    pgMidiController = [MidiMonitorViewController alloc];
    pgMidiInterface.delegate = (id<PGMidiDelegate>)pgMidiController;
}

void iosMidiExit() {
    LogI("Exiting PGMidi");
    if (!pgMidiController) {
        [pgMidiController release];
    }
    pgMidiCurrentPort = -1;
}

int iosMidiGetNumberOfPorts() {
    if (!pgMidiInterface) {return 0;}
    return (int)[pgMidiInterface.sources count];
}

std::string iosMidiGetPortName(int i) {
    EptAssert(pgMidiInterface, "Midi not initialized");
    EptAssert(i < iosMidiGetNumberOfPorts(), "Index out of range.");
    PGMidiSource *source = pgMidiInterface.sources[i];
    EptAssert(source, "Source not existing");
    return std::string([source.name UTF8String]);
}

bool iosMidiOpenPort(int i) {
    EptAssert(pgMidiInterface, "Midi not initialized");
    EptAssert(pgMidiController, "Midi controller not initialized");
    EptAssert(i < iosMidiGetNumberOfPorts(), "Index out of range.");
    PGMidiSource *source = pgMidiInterface.sources[i];
    EptAssert(source, "Source not existing");

    [source addDelegate:(id<PGMidiSourceDelegate>)pgMidiController];
    pgMidiCurrentPort = i;
    return true;
}

bool iosMidiOpenPort() {
    EptAssert(pgMidiInterface, "Midi not initialized");
    // check if there are ports
    if ([pgMidiInterface.sources count] == 0) {return false;}

    // select first non network port, first, then network port
    EptAssert(pgMidiController, "Midi controller not initialized");

    int midiPort = 0;
    for (int i = 0; i < [pgMidiInterface.sources count]; i++)
    {
        PGMidiSource *source = pgMidiInterface.sources[i];
        if (!source.isNetworkSession) {
            midiPort = i;
            break;
        }
    }

    return iosMidiOpenPort(midiPort);
}

int iosMidiGetCurrentPort() {
    return pgMidiCurrentPort;
}

// Application extensions
// =================================================================================================

// Make QIOSApplicationDelegate known
@interface QIOSApplicationDelegate
@end

// Add a category to QIOSApplicationDelegate
@interface QIOSApplicationDelegate (CategoryFileOpeningOnStartup)
@end

// Now add new methods
@implementation QIOSApplicationDelegate (CategoryFileOpeningOnStartup)
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
#pragma unused(application)
#pragma unused(sourceApplication)
#pragma unused(annotation)
    if ([url isFileURL])
    {
        NSString *file = [[NSString alloc] initWithString:[url path]];
        // Handle file being passed in
        PlatformTools::getSingleton()->openFile([file UTF8String], true);
    }
    else
    {
        // Handle custom URL scheme
    }
return TRUE;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    IF_IOS_HAS_COREMIDI
    (
        // We only create a MidiInput object on iOS versions that support CoreMIDI
        pgMidiInterface                            = [[PGMidi alloc] init];
        pgMidiInterface.networkEnabled             = YES;
        pgMidiInterface.virtualDestinationEnabled  = YES;
        pgMidiInterface.virtualSourceEnabled       = YES;
    )

    return YES;
}

@end



// midi implementation

@interface MidiMonitorViewController () <PGMidiDelegate, PGMidiSourceDelegate>
@end

@implementation MidiMonitorViewController
- (void) midi:(PGMidi*)midi sourceAdded:(PGMidiSource *)source
{
    [source addDelegate:self];
    LogI("Source added.");
}

- (void) midi:(PGMidi*)midi sourceRemoved:(PGMidiSource *)source
{
    LogI("Source removed");
}

- (void) midi:(PGMidi*)midi destinationAdded:(PGMidiDestination *)destination
{
}

- (void) midi:(PGMidi*)midi destinationRemoved:(PGMidiDestination *)destination
{
}

- (void) midiSource:(PGMidiSource*)midi midiReceived:(const MIDIPacketList *)packetList
{
    LogI("Midi packget received");
    [self performSelectorOnMainThread:@selector(addString:)
                           withObject:@"MIDI received:"
                        waitUntilDone:NO];

    const MIDIPacket *packet = &packetList->packet[0];
    for (int i = 0; i < packetList->numPackets; ++i)
    {
        packet = MIDIPacketNext(packet);
        // handle the packet:
        // only handle packets of length 3, because keypress/release have length 3
        if (packet->length != 3) {
            continue;
        }

        // read bytes
        int event = packet->data[0];
        int byte1 = packet->data[1];
        int byte2 = packet->data[2];
        double deltaTime = 0;  // unused

        // send message
        MidiAdapter::Data data = {static_cast<MidiAdapter::Event>(event), byte1, byte2, deltaTime};
        MessageHandler::send<MessageMidiEvent>(data);
    }
}
@end
