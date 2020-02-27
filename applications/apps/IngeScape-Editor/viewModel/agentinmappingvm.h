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

#ifndef AGENTINMAPPINGVM_H
#define AGENTINMAPPINGVM_H

#include <QObject>
#include <QtQml>
#include <I2PropertyHelpers.h>
#include <viewModel/mapping/objectinmappingvm.h>
#include <viewModel/agentsgroupedbynamevm.h>
#include <viewModel/link/linkinputvm.h>
#include <viewModel/link/linkoutputvm.h>


/**
 * @brief The AgentInMappingVM class defines a view model of agent in the global mapping
 * Allows to manage agents and links graphically
 */
class AgentInMappingVM : public ObjectInMappingVM
{
    Q_OBJECT

    // Agents grouped by name
    I2_QML_PROPERTY_READONLY(AgentsGroupedByNameVM*, agentsGroupedByName)

    // List of view models of link inputs
    I2_QOBJECT_LISTMODEL(LinkInputVM, linkInputsList)

    // List of view models of link outputs
    I2_QOBJECT_LISTMODEL(LinkOutputVM, linkOutputsList)

    // Flag indicating if our agent is locked reduced (prevent to open the list of Inputs/Outputs)
    I2_QML_PROPERTY_READONLY(bool, isLockedReduced)

    // Group of value type of the reduced link inputs of our agent (= brin)
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypeGroups::Value, reducedLinkInputsValueTypeGroup)

    // Group of value type of the reduced link outputs of our agent (= brin)
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypeGroups::Value, reducedLinkOutputsValueTypeGroup)

    I2_QML_PROPERTY_READONLY(bool, hadLinksAdded_WhileAgentWasOFF)
    I2_QML_PROPERTY_READONLY(bool, hadLinksRemoved_WhileAgentWasOFF)

    // Mapping currently edited (temporary until the user activate the mapping)
    //I2_CPP_NOSIGNAL_PROPERTY(AgentMappingM*, temporaryMapping)


public:
    /**
     * @brief Constructor
     * @param agentsGroupedByName View model of agents grouped by the same name
     * @param position Position of the top left corner
     * @param parent
     */
    explicit AgentInMappingVM(AgentsGroupedByNameVM* agentsGroupedByName,
                              QPointF position, qreal width,
                              QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentInMappingVM();


    /**
     * @brief Default width of the item when created
     */
    static const qreal DEFAULT_WIDTH;


    /**
     * @brief Return the list of view models of link input from an input name
     * @param inputName
     */
    QList<LinkInputVM*> getLinkInputsListFromName(QString inputName);


    /**
     * @brief Return the view model of link input from an input id
     * @param inputId
     */
    LinkInputVM* getLinkInputFromId(QString inputId);


    /**
     * @brief Return the list of view models of link output from an output name
     * @param outputName
     */
    QList<LinkOutputVM*> getLinkOutputsListFromName(QString outputName);


    /**
     * @brief Return the view model of link output from an output id
     * @param outputId
     */
    LinkOutputVM* getLinkOutputFromId(QString outputId);


    /**
     * @brief Add a link while the agent is OFF
     */
    void addLink_WhileAgentIsOFF(QString linkId, QString inputName, QString outputAgentName, QString outputName);


    /**
     * @brief Cancel the add of the link while the agent is OFF
     */
    void cancelAddLink_WhileAgentIsOFF(QString linkId);


    /**
     * @brief Cancel all added links while the agent was OFF
     */
    void cancelAllAddedLinks_WhileAgentWasOFF();


    /**
     * @brief Get the model of added mapping element (while the agent was OFF) from a link id
     */
    MappingElementM* getAddedMappingElementFromLinkId_WhileAgentWasOFF(QString linkId);


    /**
     * @brief Get the list of all added link Ids while the agent was OFF
     */
    QList<QString> getAddedLinkIds_WhileAgentWasOFF();


    /**
     * @brief Get the list of all (models of) added mapping elements while the agent was OFF
     */
    QList<MappingElementM*> getAddedMappingElements_WhileAgentWasOFF();


    /**
     * @brief Remove a link while the agent was OFF
     */
    void removeLink_WhileAgentIsOFF(QString linkId, MappingElementVM* mappingElement);


    /**
     * @brief Cancel the remove of the link while the agent was OFF
     */
    void cancelRemoveLink_WhileAgentWasOFF(QString linkId);


    /**
     * @brief Cancel all removed links while the agent was OFF
     */
    void cancelAllRemovedLinks_WhileAgentWasOFF();


    /**
     * @brief Get the view model of removed mapping element (while the agent was OFF) from a link id
     */
    MappingElementVM* getRemovedMappingElementFromLinkId_WhileAgentWasOFF(QString linkId);


    /**
     * @brief Get the list of all (view models of) removed mapping elements while the agent was OFF
     * @return
     */
    QList<MappingElementVM*> getRemovedMappingElements_WhileAgentWasOFF();


    /**
     * @brief Get the list of all names of removed mapping elements while the agent was OFF
     */
    QStringList getNamesOfRemovedMappingElements_WhileAgentWasOFF();


    /**
     * @brief A mapping element has just been removed and correspond to a link removed while the agent was off
     */
    void mappingElementRemoved_CorrespondingLinkRemovedWhileAgentWasOFF(MappingElementVM* mappingElement);


Q_SIGNALS:

    /**
     * @brief Signal emitted when some view models of link inputs have been added to our agent in mapping
     * @param addedlinkInputs
     */
    void linkInputsListHaveBeenAdded(QList<LinkInputVM*> addedlinkInputs);


    /**
     * @brief Signal emitted when some view models of link outputs have been added to our agent in mapping
     * @param addedlinkOutputs
     */
    void linkOutputsListHaveBeenAdded(QList<LinkOutputVM*> addedlinkOutputs);


    /**
     * @brief Signal emitted when some view models of link inputs will be removed from our agent in mapping
     * @param removedLinkInputs
     */
    void linkInputsListWillBeRemoved(QList<LinkInputVM*> removedLinkInputs);


    /**
     * @brief Signal emitted when some view models of link outputs will be removed from our agent in mapping
     * @param removedLinkOutputs
     */
    void linkOutputsListWillBeRemoved(QList<LinkOutputVM*> removedLinkOutputs);


private Q_SLOTS:

    /**
     * @brief Slot called when some view models of inputs have been added to the agent(s grouped by name)
     * @param newInputs
     */
    void _onInputsHaveBeenAdded(QList<InputVM*> newInputs);


    /**
     * @brief Slot called when some view models of outputs have been added to the agent(s grouped by name)
     * @param newOutputs
     */
    void _onOutputsHaveBeenAdded(QList<OutputVM*> newOutputs);


    /**
     * @brief Slot called when some view models of inputs will be removed from the agent(s grouped by name)
     * @param oldInputs
     */
    void _onInputsWillBeRemoved(QList<InputVM*> oldInputs);


    /**
     * @brief Slot called when some view models of outputs will be removed from the agent(s grouped by name)
     * @param oldOutputs
     */
    void _onOutputsWillBeRemoved(QList<OutputVM*> oldOutputs);


private:

    /**
     * @brief Update the group (of value type) of the reduced link inputs of our agent (= brin)
     */
    void _updateReducedLinkInputsValueTypeGroup();


    /**
     * @brief Update the group (of value type) of the reduced link outputs of our agent (= brin)
     */
    void _updateReducedLinkOutputsValueTypeGroup();


    /**
     * @brief Check the maximum number of Inputs/Outputs
     */
    void _checkMaxNumberOfIO();


private:

    // Input name as key is not unique (value type can be different)
    // Hash table from an input name to a list of view models of link inputs
    QHash<QString, QList<LinkInputVM*>> _hashFromNameToLinkInputsList;

    // Hash table from a (unique) input id to a view model of link input
    QHash<QString, LinkInputVM*> _hashFromIdToLinkInput;

    // Output name as key is not unique (value type can be different)
    // Hash table from an output name to a list of view models of link outputs
    QHash<QString, QList<LinkOutputVM*>> _hashFromNameToLinkOutputsList;

    // Hash table from a (unique) output id to a view model of link output
    QHash<QString, LinkOutputVM*> _hashFromIdToLinkOutput;

    // Hash table from "(unique) link id" to added link (model of mapping element) while the agent was OFF
    QHash<QString, MappingElementM*> _hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF;

    // Hash table from "(unique) link id" to removed link (view model of mapping element) while the agent was OFF
    QHash<QString, MappingElementVM*> _hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF;

};

QML_DECLARE_TYPE(AgentInMappingVM)

#endif // AGENTINMAPPINGVM_H
