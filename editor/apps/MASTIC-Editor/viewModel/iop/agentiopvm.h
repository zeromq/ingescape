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

#ifndef AGENTIOPVM_H
#define AGENTIOPVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include <model/iop/agentiopm.h>

/**
 * @brief The AgentIOPVM is the base class for derived class of View Model of agent Input / Output / Parameter
 */
class AgentIOPVM : public QObject
{
    Q_OBJECT

    // Name of our agent Input / Output / Parameter
    I2_QML_PROPERTY_READONLY(QString, name)

    // Identifier with name and value type (can be empty when ghost)
    I2_CPP_NOSIGNAL_PROPERTY(QString, id)

    // First model of our agent Input / Output / Parameter
    //I2_QML_PROPERTY_READONLY_DELETE_PROOF(AgentIOPM*, firstModel)

    // Models of our agent Input / Output / Parameter
    //I2_QOBJECT_LISTMODEL(AgentIOPM, models)


public:
    /**
     * @brief Constructor
     * @param name
     * @param id
     * @param parent
     */
    explicit AgentIOPVM(QString name,
                        QString id = "",
                        QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentIOPVM();


};

QML_DECLARE_TYPE(AgentIOPVM)

#endif // AGENTIOPVM_H
