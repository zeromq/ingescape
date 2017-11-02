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

#ifndef POINTMAPVM_H
#define POINTMAPVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include "model/iop/agentiopm.h"

/**
 * @brief The PointMapVM class is the base class derivated to create InputVM and OutputVM.
 */
class PointMapVM : public QObject
{
    Q_OBJECT

    // Name of this input/output represent by a point map
    I2_QML_PROPERTY(QString, iopName)

    // Geometry for the connector in the view
    // Position the center of the connector (Absolute coordinate)
    I2_QML_PROPERTY(QPointF, position)


public:
    /**
     * @brief Default constructor
     * @param iopName
     * @param parent
     */
    explicit PointMapVM(QString iopName, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~PointMapVM();


Q_SIGNALS:

public Q_SLOTS:

    /**
     * @brief Return true if our point can link with the point (types are compatible)
     * @param pointMap
     * @return
     */
    virtual bool canLinkWith(PointMapVM* pointMap) = 0;


protected:
    /**
     * @brief Return true if the type of the output is compatible with the type of the input
     * @param outputValueType
     * @param inputValueType
     * @return
     */
    bool _canLinkOutputToInput(AgentIOPValueTypes::Value outputValueType, AgentIOPValueTypes::Value inputValueType);

};

QML_DECLARE_TYPE(PointMapVM)

#endif // POINTMAPVM_H
