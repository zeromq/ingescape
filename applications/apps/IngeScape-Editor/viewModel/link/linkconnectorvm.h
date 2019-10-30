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

#ifndef LINKCONNECTORVM_H
#define LINKCONNECTORVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <viewModel/iop/agentiopvm.h>


/**
 * @brief The LinkConnectorVM is the base class for derived class of view model of link Input / Output
 */
class LinkConnectorVM : public QObject
{
    Q_OBJECT

    // Name of our agent Input / Output
    I2_QML_PROPERTY_READONLY(QString, name)

    // Unique identifier with name and value type
    I2_CPP_NOSIGNAL_PROPERTY(QString, uid)

    // Geometry for the connector in the view
    // Position the center of the connector (Absolute coordinate)
    I2_QML_PROPERTY(QPointF, position)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit LinkConnectorVM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LinkConnectorVM();


Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Return true if our connector can link with the other connector (types are compatible)
     * @param linkConnector
     * @return
     */
    virtual bool canLinkWith(LinkConnectorVM* linkConnector) = 0;


protected:
    /**
     * @brief Return true if the type of the output is compatible with the type of the input
     * @param outputValueType
     * @param inputValueType
     * @return
     */
    bool _canLinkOutputToInput(AgentIOPValueTypes::Value outputValueType, AgentIOPValueTypes::Value inputValueType);


};

QML_DECLARE_TYPE(LinkConnectorVM)

#endif // LINKCONNECTORVM_H
