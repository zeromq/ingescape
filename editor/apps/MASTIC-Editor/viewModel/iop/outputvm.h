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

    // Point map name (AgentIOP name) but needed for ghost input/output
    I2_QML_PROPERTY(bool, isGhost)

    // Model of our agent Output
    I2_QML_PROPERTY_READONLY_DELETE_PROOF(OutputM*, modelM)


public:
    explicit OutputVM(QString outputName,
                      OutputM* modelM,
                      QObject *parent = nullptr);

    explicit OutputVM(QString outputName,
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
