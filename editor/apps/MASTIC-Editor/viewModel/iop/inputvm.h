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

#ifndef INPUTVM_H
#define INPUTVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include <viewModel/pointmapvm.h>
#include <model/iop/agentiopm.h>

/**
 * @brief The InputVM class defines a view model of input
 */
class InputVM : public PointMapVM
{
    Q_OBJECT

    // Model of our agent Input
    I2_QML_PROPERTY_READONLY_DELETE_PROOF(AgentIOPM*, modelM)

public:
    explicit InputVM(QString agentName,
                     AgentIOPM* modelM,
                     QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~InputVM();


Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(InputVM)

#endif // INPUTVM_H
