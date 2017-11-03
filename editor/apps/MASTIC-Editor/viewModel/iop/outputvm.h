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

    // Point map name (AgentIOP name) but needed for ghost input/output
    I2_QML_PROPERTY_READONLY(bool, isGhost)

    // Flag indicating if a new value is published on our output
    I2_QML_PROPERTY_READONLY(bool, isPublishedNewValue)


public:
    explicit OutputVM(QString outputName,
                      OutputM* modelM = NULL,
                      QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~OutputVM();


Q_SIGNALS:

public Q_SLOTS:

    /**
     * @brief Return true if our output can link with the input (types are compatible)
     * @param pointMap
     * @return
     */
    bool canLinkWith(PointMapVM* pointMap) Q_DECL_OVERRIDE;

};

QML_DECLARE_TYPE(OutputVM)

#endif // OUTPUTVM_H
