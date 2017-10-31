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
 * @brief The PointMapVM class TODO ESTIA
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

};

QML_DECLARE_TYPE(PointMapVM)

#endif // POINTMAPVM_H
