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

#include "osutils.h"


#include <QDebug>
#include <QtGlobal>
#include <QQmlEngine>

#ifdef Q_OS_MAC
#include "macosutils.h"
#elif defined(Q_OS_WIN)
#include "microsoftwindowutils.h"
#elif defined(Q_OS_LINUX)
#include "linuxutils.h"
#endif



//---------------------------------------------------------------------
//
//
//  Public API
//
//
//---------------------------------------------------------------------

//
// Define our singleton instance
//
Q_GLOBAL_STATIC(OSUtils, _singletonInstance)


/**
 * @brief Constructor
 * @param parent
 */
OSUtils::OSUtils(QObject *parent)
    : QObject(parent),
      _isAwake(true), // We assume that our system is awake
      _preventEnergyEfficiencyFeatures(false)
{
    // Force C++ ownership, otherwise our singleton will be owned by the QML engine
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Subscribe to system power notifications
    connect(this, &OSUtils::systemSleep, this, &OSUtils::_onSystemSleep);
    connect(this, &OSUtils::systemWake, this, &OSUtils::_onSystemWake);
}


/**
  * @brief Destructor
  */
OSUtils::~OSUtils()
{
    // Unsubscribe to system power notifications
    disconnect(this, &OSUtils::systemSleep, this, &OSUtils::_onSystemSleep);
    disconnect(this, &OSUtils::systemWake, this, &OSUtils::_onSystemWake);
}


/**
 * @briefSet our preventEnergyEfficiencyFeatures flag
 * @param value
 */
void OSUtils::setpreventEnergyEfficiencyFeatures(bool value)
{
    if (_preventEnergyEfficiencyFeatures != value)
    {
        // Save value
        _preventEnergyEfficiencyFeatures = value;

        // Update our OS
        if (value)
        {
            _disableEnergyEfficiencyFeatures();
        }
        else
        {
            _enableEnergyEfficiencyFeatures();
        }

        // Notify change
        Q_EMIT preventEnergyEfficiencyFeaturesChanged(value);
    }
}


/**
 * @brief Get our singleton instance
 * @return
 */
OSUtils* OSUtils::instance()
{
    // Fallback value
    OSUtils* result = _singletonInstance;

    // Check OS
#ifdef Q_OS_MAC
    result = MacosUtils::instance();
#elif defined(Q_OS_WIN)
    result = MicrosoftWindowUtils::instance();
#elif defined(Q_OS_LINUX)
    result = LinuxOSUtils::instance();
#endif

    return result;
}


/**
 * @brief Method used to provide a singleton to QML
 * @param qmlEngine
 * @param scriptEngine
 * @return
 */
QObject* OSUtils::qmlSingleton(QQmlEngine* qmlEngine, QJSEngine* scriptEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(scriptEngine);

    return instance();
}


/**
 * @brief Clean-up our menu
 */
void OSUtils::removeOSGeneratedMenuItems()
{
}



//---------------------------------------------------------------------
//
//
//  Private / proctected API
//
//
//---------------------------------------------------------------------

/**
 * @brief Triggered when we receive a systemSleep signal
 */
void OSUtils::_onSystemSleep()
{qDebug() << Q_FUNC_INFO;
    // Update internal state
    setisAwake(false);

    // Re-enable energy efficiency features
    _enableEnergyEfficiencyFeatures();
}


/**
 * @brief Triggered when we receive a systemWake signal
 */
void OSUtils::_onSystemWake()
{qDebug() << Q_FUNC_INFO;
    // Update internal state
    setisAwake(true);

    // Prevent energy efficiency features if needed
    if (_preventEnergyEfficiencyFeatures)
    {
        _disableEnergyEfficiencyFeatures();
    }
}


/**
 * @brief Enable energy efficiency features
 */
void OSUtils::_enableEnergyEfficiencyFeatures()
{
}


/**
 * @brief Disable energy efficiency features
 */
void OSUtils::_disableEnergyEfficiencyFeatures()
{
}



