/*
 *	MASTIC Editor
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

#ifndef AGENTVM_H
#define AGENTVM_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/agentm.h>
#include <viewModel/iop/agentiopvm.h>

/**
  * Types:
  * - Agent Input
  * - Agent Output
  * - Agent Parameter
  */
I2_ENUM(AgentStatus, ON, OFF, ON_ASKED, OFF_ASKED)


/**
 * @brief The AgentVM class defines a view model of agent
 */
class AgentVM : public QObject
{
    Q_OBJECT

    // Model of our agent
    I2_QML_PROPERTY(AgentM*, modelM)

    // List of VM of inputsEnum
    I2_QOBJECT_LISTMODEL(AgentIOPVM, inputsList)

    // List of VM of outputs
    I2_QOBJECT_LISTMODEL(AgentIOPVM, outputsList)

    // List of VM of parameters
    I2_QOBJECT_LISTMODEL(AgentIOPVM, parametersList)

    // State string defined by the agent
    I2_QML_PROPERTY(QString, state)

    // Status: enum with ON demandé, ON effectif, OFF demandé, OFF effectif
    I2_QML_PROPERTY(AgentStatus::Value, status)

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY(bool, isMuted)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY(bool, isFrozen)

    // List of identical agents VM
    I2_QOBJECT_LISTMODEL(AgentVM, listSimilarAgentsVM)

    // List of similar agents VM
    I2_QOBJECT_LISTMODEL(AgentVM, listIdenticalAgentsVM)


public:
    explicit AgentVM(AgentM* model, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentVM();

signals:

public slots:
};

QML_DECLARE_TYPE(AgentVM)

#endif // AGENTVM_H
