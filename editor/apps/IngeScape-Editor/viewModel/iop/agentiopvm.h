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
    I2_QML_PROPERTY_READONLY(AgentIOPM*, firstModel)

    // Models of our agent Input / Output / Parameter
    I2_QOBJECT_LISTMODEL(AgentIOPM, models)

    // Flag indicating if our input/output is defined in all definitions
    I2_QML_PROPERTY_READONLY(bool, isDefinedInAllDefinitions)


public:

    /**
     * @brief Constructor
     * @param name
     * @param id
     * @param modelM
     * @param parent
     */
    explicit AgentIOPVM(QString name,
                        QString id,
                        AgentIOPM* modelM,
                        QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentIOPVM();


Q_SIGNALS:


public Q_SLOTS:


private Q_SLOTS:
    /**
     * @brief Slot called when the list of models changed
     */
    void _onModelsChanged();


private:


};

QML_DECLARE_TYPE(AgentIOPVM)

#endif // AGENTIOPVM_H
