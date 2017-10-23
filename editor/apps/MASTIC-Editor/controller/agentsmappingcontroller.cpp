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
            qDebug() << "TODO ESTIA: add VM for agent name" << agentName << "and definition" << definition->name();
        }
        // Position is defined (by Drag & Drop)
        else
        {
            qDebug() << "TODO ESTIA: add VM for agent name" << agentName << "and definition" << definition->name() << "at" << position.x() << position.y();
        }

        AgentInMappingVM * newAgentInMapping;

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

            qInfo()<<"The agent mapping already existing, new definition added to : "<<agentName;
        }
        else
        {
            //Create new Agent In Mapping
            newAgentInMapping = new AgentInMappingVM(definition,
                                                     position,
                                                     this);

            //Add in the map list
            _mapFromNameToAgentInMappingViewModelsList.insert(agentName,newAgentInMapping);

            //Add this new Agent In Mapping VM in the list for the qml
            _agentInMappingVMList.append(newAgentInMapping);

            qInfo()<<"A new agent mapping has been added : "<<agentName<<" from new definition";
        }
    }

}
