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

#ifndef IOPVALUECONDITIONM_H
#define IOPVALUECONDITIONM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "actionconditionm.h"


/**
 * @brief The IOPValueConditionM class defines an action condition on iop value
 */
class IOPValueConditionM : public ActionConditionM
{
    Q_OBJECT

    // Model of agent IOP
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentIOPM*, agentIOP)

    // Agent IOP name
    I2_QML_PROPERTY(QString, agentIOPName)

    // value in string format
    I2_QML_PROPERTY(QString, value)

    // Concatened list of iop agents items
    I2_QOBJECT_LISTMODEL(AgentIOPM, agentIopList)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit IOPValueConditionM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~IOPValueConditionM();


    /**
      * @brief Redefinition of action condition copy
      */
    void copyFrom(ActionConditionM* condition) Q_DECL_OVERRIDE;


    /**
    * @brief Setter for property "Agent"
    * @param agent
    */
    void setagent(AgentInMappingVM* agent) Q_DECL_OVERRIDE;


    /**
      * @brief Initialize the agent connections for the action condition
      */
    void initializeConnections() Q_DECL_OVERRIDE;


    /**
      * @brief Reset the agent connections for the action condition
      */
    void resetConnections() Q_DECL_OVERRIDE;


Q_SIGNALS:


protected Q_SLOTS:

    /**
      * @brief Slot called when the flag "is ON" of an agent changed
      */
    void _onAgentModelIsOnChanged(bool isON) Q_DECL_OVERRIDE;


    /**
      * @brief Slot called when the models of Inputs/Outputs/Parameters changed of the agent in mapping
      */
    void _onModelsOfIOPChanged();


    /**
     * @brief Called when our agent iop model is destroyed
     * @param sender
     */
    void _onAgentIopModelDestroyed(QObject* sender);


    /**
      * @brief Slot called when the current value of our agent iop changed
      */
    void _onCurrentValueChanged(QVariant currentValue);


private:
    /**
    * @brief Update the selected agent iop
    */
    void _updateAgentIOPSelected(AgentIOPM *newAgentIOP);

};

QML_DECLARE_TYPE(IOPValueConditionM)

#endif // IOPVALUECONDITIONM_H
