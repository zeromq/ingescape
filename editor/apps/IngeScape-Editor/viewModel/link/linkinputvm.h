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
#include <viewModel/link/linkconnectorvm.h>
#include <viewModel/iop/inputvm.h>


/**
 * @brief The LinkInputVM class defines a view model of link input
 */
class LinkInputVM : public LinkConnectorVM
{
    Q_OBJECT

    // View model of input
    I2_QML_PROPERTY_READONLY(InputVM*, input)


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


Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Return true if our input can link with the output (types are compatible)
     * @param linkConnector
     * @return
     */
    bool canLinkWith(LinkConnectorVM* linkConnector) Q_DECL_OVERRIDE;


};

QML_DECLARE_TYPE(LinkInputVM)

#endif // LINKINPUTVM_H
