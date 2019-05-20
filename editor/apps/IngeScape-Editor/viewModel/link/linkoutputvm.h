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

    // Flag indicating if our (link) output has been activated
    I2_QML_PROPERTY_READONLY(bool, hasBeenActivated)


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


    /**
     * @brief Activate
     */
    void activate();


Q_SIGNALS:

    /**
     * @brief Signal emitted when our (link) output has been activated
     */
    void activated();


public Q_SLOTS:

    /**
     * @brief Return true if our output can link with the input (types are compatible)
     * @param linkConnector
     * @return
     */
    bool canLinkWith(LinkConnectorVM* linkConnector) Q_DECL_OVERRIDE;


private Q_SLOTS:

    /**
     * @brief Slot called when the current value (of our output) changed
     * @param value
     */
    void _oncurrentValueChanged(QVariant value);


    /**
     * @brief Slot called when the timer times out
     */
    void _onTimeout();


private:

    // Timer to reset the flag "has been activated"
    // Allows to play an animation when the value changed
    QTimer _timer;

};

QML_DECLARE_TYPE(LinkOutputVM)

#endif // LINKOUTPUTVM_H
