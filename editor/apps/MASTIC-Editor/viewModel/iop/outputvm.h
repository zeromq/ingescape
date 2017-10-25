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

#ifndef OUTPUTVM_H
#define OUTPUTVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include <viewModel/pointmapvm.h>
#include <model/iop/outputm.h>

/**
 * @brief The OutputVM class defines a view model of output
 */
class OutputVM : public PointMapVM
{
    Q_OBJECT

    // Model of our agent Output
    I2_QML_PROPERTY_READONLY_DELETE_PROOF(OutputM*, modelM)


public:
    explicit OutputVM(QString agentName,
                      OutputM* modelM,
                      QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~OutputVM();


Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(OutputVM)

#endif // OUTPUTVM_H
