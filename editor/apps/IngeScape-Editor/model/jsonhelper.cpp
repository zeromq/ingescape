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

#include "jsonhelper.h"

#include <QDebug>


/**
 * @brief Constructor
 * @param parent
 */
JsonHelper::JsonHelper(QObject *parent) : QObject(parent)
{
    qInfo() << "New JSON helper";
}


/**
 * @brief Destructor
 */
JsonHelper::~JsonHelper()
{
    qInfo() << "Delete JSON helper";
}


/**
 * @brief Initialize agents list from JSON file
 * @param byteArrayOfJson
 * @return
 */
QList<QPair<QStringList, DefinitionM*>> JsonHelper::initAgentsList(QByteArray byteArrayOfJson)
{
    QList<QPair<QStringList, DefinitionM*>> agentsListToImport;

    QJsonDocument jsonAgentsList = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonAgentsList.isArray())
    {
        foreach (QJsonValue jsonValue, jsonAgentsList.array()) {
            if (jsonValue.isObject()) {
                QJsonObject jsonAgent = jsonValue.toObject();

                // Get value for keys "agentName" and "definition"
                QJsonValue jsonName = jsonAgent.value("agentName");
                QJsonValue jsonDefinition = jsonAgent.value("definition");

                if (jsonName.isString() && jsonDefinition.isObject())
                {
                    // Create a model of agent definition from JSON object
                    DefinitionM* definition = _createModelOfAgentDefinitionFromJSON(jsonDefinition.toObject());
                    if (definition != NULL)
                    {
                        qDebug() << "Initialize agent" << jsonName.toString() << "with definition" << definition->name();

                        QStringList agentNameAndParametersToRestart;
                        agentNameAndParametersToRestart.append(jsonName.toString());

                        QJsonValue jsonHostname = jsonAgent.value("hostname");
                        QJsonValue jsonCommandLine = jsonAgent.value("commandLine");

                        if (jsonHostname.isString() && jsonCommandLine.isString())
                        {
                            agentNameAndParametersToRestart.append(jsonHostname.toString());
                            agentNameAndParametersToRestart.append(jsonCommandLine.toString());
                        }

                        // Create a pair with agent name and definition
                        QPair<QStringList, DefinitionM*> pair;

                        pair.first = agentNameAndParametersToRestart;
                        pair.second = definition;

                        // Add the pair to the list
                        agentsListToImport.append(pair);
                    }
                }
            }
        }
    }

    return agentsListToImport;
}


/**
 * @brief Create a model of agent definition from a JSON file
 * @param byteArrayOfJson
 * @return
 */
DefinitionM* JsonHelper::createModelOfAgentDefinition(QByteArray byteArrayOfJson)
{
    DefinitionM* agentDefinition = NULL;

    QJsonDocument jsonAgentDefinition = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonAgentDefinition.isObject())
    {
        QJsonObject jsonObject = jsonAgentDefinition.object();

        if (jsonObject.contains("definition"))
        {
            // Create a model of agent definition from JSON object
            agentDefinition = _createModelOfAgentDefinitionFromJSON(jsonObject.value("definition").toObject());
        }
    }
    return agentDefinition;
}


/**
 * @brief Create a model of agent mapping with JSON and the corresponding input agent name
 * @param inputAgentName
 * @param byteArrayOfJson
 * @return
 */
AgentMappingM* JsonHelper::createModelOfAgentMapping(QString inputAgentName, QByteArray byteArrayOfJson)
{
    AgentMappingM* agentMapping = NULL;

    QJsonDocument jsonAgentMapping = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonAgentMapping.isObject())
    {
        QJsonObject jsonObject = jsonAgentMapping.object();

        agentMapping = _createModelOfAgentMappingFromJSON(inputAgentName, jsonObject);
    }
    return agentMapping;
}


/**
 * @brief Export the agents list
 * @param agentsListToExport list of pairs <agent name (and parameters to restart), definition>
 */
QByteArray JsonHelper::exportAgentsList(QList<QPair<QStringList, DefinitionM*>> agentsListToExport)
{
    QJsonArray jsonArray;

    for (int i = 0; i < agentsListToExport.count(); i++)
    {
        QPair<QStringList, DefinitionM*> pair = agentsListToExport.at(i);
        QStringList agentNameAndParametersToRestart = pair.first;
        DefinitionM* definition = pair.second;

        QString agentName = agentNameAndParametersToRestart.first();

        if (!agentName.isEmpty() && (definition != NULL))
        {
            QJsonObject jsonAgent;
            jsonAgent.insert("agentName", agentName);

            if (agentNameAndParametersToRestart.count() == 3)
            {
                QString hostname = agentNameAndParametersToRestart.at(1);
                QString commandLine = agentNameAndParametersToRestart.at(2);
                jsonAgent.insert("hostname", hostname);
                jsonAgent.insert("commandLine", commandLine);
            }

            QJsonObject jsonDefinition = exportAgentDefinitionToJson(definition);

            jsonAgent.insert("definition", jsonDefinition);

            jsonArray.append(jsonAgent);
        }
    }

    QJsonDocument jsonDocument = QJsonDocument(jsonArray);

    return jsonDocument.toJson();
}


/**
 * @brief Export the model of agent definition into a JSON object
 * @param agentDefinition
 * @return JSON object
 */
QJsonObject JsonHelper::exportAgentDefinitionToJson(DefinitionM* agentDefinition)
{
    QJsonObject jsonDefinition = QJsonObject();

    if (agentDefinition != NULL)
    {
        jsonDefinition.insert("name", agentDefinition->name());
        jsonDefinition.insert("version", agentDefinition->version());
        jsonDefinition.insert("description", agentDefinition->description());

        QJsonArray jsonInputs = QJsonArray();
        foreach (AgentIOPM* agentIOP, agentDefinition->inputsList()->toList()) {
            if (agentIOP != NULL) {
                // Get JSON object from the agent Input/Output/Parameter
                QJsonObject jsonAgentIOP = _getJsonFromAgentIOP(agentIOP);

                jsonInputs.append(jsonAgentIOP);
            }
        }

        QJsonArray jsonOutputs = QJsonArray();
        foreach (AgentIOPM* agentIOP, agentDefinition->outputsList()->toList()) {
            if (agentIOP != NULL) {
                // Get JSON object from the agent Input/Output/Parameter
                QJsonObject jsonAgentIOP = _getJsonFromAgentIOP(agentIOP);

                jsonOutputs.append(jsonAgentIOP);
            }
        }

        QJsonArray jsonParameters = QJsonArray();
        foreach (AgentIOPM* agentIOP, agentDefinition->parametersList()->toList()) {
            if (agentIOP != NULL) {
                // Get JSON object from the agent Input/Output/Parameter
                QJsonObject jsonAgentIOP = _getJsonFromAgentIOP(agentIOP);

                jsonParameters.append(jsonAgentIOP);
            }
        }

        jsonDefinition.insert("inputs", jsonInputs);
        jsonDefinition.insert("outputs", jsonOutputs);
        jsonDefinition.insert("parameters", jsonParameters);
    }

    return jsonDefinition;
}


/**
 * @brief Export the model of agent mapping into a JSON object
 * @param agentMapping
 * @return JSON object
 */
QJsonObject JsonHelper::exportAgentMappingToJson(AgentMappingM* agentMapping)
{
    QJsonObject jsonMapping;

    if (agentMapping != NULL)
    {
        jsonMapping.insert("name", agentMapping->name());
        jsonMapping.insert("description", agentMapping->description());
        jsonMapping.insert("version", agentMapping->version());

        QJsonArray jsonArray;
        foreach (ElementMappingM* mappingElement, agentMapping->mappingElements()->toList()) {
            if (mappingElement != NULL)
            {
                QJsonObject jsonMappingElement;
                jsonMappingElement.insert("input_name", mappingElement->input());
                jsonMappingElement.insert("agent_name", mappingElement->outputAgent());
                jsonMappingElement.insert("output_name", mappingElement->output());

                jsonArray.append(jsonMappingElement);
            }
        }

        jsonMapping.insert("mapping_out", jsonArray);
    }

    return jsonMapping;
}


/**
 * @brief Get the JSON of an agent definition
 * @param agentDefinition
 * @return
 */
QString JsonHelper::getJsonOfAgentDefinition(DefinitionM* agentDefinition, QJsonDocument::JsonFormat jsonFormat)
{
    QString jsonOfDefinition = "";

    if (agentDefinition != NULL)
    {
        QJsonObject jsonDefinition = exportAgentDefinitionToJson(agentDefinition);

        QJsonObject jsonObject;
        jsonObject.insert("definition", jsonDefinition);

        QJsonDocument jsonDocument(jsonObject);
        jsonOfDefinition = QString(jsonDocument.toJson(jsonFormat));
    }

    return jsonOfDefinition;
}


/**
 * @brief Get the JSON of an agent mapping
 * @param agentMapping
 * @return
 */
QString JsonHelper::getJsonOfAgentMapping(AgentMappingM* agentMapping, QJsonDocument::JsonFormat jsonFormat)
{
    QString jsonOfMapping = "";

    if (agentMapping != NULL)
    {
        QJsonObject jsonMapping = exportAgentMappingToJson(agentMapping);

        QJsonObject jsonObject;
        jsonObject.insert("mapping", jsonMapping);

        QJsonDocument jsonDocument(jsonObject);
        jsonOfMapping = QString(jsonDocument.toJson(jsonFormat));
    }

    return jsonOfMapping;
}


/**
 * @brief Initialize actions list from JSON file
 * @param byteArrayOfJson
 * @return
 */
scenario_import_actions_lists_t *JsonHelper::initActionsList(QByteArray byteArrayOfJson, QList<AgentInMappingVM*> listAgentsInMapping)
{
    scenario_import_actions_lists_t * scenarioImport = NULL;
    QList<ActionM*> actionsListToImport;
    QList<ActionInPaletteVM*> actionsInPalette;
    QList<ActionVM*> actionsInTimeLine;

    QHash<int, ActionM*> hashFromUidToActionM;

    QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonDocument.isObject())
    {
        QJsonObject jsonRoot = jsonDocument.object();

        scenarioImport = new scenario_import_actions_lists_t();

        // ------
        // Actions in table list
        // ------
        QJsonValue jsonActionsList = jsonRoot.value("actions");
        if (jsonActionsList.isArray())
        {
            foreach (QJsonValue jsonTmp, jsonActionsList.toArray())
            {
                if (jsonTmp.isObject())
                {
                    QJsonObject jsonAction = jsonTmp.toObject();
                    ActionM* actionM = NULL;

                    if (jsonAction.contains("uid") && jsonAction.contains("name"))
                    {
                        QJsonValue jsonUID = jsonAction.value("uid");
                        QJsonValue jsonName = jsonAction.value("name");

                        if (jsonUID.isDouble() && jsonName.isString())
                        {
                            // Create the model of action
                            actionM = new ActionM(jsonUID.toInt(), jsonName.toString());

                            QJsonValue jsonValue = jsonAction.value("validity_duration_type");
                            if (jsonValue.isString())
                            {
                                int nValidationDurationType = ValidationDurationTypes::staticEnumFromKey(jsonValue.toString().toUpper());
                                actionM->setvalidityDurationType(static_cast<ValidationDurationTypes::Value>(nValidationDurationType));
                            }

                            jsonValue = jsonAction.value("validity_duration_value");
                            if (jsonValue.isString())
                            {
                                actionM->setvalidityDurationString(jsonValue.toString());
                            }

                            jsonValue = jsonAction.value("shall_revert");
                            if (jsonValue.isBool())
                            {
                                actionM->setshallRevert(jsonValue.toBool());
                            }

                            jsonValue = jsonAction.value("shall_revert_at_validity_end");
                            if (jsonValue.isBool())
                            {
                                actionM->setshallRevertWhenValidityIsOver(jsonValue.toBool());
                            }

                            jsonValue = jsonAction.value("shall_revert_after_time");
                            if (jsonValue.isBool())
                            {
                                actionM->setshallRevertAfterTime(jsonValue.toBool());
                            }

                            jsonValue = jsonAction.value("shall_rearm");
                            if (jsonValue.isBool())
                            {
                                actionM->setshallRearm(jsonValue.toBool());
                            }

                            jsonValue = jsonAction.value("revert_after_time");
                            if (jsonValue.isString())
                            {
                                actionM->setrevertAfterTimeString(jsonValue.toString());
                            }

                            QJsonValue jsonEffectsList = jsonAction.value("effects");
                            if (jsonEffectsList.isArray())
                            {
                                foreach (QJsonValue jsonEffect, jsonEffectsList.toArray())
                                {
                                    if (jsonEffect.isObject())
                                    {
                                        QJsonObject jsonEffectObj = jsonEffect.toObject();
                                        ActionEffectVM* effectVM = _parseEffectVMFromJson(jsonEffectObj, listAgentsInMapping);

                                        if(effectVM != NULL)
                                        {
                                            actionM->addEffectToList(effectVM);
                                        }
                                    }
                                }
                            }

                            QJsonValue jsonConditionsList = jsonAction.value("conditions");
                            if (jsonConditionsList.isArray())
                            {
                                foreach (QJsonValue jsonCondition, jsonConditionsList.toArray())
                                {
                                    if (jsonCondition.isObject())
                                    {
                                        QJsonObject jsonConditionObj = jsonCondition.toObject();
                                        ActionConditionVM* conditionVM = _parseConditionsVMFromJson(jsonConditionObj, listAgentsInMapping);

                                        if(conditionVM != NULL)
                                        {
                                            actionM->addConditionToList(conditionVM);
                                        }
                                    }
                                }
                            }

                        }
                    }

                    if ((actionM != NULL) && !hashFromUidToActionM.contains(actionM->uid()))
                    {
                        actionsListToImport.append(actionM);
                        hashFromUidToActionM.insert(actionM->uid(), actionM);
                    }
                }
            }
        }


        // ------
        // Actions in palette
        // ------
        QJsonValue jsonActionsInPaletteList = jsonRoot.value("actions_palette");
        if (jsonActionsInPaletteList.isArray())
        {
            for (QJsonValue jsonTmp : jsonActionsInPaletteList.toArray())
            {
                if (jsonTmp.isObject())
                {
                    QJsonObject jsonActionInPalette = jsonTmp.toObject();

                    QJsonValue jsonActionId = jsonActionInPalette.value("action_id");
                    //QJsonValue jsonActionName = jsonActionInPalette.value("action_name");
                    QJsonValue jsonActionIndex = jsonActionInPalette.value("index");

                    if (jsonActionId.isDouble() && jsonActionIndex.isDouble())
                    {
                        int actionId = jsonActionId.toInt();

                        if (hashFromUidToActionM.contains(actionId))
                        {
                            ActionM* actionM = hashFromUidToActionM.value(actionId);
                            if (actionM != NULL)
                            {
                                int index = jsonActionIndex.toInt();
                                if (index >= 0 && index < 9)
                                {
                                    // Add action in palette
                                    actionsInPalette.append(new ActionInPaletteVM(actionM, index));
                                }
                            }
                        }

                    }
                }
            }
        }

        // ------
        // Actions in timeline
        // ------
        QJsonValue jsonActionsInTimelineList = jsonRoot.value("actions_timeline");
        if(jsonActionsInTimelineList.isArray())
        {
            for (QJsonValue jsonTmp : jsonActionsInTimelineList.toArray())
            {
                if (jsonTmp.isObject())
                {
                    QJsonObject jsonActionInTimeline = jsonTmp.toObject();

                    QJsonValue jsonActionId = jsonActionInTimeline.value("action_id");
                    //QJsonValue jsonActionName = jsonActionInTimeline.value("action_name");
                    QJsonValue jsonActionStartTime = jsonActionInTimeline.value("start_time");
                    QJsonValue jsonActionColor = jsonActionInTimeline.value("color");
                    QJsonValue jsonActionLineInTimeline = jsonActionInTimeline.value("line_number");

                    if (jsonActionId.isDouble() && jsonActionStartTime.isDouble() && jsonActionLineInTimeline.isDouble())
                    {
                        int actionId = jsonActionId.toInt();

                        if (hashFromUidToActionM.contains(actionId))
                        {
                            ActionM* actionM = hashFromUidToActionM.value(actionId);
                            if (actionM != NULL)
                            {
                                // Create the view model for the timeline
                                ActionVM* actionVM = new ActionVM(actionM, jsonActionStartTime.toInt());

                                // Set line in timeline
                                actionVM->setlineInTimeLine(jsonActionLineInTimeline.toInt());

                                if (jsonActionColor.isString()) {
                                    actionVM->setcolor(jsonActionColor.toString());
                                }

                                // Add our view model of action in the list
                                actionsInTimeLine.append(actionVM);
                            }
                        }
                    }
                }
            }
        }

        // Set results
        scenarioImport->actionsInTableList.append(actionsListToImport);
        scenarioImport->actionsInPaletteList.append(actionsInPalette);
        scenarioImport->actionsInTimelineList.append(actionsInTimeLine);
    }

    return scenarioImport;
}


/**
 * @brief Export the actions lists into json object
 * @param actions list in table
 * @param actions list in the palette
 * @param actions list in the timeline
 * @return
 */
QJsonObject JsonHelper::exportScenario(QList<ActionM*> actionsList, QList<ActionInPaletteVM*> actionsInPaletteList, QList<ActionVM*> actionsInTimeLine)
{
    QJsonObject jsonScenario;

    // ----
    // Actions list
    // ----
    QJsonArray jsonActionsArray;
    foreach (ActionM* actionM, actionsList)
    {
        // Create properties
        QJsonObject jsonAgent;
        jsonAgent.insert("uid", actionM->uid());
        jsonAgent.insert("name", actionM->name());
        jsonAgent.insert("validity_duration_type", ValidationDurationTypes::staticEnumToKey(actionM->validityDurationType()));
        jsonAgent.insert("validity_duration_value", actionM->validityDurationString());
        jsonAgent.insert("shall_revert", actionM->shallRevert());
        jsonAgent.insert("shall_revert_at_validity_end", actionM->shallRevertWhenValidityIsOver());
        jsonAgent.insert("shall_revert_after_time", actionM->shallRevertAfterTime());
        jsonAgent.insert("revert_after_time", actionM->revertAfterTimeString());
        jsonAgent.insert("shall_rearm", actionM->shallRearm());

        bool jsonFilled = false;

        QJsonArray jsonConditionsArray;
        // Create conditions view models
        for (ActionConditionVM* conditionVM : actionM->conditionsList()->toList())
        {
            ActionConditionM* actionCondition = conditionVM->modelM();
            jsonFilled = false;

            if ((actionCondition != NULL) && (actionCondition->agent() != NULL))
            {
                QJsonObject jsonCondition;
                jsonCondition.insert("type", ActionConditionTypes::staticEnumToKey(conditionVM->conditionType()));
                jsonCondition.insert("agent_name", actionCondition->agent()->name());

                switch (conditionVM->conditionType())
                {
                case ActionConditionTypes::VALUE:
                {
                    IOPValueConditionM* iopCondition = qobject_cast<IOPValueConditionM*>(actionCondition);
                    if ((iopCondition != NULL) && (iopCondition->agentIOP() != NULL))
                    {
                        jsonCondition.insert("iop_name", iopCondition->agentIOP()->name());
                        jsonCondition.insert("operator", ValueComparisonTypes::staticEnumToKey(iopCondition->valueComparisonType()));
                        jsonCondition.insert("value", iopCondition->value());

                        jsonFilled = true;
                    }

                    break;
                }
                case ActionConditionTypes::AGENT:
                {
                    ConditionOnAgentM* conditionOnAgent = qobject_cast<ConditionOnAgentM*>(actionCondition);
                    if (conditionOnAgent != NULL)
                    {
                        jsonCondition.insert("value", AgentConditionValues::staticEnumToKey(conditionOnAgent->agentConditionValue()));

                        jsonFilled = true;
                    }
                    break;
                }
                default:
                    break;
                }

                if (jsonFilled) {
                    jsonConditionsArray.append(jsonCondition);
                }

            }
        }

        // Insert the list of conditions
        jsonAgent.insert("conditions", jsonConditionsArray);

        // Create effects view models
        QJsonArray jsonEffectsArray;
        foreach (ActionEffectVM* effectVM, actionM->effectsList()->toList())
        {
            ActionEffectM* actionEffect = effectVM->modelM();

            jsonFilled = false;

            if(actionEffect != NULL)
            {
                QJsonObject jsonEffect;
                jsonEffect.insert("type", ActionEffectTypes::staticEnumToKey(effectVM->effectType()));

                switch (effectVM->effectType())
                {
                case ActionEffectTypes::AGENT:
                {
                    EffectOnAgentM* effectOnAgent = qobject_cast<EffectOnAgentM*>(actionEffect);
                    if ((effectOnAgent != NULL) && (effectOnAgent->agent() != NULL))
                    {
                        jsonEffect.insert("agent_name", effectOnAgent->agent()->name());
                        jsonEffect.insert("value", AgentEffectValues::staticEnumToKey(effectOnAgent->agentEffectValue()));

                        jsonFilled = true;
                    }
                    break;
                }
                case ActionEffectTypes::VALUE:
                {
                    IOPValueEffectM* iopEffect = qobject_cast<IOPValueEffectM*>(actionEffect);
                    if ((iopEffect != NULL) && (iopEffect->agent() != NULL) && (iopEffect->agentIOP() != NULL))
                    {
                        jsonEffect.insert("agent_name", iopEffect->agent()->name());
                        jsonEffect.insert("iop_name", iopEffect->agentIOP()->name());
                        jsonEffect.insert("value", iopEffect->value());

                        jsonFilled = true;
                    }

                    break;
                }
                case ActionEffectTypes::MAPPING:
                {
                    MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(actionEffect);
                    if ((mappingEffect != NULL) && (mappingEffect->agent() != NULL) && (mappingEffect->input() != NULL)
                            && (mappingEffect->outputAgent() != NULL)  && (mappingEffect->output() != NULL))
                    {
                        jsonEffect.insert("input_agent_name", mappingEffect->agent()->name());
                        jsonEffect.insert("input_name", mappingEffect->input()->name());
                        jsonEffect.insert("output_agent_name", mappingEffect->outputAgent()->name());
                        jsonEffect.insert("output_name", mappingEffect->output()->name());
                        jsonEffect.insert("value", MappingEffectValues::staticEnumToKey(mappingEffect->mappingEffectValue()));

                        jsonFilled = true;
                    }
                    break;
                }
                default:
                    break;
                }

                if (jsonFilled) {
                    jsonEffectsArray.append(jsonEffect);
                }
            }
        }

        // Insert the list of effects
        jsonAgent.insert("effects", jsonEffectsArray);

        jsonActionsArray.append(jsonAgent);
    }
    jsonScenario.insert("actions", jsonActionsArray);


    // ----
    // Actions list in palette
    // ----
    QJsonArray jsonActionsInPaletteArray;
    for (ActionInPaletteVM* actionInPalette : actionsInPaletteList)
    {
        if (actionInPalette->modelM() != NULL)
        {
            QJsonObject jsonActionsInPalette;
            jsonActionsInPalette.insert("index", actionInPalette->indexInPanel());
            jsonActionsInPalette.insert("action_id", actionInPalette->modelM()->uid());
            jsonActionsInPalette.insert("action_name", actionInPalette->modelM()->name());

            jsonActionsInPaletteArray.append(jsonActionsInPalette);
        }
    }
    jsonScenario.insert("actions_palette", jsonActionsInPaletteArray);


    // ----
    // Actions list in timeline
    // ----
    QJsonArray jsonActionsInTimelineArray;
    for (ActionVM* actionVM : actionsInTimeLine)
    {
        if ((actionVM != NULL) && (actionVM->modelM() != NULL) && (actionVM->startTime() >= 0))
        {
            QJsonObject jsonActionsInTimeLine;
            jsonActionsInTimeLine.insert("action_id", actionVM->modelM()->uid());
            jsonActionsInTimeLine.insert("action_name", actionVM->modelM()->name());
            jsonActionsInTimeLine.insert("start_time", actionVM->startTime());
            jsonActionsInTimeLine.insert("color", actionVM->color().name());
            jsonActionsInTimeLine.insert("line_number", actionVM->lineInTimeLine());

            jsonActionsInTimelineArray.append(jsonActionsInTimeLine);
        }
    }
    jsonScenario.insert("actions_timeline", jsonActionsInTimelineArray);

    return jsonScenario;
}


/**
 * @brief Import the mapping from the json content
 * @param byteArrayOfJson
 * @return list of mapping_agent_import_t objects
 */
QList< mapping_agent_import_t* > JsonHelper::importMapping(QByteArray byteArrayOfJson, bool fromPlatform)
{
    QList< mapping_agent_import_t* > listAgentsMapping;

    QJsonDocument jsonFileRoot = QJsonDocument::fromJson(byteArrayOfJson);
    if ((fromPlatform == false && jsonFileRoot.isArray()) || (fromPlatform == true && jsonFileRoot.isObject()))
    {
        // Take into account the origin of the opening
        // if mapping is from a patform file, we get the mappings value
        // if not we get directly the value.
        QJsonArray jsonArray;
        if(fromPlatform == true)
        {
            QJsonValue mappingsValue = jsonFileRoot.object().value("mappings");
            if(mappingsValue.isArray())
            {
                jsonArray = mappingsValue.toArray();
            }
        } else {
            jsonArray = jsonFileRoot.array();
        }

        foreach (QJsonValue jsonValue, jsonArray)
        {
            if (jsonValue.isObject())
            {
                QJsonObject jsonAllMapping = jsonValue.toObject();

                // Get value for keys "agentName" and "definition"
                QJsonValue jsonName = jsonAllMapping.value("agentName");
                QJsonValue jsonDefinition = jsonAllMapping.value("definition");
                QJsonValue jsonPosition = jsonAllMapping.value("position");

                if (jsonName.isString() && jsonDefinition.isObject())
                {
                    // Create the agent definition and mapping
                    DefinitionM* definition = _createModelOfAgentDefinitionFromJSON(jsonDefinition.toObject());
                    AgentMappingM* agentMapping = _createModelOfAgentMappingFromJSON(jsonName.toString(), jsonAllMapping);


                    if (definition != NULL)
                    {
                        // Add our agent in mapping
                        mapping_agent_import_t* mappingAgent = new mapping_agent_import_t();
                        mappingAgent->definition = definition;
                        mappingAgent->mapping = agentMapping;
                        mappingAgent->name = jsonName.toString();
                        mappingAgent->position = QPointF(0.0,0.0);

                        // Load position
                        QStringList positionStringList = jsonPosition.toString().split(',');
                        if(positionStringList.count() == 2)
                        {
                            QString xStr = positionStringList.first().remove('(');
                            QString yStr = positionStringList.last().remove(')');

                            if(xStr.isEmpty() == false && yStr.isEmpty() == false)
                            {
                                mappingAgent->position = QPointF(xStr.toFloat(), yStr.toFloat());
                            }
                        }

                        // Add to list
                        listAgentsMapping.append(mappingAgent);
                    }
                    else
                    {
                        // No definition, we delete the mapping
                        if (agentMapping != NULL)
                        {
                            delete agentMapping;
                            agentMapping = NULL;
                        }
                    }
                }
            }
        }
    }

    return listAgentsMapping;
}


/**
 * @brief Export the agents in mapping list into json array object
 * @param agents in mapping list
 * @return
 */
QJsonArray JsonHelper::exportAllAgentsInMapping(QList<AgentInMappingVM*> agentsInMapping)
{
    QJsonArray jsonArray;

    foreach (AgentInMappingVM* agentInMapVM, agentsInMapping)
    {
        if(agentInMapVM->temporaryMapping() != NULL && agentInMapVM->models()->count() > 0)
        {
            // Set agent name
            QJsonObject jsonFullMapping;
            jsonFullMapping.insert("agentName", agentInMapVM->name());
            jsonFullMapping.insert("position", "("+ QString::number(agentInMapVM->position().x())+","+QString::number(agentInMapVM->position().y())+")");

            // Set the mapping
            QJsonObject jsonMapping = exportAgentMappingToJson(agentInMapVM->temporaryMapping());
            jsonFullMapping.insert("mapping", jsonMapping);

            // Set the definition
            QJsonObject jsonDefinition = exportAgentDefinitionToJson(agentInMapVM->models()->at(0)->definition());
            jsonFullMapping.insert("definition", jsonDefinition);

            // Append to the list of mapping agents
            jsonArray.append(jsonFullMapping);
        }
    }

    return jsonArray;
}


/**
 * @brief Create a model of record from JSON data
 * @param byteArrayOfJson
 * @return
 */
QList<RecordM*> JsonHelper::createRecordModelList(QByteArray byteArrayOfJson)
{
    QList<RecordM*> recordsList;

    QJsonDocument jsonAgentDefinition = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonAgentDefinition.isObject())
    {
        QJsonDocument jsonFileRoot = QJsonDocument::fromJson(byteArrayOfJson);
        QJsonValue recordsValue = jsonFileRoot.object().value("Records");

        if (recordsValue.isArray())
        {   
            foreach (QJsonValue jsonValue, recordsValue.toArray())
            {
                if (jsonValue.isObject())
                {
                    QJsonObject jsonAllMapping = jsonValue.toObject();

                    QJsonValue jsonId = jsonAllMapping.value("id");
                    QJsonValue jsonName = jsonAllMapping.value("name_record");
                    QJsonValue jsonBeginDateTime = jsonAllMapping.value("time_beg");
                    QJsonValue jsonEndDateTime = jsonAllMapping.value("time_end");

                    if (jsonName.isString() && jsonId.isString())
                    {
                        // Create record
                        RecordM* record = new RecordM(jsonId.toString(),
                                                      jsonName.toString(),
                                                      QDateTime::fromSecsSinceEpoch(jsonBeginDateTime.toDouble()),
                                                      QDateTime::fromSecsSinceEpoch(jsonEndDateTime.toDouble()));

                        recordsList.append(record);
                    }
                }
            }
        }
    }
    return recordsList;
}


/**
 * @brief Create a model of agent definition from a JSON object
 * @param jsonDefinition
 * @return
 */
DefinitionM* JsonHelper::_createModelOfAgentDefinitionFromJSON(QJsonObject jsonDefinition)
{
    DefinitionM* definition = NULL;

    QJsonValue jsonName = jsonDefinition.value("name");
    QJsonValue jsonDescription = jsonDefinition.value("description");
    QJsonValue jsonVersion = jsonDefinition.value("version");
    QJsonValue jsonParameters = jsonDefinition.value("parameters");
    QJsonValue jsonInputs = jsonDefinition.value("inputs");
    QJsonValue jsonOutputs = jsonDefinition.value("outputs");

    if (jsonName.isString() && jsonDescription.isString() && jsonVersion.isString())
    {
        // Create the agent definition
        definition = new DefinitionM(jsonName.toString(), jsonVersion.toString(), jsonDescription.toString());

        if (jsonParameters.isArray()) {
            foreach (QJsonValue jsonParameter, jsonParameters.toArray()) {
                if (jsonParameter.isObject())
                {
                    // Create a model of agent Parameter
                    AgentIOPM* agentParameter = _createModelOfAgentIOP(jsonParameter.toObject(), AgentIOPTypes::PARAMETER);
                    if (agentParameter != NULL) {
                        definition->parametersList()->append(agentParameter);
                    }
                }
            }
        }

        if (jsonInputs.isArray()) {
            foreach (QJsonValue jsonInput, jsonInputs.toArray()) {
                if (jsonInput.isObject())
                {
                    // Create a model of agent Input
                    AgentIOPM* agentInput = _createModelOfAgentIOP(jsonInput.toObject(), AgentIOPTypes::INPUT);
                    if (agentInput != NULL) {
                        definition->inputsList()->append(agentInput);
                    }
                }
            }
        }

        if (jsonOutputs.isArray()) {
            foreach (QJsonValue jsonOutput, jsonOutputs.toArray()) {
                if (jsonOutput.isObject())
                {
                    // Create a model of agent Output
                    AgentIOPM* agentOutput = _createModelOfAgentIOP(jsonOutput.toObject(), AgentIOPTypes::OUTPUT);
                    if (agentOutput != NULL) {
                        definition->outputsList()->append(agentOutput);
                    }
                }
            }
        }
    }

    return definition;
}


/**
 * @brief Create a model of agent mapping from JsonObject and the corresponding input agent name
 * @param inputAgentName
 * @param jsonObject
 * @return
 */
AgentMappingM* JsonHelper::_createModelOfAgentMappingFromJSON(QString inputAgentName, QJsonObject jsonObject)
{
    AgentMappingM* agentMapping = NULL;

    QJsonValue jsonMapping = jsonObject.value("mapping");
    if (jsonMapping.isObject())
    {
        QJsonObject jsonSubObject = jsonMapping.toObject();

        QJsonValue jsonName = jsonSubObject.value("name");
        QJsonValue jsonDescription = jsonSubObject.value("description");
        QJsonValue jsonVersion = jsonSubObject.value("version");
        QJsonValue jsonMappingOut = jsonSubObject.value("mapping_out");

        if (jsonName.isString() && jsonDescription.isString() && jsonVersion.isString())
        {
            // Create the agent definition
            agentMapping = new AgentMappingM(jsonName.toString(), jsonVersion.toString(), jsonDescription.toString());

            if (jsonMappingOut.isArray()) {
                QList<ElementMappingM*> mappingElements;

                foreach (QJsonValue jsonMap, jsonMappingOut.toArray())
                {
                    if (jsonMap.isObject())
                    {
                        ElementMappingM* mappingElement = _createModelOfElementMapping(inputAgentName, jsonMap.toObject());
                        if (mappingElement != NULL) {
                            mappingElements.append(mappingElement);
                        }
                    }
                }
                if (mappingElements.count() > 0)
                {
                    agentMapping->mappingElements()->append(mappingElements);
                }
            }
        }
    }

    return agentMapping;
}


/**
 * @brief Create a model of agent Input/Output/Parameter from JSON object
 * @param jsonObject
 * @param agentIOPType
 * @return
 */
AgentIOPM* JsonHelper::_createModelOfAgentIOP(QJsonObject jsonObject, AgentIOPTypes::Value agentIOPType)
{
    AgentIOPM* agentIOP = NULL;

    QJsonValue jsonName = jsonObject.value("name");
    QJsonValue jsonType = jsonObject.value("type");
    QJsonValue jsonValue = jsonObject.value("value");

    if (jsonName.isString() && jsonType.isString())
    {
        int nAgentIOPValueType = AgentIOPValueTypes::staticEnumFromKey(jsonType.toString());
        if (nAgentIOPValueType > -1)
        {
            if (agentIOPType == AgentIOPTypes::OUTPUT)
            {
                // Create the agent Output
                agentIOP = new OutputM(jsonName.toString(),
                                       static_cast<AgentIOPValueTypes::Value>(nAgentIOPValueType));
            }
            else
            {
                // Create the agent Input/Parameter
                agentIOP = new AgentIOPM(agentIOPType,
                                         jsonName.toString(),
                                         static_cast<AgentIOPValueTypes::Value>(nAgentIOPValueType));
            }

            switch (agentIOP->agentIOPValueType())
            {
            case AgentIOPValueTypes::INTEGER:
                if (jsonValue.isDouble()) {
                    int value = jsonValue.toInt();

                    agentIOP->setdefaultValue(QVariant(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not an int";
                }
                break;

            case AgentIOPValueTypes::DOUBLE:
                if (jsonValue.isDouble()) {
                    double value = jsonValue.toDouble();

                    agentIOP->setdefaultValue(QVariant(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a double";
                }
                break;

            case AgentIOPValueTypes::STRING:
                if (jsonValue.isString()) {
                    QString value = jsonValue.toString();

                    agentIOP->setdefaultValue(QVariant(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a string";
                }
                break;

            case AgentIOPValueTypes::BOOL:
                if (jsonValue.isString()) {
                    // Lower case
                    QString strValue = jsonValue.toString().toLower();
                    if ((strValue == "false") || (strValue == "true"))
                    {
                        bool value = (strValue == "false") ? false : true;

                        agentIOP->setdefaultValue(QVariant(value));
                    }
                    else {
                        qCritical() << "IOP '" << agentIOP->name() << "': The value '" << strValue << "' is not a bool";
                    }
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a bool";
                }
                break;

            case AgentIOPValueTypes::IMPULSION:
                // Nothing to do
                break;

            case AgentIOPValueTypes::DATA:
                if (jsonValue.isString()) {
                    QString strValue = jsonValue.toString();

                    QByteArray value = strValue.toLocal8Bit();
                    //QByteArray value = strValue.toUtf8();

                    agentIOP->setdefaultValue(QVariant(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a data of bytes";
                }
                break;

            default:
                qCritical() << "IOP '" << agentIOP->name() << "' has a bad type" << jsonType.toString();
                break;
            }

            //qDebug() << agentIOP->name() << "(" << AgentIOPValueTypes::staticEnumToKey(agentIOP->agentIOPValueType()) << ")" << agentIOP->displayableDefaultValue();
        }
        else {
            qCritical() << "IOP '" << jsonName.toString() << "': The value type '" << jsonType.toString() << "' is wrong (must be INTEGER, DOUBLE, STRING, BOOL, IMPULSION or DATA)";
        }
    }

    return agentIOP;
}


/**
 * @brief Get JSON object from an agent Input/Output/Parameter
 * @param agentIOP
 * @return
 */
QJsonObject JsonHelper::_getJsonFromAgentIOP(AgentIOPM* agentIOP)
{
    QJsonObject jsonAgentIOP;

    if (agentIOP != NULL)
    {
        jsonAgentIOP.insert("name", agentIOP->name());
        jsonAgentIOP.insert("type", AgentIOPValueTypes::staticEnumToKey(agentIOP->agentIOPValueType()));

        switch (agentIOP->agentIOPValueType())
        {
        case AgentIOPValueTypes::INTEGER:
        {
            bool success = false;
            int number = agentIOP->defaultValue().toInt(&success);
            if (success) {
                jsonAgentIOP.insert("value", number);
            }
            else {
                jsonAgentIOP.insert("value", 0);
                qWarning() << "Default value" << agentIOP->displayableDefaultValue() << "of I/O/P" << agentIOP->name() << "is NOT an int !";
            }
        }
            break;

        case AgentIOPValueTypes::DOUBLE:
        {
            bool success = false;
            double number = agentIOP->defaultValue().toDouble(&success);
            if (success) {
                jsonAgentIOP.insert("value", number);
            }
            else {
                jsonAgentIOP.insert("value", 0.0);
                qWarning() << "Default value" << agentIOP->displayableDefaultValue() << "of I/O/P" << agentIOP->name() << "is NOT a double !";
            }
        }
            break;

        case AgentIOPValueTypes::STRING:
            //jsonAgentIOP.insert("value", agentIOP->defaultValue().toString());
            jsonAgentIOP.insert("value", agentIOP->displayableDefaultValue());
            break;

        case AgentIOPValueTypes::BOOL:
            //jsonAgentIOP.insert("value", agentIOP->defaultValue().toString());
            jsonAgentIOP.insert("value", agentIOP->displayableDefaultValue());
            break;

        case AgentIOPValueTypes::IMPULSION:
            jsonAgentIOP.insert("value", "");
            break;

        case AgentIOPValueTypes::DATA:
            //jsonAgentIOP.insert("value", agentIOP->defaultValue().toString());
            jsonAgentIOP.insert("value", agentIOP->displayableDefaultValue());
            break;

        default:
            jsonAgentIOP.insert("value", "");
            break;
        }
    }

    return jsonAgentIOP;
}


/**
 * @brief Create a model of element mapping Input name/Output agent name/Output name with JSON
 * @param inputAgentName
 * @param jsonObject
 * @return
 */
ElementMappingM* JsonHelper::_createModelOfElementMapping(QString inputAgentName, QJsonObject jsonObject)
{
    ElementMappingM* mappingElement = NULL;

    QJsonValue jsonInputName = jsonObject.value("input_name");
    QJsonValue jsonOutputAgentName = jsonObject.value("agent_name");
    QJsonValue jsonOutputName = jsonObject.value("output_name");

    // All names must be defined
    if (jsonInputName.isString() && jsonOutputAgentName.isString() && jsonOutputName.isString())
    {
        mappingElement = new ElementMappingM(inputAgentName,
                                             jsonInputName.toString(),
                                             jsonOutputAgentName.toString(),
                                             jsonOutputName.toString());
    }

    return mappingElement;
}


/**
 * @brief Create an action effect VM from JSON object
 * @param jsonObject
 * @param list of agents in mapping
 * @return
 */
ActionEffectVM* JsonHelper::_parseEffectVMFromJson(QJsonObject jsonEffect, QList<AgentInMappingVM*> listAgentsInMapping)
{
    ActionEffectVM* actionEffectVM = NULL;

    QJsonValue jsonValue = jsonEffect.value("type");
    if(jsonValue.isString())
    {
        int effectType = ActionEffectTypes::staticEnumFromKey(jsonValue.toString().toUpper());
        if (effectType >= 0)
        {
            switch (effectType)
            {
                // VALUE
                case ActionEffectTypes::VALUE:
                {
                    QJsonValue jsonAgentName = jsonEffect.value("agent_name");
                    QJsonValue jsonIOPName = jsonEffect.value("iop_name");
                    if(jsonAgentName.isString() && jsonIOPName.isString())
                    {
                        // Check agent name and iop name exists
                        QString agentName = jsonAgentName.toString();
                        QString agentIOPName = jsonIOPName.toString();

                        AgentInMappingVM* agent = NULL;
                        AgentIOPM* iopAgentM = NULL;
                        QList<AgentIOPM*> listIOPAgents;
                        bool found = false;

                        foreach (AgentInMappingVM* iterator, listAgentsInMapping)
                        {
                            if ((iterator != NULL) && (iterator->name() == agentName))
                            {
                                agent = iterator;

                                // Go through the inputs
                                foreach (InputVM* inputVM, iterator->inputsList()->toList())
                                {
                                    if (!found && (inputVM->name() == agentIOPName))
                                    {
                                        iopAgentM = inputVM->firstModel();
                                        found = true;
                                    }

                                    if (inputVM->firstModel() != NULL) {
                                        listIOPAgents.append(inputVM->firstModel());
                                    }
                                }

                                // Go through the outputs
                                foreach (OutputVM* outputVM, iterator->outputsList()->toList())
                                {
                                    if (!found && (outputVM->name() == agentIOPName))
                                    {
                                        iopAgentM = outputVM->firstModel();
                                        found = true;
                                    }

                                    if (outputVM->firstModel() != NULL) {
                                        listIOPAgents.append(outputVM->firstModel());
                                    }
                                }
                                break;
                            }
                        }

                        if ((agent != NULL) && (iopAgentM != NULL))
                        {
                            // Create model
                            IOPValueEffectM* iopEffectM = new IOPValueEffectM();

                            // Create view model
                            actionEffectVM = new ActionEffectVM();
                            actionEffectVM->seteffectType(ActionEffectTypes::VALUE);

                            actionEffectVM->setmodelM(iopEffectM);

                            // set value
                            jsonValue = jsonEffect.value("value");
                            if (jsonValue.isString()) {
                                iopEffectM->setvalue(jsonValue.toString());
                            }

                            // Set the list of agent iop
                            iopEffectM->iopMergedList()->append(listIOPAgents);

                            // set agent and I/O/P
                            iopEffectM->setagent(agent);
                            iopEffectM->setagentIOP(iopAgentM);
                        }
                    }

                    break;
                }
                // AGENT
                case ActionEffectTypes::AGENT:
                {
                    QJsonValue jsonAgentName = jsonEffect.value("agent_name");
                    if(jsonAgentName.isString())
                    {
                        // Check agent name and iop name exists
                        QString agentName = jsonAgentName.toString();

                        AgentInMappingVM* agent = NULL;

                        foreach (AgentInMappingVM* iterator, listAgentsInMapping)
                        {
                            if ((iterator != NULL) && (iterator->name() == agentName))
                            {
                                agent = iterator;
                                break;
                            }
                        }

                        if (agent != NULL)
                        {
                            // Create model
                            EffectOnAgentM* effectOnAgent = new EffectOnAgentM();

                            // Create view model
                            actionEffectVM = new ActionEffectVM();
                            actionEffectVM->seteffectType(ActionEffectTypes::AGENT);

                            actionEffectVM->setmodelM(effectOnAgent);

                            // set value
                            jsonValue = jsonEffect.value("value");
                            if (jsonValue.isString()) {
                                int nAgentEffectValue = AgentEffectValues::staticEnumFromKey(jsonValue.toString().toUpper());
                                effectOnAgent->setagentEffectValue(static_cast<AgentEffectValues::Value>(nAgentEffectValue));
                            }

                            // set agent
                            effectOnAgent->setagent(agent);
                        }
                    }
                    break;
                }
                case ActionEffectTypes::MAPPING:
                {
                    QJsonValue jsonOutputAgentName = jsonEffect.value("output_agent_name");
                    QJsonValue jsonOutputName = jsonEffect.value("output_name");
                    QJsonValue jsonInputAgentName = jsonEffect.value("input_agent_name");
                    QJsonValue jsonInputName = jsonEffect.value("input_name");

                    if (jsonOutputAgentName.isString() && jsonOutputName.isString() && jsonInputAgentName.isString() && jsonInputName.isString())
                    {
                        // Check agent name and iop name exists
                        QString outputAgentName = jsonOutputAgentName.toString();
                        QString outputName = jsonOutputName.toString();
                        QString inputAgentName = jsonInputAgentName.toString();
                        QString inputName = jsonInputName.toString();

                        AgentInMappingVM* inputAgent = NULL;
                        AgentIOPM* input = NULL;
                        AgentInMappingVM* outputAgent = NULL;
                        AgentIOPM* output = NULL;
                        bool found = false;

                        QList<AgentIOPM*> outputsList;
                        QList<AgentIOPM*> inputsList;

                        foreach (AgentInMappingVM* iterator, listAgentsInMapping)
                        {
                            if (iterator != NULL)
                            {
                                if (iterator->name() == outputAgentName)
                                {
                                    outputAgent = iterator;
                                    found = false;

                                    /*// Go through the inputs
                                    foreach (InputVM* inputVM, iterator->inputsList()->toList())
                                    {
                                        if (!found && (inputVM->name() == outputName))
                                        {
                                            output = inputVM->firstModel();
                                            found = true;
                                        }

                                        if (inputVM->firstModel() != NULL) {
                                            outputsList.append(inputVM->firstModel());
                                        }
                                    }*/

                                    // Go through the outputs
                                    foreach (OutputVM* outputVM, iterator->outputsList()->toList())
                                    {
                                        if (!found && (outputVM->name() == outputName))
                                        {
                                            output = outputVM->firstModel();
                                            found = true;
                                        }

                                        if (outputVM->firstModel() != NULL) {
                                            outputsList.append(outputVM->firstModel());
                                        }
                                    }
                                }

                                if (iterator->name() == inputAgentName)
                                {
                                    inputAgent = iterator;
                                    found = false;

                                    // Go through the inputs
                                    foreach (InputVM* inputVM, iterator->inputsList()->toList())
                                    {
                                        if (!found && (inputVM->name() == inputName))
                                        {
                                            input = inputVM->firstModel();
                                            found = true;
                                        }

                                        if (inputVM->firstModel() != NULL) {
                                            inputsList.append(inputVM->firstModel());
                                        }
                                    }

                                    /*// Go through the outputs
                                    foreach (OutputVM* outputVM, iterator->outputsList()->toList())
                                    {
                                        if (!found && (outputVM->name() == inputName))
                                        {
                                            input = outputVM->firstModel();
                                            found = true;
                                        }

                                        if (outputVM->firstModel() != NULL) {
                                            inputsList.append(outputVM->firstModel());
                                        }
                                    }*/
                                }
                            }
                        }

                        if ((inputAgent != NULL) && (input != NULL) && (outputAgent != NULL) && (output != NULL))
                        {
                            // Create model
                            MappingEffectM* mappingEffectM = new MappingEffectM();

                            // Create view model
                            actionEffectVM = new ActionEffectVM();
                            actionEffectVM->seteffectType(ActionEffectTypes::MAPPING);
                            actionEffectVM->setmodelM(mappingEffectM);

                            // set (input) agent, input, output agent and output
                            mappingEffectM->setagent(inputAgent);
                            mappingEffectM->setinput(input);
                            mappingEffectM->setoutputAgent(outputAgent);
                            mappingEffectM->setoutput(output);

                            // set value
                            jsonValue = jsonEffect.value("value");
                            if (jsonValue.isString()) {
                                int nMappingEffectValue = MappingEffectValues::staticEnumFromKey(jsonValue.toString().toUpper());
                                mappingEffectM->setmappingEffectValue(static_cast<MappingEffectValues::Value>(nMappingEffectValue));
                            }

                            // Set the list of agent iop
                            mappingEffectM->outputsList()->append(outputsList);
                            mappingEffectM->inputsList()->append(inputsList);
                        }
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }
    return actionEffectVM;
}


/**
 * @brief Create an action condition VM from JSON object
 * @param jsonObject
 * @param list of agents in mapping
 * @return
 */
ActionConditionVM* JsonHelper::_parseConditionsVMFromJson(QJsonObject jsonCondition, QList<AgentInMappingVM*> listAgentsInMapping)
{
    ActionConditionVM* actionConditionVM = NULL;

    QJsonValue jsonValue = jsonCondition.value("type");
    if(jsonValue.isString())
    {
        int conditionType = ActionConditionTypes::staticEnumFromKey(jsonValue.toString().toUpper());
        if(conditionType >= 0)
        {
            switch (conditionType)
            {
                case ActionConditionTypes::VALUE:
                {
                    QJsonValue jsonAgentName = jsonCondition.value("agent_name");
                    QJsonValue jsonIOPName = jsonCondition.value("iop_name");

                    if(jsonAgentName.isString() && jsonIOPName.isString())
                    {
                        // Check agent name and iop name exists
                        QString agentName = jsonAgentName.toString();
                        QString agentIOPName = jsonIOPName.toString();

                        AgentInMappingVM* agentM = NULL;
                        AgentIOPM* iopAgentM = NULL;
                        QList<AgentIOPM*> listIOPAgents;
                        bool found = false;

                        foreach (AgentInMappingVM* agent, listAgentsInMapping)
                        {
                            if (agent->name() == agentName)
                            {
                                agentM = agent;

                                // Go through the inputs
                                foreach (InputVM* inputVM, agent->inputsList()->toList())
                                {
                                    if (!found && (inputVM->name() == agentIOPName))
                                    {
                                        iopAgentM = inputVM->firstModel();
                                        found = true;
                                    }

                                    if(inputVM->firstModel() != NULL)
                                    {
                                        listIOPAgents.append(inputVM->firstModel());
                                    }
                                }

                                // Go through the outputs
                                foreach (OutputVM* outputVM, agent->outputsList()->toList())
                                {
                                    if (!found && (outputVM->name() == agentIOPName))
                                    {
                                        iopAgentM = outputVM->firstModel();
                                        found = true;
                                    }

                                    if(outputVM->firstModel() != NULL)
                                    {
                                        listIOPAgents.append(outputVM->firstModel());
                                    }
                                }


                                break;
                            }
                        }

                        if ((agentM != NULL) && (iopAgentM != NULL))
                        {
                            // Create model
                            IOPValueConditionM* iopConditionM = new IOPValueConditionM();

                            // Create view model
                            actionConditionVM = new ActionConditionVM();
                            actionConditionVM->setconditionType(ActionConditionTypes::VALUE);

                            actionConditionVM->setmodelM(iopConditionM);

                            // set operator
                            jsonValue = jsonCondition.value("operator");
                            if (jsonValue.isString())
                            {
                                int nValueComparisonType = ValueComparisonTypes::staticEnumFromKey(jsonValue.toString().toUpper());
                                iopConditionM->setvalueComparisonType(static_cast<ValueComparisonTypes::Value>(nValueComparisonType));
                            }

                            // set value
                            jsonValue = jsonCondition.value("value");
                            if(jsonValue.isString())
                            {
                                iopConditionM->setvalue(jsonValue.toString());
                            }

                            // Set the list of agent iop
                            iopConditionM->agentIopList()->append(listIOPAgents);

                            // set agent
                            iopConditionM->setagent(agentM);
                            iopConditionM->setagentIOP(iopAgentM);                            
                        }
                    }

                    break;
                }
                case ActionConditionTypes::AGENT:
                {
                    QJsonValue jsonAgentName = jsonCondition.value("agent_name");
                    if (jsonAgentName.isString())
                    {
                        // Check agent name and iop name exists
                        QString agentName = jsonAgentName.toString();

                        AgentInMappingVM* agentM = NULL;

                        foreach (AgentInMappingVM* agent, listAgentsInMapping)
                        {
                            if (agent->name() == agentName)
                            {
                                agentM = agent;
                                break;
                            }
                        }

                        if (agentM != NULL)
                        {
                            // Create model
                            ConditionOnAgentM* conditionOnAgent = new ConditionOnAgentM();

                            // Create view model
                            actionConditionVM = new ActionConditionVM();
                            actionConditionVM->setconditionType(ActionConditionTypes::AGENT);

                            actionConditionVM->setmodelM(conditionOnAgent);

                            // set value
                            jsonValue = jsonCondition.value("value");
                            if (jsonValue.isString())
                            {
                                int nAgentConditionValue = AgentConditionValues::staticEnumFromKey(jsonValue.toString().toUpper());
                                conditionOnAgent->setagentConditionValue(static_cast<AgentConditionValues::Value>(nAgentConditionValue));
                            }

                            // set agent
                            conditionOnAgent->setagent(agentM);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
    return actionConditionVM;
}
