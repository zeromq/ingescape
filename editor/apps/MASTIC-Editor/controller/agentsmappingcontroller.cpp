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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "agentsmappingcontroller.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param modelManager
 * @param parent
 */
AgentsMappingController::AgentsMappingController(MasticModelManager* modelManager, QObject *parent)
    : QObject(parent),
      _isEmptyMapping(true),
      _selectedAgent(NULL),
      _selectedMapBetweenIOP(NULL),
      _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelManager != NULL)
    {   
        // Connect to signal "Count Changed" from the list of agents in mapping
        connect(&_agentInMappingVMList, &AbstractI2CustomItemListModel::countChanged, this, &AgentsMappingController::_onAgentsInMappingChanged);
    }
}


/**
 * @brief Destructor
 */
AgentsMappingController::~AgentsMappingController()
{
    // Clean-up current selection
    setselectedAgent(NULL);
    setselectedMapBetweenIOP(NULL);

    // DIS-connect from signal "Count Changed" from the list of agents in mapping
    disconnect(&_agentInMappingVMList, 0, this, 0);

    // Clear the previous list
    _previousListOfAgentsInMapping.clear();

    //Clea all list
    _allMapInMapping.deleteAllItems();
    _allPartialMapInMapping.deleteAllItems();
    _agentInMappingVMList.deleteAllItems();

    _modelManager = NULL;
}


/**
 * @brief Remove the agent from the mapping
 * @param agent
 */
void AgentsMappingController::removeAgentFromMapping(AgentInMappingVM* agent)
{
    // FIXME TODO ? une seule méthode est nécessaire entre:
    // - celle-ci: removeAgentFromMapping
    // - _deleteAgentInMapping

    if (agent != NULL) {
        qInfo() << "Delete Agen in Mapping" << agent->name();

        // Delete an Agent in Mapping
        _deleteAgentInMapping(agent);
    }
}


/**
 * @brief Remove a link between two agents from the mapping
 * @param link
 */
void AgentsMappingController::deleteLinkBetweenTwoAgents(MapBetweenIOPVM* link)
{
    if ((link != NULL) && (link->agentFrom() != NULL) && (link->agentTo() != NULL)) {
        qInfo() << "Delete Link Between agents" << link->agentFrom()->name() << "and" << link->agentTo()->name();

        if (_selectedMapBetweenIOP == link) {
            setselectedMapBetweenIOP(NULL);
        }

        // Remove from the list
        _allMapInMapping.remove(link);

        // Free memory
        delete link;
    }
}


/**
 * @brief Slot when an agent from the list is dropped on the current mapping at a position
 * @param agentName
 * @param list
 * @param position
 */
void AgentsMappingController::dropAgentToMappingAtPosition(QString agentName, AbstractI2CustomItemListModel* list, QPointF position)
{
    // Check that there is NOT yet an agent in the current mapping for this name
    AgentInMappingVM* agentInMapping = getAgentInMappingForName(agentName);
    if (agentInMapping == NULL)
    {
        I2CustomItemListModel<AgentM>* agentsList = dynamic_cast<I2CustomItemListModel<AgentM>*>(list);
        if (agentsList != NULL)
        {
            // Add new model(s) of agent to the current mapping at a specific position
            _addAgentModelsToMappingAtPosition(agentName, agentsList->toList(), position);

            // Selects this new agent
            agentInMapping = getAgentInMappingForName(agentName);
            if (agentInMapping != NULL) {
                setselectedAgent(agentInMapping);
            }
        }
    }
}


/**
 * @brief Slot when a previous agent model is replaced by a new one strictly identical
 * @param previousModel
 * @param newModel
 */
void AgentsMappingController::onIdenticalAgentModelReplaced(AgentM* previousModel, AgentM* newModel)
{
    if ((previousModel != NULL) && (newModel != NULL))
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingForName(newModel->name());
        if (agentInMapping != NULL)
        {
            int index = agentInMapping->models()->indexOf(previousModel);
            if (index > -1) {
                agentInMapping->models()->replace(index, newModel);
            }
        }
    }
}


/**
 * @brief Slot when an identical agent model is added
 * @param newModel
 */
void AgentsMappingController::onIdenticalAgentModelAdded(AgentM* newModel)
{
    if (newModel != NULL)
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingForName(newModel->name());
        if (agentInMapping != NULL)
        {
            agentInMapping->models()->append(newModel);
        }
    }
}


/**
 * @brief Slot when a link from an output is dropped over an input on the current mapping (or when a link to an input is dropped over an output)
 * @param outputAgent
 * @param output
 * @param inputAgent
 * @param input
 */
void AgentsMappingController::addMapBetweenAgents(AgentInMappingVM* outputAgent, OutputVM* output, AgentInMappingVM* inputAgent, InputVM* input)
{
    if ((outputAgent != NULL) && (output != NULL)
            && (inputAgent != NULL) && (input != NULL))
    {
        if (output->canLinkWith(input)) {
            // Create a new map between agents
            MapBetweenIOPVM* mapBetweenIOP = new MapBetweenIOPVM(outputAgent, output, inputAgent, input, this);

            // Add to the list
            _allMapInMapping.append(mapBetweenIOP);

            // Emit signal "Command asked to agent about Mapping Input"
            Q_EMIT commandAskedToAgentAboutMappingInput(inputAgent->getPeerIdsList(), "MAP", input->name(), outputAgent->name(), output->name());
        }
        else {
            if ((output->firstModel() != NULL) && (input->firstModel() != NULL)) {
                qDebug() << "Can not link output" << output->name() << "with type" << AgentIOPValueTypes::staticEnumToString(output->firstModel()->agentIOPValueType()) << "(of agent" << outputAgent->name() << ")"
                         << "and input" << input->name() << "with type" << AgentIOPValueTypes::staticEnumToString(input->firstModel()->agentIOPValueType()) << "(of agent" << inputAgent->name() << ")";
            }
        }
    }
}


/**
 * @brief Slot when the flag "is Activated Mapping" changed
 * @param isActivatedMapping
 */
void AgentsMappingController::onIsActivatedMappingChanged(bool isActivatedMapping)
{
    if (isActivatedMapping) {
        qInfo() << "Mapping ACTIVATED !";

        if (_isEmptyMapping) {
            qDebug() << "...et il est vide, on tente d'ajouter des choses...";

            if (_modelManager != NULL) {
                // Get the map from agent name to list of active agents
                QHash<QString, QList<AgentM*>> mapFromAgentNameToActiveAgentsList = _modelManager->getMapFromAgentNameToActiveAgentsList();

                foreach (QString agentName, mapFromAgentNameToActiveAgentsList.keys()) {
                    QList<AgentM*> activeAgentsList = mapFromAgentNameToActiveAgentsList.value(agentName);

                    // Add new model(s) of agent to the current mapping
                    _addAgentModelsToMappingAtPosition(agentName, activeAgentsList, QPointF());
                }
            }
        }
        else {
            qDebug() << "...et il n'est PAS vide, on ne fait rien de plus";
        }
    }
    else {
        qInfo() << "Mapping DE-activated !";
    }
}


/**
 * @brief Slot when a model of agent will be deleted
 * @param agent
 */
void AgentsMappingController::onAgentModelWillBeDeleted(AgentM* agent)
{
    if (agent != NULL)
    {
        // Get the agent in mapping for the agent name
        AgentInMappingVM* agentInMapping = getAgentInMappingForName(agent->name());
        if (agentInMapping != NULL)
        {
            // Remove the model
            agentInMapping->models()->remove(agent);
            // TODO ESTIA: Remove MapBetweenIOP  unique à ce model .

            // If it was the last one...
            if (agentInMapping->models()->count() == 0) {
                // ...delete this agent in mapping
                _deleteAgentInMapping(agentInMapping);
            }
        }
    }
}


/**
 * @brief Get the agent in mapping for an agent name
 * @param name
 * @return
 */
AgentInMappingVM* AgentsMappingController::getAgentInMappingForName(QString name)
{
    if (_mapFromNameToAgentInMappingViewModelsList.contains(name)) {
        return _mapFromNameToAgentInMappingViewModelsList.value(name);
    }
    else {
        return NULL;
    }
}

/**
 * @brief Slot when inside an agentInMappingVM, new inputsVM are created.
 *      Check if a map need to be created from the element mapping list in the model manager.
 *      A missing agent is substituted by a ghost agent with same name. > create a partial map.
 *      A missing output is substituted by a ghost output with same name. > create partial map.
 * @param currentAgentInMapping
 * @param inputsListAdded
 */
void AgentsMappingController::_generateAllMapBetweenIopUsingNewlyAddedInputsVM(AgentInMappingVM* currentAgentInMapping, QList<InputVM*> inputsListAdded)
{
    if ((_modelManager != NULL) && (currentAgentInMapping != NULL))
    {
        // All newly generated mapBetweenIOP
        QList<MapBetweenIOPVM *> newMapBetweenIOP;

        // Sublist of elementMapping that conserns the agent mappings in input
        QList<ElementMappingM *> elementsMappingFound = _modelManager->getMergedListOfInputMappingElementsFromAgentName(currentAgentInMapping->name());

        // if empty -> skiped
        if (!elementsMappingFound.isEmpty())
        {
            //Initialize

            // iterate on the list of element mapping
            foreach (ElementMappingM* currentElementMapping, elementsMappingFound)
            {
                //Check if the mapping doesnot already exist.
                if( (currentElementMapping != NULL))
                {
                    // Get the input conserned by the current elementMapping.
                    foreach(InputVM* inputPointVM, inputsListAdded)
                    {
                        if(inputPointVM != NULL)
                        {
                            if(inputPointVM->name() == currentElementMapping->input())
                            {
                                // Search for the output agent based on the current elementMapping
                                AgentInMappingVM* outputAgent = getAgentInMappingForName(currentElementMapping->outputAgent());

                                OutputVM* outputPointVM = NULL;

                                if (outputAgent != NULL)
                                {
                                    // Get the list of view models of output from the output name
                                    QList<OutputVM*> outputsWithSameName = outputAgent->getOutputsListFromName(currentElementMapping->output());
                                    if (outputsWithSameName.count() == 0)
                                    {
                                        // Create a new ghost output (OutputVM) as substitute of the missing output.
                                        outputPointVM = new OutputVM(currentElementMapping->output());
                                    }
                                    else if (outputsWithSameName.count() == 1) {
                                        outputPointVM = outputsWithSameName.first();
                                    }
                                    else {
                                        qWarning() << "There are" << outputsWithSameName.count() << "outputs with the same name" << currentElementMapping->output() << "."
                                                   << "We cannot choose and create the link" << currentElementMapping->outputAgent() << "." << currentElementMapping->output() << "-->" << currentElementMapping->inputAgent() << "." << currentElementMapping->input();
                                    }
                                }
                                else
                                {
                                    // Handle output agent is missing

                                    //Create a new ghost agent (AgentInMappingVM) as substitute of the missing agent.
                                    outputAgent = new AgentInMappingVM(currentElementMapping->outputAgent());

                                    //Create a new ghost output (OutputVM) as substitute of the missing output.
                                    outputPointVM = new OutputVM(currentElementMapping->output());
                                }

                                if ((outputAgent != NULL) && (outputPointVM != NULL))
                                {
                                    // Create the new MapBetweenIOP ...
                                    if( (!outputAgent->isGhost()) && (!outputPointVM->isGhost()) )
                                    {
                                        // Handle everything went smoothly i.e. no ghost output nor ghost agent.
                                        if(!_checkIfMapBetweenIOPVMAlreadyExist(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM))
                                        {
                                            MapBetweenIOPVM* map = new MapBetweenIOPVM(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM);

                                            //Add the new MapBetweenIOP to the temp list.
                                            newMapBetweenIOP.append(map);

                                            qInfo() << "Create the MapBetweenIOPVM : " << currentAgentInMapping->name() << "." << inputPointVM->name() << " -> " << outputAgent->name() << "." << outputPointVM->name();
                                        }
                                        else {
                                            qDebug() << "MapBetweenIOPVM already exist : " << currentAgentInMapping->name() << "." << inputPointVM->name() << " -> " << outputAgent->name() << "." << outputPointVM->name();
                                        }
                                    }
                                    else
                                    {
                                        // Handle the new partial MapBetweenIOPVM
                                        if(!_checkIfMapBetweenIOPVMAlreadyExist(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM))
                                        {
                                            MapBetweenIOPVM* partialMap = new MapBetweenIOPVM(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM);

                                            //Add the new MapBetweenIOP to the temp list.
                                            _allPartialMapInMapping.append(partialMap);

                                            //Map partial mapBetweenIOP with output agent name to active search
                                            _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insertMulti(outputAgent->name(), partialMap);

                                            qInfo() << "Create the partial MapBetweenIOPVM : " << currentAgentInMapping->name() << "." << inputPointVM->name() << " -> " << outputAgent->name() << "." << outputPointVM->name();
                                        }
                                        else {
                                            qDebug() << "Partial MapBetweenIOPVM already exist : " << currentAgentInMapping->name() << "." << inputPointVM->name() << " -> " << outputAgent->name() << "." << outputPointVM->name();
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            qWarning() << "Input " << currentElementMapping->input() << "is missing while creating MapBetweenIOPVM";
                        }
                    }
                }
            }
        }

         //Append all new mapBetweenIOP.
        if (!newMapBetweenIOP.isEmpty())
        {
            _allMapInMapping.append(newMapBetweenIOP);
        }
    }
}

/**
 * @brief Slot when inside an agentInMappingVM, new outputsVM are created.
 *      Check if a map need can be completed from internal partial maps list in the mapping controller.
 *      A missing agent is substituted by a ghost agent with same name. > create a partial map.
 *      A missing output is substituted by a ghost output with same name. > create partial map.
 * @param currentAgentInMapping
 * @param outputsListAdded
 */
void AgentsMappingController::_completeAllPartialMapBetweenIopUsingNewlyOutputsVM(AgentInMappingVM* currentAgentInMapping, QList<OutputVM*> outputsListAdded)
{
    if(currentAgentInMapping != NULL)
    {
        // All newly generated mapBetweenIOP
        QList<MapBetweenIOPVM *> newMapBetweenIOP;

        if (_mapFromAgentNameToPartialMapBetweenIOPViewModelsList.contains(currentAgentInMapping->name()))
        {
            // Sublist of partial maps where the ghost agent is matching with the new agent in mapping VM. (Outputs that were needed to realise other agents' mappings)
            QList<MapBetweenIOPVM*> foundPartialMapBetweenIOP  = _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.values(currentAgentInMapping->name());

            foreach (MapBetweenIOPVM* partialMap, foundPartialMapBetweenIOP)
            {
                // Get the real Output from the name of the ghost Output used to create the mapBetweenIOP.
                foreach (OutputVM* missingOutput, outputsListAdded)
                {
                    if (missingOutput != NULL)
                    {
                        if(missingOutput->name() == partialMap->pointFrom()->name())
                        {
                            //Set the ghost elements in the partial map with the real stuff.
                            if (partialMap->agentFrom()->isGhost())
                            {
                                // Get our ghost agent
                                AgentInMappingVM* agentToDestroy = partialMap->agentFrom();

                                //Set real agent
                                partialMap->setagentFrom(currentAgentInMapping);

                                // Destroy ghost agent
                                delete agentToDestroy;
                                agentToDestroy = NULL;
                            }
                            if (partialMap->pointFrom()->isGhost())
                            {
                                // Get our ghost output.
                                OutputVM* outputToDestroy = partialMap->pointFrom();

                                //Set real output
                                partialMap->setpointFrom(missingOutput);

                                // Destroy output
                                delete outputToDestroy;
                                outputToDestroy = NULL;
                            }
                            //Add newly finalised MapBetweenIOP.
                            newMapBetweenIOP.append(partialMap);

                            _allPartialMapInMapping.remove(partialMap);
                            qInfo() <<"Delete Partial Map; " << currentAgentInMapping->name() << "." << missingOutput->name();
                        }
                    }
                }
            }
        }

        //Append all new mapBetweenIOP.
        if (!newMapBetweenIOP.isEmpty())
        {
           _allMapInMapping.append(newMapBetweenIOP);
        }
    }
}


/**
 * @brief Slot when the list of "Agents in Mapping" changed
 */
void AgentsMappingController::_onAgentsInMappingChanged()
{
    // Update the flag "is Empty Mapping"
    if (_agentInMappingVMList.count() == 0) {
        setisEmptyMapping(true);
    }
    else {
        setisEmptyMapping(false);
    }

    QList<AgentInMappingVM*> newListOfAgentsInMapping = _agentInMappingVMList.toList();

    // Agent in Mapping added
    if (_previousListOfAgentsInMapping.count() < newListOfAgentsInMapping.count())
    {
        qDebug() << _previousListOfAgentsInMapping.count() << "--> Agent in Mapping ADDED --> " << newListOfAgentsInMapping.count();

        for (AgentInMappingVM* agentInMapping : newListOfAgentsInMapping) {
            if ((agentInMapping != NULL) && !_previousListOfAgentsInMapping.contains(agentInMapping))
            {
                qDebug() << "Agent in mapping" << agentInMapping->name() << "ADDED";

                // Connect to signals from the new agent in mapping
                connect(agentInMapping, &AgentInMappingVM::inputsListAdded, this, &AgentsMappingController::_onInputsListAdded);
                connect(agentInMapping, &AgentInMappingVM::outputsListAdded, this, &AgentsMappingController::_onOutputsListAdded);
                //connect(agentInMapping, &AgentInMappingVM::outputsListRemoved, this, &AgentsMappingController::_onOutputsListRemoved);
                connect(agentInMapping, &AgentInMappingVM::inputModelsFilled, this, &AgentsMappingController::_onInputModelsFilled);
                connect(agentInMapping, &AgentInMappingVM::outputModelsFilled, this, &AgentsMappingController::_onOutputModelsFilled);
                connect(agentInMapping, &AgentInMappingVM::inputModelsCleared, this, &AgentsMappingController::_onInputModelsCleared);
                connect(agentInMapping, &AgentInMappingVM::outputModelsCleared, this, &AgentsMappingController::_onOutputModelsCleared);

                // Emit signals "Inputs/Outputs List Added" for initial list of inputs and initial list of outputs
                agentInMapping->inputsListAdded(agentInMapping->inputsList()->toList());
                agentInMapping->outputsListAdded(agentInMapping->outputsList()->toList());

                // Emit the signal "Agent in Mapping Added"
                Q_EMIT agentInMappingAdded(agentInMapping->name());
            }
        }
    }
    // Agent in Mapping removed
    else if (_previousListOfAgentsInMapping.count() > newListOfAgentsInMapping.count())
    {
        qDebug() << _previousListOfAgentsInMapping.count() << "--> Agent in Mapping REMOVED --> " << newListOfAgentsInMapping.count();

        for (AgentInMappingVM* agentInMapping : _previousListOfAgentsInMapping) {
            if ((agentInMapping != NULL) && !newListOfAgentsInMapping.contains(agentInMapping))
            {
                qDebug() << "Agent in mapping" << agentInMapping->name() << "REMOVED";

                // Emit signal "Outputs List Removed" with current list of outputs
                agentInMapping->outputsListRemoved(agentInMapping->outputsList()->toList());

                // DIS-connect to signals from the previous agent in mapping
                disconnect(agentInMapping, &AgentInMappingVM::inputsListAdded, this, &AgentsMappingController::_onInputsListAdded);
                disconnect(agentInMapping, &AgentInMappingVM::outputsListAdded, this, &AgentsMappingController::_onOutputsListAdded);
                //disconnect(agentInMapping, &AgentInMappingVM::outputsListRemoved, this, &AgentsMappingController::_onOutputsListRemoved);
                disconnect(agentInMapping, &AgentInMappingVM::inputModelsFilled, this, &AgentsMappingController::_onInputModelsFilled);
                disconnect(agentInMapping, &AgentInMappingVM::outputModelsFilled, this, &AgentsMappingController::_onOutputModelsFilled);
                disconnect(agentInMapping, &AgentInMappingVM::inputModelsCleared, this, &AgentsMappingController::_onInputModelsCleared);
                disconnect(agentInMapping, &AgentInMappingVM::outputModelsCleared, this, &AgentsMappingController::_onOutputModelsCleared);

                // Emit the signal "Agent in Mapping Removed"
                Q_EMIT agentInMappingRemoved(agentInMapping->name());
            }
        }
    }

    _previousListOfAgentsInMapping = newListOfAgentsInMapping;
}


/**
 * @brief Slot when some view models of inputs have been added to an agent in mapping
 * @param inputsListAdded
 */
void AgentsMappingController::_onInputsListAdded(QList<InputVM*> inputsListAdded)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && (inputsListAdded.count() > 0))
    {
        qDebug() << "_on Inputs List Added from agent" << agentInMapping->name() << inputsListAdded.count();

        _generateAllMapBetweenIopUsingNewlyAddedInputsVM(agentInMapping, inputsListAdded);
    }
}


/**
 * @brief Slot when some view models of outputs have been added to an agent in mapping
 * @param outputsListAdded
 */
void AgentsMappingController::_onOutputsListAdded(QList<OutputVM*> outputsListAdded)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && (outputsListAdded.count() > 0))
    {
        qDebug() << "_on Outputs List Added from agent" << agentInMapping->name() << outputsListAdded.count();

        _completeAllPartialMapBetweenIopUsingNewlyOutputsVM(agentInMapping ,outputsListAdded);
    }
}


/**
 * @brief Slot when some view models of outputs will be removed from an agent in mapping
 * @param outputsListWillBeRemoved
 */
/*void AgentsMappingController::_onOutputsListRemoved(QList<OutputVM*> outputsListWillBeRemoved)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && (outputsListWillBeRemoved.count() > 0))
    {
        qDebug() << "_on Outputs List Removed from agent" << agentInMapping->name() << outputsListWillBeRemoved.count();
    }
}*/


/**
 * @brief Slot when the list of models of an input view model is filled (0 model -> 1 model)
 * @param inputName
 */
void AgentsMappingController::_onInputModelsFilled(QString inputName)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if (agentInMapping != NULL)
    {
        qDebug() << "_on Input Models Filled" << agentInMapping->name() << "." << inputName;
        // TODO ESTIA
    }
}


/**
 * @brief Slot when the list of models of an output view model is filled (0 model -> 1 model)
 * @param outputName
 */
void AgentsMappingController::_onOutputModelsFilled(QString outputName)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if (agentInMapping != NULL)
    {
        qDebug() << "_on Output Models Filled" << agentInMapping->name() << "." << outputName;
        // TODO ESTIA
    }
}


/**
 * @brief Slot when the list of models of an input view model is cleared (1 model -> 0 model)
 * @param inputName
 */
void AgentsMappingController::_onInputModelsCleared(QString inputName)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if (agentInMapping != NULL)
    {
        qDebug() << "_on Input Models Cleared" << agentInMapping->name() << "." << inputName;
        // TODO ESTIA
    }
}


/**
 * @brief Slot when the list of models of an output view model is cleared (1 model -> 0 model)
 * @param outputName
 */
void AgentsMappingController::_onOutputModelsCleared(QString outputName)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if (agentInMapping != NULL)
    {
        qDebug() << "_on Output Models Cleared" << agentInMapping->name() << "." << outputName;
        // TODO ESTIA
    }
}


/**
 * @brief Add new model(s) of agent to the current mapping at a specific position
 * @param agentName
 * @param agentsList
 * @param position
 */
void AgentsMappingController::_addAgentModelsToMappingAtPosition(QString agentName, QList<AgentM*> agentsList, QPointF position)
{
    if (!agentName.isEmpty() && (agentsList.count() > 0))
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingForName(agentName);

        // The agent is defined, only add models of agent in the internal list of the agentInMappingVM
        if (agentInMapping != NULL)
        {   
            foreach (AgentM* model, agentsList) {
                if (model != NULL) {
                    agentInMapping->models()->append(model);
                }
            }

            qInfo() << "The agent in mapping already exist, new agent models added to" << agentName;
        }
        // Instanciate a new view model of agent in mapping (and add models)
        else
        {
            // Create a new Agent In Mapping
            agentInMapping = new AgentInMappingVM(agentsList,
                                                  position,
                                                  this);

            // Add in the map list
            _mapFromNameToAgentInMappingViewModelsList.insert(agentName, agentInMapping);

            // Add this new Agent In Mapping VM in the list for the qml
            _agentInMappingVMList.append(agentInMapping);

            qInfo() << "A new agent mapping has been added:" << agentName;
        }
    }
}


/**
 * @brief Delete an Agent in Mapping
 * @param agentInMapping
 */
void AgentsMappingController::_deleteAgentInMapping(AgentInMappingVM* agentInMapping)
{
    if (agentInMapping != NULL)
    {
        qInfo() << "An agent mapping has been removed:" << agentInMapping->name();

        // Unselect our agent if needed
        if (_selectedAgent == agentInMapping) {
            setselectedAgent(NULL);
        }

        // Remove from the map list
        _mapFromNameToAgentInMappingViewModelsList.remove(agentInMapping->name());

        _deleteAllMappingMadeOnTargetAgent(agentInMapping);

        // Remove this Agent In Mapping VM from the list for the qml
        _agentInMappingVMList.remove(agentInMapping);

        // Free memory
        delete agentInMapping;
    }
}

/**
 * @brief Deletes all the mapBetweenIOPVM where agent in paramater is involved.
 * @param agentInMapping
 */
void AgentsMappingController::_deleteAllMappingMadeOnTargetAgent(AgentInMappingVM* agentInMapping)
{
    if(agentInMapping != NULL)
    {
         // Check among the list of all map
         if(!_allMapInMapping.isEmpty())
         {
             // Temporary Partial Maps
             QList<MapBetweenIOPVM*> newListOfPartialMap;

             foreach(MapBetweenIOPVM* mapBetweenIOP, _allMapInMapping.toList())
             {
                 if(mapBetweenIOP != NULL){
                     if(mapBetweenIOP->agentFrom()->name() == agentInMapping->name())
                     {
                         // if involved as agentFrom:
                         // 1- remove from the list of all the map
                         _allMapInMapping.remove(mapBetweenIOP);

                         // 2 - Transform the mapping in a partial map and add to the list of all the map.
                         // Create ghost output and agent
                         OutputVM* ghostOutput = new OutputVM(mapBetweenIOP->pointFrom()->name());
                         AgentInMappingVM* ghostAgent = new AgentInMappingVM(mapBetweenIOP->agentFrom()->name());

                         // Edit map to be partial
                         mapBetweenIOP->setagentFrom(ghostAgent);
                         mapBetweenIOP->setpointFrom(ghostOutput);

                         qInfo() << "Turn a MapBetweenIOPVM into a the partial MapBetweenIOPVM : " << mapBetweenIOP->agentTo()->name() << "." << mapBetweenIOP->pointTo()->name() << " -> " << ghostAgent->name() << "." << ghostOutput->name();
                         //Add to Hash and to temp partial maps list
                         _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insertMulti(ghostOutput->name(), mapBetweenIOP);
                         newListOfPartialMap.append(mapBetweenIOP);

                     }
                     else if(mapBetweenIOP->agentTo()->name() == agentInMapping->name())
                     {
                         // if involved as agentTo:
                         // Just destroy it.
                         _allMapInMapping.remove(mapBetweenIOP);
                         delete(mapBetweenIOP);
                     }
                 }
             }

             // Push partialMaps into internal list
             _allPartialMapInMapping.append(newListOfPartialMap);
         }

         // Check among the list of all partial map
         if(!_allPartialMapInMapping.isEmpty())
         {
             foreach (MapBetweenIOPVM* partialMapBetweenIOP, _allPartialMapInMapping.toList()) {
                 if(partialMapBetweenIOP != NULL){
                     if(partialMapBetweenIOP->agentTo()->name() == agentInMapping->name())
                     {
                         // If involved as agentTo
                         _allMapInMapping.remove(partialMapBetweenIOP);
                         delete(partialMapBetweenIOP);
                     }
                 }
             }
         }
    }
}

/**
 * @brief Check if the map between an agent output and an agent input already exist.
 * @param agentFrom
 * @param pointFrom
 * @param agentTo
 * @param pointTo
 * @return
 */
bool AgentsMappingController::_checkIfMapBetweenIOPVMAlreadyExist(AgentInMappingVM* agentFrom, OutputVM *pointFrom, AgentInMappingVM* agentTo, InputVM *pointTo)
{
    if(pointFrom->isGhost() || agentFrom->isGhost())     // Partial Map
    {
        if(!_allPartialMapInMapping.isEmpty())
        {
             foreach(MapBetweenIOPVM* partialMap, _allPartialMapInMapping.toList())
             {
                if(partialMap != NULL)
                {
                   // pointsTo have same name && pointsFrom have same name
                   if( (partialMap->pointTo()->name() == pointTo->name()) && (partialMap->pointFrom()->name() == pointFrom->name()) )
                   {
                       // agentsTo have same name && agentsFrom have same name
                       if( (partialMap->agentTo()->name() == agentTo->name()) && (partialMap->agentFrom()->name() == agentFrom->name()) )
                       {
                           // Exactly same so must return true
                           return true;
                       }
                   }
                }
             }
        }
    }
    else
    {
        if(!_allMapInMapping.isEmpty())     // Complete Map
        {
            foreach(MapBetweenIOPVM* map, _allMapInMapping.toList())
            {
                if(map != NULL)
                {
                    if( !map->pointFrom()->id().isEmpty() && !map->pointTo()->id().isEmpty() )
                    {
                        if ( (map->pointTo()->id() == pointTo->id()) && (map->pointFrom()->id() == pointFrom->id()) )
                        {
                            // pointsTo have same name && pointsFrom have same name
                            if((map->agentTo()->name() == agentTo->name()) && (map->agentFrom()->name() == agentFrom->name()) )
                            {
                                // Exactly same so must return true
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

