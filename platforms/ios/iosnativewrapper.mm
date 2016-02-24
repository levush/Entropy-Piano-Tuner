/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

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

unsigned long long iosGetInstalledPhysicalMemoryInB() {
    return [NSProcessInfo processInfo].physicalMemory;
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
    if (pgMidiController != nil) {
        [pgMidiController release];
        pgMidiController = nil;
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
    for (uint i = 0; i < [pgMidiInterface.sources count]; i++)
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
    (void)application; // unused parameter
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
    (void) midi; // unused parameter
    LogI("Source added.");
    for (uint i = 0; i < [pgMidiInterface.sources count]; i++)
    {
        if (pgMidiInterface.sources[i] == source) {
            iosMidiOpenPort(i);
            break;
        }
    }
}

- (void) midi:(PGMidi*)midi sourceRemoved:(PGMidiSource *)source
{
    (void)midi; // unused parameter
    (void)source; // unused parameter
    LogI("Source removed");
}

- (void) midi:(PGMidi*)midi destinationAdded:(PGMidiDestination *)destination
{
    (void)midi; // unused parameter
    (void)destination; // unused parameter
}

- (void) midi:(PGMidi*)midi destinationRemoved:(PGMidiDestination *)destination
{
    (void)midi; // unused parameter
    (void)destination; // unused parameter
}

- (void) midiSource:(PGMidiSource*)midi midiReceived:(const MIDIPacketList *)packetList
{
    (void)midi; // unused parameter
    const MIDIPacket *packet = &packetList->packet[0];
    for (uint i = 0; i < packetList->numPackets; ++i)
    {
        // handle the packet:
        // only handle packets of length 3, because keypress/release have length 3
        if (packet->length == 3) {
            // read bytes
            int byte0 = packet->data[0];
            int byte1 = packet->data[1];
            int byte2 = packet->data[2];
            double deltaTime = 0;  // unused

            // send message
            MidiAdapter::Data data = {MidiAdapter::byteToEvent(byte0), byte1, byte2, deltaTime};
            MessageHandler::send<MessageMidiEvent>(data);
        }

        packet = MIDIPacketNext(packet);
    }
}
@end
