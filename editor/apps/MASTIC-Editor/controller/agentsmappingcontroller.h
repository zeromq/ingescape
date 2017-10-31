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

#ifndef AGENTSMAPPINGCONTROLLER_H
#define AGENTSMAPPINGCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <controller/masticmodelmanager.h>
#include <viewModel/agentinmappingvm.h>


/**
 * @brief The AgentsMappingController class defines the controller for agents mapping
 */
class AgentsMappingController : public QObject
{
    Q_OBJECT

    // List of all agents in mapping
    I2_QOBJECT_LISTMODEL(AgentInMappingVM, agentInMappingVMList)

    // List of all maps between agents
    I2_QOBJECT_LISTMODEL(MapBetweenIOPVM, allMapInMapping)

    // List of all partial maps between agents (input and input agent are fully defined, output and output agent are ghost element)
    // This list is not empty in the case of the presence of Ghost Agents in the mapping
    I2_QOBJECT_LISTMODEL(MapBetweenIOPVM, allPartialMapInMapping)

    // Flag indicating if our mapping is empty
    I2_QML_PROPERTY_READONLY(bool, isEmptyMapping)

    // Selected agent in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, selectedAgent)

public:
    /**
     * @brief Default constructor
     * @param modelManager
     * @param parent
     */
    explicit AgentsMappingController(MasticModelManager* modelManager,
                                     QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsMappingController();

Q_SIGNALS:

public Q_SLOTS:

    /**
     * @brief Slot when an agent from the list is dropped on the current mapping at a position
     * @param agentName
     * @param list
     * @param position
     */
    void addAgentToMappingAtPosition(QString agentName, AbstractI2CustomItemListModel* list, QPointF position);


    /**
     * @brief Slot when the flag "is Activated Mapping" changed
     * @param isActivatedMapping
     */
    void onIsActivatedMappingChanged(bool isActivatedMapping);


private Q_SLOTS:
    /**
     * @brief Slot when a new view model of a agent mapping is created on the main view mapping.
     *      Check if a map need to be created from the element mapping list in the model manager.
     *      The two agents corresponding need to be visible in the list.
     * @param currentAgentInMapping
     */
    void createMapBetweenIopInMappingFromAgentInMapping(AgentInMappingVM* currentAgentInMapping);


    /**
     * @brief Slot when the list of "Agents in Mapping" changed
     */
    void _onAgentsInMappingChanged();


private:
    /**
     * @brief Add new model(s) of agent to the current mapping at a specific position
     * @param agentName
     * @param agentsList
     * @param position
     */
    void _addAgentModelsToMappingAtPosition(QString agentName, QList<AgentM*> agentsList, QPointF position);


private:
    // Usefull to save it
    MasticModelManager* _modelManager;

    QHash<QString, AgentInMappingVM *> _mapFromNameToAgentInMappingViewModelsList;

    QHash<QString, MapBetweenIOPVM*> _mapFromAgentNameToPartialMapBetweenIOPViewModelsList;
};

QML_DECLARE_TYPE(AgentsMappingController)

#endif // AGENTSMAPPINGCONTROLLER_H
