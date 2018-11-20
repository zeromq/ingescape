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

#ifndef INPUTVM_H
#define INPUTVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <viewModel/iop/agentiopvm.h>


/**
 * @brief The InputVM class defines a view model of input
 */
class InputVM : public AgentIOPVM
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param inputName
     * @param inputId
     * @param modelM
     * @param parent
     */
    explicit InputVM(QString inputName,
                     QString inputId,
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
