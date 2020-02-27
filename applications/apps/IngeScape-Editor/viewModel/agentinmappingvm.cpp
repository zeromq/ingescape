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

#include "agentinmappingvm.h"

// Maximum number of Inputs/Outputs to reduce an agent (by default)
#define MAX_NUMBER_OF_IO_TO_REDUCE 30

// Maximum number of Inputs/Outputs to lock reduced (prevent to open the list of Inputs/Outputs)
#define MAX_NUMBER_OF_IO_TO_LOCK_REDUCED 100


/**
 * @brief Default width of the item when created
 */
const qreal AgentInMappingVM::DEFAULT_WIDTH = 258;


/**
 * @brief Constructor
 * @param agentsGroupedByName View model of agents grouped by the same name
 * @param position Position of the top left corner
 * @param parent
 */
AgentInMappingVM::AgentInMappingVM(AgentsGroupedByNameVM* agentsGroupedByName,
                                   QPointF position,
                                   qreal width,
                                   QObject *parent) : ObjectInMappingVM(ObjectInMappingTypes::AGENT,
                                                                        position,
                                                                        width,
                                                                        parent),
    _agentsGroupedByName(agentsGroupedByName),
    _isLockedReduced(false),
    _reducedLinkInputsValueTypeGroup(AgentIOPValueTypeGroups::MIXED),
    _reducedLinkOutputsValueTypeGroup(AgentIOPValueTypeGroups::MIXED),
    _hadLinksAdded_WhileAgentWasOFF(false),
    _hadLinksRemoved_WhileAgentWasOFF(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_agentsGroupedByName != nullptr)
    {
        _name = _agentsGroupedByName->name();

        // Name of an agent in the mapping is unique
        _uid = _name;

        qInfo() << "New Agent" << _name << "in the global mapping";

        // Connect to signals from the agents grouped by name
        connect(_agentsGroupedByName, &AgentsGroupedByNameVM::inputsHaveBeenAdded, this, &AgentInMappingVM::_onInputsHaveBeenAdded);
        connect(_agentsGroupedByName, &AgentsGroupedByNameVM::outputsHaveBeenAdded, this, &AgentInMappingVM::_onOutputsHaveBeenAdded);
        connect(_agentsGroupedByName, &AgentsGroupedByNameVM::inputsWillBeRemoved, this, &AgentInMappingVM::_onInputsWillBeRemoved);
        connect(_agentsGroupedByName, &AgentsGroupedByNameVM::outputsWillBeRemoved, this, &AgentInMappingVM::_onOutputsWillBeRemoved);


        // FIXME TO DELETE
        /*// Create the mapping currently edited
        QString mappingName = QString("Mapping name of %1 in IngeScape Editor").arg(_name);
        QString mappingDescription = QString("Mapping description of %1 in IngeScape Editor").arg(_name);
        _temporaryMapping = new AgentMappingM(mappingName, "0.0", mappingDescription);*/


        if (!_agentsGroupedByName->inputsList()->isEmpty())
        {
            // Some view models of inputs have been added to the agent(s grouped by name)
            _onInputsHaveBeenAdded(_agentsGroupedByName->inputsList()->toList());
        }

        if (!_agentsGroupedByName->outputsList()->isEmpty())
        {
            // Some view models of outputs have been added to the agent(s grouped by name)
            _onOutputsHaveBeenAdded(_agentsGroupedByName->outputsList()->toList());
        }

        // If there are a lot of Inputs/Outputs, reduce the agent (by default)
        if (qMax(_linkInputsList.count(), _linkOutputsList.count()) > MAX_NUMBER_OF_IO_TO_REDUCE)
        {
            _isReduced = true;
        }
    }
}


/**
 * @brief Destructor
 */
AgentInMappingVM::~AgentInMappingVM()
{
    qInfo() << "Delete Agent" << _name << "in the global mapping";

    // Clear hash tables of Inputs, Outputs and Parameters
    _hashFromNameToLinkInputsList.clear();
    _hashFromIdToLinkInput.clear();
    _hashFromNameToLinkOutputsList.clear();
    _hashFromIdToLinkOutput.clear();

    // Delete all view models of link Inputs/Outputs
    _linkInputsList.deleteAllItems();
    _linkOutputsList.deleteAllItems();

    if (_agentsGroupedByName != nullptr)
    {
        // DIS-connect to signals from the agents grouped by name
        disconnect(_agentsGroupedByName, nullptr, this, nullptr);

        // Deleted elsewhere
        setagentsGroupedByName(nullptr);
    }

    /*// Delete the temporary mapping
    if (_temporaryMapping != nullptr)
    {
        AgentMappingM* temp = _temporaryMapping;
        settemporaryMapping(nullptr);
        delete temp;
    }*/

    // Free memory
    cancelAllAddedLinks_WhileAgentWasOFF();
    cancelAllRemovedLinks_WhileAgentWasOFF();
}


/**
 * @brief Return the list of view models of link input from an input name
 * @param inputName
 */
QList<LinkInputVM*> AgentInMappingVM::getLinkInputsListFromName(QString inputName)
{
    return _hashFromNameToLinkInputsList.value(inputName, QList<LinkInputVM*>());
}


/**
 * @brief Return the view model of link input from an input id
 * @param inputId
 */
LinkInputVM* AgentInMappingVM::getLinkInputFromId(QString inputId)
{
    return _hashFromIdToLinkInput.value(inputId, nullptr);
}


/**
 * @brief Return the list of view models of link output from an output name
 * @param outputName
 */
QList<LinkOutputVM*> AgentInMappingVM::getLinkOutputsListFromName(QString outputName)
{
    return _hashFromNameToLinkOutputsList.value(outputName, QList<LinkOutputVM*>());
}


/**
 * @brief Return the view model of link output from an output id
 * @param outputId
 */
LinkOutputVM* AgentInMappingVM::getLinkOutputFromId(QString outputId)
{
    return _hashFromIdToLinkOutput.value(outputId, nullptr);
}


/**
 * @brief Add a link while the agent is OFF
 */
void AgentInMappingVM::addLink_WhileAgentIsOFF(QString linkId, QString inputName, QString outputAgentName, QString outputName)
{
    qDebug() << "Add Link while agent is OFF " << linkId;
    if (!_hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.contains(linkId))
    {
        // Create a model of mapping element with agent names and with input/output names
        MappingElementM* mappingElement = new MappingElementM(_name, inputName, outputAgentName, outputName);
        _hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.insert(linkId, mappingElement);
        sethadLinksAdded_WhileAgentWasOFF(true);
    }
}


/**
 * @brief Cancel the add of the link while the agent is OFF
 */
void AgentInMappingVM::cancelAddLink_WhileAgentIsOFF(QString linkId)
{
    qDebug() << "cancelAddLink_WhileAgentIsOFF" << linkId;

    MappingElementM* mappingElement = getAddedMappingElementFromLinkId_WhileAgentWasOFF(linkId);
    if (mappingElement != nullptr)
    {
        _hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.remove(linkId);
        delete mappingElement; // Free memory
        if (_hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.isEmpty())
        {
            sethadLinksAdded_WhileAgentWasOFF(false);
        }
    }
}


/**
 * @brief Cancel all added links while the agent was OFF
 */
void AgentInMappingVM::cancelAllAddedLinks_WhileAgentWasOFF()
{
    qDebug() << "Cancel all added links while the agent was OFF";

    QList<MappingElementM*> addedMappingElements = _hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.values();
    _hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.clear();
    sethadLinksAdded_WhileAgentWasOFF(false);
    qDeleteAll(addedMappingElements); // Free memory
}


/**
 * @brief Get the model of added mapping element (while the agent was OFF) from a link id
 */
MappingElementM *AgentInMappingVM::getAddedMappingElementFromLinkId_WhileAgentWasOFF(QString linkId)
{
    return _hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.value(linkId, nullptr);
}


/**
 * @brief Get the list of all added link Ids while the agent was OFF
 */
QList<QString> AgentInMappingVM::getAddedLinkIds_WhileAgentWasOFF()
{
    return _hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.keys();
}


/**
 * @brief Get the list of all (models of) added mapping elements while the agent was OFF
 */
QList<MappingElementM*> AgentInMappingVM::getAddedMappingElements_WhileAgentWasOFF()
{
    return _hashFromLinkIdToAddedMappingElement_WhileAgentWasOFF.values();
}


/**
 * @brief Remove a link while the agent was OFF
 */
void AgentInMappingVM::removeLink_WhileAgentIsOFF(QString linkId, MappingElementVM* mappingElement)
{
    qDebug() << "Remove the link" << linkId << "while the agent is OFF";

    if ((mappingElement != nullptr) && !_hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.contains(linkId))
    {
        _hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.insert(linkId, mappingElement);
        sethadLinksRemoved_WhileAgentWasOFF(true);
    }
}


/**
 * @brief Cancel the remove of the link while the agent was OFF
 */
void AgentInMappingVM::cancelRemoveLink_WhileAgentWasOFF(QString linkId)
{
    qDebug() << "Cancel the remove of the link" << linkId << "while the agent was OFF";

    if (_hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.contains(linkId))
    {
        _hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.remove(linkId);
        if (_hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.isEmpty())
        {
            sethadLinksRemoved_WhileAgentWasOFF(false);
        }
    }
}


/**
 * @brief Cancel all removed links while the agent was OFF
 */
void AgentInMappingVM::cancelAllRemovedLinks_WhileAgentWasOFF()
{
    qDebug() << "Cancel all removed links while the agent was OFF";

    // View models of mapping elements are stored and deleted else where
    _hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.clear();
    sethadLinksRemoved_WhileAgentWasOFF(false);
}


/**
 * @brief Get the view model of removed mapping element (while the agent was OFF) from a link id
 */
MappingElementVM* AgentInMappingVM::getRemovedMappingElementFromLinkId_WhileAgentWasOFF(QString linkId)
{
    return _hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.value(linkId, nullptr);
}


/**
 * @brief Get the list of all (view models of) removed mapping elements while the agent was OFF
 * @return
 */
QList<MappingElementVM*> AgentInMappingVM::getRemovedMappingElements_WhileAgentWasOFF()
{
    return _hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.values();
}


/**
 * @brief Get the list of all names of removed mapping elements while the agent was OFF
 */
QStringList AgentInMappingVM::getNamesOfRemovedMappingElements_WhileAgentWasOFF()
{
    QStringList namesOfRemovedMappingElements;
    for (MappingElementVM* mappingElement : _hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.values())
    {
        if ((mappingElement != nullptr) && !mappingElement->name().isEmpty())
        {
            namesOfRemovedMappingElements.append(mappingElement->name());
        }
    }
    return namesOfRemovedMappingElements;
}


/**
 * @brief A mapping element has just been removed and correspond to a link removed while the agent was off
 */
void AgentInMappingVM::mappingElementRemoved_CorrespondingLinkRemovedWhileAgentWasOFF(MappingElementVM* mappingElement)
{
    if (mappingElement != nullptr)
    {
        QString linkId = "";
        for (QString iterator : _hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.keys())
        {
            if (_hashFromLinkIdToRemovedMappingElement_WhileAgentWasOFF.value(iterator) == mappingElement)
            {
                linkId = iterator;
                break;
            }
        }
        if (!linkId.isEmpty())
        {
            cancelRemoveLink_WhileAgentWasOFF(linkId);
        }
    }
}


/**
 * @brief Slot called when some view models of inputs have been added to the agent(s grouped by name)
 * @param newInputs
 */
void AgentInMappingVM::_onInputsHaveBeenAdded(QList<InputVM*> newInputs)
{
    QList<LinkInputVM*> tempLinkInputs;

    for (InputVM* input : newInputs)
    {
        if ((input != nullptr) && !input->name().isEmpty())
        {
            //qDebug() << "AgentInMappingVM::_onInputsHaveBeenAdded" << input->name();

            LinkInputVM* linkInput = new LinkInputVM(input);

            tempLinkInputs.append(linkInput);

            if (!input->uid().isEmpty()) {
                _hashFromIdToLinkInput.insert(input->uid(), linkInput);
            }

            // Update the list of view models of link input for this name
            QList<LinkInputVM*> linkInputsWithSameName = getLinkInputsListFromName(input->name());
            linkInputsWithSameName.append(linkInput);
            _hashFromNameToLinkInputsList.insert(input->name(), linkInputsWithSameName);
        }
    }

    if (!tempLinkInputs.isEmpty())
    {
        _linkInputsList.append(tempLinkInputs);

        Q_EMIT linkInputsListHaveBeenAdded(tempLinkInputs);

        // Update the group (of value type) of the reduced link inputs of our agent (= brin)
        _updateReducedLinkInputsValueTypeGroup();

        // Check the maximum number of Inputs/Outputs
        _checkMaxNumberOfIO();
    }
}


/**
 * @brief Slot called when some view models of outputs have been added to the agent(s grouped by name)
 * @param newOutputs
 */
void AgentInMappingVM::_onOutputsHaveBeenAdded(QList<OutputVM*> newOutputs)
{
    QList<LinkOutputVM*> tempLinkOutputs;

    for (OutputVM* output : newOutputs)
    {
        if ((output != nullptr) && !output->name().isEmpty())
        {
            //qDebug() << "AgentInMappingVM::_onOutputsHaveBeenAdded" << output->name();

            LinkOutputVM* linkOutput = new LinkOutputVM(output);

            tempLinkOutputs.append(linkOutput);

            if (!output->uid().isEmpty()) {
                _hashFromIdToLinkOutput.insert(output->uid(), linkOutput);
            }

            // Update the list of view models of link output for this name
            QList<LinkOutputVM*> linkOutputsWithSameName = getLinkOutputsListFromName(output->name());
            linkOutputsWithSameName.append(linkOutput);
            _hashFromNameToLinkOutputsList.insert(output->name(), linkOutputsWithSameName);
        }
    }

    if (!tempLinkOutputs.isEmpty())
    {
        _linkOutputsList.append(tempLinkOutputs);

        Q_EMIT linkOutputsListHaveBeenAdded(tempLinkOutputs);

        // Update the group (of value type) of the reduced link outputs of our agent (= brin)
        _updateReducedLinkOutputsValueTypeGroup();

        // Check the maximum number of Inputs/Outputs
        _checkMaxNumberOfIO();
    }
}


/**
 * @brief Slot called when some view models of inputs will be removed from the agent(s grouped by name)
 * @param oldInputs
 */
void AgentInMappingVM::_onInputsWillBeRemoved(QList<InputVM*> oldInputs)
{
    QList<LinkInputVM*> tempLinkInputs;

    for (InputVM* input : oldInputs)
    {
        if ((input != nullptr) && !input->name().isEmpty())
        {
            //qDebug() << "AgentInMappingVM::_onInputsWillBeRemoved" << input->name();

            // Get the list of view models of link input for this name
            QList<LinkInputVM*> linkInputsWithSameName = getLinkInputsListFromName(input->name());

            LinkInputVM* linkInput = nullptr;

            // Input Id is defined
            if (!input->uid().isEmpty())
            {
                // Get the view model of link input for this input id
                linkInput = getLinkInputFromId(input->uid());
                if (linkInput != nullptr)
                {
                    // Remove from the hash table with the input id
                    _hashFromIdToLinkInput.remove(input->uid());
                }
            }
            // Input Id is empty...
            else
            {
                // ...search our link in the list
                for (LinkInputVM* iterator : linkInputsWithSameName)
                {
                    if ((iterator != nullptr) && (iterator->input() != nullptr) && (iterator->input() == input))
                    {
                        linkInput = iterator;
                        break;
                    }
                }
            }

            if (linkInput != nullptr)
            {
                tempLinkInputs.append(linkInput);

                linkInputsWithSameName.removeOne(linkInput);
            }

            // Update the list of view models of link input for this name
            _hashFromNameToLinkInputsList.insert(input->name(), linkInputsWithSameName);
        }
    }

    if (!tempLinkInputs.isEmpty())
    {
        _linkInputsList.remove(tempLinkInputs);

        Q_EMIT linkInputsListWillBeRemoved(tempLinkInputs);

        // Update the group (of value type) of the reduced link inputs of our agent (= brin)
        _updateReducedLinkInputsValueTypeGroup();

        // Check the maximum number of Inputs/Outputs
        _checkMaxNumberOfIO();
    }
}


/**
 * @brief Slot called when some view models of outputs will be removed from the agent(s grouped by name)
 * @param oldOutputs
 */
void AgentInMappingVM::_onOutputsWillBeRemoved(QList<OutputVM*> oldOutputs)
{
    QList<LinkOutputVM*> tempLinkOutputs;

    for (OutputVM* output : oldOutputs)
    {
        if ((output != nullptr) && !output->name().isEmpty())
        {
            //qDebug() << "AgentInMappingVM::_onOutputsWillBeRemoved" << output->name();

            // Get the list of view models of link output for this name
            QList<LinkOutputVM*> linkOutputsWithSameName = getLinkOutputsListFromName(output->name());

            LinkOutputVM* linkOutput = nullptr;

            // Output Id is defined
            if (!output->uid().isEmpty())
            {
                // Get the view model of link output for this output id
                linkOutput = getLinkOutputFromId(output->uid());
                if (linkOutput != nullptr)
                {
                    // Remove from the hash table with the output id
                    _hashFromIdToLinkOutput.remove(output->uid());
                }
            }
            // Output Id is empty...
            else
            {
                // ...search our link in the list
                for (LinkOutputVM* iterator : linkOutputsWithSameName)
                {
                    if ((iterator != nullptr) && (iterator->output() != nullptr) && (iterator->output() == output))
                    {
                        linkOutput = iterator;
                        break;
                    }
                }
            }

            if (linkOutput != nullptr)
            {
                tempLinkOutputs.append(linkOutput);

                linkOutputsWithSameName.removeOne(linkOutput);
            }

            // Update the list of view models of link output for this name
            _hashFromNameToLinkOutputsList.insert(output->name(), linkOutputsWithSameName);
        }
    }

    if (!tempLinkOutputs.isEmpty())
    {
        _linkOutputsList.remove(tempLinkOutputs);

        Q_EMIT linkOutputsListWillBeRemoved(tempLinkOutputs);

        // Update the group (of value type) of the reduced link outputs of our agent (= brin)
        _updateReducedLinkOutputsValueTypeGroup();

        // Check the maximum number of Inputs/Outputs
        _checkMaxNumberOfIO();
    }
}


/**
 * @brief Update the group (of value type) of the reduced link inputs of our agent (= brin)
 */
void AgentInMappingVM::_updateReducedLinkInputsValueTypeGroup()
{
    AgentIOPValueTypeGroups::Value globalReducedLinkInputsValueTypeGroup = AgentIOPValueTypeGroups::UNKNOWN;

    for (int i = 0; i < _linkInputsList.count(); i++)
    {
        LinkInputVM* linkInput = _linkInputsList.at(i);

        if ((linkInput != nullptr) && (linkInput->input() != nullptr) && (linkInput->input()->firstModel() != nullptr))
        {
            if (i == 0) {
                globalReducedLinkInputsValueTypeGroup = linkInput->input()->firstModel()->agentIOPValueTypeGroup();
            }
            else
            {
                if (globalReducedLinkInputsValueTypeGroup != linkInput->input()->firstModel()->agentIOPValueTypeGroup())
                {
                    globalReducedLinkInputsValueTypeGroup = AgentIOPValueTypeGroups::MIXED;
                    break;
                }
            }
        }
    }
    setreducedLinkInputsValueTypeGroup(globalReducedLinkInputsValueTypeGroup);
}


/**
 * @brief Update the group (of value type) of the reduced link outputs of our agent (= brin)
 */
void AgentInMappingVM::_updateReducedLinkOutputsValueTypeGroup()
{
    AgentIOPValueTypeGroups::Value globalReducedLinkOutputsValueTypeGroup = AgentIOPValueTypeGroups::UNKNOWN;

    for (int i = 0; i < _linkOutputsList.count(); i++)
    {
        LinkOutputVM* linkOutput = _linkOutputsList.at(i);

        if ((linkOutput != nullptr) && (linkOutput->output() != nullptr) && (linkOutput->output()->firstModel() != nullptr))
        {
            if (i == 0) {
                globalReducedLinkOutputsValueTypeGroup = linkOutput->output()->firstModel()->agentIOPValueTypeGroup();
            }
            else
            {
                if (globalReducedLinkOutputsValueTypeGroup != linkOutput->output()->firstModel()->agentIOPValueTypeGroup())
                {
                    globalReducedLinkOutputsValueTypeGroup = AgentIOPValueTypeGroups::MIXED;
                    break;
                }
            }
        }
    }
    setreducedLinkOutputsValueTypeGroup(globalReducedLinkOutputsValueTypeGroup);
}


/**
 * @brief Check the maximum number of Inputs/Outputs
 */
void AgentInMappingVM::_checkMaxNumberOfIO()
{
    int maxNumberOfIO = qMax(_linkInputsList.count(), _linkOutputsList.count());

    if (maxNumberOfIO > MAX_NUMBER_OF_IO_TO_LOCK_REDUCED)
    {
        // Force the QML to reduce the list of Inputs/Outputs
        setisReduced(true);

        // Lock
        setisLockedReduced(true);
    }
    else
    {
        // UN-lock
        setisLockedReduced(false);
    }
}
