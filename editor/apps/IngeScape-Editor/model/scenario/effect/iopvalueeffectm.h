/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef IOPVALUEEFFECTM_H
#define IOPVALUEEFFECTM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include <model/scenario/effect/actioneffectm.h>


/**
 * @brief The IOPValueEffectM class defines an action effect on an IOP value
 */
class IOPValueEffectM : public ActionEffectM
{
    Q_OBJECT

    // View model of agent Input/Output/Parameter
    //I2_QML_PROPERTY_CUSTOM_SETTER(AgentIOPVM*, agentIOP)
    I2_QML_PROPERTY(AgentIOPVM*, agentIOP)

    // Value (in string format)
    I2_QML_PROPERTY(QString, value)

    // Merged list of Inputs/Outputs/Parameters of the agent
    I2_QOBJECT_LISTMODEL(AgentIOPVM, iopMergedList)

    // Number of Inputs/Outputs/Parameters
    I2_QML_PROPERTY_READONLY(int, inputsNumber)
    I2_QML_PROPERTY_READONLY(int, outputsNumber)
    I2_QML_PROPERTY_READONLY(int, parametersNumber)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit IOPValueEffectM(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~IOPValueEffectM();


    /**
      * @brief Redefinition of action effect copy
      */
    void copyFrom(ActionEffectM* effect) Q_DECL_OVERRIDE;


    /**
    * @brief Setter for property "Agent"
    * @param agent
    */
    void setagent(AgentsGroupedByNameVM* agent) Q_DECL_OVERRIDE;


    /**
     * @brief Get a pair with the agent and the command (with parameters) of our effect
     * @return
     */
    QPair<AgentsGroupedByNameVM*, QStringList> getAgentAndCommandWithParameters() Q_DECL_OVERRIDE;


    /**
     * @brief Get a pair with the agent name and the reverse command (with parameters) of our effect
     * @return
     */
    QPair<QString, QStringList> getAgentNameAndReverseCommandWithParameters() Q_DECL_OVERRIDE;


Q_SIGNALS:


protected Q_SLOTS:

    /**
     * @brief Slot called when our agent IOP is destroyed
     * @param sender
     */
    //void _onAgentIOPDestroyed(QObject* sender);


    /**
     * @brief Slot called when some view models of inputs have been added to the agent(s grouped by name)
     * @param newInputs
     */
    void _onInputsHaveBeenAdded(QList<InputVM*> newInputs);


    /**
     * @brief Slot called when some view models of outputs have been added to the agent(s grouped by name)
     * @param newOutputs
     */
    void _onOutputsHaveBeenAdded(QList<OutputVM*> newOutputs);


    /**
     * @brief Slot called when some view models of parameters have been added to our agent(s grouped by name)
     * @param newParameters
     */
    void _onParametersHaveBeenAdded(QList<ParameterVM*> newParameters);


    /**
     * @brief Slot called when some view models of inputs will be removed from the agent(s grouped by name)
     * @param oldInputs
     */
    void _onInputsWillBeRemoved(QList<InputVM*> oldInputs);


    /**
     * @brief Slot called when some view models of outputs will be removed from the agent(s grouped by name)
     * @param oldOutputs
     */
    void _onOutputsWillBeRemoved(QList<OutputVM*> oldOutputs);


    /**
     * @brief Slot called when some view models of parameters will be removed from our agent(s grouped by name)
     * @param oldParameters
     */
    void _onParametersWillBeRemoved(QList<ParameterVM*> oldParameters);

};

QML_DECLARE_TYPE(IOPValueEffectM)

#endif // IOPVALUEEFFECTM_H
