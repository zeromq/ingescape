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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#ifndef OUTPUTVM_H
#define OUTPUTVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/iop/outputm.h>
#include <viewModel/iop/agentiopvm.h>


/**
 * @brief The OutputVM class defines a view model of output
 */
class OutputVM : public AgentIOPVM
{
    Q_OBJECT

    // First model of our agent Output
    //I2_QML_PROPERTY_READONLY_DELETE_PROOF(OutputM*, firstModel)

    // Models of our agent output
    //I2_QOBJECT_LISTMODEL(OutputM, models)


public:
    /**
     * @brief Constructor
     * @param outputName
     * @param outputId
     * @param modelM
     * @param parent
     */
    explicit OutputVM(QString outputName,
                      QString outputId,
                      OutputM* modelM,
                      QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~OutputVM();


Q_SIGNALS:


public Q_SLOTS:

};

QML_DECLARE_TYPE(OutputVM)

#endif // OUTPUTVM_H
