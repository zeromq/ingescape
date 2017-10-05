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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#ifndef LINKVM_H
#define LINKVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/agentmappingm.h>

#include <viewModel/iop/agentiopvm.h>



/**
 * @brief The LinkVM class defines the view model of a link between two agents
 */
class LinkVM : public QObject
{
    Q_OBJECT


    // View model of the output slot  associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(AgentIOPVM*, outputSlotVM)

    // View model of the input slot  associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(AgentIOPVM*, inputSlotVM)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit LinkVM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LinkVM();

Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(LinkVM)

#endif // LINKVM_H
