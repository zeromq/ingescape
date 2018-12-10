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
#include <viewModel/agentinmappingvm.h>
#include <viewModel/link/linkinputvm.h>
#include <viewModel/link/linkoutputvm.h>


/**
 * @brief The LinkVM class defines a view model of link between two agents in the global mapping
 */
class LinkVM : public QObject
{
    Q_OBJECT

    // Name with all names formatted: "outputAgent##output-->inputAgent##input"
    I2_CPP_NOSIGNAL_PROPERTY(QString, name)

    // Unique identifier with agents names and Input/Output ids: "outputAgent##output::outputType-->inputAgent##input::inputType"
    I2_CPP_NOSIGNAL_PROPERTY(QString, uid)

    // View model of the corresponding mapping element
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(MappingElementVM*, mappingElement)

    // View model of the output agent of our link (link starts from this agent)
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, outputAgent)

    // View model of the output slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(LinkOutputVM*, linkOutput)

    // View model of the input agent of our link (link ends to this agent)
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, inputAgent)

    // View model of the input slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(LinkInputVM*, linkInput)

    // Flag indicating if our link is displayed with a dashed line (during the request to the input agent to link/un-link)
    I2_QML_PROPERTY_READONLY(bool, isDashedLine)


public:

    /**
     * @brief Constructor
     * @param name
     * @param mappingElement corresponding mapping element
     * @param outputAgent The link starts from this agent
     * @param linkOutput The link starts from this output of the output agent
     * @param inputAgent The link ends to this agent
     * @param linkInput The link ends to this input of the input agent
     * @param isDashedLine
     * @param parent
     */
    explicit LinkVM(QString name,
                    MappingElementVM* mappingElement,
                    AgentInMappingVM* outputAgent,
                    LinkOutputVM* linkOutput,
                    AgentInMappingVM* inputAgent,
                    LinkInputVM* linkInput,
                    bool isDashedLine,
                    QObject *parent = nullptr);



    /**
     * @brief Destructor
     */
    ~LinkVM();


    /**
     * @brief Get the link id (with format "outputAgent##output::outputType-->inputAgent##input::inputType") from agent names and Input/Output ids
     * @param outputAgent
     * @param output
     * @param inputAgent
     * @param input
     * @return link id with format "outputAgent##output::outputType-->inputAgent##input::inputType"
     */
    static QString getLinkIdFromAgentNamesAndIOids(QString outputAgent, QString outputId, QString inputAgent, QString inputId);

};

QML_DECLARE_TYPE(LinkVM)

#endif // LINKVM_H
