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

#ifndef PARAMETERVM_H
#define PARAMETERVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <viewModel/iop/agentiopvm.h>


/**
 * @brief The ParameterVM class defines a view model of parameter
 */
class ParameterVM : public AgentIOPVM
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param parameterName
     * @param parameterId
     * @param modelM
     * @param parent
     */
    explicit ParameterVM(QString parameterName,
                         QString parameterId,
                         AgentIOPM* modelM,
                         QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ParameterVM();


Q_SIGNALS:


public Q_SLOTS:

};

QML_DECLARE_TYPE(ParameterVM)

#endif // PARAMETERVM_H
