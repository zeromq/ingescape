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
-(void)subcribeToSystemPowerNotification;

// Unsubscribe to system power notifications
-(void)unsubcribeToSystemPowerNotification;


// Enable App Nap
-(void)enableAppNap;

// Disable App Nap
-(void)disableAppNap;


@end



//
// Implementation
//
@implementation MacosUtilsNative

@synthesize preventAppNapActivity = _preventAppNapActivity;


// Callback NSWorkspaceWillSleepNotification
-(void)receiveSleepNotification: (NSNotification*) notification
{
#pragma unused(notification)
    if (MacosUtils::instance() != nullptr)
    {
        MacosUtils::instance()->systemSleep();
    }
}


// Callback receiveWakeNotification
-(void)receiveWakeNotification: (NSNotification*) notification
{
#pragma unused(notification)
    if (MacosUtils::instance() != nullptr)
    {
        MacosUtils::instance()->systemWake();
    }
}


// Subscribe to system power notifications
-(void)subcribeToSystemPowerNotification
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
-(void)unsubcribeToSystemPowerNotification
{
    [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver: self
                                                        name: NSWorkspaceWillSleepNotification
                                                        object: nil];



    [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver: self
                                                        name: NSWorkspaceDidWakeNotification
                                                        object: nil];
}



// Enable App Nap
-(void)enableAppNap
{
   if (_preventAppNapActivity != nil)
   {
       NSProcessInfo* processInfo = [NSProcessInfo processInfo];
       if (
           (processInfo != nil)
           &&
           [processInfo respondsToSelector:@selector(beginActivityWithOptions:reason:)]
           )
       {
           _preventAppNapActivity = [processInfo beginActivityWithOptions:NSActivityUserInitiatedAllowingIdleSystemSleep
                                                 reason:@"Disable App Nap from Qt"];
       }
   }
}


// Disbale App Nap
-(void)disableAppNap
{
    if (_preventAppNapActivity != nil) {

        [[NSProcessInfo processInfo] endActivity:_preventAppNapActivity];
        _preventAppNapActivity = nil;
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
    [_privateAPI->objectiveCAPI subcribeToSystemPowerNotification];
}


/**
 * @brief Destructor
 */
MacosUtils::~MacosUtils()
{
    // Unubscribe to system power notifications
    [_privateAPI->objectiveCAPI unsubcribeToSystemPowerNotification];
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

