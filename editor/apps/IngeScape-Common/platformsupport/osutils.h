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
#include <QTranslator>

#include "I2PropertyHelpers.h"

/**
 * @brief The OSUtils class defines a set of utility functions to perform OS specific operations
 */
class OSUtils : public QObject
{
    Q_OBJECT

    // Flag indicating if our system is awake
    I2_QML_PROPERTY_READONLY(bool, isAwake)


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
      * @brief Init
      */
     void init();


     /**
      * @brief Clean-up our menu
      */
     void removeOSGeneratedMenuItems();


Q_SIGNALS:
     /**
      * @brief Triggered when our machine will go to sleep
      */
     void systemSleep();


     /**
      * @ brief Triggered when our machine did wake from sleep
      */
     void systemWake();


protected:
    /**
     * @brief Triggered when we receive a systemSleep signal
     */
    void _onSystemSleep();


    /**
     * @brief Triggered when we receive a systemWake signal
     */
    void _onSystemWake();


protected:
     /**
      * @brief Clean-up
      */
     void _clean();


     /**
      * @brief Called when our preventEnergyEfficiencyFeatures flag has changed
      *
      * @param value
      */
     void _osPreventEnergyEfficiencyFeatures(bool value);
};

QML_DECLARE_TYPE(OSUtils)

#endif // OSUTILS_H
