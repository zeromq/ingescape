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

    _modelManager = NULL;
}


/**
 * @brief Slot when an agent from the list is dropped on the current mapping at a position
 * @param agentName
 * @param list
 * @param position
 */
void AgentsMappingController::addAgentToMappingAtPosition(QString agentName, AbstractI2CustomItemListModel* list, QPointF position)
{
    I2CustomItemListModel<AgentM>* agentsList = dynamic_cast<I2CustomItemListModel<AgentM>*>(list);
    if (agentsList != NULL) {
        // Add new model(s) of agent to the current mapping at a specific position
        _addAgentModelsToMappingAtPosition(agentName, agentsList->toList(), position);
    }
}


/**
 * @brief Slot when a new view model of a agent mapping is created on the main view mapping.
 *      Check if a map need to be created from the element mapping list in the model manager.
 *      The two agents corresponding need to be visible in the list.
 * @param currentAgentInMapping
 */
void AgentsMappingController::createMapBetweenIopInMappingFromAgentInMapping(AgentInMappingVM* currentAgentInMapping)
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

            foreach(MapBetweenIOPVM* partialMap, foundPartialMapBetweenIOP)
            {
                // Get the real Output from the name of the ghost Output used to create the mapBetweenIOP.
                OutputVM* missingOutput = currentAgentInMapping->getPointMapFromOutputName(partialMap->pointFrom()->iopName());

                if(missingOutput != NULL)
                {
                    //Stuff the ghost elements in the partial map with the real things.

                    //Destroy ghost agent
                    AgentInMappingVM* agentToDestroy = partialMap->agentFrom();
                    agentToDestroy->~AgentInMappingVM();
                    //Set real agent
                    partialMap->setagentFrom(currentAgentInMapping);

                    //Destroy ghost output.
                    OutputVM* outputToDestroy = partialMap->pointFrom();
                    outputToDestroy->~OutputVM();
                    //Set real output
                    partialMap->setpointFrom(missingOutput);

                    //Add newly finalised MapBetweenIOP.
                    newMapBetweenIOP.append(partialMap);

                    _allPartialMapInMapping.remove(partialMap);
                    qInfo() <<"Delete Partial Map; " << currentAgentInMapping->agentName() << "." << missingOutput->iopName();
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
        if(!elementsMappingFound.isEmpty())
        {
            //Initialize

            // iterate on the list of element mapping
            foreach (ElementMappingM* currentElementMapping, elementsMappingFound)
            {
                //Check if the iop exist
                if(currentElementMapping != NULL)
                {
                    // Get the input conserned by the current elementMapping.

                    InputVM* inputPointVM = NULL;
                    inputPointVM = currentAgentInMapping->getPointMapFromInputName(currentElementMapping->input());

                    if(inputPointVM != NULL)
                    {
                        // Search for the output agent based on the current elementMapping
                        AgentInMappingVM* outputAgent = _mapFromNameToAgentInMappingViewModelsList.value(currentElementMapping->outputAgent());

                        OutputVM* outputPointVM = NULL;

                        // if NOT NULL proceed to create the MapBetweenIOP else create a ghost agent.
                        if(outputAgent != NULL)
                        {
                            // Get the output conserned by the mapping.
                            outputPointVM = outputAgent->getPointMapFromOutputName(currentElementMapping->output());

                            //We can proceed with the creation of the new MapBetweenIOP
                            if(outputPointVM != NULL)
                            {
                                //Create the MapBetweenIOPVM
                                MapBetweenIOPVM* map = new MapBetweenIOPVM(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM);

                                //Add the new MapBetweenIOP to the temp list.
                                newMapBetweenIOP.append(map);

                                qInfo() << "Create the MapBetweenIOPVM : " << currentAgentInMapping->agentName() << "." << inputPointVM->modelM()->name() << " -> " << outputAgent->agentName() << "." << outputPointVM->modelM()->name();
                            }
                            else
                            {
                                // TODO ESTIA: ON fait quoi si l'output n'est pas trouvée.
                                // Handle a missing Output
                                qCritical() << outputAgent->agentName() << "." << currentElementMapping->output() << " is missing!";
                            }

                        }
                        else
                        {
                            //Create a new ghost agent (AgentInMappingVM) as substitute of the missing agent name.
                            outputAgent = new AgentInMappingVM(currentElementMapping->outputAgent());

                            //Create a new ghost output (OutputVM) as substitute of the missing agent name.
                            outputPointVM = new OutputVM(currentElementMapping->output());

                            if(outputPointVM != NULL && outputAgent != NULL)
                            {
                                MapBetweenIOPVM* partialMap = new MapBetweenIOPVM(outputAgent, outputPointVM, currentAgentInMapping, inputPointVM);

                                //Add the new MapBetweenIOP to the temp list.
                                _allPartialMapInMapping.append(partialMap);

                                //Map partial mapBetweenIOP with output agent name to active search
                                _mapFromAgentNameToPartialMapBetweenIOPViewModelsList.insertMulti(outputAgent->agentName(), partialMap);

                                qInfo() << "Create the partial MapBetweenIOPVM : " << currentAgentInMapping->agentName() << "." << inputPointVM->modelM()->name() << " -> " << outputAgent->agentName() << "." << outputPointVM->iopName();
                            }
                        }
                    }
                }
            }
        }


         //Append all new mapBetweenIOP.
        if(!newMapBetweenIOP.isEmpty())
        {
            _allMapInMapping.append(newMapBetweenIOP);
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
        // Position is NOT defined (from network)
        if (position.isNull())
        {
            qDebug() << "Create an Agent Mapping VM for name" << agentName << "and with definitions of" << agentsList.count() << "agents";
        }
        // Position is defined (by Drag & Drop)
        else
        {
            qDebug() << "Create an Agent Mapping VM for name" << agentName << "and with definitions of" << agentsList.count() << "agents at ( " << position.x() << " ; " << position.y() << " ).";
        }

        AgentInMappingVM * newAgentInMapping = NULL;

        // FIXME: juste pour pouvoir compiler... TODOESTIA
        DefinitionM* definition = NULL;
        AgentM* agent = agentsList.first();
        if (agent != NULL) {
            definition = agent->definition();
        }

        //Check if it's already in the list
        bool exists = _mapFromNameToAgentInMappingViewModelsList.contains(agentName);


        //if yes only add the definition to the corresponding item
        //if not instanciate and add it
        if(exists)
        {
            newAgentInMapping  = _mapFromNameToAgentInMappingViewModelsList.value(agentName);

            newAgentInMapping->addDefinitionInInternalList(definition);

            qInfo() << "The agent mapping already existing, new definition added to : " << agentName;
        }
        else
        {


            //Create new Agent In Mapping
            newAgentInMapping = new AgentInMappingVM(definition,
                                                     position,
                                                     this);
            newAgentInMapping->setisON(agent->isON());

            //Add in the map list
            _mapFromNameToAgentInMappingViewModelsList.insert(agentName,newAgentInMapping);

            //Add this new Agent In Mapping VM in the list for the qml
            _agentInMappingVMList.append(newAgentInMapping);

            //Call for the first shot -> new call will be proceed by the connect mechanism
            createMapBetweenIopInMappingFromAgentInMapping(newAgentInMapping);

            // Connect to signal "new Definition added to agent in mapping" from the new definition
            connect(newAgentInMapping, &AgentInMappingVM::newDefinitionInAgentMapping,
                    this, &AgentsMappingController::createMapBetweenIopInMappingFromAgentInMapping);

            qInfo() << "A new agent mapping has been added : " << agentName << " from new definition";
        }
    }

}
