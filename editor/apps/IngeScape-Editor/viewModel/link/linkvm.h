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

#ifndef LINKVM_H
#define LINKVM_H

#include <QObject>

#include <I2PropertyHelpers.h>
#include <viewModel/mapping/mappingelementvm.h>
//#include <viewModel/agentinmappingvm.h>
#include <viewModel/mapping/objectinmappingvm.h>
#include <viewModel/link/linkinputvm.h>
#include <viewModel/link/linkoutputvm.h>


/**
 * @brief The LinkVM class defines a view model of link between two agents in the global mapping
 */
class LinkVM : public QObject
{
    Q_OBJECT

    // Name with all names formatted: "outputObject##output-->inputObject##input"
    I2_CPP_NOSIGNAL_PROPERTY(QString, name)

    // Unique identifier with objects ids and Input/Output ids: "outputObject##output::outputType-->inputObject##input::inputType"
    I2_CPP_NOSIGNAL_PROPERTY(QString, uid)

    // View model of the corresponding mapping element
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(MappingElementVM*, mappingElement)

    // View model of the output object of our link (link starts from this object)
    I2_QML_PROPERTY_DELETE_PROOF(ObjectInMappingVM*, outputObject)

    // View model of the output slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(LinkOutputVM*, linkOutput)

    // View model of the input object of our link (link ends to this object)
    I2_QML_PROPERTY_DELETE_PROOF(ObjectInMappingVM*, inputObject)

    // View model of the input slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(LinkInputVM*, linkInput)

    // Flag indicating if our link is temporary (during the request and the reply to the input agent to link/un-link)
    I2_QML_PROPERTY_READONLY(bool, isTemporary)


public:

    /**
     * @brief Constructor
     * @param name
     * @param mappingElement corresponding mapping element
     * @param outputObject The link starts from this object (in the global mapping)
     * @param linkOutput The link starts from this output of the output agent
     * @param inputObject The link ends to this object (in the global mapping)
     * @param linkInput The link ends to this input of the input agent
     * @param isTemporary
     * @param parent
     */
    explicit LinkVM(QString name,
                    MappingElementVM* mappingElement,
                    ObjectInMappingVM* outputObject,
                    LinkOutputVM* linkOutput,
                    ObjectInMappingVM* inputObject,
                    LinkInputVM* linkInput,
                    bool isTemporary,
                    QObject *parent = nullptr);



    /**
     * @brief Destructor
     */
    ~LinkVM();


    /**
     * @brief Get the link id (with format "outputObjectId##output::outputType-->inputObjectId##input::inputType") from object ids and Input/Output ids
     * @param outputObjectId
     * @param outputId
     * @param inputObjectId
     * @param inputId
     * @return link id with format "outputObjectId##output::outputType-->inputObjectId##input::inputType"
     */
    static QString getLinkIdFromObjectIdsAndIOids(QString outputObjectId, QString outputId, QString inputObjectId, QString inputId);


Q_SIGNALS:

    /**
     * @brief Signal emitted when the output has been activated, so we have to activate the input (of the input object in the global mapping)
     * @param inputObject
     * @param linkInput
     */
    void activateInputOfObjectInMapping(ObjectInMappingVM* inputObject, LinkInputVM* linkInput);


private Q_SLOTS:

    /**
     * @brief Slot called when the (link) output has been activated
     */
    void _onOutputActivated();

};

QML_DECLARE_TYPE(LinkVM)

#endif // LINKVM_H
