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

#include <viewModel/iop/agentiopvm.h>


/**
 * @brief The PointMapVM class is the base class derived to create InputVM and OutputVM.
 */
class PointMapVM : public AgentIOPVM
{
    Q_OBJECT

    // Geometry for the connector in the view
    // Position the center of the connector (Absolute coordinate)
    I2_QML_PROPERTY(QPointF, position)


public:

    /**
     * @brief Constructor
     * @param name
     * @param id
     * @param parent
     */
    explicit PointMapVM(QString name,
                        QString id,
                        QObject *parent = nullptr);


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
