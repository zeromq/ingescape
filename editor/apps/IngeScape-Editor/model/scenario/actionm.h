/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
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

#include <viewModel/scenario/actionconditionvm.h>
#include <viewModel/scenario/actioneffectvm.h>

/**
  * @brief Types of validation duration (for an action)
  */
I2_ENUM_CUSTOM(ValidationDurationTypes, IMMEDIATE, FOREVER, CUSTOM)


/**
 * @brief The ActionM class defines a model of action
 */
class ActionM: public QObject
{
    Q_OBJECT

    // Unique identifier of our action
    I2_CPP_NOSIGNAL_PROPERTY(int, uid)

    // Action name
    I2_QML_PROPERTY(QString, name)

    // Validity duration type
    I2_QML_PROPERTY(ValidationDurationTypes::Value, validityDurationType)

    // Validity duration in milliseconds
    I2_QML_PROPERTY(int, validityDuration)

    // Validity duration in milliseconds in string format
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, validityDurationString)

    // Flag indicating if we shall revert effect(s) of our action
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

    // Rearm after time in milliseconds
    I2_QML_PROPERTY(int, rearmAfterTime)

    // Rearm after time in string format
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, rearmAfterTimeString)

    // FIXME: a Model must not contain a VM
    // List of effects for the action
    I2_QOBJECT_LISTMODEL(ActionEffectVM, effectsList)

    // FIXME: a Model must not contain a VM
    // List of conditions for the action
    I2_QOBJECT_LISTMODEL(ActionConditionVM, conditionsList)

    // Flag indicating if all conditions of our action are valid
    I2_QML_PROPERTY(bool, isValid)

    // Is connected flag
    I2_QML_PROPERTY(bool, isConnected)


public:

    /**
     * @brief Constructor
     * @param uid
     * @param name
     * @param parent
     */
    explicit ActionM(int uid, QString name, QObject *parent = 0);


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


    /**
     * @brief Add effect to the list
     * @param effectVM
     */
    void addEffectToList(ActionEffectVM* effectVM);


    /**
     * @brief Add condition to the list
     * @param conditionVM
     */
    void addConditionToList(ActionConditionVM* conditionVM);


Q_SIGNALS:


public Q_SLOTS:

protected Q_SLOTS:

    /**
     * @brief Slot called when the flag "is Valid" of a condition changed
     * @param isValid
     */
    void _onIsValidConditionChanged(bool isValid);


    /**
     * @brief Triggered when an agent model associated to an effect has been destroyed from the mapping
     *        The effect does not need to exist anymore, we can delete it
     */
    void _onEffectDestructionAsked();


    /**
     * @brief Triggered when an agent model associated to a condition has been destroyed from the mapping
     *        The condition does not need to exist anymore, we can delete it
     */
    void _onConditionDestructionAsked();

protected:



};

QML_DECLARE_TYPE(ActionM)

#endif // ACTIONM_H
