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

    // First model of our agent parameter
    I2_QML_PROPERTY_READONLY_DELETE_PROOF(AgentIOPM*, firstModel)

    // Models of our agent parameter
    I2_QOBJECT_LISTMODEL(AgentIOPM, models)


public:
    /**
     * @brief Constructor
     * @param name
     * @param id
     * @param modelM
     * @param parent
     */
    explicit ParameterVM(QString name,
                         QString id,
                         AgentIOPM* modelM,
                         QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ParameterVM();


Q_SIGNALS:


public Q_SLOTS:


private Q_SLOTS:
    /**
     * @brief Slot when the list of models changed
     */
    void _onModelsChanged();


private:


};

QML_DECLARE_TYPE(ParameterVM)

#endif // PARAMETERVM_H
