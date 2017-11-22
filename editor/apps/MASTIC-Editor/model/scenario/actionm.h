
/*
 *	ActionM
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *
 */

#ifndef ACTIONM_H
#define ACTIONM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QColor>

#include "I2PropertyHelpers.h"

#include "viewModel/actionconditionvm.h"
#include "viewModel/actioneffectvm.h"

/**
  * @brief Validation duration type for an action
  * Types:
  * - Custom
  * - Immediate
  * - Forever
  */
I2_ENUM_CUSTOM(ValidationDurationType, IMMEDIATE, FOREVER, CUSTOM)


/**
 * @brief The ActionM class defines an action main model
 */
class ActionM: public QObject
{
    Q_OBJECT

    // Action name
    I2_QML_PROPERTY(QString, name)

    // Validity duration type
    I2_QML_PROPERTY(ValidationDurationType::Value, validityDurationType)

    // Validity duration in milliseconds
    I2_QML_PROPERTY(int, validityDuration)

    // Validity duration in milliseconds in string format
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, validityDurationString)

    // Shall revert the action
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, shallRevert)

    // Shall revert action when validity is over
    I2_QML_PROPERTY(bool, shallRevertWhenValidityIsOver)

    // Shall revert after date time flag
    I2_QML_PROPERTY(bool, shallRevertAfterTime)

    // Revert after time in milliseconds
    I2_QML_PROPERTY(int, revertAfterTime)

    // Revert after time in string format
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, revertAfterTimeString)

    // Flag to rearm the action
    I2_QML_PROPERTY(bool, shallRearm)

    // List of effects for the action
    I2_QOBJECT_LISTMODEL(ActionEffectVM, effectsList)

    // List of conditions for the action
    I2_QOBJECT_LISTMODEL(ActionConditionVM, conditionsList)

    // Is valid flag
    I2_QML_PROPERTY(bool, isValid)

    // Is connected flag
    I2_QML_PROPERTY(bool, isConnected)

    // FIXME : Liste des temps de déclenchement (1 ou plus si réarmable) >> VP

public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionM(QString name, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ActionM();

    /**
     * @brief Copy from another action model
     * @param action model to copy
     */
    void copyFrom(ActionM* actionModel);

    /**
     * @brief Initialize connections for conditions
     */
    void initializeConditionsConnections();

    /**
     * @brief Reset connections for conditions
     */
    void resetConditionsConnections();


Q_SIGNALS:


public Q_SLOTS:

protected Q_SLOTS:

    /**
     * @brief Slot on the condition validation change
     */
    void _onConditionValidationChange(bool isValid);


protected:



};

QML_DECLARE_TYPE(ActionM)

#endif // ACTIONM_H
