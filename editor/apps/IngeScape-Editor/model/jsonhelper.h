/*
 *	IngeScape Editor
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

#include <model/definitionm.h>
#include <model/mapping/agentmappingm.h>
#include <model/recordm.h>

#include "viewModel/scenario/actionvm.h"
#include "viewModel/scenario/actioneffectvm.h"
#include "viewModel/scenario/actionconditionvm.h"
#include "viewModel/scenario/actioninpalettevm.h"

/**
 * @brief Structure used for scenario importation from a json string
 * @param list of actions models in the list
 * @param list of actions in the palette
 * @param list of actions in the timeline
 */
struct scenario_import_actions_lists_t {
  QList<ActionM*>           actionsInTableList;
  QList<ActionInPaletteVM*> actionsInPaletteList;
  QList<ActionVM*>          actionsInTimelineList;
};


/**
 * @brief Structure used for mapping importation from a json string
 * @param name
 * @param position in mapping area
 * @param definition
 * @param mapping
 */
struct mapping_agent_import_t {
  QString               name;
  QPointF               position;
  QString               hostname;
  QString               commandLine;
  DefinitionM*          definition;
  AgentMappingM*        mapping;
};


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
     * @brief Import a list of agents from a JSON file
     * @param byteArrayOfJson
     * @return
     */
    QList<QPair<QStringList, DefinitionM*>> importAgentsList(QByteArray byteArrayOfJson);


    /**
     * @brief Create a model of agent definition from a JSON file
     * @param byteArrayOfJson
     * @return
     */
    DefinitionM* createModelOfAgentDefinition(QByteArray byteArrayOfJson);


    /**
     * @brief Create a model of agent mapping with JSON and the corresponding input agent name
     * @param inputAgentName
     * @param byteArrayOfJson
     * @return
     */
    AgentMappingM* createModelOfAgentMapping(QString inputAgentName, QByteArray byteArrayOfJson);


    /**
     * @brief Export the agents list
     * @param agentsListToExport list of pairs <agent name (and parameters to restart), definition>
     */
    QByteArray exportAgentsList(QList<QPair<QStringList, DefinitionM*>> agentsListToExport);


    /**
     * @brief Export the model of agent definition into a JSON object
     * @param agentDefinition
     * @return JSON object
     */
    QJsonObject exportAgentDefinitionToJson(DefinitionM* agentDefinition);


    /**
     * @brief Export the model of agent mapping into a JSON object
     * @param agentMapping
     * @return JSON object
     */
    QJsonObject exportAgentMappingToJson(AgentMappingM* agentMapping);


    /**
     * @brief Get the JSON of an agent definition
     * @param agentDefinition
     * @param jsonFormat
     * @return
     */
    QString getJsonOfAgentDefinition(DefinitionM* agentDefinition, QJsonDocument::JsonFormat jsonFormat);


    /**
     * @brief Get the JSON of an agent mapping
     * @param agentMapping
     * @param jsonFormat
     * @return
     */
    QString getJsonOfAgentMapping(AgentMappingM* agentMapping, QJsonDocument::JsonFormat jsonFormat);


    /**
     * @brief Initialize actions list from JSON file
     * @param byteArrayOfJson
     * @param agents list
     * @return the scenario importation structure containing the list of actions
     *         in the list, in the palette and in the timeline
     */
    scenario_import_actions_lists_t * initActionsList(QByteArray byteArrayOfJson, QList<AgentInMappingVM*> listAgentsInMapping);


    /**
     * @brief Export the actions lists into json object
     * @param actions list
     * @param actions list in the palette
     * @param actions list in the timeline
     * @return
     */
    QJsonObject exportScenario(QList<ActionM*> actionsList, QList<ActionInPaletteVM*> actionsInPaletteList, QList<ActionVM*> actionsInTimeLine);


    /**
     * @brief Import the mapping from the json content
     * @param byteArrayOfJson
     * @return list of mapping_agent_import_t objects
     */
    QList<mapping_agent_import_t *> importMapping(QByteArray byteArrayOfJson);


    /**
     * @brief Export the agents in mapping list into json array object
     * @param agents in mapping list
     * @return
     */
    QJsonArray exportAllAgentsInMapping(QList<AgentInMappingVM*> agentsInMapping);


    /**
     * @brief Create a model of record from JSON data
     * @param byteArrayOfJson
     * @return
     */
    QList<RecordM*> createRecordModelList(QByteArray byteArrayOfJson);


Q_SIGNALS:

public Q_SLOTS:

private:

    /**
     * @brief Create a model of agent definition from a JSON object
     * @param jsonDefinition
     * @return
     */
    DefinitionM* _createModelOfAgentDefinitionFromJSON(QJsonObject jsonDefinition);


    /**
     * @brief Create a model of agent mapping from JsonObject and the corresponding input agent name
     * @param inputAgentName
     * @param jsonObject
     * @return
     */
    AgentMappingM* _createModelOfAgentMappingFromJSON(QString inputAgentName,QJsonObject jsonObject);


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
