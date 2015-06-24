#include "iosnativewrapper.h"
#include "core/system/log.h"
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

void iosInit() {
    LogI("Initializing PGMidi");
    pgMidiController = [MidiMonitorViewController alloc];
    pgMidiInterface.delegate = (id<PGMidiDelegate>)pgMidiController;
    for (PGMidiSource *source in pgMidiInterface.sources)
    {
        [source addDelegate:(id<PGMidiSourceDelegate>)pgMidiController];
    }
}

void iosDisableScreensaver() {
    [UIApplication sharedApplication].idleTimerDisabled = YES;
}

void iosReleaseScreensaverLock() {
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

// Application extensions

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
