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


/**
 * @brief The AgentsMappingController class defines the controller for agents mapping
 */
class AgentsMappingController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor
     * @param modelManager
     * @param parent
     */
    explicit AgentsMappingController(MasticModelManager* modelManager, QObject *parent = nullptr);


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


private:
    // Usefull to save it
    MasticModelManager* _modelManager;
};

QML_DECLARE_TYPE(AgentsMappingController)

#endif // AGENTSMAPPINGCONTROLLER_H
