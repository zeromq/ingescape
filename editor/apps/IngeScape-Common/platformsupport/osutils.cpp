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
#elif defined (Q_OS_WIN)
#include "microsoftwindowutils.h"
#endif

//
// Define our singleton instance
//
Q_GLOBAL_STATIC(OSUtils, _singletonInstance)



//---------------------------------------------------------------------
//
//
//  Public API
//
//
//---------------------------------------------------------------------



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

    //FIXME: check if we need extra parameters
    init();
}


/**
  * @brief Destructor
  */
OSUtils::~OSUtils()
{
   _clean();
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
        _osPreventEnergyEfficiencyFeatures(value);

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
    return _singletonInstance();
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

    OSUtils* singleton = _singletonInstance();

    return singleton;
}


/**
 * @brief Init
 */
void OSUtils::init()
{
#ifdef Q_OS_MAC
    //
    // Macos
    //

    // Subscribe to power notifications
    connect(&MacosUtils::instance(), &MacosUtils::systemSleep, this, &OSUtils::_onSystemSleep);
    connect(&MacosUtils::instance(), &MacosUtils::systemWake, this, &OSUtils::_onSystemWake);

    // Init
    MacosUtils::instance().init();

#elif defined (Q_OS_WIN)
    //
    // Microsoft Windows
    //

    // Subscribe to power notifications
    connect(&MicrosoftWindowUtils::instance(), &MicrosoftWindowUtils::systemSleep, this, &OSUtils::_onSystemSleep);
    connect(&MicrosoftWindowUtils::instance(), &MicrosoftWindowUtils::systemWake, this, &OSUtils::_onSystemWake);


    // Init
    MicrosoftWindowUtils::instance().init();

#else
    //
    // Other OS
    //
    qDebug() << Q_FUNC_INFO << "no implemented for this OS";

#endif


    // Prevent energy efficiency features ?
    _osPreventEnergyEfficiencyFeatures(_preventEnergyEfficiencyFeatures);
}



/**
 * @brief Clean-up our menu
 */
void OSUtils::removeOSGeneratedMenuItems()
{
#ifdef Q_OS_MAC
    // Macos
    MacosUtils::instance().removeOSGeneratedMenuItems();
#endif
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
    _osPreventEnergyEfficiencyFeatures(false);

    // Notify signal
    Q_EMIT systemSleep();
}


/**
 * @brief Triggered when we receive a systemWake signal
 */
void OSUtils::_onSystemWake()
{qDebug() << Q_FUNC_INFO;
    // Update internal state
    setisAwake(true);

    // Prevent energy efficiency features if needed
    _osPreventEnergyEfficiencyFeatures(_preventEnergyEfficiencyFeatures);

    // Notify signal
    Q_EMIT systemWake();
}


/**
 * @brief Clean-up
 */
void OSUtils::_clean()
{
    // Enable energy efficiency features
    _osPreventEnergyEfficiencyFeatures(true);


#ifdef Q_OS_MAC
    //
    // Macos
    //

    // Unsubscribe to power notifications
    disconnect(&MacosUtils::instance(), &MacosUtils::systemSleep, this, &OSUtils::_onSystemSleep);
    disconnect(&MacosUtils::instance(), &MacosUtils::systemWake, this, &OSUtils::_onSystemWake);


    // Clean-up
    MacosUtils::instance().clean();

#elif defined (Q_OS_WIN)
    //
    // Microsoft Windows
    //

    // Unsubscribe to power notifications
    disconnect(&MicrosoftWindowUtils::instance(), &MicrosoftWindowUtils::systemSleep, this, &OSUtils::_onSystemSleep);
    disconnect(&MicrosoftWindowUtils::instance(), &MicrosoftWindowUtils::systemWake, this, &OSUtils::_onSystemWake);


    // Clean-up
    MicrosoftWindowUtils::instance().clean();

#else
    //
    // Other OS
    //
    qDebug() << Q_FUNC_INFO << "no implemented for this OS";
#endif
}



/**
 * @brief Called when our preventEnergyEfficiencyFeatures flag has changed
 *
 * @param value
 */
void OSUtils::_osPreventEnergyEfficiencyFeatures(bool value)
{
#ifdef Q_OS_MAC
    //
    // Macos
    //
    MacosUtils::instance().energyEfficiencyFeaturesEnabled(!value);

#elif defined (Q_OS_WIN)
    //
    // Microsoft Windows
    //
    MicrosoftWindowUtils::instance().energyEfficiencyFeaturesEnabled(!value);

#else
    //
    // Other OS
    //
    Q_UNUSED(value)
    qDebug() << Q_FUNC_INFO << "no implemented for this OS";
#endif
}
