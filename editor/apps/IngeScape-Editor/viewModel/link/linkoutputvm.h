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

#ifndef LINKOUTPUTVM_H
#define LINKOUTPUTVM_H

#include <QObject>

#include <I2PropertyHelpers.h>
#include <viewModel/iop/outputvm.h>


/**
 * @brief The LinkOutputVM class defines a view model of link output
 */
class LinkOutputVM : public QObject
{
    Q_OBJECT

    // Geometry for the connector in the view
    // Position the center of the connector (Absolute coordinate)
    I2_QML_PROPERTY(QPointF, position)

    // OutputVM viewModel


public:
    explicit LinkOutputVM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LinkOutputVM();

signals:

public slots:

};

QML_DECLARE_TYPE(LinkOutputVM)

#endif // LINKOUTPUTVM_H
