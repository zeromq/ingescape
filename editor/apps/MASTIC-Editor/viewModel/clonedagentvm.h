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

#ifndef CLONEDAGENTVM_H
#define CLONEDAGENTVM_H

#include <viewModel/agentvm.h>

/**
 * @brief The ClonedAgentVM class defines a view model of "Cloned" agent
 * Allows to manage when several agents have exactly the same name and the same definition
 * Only Peer ID is different (and HostName can also be different)
 */
class ClonedAgentVM : public AgentVM
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY(QString, name)

    // List of models of agents
    I2_QOBJECT_LISTMODEL(AgentM, models)


public:
    /**
     * @brief Constructor
     * @param name
     * @param parent
     */
    explicit ClonedAgentVM(QString name, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ClonedAgentVM();

Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(ClonedAgentVM)

#endif // CLONEDAGENTVM_H
