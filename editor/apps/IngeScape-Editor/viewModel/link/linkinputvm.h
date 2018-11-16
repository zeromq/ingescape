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

#ifndef LINKINPUTVM_H
#define LINKINPUTVM_H

#include <QObject>

#include <I2PropertyHelpers.h>
#include <viewModel/iop/inputvm.h>


/**
 * @brief The LinkInputVM class defines a view model of link input
 */
class LinkInputVM : public QObject
{
    Q_OBJECT

    // Name of our agent Input
    I2_QML_PROPERTY_READONLY(QString, name)

    // View model of input
    I2_QML_PROPERTY_READONLY(InputVM*, input)

    // Geometry for the connector in the view
    // Position the center of the connector (Absolute coordinate)
    I2_QML_PROPERTY(QPointF, position)


public:

    /**
     * @brief Constructor
     * @param input
     * @param parent
     */
    explicit LinkInputVM(InputVM* input,
                         QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LinkInputVM();


signals:

public slots:

};

QML_DECLARE_TYPE(LinkInputVM)

#endif // LINKINPUTVM_H
