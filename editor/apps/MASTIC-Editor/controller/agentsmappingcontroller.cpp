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
      _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelManager != NULL)
    {
        _agentInMappingVMList.setSortProperty("agentName");
    }
}


/**
 * @brief Destructor
 */
AgentsMappingController::~AgentsMappingController()
{
    _modelManager = NULL;
}


/**
 * @brief Slot when a new model of agent definition must be added to current mapping
 * @param agentName
 * @param definition
 */
void AgentsMappingController::addAgentDefinitionToMapping(QString agentName, DefinitionM* definition)
{
    addAgentDefinitionToMappingAtPosition(agentName, definition, QPointF());
}


/**
 * @brief Slot when a new model of agent definition must be added to current mapping at a specific position
 * @param agentName
 * @param definition
 * @param position
 */
void AgentsMappingController::addAgentDefinitionToMappingAtPosition(QString agentName, DefinitionM* definition, QPointF position)
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

            //Call for the fist shot the another call will be proceed by the connect mechanism
            createMapBetweenIopInMappingFromAgentName(newAgentInMapping->agentName());

            // Connect to signal "new Definition added to agent in mapping" from the new definition
            connect(newAgentInMapping, &AgentInMappingVM::newDefinitionInAgentMapping,
                    this, &AgentsMappingController::createMapBetweenIopInMappingFromAgentName);

            //Add in the map list
            _mapFromNameToAgentInMappingViewModelsList.insert(agentName,newAgentInMapping);

            //Add this new Agent In Mapping VM in the list for the qml
            _agentInMappingVMList.append(newAgentInMapping);

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
    AgentInMappingVM * currentAgentInMapping = NULL;
    currentAgentInMapping = _mapFromNameToAgentInMappingViewModelsList.value(agentName);


    if(_modelManager != NULL && currentAgentInMapping != NULL)
    {
        qInfo() << "Agent input " << agentName << " found. /n";

        //Find the element mapping
        QList<ElementMappingM *> elementsMappingFound;

        //Initialize
        PointMapVM* inputPointMap = NULL;
        PointMapVM* outputPointMap = NULL;

        //
        // Input management
        //
        elementsMappingFound = _modelManager->getMergedListOfInputMappingElementsFromAgentName(agentName);

        if(!elementsMappingFound.isEmpty())
        {
             qInfo() << "Element Mapping List in input of the current Agent" << agentName << "is not empty. /n";

            foreach (ElementMappingM* currentElementMapping, elementsMappingFound)
            {
                //Check if the iop exist
                if(currentElementMapping != NULL)
                {

                    QString inputName = currentElementMapping->input();
                    qInfo() << "Node in input is: " << inputName << ". /n";
                    if(inputName != "")
                    {
                        inputPointMap = currentAgentInMapping->getPointMapFromInputName(inputName);

                        if(inputPointMap != NULL)
                        {
                            qInfo() << "PointMap " << inputName << " found. /n";
                            outputPointMap = findTheSecondPointOfElementMap(currentElementMapping->outputAgent(),
                                                           currentElementMapping->output());
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
            qInfo() << "Element Mapping List in output of the current Agent is not empty. /n";

            foreach (ElementMappingM* currentElementMapping, elementsMappingFound)
            {
                //Check if the iop exist
                if(currentElementMapping != NULL)
                {
                    QString outputName = currentElementMapping->output();
                    if(outputName != "")
                    {
                        outputPointMap = currentAgentInMapping->getPointMapFromOutputName(outputName);
                        qInfo() << "Node in input is: " << outputName << ". /n";

                        if(outputPointMap != NULL)
                        {
                            qInfo() << "PointMap " << outputName << " found. /n";
                            inputPointMap = findTheSecondPointOfElementMap(currentElementMapping->inputAgent(),
                                                           currentElementMapping->input());
                        }
                    }
                }
            }
        }

        //Add the Map between agent
        if((inputPointMap != NULL) && (outputPointMap != NULL))
        {
            MapBetweenIOPVM* map = new MapBetweenIOPVM(outputPointMap, inputPointMap);
            _allMapInMapping.append(map);

            qInfo() << "Create the MapBetweenIOPVM : " << inputPointMap->nameAgent() << "." << inputPointMap->iopModel()->name() << " -> " << outputPointMap->nameAgent() << "." << outputPointMap->iopModel()->name();

        }
    }
}

/**
 * @brief Slot which allow to find the second point element to map in the view with the name of the second agent and the iop corresponding
 * @param agentName The second agent in mapping name
 * @param iopName The input/output to map with
 */
PointMapVM * AgentsMappingController::findTheSecondPointOfElementMap(QString agentName,
                                                                     QString iopName)
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
         qInfo() << "Agent in output" << agentName << " found. /n";

        //First check in input list
        secondPointMapVM = secondAgentInMapping->getPointMapFromInputName(iopName);
        if(secondPointMapVM != NULL)
        {
            qInfo() << "PointMap " << iopName << " found. /n";
            return secondPointMapVM;
        }

        //Second check in output list
        secondPointMapVM = secondAgentInMapping->getPointMapFromOutputName(iopName);
        if(secondPointMapVM != NULL)
        {
            qInfo() << "PointMap " << iopName << " found. /n";
            return secondPointMapVM;
        }
    }

    qInfo() << "PointMap " << iopName << " NOT found. /n";
    return secondPointMapVM;
}
