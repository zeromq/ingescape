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

    // Clear all lists
    _allMapInMapping.deleteAllItems();
    _allPartialMapInMapping.deleteAllItems();
    _agentInMappingVMList.deleteAllItems();

    _modelManager = NULL;
}


/**
 * @brief Create a new Mapping
 */
void AgentsMappingController::createNewMapping()
{
    qInfo() << "Clear current (previous) mapping";

    // Remove all agents from the mapping and delete them
    foreach (AgentInMappingVM* agent, _agentInMappingVMList.toList()) {
        if (agent != NULL) {
            deleteAgentInMapping(agent);
        }
    }

    if (_modelManager != NULL) {
        _modelManager->setisActivatedMapping(false);
    }

    qInfo() << "Create a new (empty) Mapping";
}


/**
 * @brief Remove the agent from the mapping and delete it
 * @param agent
 */
void AgentsMappingController::deleteAgentInMapping(AgentInMappingVM* agent)
{
    if (agent != NULL)
    {
        qInfo() << "Delete Agent in Mapping" << agent->name();

        // Unselect our agent if needed
        if (_selectedAgent == agent) {
            setselectedAgent(NULL);
        }

        // Remove from hash table
        _mapFromNameToAgentInMapping.remove(agent->name());

        //_deleteAllMappingMadeOnTargetAgent(agent);

        // Remove all the links with this agent
        _removeAllLinksWithAgent(agent);

        // Remove this Agent In Mapping from the list to update view (QML)
        _agentInMappingVMList.remove(agent);

        // Free memory
        delete agent;
    }
}


/**
 * @brief Remove a link between two agents from the mapping
 * @param link
 */
void AgentsMappingController::removeLinkBetweenTwoAgents(MapBetweenIOPVM* link)
{
    if (link != NULL)
    {
        if ((link->agentFrom() != NULL) && (link->agentTo() != NULL)) {
            qInfo() << "QML asked to delete the link between agents" << link->agentFrom()->name() << "and" << link->agentTo()->name();
        }

        if (link->isVirtual())
        {
            // Delete the link between two agents
            _deleteLinkBetweenTwoAgents(link);
        }
        else
        {
            // Set to virtual to give a feedback to the user
            link->setisVirtual(true);

            // Mapping is activated
            if ((_modelManager != NULL) && _modelManager->isActivatedMapping())
            {
                if ((link->agentFrom() != NULL) && (link->agentTo() != NULL) && (link->pointFrom() != NULL) && (link->pointTo() != NULL))
                {
                    // Emit signal "Command asked to agent about Mapping Input"
                    Q_EMIT commandAskedToAgentAboutMappingInput(link->agentTo()->getPeerIdsList(), "UNMAP", link->pointTo()->name(), link->agentFrom()->name(), link->pointFrom()->name());
                }
            }
        }
    }
}


/**
 * @brief Called when an agent from the list is dropped on the current mapping at a position
 * @param agentName
 * @param models
 * @param position
 */
void AgentsMappingController::dropAgentToMappingAtPosition(QString agentName, AbstractI2CustomItemListModel* models, QPointF position)
{
    // Check that there is NOT yet an agent in the current mapping for this name
    AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);
    if (agentInMapping == NULL)
    {
        I2CustomItemListModel<AgentM>* agentsList = dynamic_cast<I2CustomItemListModel<AgentM>*>(models);
        if (agentsList != NULL)
        {
            // Add new model(s) of agent to the current mapping at a specific position
            _addAgentModelsToMappingAtPosition(agentName, agentsList->toList(), position);

            agentInMapping = getAgentInMappingFromName(agentName);
            if (agentInMapping != NULL)
            {
                // Mapping is activated
                //if ((_modelManager != NULL) && _modelManager->isActivatedMapping()) {

                //
                foreach (AgentM* model, agentsList->toList()) {
                    if (model != NULL)
                    {
                        // Model is ON
                        if (model->isON())
                        {
                            QStringList peerIdsList;
                            peerIdsList.append(model->peerId());

                            // Emit the signal to send the command "CLEAR_MAPPING" on the network to the agent
                            Q_EMIT commandAskedToAgent(peerIdsList, "CLEAR_MAPPING");
                        }
                        // Model is OFF
                        else {
                            model->setmustClearMapping(true);
                        }
                    }
                }

                // Selects this new agent
                setselectedAgent(agentInMapping);
            }
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
    if ((outputAgent != NULL) && (output != NULL) && (inputAgent != NULL) && (input != NULL))
    {
        // Check that the input can link to the output
        if (input->canLinkWith(output))
        {
            // Search if the same link already exists
            bool alreadyLinked = false;
            foreach (MapBetweenIOPVM* iterator, _allMapInMapping.toList()) {
                if ((iterator != NULL) && (iterator->agentFrom() == outputAgent) && (iterator->pointFrom() == output)
                        && (iterator->agentTo() == inputAgent) && (iterator->pointTo() == input)) {
                    alreadyLinked = true;
                    break;
                }
            }

            // Check that the same link does not yet exist
            if (!alreadyLinked)
            {
                qInfo() << "QML asked to create the link between agents" << outputAgent->name() << "and" << inputAgent->name();

                // Create a new VIRTUAL link between agents
                MapBetweenIOPVM* link = new MapBetweenIOPVM(outputAgent, output, inputAgent, input, true, this);

                // Add to the list
                _allMapInMapping.append(link);

                // Mapping is activated
                if ((_modelManager != NULL) && _modelManager->isActivatedMapping()) {
                    // Emit signal "Command asked to agent about Mapping Input"
                    Q_EMIT commandAskedToAgentAboutMappingInput(inputAgent->getPeerIdsList(), "MAP", input->name(), outputAgent->name(), output->name());
                }
            }
            else {
                qWarning() << "The input" << input->name() << "(of agent" << inputAgent->name() << ") is already linked to output" << output->name() << "(of agent" << outputAgent->name() << ")";
            }
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
 * @brief Get the agent in mapping from an agent name
 * @param name
 * @return
 */
AgentInMappingVM* AgentsMappingController::getAgentInMappingFromName(QString name)
{
    if (_mapFromNameToAgentInMapping.contains(name)) {
        return _mapFromNameToAgentInMapping.value(name);
    }
    else {
        return NULL;
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
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(newModel->name());
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
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(newModel->name());
        if (agentInMapping != NULL)
        {
            agentInMapping->models()->append(newModel);
        }
    }
}


/**
 * @brief Slot when the flag "is Activated Mapping" changed
 * @param isActivatedMapping
 */
void AgentsMappingController::onIsActivatedMappingChanged(bool isActivatedMapping)
{
    if (isActivatedMapping)
    {
        // Empty mapping
        if (_isEmptyMapping)
        {
            qDebug() << "Mapping Activated and it is completely EMPTY, we add agents (and links) on our HMI";

            if (_modelManager != NULL)
            {
                // Get the map from agent name to list of active agents
                QHash<QString, QList<AgentM*>> mapFromAgentNameToActiveAgentsList = _modelManager->getMapFromAgentNameToActiveAgentsList();

                // Initial size of our window: 1920 x 1080
                // Minimal size of our window: 1024 x 768
                // Width of our left panel: 320
                // Height of our bottom panel: 200
                int availableMinWidth = 1920 - 320;
                int availableMinHeight = 1080 - 200;
                double randomMax = (double)RAND_MAX;

                // Create all agents in mapping
                foreach (QString agentName, mapFromAgentNameToActiveAgentsList.keys())
                {
                    QList<AgentM*> activeAgentsList = mapFromAgentNameToActiveAgentsList.value(agentName);

                    double randomX = (double)qrand() / randomMax;
                    double randomY = (double)qrand() / randomMax;
                    QPointF position = QPointF(randomX * availableMinWidth, randomY * availableMinHeight);
                    //qDebug() << "Random position:" << position << "for agent" << agentName << "(" << randomX << randomY << ")";

                    // Add new model(s) of agent to the current mapping
                    _addAgentModelsToMappingAtPosition(agentName, activeAgentsList, position);
                }

                // FIXME: à optimiser...
                // Create all links in mapping
                foreach (AgentInMappingVM* agent, _agentInMappingVMList.toList()) {
                    if (agent != NULL) {
                        foreach (AgentM* model, agent->models()->toList()) {
                            if ((model != NULL) && (model->mapping() != NULL)) {
                                foreach (ElementMappingM* mappingElement, model->mapping()->elementMappingsList()->toList())
                                {
                                    // Simulate slot "on Mapped"
                                    onMapped(mappingElement);
                                }
                            }
                        }
                    }
                }
            }
        }
        // Mapping has some agents (and links)
        else
        {
            qDebug() << "Mapping Activated and there are already some agents (and links) on our HMI";

            // Ask to create all virtual links in mapping
            foreach (MapBetweenIOPVM* link, _allMapInMapping.toList())
            {
                if ((link != NULL) && (link->agentFrom() != NULL) && (link->agentTo() != NULL) && (link->pointFrom() != NULL) && (link->pointTo() != NULL))
                {
                    // MAP or UNMAP ?

                    // Emit signal "Command asked to agent about Mapping Input"
                    Q_EMIT commandAskedToAgentAboutMappingInput(link->agentTo()->getPeerIdsList(), "MAP", link->pointTo()->name(), link->agentFrom()->name(), link->pointFrom()->name());
                }
            }
        }
    }
    //else {
    //}
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
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agent->name());
        if (agentInMapping != NULL)
        {
            // Remove the model
            agentInMapping->models()->remove(agent);
            // TODO ESTIA: Remove MapBetweenIOP  unique à ce model

            // If it was the last one...
            if (agentInMapping->models()->count() == 0)
            {
                // ...delete this agent in mapping
                deleteAgentInMapping(agentInMapping);
            }
        }
    }
}


/**
 * @brief Slot when two agents are mapped
 * @param mappingElement
 */
void AgentsMappingController::onMapped(ElementMappingM* mappingElement)
{
    if (mappingElement != NULL)
    {
        // Try to get the virtual link which corresponds to the mapping element
        MapBetweenIOPVM* link = _getLinkFromMappingElement(mappingElement);

        // A virtual link already exists
        if ((link != NULL) && link->isVirtual())
        {
            // Set this previously virtual link as real link
            link->setisVirtual(false);
        }
        else
        {
            AgentInMappingVM* outputAgent = getAgentInMappingFromName(mappingElement->outputAgent());
            AgentInMappingVM* inputAgent = getAgentInMappingFromName(mappingElement->inputAgent());

            if ((outputAgent != NULL) && (inputAgent != NULL))
            {
                OutputVM* output = NULL;
                InputVM* input = NULL;

                // Get the list of view models of output from the output name
                QList<OutputVM*> outputsWithSameName = outputAgent->getOutputsListFromName(mappingElement->output());
                if (outputsWithSameName.count() == 1) {
                    output = outputsWithSameName.first();
                }
                else {
                    qWarning() << "There are" << outputsWithSameName.count() << "outputs with the same name" << mappingElement->output() << "."
                               << "We cannot choose and create the link" << mappingElement->outputAgent() << "." << mappingElement->output() << "-->" << mappingElement->inputAgent() << "." << mappingElement->input();
                }

                // Get the list of view models of input from the input name
                QList<InputVM*> inputsWithSameName = inputAgent->getInputsListFromName(mappingElement->input());
                if (inputsWithSameName.count() == 1) {
                    input = inputsWithSameName.first();
                }
                else {
                    qWarning() << "There are" << inputsWithSameName.count() << "inputs with the same name" << mappingElement->input() << "."
                               << "We cannot choose and create the link" << mappingElement->outputAgent() << "." << mappingElement->output() << "-->" << mappingElement->inputAgent() << "." << mappingElement->input();
                }

                if ((output != NULL) && (input != NULL))
                {
                    qInfo() << "MAPPED" << mappingElement->id();

                    // Create a new map between agents
                    link = new MapBetweenIOPVM(outputAgent, output, inputAgent, input, false, this);

                    // Add to the list
                    _allMapInMapping.append(link);
                }
            }
        }
    }
}


/**
 * @brief Slot when two agents are unmapped
 * @param mappingElement
 */
void AgentsMappingController::onUnmapped(ElementMappingM* mappingElement)
{
    if (mappingElement != NULL)
    {
        qInfo() << "UN-mapped" << mappingElement->id();

        // Get the view model of link which corresponds to a mapping element
        MapBetweenIOPVM* link = _getLinkFromMappingElement(mappingElement);
        if (link != NULL)
        {
            // Delete the link between two agents
            _deleteLinkBetweenTwoAgents(link);
        }
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
/*void AgentsMappingController::_generateAllMapBetweenIopUsingNewlyAddedInputsVM(AgentInMappingVM* currentAgentInMapping, QList<InputVM*> inputsListAdded)
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
                                AgentInMappingVM* outputAgent = getAgentInMappingFromName(currentElementMapping->outputAgent());

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
                                    if ( !outputAgent->isGhost() && !outputPointVM->isGhost())
                                    {
                                        // Handle everything went smoothly i.e. no ghost output nor ghost agent.
                                        if(!_checkIfMapBetweenIOPVMAlreadyExist(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM))
                                        {
                                            // FIXME virtual ?
                                            MapBetweenIOPVM* map = new MapBetweenIOPVM(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM, false);

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
                                        if (!_checkIfMapBetweenIOPVMAlreadyExist(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM))
                                        {
                                            // FIXME virtual ?
                                            MapBetweenIOPVM* partialMap = new MapBetweenIOPVM(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM, false);

                                            //Add the new MapBetweenIOP to the temp list.
                                            _allPartialMapInMapping.append(partialMap);

                                            //Map partial mapBetweenIOP with output agent name to active search
                                            QList<MapBetweenIOPVM*> listOfMapUnderKey;
                                            if(_mapFromAgentNameToPartialMapBetweenIOPViewModelsList.contains(outputAgent->name()))
                                            {
                                                listOfMapUnderKey = _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.value((outputAgent->name()));
                                            }
                                            listOfMapUnderKey.append(partialMap);
                                            _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insert(outputAgent->name(), listOfMapUnderKey);

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
}*/


/**
 * @brief Slot when inside an agentInMappingVM, new outputsVM are created.
 *      Check if a map need can be completed from internal partial maps list in the mapping controller.
 *      A missing agent is substituted by a ghost agent with same name. > create a partial map.
 *      A missing output is substituted by a ghost output with same name. > create partial map.
 * @param currentAgentInMapping
 * @param outputsListAdded
 */
/*void AgentsMappingController::_completeAllPartialMapBetweenIopUsingNewlyOutputsVM(AgentInMappingVM* currentAgentInMapping, QList<OutputVM*> outputsListAdded)
{
    if(currentAgentInMapping != NULL)
    {
        // All newly generated mapBetweenIOP
        QList<MapBetweenIOPVM *> newMapBetweenIOP;

        if (_mapFromAgentNameToPartialMapBetweenIOPViewModelsList.contains(currentAgentInMapping->name()))
        {
            // Sublist of partial maps where the ghost agent is matching with the new agent in mapping VM. (Outputs that were needed to realise other agents' mappings)
            QList<MapBetweenIOPVM*> foundPartialMapBetweenIOP  = _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.value(currentAgentInMapping->name());
            _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.remove(currentAgentInMapping->name());

            foreach (MapBetweenIOPVM* partialMap, foundPartialMapBetweenIOP)
            {
                // Get the real Output from the name of the ghost Output used to create the mapBetweenIOP.
                foreach (OutputVM* missingOutput, outputsListAdded)
                {
                    if (missingOutput != NULL)
                    {
                        if(missingOutput->name() == partialMap->pointFrom()->name())
                        {
                            //Delete the output from the list since it has already been dealt with.
                             outputsListAdded.removeOne(missingOutput);

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

                            // Get our ghost output.
                            OutputVM* outputToDestroy = partialMap->pointFrom();

                            //Set real output
                            partialMap->setpointFrom(missingOutput);

                            // Destroy output
                            delete outputToDestroy;
                            outputToDestroy = NULL;

                            //Add newly finalised MapBetweenIOP.
                            newMapBetweenIOP.append(partialMap);

                            //Delete ex-partial map from temp list.
                            foundPartialMapBetweenIOP.removeOne(partialMap);

                            //Delete ex-partial map from internal list.
                            _allPartialMapInMapping.remove(partialMap);
                            qInfo() <<"Delete Partial Map; " << currentAgentInMapping->name() << "." << missingOutput->name();
                        }
                    }
                }
            }
            if(!foundPartialMapBetweenIOP.isEmpty())
            {
                //if all the partial could not be handled then insert them back in the hash table.
                _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insert(currentAgentInMapping->name(), foundPartialMapBetweenIOP);
            }
        }

        //Append all new mapBetweenIOP.
        if (!newMapBetweenIOP.isEmpty())
        {
           _allMapInMapping.append(newMapBetweenIOP);
        }
    }
}*/


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
        //qDebug() << _previousListOfAgentsInMapping.count() << "--> Agent in Mapping ADDED --> " << newListOfAgentsInMapping.count();

        for (AgentInMappingVM* agentInMapping : newListOfAgentsInMapping) {
            if ((agentInMapping != NULL) && !_previousListOfAgentsInMapping.contains(agentInMapping))
            {
                qDebug() << "Agents Mapping Controller: Agent in mapping" << agentInMapping->name() << "ADDED";

                // Emit the signal "Agent in Mapping Added"
                Q_EMIT agentInMappingAdded(agentInMapping);

                // Connect to signals from the new agent in mapping
                connect(agentInMapping, &AgentInMappingVM::inputsListAdded, this, &AgentsMappingController::_onInputsListAdded);
                connect(agentInMapping, &AgentInMappingVM::outputsListAdded, this, &AgentsMappingController::_onOutputsListAdded);
                connect(agentInMapping, &AgentInMappingVM::inputsListWillBeRemoved, this, &AgentsMappingController::_onInputsListWillBeRemoved);
                connect(agentInMapping, &AgentInMappingVM::outputsListWillBeRemoved, this, &AgentsMappingController::_onOutputsListWillBeRemoved);

                // Emit signals "Inputs/Outputs List Added" for initial list of inputs and initial list of outputs
                agentInMapping->inputsListAdded(agentInMapping->inputsList()->toList());
                agentInMapping->outputsListAdded(agentInMapping->outputsList()->toList());
            }
        }
    }
    // Agent in Mapping removed
    else if (_previousListOfAgentsInMapping.count() > newListOfAgentsInMapping.count())
    {
        //qDebug() << _previousListOfAgentsInMapping.count() << "--> Agent in Mapping REMOVED --> " << newListOfAgentsInMapping.count();

        for (AgentInMappingVM* agentInMapping : _previousListOfAgentsInMapping) {
            if ((agentInMapping != NULL) && !newListOfAgentsInMapping.contains(agentInMapping))
            {
                qDebug() << "Agents Mapping Controller: Agent in mapping" << agentInMapping->name() << "REMOVED";

                // Emit the signal "Agent in Mapping Removed"
                Q_EMIT agentInMappingRemoved(agentInMapping);

                // Emit signals "Inputs/Outputs List Removed" with current list of inputs and current list of outputs
                agentInMapping->inputsListWillBeRemoved(agentInMapping->inputsList()->toList());
                agentInMapping->outputsListWillBeRemoved(agentInMapping->outputsList()->toList());

                // DIS-connect to signals from the previous agent in mapping
                disconnect(agentInMapping, &AgentInMappingVM::inputsListAdded, this, &AgentsMappingController::_onInputsListAdded);
                disconnect(agentInMapping, &AgentInMappingVM::outputsListAdded, this, &AgentsMappingController::_onOutputsListAdded);
                disconnect(agentInMapping, &AgentInMappingVM::inputsListWillBeRemoved, this, &AgentsMappingController::_onInputsListWillBeRemoved);
                disconnect(agentInMapping, &AgentInMappingVM::outputsListWillBeRemoved, this, &AgentsMappingController::_onOutputsListWillBeRemoved);
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

        //_generateAllMapBetweenIopUsingNewlyAddedInputsVM(agentInMapping, inputsListAdded);
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

        //_completeAllPartialMapBetweenIopUsingNewlyOutputsVM(agentInMapping ,outputsListAdded);
    }
}


/**
 * @brief Slot when some view models of inputs will be removed from an agent in mapping
 * @param inputsListWillBeRemoved
 */
void AgentsMappingController::_onInputsListWillBeRemoved(QList<InputVM*> inputsListWillBeRemoved)
{
    //TODO later.
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && (inputsListWillBeRemoved.count() > 0))
    {
        qDebug() << "_on Intputs List Will Be Removed from agent" << agentInMapping->name() << inputsListWillBeRemoved.count();

//        foreach(InputVM* removedInput, inputVMs)
//        {
//            foreach(MapBetweenIOPVM* mapBetweenIOP, _allMapInMapping.toList())
//            {
//                if( (mapBetweenIOP != NULL) && (mapBetweenIOP->agentFrom()->name() == agentInMapping->name()) )
//                {
//                    if( (removedInput != NULL) && (mapBetweenIOP->pointTo()->name() == removedInput->name()) )
//                    {
//                        _allMapInMapping.remove(mapBetweenIOP);
//                        inputsListWillBeRemoved.removeAll(removedInput);
//                        delete(mapBetweenIOP);
//                    }
//                }
//            }
//        }
    }
}


/**
 * @brief Slot when some view models of outputs will be removed from an agent in mapping
 * @param outputsListWillBeRemoved
 */
void AgentsMappingController::_onOutputsListWillBeRemoved(QList<OutputVM*> outputsListWillBeRemoved)
{
    //TODO later.
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
     if ((agentInMapping != NULL) && (outputsListWillBeRemoved.count() > 0))
     {
         qDebug() << "_on Outputs List Will Be Removed from agent" << agentInMapping->name() << outputsListWillBeRemoved.count();

 //        QList<MapBetweenIOPVM*> newListOfPartialMap;
 //        foreach(OutputVM* removedOutput, outputVMs)
 //        {
 //            foreach(MapBetweenIOPVM* mapBetweenIOP, _allMapInMapping.toList())
 //            {
 //                if( (mapBetweenIOP != NULL) && (mapBetweenIOP->agentFrom()->name() == agentInMapping->name()) )
 //                {
 //                    if( (removedOutput != NULL) && (mapBetweenIOP->pointFrom()->name() == removedOutput->name()) )
 //                    {
 //                        // if involved as agentFrom:
 //                        // 1- remove from the list of all the map
 //                        _allMapInMapping.remove(mapBetweenIOP);
 //                        outputsListWillBeRemoved(removedOutput);

 //                        // 2 - Transform the mapping in a partial map and add to the list of all the map.
 //                        // Create ghost output and agent
 //                        OutputVM* ghostOutput = new OutputVM(removedOutput->name());
 //                        AgentInMappingVM* ghostAgent = new AgentInMappingVM(agentInMapping->name());

 //                        // Edit map to be partial
 //                        mapBetweenIOP->setagentFrom(ghostAgent);
 //                        mapBetweenIOP->setpointFrom(ghostOutput);

 //                        if(!_checkIfMapBetweenIOPVMAlreadyExist(ghostAgent, ghostOutput, mapBetweenIOP->agentTo(), mapBetweenIOP->pointTo()))
 //                        {
 //                           qInfo() << "Turn a MapBetweenIOPVM into a the partial MapBetweenIOPVM : " << mapBetweenIOP->agentTo()->name() << "." << mapBetweenIOP->pointTo()->name() << " -> " << ghostAgent->name() << "." << ghostOutput->name();
 //                           newListOfPartialMap.append(mapBetweenIOP);
 //                        }
 //                    }
 //                }
 //            }
 //        }
 //        if(!newListOfPartialMap.isEmpty())
 //        {
 //            // Push partialMaps into internal list
 //            _allPartialMapInMapping.append(newListOfPartialMap);

 //            //Add to Hash and to temp partial maps list
 //            QList<MapBetweenIOPVM*> listOfMapUnderKey;
 //            if(_mapFromAgentNameToPartialMapBetweenIOPViewModelsList.contains(agentInMapping->name()))
 //            {
 //                listOfMapUnderKey = _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.value((agentInMapping->name()));
 //            }
 //            listOfMapUnderKey.append(newListOfPartialMap);
 //            _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insert(agentInMapping->name(), listOfMapUnderKey);
 //        }
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
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);

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
            _mapFromNameToAgentInMapping.insert(agentName, agentInMapping);

            // Add this new Agent In Mapping VM in the list for the qml
            _agentInMappingVMList.append(agentInMapping);

            qInfo() << "A new agent mapping has been added:" << agentName;
        }
    }
}


/**
 * @brief Deletes all the mapBetweenIOPVM where agent in paramater is involved.
 * @param agentInMapping
 */
/*void AgentsMappingController::_deleteAllMappingMadeOnTargetAgent(AgentInMappingVM* agentInMapping)
{
    if(agentInMapping != NULL)
    {
        // Check among the list of all partial map
        if(!_allPartialMapInMapping.isEmpty())
        {
            foreach (MapBetweenIOPVM* partialMapBetweenIOP, _allPartialMapInMapping.toList())
            {
                if(partialMapBetweenIOP != NULL)
                {
                    if(partialMapBetweenIOP->agentTo()->name() == agentInMapping->name())
                    {
                        // Update the list of mapbetween store under the 'agentFrom' key of the hash table.
                        QList<MapBetweenIOPVM*> listOfMapUnderKey;
                        if(_mapFromAgentNameToPartialMapBetweenIOPViewModelsList.contains(partialMapBetweenIOP->agentFrom()->name()))
                        {
                            listOfMapUnderKey = _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.value((partialMapBetweenIOP->agentFrom()->name()));
                        }
                        listOfMapUnderKey.removeOne(partialMapBetweenIOP);
                        _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insert(partialMapBetweenIOP->agentFrom()->name(), listOfMapUnderKey);

                        // If involved as agentTo
                        _allPartialMapInMapping.remove(partialMapBetweenIOP);
                        delete(partialMapBetweenIOP);
                    }
                }
            }
        }

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
             if(!newListOfPartialMap.isEmpty()){
                 // Push partialMaps into internal list
                 _allPartialMapInMapping.append(newListOfPartialMap);

                 //Add to Hash and to temp partial maps list
                 QList<MapBetweenIOPVM*> listOfMapUnderKey;
                 if(_mapFromAgentNameToPartialMapBetweenIOPViewModelsList.contains(agentInMapping->name()))
                 {
                     listOfMapUnderKey = _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.value((agentInMapping->name()));
                 }
                 listOfMapUnderKey.append(newListOfPartialMap);
                 _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insert(agentInMapping->name(), listOfMapUnderKey);
             }
         }
    }
}*/


/**
 * @brief Check if the map between an agent output and an agent input already exist.
 * @param agentFrom
 * @param pointFrom
 * @param agentTo
 * @param pointTo
 * @return
 */
/*bool AgentsMappingController::_checkIfMapBetweenIOPVMAlreadyExist(AgentInMappingVM* agentFrom, OutputVM *pointFrom, AgentInMappingVM* agentTo, InputVM *pointTo)
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
}*/


/**
 * @brief Delete a link between two agents
 * @param link
 */
void AgentsMappingController::_deleteLinkBetweenTwoAgents(MapBetweenIOPVM* link)
{
    if (link != NULL)
    {
        // Unselect the link if it is the currently selected one
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
 * @brief Get the view model of link which corresponds to a mapping element
 * @param mappingElement
 * @return
 */
MapBetweenIOPVM* AgentsMappingController::_getLinkFromMappingElement(ElementMappingM* mappingElement)
{
    MapBetweenIOPVM* link = NULL;

    if (mappingElement != NULL)
    {
        foreach (MapBetweenIOPVM* iterator, _allMapInMapping.toList())
        {
            // FIXME: An agent in mapping can have several Inputs (or Outputs) with the same name but with different types
            // --> Instead, this method must return a list of MapBetweenIOPVM
            if ((iterator != NULL)
                    && (iterator->agentFrom() != NULL) && (iterator->agentFrom()->name() == mappingElement->outputAgent())
                    && (iterator->agentTo() != NULL) && (iterator->agentTo()->name() == mappingElement->inputAgent())
                    && (iterator->pointFrom() != NULL) && (iterator->pointFrom()->name() == mappingElement->output())
                    && (iterator->pointTo() != NULL) && (iterator->pointTo()->name() == mappingElement->input()))
            {
                link = iterator;
                break;
            }
        }
    }

    return link;
}


/**
 * @brief Remove all the links with this agent
 * @param agent
 */
void AgentsMappingController::_removeAllLinksWithAgent(AgentInMappingVM* agent)
{
    if (agent != NULL)
    {
        foreach (MapBetweenIOPVM* link, _allMapInMapping.toList())
        {
            if ( (link != NULL) &&
                    ((link->agentFrom() == agent) || (link->agentTo() == agent)) )
            {
                // Remove a link between two agents from the mapping
                removeLinkBetweenTwoAgents(link);

                // Delete the link between two agents
                _deleteLinkBetweenTwoAgents(link);
            }
        }
    }
}
