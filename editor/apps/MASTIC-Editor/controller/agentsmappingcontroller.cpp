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
        }
        else {
            if ((output->firstModel() != NULL) && (input->firstModel() != NULL)) {
                qDebug() << "Can not link output" << output->name() << "with type" << AgentIOPValueTypes::staticEnumToString(output->firstModel()->agentIOPValueType()) << "(of agent" << outputAgent->agentName() << ")"
                         << "and input" << input->name() << "with type" << AgentIOPValueTypes::staticEnumToString(input->firstModel()->agentIOPValueType()) << "(of agent" << inputAgent->agentName() << ")";
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
 * @brief Slot when a new view model of a agent mapping is created on the main view mapping.
 *      Check if a map need to be created from the element mapping list in the model manager.
 *      The two agents corresponding need to be visible in the list.
 * @param currentAgentInMapping
 */
void AgentsMappingController::_onCreateMapBetweenIopInMappingFromAgentInMapping(AgentInMappingVM* currentAgentInMapping)
{
    if ((_modelManager != NULL) && (currentAgentInMapping != NULL))
    {
        // All newly generated mapBetweenIOP
        QList<MapBetweenIOPVM *> newMapBetweenIOP;

        //
        // Handle all mappings that could not be finalised.
        //
        if (_mapFromAgentNameToPartialMapBetweenIOPViewModelsList.contains(currentAgentInMapping->agentName()))
        {
            // Sublist of partial maps where the ghost agent is matching with the new agent in mapping VM. (Outputs that were needed to realise other agents' mappings)
            QList<MapBetweenIOPVM*> foundPartialMapBetweenIOP  = _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.values(currentAgentInMapping->agentName());

            foreach (MapBetweenIOPVM* partialMap, foundPartialMapBetweenIOP)
            {
                // Get the real Output from the name of the ghost Output used to create the mapBetweenIOP.
                OutputVM* missingOutput = currentAgentInMapping->getOutputFromName(partialMap->pointFrom()->name());

                if (missingOutput != NULL)
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
                    qInfo() <<"Delete Partial Map; " << currentAgentInMapping->agentName() << "." << missingOutput->name();
                }

            }

        }

        //
        // Handle all mappings of the current agents.
        // Note: No need to check for empty string from elementMapping.
        //

        // Sublist of elementMapping that conserns the agent mappings in input
        QList<ElementMappingM *> elementsMappingFound = _modelManager->getMergedListOfInputMappingElementsFromAgentName(currentAgentInMapping->agentName());

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
                    InputVM* inputPointVM = currentAgentInMapping->getInputFromName(currentElementMapping->input());

                    if(inputPointVM != NULL)
                    {
                        // Search for the output agent based on the current elementMapping
                        AgentInMappingVM* outputAgent = getAgentInMappingForName(currentElementMapping->outputAgent());

                        OutputVM* outputPointVM = NULL;

                        if(outputAgent != NULL)
                        {
                            // Get the output conserned by the mapping.
                            outputPointVM = outputAgent->getOutputFromName(currentElementMapping->output());

                            if(outputPointVM == NULL)
                            {
                               // Handle Output is missing

                               //Create a new ghost output (OutputVM) as substitute of the missing output.
                               outputPointVM = new OutputVM(currentElementMapping->output());
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

                        // Create the new MapBetweenIOP ...
                        if( (!outputAgent->isGhost()) && (!outputPointVM->isGhost()) )
                        {
                            // Handle everything went smoothly i.e. no ghost output nor ghost agent.
                            if(!_checkIfMapBetweenIOPVMAlreadyExist(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM))
                            {
                                MapBetweenIOPVM* map = new MapBetweenIOPVM(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM);

                                //Add the new MapBetweenIOP to the temp list.
                                newMapBetweenIOP.append(map);

                                qInfo() << "Create the MapBetweenIOPVM : " << currentAgentInMapping->agentName() << "." << inputPointVM->name() << " -> " << outputAgent->agentName() << "." << outputPointVM->name();
                            }
                            else
                            {
                                qInfo() << "MapBetweenIOPVM already exist : " << currentAgentInMapping->agentName() << "." << inputPointVM->name() << " -> " << outputAgent->agentName() << "." << outputPointVM->name();
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
                                _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insertMulti(outputAgent->agentName(), partialMap);

                                qInfo() << "Create the partial MapBetweenIOPVM : " << currentAgentInMapping->agentName() << "." << inputPointVM->name() << " -> " << outputAgent->agentName() << "." << outputPointVM->name();
                            }
                            else
                            {
                                qInfo() << "Partial MapBetweenIOPVM already exist : " << currentAgentInMapping->agentName() << "." << inputPointVM->name() << " -> " << outputAgent->agentName() << "." << outputPointVM->name();
                            }
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
                qDebug() << "Agent in mapping" << agentInMapping->agentName() << "ADDED";

                // Connect to signals from the new agent in mapping
                connect(agentInMapping, &AgentInMappingVM::inputsListAdded, this, &AgentsMappingController::_onInputsListAdded);
                connect(agentInMapping, &AgentInMappingVM::outputsListAdded, this, &AgentsMappingController::_onOutputsListAdded);
                connect(agentInMapping, &AgentInMappingVM::outputsListRemoved, this, &AgentsMappingController::_onOutputsListRemoved);

                // Emit signals "Inputs/Outputs List Added" for initial list of inputs and initial list of outputs
                agentInMapping->inputsListAdded(agentInMapping->inputsList()->toList());
                agentInMapping->outputsListAdded(agentInMapping->outputsList()->toList());
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
                qDebug() << "Agent in mapping" << agentInMapping->agentName() << "REMOVED";

                // Emit signal "Outputs List Removed" with current list of outputs
                agentInMapping->outputsListRemoved(agentInMapping->outputsList()->toList());

                // DIS-connect to signals from the previous agent in mapping
                disconnect(agentInMapping, &AgentInMappingVM::inputsListAdded, this, &AgentsMappingController::_onInputsListAdded);
                disconnect(agentInMapping, &AgentInMappingVM::outputsListAdded, this, &AgentsMappingController::_onOutputsListAdded);
                disconnect(agentInMapping, &AgentInMappingVM::outputsListRemoved, this, &AgentsMappingController::_onOutputsListRemoved);
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
    if ((agentInMapping != NULL) && (inputsListAdded.count() > 0)) {
        qDebug() << "_on Inputs List Added from agent" << agentInMapping->agentName() << inputsListAdded.count();

        // FIXME TODO ESTIA: appel à _onCreateMapBetweenIopInMappingFromAgentInMapping en l'adaptant en fonction des paramètres (agentInMapping->agentName() et inputsListAdded ?)
    }
}


/**
 * @brief Slot when some view models of outputs have been added to an agent in mapping
 * @param outputsListAdded
 */
void AgentsMappingController::_onOutputsListAdded(QList<OutputVM*> outputsListAdded)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && (outputsListAdded.count() > 0)) {
        qDebug() << "_on Outputs List Added from agent" << agentInMapping->agentName() << outputsListAdded.count();

        // FIXME TODO ESTIA: appel à _onCreateMapBetweenIopInMappingFromAgentInMapping en l'adaptant en fonction des paramètres (agentInMapping->agentName() et outputsListAdded ?)

        QList<OutputM*> models;
        foreach (OutputVM* output, outputsListAdded) {
            if ((output != NULL) && (output->firstModel() != NULL)) {
                models.append(output->firstModel());
            }
        }
        if (models.count() > 0) {
            // Emit signal "Add Inputs to Editor for Outputs"
            Q_EMIT addInputsToEditorForOutputs(agentInMapping->agentName(), models);
        }
    }
}


/**
 * @brief Slot when some view models of outputs will be removed from an agent in mapping
 * @param outputsListWillBeRemoved
 */
void AgentsMappingController::_onOutputsListRemoved(QList<OutputVM*> outputsListWillBeRemoved)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && (outputsListWillBeRemoved.count() > 0)) {
        qDebug() << "_on Outputs List Removed from agent" << agentInMapping->agentName() << outputsListWillBeRemoved.count();

        QList<OutputM*> models;
        foreach (OutputVM* output, outputsListWillBeRemoved) {
            if ((output != NULL) && (output->firstModel() != NULL)) {
                models.append(output->firstModel());
            }
        }
        if (models.count() > 0) {
            // Emit signal "Remove Inputs to Editor for Outputs"
            Q_EMIT removeInputsToEditorForOutputs(agentInMapping->agentName(), models);
        }
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
                       if( (partialMap->agentTo()->agentName() == agentTo->agentName()) && (partialMap->agentFrom()->agentName() == agentFrom->agentName()) )
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
                            if((map->agentTo()->agentName() == agentTo->agentName()) && (map->agentFrom()->agentName() == agentFrom->agentName()) )
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

