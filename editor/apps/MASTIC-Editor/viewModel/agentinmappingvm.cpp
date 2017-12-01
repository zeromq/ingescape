/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
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
 * @brief Default constructor
 * @param models. The first agent is needed to instanciate an agent mapping VM.
 * Typically passing during the drag-drop from the list of agents on the left side.
 * @param position Position of the top left corner
 * @param parent
 */
AgentInMappingVM::AgentInMappingVM(QList<AgentM*> models,
                                   QPointF position,
                                   QObject *parent) : QObject(parent),
    _name(""),
    _position(position),
    _isON(false),
    _isReduced(false),
    _reducedMapValueTypeGroupInInput(AgentIOPValueTypeGroups::MIXED),
    _reducedMapValueTypeGroupInOutput(AgentIOPValueTypeGroups::MIXED),
    _isGhost(false),
    _areIdenticalsAllDefinitions(true),
    _activeAgentsNumber(0),
    _temporaryMapping(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (models.count() > 0)
    {
        AgentM* firstModel = models.first();
        if (firstModel != NULL)
        {
            // Set the name of our agent in mapping
            _name = firstModel->name();


            // Create the mapping currently edited
            QString mappingName = QString("Mapping name of %1 in MASTIC Editor").arg(_name);
            QString mappingDescription = QString("Mapping description of %1 in MASTIC Editor").arg(_name);
            _temporaryMapping = new AgentMappingM(mappingName, "0.0", mappingDescription);


            // Connect to signal "Count Changed" from the list of models
            connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onModelsChanged);
            //connect(&_inputsList, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onInputsListChanged);
            //connect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onOutputsListChanged);

            // Initialize our list
            _models.append(models);
        }
        else {
            qCritical() << "No agent model for the agent in mapping !";
        }
    }
}


/**
 * @brief Ghost Constructor: model (and definition) is not defined.
 * The agent is an empty shell only defined by a name.
 * @param name
 * @param parent
 */
AgentInMappingVM::AgentInMappingVM(QString name,
                                   QObject *parent) : AgentInMappingVM(QList<AgentM*>(),
                                                                       QPointF(),
                                                                       parent)
{
    setname(name);
    setisGhost(true);

    qInfo() << "New Ghost of Agent in Mapping" << _name;
}


/**
 * @brief Destructor
 */
AgentInMappingVM::~AgentInMappingVM()
{
    qInfo() << "Delete View Model of Agent in Mapping" << _name;

    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onModelsChanged);
    //disconnect(&_inputsList, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onInputsListChanged);
    //disconnect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onOutputsListChanged);

    if (_temporaryMapping != NULL) {
        delete _temporaryMapping;
    }

    // Clear maps of Inputs & Outputs
    _mapFromNameToInputsList.clear();
    _mapFromUniqueIdToInput.clear();
    _mapFromNameToOutputsList.clear();
    _mapFromUniqueIdToOutput.clear();

    // Delete elements in the lists of Inputs & Outputs
    _inputsList.deleteAllItems();
    _outputsList.deleteAllItems();

    // Clear the previous list of models
    _previousAgentsList.clear();

    // Clear the list of definition
    _models.clear();
}


/**
 * @brief Get the list of peer ids of our models
 * @return
 */
QStringList AgentInMappingVM::getPeerIdsList()
{
    return _peerIdsList;
}


/**
 * @brief Add a temporary link (this temporary link will became a real link when the user will activate the mapping)
 * @param inputName
 * @param outputAgentName
 * @param outputName
 */
void AgentInMappingVM::addTemporaryLink(QString inputName, QString outputAgentName, QString outputName)
{
    if (_temporaryMapping != NULL)
    {
        // Check that there is not already the same link
        ElementMappingM* temporaryLink = _getTemporaryLink(inputName, outputAgentName, outputName);
        if (temporaryLink == NULL)
        {
            temporaryLink = new ElementMappingM(_name, inputName, outputAgentName, outputName);

            _temporaryMapping->elementMappingsList()->append(temporaryLink);
        }
    }
}


/**
 * @brief Remove temporary link (this temporary link will be removed when the user will activate the mapping)
 * @param inputName
 * @param outputAgentName
 * @param outputName
 */
void AgentInMappingVM::removeTemporaryLink(QString inputName, QString outputAgentName, QString outputName)
{
    if (_temporaryMapping != NULL)
    {
        // Check that there is not already the same link
        ElementMappingM* temporaryLink = _getTemporaryLink(inputName, outputAgentName, outputName);
        if (temporaryLink != NULL)
        {
            _temporaryMapping->elementMappingsList()->remove(temporaryLink);
        }
    }
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
 * @brief Slot when the list of models changed
 */
void AgentInMappingVM::_onModelsChanged()
{
    QList<AgentM*> newAgentsList = _models.toList();

    // Model of agent added
    if (_previousAgentsList.count() < newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> ADD --> " << newAgentsList.count();

        for (AgentM* model : newAgentsList) {
            if ((model != NULL) && !_previousAgentsList.contains(model))
            {
                qDebug() << "Agent in Mapping VM: New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals from a model
                connect(model, &AgentM::isONChanged, this, &AgentInMappingVM::_onIsONofModelChanged);
                //connect(model, &AgentM::definitionChanged, this, &AgentInMappingVM::_onDefinitionOfModelChanged);

                // A model of agent has been added to our list
                _agentModelAdded(model);
            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList) {
            if ((model != NULL) && !newAgentsList.contains(model))
            {
                qDebug() << "Agent in Mapping VM: Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals from a model
                disconnect(model, &AgentM::isONChanged, this, &AgentInMappingVM::_onIsONofModelChanged);
                //disconnect(model, &AgentM::definitionChanged, this, &AgentInMappingVM::_onDefinitionOfModelChanged);

                // A model of agent has been removed from our list
                _agentModelRemoved(model);
            }
        }
    }

    _previousAgentsList = newAgentsList;

    // Update with all models
    _updateWithAllModels();
}


/**
 * @brief Slot when the list of (view models of) inputs changed
 */
/*void AgentInMappingVM::_onInputsListChanged()
{
    foreach (InputVM* input, _inputsList.toList()) {
        if ((input != NULL) && (input->firstModel() != NULL)) {
        }
    }
}*/


/**
 * @brief Slot when the list of (view models of) outputs changed
 */
/*void AgentInMappingVM::_onOutputsListChanged()
{
    foreach (OutputVM* output, _outputsList.toList()) {
        if ((output != NULL) && (output->firstModel() != NULL)) {
        }
    }
}*/


/**
 * @brief Slot when the flag "is ON" of a model changed
 * @param isON
 */
void AgentInMappingVM::_onIsONofModelChanged(bool isON)
{
    Q_UNUSED(isON)

    // Update the flag "is ON" in function of flags of all models
    _updateIsON();
}


/**
 * @brief A model of agent has been added to our list
 * @param model
 */
void AgentInMappingVM::_agentModelAdded(AgentM* model)
{
    if ((model != NULL) && (model->definition() != NULL))
    {
        QList<InputVM*> inputsListToAdd;
        QList<OutputVM*> outputsListToAdd;

        // Traverse the list of models of inputs in the definition
        foreach (AgentIOPM* input, model->definition()->inputsList()->toList())
        {
            InputVM* inputVM = _inputModelAdded(input);
            if (inputVM != NULL)
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
            if (outputVM != NULL)
            {
                // New view model of output
                if (!_outputsList.contains(outputVM)) {
                    outputsListToAdd.append(outputVM);
                }
            }
        }

        if (inputsListToAdd.count() > 0) {
            _inputsList.append(inputsListToAdd);

            // Emit signal "Inputs List Added"
            Q_EMIT inputsListAdded(inputsListToAdd);
        }

        if (outputsListToAdd.count() > 0) {
            _outputsList.append(outputsListToAdd);

            // Emit signal "Outputs List Added"
            Q_EMIT outputsListAdded(outputsListToAdd);
        }

        // Emit signal "models of Inputs and Outputs Changed"
        Q_EMIT modelsOfInputsAndOutputsChanged();
    }
}


/**
 * @brief A model of agent has been removed from our list
 * @param model
 */
void AgentInMappingVM::_agentModelRemoved(AgentM* model)
{
    if ((model != NULL) && (model->definition() != NULL))
    {
        QList<InputVM*> inputsListToRemove;
        QList<OutputVM*> outputsListToRemove;

        // Traverse the list of models of inputs in the definition
        foreach (AgentIOPM* input, model->definition()->inputsList()->toList())
        {
            InputVM* inputVM = _inputModelRemoved(input);
            if (inputVM != NULL)
            {
                // The view model of input is empty
                if (inputVM->models()->count() == 0) {
                    inputsListToRemove.append(inputVM);
                }
            }
        }

        // Traverse the list of models of outputs in the definition
        foreach (OutputM* output, model->definition()->outputsList()->toList())
        {
            OutputVM* outputVM = _outputModelRemoved(output);
            if (outputVM != NULL)
            {
                // The view model of output is empty
                if (outputVM->models()->count() == 0) {
                    outputsListToRemove.append(outputVM);
                }
            }
        }

        if (inputsListToRemove.count() > 0) {
            // Emit signal "Inputs List Will Be Removed"
            Q_EMIT inputsListWillBeRemoved(inputsListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_inputsList.remove(inputsListToRemove);
            foreach (InputVM* inputVM, inputsListToRemove) {
                _inputsList.remove(inputVM);
            }
        }

        if (outputsListToRemove.count() > 0) {
            // Emit signal "Outputs List Will Be Removed"
            Q_EMIT outputsListWillBeRemoved(outputsListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_outputsList.remove(outputsListToRemove);
            foreach (OutputVM* outputVM, outputsListToRemove) {
                _outputsList.remove(outputVM);
            }
        }

        // Emit signal "models of Inputs and Outputs Changed"
        Q_EMIT modelsOfInputsAndOutputsChanged();
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

    if (input != NULL)
    {
        // First, we get a ghost of this input: an input without id (only the same name)
        QList<InputVM*> inputsWithSameName = getInputsListFromName(input->name());
        InputVM* inputWithoutId = NULL;

        foreach (InputVM* iterator, inputsWithSameName)
        {
            // Already a view model with an EMPTY id (NOT defined)
            if ((iterator != NULL) && iterator->id().isEmpty()) {
                inputWithoutId = iterator;
                break;
            }
        }

        // Input id is NOT defined
        if (input->id().isEmpty())
        {
            // There is already a view model without id
            if (inputWithoutId != NULL)
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
            if (inputWithoutId != NULL)
            {
                // FIXME TODO: gestion du ghost...passage en view model avec id
            }

            inputVM = getInputFromId(input->id());

            // There is already a view model for this id
            if (inputVM != NULL)
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

    if (input != NULL)
    {
        // Input id is defined
        if (!input->id().isEmpty())
        {
            inputVM = getInputFromId(input->id());
            if (inputVM != NULL)
            {
                // Remove this model from the list
                inputVM->models()->remove(input);
            }
            /*else
            {
                inputVM = getInputFromName(input->name());
                if (inputVM != NULL)
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

    if (output != NULL)
    {
        // First, we get a ghost of this output: an output without id (only the same name)
        QList<OutputVM*> outputsWithSameName = getOutputsListFromName(output->name());
        OutputVM* outputWithoutId = NULL;

        foreach (OutputVM* iterator, outputsWithSameName)
        {
            // Already a view model with an EMPTY id (NOT defined)
            if ((iterator != NULL) && iterator->id().isEmpty()) {
                outputWithoutId = iterator;
                break;
            }
        }

        // Output id is NOT defined
        if (output->id().isEmpty())
        {
            // There is already a view model without id
            if (outputWithoutId != NULL)
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
            if (outputWithoutId != NULL)
            {
                // FIXME TODO: gestion du ghost...passage en view model avec id
            }

            outputVM = getOutputFromId(output->id());

            // There is already a view model for this id
            if (outputVM != NULL)
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

    if (output != NULL)
    {
        // Output id is defined
        if (!output->id().isEmpty())
        {
            outputVM = getOutputFromId(output->id());
            if (outputVM != NULL)
            {
                // Remove this model from the list
                outputVM->models()->remove(output);
            }
            /*else
            {
                outputVM = getOutputFromName(output->name());
                if (outputVM != NULL)
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
 * @brief Update with all models of agents
 */
void AgentInMappingVM::_updateWithAllModels()
{
    _peerIdsList.clear();
    bool areIdenticalsAllDefinitions = true;

    if (_models.count() > 0)
    {
        QList<AgentM*> modelsList = _models.toList();
        DefinitionM* firstDefinition = NULL;

        for (int i = 0; i < modelsList.count(); i++)
        {
            AgentM* model = modelsList.at(i);
            if (model != NULL)
            {
                if (!model->peerId().isEmpty()) {
                    _peerIdsList.append(model->peerId());
                }

                if (i == 0) {
                    firstDefinition = model->definition();
                }
                else if ((firstDefinition != NULL) && (model->definition() != NULL)
                         && !DefinitionM::areIdenticals(firstDefinition, model->definition())) {
                    areIdenticalsAllDefinitions = false;
                }
            }
        }
    }
    setareIdenticalsAllDefinitions(areIdenticalsAllDefinitions);

    // Update flags in function of models
    _updateIsON();

    // Update the group (of value type) of the reduced map (= brin) in input and in output of our agent
    _updateReducedMapValueTypeGroupInInput();
    _updateReducedMapValueTypeGroupInOutput();
}


/**
 * @brief Update the flag "is ON" in function of flags of models
 */
void AgentInMappingVM::_updateIsON()
{
    bool globalIsON = false;
    int activeAgentsNumber = 0;

    foreach (AgentM* model, _models.toList())
    {
        if ((model != NULL) && model->isON())
        {
            globalIsON = true;
            //break;
            activeAgentsNumber++;
        }
    }

    setisON(globalIsON);
    setactiveAgentsNumber(activeAgentsNumber);
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
        if ((input != NULL) && (input->firstModel() != NULL)) {
            if (i == 0) {
                globalReducedMapValueTypeGroupInInput = input->firstModel()->agentIOPValueTypeGroup();
            }
            else {
                if (globalReducedMapValueTypeGroupInInput != input->firstModel()->agentIOPValueTypeGroup()) {
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
        if ((output != NULL) && (output->firstModel() != NULL)) {
            if (i == 0) {
                globalReducedMapValueTypeGroupInOutput = output->firstModel()->agentIOPValueTypeGroup();
            }
            else {
                if (globalReducedMapValueTypeGroupInOutput != output->firstModel()->agentIOPValueTypeGroup()) {
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
    foreach (ElementMappingM* iterator, _temporaryMapping->elementMappingsList()->toList())
    {
        if ((iterator != NULL) && (iterator->inputAgent() == _name)
                && (iterator->input() == inputName) && (iterator->outputAgent() == outputAgentName) && (iterator->output() == outputName))
        {
            return iterator;
        }
    }
    return NULL;
}
