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
#define MAX_NUMBER_OF_IO_TO_REDUCE 15

// Maximum number of Inputs/Outputs to lock reduced (prevent to open the list of Inputs/Outputs)
#define MAX_NUMBER_OF_IO_TO_LOCK_REDUCED 30


/**
 * @brief Constructor
 * @param agentsGroupedByName Models of agents grouped by the same name
 * @param position Position of the top left corner
 * @param parent
 */
AgentInMappingVM::AgentInMappingVM(AgentsGroupedByNameVM* agentsGroupedByName,
                                   QPointF position,
                                   QObject *parent) : QObject(parent),
    _name(""),
    _agentsGroupedByName(agentsGroupedByName),
    _position(position),
    _isReduced(false),
    _isLockedReduced(false),
    _reducedLinkInputsValueTypeGroup(AgentIOPValueTypeGroups::MIXED),
    _reducedLinkOutputsValueTypeGroup(AgentIOPValueTypeGroups::MIXED)
    //_temporaryMapping(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_agentsGroupedByName != nullptr)
    {
        _name = _agentsGroupedByName->name();

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
        // Deleted elsewhere
        setagentsGroupedByName(nullptr);
    }


    // FIXME REPAIR
    /*if (_temporaryMapping != nullptr)
    {
        AgentMappingM* temp = _temporaryMapping;
        settemporaryMapping(nullptr);
        delete temp;
    }*/
    // Delete all temporary mapping elements
    //_hashFromLinkIdToTemporaryMappingElement.clear();
    //_temporaryMappingElements.deleteAllItems();
}


/**
 * @brief Return the list of view models of link input from an input name
 * @param inputName
 */
QList<LinkInputVM*> AgentInMappingVM::getLinkInputsListFromName(QString inputName)
{
    if (_hashFromNameToLinkInputsList.contains(inputName)) {
        return _hashFromNameToLinkInputsList.value(inputName);
    }
    else {
        return QList<LinkInputVM*>();
    }
}


/**
 * @brief Return the view model of link input from an input id
 * @param inputId
 */
LinkInputVM* AgentInMappingVM::getLinkInputFromId(QString inputId)
{
    if (_hashFromIdToLinkInput.contains(inputId)) {
        return _hashFromIdToLinkInput.value(inputId);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Return the list of view models of link output from an output name
 * @param outputName
 */
QList<LinkOutputVM*> AgentInMappingVM::getLinkOutputsListFromName(QString outputName)
{
    if (_hashFromNameToLinkOutputsList.contains(outputName)) {
        return _hashFromNameToLinkOutputsList.value(outputName);
    }
    else {
        return QList<LinkOutputVM*>();
    }
}


/**
 * @brief Return the view model of link output from an output id
 * @param outputId
 */
LinkOutputVM* AgentInMappingVM::getLinkOutputFromId(QString outputId)
{
    if (_hashFromIdToLinkOutput.contains(outputId)) {
        return _hashFromIdToLinkOutput.value(outputId);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief FIXME REMOVE Add a temporary mapping element
 * This temporary mapping element will become a real link:
 * - when the user will activate the mapping
 * - or when our agent will evolve from OFF to ON
 * @param linkId
 * @param inputId
 * @param outputAgentName
 * @param outputId
 */
/*void AgentInMappingVM::addTemporaryMappingElement(QString linkId, QString inputId, QString outputAgentName, QString outputId)
{
    // Check that there is not already the same mapping element
    ElementMappingM* temporaryMappingElement = _getTemporaryMappingElementFromLinkId(linkId);
    if (temporaryMappingElement == nullptr)
    {
        // Create a temporary mapping element
        temporaryMappingElement = new ElementMappingM(_name, inputId, outputAgentName, outputId);

        // Add to the list and to the hash table
        _temporaryMappingElements.append(temporaryMappingElement);
        _hashFromLinkIdToTemporaryMappingElement.insert(linkId, temporaryMappingElement);
    }
}
void AgentInMappingVM::waitingReplyForAddedMappingElement(QString linkId, ElementMappingM* mappingElement)
{
    if (mappingElement != nullptr)
    {
        // Add to the list and to the hash table
        _addedMappingElements_WaitingReply.append(mappingElement);
        _hashFromLinkIdToAddedMappingElement_WaitingReply.insert(linkId, mappingElement);
    }
}
void AgentInMappingVM::waitingReplyForRemovedMappingElement(QString linkId, ElementMappingM* mappingElement)
{
    if (mappingElement != nullptr)
    {
        // Add to the list and to the hash table
        _removedMappingElements_WaitingReply.append(mappingElement);
        _hashFromLinkIdToRemovedMappingElement_WaitingReply.insert(linkId, mappingElement);
    }
}*/


/**
 * @brief FIXME REMOVE Remove a temporary mapping element
 * This temporary link mapping element became a real link:
 * - when the user activated the mapping
 * - or when our agent evolved from OFF to ON
 * @param linkId
 */
/*void AgentInMappingVM::removeTemporaryMappingElement(QString linkId)
{
    // Get the temporary mapping element from the link id
    ElementMappingM* temporaryMappingElement = _getTemporaryMappingElementFromLinkId(linkId);
    if (temporaryMappingElement != nullptr)
    {
        // Remove from the list and from the hash table
        _temporaryMappingElements.remove(temporaryMappingElement);
        _hashFromLinkIdToTemporaryMappingElement.remove(linkId);

        // Free memory
        delete temporaryMappingElement;
    }
}
void AgentInMappingVM::replyReceivedForAddedMappingElement(QString linkId)
{
    ElementMappingM* mappingElement = getAddedMappingElementFromLinkId_WaitingReply(linkId);
    if (mappingElement != nullptr)
    {
        // Remove from the list and from the hash table
        _addedMappingElements_WaitingReply.remove(mappingElement);
        _hashFromLinkIdToAddedMappingElement_WaitingReply.remove(linkId);
    }
}
void AgentInMappingVM::replyReceivedForRemovedMappingElement(QString linkId)
{
    ElementMappingM* mappingElement = getRemovedMappingElementFromLinkId_WaitingReply(linkId);
    if (mappingElement != nullptr)
    {
        // Remove from the list and from the hash table
        _removedMappingElements_WaitingReply.remove(mappingElement);
        _hashFromLinkIdToRemovedMappingElement_WaitingReply.remove(linkId);
    }
}*/


/**
 * @brief Get the model of added mapping element (for which we are waiting a reply to the request "add") from a link id
 * @param linkId
 * @return
 */
/*ElementMappingM* AgentInMappingVM::getAddedMappingElementFromLinkId_WaitingReply(QString linkId)
{
    if (_hashFromLinkIdToAddedMappingElement_WaitingReply.contains(linkId)) {
        return _hashFromLinkIdToAddedMappingElement_WaitingReply(linkId);
    }
    else {
        return nullptr;
    }
}*/
/**
 * @brief Get the model of removed mapping element (for which we are waiting a reply to the request "remove") from a link id
 * @param linkId
 * @return
 */
/*ElementMappingM* AgentInMappingVM::getRemovedMappingElementFromLinkId_WaitingReply(QString linkId)
{
    if (_hashFromLinkIdToRemovedMappingElement_WaitingReply.contains(linkId)) {
        return _hashFromLinkIdToRemovedMappingElement_WaitingReply(linkId);
    }
    else {
        return nullptr;
    }
}*/


/**
 * @brief Get the view model of added mapping element (while the mapping was UN-activated) from a link id
 * @param linkId
 * @return
 */
MappingElementVM* AgentInMappingVM::getAddedMappingElementFromLinkId_WhileMappingWasUNactivated(QString linkId)
{
    if (_hashFromLinkIdToAddedMappingElement_WhileMappingWasUNactivated.contains(linkId)) {
        return _hashFromLinkIdToAddedMappingElement_WhileMappingWasUNactivated.value(linkId);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get the view model of removed mapping element (while the mapping was UN-activated) from a link id
 * @param linkId
 * @return
 */
MappingElementVM* AgentInMappingVM::getRemovedMappingElementFromLinkId_WhileMappingWasUNactivated(QString linkId)
{
    if (_hashFromLinkIdToRemovedMappingElement_WhileMappingWasUNactivated.contains(linkId)) {
        return _hashFromLinkIdToRemovedMappingElement_WhileMappingWasUNactivated.value(linkId);
    }
    else {
        return nullptr;
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
        // FIXME TODO I2 Quick: Allow to remove a QList
        //_linkInputsList.remove(tempLinkInputs);
        for (LinkInputVM* linkInput : tempLinkInputs)
        {
            if (linkInput != nullptr) {
                _linkInputsList.remove(linkInput);
            }
        }

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
        // FIXME TODO I2 Quick: Allow to remove a QList
        //_linkOutputsList.remove(tempLinkOutputs);
        for (LinkOutputVM* linkOutput : tempLinkOutputs)
        {
            if (linkOutput != nullptr) {
                _linkOutputsList.remove(linkOutput);
            }
        }

        Q_EMIT linkOutputsListWillBeRemoved(tempLinkOutputs);

        // Update the group (of value type) of the reduced link outputs of our agent (= brin)
        _updateReducedLinkOutputsValueTypeGroup();

        // Check the maximum number of Inputs/Outputs
        _checkMaxNumberOfIO();
    }
}


/**
 * @brief Slot when the list of models changed
 */
/*void AgentInMappingVM::_onModelsChanged()
{
    QList<AgentM*> newAgentsList = _models.toList();

    // Model of agent added
    if (_previousAgentsList.count() < newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> ADD --> " << newAgentsList.count();

        for (AgentM* model : newAgentsList)
        {
            if ((model != nullptr) && !_previousAgentsList.contains(model))
            {
                qDebug() << "Agent in Mapping VM: New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals from a model
                connect(model, &AgentM::definitionChangedWithPreviousAndNewValues, this, &AgentInMappingVM::_onDefinitionOfModelChangedWithPreviousAndNewValues);

                // A model of agent has been added to our list
                _agentModelAdded(model);
            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList)
        {
            if ((model != nullptr) && !newAgentsList.contains(model))
            {
                qDebug() << "Agent in Mapping VM: Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals from a model
                disconnect(model, &AgentM::definitionChangedWithPreviousAndNewValues, this, &AgentInMappingVM::_onDefinitionOfModelChangedWithPreviousAndNewValues);

                // A model of agent has been removed from our list
                _agentModelRemoved(model);
            }
        }
    }

    _previousAgentsList = newAgentsList;
}*/


/**
 * @brief Slot called when the definition of a model changed (with previous and new values)
 * @param previousValue
 * @param newValue
 */
/*void AgentInMappingVM::_onDefinitionOfModelChangedWithPreviousAndNewValues(DefinitionM* previousValue, DefinitionM* newValue)
{
    //AgentM* agent = qobject_cast<AgentM*>(sender());
    if ((previousValue != nullptr) && (newValue != nullptr)) // && (agent != nullptr)
    {
        //
        // Check if input(s) have been removed
        //
        QList<InputVM*> inputsListToRemove;
        for (AgentIOPM* input : previousValue->inputsList()->toList())
        {
            if ((input != nullptr) && !input->id().isEmpty() && !newValue->inputsIdsList().contains(input->id()))
            {
                InputVM* inputVM = _inputModelRemoved(input);
                if (inputVM != nullptr)
                {
                    // The view model of input is empty
                    if (inputVM->models()->isEmpty()) {
                        inputsListToRemove.append(inputVM);
                    }
                }
            }
        }
        if (!inputsListToRemove.isEmpty())
        {
            // Emit signal "Inputs List Will Be Removed"
            Q_EMIT inputsListWillBeRemoved(inputsListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_inputsList.remove(inputsListToRemove);
            for (InputVM* inputVM, inputsListToRemove) {
                _inputsList.remove(inputVM);
            }
        }


        //
        // Check if output(s) have been removed
        //
        QList<OutputVM*> outputsListToRemove;
        for (OutputM* output : previousValue->outputsList()->toList())
        {
            if ((output != nullptr) && !output->id().isEmpty() && !newValue->outputsIdsList().contains(output->id()))
            {
                OutputVM* outputVM = _outputModelRemoved(output);
                if (outputVM != nullptr)
                {
                    // The view model of output is empty
                    if (outputVM->models()->isEmpty()) {
                        outputsListToRemove.append(outputVM);
                    }
                }
            }
        }
        if (!outputsListToRemove.isEmpty())
        {
            // Emit signal "Outputs List Will Be Removed"
            Q_EMIT outputsListWillBeRemoved(outputsListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_outputsList.remove(outputsListToRemove);
            for (OutputVM* outputVM, outputsListToRemove) {
                _outputsList.remove(outputVM);
            }
        }


        //
        // Check if parameter(s) have been removed
        //
        QList<ParameterVM*> parametersListToRemove;
        for (AgentIOPM* parameter : previousValue->parametersList()->toList())
        {
            if ((parameter != nullptr) && !parameter->id().isEmpty() && !newValue->parametersIdsList().contains(parameter->id()))
            {
                ParameterVM* parameterVM = _parameterModelRemoved(parameter);
                if (parameterVM != nullptr)
                {
                    // The view model of parameter is empty
                    if (parameterVM->models()->isEmpty()) {
                        parametersListToRemove.append(parameterVM);
                    }
                }
            }
        }
        if (!parametersListToRemove.isEmpty())
        {
            // Emit signal "Parameters List Will Be Removed"
            //Q_EMIT parametersListWillBeRemoved(parametersListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_parametersList.remove(parametersListToRemove);
            for (ParameterVM* parameterVM, parametersListToRemove) {
                _parametersList.remove(parameterVM);
            }
        }


        //
        // Check if input(s) have been added
        //
        QList<InputVM*> inputsListToAdd;
        for (AgentIOPM* input : newValue->inputsList()->toList())
        {
            if ((input != nullptr) && !input->id().isEmpty())
            {
                InputVM* inputVM = nullptr;

                // This input was already in the previous definition (just replace the model)
                if (previousValue->inputsIdsList().contains(input->id()))
                {
                    AgentIOPM* previousModelOfInput = previousValue->getInputWithName(input->name());
                    inputVM = getInputFromId(input->id());

                    if ((inputVM != nullptr) && (previousModelOfInput != nullptr))
                    {
                        int index = inputVM->models()->indexOf(previousModelOfInput);
                        if (index > -1) {
                            inputVM->models()->replace(index, input);
                        }
                    }
                }
                // This input is a new one
                else
                {
                    inputVM = _inputModelAdded(input);
                    if (inputVM != nullptr)
                    {
                        // New view model of input
                        if (!_inputsList.contains(inputVM)) {
                            inputsListToAdd.append(inputVM);
                        }
                    }
                }
            }
        }
        if (!inputsListToAdd.isEmpty())
        {
            _inputsList.append(inputsListToAdd);

            // Emit signal "Inputs List Have Been Added"
            Q_EMIT inputsListHaveBeenAdded(inputsListToAdd);
        }


        //
        // Check if output(s) have been added
        //
        QList<OutputVM*> outputsListToAdd;
        for (OutputM* output : newValue->outputsList()->toList())
        {
            if ((output != nullptr) && !output->id().isEmpty())
            {
                OutputVM* outputVM = nullptr;

                // This output was already in the previous definition (just replace the model)
                if (previousValue->outputsIdsList().contains(output->id()))
                {
                    OutputM* previousModelOfOutput = previousValue->getOutputWithName(output->name());
                    outputVM = getOutputFromId(output->id());

                    if ((outputVM != nullptr)  && (previousModelOfOutput != nullptr))
                    {
                        int index = outputVM->models()->indexOf(previousModelOfOutput);
                        if (index > -1) {
                            outputVM->models()->replace(index, output);
                        }
                    }
                }
                // This output is a new one
                else
                {
                    outputVM = _outputModelAdded(output);
                    if (outputVM != nullptr)
                    {
                        // New view model of output
                        if (!_outputsList.contains(outputVM)) {
                            outputsListToAdd.append(outputVM);
                        }
                    }
                }
            }
        }
        if (!outputsListToAdd.isEmpty())
        {
            _outputsList.append(outputsListToAdd);

            // Emit signal "Outputs List Have Been Added"
            Q_EMIT outputsListHaveBeenAdded(outputsListToAdd);
        }


        //
        // Check if parameter(s) have been added
        //
        QList<ParameterVM*> parametersListToAdd;
        for (AgentIOPM* parameter : newValue->parametersList()->toList())
        {
            if ((parameter != nullptr) && !parameter->id().isEmpty())
            {
                ParameterVM* parameterVM = nullptr;

                // This parameter was already in the previous definition (just replace the model)
                if (previousValue->parametersIdsList().contains(parameter->id()))
                {
                    AgentIOPM* previousModelOfParameter = previousValue->getParameterWithName(parameter->name());
                    parameterVM = getParameterFromId(parameter->id());

                    if ((parameterVM != nullptr) && (previousModelOfParameter != nullptr))
                    {
                        int index = parameterVM->models()->indexOf(previousModelOfParameter);
                        if (index > -1) {
                            parameterVM->models()->replace(index, parameter);
                        }
                    }
                }
                // This parameter is a new one
                else
                {
                    parameterVM = _parameterModelAdded(parameter);
                    if (parameterVM != nullptr)
                    {
                        // New view model of parameter
                        if (!_parametersList.contains(parameterVM)) {
                            parametersListToAdd.append(parameterVM);
                        }
                    }
                }
            }
        }
        if (!parametersListToAdd.isEmpty())
        {
            _parametersList.append(parametersListToAdd);

            // Emit signal "parameters List Have Been Added"
            //Q_EMIT parametersListHaveBeenAdded(parametersListToAdd);
        }
    }
}*/


/**
 * @brief A model of agent has been added to our list
 * @param model
 */
/*void AgentInMappingVM::_agentModelAdded(AgentM* model)
{
    if ((model != nullptr) && (model->definition() != nullptr))
    {
        QList<InputVM*> inputsListToAdd;
        QList<OutputVM*> outputsListToAdd;
        QList<ParameterVM*> parametersListToAdd;

        // Traverse the list of models of inputs in the definition
        for (AgentIOPM* input, model->definition()->inputsList()->toList())
        {
            InputVM* inputVM = _inputModelAdded(input);
            if (inputVM != nullptr)
            {
                // New view model of input
                if (!_inputsList.contains(inputVM)) {
                    inputsListToAdd.append(inputVM);
                }
            }
        }

        // Traverse the list of models of outputs in the definition
        for (OutputM* output, model->definition()->outputsList()->toList())
        {
            OutputVM* outputVM = _outputModelAdded(output);
            if (outputVM != nullptr)
            {
                // New view model of output
                if (!_outputsList.contains(outputVM)) {
                    outputsListToAdd.append(outputVM);
                }
            }
        }

        // Traverse the list of models of parameters in the definition
        for (AgentIOPM* parameter, model->definition()->parametersList()->toList())
        {
            ParameterVM* parameterVM = _parameterModelAdded(parameter);
            if (parameterVM != nullptr)
            {
                // New view model of parameter
                if (!_parametersList.contains(parameterVM)) {
                    parametersListToAdd.append(parameterVM);
                }
            }
        }

        if (!inputsListToAdd.isEmpty()) {
            _inputsList.append(inputsListToAdd);
        }
        if (!outputsListToAdd.isEmpty()) {
            _outputsList.append(outputsListToAdd);
        }
        if (!parametersListToAdd.isEmpty()) {
            _parametersList.append(parametersListToAdd);
        }
    }
}*/


/**
 * @brief A model of agent has been removed from our list
 * @param model
 */
/*void AgentInMappingVM::_agentModelRemoved(AgentM* model)
{
    if ((model != nullptr) && (model->definition() != nullptr))
    {
        //
        // Traverse the list of models of inputs in the definition
        //
        QList<InputVM*> inputsListToRemove;
        for (AgentIOPM* input, model->definition()->inputsList()->toList())
        {
            InputVM* inputVM = _inputModelRemoved(input);
            if (inputVM != nullptr)
            {
                // The view model of input is empty
                if (inputVM->models()->isEmpty()) {
                    inputsListToRemove.append(inputVM);
                }
            }
        }
        if (!inputsListToRemove.isEmpty())
        {
            // Emit signal "Inputs List Will Be Removed"
            Q_EMIT inputsListWillBeRemoved(inputsListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_inputsList.remove(inputsListToRemove);
            for (InputVM* inputVM, inputsListToRemove) {
                _inputsList.remove(inputVM);
            }
        }


        //
        // Traverse the list of models of outputs in the definition
        //
        QList<OutputVM*> outputsListToRemove;
        for (OutputM* output, model->definition()->outputsList()->toList())
        {
            OutputVM* outputVM = _outputModelRemoved(output);
            if (outputVM != nullptr)
            {
                // The view model of output is empty
                if (outputVM->models()->isEmpty()) {
                    outputsListToRemove.append(outputVM);
                }
            }
        }
        if (!outputsListToRemove.isEmpty())
        {
            // Emit signal "Outputs List Will Be Removed"
            Q_EMIT outputsListWillBeRemoved(outputsListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_outputsList.remove(outputsListToRemove);
            for (OutputVM* outputVM, outputsListToRemove) {
                _outputsList.remove(outputVM);
            }
        }


        //
        // Traverse the list of models of parameters in the definition
        //
        QList<ParameterVM*> parametersListToRemove;
        for (AgentIOPM* parameter, model->definition()->parametersList()->toList())
        {
            ParameterVM* parameterVM = _parameterModelRemoved(parameter);
            if (parameterVM != nullptr)
            {
                // The view model of parameter is empty
                if (parameterVM->models()->isEmpty()) {
                    parametersListToRemove.append(parameterVM);
                }
            }
        }
        if (!parametersListToRemove.isEmpty())
        {
            // Emit signal "Parameters List Will Be Removed"
            //Q_EMIT parametersListWillBeRemoved(parametersListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_parametersList.remove(parametersListToRemove);
            for (ParameterVM* parameterVM, parametersListToRemove) {
                _parametersList.remove(parameterVM);
            }
        }
    }
}*/


/**
 * @brief A model of input has been added
 * @param input
 * @return
 */
/*InputVM* AgentInMappingVM::_inputModelAdded(AgentIOPM* input)
{
    InputVM* inputVM = nullptr;

    if (input != nullptr)
    {
        // First, we get a ghost of this input: an input without id (only the same name)
        QList<InputVM*> inputsWithSameName = getInputsListFromName(input->name());
        InputVM* inputWithoutId = nullptr;

        for (InputVM* iterator, inputsWithSameName)
        {
            // Already a view model with an EMPTY id (NOT defined)
            if ((iterator != nullptr) && iterator->id().isEmpty()) {
                inputWithoutId = iterator;
                break;
            }
        }

        // Input id is NOT defined
        if (input->id().isEmpty())
        {
            // There is already a view model without id
            if (inputWithoutId != nullptr)
            {
                inputVM = inputWithoutId;

                // Add this new model to the list
                inputVM->models()->append(input);
            }
            // There is not yet a view model without id
            else
            {
                // Create a new view model of input (without id)
                inputVM = new InputVM(input->name(),
                                      "",
                                      input,
                                      this);

                // Don't add to the list here (this input will be added globally via temporary list)

                // Update the hash table with the input name
                inputsWithSameName.append(inputVM);
                _mapFromNameToInputsList.insert(input->name(), inputsWithSameName);
            }
        }
        // Input id is defined
        else
        {
            // There is already a view model without id
            if (inputWithoutId != nullptr)
            {
                // FIXME TODO: gestion du ghost...passage en view model avec id
            }

            inputVM = getInputFromId(input->id());

            // There is already a view model for this id
            if (inputVM != nullptr)
            {
                // Add this new model to the list
                inputVM->models()->append(input);
            }
            // There is not yet a view model for this id
            else
            {
                // Create a new view model of input
                inputVM = new InputVM(input->name(),
                                      input->id(),
                                      input,
                                      this);

                // Don't add to the list here (this input will be added globally via temporary list)

                // Update the hash table with the input id
                _mapFromUniqueIdToInput.insert(input->id(), inputVM);

                // Update the hash table with the input name
                inputsWithSameName.append(inputVM);
                _mapFromNameToInputsList.insert(input->name(), inputsWithSameName);
            }
        }
    }

    return inputVM;
}*/


/**
 * @brief A model of input has been removed
 * @param input
 * @return
 */
/*InputVM* AgentInMappingVM::_inputModelRemoved(AgentIOPM* input)
{
    InputVM* inputVM = nullptr;

    if (input != nullptr)
    {
        // Input id is defined
        if (!input->id().isEmpty())
        {
            inputVM = getInputFromId(input->id());
            if (inputVM != nullptr)
            {
                // Remove this model from the list
                inputVM->models()->remove(input);
            }
        }
    }

    return inputVM;
}*/


/**
 * @brief A model of output has been added
 * @param output
 * @return
 */
/*OutputVM* AgentInMappingVM::_outputModelAdded(OutputM* output)
{
    OutputVM* outputVM = nullptr;

    if (output != nullptr)
    {
        // First, we get a ghost of this output: an output without id (only the same name)
        QList<OutputVM*> outputsWithSameName = getOutputsListFromName(output->name());
        OutputVM* outputWithoutId = nullptr;

        for (OutputVM* iterator, outputsWithSameName)
        {
            // Already a view model with an EMPTY id (NOT defined)
            if ((iterator != nullptr) && iterator->id().isEmpty()) {
                outputWithoutId = iterator;
                break;
            }
        }

        // Output id is NOT defined
        if (output->id().isEmpty())
        {
            // There is already a view model without id
            if (outputWithoutId != nullptr)
            {
                outputVM = outputWithoutId;

                // Add this new model to the list
                outputVM->models()->append(output);
            }
            // There is not yet a view model without id
            else
            {
                // Create a new view model of output (without id)
                outputVM = new OutputVM(output->name(),
                                        "",
                                        output,
                                        this);

                // Don't add to the list here (this output will be added globally via temporary list)

                // Update the hash table with the output name
                outputsWithSameName.append(outputVM);
                _mapFromNameToOutputsList.insert(output->name(), outputsWithSameName);
            }
        }
        // Output id is defined
        else
        {
            // There is already a view model without id
            if (outputWithoutId != nullptr)
            {
                // FIXME TODO: gestion du ghost...passage en view model avec id
            }

            outputVM = getOutputFromId(output->id());

            // There is already a view model for this id
            if (outputVM != nullptr)
            {
                // Add this new model to the list
                outputVM->models()->append(output);
            }
            // There is not yet a view model for this id
            else
            {
                // Create a new view model of output
                outputVM = new OutputVM(output->name(),
                                        output->id(),
                                        output,
                                        this);

                // Don't add to the list here (this output will be added globally via temporary list)

                // Update the hash table with the output id
                _mapFromUniqueIdToOutput.insert(output->id(), outputVM);

                // Update the hash table with the output name
                outputsWithSameName.append(outputVM);
                _mapFromNameToOutputsList.insert(output->name(), outputsWithSameName);
            }
        }
    }

    return outputVM;
}*/



/**
 * @brief A model of output has been removed
 * @param output
 * @return
 */
/*OutputVM* AgentInMappingVM::_outputModelRemoved(OutputM* output)
{
    OutputVM* outputVM = nullptr;

    if (output != nullptr)
    {
        // Output id is defined
        if (!output->id().isEmpty())
        {
            outputVM = getOutputFromId(output->id());
            if (outputVM != nullptr)
            {
                // Remove this model from the list
                outputVM->models()->remove(output);
            }
        }
    }

    return outputVM;
}*/


/**
 * @brief A model of parameter has been added
 * @param parameter
 * @return
 */
/*ParameterVM* AgentInMappingVM::_parameterModelAdded(AgentIOPM* parameter)
{
    ParameterVM* parameterVM = nullptr;

    if (parameter != nullptr)
    {
        // First, we get a ghost of this parameter: a parameter without id (only the same name)
        QList<ParameterVM*> parametersWithSameName = getParametersListFromName(parameter->name());
        ParameterVM* parameterWithoutId = nullptr;

        for (ParameterVM* iterator, parametersWithSameName)
        {
            // Already a view model with an EMPTY id (NOT defined)
            if ((iterator != nullptr) && iterator->id().isEmpty()) {
                parameterWithoutId = iterator;
                break;
            }
        }

        // Parameter id is NOT defined
        if (parameter->id().isEmpty())
        {
            // There is already a view model without id
            if (parameterWithoutId != nullptr)
            {
                parameterVM = parameterWithoutId;

                // Add this new model to the list
                parameterVM->models()->append(parameter);
            }
            // There is not yet a view model without id
            else
            {
                // Create a new view model of parameter (without id)
                parameterVM = new ParameterVM(parameter->name(),
                                              "",
                                              parameter,
                                              this);

                // Don't add to the list here (this parameter will be added globally via temporary list)

                // Update the hash table with the parameter name
                parametersWithSameName.append(parameterVM);
                _mapFromNameToParametersList.insert(parameter->name(), parametersWithSameName);
            }
        }
        // Parameter id is defined
        else
        {
            // There is already a view model without id
            if (parameterWithoutId != nullptr)
            {
                // FIXME TODO: gestion du ghost...passage en view model avec id
            }

            parameterVM = getParameterFromId(parameter->id());

            // There is already a view model for this id
            if (parameterVM != nullptr)
            {
                // Add this new model to the list
                parameterVM->models()->append(parameter);
            }
            // There is not yet a view model for this id
            else
            {
                // Create a new view model of parameter
                parameterVM = new ParameterVM(parameter->name(),
                                              parameter->id(),
                                              parameter,
                                              this);

                // Don't add to the list here (this parameter will be added globally via temporary list)

                // Update the hash table with the parameter id
                _mapFromUniqueIdToParameter.insert(parameter->id(), parameterVM);

                // Update the hash table with the parameter name
                parametersWithSameName.append(parameterVM);
                _mapFromNameToParametersList.insert(parameter->name(), parametersWithSameName);
            }
        }
    }

    return parameterVM;
}*/


/**
 * @brief A model of parameter has been removed
 * @param parameter
 * @return
 */
/*ParameterVM* AgentInMappingVM::_parameterModelRemoved(AgentIOPM* parameter)
{
    ParameterVM* parameterVM = nullptr;

    if (parameter != nullptr)
    {
        // Parameter id is defined
        if (!parameter->id().isEmpty())
        {
            parameterVM = getParameterFromId(parameter->id());
            if (parameterVM != nullptr)
            {
                // Remove this model from the list
                parameterVM->models()->remove(parameter);
            }
        }
    }

    return parameterVM;
}*/


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
