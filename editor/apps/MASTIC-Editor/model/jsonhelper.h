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
 *
 */

#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <I2PropertyHelpers.h>

//#include <model/agentm.h>
#include "model/definitionm.h"
#include "model/agentmappingm.h"
#include "viewModel/actionvm.h"
#include "viewModel/actioneffectvm.h"
#include "viewModel/actionconditionvm.h"
#include "viewModel/actioninpalettevm.h"

/**
 * @brief The JsonHelper class defines a helper to manage JSON definitions of agents
 */
class JsonHelper : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit JsonHelper(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~JsonHelper();


    /**
     * @brief Initialize agents list from JSON file
     * @param byteArrayOfJson
     * @return
     */
    QList<QPair<QString, DefinitionM*>> initAgentsList(QByteArray byteArrayOfJson);


    /**
     * @brief Create a model of agent definition from JSON file
     * @param byteArrayOfJson
     * @return
     */
    DefinitionM* createModelOfDefinition(QByteArray byteArrayOfJson);


    /**
     * @brief Export the agents list
     * @param agentsListToExport list of pairs <agent name, definition>
     */
    QByteArray exportAgentsList(QList<QPair<QString, DefinitionM*>> agentsListToExport);


    /**
     * @brief Create a model of agent mapping with JSON and the input agent name corresponding
     * @param inputAgentName, byteArrayOfJson
     * @return
     */
    AgentMappingM* createModelOfAgentMapping(QString inputAgentName, QByteArray byteArrayOfJson);

    /**
     * @brief Initialize actions list from JSON file
     * @param byteArrayOfJson
     * @param agents list
     * @return
     */
    QPair< QPair< QList<ActionM*>, QList<ActionInPaletteVM*> > , QList<ActionVM*> > initActionsList(QByteArray byteArrayOfJson, QList<AgentInMappingVM*> listAgentsInMapping);

Q_SIGNALS:

public Q_SLOTS:

private:

    /**
     * @brief Create a model of agent definition from JSON object
     * @param jsonDefinition
     * @return
     */
    DefinitionM* _createModelOfDefinitionFromJSON(QJsonObject jsonDefinition);


    /**
     * @brief Create a model of agent Input/Output/Parameter from JSON object
     * @param jsonObject
     * @param agentIOPType
     * @return
     */
    AgentIOPM* _createModelOfAgentIOP(QJsonObject jsonObject, AgentIOPTypes::Value agentIOPType);


    /**
     * @brief Get JSON object from an agent Input/Output/Parameter
     * @param agentIOP
     * @return
     */
    QJsonObject _getJsonFromAgentIOP(AgentIOPM* agentIOP);


    /**
     * @brief Create a model of element mapping Input name/Output agent name/Output name with JSON
     * @param inputAgentName
     * @param jsonObject
     * @return
     */
    ElementMappingM* _createModelOfElementMapping(QString inputAgentName, QJsonObject jsonObject);

    /**
     * @brief Create a model of agent mapping with JSON and the input agent name corresponding
     * @param inputAgentName, byteArrayOfJson
     * @return
     */
    ActionEffectVM* _parseEffectVMFromJson(QJsonObject jsonEffect, QList<AgentInMappingVM *> listAgentsInMapping);

    /**
     * @brief Create an action condition VM from JSON object
     * @param jsonObject
     * @param list of agents in mapping
     * @return
     */
    ActionConditionVM* _parseConditionsVMFromJson(QJsonObject jsonCondition, QList<AgentInMappingVM*> listAgentsInMapping);
};

#endif // JSONHELPER_H
