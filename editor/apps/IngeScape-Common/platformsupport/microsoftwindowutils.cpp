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

#include "microsoftwindowutils.h"

#include <QDebug>


#ifdef Q_OS_WIN

#include <windows.h>
#include <Wtsapi32.h>


//---------------------------------------------------------------------
//
//
//  Microsoft Window API
//
//
//---------------------------------------------------------------------

/**
 * @brief Constructor
 */
MicrosoftWindowUtilsEventFilter::MicrosoftWindowUtilsEventFilter()
{
}


/**
 * @brief This method is called for every native event
 * @param eventType
 * @param message
 * @param result
 * @return
 */
bool MicrosoftWindowUtilsEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(message)
    Q_UNUSED(result)

    //TODO: check if we also need "windows_dispatcher_MSG" (system-wide messages)
    if (eventType == "windows_generic_MSG")
    {
        MSG* msg = static_cast<MSG*>(message);
        if (msg != nullptr)
        {
            // Check if we have a system power notification
            if (msg->message == WM_POWERBROADCAST)
            {
                // See https://docs.microsoft.com/en-us/windows/win32/power/wm-powerbroadcast

                if (msg->wParam == PBT_APMRESUMEAUTOMATIC)
                {
                    if (MicrosoftWindowUtils::instance() != nullptr)
                    {
                        MicrosoftWindowUtils::instance()->systemWake();
                    }
                }
                else if (msg->wParam == PBT_APMSUSPEND)
                {
                    if (MicrosoftWindowUtils::instance() != nullptr)
                    {
                        MicrosoftWindowUtils::instance()->systemSleep();
                    }
                }
            }
            // Check if we have a user session notification
            else if (msg->message == WM_WTSSESSION_CHANGE)
            {
                // NB: WM_WTSSESSION_CHANGE messages are received twice

                if (msg->wParam == WTS_SESSION_LOCK)
                {
                    if (
                        (MicrosoftWindowUtils::instance() != nullptr)
                        &&
                        // We need this guard because WTS_SESSION_LOCK is received twice
                        !(MicrosoftWindowUtils::instance()->isUserSessionLocked())
                        )
                    {
                        MicrosoftWindowUtils::instance()->userSessionLocked();
                    }
                }
                else if (msg->wParam == WTS_SESSION_UNLOCK)
                {
                    if (
                        (MicrosoftWindowUtils::instance() != nullptr)
                        &&
                        // We need this guard because WTS_SESSION_UNLOCK is received twice
                        MicrosoftWindowUtils::instance()->isUserSessionLocked()
                        )
                    {
                        MicrosoftWindowUtils::instance()->userSessionUnlocked();
                    }
                }
            }
        }
    }

    // No filter
    return false;
}



//
// Define our singleton instance
//
Q_GLOBAL_STATIC(MicrosoftWindowUtils, _singletonInstance)


/**
 * @brief Constructor
 * @param parent
 */
MicrosoftWindowUtils::MicrosoftWindowUtils(QObject *parent)
    : OSUtils(parent),
      _eventFilter(nullptr)
{
    // Try to install our event filter
    if (QCoreApplication::instance() != nullptr)
    {
        _eventFilter = new MicrosoftWindowUtilsEventFilter();
        QCoreApplication::instance()->installNativeEventFilter(_eventFilter);
    }
}


/**
 * @brief Destructor
 */
MicrosoftWindowUtils::~MicrosoftWindowUtils()
{
    // Clean-up our event filter
    if (_eventFilter != nullptr)
    {
        if (QCoreApplication::instance() != nullptr)
        {
            QCoreApplication::instance()->removeNativeEventFilter(_eventFilter);
        }
        // Else: should not happen because we can not create an event filter without a QCoreApplication

        delete _eventFilter;
        _eventFilter = nullptr;
    }    
}


/**
 * @brief Override the setter of our currentWindow property
 * @param value
 * @return
 */
bool MicrosoftWindowUtils::setcurrentWindow (QQuickWindow* value)
{
    bool hasChanged = false;

    if (_currentWindow != value)
    {
        // Unsubscribe to user session notifications
        _unsubscribeToUserSessionNotifications(_currentWindow);

        // Subscribe to user session notifications
        _subscribeToUserSessionNotifications(value);

        // Update value
        hasChanged = OSUtils::setcurrentWindow(value);
    }

    return hasChanged;
}



/**
 * @brief Get our singleton instance
 * @return
 */
MicrosoftWindowUtils* MicrosoftWindowUtils::instance()
{
    return _singletonInstance;
}


/**
 * @brief Remove all menu items generated by Qt to handle conventions
 */
void MicrosoftWindowUtils::removeOSGeneratedMenuItems()
{
    // Nothing to do: Qt does not add extra menu items on Windows
}


/**
 * @brief Enable energy efficiency features
 */
void MicrosoftWindowUtils::_enableEnergyEfficiencyFeatures()
{
    //TODO: Power Throttling (Windows 10 April 2018 and above)
}


/**
 * @brief Disable energy efficiency features
 */
void MicrosoftWindowUtils::_disableEnergyEfficiencyFeatures()
{
    //TODO: Power Throttling (Windows 10 April 2018 and above)
}


/**
 * @brief Subscribe to user session notifications
 */
void MicrosoftWindowUtils::_subscribeToUserSessionNotifications(QQuickWindow* window)
{
    if (window != nullptr)
    {
        HWND hwnd = reinterpret_cast<HWND>(window->winId());
        if (hwnd != NULL)
        {
            if (!WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION))
            {
                qWarning() << "MicrosoftWindowUtils failed to register user session notifications: "
                           << GetLastError();
            }
        }
    }
}


/**
 * @brief Unsubscribe to user session notifications
 */
void MicrosoftWindowUtils::_unsubscribeToUserSessionNotifications(QQuickWindow* window)
{
    if (window != nullptr)
    {
        HWND hwnd = reinterpret_cast<HWND>(window->winId());
        if (hwnd != NULL)
        {
            if (!WTSUnRegisterSessionNotification(hwnd))
            {
                qWarning() << "MicrosoftWindowUtils failed to unregister user session notifications: "
                           << GetLastError();
            }
        }
    }
}



#else

//---------------------------------------------------------------------
//
//  Fake API to avoid compiler errors (empty source file)
//
//---------------------------------------------------------------------

/**
 * @brief Constructor
 * @param parent
 */
MicrosoftWindowUtils::MicrosoftWindowUtils(QObject *parent)
    : OSUtils(parent)
{
}


#endif

