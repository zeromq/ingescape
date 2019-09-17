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
      _currentWindow(nullptr),
      _isAwake(true), // We assume that our system is awake
      _isUserSessionLocked(false), // We assume that our user session is not locked
      _hasEnergyEfficiencyFeatures(false), // We assume that all OS don't have energy efficiency features
      _preventEnergyEfficiencyFeatures(false)
{
    // Force C++ ownership, otherwise our singleton will be owned by the QML engine
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Subscribe to system power notifications
    connect(this, &OSUtils::systemSleep, this, &OSUtils::_onSystemSleep);
    connect(this, &OSUtils::systemWake, this, &OSUtils::_onSystemWake);

    // Subscribe to user session notifications
    connect(this, &OSUtils::userSessionLocked, this, &OSUtils::_onUserSessionLocked);
    connect(this, &OSUtils::userSessionUnlocked, this, &OSUtils::_onUserSessionUnlocked);

    // Subscribe to network configuration events
    connect(&_networkConfigurationManager, &QNetworkConfigurationManager::configurationAdded, this, &OSUtils::_onConfigurationAdded);
    connect(&_networkConfigurationManager, &QNetworkConfigurationManager::configurationRemoved, this, &OSUtils::_onConfigurationRemoved);
    connect(&_networkConfigurationManager, &QNetworkConfigurationManager::configurationChanged, this, &OSUtils::_onConfigurationChanged);
}


/**
  * @brief Destructor
  */
OSUtils::~OSUtils()
{
    // Clean-up properties
    setcurrentWindow(nullptr);

    // Unsubscribe to network configuration events
    disconnect(&_networkConfigurationManager, &QNetworkConfigurationManager::configurationAdded, this, &OSUtils::_onConfigurationAdded);
    disconnect(&_networkConfigurationManager, &QNetworkConfigurationManager::configurationRemoved, this, &OSUtils::_onConfigurationRemoved);
    disconnect(&_networkConfigurationManager, &QNetworkConfigurationManager::configurationChanged, this, &OSUtils::_onConfigurationChanged);

    // Unsubscribe to system power notifications
    disconnect(this, &OSUtils::systemSleep, this, &OSUtils::_onSystemSleep);
    disconnect(this, &OSUtils::systemWake, this, &OSUtils::_onSystemWake);

    // Unsubscribe to user session notifications
    disconnect(this, &OSUtils::userSessionLocked, this, &OSUtils::_onUserSessionLocked);
    disconnect(this, &OSUtils::userSessionUnlocked, this, &OSUtils::_onUserSessionUnlocked);
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
    result = LinuxUtils::instance();
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
 * @brief Called when we receive a systemSleep signal
 */
void OSUtils::_onSystemSleep()
{qDebug() << Q_FUNC_INFO;
    // Update internal state
    setisAwake(false);

    // Re-enable energy efficiency features
    _enableEnergyEfficiencyFeatures();
}


/**
 * @brief Called when we receive a systemWake signal
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
 * @brief Called when we receive a userSessionLocked signal
 */
void OSUtils::_onUserSessionLocked()
{qDebug() << Q_FUNC_INFO;
    // Update internal state
    setisUserSessionLocked(true);

    //FIXME: do we need to re-enable energy efficiency features ?
}


/**
 * @brief Called when we receive a userSessionUnlocked signal
 */
void OSUtils::_onUserSessionUnlocked()
{qDebug() << Q_FUNC_INFO;
    // Update internal state
    setisUserSessionLocked(false);
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


/**
 * @brief Called when a configuration is added to the system
 * @param config
 */
void OSUtils::_onConfigurationAdded(const QNetworkConfiguration &config)
{
    Q_UNUSED(config)
    Q_EMIT systemNetworkConfigurationsUpdated();
}


/**
 * @brief Called when a configuration is about to be removed from the system
 * @param config
 */
void OSUtils::_onConfigurationRemoved(const QNetworkConfiguration &config)
{
    Q_UNUSED(config)
    Q_EMIT systemNetworkConfigurationsUpdated();
}


/**
 * @brief Called when a configuration has changed (ex: Wifi on <=> off)
 * @param config
 */
void OSUtils::_onConfigurationChanged(const QNetworkConfiguration &config)
{
    Q_UNUSED(config)
    Q_EMIT systemNetworkConfigurationsUpdated();
}

