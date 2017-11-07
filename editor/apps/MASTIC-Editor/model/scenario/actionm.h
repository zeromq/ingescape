
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

#include "model/scenario/actioneffectm.h"
#include "model/scenario/actionconditionm.h"

/**
  * Types:
  * - Custom
  * - Immediate
  * - Infinite
  */
I2_ENUM(ValidationDurationType, CUSTOM, IMMEDIATE, INFINITE)


/**
 * @brief The ActionM class defines the main controller of our application
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

    // Shall revert the action
    I2_QML_PROPERTY(bool, shallRevert)

    // Shall revert action when validity is over
    I2_QML_PROPERTY(bool, shallRevertWhenValidityIsOver)

    // Shall revert after date time flag
    I2_QML_PROPERTY(bool, shallRevertAfterTime)

    // Revert after date time flag
    I2_QML_PROPERTY(int, revertAfterTimeInSec)

    // Revert after date time flag
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, revertAfterTime)

    // Flag to rearm the action
    I2_QML_PROPERTY(bool, shallRearm)

    // List of effects for the action
    I2_QOBJECT_LISTMODEL(ActionEffectM, effectsList)

    // List of conditions for the action
    I2_QOBJECT_LISTMODEL(ActionConditionM, conditionsList)

    // Index in the actions panel
    I2_QML_PROPERTY(int, actionsPanelIndex)

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


Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(ActionM)

#endif // ACTIONM_H
