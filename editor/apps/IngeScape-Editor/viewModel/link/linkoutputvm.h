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
#include <viewModel/link/linkconnectorvm.h>
#include <viewModel/iop/outputvm.h>


/**
 * @brief The LinkOutputVM class defines a view model of link output
 */
class LinkOutputVM : public LinkConnectorVM
{
    Q_OBJECT

    // View model of output
    I2_QML_PROPERTY_READONLY(OutputVM*, output)

    // FIXME: rather dans LinkOutputVM than in OutputVM
    // Flag indicating if a new value is published on our output
    //I2_QML_PROPERTY_READONLY(bool, isPublishedNewValue)


public:
    /**
     * @brief Constructor
     * @param output
     * @param parent
     */
    explicit LinkOutputVM(OutputVM* output,
                          QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LinkOutputVM();


Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Return true if our output can link with the input (types are compatible)
     * @param linkConnector
     * @return
     */
    bool canLinkWith(LinkConnectorVM* linkConnector) Q_DECL_OVERRIDE;

};

QML_DECLARE_TYPE(LinkOutputVM)

#endif // LINKOUTPUTVM_H
