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
 * @param agentModelList. The first agent is needed to instanciate an agent mapping VM.
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

            foreach (AgentM* model, models) {
                if (model != NULL) {
                    addAgentToInternalList(model);

                    // Add to the list
                    //_models.append(model);
                }
            }
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
 * @brief Add agent dynamically to the internal list
 * @param newAgent The definition to add
 */
void AgentInMappingVM::addAgentToInternalList(AgentM* newAgentM)
{
    //Check if agent model and agent in mapping share the same name
    if(newAgentM->name() == _agentName)
    {
        // First model of agent to be ever submitted.
        if (_models.count() == 0)
        {
            qInfo() << "Add initial agent model.";

            // Create the list of input (PointMapVM)
            _addPointMapInInternalInputList(newAgentM->definition());

            // Create the list of output (PointMapVM)
            _addPointMapInInternalOutputList(newAgentM->definition());

            // Pull the creation of newMapBetweenIOPVM...
            Q_EMIT newDefinitionInAgentMapping(this);
        }
        else // Another model of agent is provided. Must show "patte blanche".
        {
            //Check if agentM has same definition as the first agent of internal agentList().
            if(!DefinitionM::areIdenticals(_models.toList().first()->definition(), newAgentM->definition()))
            {
                // Should handle the difference by creating the missing INPUT / OUTPUT. More work is done at their creation.
                qInfo() << "Add initial agent model but defintion is different.";

                // Create the list of input (PointMapVM)
                _addPointMapInInternalInputList(newAgentM->definition());

                // Create the list of output (PointMapVM)
                _addPointMapInInternalOutputList(newAgentM->definition());

                // Pull the creation of newMapBetweenIOPVM...
                Q_EMIT newDefinitionInAgentMapping(this);
            }
        }

        //Add new agent model
        _models.append(newAgentM);

    }
    else {
        qInfo()<<"The agent model named '" << _agentName << "' could not be add to the agent mapping VM named '" << newAgentM->name() << "'";
    }
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
                //qDebug() << "New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals from a model
                connect(model, &AgentM::isONChanged, this, &AgentInMappingVM::_onIsONofModelChanged);
                //connect(model, &AgentM::definitionChanged, this, &AgentInMappingVM::_onDefinitionOfModelChanged);
                //connect(model, &AgentM::mappingChanged, this, &AgentInMappingVM::_onMappingOfModelChanged);
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
                //qDebug() << "Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals from a model
                disconnect(model, &AgentM::isONChanged, this, &AgentInMappingVM::_onIsONofModelChanged);
                //disconnect(model, &AgentM::definitionChanged, this, &AgentInMappingVM::_onDefinitionOfModelChanged);
                //disconnect(model, &AgentM::mappingChanged, this, &AgentInMappingVM::_onMappingOfModelChanged);
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
     * @brief Add new points Map to the inputs list from a definition model
     * @param newDefinition The definition model
     */
void AgentInMappingVM::_addPointMapInInternalInputList(DefinitionM *newDefinition)
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
}


/**
     * @brief Add new points Map to the outputs list from a definition model
     * @param newDefinition The definition model
     */
void AgentInMappingVM::_addPointMapInInternalOutputList(DefinitionM *newDefinition)
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
}


/**
     * @brief This function check if the InputVM already exist in the input list
     * @param currentInput The newly created Input VM
     */
bool AgentInMappingVM::_checkIfAlreadyInInputList(InputVM* currentInput)
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
}


/**
     * @brief This function check if the OutputVM already exist in the input list
     * @param currentOuput The newly created OutputVM
     */
bool AgentInMappingVM::_checkIfAlreadyInOutputList(OutputVM* currentOuput)
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
}


/**
 * @brief Update with all models of agents
 */
void AgentInMappingVM::_updateWithAllModels()
{
    /*foreach (AgentM* model, _models.toList()) {
        if (model != NULL)
        {
        }
    }*/

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
