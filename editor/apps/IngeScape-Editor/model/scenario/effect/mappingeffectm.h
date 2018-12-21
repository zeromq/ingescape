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

#ifndef MAPPINGEFFECTM_H
#define MAPPINGEFFECTM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include <model/scenario/effect/actioneffectm.h>


/**
 * Values of effect on mapping
 */
I2_ENUM_CUSTOM(MappingEffectValues, MAPPED, UNMAPPED)


/**
 * @brief The MappingEffectM class defines an action effect on a mapping between two iop value
 */
class MappingEffectM : public ActionEffectM
{
    Q_OBJECT

    // Value of our effect on mapping
    I2_QML_PROPERTY(MappingEffectValues::Value, mappingEffectValue)

    // Output Agent
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentsGroupedByNameVM*, outputAgent)

    // Output of the output agent
    I2_QML_PROPERTY(AgentIOPVM*, output)

    // Input Agent is stored in the base class "ActionEffectM"

    // Input of the input agent
    I2_QML_PROPERTY(AgentIOPVM*, input)

    // List of outputs
    //I2_QOBJECT_LISTMODEL(AgentIOPVM, outputsList)
    I2_QOBJECT_LISTMODEL(OutputVM, outputsList)

    // List of inputs
    //I2_QOBJECT_LISTMODEL(AgentIOPVM, inputsList)
    I2_QOBJECT_LISTMODEL(InputVM, inputsList)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit MappingEffectM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~MappingEffectM();


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
     * @brief Called when our "output agent" is destroyed
     * @param sender
     */
    void _onOutputAgentDestroyed(QObject* sender);


    /**
     * @brief Slot called when some view models of inputs have been added to the input agent(s grouped by name)
     * @param newInputs
     */
    void _onInputsHaveBeenAddedToInputAgent(QList<InputVM*> newInputs);


    /**
     * @brief Slot called when some view models of outputs have been added to the output agent(s grouped by name)
     * @param newOutputs
     */
    void _onOutputsHaveBeenAddedToOutputAgent(QList<OutputVM*> newOutputs);


    /**
     * @brief Slot called when some view models of inputs will be removed from the input agent(s grouped by name)
     * @param oldInputs
     */
    void _onInputsWillBeRemovedToInputAgent(QList<InputVM*> oldInputs);


    /**
     * @brief Slot called when some view models of outputs will be removed from the output agent(s grouped by name)
     * @param oldOutputs
     */
    void _onOutputsWillBeRemovedToOutputAgent(QList<OutputVM*> oldOutputs);


    /**
     * @brief Called when our input is destroyed
     * @param sender
     */
    //void _onInputDestroyed(QObject* sender);


    /**
     * @brief Called when our output is destroyed
     * @param sender
     */
    //void _onOutputDestroyed(QObject* sender);

};

QML_DECLARE_TYPE(MappingEffectM)

#endif // MAPPINGEFFECTM_H
