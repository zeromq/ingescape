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

    // List of all agent in mapping VM
    //TODOESTIA : vérifier si le sort est utile ou non (je pense que oui dans la partie qml)
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentInMappingVM, agentInMappingVMList)

    //List of all agent
    I2_QOBJECT_LISTMODEL(MapBetweenIOPVM, allMapInMapping)

    //Flag to process the mapping when it's needed
    I2_QML_PROPERTY(bool, mappingIsApplied)
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
     * @brief Slot when a new model of agent definition must be added to current mapping
     * @param agentName
     * @param definition
     */
    void addAgentDefinitionToMapping(QString agentName, DefinitionM* definition);


    /**
     * @brief Slot when a new model of agent definition must be added to current mapping at a specific position
     * @param agentName
     * @param definition
     * @param position
     */
    void addAgentDefinitionToMappingAtPosition(QString agentName, DefinitionM* definition, QPointF position);

private Q_SLOTS:
    /**
     * @brief Slot when a new view model of a agent mapping is created on the main view mapping.
     *      Check if a map need to be created from the element mapping list in the model manager.
     *      The two agents corresponding need to be visible in the list.
     * @param agentName
     */
    void createMapBetweenIopInMappingFromAgentName(QString agentName);

    /**
     * @brief Slot which allow to find the second point element to map in the view with the name of the second agent and the iop corresponding
     * @param agentName The second agent in mapping name
     * @param iopName The input/output to map with
     */
    PointMapVM* findTheSecondPointOfElementMap(QString agentName, QString iopName);

private:
    // Usefull to save it
    MasticModelManager* _modelManager;

    QHash<QString, AgentInMappingVM *> _mapFromNameToAgentInMappingViewModelsList;
};

QML_DECLARE_TYPE(AgentsMappingController)

#endif // AGENTSMAPPINGCONTROLLER_H
