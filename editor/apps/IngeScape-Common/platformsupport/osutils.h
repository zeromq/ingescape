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


#ifndef OSUTILS_H
#define OSUTILS_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQuickWindow>

#include "I2PropertyHelpers.h"



/**
 * @brief The OSUtils class defines a set of utility functions to perform OS specific operations
 */
class OSUtils : public QObject
{
    Q_OBJECT

    // Current window
    I2_QML_PROPERTY_DELETE_PROOF(QQuickWindow*, currentWindow)

    // Flag indicating if our system is awake
    I2_QML_PROPERTY_READONLY(bool, isAwake)

    // Flag indicating if our user session is locked
    I2_QML_PROPERTY_READONLY(bool, isUserSessionLocked)

    // Flag indicating if we want to prevent energy efficency features (Macos App Nap, etc.)
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, preventEnergyEfficiencyFeatures)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit OSUtils(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~OSUtils();


    /**
     * @brief Get our singleton instance
     * @return
     */
    static OSUtils* instance();


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


protected:
    /**
     * @brief Enable energy efficiency features
     */
    virtual void _enableEnergyEfficiencyFeatures();


    /**
     * @brief Disable energy efficiency features
     */
    virtual void _disableEnergyEfficiencyFeatures();
};

QML_DECLARE_TYPE(OSUtils)

#endif // OSUTILS_H
