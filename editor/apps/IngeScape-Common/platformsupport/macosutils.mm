/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */


#include "macosutils.h"

#include <QDebug>

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>



//---------------------------------------------------------------------
//
//
//  OBJECTIVE-C
//
//
//---------------------------------------------------------------------



//
// Interface
//
@interface MacosUtilsNative : NSObject
// Activity used to prevent app nap
@property (atomic, strong) id preventAppNapActivity;


// Subscribe to system power notifications
-(void)subscribeToSystemPowerNotifications;

// Unsubscribe to system power notifications
-(void)unsubscribeToSystemPowerNotifications;

// Subscribe to user session notifications
-(void)subscribeToUserSessionNotifications;

// Unsubscribe to user session notifications
-(void)unsubscribeToUserSessionNotifications;

// Enable App Nap
-(void)enableAppNap;

// Disable App Nap
-(void)disableAppNap;


@end



//
// Implementation
//
@implementation MacosUtilsNative

@synthesize preventAppNapActivity;


// Callback NSWorkspaceWillSleepNotification
-(void)receiveSleepNotification: (NSNotification*) notification
{
#pragma unused(notification)
    if (MacosUtils::instance() != nullptr)
    {
        MacosUtils::instance()->systemSleep();
    }
}


// Callback NSWorkspaceDidWakeNotification
-(void)receiveWakeNotification: (NSNotification*) notification
{
#pragma unused(notification)
    if (MacosUtils::instance() != nullptr)
    {
        MacosUtils::instance()->systemWake();
    }
}


// Callback "com.apple.screenIsLocked"
-(void)receiveUserSessionIsLocked
{
    if (MacosUtils::instance() != nullptr)
    {
        MacosUtils::instance()->userSessionLocked();
    }
}


// Callback "com.apple.screenIsLocked"
-(void)receiveUserSessionIsUnlocked
{
    if (MacosUtils::instance() != nullptr)
    {
        MacosUtils::instance()->userSessionUnlocked();
    }
}


// Subscribe to system power notifications
-(void)subscribeToSystemPowerNotifications
{
    [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver: self
                                                        selector: @selector(receiveSleepNotification:)
                                                        name: NSWorkspaceWillSleepNotification
                                                        object: nil];



    [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver: self
                                                        selector: @selector(receiveWakeNotification:)
                                                        name: NSWorkspaceDidWakeNotification
                                                        object: nil];
}


// Unsubscribe to system power notifications
-(void)unsubscribeToSystemPowerNotifications
{
    [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver: self
                                                        name: NSWorkspaceWillSleepNotification
                                                        object: nil];



    [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver: self
                                                        name: NSWorkspaceDidWakeNotification
                                                        object: nil];
}


// Subscribe to user session notifications
-(void)subscribeToUserSessionNotifications
{
    [[NSDistributedNotificationCenter defaultCenter] addObserver: self
                                                     selector: @selector(receiveUserSessionIsLocked)
                                                     name: @"com.apple.screenIsLocked"
                                                     object: nil];

    [[NSDistributedNotificationCenter defaultCenter] addObserver: self
                                                     selector: @selector(receiveUserSessionIsUnlocked)
                                                     name: @"com.apple.screenIsUnlocked"
                                                     object: nil];
}


// Unsubscribe to user session notifications
-(void)unsubscribeToUserSessionNotifications
{
    [[NSDistributedNotificationCenter defaultCenter] removeObserver: self
                                                     name: @"com.apple.screenIsLocked"
                                                     object: nil];

    [[NSDistributedNotificationCenter defaultCenter] removeObserver: self
                                                     name: @"com.apple.screenIsUnlocked"
                                                     object: nil];
}




// Enable App Nap
-(void)enableAppNap
{
   if (self.preventAppNapActivity != nil)
   {
       NSProcessInfo* processInfo = [NSProcessInfo processInfo];
       if (
           (processInfo != nil)
           &&
           [processInfo respondsToSelector:@selector(beginActivityWithOptions:reason:)]
           )
       {
           self.preventAppNapActivity = [processInfo beginActivityWithOptions:NSActivityUserInitiatedAllowingIdleSystemSleep
                                                 reason:@"Disable App Nap from Qt"];
       }
   }
}


// Disbale App Nap
-(void)disableAppNap
{
    if (self.preventAppNapActivity != nil) {

        [[NSProcessInfo processInfo] endActivity:self.preventAppNapActivity];
        self.preventAppNapActivity = nil;
    }
}

@end




//---------------------------------------------------------------------
//
//
//  PRIVATE API
//
//
//---------------------------------------------------------------------


/**
 * @brief The MacosUtilsPrivate class defines a private API used to store Objective-C objects
 */
class MacosUtilsPrivate {
public:
    /**
     * @brief Constructor
     */
    MacosUtilsPrivate()
        : objectiveCAPI(nil)
    {
    }


public:
    MacosUtilsNative* objectiveCAPI;
};




//---------------------------------------------------------------------
//
//
//  MacosUtils
//
//
//---------------------------------------------------------------------


//
// Define our singleton instance
//
Q_GLOBAL_STATIC(MacosUtils, _singletonInstance)



/**
 * @brief Constructor
 */
MacosUtils::MacosUtils(QObject* parent)
    : OSUtils(parent),
      _privateAPI(new MacosUtilsPrivate())
{
    MacosUtilsNative* macosUtilsNative = [[MacosUtilsNative alloc] init];
    _privateAPI->objectiveCAPI = macosUtilsNative;

    // Subscribe to system power notifications
    [_privateAPI->objectiveCAPI subscribeToSystemPowerNotifications];

    // Subscribe to user session notifications
    [_privateAPI->objectiveCAPI subscribeToUserSessionNotifications];
}


/**
 * @brief Destructor
 */
MacosUtils::~MacosUtils()
{
    // Unubscribe to system power notifications
    [_privateAPI->objectiveCAPI unsubscribeToSystemPowerNotifications];

    // Unsubscribe to user session notifications
    [_privateAPI->objectiveCAPI unsubscribeToUserSessionNotifications];

    // Re-enable App Nap
    [_privateAPI->objectiveCAPI enableAppNap];
}


/**
 * @brief Get our singleton instance
 * @return
 */
MacosUtils* MacosUtils::instance()
{
    return _singletonInstance;
}


/**
 * @brief Remove all menu items generated by Qt to handle conventions
 */
void MacosUtils::removeOSGeneratedMenuItems()
{
    // Clean-up our Edit menu
    _removeMenuItemStartDictation();
    _removeMenuItemEmojiAndSymbols();
}


/**
 * @brief Enable energy efficiency features
 */
void MacosUtils::_enableEnergyEfficiencyFeatures()
{
    [_privateAPI->objectiveCAPI enableAppNap];
}


/**
 * @brief Disable energy efficiency features
 */
void MacosUtils::_disableEnergyEfficiencyFeatures()
{
    [_privateAPI->objectiveCAPI disableAppNap];
}


/**
 * @brief Remove the "Start Dictation..." menu item from the "Edit" menu
 */
void MacosUtils::_removeMenuItemStartDictation()
{
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledDictationMenuItem"];
}


/**
 * @brief Remove the "Emoji & Symbols" menu item from the "Edit" menu
 */
void MacosUtils::_removeMenuItemEmojiAndSymbols()
{
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledCharacterPaletteMenuItem"];
}

