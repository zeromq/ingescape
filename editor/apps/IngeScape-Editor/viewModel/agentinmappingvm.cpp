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
    _reducedMapValueTypeGroupInInput(AgentIOPValueTypeGroups::MIXED),
    _reducedMapValueTypeGroupInOutput(AgentIOPValueTypeGroups::MIXED),
    _temporaryMapping(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_agentsGroupedByName != nullptr)
    {
        _name = _agentsGroupedByName->name();

    }

    if (models.count() > 0)
    {
        AgentM* firstModel = models.first();
        if (firstModel != nullptr)
        {
            // Set the name of our agent in mapping
            _name = firstModel->name();


            // Create the mapping currently edited
            QString mappingName = QString("Mapping name of %1 in IngeScape Editor").arg(_name);
            QString mappingDescription = QString("Mapping description of %1 in IngeScape Editor").arg(_name);
            _temporaryMapping = new AgentMappingM(mappingName, "0.0", mappingDescription);


            // Connect to signal "Count Changed" from the list of ...
            //connect(&_inputsList, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onInputsListChanged);
            //connect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onOutputsListChanged);

            // Initialize our list
            //_models.append(models);
        }
        else {
            qCritical() << "No agent model for the agent in mapping !";
        }
    }
}


/**
 * @brief Destructor
 */
AgentInMappingVM::~AgentInMappingVM()
{
    qInfo() << "Delete View Model of Agent in Mapping" << _name;

    //disconnect(&_inputsList, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onInputsListChanged);
    //disconnect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onOutputsListChanged);

    if (_agentsGroupedByName != nullptr) {
        // Deleted elsewhere
        setagentsGroupedByName(nullptr);
    }

    if (_temporaryMapping != nullptr) {
        delete _temporaryMapping;
    }

    // Clear maps of Inputs, Outputs and Parameters
    _mapFromNameToInputsList.clear();
    _mapFromUniqueIdToInput.clear();
    _mapFromNameToOutputsList.clear();
    _mapFromUniqueIdToOutput.clear();
    _mapFromNameToParametersList.clear();
    _mapFromUniqueIdToParameter.clear();

    // Delete elements in the lists of Inputs & Outputs
    _inputsList.deleteAllItems();
    _outputsList.deleteAllItems();
    _parametersList.deleteAllItems();
}


/**
 * @brief Add a temporary link (this temporary link will became a real link when the user will activate the mapping)
 * @param inputName
 * @param outputAgentName
 * @param outputName
 * @return true if the link has been added
 */
bool AgentInMappingVM::addTemporaryLink(QString inputName, QString outputAgentName, QString outputName)
{
    bool hasBeenAdded = false;

    if (_temporaryMapping != nullptr)
    {
        // Check that there is not already the same link
        ElementMappingM* temporaryLink = _getTemporaryLink(inputName, outputAgentName, outputName);
        if (temporaryLink == nullptr)
        {
            temporaryLink = new ElementMappingM(_name, inputName, outputAgentName, outputName);

            _temporaryMapping->mappingElements()->append(temporaryLink);

            hasBeenAdded = true;
        }
    }
    return hasBeenAdded;
}


/**
 * @brief Remove temporary link (this temporary link will be removed when the user will activate the mapping)
 * @param inputName
 * @param outputAgentName
 * @param outputName
 * @return true if the link has been removed
 */
bool AgentInMappingVM::removeTemporaryLink(QString inputName, QString outputAgentName, QString outputName)
{
    bool hasBeenRemoved = false;

    if (_temporaryMapping != nullptr)
    {
        // Get the temporary link with same names
        ElementMappingM* temporaryLink = _getTemporaryLink(inputName, outputAgentName, outputName);
        if (temporaryLink != nullptr)
        {
            _temporaryMapping->mappingElements()->remove(temporaryLink);

            hasBeenRemoved = true;
        }
    }
    return hasBeenRemoved;
}


/**
 * @brief Return the list of view models of input from the input name
 * @param inputName
 */
QList<InputVM*> AgentInMappingVM::getInputsListFromName(QString inputName)
{
    if (_mapFromNameToInputsList.contains(inputName)) {
        return _mapFromNameToInputsList.value(inputName);
    }
    else {
        return QList<InputVM*>();
    }
}


/**
 * @brief Return the view model of input from the input id
 * @param inputId
 */
InputVM* AgentInMappingVM::getInputFromId(QString inputId)
{
    if (_mapFromUniqueIdToInput.contains(inputId)) {
        return _mapFromUniqueIdToInput.value(inputId);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Return the list of view models of output from the output name
 * @param outputName
 */
QList<OutputVM*> AgentInMappingVM::getOutputsListFromName(QString outputName)
{
    if (_mapFromNameToOutputsList.contains(outputName)) {
        return _mapFromNameToOutputsList.value(outputName);
    }
    else {
        return QList<OutputVM*>();
    }
}


/**
 * @brief Return the view model of output from the output id
 * @param outputId
 */
OutputVM* AgentInMappingVM::getOutputFromId(QString outputId)
{
    if (_mapFromUniqueIdToOutput.contains(outputId)) {
        return _mapFromUniqueIdToOutput.value(outputId);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Return the list of view models of parameter from the parameter name
 * @param parameterName
 */
QList<ParameterVM*> AgentInMappingVM::getParametersListFromName(QString parameterName)
{
    if (_mapFromNameToParametersList.contains(parameterName)) {
        return _mapFromNameToParametersList.value(parameterName);
    }
    else {
        return QList<ParameterVM*>();
    }
}


/**
 * @brief Return the view model of parameter from the parameter id
 * @param parameterId
 */
ParameterVM* AgentInMappingVM::getParameterFromId(QString parameterId)
{
    if (_mapFromUniqueIdToParameter.contains(parameterId)) {
        return _mapFromUniqueIdToParameter.value(parameterId);
    }
    else {
        return NULL;
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


    // Update the group (of value type) of the reduced map (= brin) in input and in output of our agent
    _updateReducedMapValueTypeGroupInInput();
    _updateReducedMapValueTypeGroupInOutput();
}*/


/**
 * @brief Slot when the list of (view models of) inputs changed
 */
/*void AgentInMappingVM::_onInputsListChanged()
{
    foreach (InputVM* input, _inputsList.toList()) {
        if ((input != nullptr) && (input->firstModel() != nullptr)) {
        }
    }
}*/


/**
 * @brief Slot when the list of (view models of) outputs changed
 */
/*void AgentInMappingVM::_onOutputsListChanged()
{
    foreach (OutputVM* output, _outputsList.toList()) {
        if ((output != nullptr) && (output->firstModel() != nullptr)) {
        }
    }
}*/


/**
 * @brief Slot called when the definition of a model changed (with previous and new values)
 * @param previousValue
 * @param newValue
 */
void AgentInMappingVM::_onDefinitionOfModelChangedWithPreviousAndNewValues(DefinitionM* previousValue, DefinitionM* newValue)
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
            foreach (InputVM* inputVM, inputsListToRemove) {
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
            foreach (OutputVM* outputVM, outputsListToRemove) {
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
            foreach (ParameterVM* parameterVM, parametersListToRemove) {
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
                InputVM* inputVM = NULL;

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
                OutputVM* outputVM = NULL;

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
                ParameterVM* parameterVM = NULL;

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


        // Emit signal "models of Inputs/Outputs/Parameters Changed"
        Q_EMIT modelsOfIOPChanged();


        // Update the flag "Are Identicals All Definitions"
        _updateIsDefinedInAllDefinitionsForEachIOP();
    }
}


/**
 * @brief A model of agent has been added to our list
 * @param model
 */
void AgentInMappingVM::_agentModelAdded(AgentM* model)
{
    if ((model != nullptr) && (model->definition() != nullptr))
    {
        QList<InputVM*> inputsListToAdd;
        QList<OutputVM*> outputsListToAdd;
        QList<ParameterVM*> parametersListToAdd;

        // Traverse the list of models of inputs in the definition
        foreach (AgentIOPM* input, model->definition()->inputsList()->toList())
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
        foreach (OutputM* output, model->definition()->outputsList()->toList())
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
        foreach (AgentIOPM* parameter, model->definition()->parametersList()->toList())
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

        // Emit signal "models of Inputs/Outputs/Parameters Changed"
        Q_EMIT modelsOfIOPChanged();

        // Update the flag "Are Identicals All Definitions"
        _updateIsDefinedInAllDefinitionsForEachIOP();
    }
}


/**
 * @brief A model of agent has been removed from our list
 * @param model
 */
void AgentInMappingVM::_agentModelRemoved(AgentM* model)
{
    if ((model != nullptr) && (model->definition() != nullptr))
    {
        //
        // Traverse the list of models of inputs in the definition
        //
        QList<InputVM*> inputsListToRemove;
        foreach (AgentIOPM* input, model->definition()->inputsList()->toList())
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
            foreach (InputVM* inputVM, inputsListToRemove) {
                _inputsList.remove(inputVM);
            }
        }


        //
        // Traverse the list of models of outputs in the definition
        //
        QList<OutputVM*> outputsListToRemove;
        foreach (OutputM* output, model->definition()->outputsList()->toList())
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
            foreach (OutputVM* outputVM, outputsListToRemove) {
                _outputsList.remove(outputVM);
            }
        }


        //
        // Traverse the list of models of parameters in the definition
        //
        QList<ParameterVM*> parametersListToRemove;
        foreach (AgentIOPM* parameter, model->definition()->parametersList()->toList())
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
            foreach (ParameterVM* parameterVM, parametersListToRemove) {
                _parametersList.remove(parameterVM);
            }
        }


        // Emit signal "models of Inputs/Outputs/Parameters Changed"
        Q_EMIT modelsOfIOPChanged();

        // Update the flag "Are Identicals All Definitions"
        _updateIsDefinedInAllDefinitionsForEachIOP();
    }
}


/**
 * @brief A model of input has been added
 * @param input
 * @return
 */
InputVM* AgentInMappingVM::_inputModelAdded(AgentIOPM* input)
{
    InputVM* inputVM = NULL;

    if (input != nullptr)
    {
        // First, we get a ghost of this input: an input without id (only the same name)
        QList<InputVM*> inputsWithSameName = getInputsListFromName(input->name());
        InputVM* inputWithoutId = NULL;

        foreach (InputVM* iterator, inputsWithSameName)
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
}


/**
 * @brief A model of input has been removed
 * @param input
 * @return
 */
InputVM* AgentInMappingVM::_inputModelRemoved(AgentIOPM* input)
{
    InputVM* inputVM = NULL;

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
            /*else
            {
                inputVM = getInputFromName(input->name());
                if (inputVM != nullptr)
                {
                    // Remove this model from the list
                    inputVM->models().remove(input);
                }
            }*/
        }
    }

    return inputVM;
}


/**
 * @brief A model of output has been added
 * @param output
 * @return
 */
OutputVM* AgentInMappingVM::_outputModelAdded(OutputM* output)
{
    OutputVM* outputVM = NULL;

    if (output != nullptr)
    {
        // First, we get a ghost of this output: an output without id (only the same name)
        QList<OutputVM*> outputsWithSameName = getOutputsListFromName(output->name());
        OutputVM* outputWithoutId = NULL;

        foreach (OutputVM* iterator, outputsWithSameName)
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
}



/**
 * @brief A model of output has been removed
 * @param output
 * @return
 */
OutputVM* AgentInMappingVM::_outputModelRemoved(OutputM* output)
{
    OutputVM* outputVM = NULL;

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
            /*else
            {
                outputVM = getOutputFromName(output->name());
                if (outputVM != nullptr)
                {
                    // Remove this model from the list
                    outputVM->models().remove(output);
                }
            }*/
        }
    }

    return outputVM;
}


/**
 * @brief A model of parameter has been added
 * @param parameter
 * @return
 */
ParameterVM* AgentInMappingVM::_parameterModelAdded(AgentIOPM* parameter)
{
    ParameterVM* parameterVM = NULL;

    if (parameter != nullptr)
    {
        // First, we get a ghost of this parameter: a parameter without id (only the same name)
        QList<ParameterVM*> parametersWithSameName = getParametersListFromName(parameter->name());
        ParameterVM* parameterWithoutId = NULL;

        foreach (ParameterVM* iterator, parametersWithSameName)
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
}


/**
 * @brief A model of parameter has been removed
 * @param parameter
 * @return
 */
ParameterVM* AgentInMappingVM::_parameterModelRemoved(AgentIOPM* parameter)
{
    ParameterVM* parameterVM = NULL;

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
            /*else
            {
                parameterVM = getParameterFromName(parameter->name());
                if (parameterVM != nullptr)
                {
                    // Remove this model from the list
                    parameterVM->models().remove(parameter);
                }
            }*/
        }
    }

    return parameterVM;
}


/**
 * @brief Update the flag "Is Defined in All Definitions" for each Input/Output/Parameter
 */
void AgentInMappingVM::_updateIsDefinedInAllDefinitionsForEachIOP()
{
    int numberOfModels = _models.count();

    // Update the flag "Is Defined in All Definitions" for each input
    for (InputVM* input : _inputsList.toList())
    {
        if (input != nullptr)
        {
            if (input->models()->count() == numberOfModels) {
                input->setisDefinedInAllDefinitions(true);
            }
            else {
                input->setisDefinedInAllDefinitions(false);
            }
        }
    }

    // Update the flag "Is Defined in All Definitions" for each output
    for (OutputVM* output : _outputsList.toList())
    {
        if (output != nullptr)
        {
            if (output->models()->count() == numberOfModels) {
                output->setisDefinedInAllDefinitions(true);
            }
            else {
                output->setisDefinedInAllDefinitions(false);
            }
        }
    }

    // Update the flag "Is Defined in All Definitions" for each parameter
    for (ParameterVM* parameter : _parametersList.toList())
    {
        if (parameter != nullptr)
        {
            if (parameter->models()->count() == numberOfModels) {
                parameter->setisDefinedInAllDefinitions(true);
            }
            else {
                parameter->setisDefinedInAllDefinitions(false);
            }
        }
    }
}


/**
 * @brief Update the group (of value type) of the reduced map (= brin) in input of our agent
 */
void AgentInMappingVM::_updateReducedMapValueTypeGroupInInput()
{
    AgentIOPValueTypeGroups::Value globalReducedMapValueTypeGroupInInput = AgentIOPValueTypeGroups::UNKNOWN;

    for (int i = 0; i < _inputsList.count(); i++)
    {
        InputVM* input = _inputsList.at(i);
        if ((input != nullptr) && (input->firstModel() != nullptr))
        {
            if (i == 0) {
                globalReducedMapValueTypeGroupInInput = input->firstModel()->agentIOPValueTypeGroup();
            }
            else {
                if (globalReducedMapValueTypeGroupInInput != input->firstModel()->agentIOPValueTypeGroup())
                {
                    globalReducedMapValueTypeGroupInInput = AgentIOPValueTypeGroups::MIXED;
                    break;
                }
            }
        }
    }
    setreducedMapValueTypeGroupInInput(globalReducedMapValueTypeGroupInInput);
}


/**
 * @brief Update the group (of value type) of the reduced map (= brin) in output of our agent
 */
void AgentInMappingVM::_updateReducedMapValueTypeGroupInOutput()
{
    AgentIOPValueTypeGroups::Value globalReducedMapValueTypeGroupInOutput = AgentIOPValueTypeGroups::UNKNOWN;

    for (int i = 0; i < _outputsList.count(); i++)
    {
        OutputVM* output = _outputsList.at(i);
        if ((output != nullptr) && (output->firstModel() != nullptr)) {
            if (i == 0) {
                globalReducedMapValueTypeGroupInOutput = output->firstModel()->agentIOPValueTypeGroup();
            }
            else {
                if (globalReducedMapValueTypeGroupInOutput != output->firstModel()->agentIOPValueTypeGroup())
                {
                    globalReducedMapValueTypeGroupInOutput = AgentIOPValueTypeGroups::MIXED;
                    break;
                }
            }
        }
    }
    setreducedMapValueTypeGroupInOutput(globalReducedMapValueTypeGroupInOutput);
}


/**
 * @brief Get the temporary link with same names
 * @param inputName
 * @param outputAgentName
 * @param outputName
 * @return
 */
ElementMappingM* AgentInMappingVM::_getTemporaryLink(QString inputName, QString outputAgentName, QString outputName)
{
    for (ElementMappingM* iterator : _temporaryMapping->mappingElements()->toList())
    {
        if ((iterator != nullptr) && (iterator->inputAgent() == _name)
                && (iterator->input() == inputName) && (iterator->outputAgent() == outputAgentName) && (iterator->output() == outputName))
        {
            return iterator;
        }
    }
    return nullptr;
}
