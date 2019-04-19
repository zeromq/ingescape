/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
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
#include <model/recordm.h>
#include <model/scenario/scenariom.h>


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
     * @brief Create a model of agent definition from bytes (JSON file content)
     * @param byteArrayOfJson
     * @return
     */
    DefinitionM* createModelOfAgentDefinitionFromBytes(QByteArray byteArrayOfJson);


    /**
     * @brief Create a model of agent definition from a JSON object
     * @param jsonDefinition
     * @return
     */
    DefinitionM* createModelOfAgentDefinitionFromJSON(QJsonObject jsonDefinition);


    /**
     * @brief Create a model of agent mapping from bytes (JSON file content) and the corresponding input agent name
     * @param inputAgentName
     * @param byteArrayOfJson
     * @return
     */
    AgentMappingM* createModelOfAgentMappingFromBytes(QString inputAgentName, QByteArray byteArrayOfJson);


    /**
     * @brief Create a model of agent mapping from a JSON object and the corresponding input agent name
     * @param inputAgentName
     * @param jsonMapping
     * @return
     */
    AgentMappingM* createModelOfAgentMappingFromJSON(QString inputAgentName, QJsonObject jsonMapping);


    /**
     * @brief Export the model of agent definition into a JSON object
     * @param agentDefinition
     * @return JSON object
     */
    QJsonObject exportAgentDefinitionToJson(DefinitionM* agentDefinition);


    /**
     * @brief Export the model of an agent mapping into a JSON object
     * @param agentMapping
     * @return JSON object
     */
    QJsonObject exportAgentMappingToJson(AgentMappingM* agentMapping);


    /**
     * @brief Export the model of an agent mapping with changes (applied while the global mapping was UN-activated) into a JSON object
     * @param agentMapping
     * @param addedMappingElements
     * @param namesOfRemovedMappingElements
     * @return JSON object
     */
    QJsonObject exportAgentMappingWithChangesToJson(AgentMappingM* agentMapping,
                                                    QList<MappingElementM*> addedMappingElements,
                                                    QStringList namesOfRemovedMappingElements);


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
     * @brief Get the JSON of an agent mapping with changes (applied while the global mapping was UN-activated)
     * @param agentMapping
     * @param addedMappingElements
     * @param namesOfRemovedMappingElements
     * @param jsonFormat
     * @return
     */
    QString getJsonOfAgentMappingWithChanges(AgentMappingM* agentMapping,
                                             QList<MappingElementM*> addedMappingElements,
                                             QStringList namesOfRemovedMappingElements,
                                             QJsonDocument::JsonFormat jsonFormat);


    /**
     * @brief Create a model of scenario (actions in the list, in the palette and in the timeline) from JSON
     * @param jsonScenario
     * @param hashFromNameToAgentsGrouped
     * @return
     */
    ScenarioM* createModelOfScenarioFromJSON(QJsonObject jsonScenario,
                                             QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped);


    /**
     * @brief Export the actions lists into json object
     * @param actions list
     * @param actions list in the palette
     * @param actions list in the timeline
     * @return
     */
    QJsonObject exportScenario(QList<ActionM*> actionsList, QList<ActionInPaletteVM*> actionsInPaletteList, QList<ActionVM*> actionsInTimeLine);


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
     * @brief Create a model of mapping element Input name/Output agent name/Output name with JSON
     * @param inputAgentName
     * @param jsonObject
     * @return
     */
    MappingElementM* _createModelOfMappingElement(QString inputAgentName, QJsonObject jsonObject);


    /**
     * @brief Create an action effect VM from JSON object
     * @param jsonEffect
     * @param hashFromNameToAgentsGrouped
     * @return
     */
    ActionEffectVM* _parseEffectVMFromJson(QJsonObject jsonEffect,
                                           QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped);


    /**
     * @brief Create an action condition VM from JSON object
     * @param jsonObject
     * @param hashFromNameToAgentsGrouped
     * @return
     */
    ActionConditionVM* _parseConditionsVMFromJson(QJsonObject jsonCondition,
                                                  QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped);

};

#endif // JSONHELPER_H
