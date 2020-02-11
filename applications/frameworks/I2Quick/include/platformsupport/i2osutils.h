/*
 *  I2Quick plugin
 *
 *  Copyright (c) 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#ifndef I2OSUTILS_H
#define I2OSUTILS_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQuickWindow>

#include <QNetworkConfigurationManager>


#include "I2PropertyHelpers.h"

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2OSUtils class defines a set of utility functions to perform OS specific operations
 */
class I2QUICK_EXPORT I2OSUtils : public QObject
{
    Q_OBJECT

    // Current window
    I2_QML_PROPERTY_DELETE_PROOF(QQuickWindow*, currentWindow)

    // Flag indicating if our system is awake
    I2_QML_PROPERTY_READONLY(bool, isAwake)

    // Flag indicating if our user session is locked
    I2_QML_PROPERTY_READONLY(bool, isUserSessionLocked)

    // Flag indicating if our system has energy efficency features (Macos App Nap, etc.)
    I2_QML_PROPERTY_READONLY(bool, hasEnergyEfficiencyFeatures)

    // Flag indicating if we want to prevent energy efficency features (Macos App Nap, etc.)
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, preventEnergyEfficiencyFeatures)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit I2OSUtils(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~I2OSUtils();


    /**
     * @brief Get our singleton instance
     * @return
     */
    static I2OSUtils* instance();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


     /**
      * @brief Clean-up our menu
      */
     virtual void removeOSGeneratedMenuItems();


Q_SIGNALS:
     /**
      * @brief Triggered when our machine will go to sleep
      */
     void systemSleep();


     /**
      * @ brief Triggered when our machine did wake from sleep
      */
     void systemWake();


     /**
      * @brief Triggered when our user session is locked
      */
     void userSessionLocked();


     /**
      * @brief Triggered when our user session is unlocked
      */
     void userSessionUnlocked();


     /**
      * @brief Triggered when a network configuration is added, removed or changed
      */
     void systemNetworkConfigurationsUpdated();


protected:
    /**
     * @brief Called when we receive a systemSleep signal
     */
    void _onSystemSleep();


    /**
     * @brief Called when we receive a systemWake signal
     */
    void _onSystemWake();


    /**
     * @brief Called when we receive a userSessionLocked signal
     */
    void _onUserSessionLocked();


    /**
     * @brief Called when we receive a userSessionUnlocked signal
     */
    void _onUserSessionUnlocked();


    /**
     * @brief Called when a configuration is added to the system
     * @param config
     */
    void _onConfigurationAdded(const QNetworkConfiguration &config);


    /**
     * @brief Called when a configuration is about to be removed from the system
     * @param config
     */
    void _onConfigurationRemoved(const QNetworkConfiguration &config);


    /**
     * @brief Called when a configuration has changed (ex: Wifi on <=> off)
     * @param config
     */
    void _onConfigurationChanged(const QNetworkConfiguration &config);


protected:
    /**
     * @brief Enable energy efficiency features
     */
    virtual void _enableEnergyEfficiencyFeatures();


    /**
     * @brief Disable energy efficiency features
     */
    virtual void _disableEnergyEfficiencyFeatures();


protected:
    // Used to manages the network configurations provided by the system
    QNetworkConfigurationManager _networkConfigurationManager;
};

QML_DECLARE_TYPE(I2OSUtils)

#endif // I2OSUTILS_H
