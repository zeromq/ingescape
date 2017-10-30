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
 * @param agentName
 */
void AgentsMappingController::createMapBetweenIopInMappingFromAgentName(QString agentName)
{
    if (_mapFromNameToAgentInMappingViewModelsList.contains(agentName)) {
        AgentInMappingVM* currentAgentInMapping = _mapFromNameToAgentInMappingViewModelsList.value(agentName);

        if ((_modelManager != NULL) && (currentAgentInMapping != NULL))
        {
            //Find the element mapping
            QList<ElementMappingM *> elementsMappingFound;
            QList<MapBetweenIOPVM *> newMapBetweenIOP;

            //Initialize
            InputVM* inputPointVM = NULL;
            OutputVM* outputPointVM = NULL;

            //
            // Input management
            //
            elementsMappingFound = _modelManager->getMergedListOfInputMappingElementsFromAgentName(agentName);

            if(!elementsMappingFound.isEmpty())
            {
                foreach (ElementMappingM* currentElementMapping, elementsMappingFound)
                {
                    //Check if the iop exist
                    if(currentElementMapping != NULL)
                    {
                        QString inputName = currentElementMapping->input();
                        if(inputName != "")
                        {
                            inputPointVM = currentAgentInMapping->getPointMapFromInputName(inputName);

                            if(inputPointVM != NULL)
                            {
                                qInfo() << "InputVM " << inputName << " found. ";

                                // Create outputAgent to capture the agentInMapping while searching for the second pointMap
                                AgentInMappingVM* outputAgent = NULL;
                                PointMapVM* pointTemp = findTheSecondPointOfElementMap(currentElementMapping->outputAgent(),
                                                                                       currentElementMapping->output(),
                                                                                       &outputAgent);
                                outputPointVM = dynamic_cast<OutputVM*>(pointTemp);

                                //Add the Map between agent
                                if((inputPointVM != NULL) && (outputPointVM != NULL) && ((outputAgent) != NULL))
                                {
                                    //Create the MapBetweenIOPVM
                                    MapBetweenIOPVM* map = new MapBetweenIOPVM((outputAgent), outputPointVM, currentAgentInMapping,inputPointVM);
                                    if(map != NULL)
                                    {
                                        //Add the new MapBetweenIOP to the temp list.
                                        newMapBetweenIOP.append(map);
                                    }
                                    qInfo() << "Create the MapBetweenIOPVM : " << inputPointVM->nameAgent() << "." << inputPointVM->modelM()->name() << " -> " << outputPointVM->nameAgent() << "." << outputPointVM->modelM()->name();

                                }
                            }
                        }
                    }
                }
            }

            //
            // Output management
            //
            elementsMappingFound = _modelManager->getMergedListOfOutputMappingElementsFromAgentName(agentName);

            if(!elementsMappingFound.isEmpty())
            {
                foreach (ElementMappingM* currentElementMapping, elementsMappingFound)
                {
                    //Check if the iop exist
                    if(currentElementMapping != NULL)
                    {
                        QString outputName = currentElementMapping->output();
                        if(outputName != "")
                        {
                            outputPointVM = currentAgentInMapping->getPointMapFromOutputName(outputName);

                            if(outputPointVM != NULL)
                            {
                                qInfo() << "OutputVM " << outputName << " found. ";

                                // Create inputAgent to capture the agentInMapping while searching for the second pointMap
                                AgentInMappingVM* inputAgent = NULL;
                                PointMapVM* pointTemp = findTheSecondPointOfElementMap(currentElementMapping->inputAgent(),
                                                                                       currentElementMapping->input(),
                                                                                       &inputAgent);
                                inputPointVM = dynamic_cast<InputVM*>(pointTemp);

                                //Add the Map between agent
                                if((inputPointVM != NULL) && (outputPointVM != NULL) && (inputAgent != NULL))
                                {
                                    //Create new MapBetweenIOP.
                                    MapBetweenIOPVM* map = new MapBetweenIOPVM(currentAgentInMapping, outputPointVM, inputAgent, inputPointVM);
                                    if(map != NULL)
                                    {
                                        //Add the new MapBetweenIOP to the temp list.
                                        newMapBetweenIOP.append(map);
                                    }
                                    qInfo() << "Create the MapBetweenIOPVM : " << inputPointVM->nameAgent() << "." << inputPointVM->modelM()->name() << " -> " << outputPointVM->nameAgent() << "." << outputPointVM->modelM()->name();

                                }
                            }
                        }
                    }
                }
            }

            if(!newMapBetweenIOP.isEmpty())
            {
                //Append all new mapBetweenIOP.
                _allMapInMapping.append(newMapBetweenIOP);
            }
        }
    }
}

/**
 * @brief Slot which allow to find the second point element to map in the view with the name of the second agent and the iop corresponding
 * @param agentName The second agent in mapping name
 * @param iopName The input/output to map with
 * @param secondAgentInMapping Pointer of the second AgentInMapping.
 */
PointMapVM* AgentsMappingController::findTheSecondPointOfElementMap(QString agentName, QString iopName, AgentInMappingVM** secondAgentInMapping)
{
    PointMapVM* secondPointMapVM = NULL;

    //Check If the second Agent is in the mapping
    (*secondAgentInMapping) = _mapFromNameToAgentInMappingViewModelsList.value(agentName);

    //
    // Check if the iop is part of the definition of the second agent
    //
    if((*secondAgentInMapping) != NULL)
    {
         qInfo() << "Agent in output" << agentName << " found. ";

        //First check in input list
        secondPointMapVM = (*secondAgentInMapping)->getPointMapFromInputName(iopName);
        if(secondPointMapVM != NULL)
        {
            qInfo() << "PointMap " << iopName << " found. ";
            return secondPointMapVM;
        }

        //Second check in output list
        secondPointMapVM = (*secondAgentInMapping)->getPointMapFromOutputName(iopName);
        if(secondPointMapVM != NULL)
        {
            qInfo() << "PointMap " << iopName << " found. ";
            return secondPointMapVM;
        }
    }

    qInfo() << "PointMap " << iopName << " NOT found. ";
    return secondPointMapVM;
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

            //Call for the fist shot the another call will be proceed by the connect mechanism
            createMapBetweenIopInMappingFromAgentName(newAgentInMapping->agentName());

            //TODOESTIA createMapBetweenIopInMappingFromAgent(AgentInMappingVM* )
            // Connect to signal "new Definition added to agent in mapping" from the new definition
            connect(newAgentInMapping, &AgentInMappingVM::newDefinitionInAgentMapping,
                    this, &AgentsMappingController::createMapBetweenIopInMappingFromAgentName);


            qInfo() << "A new agent mapping has been added : " << agentName << " from new definition";
        }
    }

}
