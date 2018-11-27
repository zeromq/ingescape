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
  * Types of value (of an agent) comparison
  */
I2_ENUM_CUSTOM(ValueComparisonTypes, EQUAL_TO, SUPERIOR_TO, INFERIOR_TO)


/**
 * @brief The IOPValueConditionM class defines an action condition on an IOP value
 */
class IOPValueConditionM : public ActionConditionM
{
    Q_OBJECT

    // View model of agent Input/Output/Parameter
    //I2_QML_PROPERTY_CUSTOM_SETTER(AgentIOPVM*, agentIOP)
    I2_QML_PROPERTY(AgentIOPVM*, agentIOP)

    // Agent IOP name
    //I2_QML_PROPERTY(QString, agentIOPName)

    // Type of value comparison
    I2_QML_PROPERTY(ValueComparisonTypes::Value, valueComparisonType)

    // Comparison value (in string format)
    I2_QML_PROPERTY(QString, comparisonValue)

    // Merged list of Inputs/Outputs/Parameters of the agent
    I2_QOBJECT_LISTMODEL(AgentIOPVM, iopMergedList)

    // Number of Inputs/Outputs/Parameters
    I2_QML_PROPERTY_READONLY(int, inputsNumber)
    I2_QML_PROPERTY_READONLY(int, outputsNumber)
    //I2_QML_PROPERTY_READONLY(int, parametersNumber)


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
    void setagent(AgentsGroupedByNameVM* agent) Q_DECL_OVERRIDE;


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
     * @brief Slot called when the flag "is ON" of the agent changed
     * @param isON
     */
    void _onAgentIsOnChanged(bool isON) Q_DECL_OVERRIDE;


    /**
     * @brief Slot called when our agent IOP is destroyed
     * @param sender
     */
    //void _onAgentIOPDestroyed(QObject* sender);


    /**
     * @brief Slot called when some view models of outputs have been added to the agent(s grouped by name)
     * @param newOutputs
     */
    void _onOutputsHaveBeenAdded(QList<OutputVM*> newOutputs);


    /**
     * @brief Slot called when some view models of outputs will be removed from the agent(s grouped by name)
     * @param oldOutputs
     */
    void _onOutputsWillBeRemoved(QList<OutputVM*> oldOutputs);


    /**
      * @brief Slot called when the current value of our agent iop changed
      */
    void _onCurrentValueChanged(QVariant currentValue);

};

QML_DECLARE_TYPE(IOPValueConditionM)

#endif // IOPVALUECONDITIONM_H
