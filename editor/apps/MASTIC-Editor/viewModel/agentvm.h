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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#ifndef AGENTVM_H
#define AGENTVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/agentm.h>
#include <viewModel/iop/agentiopvm.h>



/**
 * @brief The AgentVM class defines a view model of agent
 */
class AgentVM : public QObject
{
    Q_OBJECT

    // Model of our agent
    I2_QML_PROPERTY(AgentM*, modelM)

    // List of VM of inputs
    I2_QOBJECT_LISTMODEL(AgentIOPVM, inputsList)

    // List of VM of outputs
    I2_QOBJECT_LISTMODEL(AgentIOPVM, outputsList)

    // List of VM of parameters
    I2_QOBJECT_LISTMODEL(AgentIOPVM, parametersList)

    // Abscissa of our view
    I2_QML_PROPERTY(qreal, x)

    // Ordinate of our view
    I2_QML_PROPERTY(qreal, y)

    // State: enum with ON demandé, ON effectif, OFF demandé, OFF effectif

    // Status ?

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY(bool, isMuted)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY(bool, isFrozen)


public:
    /**
     * @brief Default constructor
     * @param model
     * @param parent
     */
    explicit AgentVM(AgentM* model, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentVM();

Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(AgentVM)

#endif // AGENTVM_H
