#include "iosnativewrapper.h"
#include "core/system/log.h"
#include "tunerapplication.h"
#include "Qt/platformtools.h"

#import <UIKit/UIKit.h>
#import "PGMidi.h"
#import "iOSVersionDetection.h"

void iosDisableScreensaver() {
    [UIApplication sharedApplication].idleTimerDisabled = YES;
}

void iosReleaseScreensaverLock() {
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

// Application extensions
PGMidi                    *pgMidiInterface;

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
