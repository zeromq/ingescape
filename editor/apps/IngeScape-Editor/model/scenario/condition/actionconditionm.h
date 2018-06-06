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

#ifndef ACTIONCONDITIONM_H
#define ACTIONCONDITIONM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "viewModel/agentinmappingvm.h"


/**
 * @brief The ActionConditionM class defines an action condition model
 */
class ActionConditionM: public QObject
{
    Q_OBJECT

    // Agent
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentInMappingVM*, agent)

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
    virtual void copyFrom(ActionConditionM* condition);


    /**
      * @brief Initialize the agent connections for the action condition
      */
    virtual void initializeConnections();


    /**
      * @brief Reset the agent connections for the action condition
      */
    virtual void resetConnections();


Q_SIGNALS:

    /**
     * @brief Signal the parent that the action condition can be destroyed and removed from the list
     */
    void askForDestruction();


protected Q_SLOTS:

    /**
     * @brief Slot called when the flag "is ON" of an agent changed
     * @param isON
     */
    virtual void _onAgentIsOnChanged(bool isON);


    /**
     * @brief Called when our agent is destroyed
     * @param sender
     */
    void _onAgentDestroyed(QObject* sender);

};

QML_DECLARE_TYPE(ActionConditionM)

#endif // ACTIONCONDITIONM_H
