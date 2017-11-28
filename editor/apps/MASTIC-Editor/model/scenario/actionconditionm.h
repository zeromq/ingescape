
/*
 *	ActionConditionM
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

#ifndef ACTIONCONDITIONM_H
#define ACTIONCONDITIONM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "viewModel/agentinmappingvm.h"

/**
  * Comparison type for an action: SUPERIOR_TO, INFERIOR_TO, DIFFER_TO, ON, OFF
  */
I2_ENUM_CUSTOM(ActionComparisonValueType, EQUAL_TO, SUPERIOR_TO, INFERIOR_TO, ON, OFF)


/**
 * @brief The ActionConditionM class defines an action condition model
 */
class ActionConditionM: public QObject
{
    Q_OBJECT

    // Agent model
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentInMappingVM*, agent)

    // Effect type
    I2_QML_PROPERTY(ActionComparisonValueType::Value, comparison)

    // Flag indicating if our condition is valid
    I2_QML_PROPERTY(bool, isValid)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionConditionM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    virtual ~ActionConditionM();

    /**
    * @brief Copy from another condition model
    * @param condition to copy
    */
    void copyFrom(ActionConditionM* condition);

    /**
      * @brief Initialize the agent connections for the action condition
      */
    virtual void initializeConnections();

    /**
      * @brief Reset the agent connections for the action condition
      */
    virtual void resetConnections();

Q_SIGNALS:


public Q_SLOTS:

    /**
      * @brief Slot on IsON flag agent change
      */
    virtual void onAgentModelIsOnChange(bool isON);

protected Q_SLOTS:

    /**
     * @brief Called when our agent is destroyed
     * @param sender
     */
    void _onAgentDestroyed(QObject* sender);

protected:



};

QML_DECLARE_TYPE(ActionConditionM)

#endif // ACTIONCONDITIONM_H
