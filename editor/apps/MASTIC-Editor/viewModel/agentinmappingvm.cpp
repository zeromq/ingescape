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
    _agentName(""),
    _position(position),
    _isON(false),
    _isReduced(false),
    _reducedMapValueTypeInInput(AgentIOPValueTypes::MIXED),
    _reducedMapValueTypeInOutput(AgentIOPValueTypes::MIXED),
    _isGhost(false),
    _areIdenticalsAllDefinitions(true)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (models.count() > 0)
    {
        AgentM* firstModel = models.first();
        if (firstModel != NULL)
        {
            // Set the name of our agent in mapping
            _agentName = firstModel->name();

            // Connect to signal "Count Changed" from the list of models
            connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onModelsChanged);

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
 * @param agentName
 * @param parent
 */
AgentInMappingVM::AgentInMappingVM(QString agentName,
                                   QObject *parent) : AgentInMappingVM(QList<AgentM*>(),
                                                                       QPointF(),
                                                                       parent)
{
    setagentName(agentName);
    setisGhost(true);

    qInfo() << "New Ghost of Agent in Mapping" << _agentName;
}


/**
 * @brief Destructor
 */
AgentInMappingVM::~AgentInMappingVM()
{
    qInfo() << "Delete View Model of Agent in Mapping" << _agentName;

    // Clear maps of Inputs & Outputs
    _mapOfInputsFromInputName.clear();
    _mapOfOutputsFromOutputName.clear();

    // Delete elements in the lists of Inputs & Outputs
    _inputsList.deleteAllItems();
    _outputsList.deleteAllItems();

    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentInMappingVM::_onModelsChanged);

    // Clear the previous list of models
    _previousAgentsList.clear();

    // Clear the list of definition
    _models.clear();
}


/**
 * @brief Return the corresponding view model of input from the input name
 * @param inputName
 */
InputVM* AgentInMappingVM::getInputFromName(QString inputName)
{
    if (_mapOfInputsFromInputName.contains(inputName)) {
        return _mapOfInputsFromInputName.value(inputName);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Return the corresponding view model of input from the input id
 * @param inputId
 */
InputVM* AgentInMappingVM::getInputFromId(QString inputId)
{
    if (_mapOfInputsFromInputId.contains(inputId)) {
        return _mapOfInputsFromInputId.value(inputId);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Return the corresponding view model of output from the output name
 * @param outputName
 */
OutputVM* AgentInMappingVM::getOutputFromName(QString outputName)
{
    if (_mapOfOutputsFromOutputName.contains(outputName)) {
        return _mapOfOutputsFromOutputName.value(outputName);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Return the corresponding view model of output from the output id
 * @param outputId
 */
OutputVM* AgentInMappingVM::getOutputFromId(QString outputId)
{
    if (_mapOfOutputsFromOutputId.contains(outputId)) {
        return _mapOfOutputsFromOutputId.value(outputId);
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
        qDebug() << _previousAgentsList.count() << "--> ADD --> " << newAgentsList.count();

        for (AgentM* model : newAgentsList) {
            if ((model != NULL) && !_previousAgentsList.contains(model))
            {
                qDebug() << "New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals from a model
                connect(model, &AgentM::isONChanged, this, &AgentInMappingVM::_onIsONofModelChanged);
                //connect(model, &AgentM::definitionChanged, this, &AgentInMappingVM::_onDefinitionOfModelChanged);
                //connect(model, &AgentM::mappingChanged, this, &AgentInMappingVM::_onMappingOfModelChanged);

                // A model of agent has been added to our list
                _agentModelAdded(model);
            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList) {
            if ((model != NULL) && !newAgentsList.contains(model))
            {
                qDebug() << "Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals from a model
                disconnect(model, &AgentM::isONChanged, this, &AgentInMappingVM::_onIsONofModelChanged);
                //disconnect(model, &AgentM::definitionChanged, this, &AgentInMappingVM::_onDefinitionOfModelChanged);
                //disconnect(model, &AgentM::mappingChanged, this, &AgentInMappingVM::_onMappingOfModelChanged);

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
            InputVM* newInputVM = _inputModelAdded(input);
            if (newInputVM != NULL) {
                inputsListToAdd.append(newInputVM);
            }
        }

        // Traverse the list of models of outputs in the definition
        foreach (OutputM* output, model->definition()->outputsList()->toList())
        {
            OutputVM* newOutputVM = _outputModelAdded(output);
            if (newOutputVM != NULL) {
                outputsListToAdd.append(newOutputVM);
            }
        }

        _inputsList.append(inputsListToAdd);
        _outputsList.append(outputsListToAdd);

        // Emit signal "Inputs List Added"
        Q_EMIT inputsListAdded(inputsListToAdd);

        // Emit signal "Outputs List Added"
        Q_EMIT outputsListAdded(outputsListToAdd);
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
        // Traverse the list of models of inputs in the definition
        foreach (AgentIOPM* input, model->definition()->inputsList()->toList())
        {
            InputVM* inputVM = _inputModelRemoved(input);
            Q_UNUSED(inputVM)

            // Usefull ?
            /*if ((inputVM != NULL) && (inputVM->models()->count() == 0)) {
                _inputsList.remove(inputVM);
            }*/
        }

        // Traverse the list of models of outputs in the definition
        foreach (OutputM* output, model->definition()->outputsList()->toList())
        {
            OutputVM* outputVM = _outputModelRemoved(output);
            Q_UNUSED(outputVM)

            // Usefull ?
            /*if ((outputVM != NULL) && (outputVM->models()->count() == 0)) {
                _outputsList.remove(outputVM);
            }*/
        }
    }
}


/**
 * @brief A model of input has been added
 * @param input
 * @return
 */
InputVM* AgentInMappingVM::_inputModelAdded(AgentIOPM* input)
{
    InputVM* newInputVM = NULL;

    if (input != NULL)
    {
        // Input id is defined
        if (!input->id().isEmpty())
        {
            InputVM* inputVM = getInputFromId(input->id());
            if (inputVM == NULL)
            {
                inputVM = getInputFromName(input->name());
                if (inputVM == NULL)
                {
                    // Create a new view model of input
                    newInputVM = new InputVM(input->name(),
                                             input->id(),
                                             input,
                                             this);

                    // Don't add to the list here (this input will be added globally via temporary list)

                    // Update the hash table with input id
                    _mapOfInputsFromInputId.insert(newInputVM->id(), newInputVM);

                    // Update the hash table with input name
                    _mapOfInputsFromInputName.insert(newInputVM->name(), newInputVM);
                }
                // View model of input exists for this name (but not yet id)
                else {
                    // FIXME TODO: gestion des ghost...
                }
            }
            else {
                // Add this new model to the list
                inputVM->models()->append(input);
            }
        }
        // Input id is NOT defined
        else {
            // FIXME TODO: gestion des ghost...
        }
    }

    return newInputVM;
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
    OutputVM* newOutputVM = NULL;

    if (output != NULL)
    {
        // Input id is defined
        if (!output->id().isEmpty())
        {
            OutputVM* outputVM = getOutputFromId(output->id());
            if (outputVM == NULL)
            {
                outputVM = getOutputFromName(output->name());
                if (outputVM == NULL)
                {
                    // Create a new view model of output
                    newOutputVM = new OutputVM(output->name(),
                                               output->id(),
                                               output,
                                               this);

                    // Don't add to the list here (this output will be added globally via temporary list)

                    // Update the hash table with output id
                    _mapOfOutputsFromOutputId.insert(newOutputVM->id(), newOutputVM);

                    // Update the hash table with output name
                    _mapOfOutputsFromOutputName.insert(newOutputVM->name(), newOutputVM);
                }
                // View model of output exists for this name (but not yet id)
                else {
                    // FIXME TODO: gestion des ghost...
                }
            }
            else {
                // Add this new model to the list
                outputVM->models()->append(output);
            }
        }
        // Output id is NOT defined
        else {
            // FIXME TODO: gestion des ghost...
        }
    }

    return newOutputVM;
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
        // Input id is defined
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
     * @brief Add new points Map to the inputs list from a definition model
     * @param newDefinition The definition model
     */
/*void AgentInMappingVM::_addPointMapInInternalInputList(DefinitionM *newDefinition)
{
    if(newDefinition != NULL)
    {
        // Create the list of Point Map to add in the agent in mapping
        QList<InputVM*> listOfPointMapTemp;

        foreach (AgentIOPM* iopM, newDefinition->inputsList()->toList())
        {
            if (iopM != NULL)
            {
                // New pointMapVM (from agent and input model
                InputVM* inputPoint = new InputVM(iopM->name(), iopM, this);

                //Check if it's not alreafy exist in the list & add it
                if(!_checkIfAlreadyInInputList(inputPoint))
                {
                    // Add to the list of newly created pointMap
                    listOfPointMapTemp.append(inputPoint);

                    // Update the hash table
                    _mapOfInputsFromInputName.insert(iopM->name(), inputPoint);

                    qInfo()<<"Add new point map " << _agentName << "." << iopM->name() << " in the inputs list.";
                }
                else {
                    qInfo() << "Point map : " << _agentName << "." << iopM->name() << " is already present in the list.";
                }
            }
        }
        _inputsList.append(listOfPointMapTemp);
    }
}*/


/**
     * @brief Add new points Map to the outputs list from a definition model
     * @param newDefinition The definition model
     */
/*void AgentInMappingVM::_addPointMapInInternalOutputList(DefinitionM *newDefinition)
{
    if(newDefinition != NULL)
    {
        // Create the list of Point Map to add in the agent in mapping
        QList<OutputVM*> listOfPointMapTemp;

        foreach (OutputM* iopM, newDefinition->outputsList()->toList())
        {
            if (iopM != NULL)
            {
                // New pointMapVM (from agent and input model
                OutputVM* outputPoint = new OutputVM(iopM->name(), iopM, this);

                //Check if it's not alreafy exist in the list & add it
                if(!_checkIfAlreadyInOutputList(outputPoint))
                {
                    // Add to the list of newly created pointMap
                    listOfPointMapTemp.append(outputPoint);

                    // Update the hash table
                    _mapOfOutputsFromOutputName.insert(iopM->name(), outputPoint);

                    qInfo()<<"Add new point map " << _agentName << "." << iopM->name() << " in the inputs list.";
                }
                else {
                    qInfo() << "Point map : " << _agentName << "." << iopM->name() << " is already present in the list.";
                }
            }
        }
        _outputsList.append(listOfPointMapTemp);
    }
}*/


/**
     * @brief This function check if the InputVM already exist in the input list
     * @param currentInput The newly created Input VM
     */
/*bool AgentInMappingVM::_checkIfAlreadyInInputList(InputVM* currentInput)
{
    foreach (InputVM* iterator, _inputsList.toList())
    {
        if( (currentInput != NULL) && (iterator != NULL))
        {
            if(currentInput->modelM() != NULL && iterator->modelM() != NULL )
            {
                if( currentInput->modelM()->id() == iterator->modelM()->id() )
                {
                    // Exactly the same point
                    return true;
                }
            }
        }
    }
    return false;
}*/


/**
     * @brief This function check if the OutputVM already exist in the input list
     * @param currentOuput The newly created OutputVM
     */
/*bool AgentInMappingVM::_checkIfAlreadyInOutputList(OutputVM* currentOuput)
{
    foreach (OutputVM* iterator, _outputsList.toList())
    {
        if( (currentOuput != NULL) && (iterator != NULL))
        {
            if(currentOuput->modelM() != NULL && iterator->modelM() != NULL )
            {
                if( currentOuput->modelM()->id() == iterator->modelM()->id() )
                {
                    // Exactly the same point
                    return true;
                }
            }
        }
    }
    return false;
}*/


/**
 * @brief Update with all models of agents
 */
void AgentInMappingVM::_updateWithAllModels()
{
    bool areIdenticalsAllDefinitions = true;

    if (_models.count() > 1)
    {
        QList<AgentM*> modelsList = _models.toList();

        AgentM* firstModel = modelsList.at(0);
        DefinitionM* firstDefinition = NULL;

        if ((firstModel != NULL) && (firstModel->definition() != NULL))
        {
            firstDefinition = firstModel->definition();

            for (int i = 1; i < modelsList.count(); i++) {
                AgentM* model = modelsList.at(i);

                if ((model != NULL) && (model->definition() != NULL))
                {
                    if (!DefinitionM::areIdenticals(firstDefinition, model->definition())) {
                        areIdenticalsAllDefinitions = false;
                        break;
                    }
                }
            }
        }
    }
    setareIdenticalsAllDefinitions(areIdenticalsAllDefinitions);

    // Update flags in function of models
    _updateIsON();
}


/**
 * @brief Update the flag "is ON" in function of flags of models
 */
void AgentInMappingVM::_updateIsON()
{
    bool globalIsON = false;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && model->isON()) {
            globalIsON = true;
            break;
        }
    }
    setisON(globalIsON);
}
