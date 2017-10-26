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
    // DIS-connect from signal "Count Changed" from the list of agents in mapping
    disconnect(&_agentInMappingVMList, 0, this, 0);

    _modelManager = NULL;
}


/**
 * @brief Slot when a new model of agent definition must be added to current mapping
 * @param agentName
 * @param definition
 */
void AgentsMappingController::addAgentDefinitionToMapping(QString agentName, bool isON, DefinitionM* definition)
{
    addAgentDefinitionToMappingAtPosition(agentName, isON, definition, QPointF());
}


/**
 * @brief Slot when a new model of agent definition must be added to current mapping at a specific position
 * @param agentName
 * @param definition
 * @param position
 */
void AgentsMappingController::addAgentDefinitionToMappingAtPosition(QString agentName, bool isON, DefinitionM* definition, QPointF position)
{
    if (!agentName.isEmpty() && (definition != NULL))
    {
        // Position is NOT defined (from network)
        if (position.isNull())
        {
            qInfo() << "TODO ESTIA: add VM for agent name" << agentName << "and definition" << definition->name();
        }
        // Position is defined (by Drag & Drop)
        else
        {
            qInfo() << "TODO ESTIA: add VM for agent name" << agentName << "and definition" << definition->name() << "at ( " << position.x() << " ; " << position.y() << " ).";
        }

        AgentInMappingVM * newAgentInMapping = NULL;

        //Extract the name of tha agent
        QString agentName = definition->name();


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
            newAgentInMapping->setisON(isON);

            //Add in the map list
            _mapFromNameToAgentInMappingViewModelsList.insert(agentName,newAgentInMapping);

            //Add this new Agent In Mapping VM in the list for the qml
            _agentInMappingVMList.append(newAgentInMapping);

            //Call for the fist shot the another call will be proceed by the connect mechanism
            createMapBetweenIopInMappingFromAgentName(newAgentInMapping->agentName());

            // Connect to signal "new Definition added to agent in mapping" from the new definition
            connect(newAgentInMapping, &AgentInMappingVM::newDefinitionInAgentMapping,
                    this, &AgentsMappingController::createMapBetweenIopInMappingFromAgentName);


            qInfo() << "A new agent mapping has been added : " << agentName << " from new definition";
        }
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
                                PointMapVM* pointTemp = findTheSecondPointOfElementMap(currentElementMapping->outputAgent(),
                                                                                       currentElementMapping->output());
                                outputPointVM = dynamic_cast<OutputVM*>(pointTemp);


                                //Add the Map between agent
                                if((inputPointVM != NULL) && (outputPointVM != NULL))
                                {
                                    MapBetweenIOPVM* map = new MapBetweenIOPVM(outputPointVM, inputPointVM);
                                    if(map != NULL)
                                    {
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
                                PointMapVM* pointTemp = findTheSecondPointOfElementMap(currentElementMapping->inputAgent(),
                                                                                       currentElementMapping->input());
                                inputPointVM = dynamic_cast<InputVM*>(pointTemp);

                                //Add the Map between agent
                                if((inputPointVM != NULL) && (outputPointVM != NULL))
                                {
                                    MapBetweenIOPVM* map = new MapBetweenIOPVM(outputPointVM, inputPointVM);
                                    if(map != NULL)
                                    {
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
                _allMapInMapping.append(newMapBetweenIOP);
            }
        }
    }
}

/**
 * @brief Slot which allow to find the second point element to map in the view with the name of the second agent and the iop corresponding
 * @param agentName The second agent in mapping name
 * @param iopName The input/output to map with
 */
PointMapVM* AgentsMappingController::findTheSecondPointOfElementMap(QString agentName, QString iopName)
{
    PointMapVM* secondPointMapVM = NULL;

    //TODOESTIA : implement the function

    //Check If the second Agent is in the mapping
    AgentInMappingVM* secondAgentInMapping = NULL;
    secondAgentInMapping = _mapFromNameToAgentInMappingViewModelsList.value(agentName);

    //
    // Check if the iop is part of the definition of the second agent
    //
    if(secondAgentInMapping != NULL)
    {
         qInfo() << "Agent in output" << agentName << " found. ";

        //First check in input list
        secondPointMapVM = secondAgentInMapping->getPointMapFromInputName(iopName);
        if(secondPointMapVM != NULL)
        {
            qInfo() << "PointMap " << iopName << " found. ";
            return secondPointMapVM;
        }

        //Second check in output list
        secondPointMapVM = secondAgentInMapping->getPointMapFromOutputName(iopName);
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
                    qDebug() << "Create an AgentMappingVM for name" << agentName << "and with definitions of" << activeAgentsList.count() << "agents";

                    // FIXME VINCENT: En attendant de regrouper au sein de la classe AgentM: la définition et le mapping
                    if (activeAgentsList.count() == 1) {
                        QList<DefinitionM*> definitionsList = _modelManager->getAgentDefinitionsListFromName(agentName);

                        foreach (DefinitionM* definition, definitionsList) {
                            if (definition != NULL) {
                                addAgentDefinitionToMappingAtPosition(agentName, true, definition, QPointF());
                            }
                        }
                    }
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
