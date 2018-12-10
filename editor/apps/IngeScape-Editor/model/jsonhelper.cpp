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
#include <misc/ingescapeeditorutils.h>

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
 * @brief Create a model of agent definition from bytes (JSON file content)
 * @param byteArrayOfJson
 * @return
 */
DefinitionM* JsonHelper::createModelOfAgentDefinitionFromBytes(QByteArray byteArrayOfJson)
{
    DefinitionM* agentDefinition = nullptr;

    QJsonDocument jsonAgentDefinition = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonAgentDefinition.isObject())
    {
        QJsonObject jsonObject = jsonAgentDefinition.object();

        if (jsonObject.contains("definition"))
        {
            QJsonValue jsonValue = jsonObject.value("definition");
            if (jsonValue.isObject())
            {
                // Create a model of agent definition from JSON object
                agentDefinition = createModelOfAgentDefinitionFromJSON(jsonValue.toObject());
            }
        }
    }
    return agentDefinition;
}


/**
 * @brief Create a model of agent definition from a JSON object
 * @param jsonDefinition
 * @return
 */
DefinitionM* JsonHelper::createModelOfAgentDefinitionFromJSON(QJsonObject jsonDefinition)
{
    DefinitionM* definition = nullptr;

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

        if (jsonParameters.isArray())
        {
            for (QJsonValue jsonParameter : jsonParameters.toArray())
            {
                if (jsonParameter.isObject())
                {
                    // Create a model of agent Parameter
                    AgentIOPM* agentParameter = _createModelOfAgentIOP(jsonParameter.toObject(), AgentIOPTypes::PARAMETER);
                    if (agentParameter != nullptr) {
                        definition->parametersList()->append(agentParameter);
                    }
                }
            }
        }

        if (jsonInputs.isArray())
        {
            for (QJsonValue jsonInput : jsonInputs.toArray())
            {
                if (jsonInput.isObject())
                {
                    // Create a model of agent Input
                    AgentIOPM* agentInput = _createModelOfAgentIOP(jsonInput.toObject(), AgentIOPTypes::INPUT);
                    if (agentInput != nullptr) {
                        definition->inputsList()->append(agentInput);
                    }
                }
            }
        }

        if (jsonOutputs.isArray())
        {
            for (QJsonValue jsonOutput : jsonOutputs.toArray())
            {
                if (jsonOutput.isObject())
                {
                    // Create a model of agent Output
                    AgentIOPM* agentOutput = _createModelOfAgentIOP(jsonOutput.toObject(), AgentIOPTypes::OUTPUT);
                    if (agentOutput != nullptr) {
                        definition->outputsList()->append(agentOutput);
                    }
                }
            }
        }
    }
    return definition;
}


/**
 * @brief Create a model of agent mapping from bytes (JSON file content) and the corresponding input agent name
 * @param inputAgentName
 * @param byteArrayOfJson
 * @return
 */
AgentMappingM* JsonHelper::createModelOfAgentMappingFromBytes(QString inputAgentName, QByteArray byteArrayOfJson)
{
    AgentMappingM* agentMapping = nullptr;

    QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonDocument.isObject())
    {
        QJsonObject jsonRoot = jsonDocument.object();

        if (jsonRoot.contains("mapping"))
        {
            QJsonValue jsonAgentMapping = jsonRoot.value("mapping");
            if (jsonAgentMapping.isObject())
            {
                // Create a model of agent mapping from JSON object
                agentMapping = createModelOfAgentMappingFromJSON(inputAgentName, jsonAgentMapping.toObject());
            }
        }
    }
    return agentMapping;
}


/**
 * @brief Create a model of agent mapping from a JSON object and the corresponding input agent name
 * @param inputAgentName
 * @param jsonMapping
 * @return
 */
AgentMappingM* JsonHelper::createModelOfAgentMappingFromJSON(QString inputAgentName, QJsonObject jsonMapping)
{
    AgentMappingM* agentMapping = nullptr;

    QJsonValue jsonName = jsonMapping.value("name");
    QJsonValue jsonDescription = jsonMapping.value("description");
    QJsonValue jsonVersion = jsonMapping.value("version");
    QJsonValue jsonMappingOut = jsonMapping.value("mapping_out");

    if (jsonName.isString() && jsonDescription.isString() && jsonVersion.isString())
    {
        // Create the agent mapping
        agentMapping = new AgentMappingM(jsonName.toString(), jsonVersion.toString(), jsonDescription.toString());

        if (jsonMappingOut.isArray())
        {
            QList<ElementMappingM*> tempMappingElements;

            for (QJsonValue jsonMap : jsonMappingOut.toArray())
            {
                if (jsonMap.isObject())
                {
                    ElementMappingM* mappingElement = _createModelOfElementMapping(inputAgentName, jsonMap.toObject());
                    if (mappingElement != nullptr) {
                        tempMappingElements.append(mappingElement);
                    }
                }
            }

            if (!tempMappingElements.isEmpty()) {
                agentMapping->mappingElements()->append(tempMappingElements);
            }
        }
    }
    return agentMapping;
}


/**
 * @brief Export the model of agent definition into a JSON object
 * @param agentDefinition
 * @return JSON object
 */
QJsonObject JsonHelper::exportAgentDefinitionToJson(DefinitionM* agentDefinition)
{
    QJsonObject jsonDefinition = QJsonObject();

    if (agentDefinition != nullptr)
    {
        jsonDefinition.insert("name", agentDefinition->name());
        jsonDefinition.insert("version", agentDefinition->version());
        jsonDefinition.insert("description", agentDefinition->description());

        QJsonArray jsonInputs = QJsonArray();
        for (AgentIOPM* agentIOP : agentDefinition->inputsList()->toList())
        {
            if (agentIOP != nullptr) {
                // Get JSON object from the agent Input/Output/Parameter
                QJsonObject jsonAgentIOP = _getJsonFromAgentIOP(agentIOP);

                jsonInputs.append(jsonAgentIOP);
            }
        }

        QJsonArray jsonOutputs = QJsonArray();
        for (AgentIOPM* agentIOP : agentDefinition->outputsList()->toList())
        {
            if (agentIOP != nullptr) {
                // Get JSON object from the agent Input/Output/Parameter
                QJsonObject jsonAgentIOP = _getJsonFromAgentIOP(agentIOP);

                jsonOutputs.append(jsonAgentIOP);
            }
        }

        QJsonArray jsonParameters = QJsonArray();
        for (AgentIOPM* agentIOP : agentDefinition->parametersList()->toList())
        {
            if (agentIOP != nullptr) {
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

    if (agentMapping != nullptr)
    {
        jsonMapping.insert("name", agentMapping->name());
        jsonMapping.insert("description", agentMapping->description());
        jsonMapping.insert("version", agentMapping->version());

        QJsonArray jsonArray;
        for (ElementMappingM* mappingElement : agentMapping->mappingElements()->toList())
        {
            if (mappingElement != nullptr)
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
 * @brief Export the model of agent mapping plus its temporary list of mapping elements into a JSON object
 * @param agentMapping
 * @param temporaryMappingElements
 * @return JSON object
 */
QJsonObject JsonHelper::exportAgentTemporaryMappingToJson(AgentMappingM* agentMapping, QList<ElementMappingM*> temporaryMappingElements)
{
    QJsonObject jsonMapping;

    if (agentMapping != nullptr)
    {
        jsonMapping.insert("name", agentMapping->name());
        jsonMapping.insert("description", agentMapping->description());
        jsonMapping.insert("version", agentMapping->version());

        QJsonArray jsonArray;

        QList<ElementMappingM*> completeList = agentMapping->mappingElements()->toList();
        completeList.append(temporaryMappingElements);

        for (ElementMappingM* mappingElement : completeList)
        {
            if (mappingElement != nullptr)
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

    if (agentDefinition != nullptr)
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

    if (agentMapping != nullptr)
    {
        // Export the model of agent mapping into a JSON object
        QJsonObject jsonMapping = exportAgentMappingToJson(agentMapping);

        QJsonObject jsonObject;
        jsonObject.insert("mapping", jsonMapping);

        QJsonDocument jsonDocument(jsonObject);
        jsonOfMapping = QString(jsonDocument.toJson(jsonFormat));
    }
    return jsonOfMapping;
}


/**
 * @brief Get the JSON of an agent mapping plus its temporary list of mapping elements
 * @param agentMapping
 * @param temporaryMappingElements
 * @param jsonFormat
 * @return
 */
QString JsonHelper::getJsonOfAgentTemporaryMapping(AgentMappingM* agentMapping, QList<ElementMappingM*> temporaryMappingElements, QJsonDocument::JsonFormat jsonFormat)
{
    QString jsonOfMapping = "";

    if (agentMapping != nullptr)
    {
        // Export the model of agent mapping plus its temporary list of mapping elements into a JSON object
        QJsonObject jsonMapping = exportAgentTemporaryMappingToJson(agentMapping, temporaryMappingElements);

        QJsonObject jsonObject;
        jsonObject.insert("mapping", jsonMapping);

        QJsonDocument jsonDocument(jsonObject);
        jsonOfMapping = QString(jsonDocument.toJson(jsonFormat));
    }
    return jsonOfMapping;
}


/**
 * @brief Create a model of scenario (actions in the list, in the palette and in the timeline) from JSON
 * @param jsonScenario
 * @param hashFromNameToAgentsGrouped
 * @return
 */
ScenarioM* JsonHelper::createModelOfScenarioFromJSON(QJsonObject jsonScenario,
                                                     QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped)
{
    ScenarioM* scenarioImport = new ScenarioM();

    QList<ActionM*> actionsListToImport;
    QList<ActionInPaletteVM*> actionsInPalette;
    QList<ActionVM*> actionsInTimeLine;

    QHash<int, ActionM*> hashFromUidToActionM;

    // ------
    // Actions list
    // ------
    QJsonValue jsonActionsList = jsonScenario.value("actions");
    if (jsonActionsList.isArray())
    {
        for (QJsonValue jsonTmp : jsonActionsList.toArray())
        {
            if (jsonTmp.isObject())
            {
                QJsonObject jsonAction = jsonTmp.toObject();
                ActionM* actionM = nullptr;

                if (jsonAction.contains("uid") && jsonAction.contains("name"))
                {
                    QJsonValue jsonUID = jsonAction.value("uid");
                    QJsonValue jsonName = jsonAction.value("name");

                    if (jsonUID.isDouble() && jsonName.isString())
                    {
                        int uid = jsonUID.toInt();

                        // Book the UID of the new model of action
                        IngeScapeEditorUtils::bookUIDforActionM(uid);

                        // Create the model of action
                        actionM = new ActionM(uid, jsonName.toString());

                        QJsonValue jsonValue = jsonAction.value("validity_duration_type");
                        if (jsonValue.isString())
                        {
                            int nValidationDurationType = ValidationDurationTypes::staticEnumFromKey(jsonValue.toString().toUpper());
                            actionM->setvalidityDurationType(static_cast<ValidationDurationTypes::Value>(nValidationDurationType));
                        }

                        jsonValue = jsonAction.value("validity_duration_value");
                        if (jsonValue.isString()) {
                            actionM->setvalidityDurationString(jsonValue.toString());
                        }


                        //
                        // REVERT
                        //
                        jsonValue = jsonAction.value("shall_revert");
                        if (jsonValue.isBool()) {
                            actionM->setshallRevert(jsonValue.toBool());
                        }

                        jsonValue = jsonAction.value("shall_revert_at_validity_end");
                        if (jsonValue.isBool()) {
                            actionM->setshallRevertWhenValidityIsOver(jsonValue.toBool());
                        }

                        jsonValue = jsonAction.value("shall_revert_after_time");
                        if (jsonValue.isBool()) {
                            actionM->setshallRevertAfterTime(jsonValue.toBool());
                        }

                        jsonValue = jsonAction.value("revert_after_time");
                        if (jsonValue.isString()) {
                            actionM->setrevertAfterTimeString(jsonValue.toString());
                        }


                        //
                        // REARM
                        //
                        jsonValue = jsonAction.value("shall_rearm");
                        if (jsonValue.isBool()) {
                            actionM->setshallRearm(jsonValue.toBool());
                        }

                        jsonValue = jsonAction.value("rearm_after_time");
                        if (jsonValue.isString()) {
                            actionM->setrearmAfterTimeString(jsonValue.toString());
                        }


                        //
                        // EFFECTS
                        //
                        QJsonValue jsonEffectsList = jsonAction.value("effects");
                        if (jsonEffectsList.isArray())
                        {
                            for (QJsonValue jsonEffect : jsonEffectsList.toArray())
                            {
                                if (jsonEffect.isObject())
                                {
                                    ActionEffectVM* effectVM = _parseEffectVMFromJson(jsonEffect.toObject(), hashFromNameToAgentsGrouped);
                                    if (effectVM != nullptr)
                                    {
                                        actionM->addEffectToList(effectVM);
                                    }
                                }
                            }
                        }


                        //
                        // CONDITIONS
                        //
                        QJsonValue jsonConditionsList = jsonAction.value("conditions");
                        if (jsonConditionsList.isArray())
                        {
                            for (QJsonValue jsonCondition : jsonConditionsList.toArray())
                            {
                                if (jsonCondition.isObject())
                                {
                                    ActionConditionVM* conditionVM = _parseConditionsVMFromJson(jsonCondition.toObject(), hashFromNameToAgentsGrouped);
                                    if (conditionVM != nullptr)
                                    {
                                        actionM->addConditionToList(conditionVM);
                                    }
                                }
                            }
                        }

                    }
                }

                if ((actionM != nullptr) && !hashFromUidToActionM.contains(actionM->uid()))
                {
                    actionsListToImport.append(actionM);
                    hashFromUidToActionM.insert(actionM->uid(), actionM);
                }
            }
        }
    }


    // ------
    // Actions in the palette
    // ------
    QJsonValue jsonActionsInPaletteList = jsonScenario.value("actions_palette");
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
                        if (actionM != nullptr)
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
    // Actions in the timeline
    // ------
    QJsonValue jsonActionsInTimelineList = jsonScenario.value("actions_timeline");
    if (jsonActionsInTimelineList.isArray())
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
                        if (actionM != nullptr)
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
    scenarioImport->actionsList()->append(actionsListToImport);
    scenarioImport->actionsInPaletteList()->append(actionsInPalette);
    scenarioImport->actionsInTimelineList()->append(actionsInTimeLine);

    return scenarioImport;
}


/**
 * @brief Export the actions lists into json object
 * @param actions list
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
    for (ActionM* actionM : actionsList)
    {
        // Create properties
        QJsonObject jsonAgent;
        jsonAgent.insert("uid", actionM->uid());
        jsonAgent.insert("name", actionM->name());
        jsonAgent.insert("validity_duration_type", ValidationDurationTypes::staticEnumToKey(actionM->validityDurationType()));
        jsonAgent.insert("validity_duration_value", actionM->validityDurationString());

        // REVERT
        jsonAgent.insert("shall_revert", actionM->shallRevert());
        jsonAgent.insert("shall_revert_at_validity_end", actionM->shallRevertWhenValidityIsOver());
        jsonAgent.insert("shall_revert_after_time", actionM->shallRevertAfterTime());
        jsonAgent.insert("revert_after_time", actionM->revertAfterTimeString());

        // REARM
        jsonAgent.insert("shall_rearm", actionM->shallRearm());
        jsonAgent.insert("rearm_after_time", actionM->rearmAfterTimeString());

        bool jsonFilled = false;


        //
        // CONDITIONS
        //
        QJsonArray jsonConditionsArray;

        for (ActionConditionVM* conditionVM : actionM->conditionsList()->toList())
        {
            ActionConditionM* actionCondition = conditionVM->modelM();
            jsonFilled = false;

            if ((actionCondition != nullptr) && (actionCondition->agent() != nullptr))
            {
                QJsonObject jsonCondition;
                jsonCondition.insert("type", ActionConditionTypes::staticEnumToKey(conditionVM->conditionType()));
                jsonCondition.insert("agent_name", actionCondition->agent()->name());

                switch (conditionVM->conditionType())
                {
                case ActionConditionTypes::VALUE:
                {
                    IOPValueConditionM* iopValueCondition = qobject_cast<IOPValueConditionM*>(actionCondition);
                    if ((iopValueCondition != nullptr) && (iopValueCondition->agentIOP() != nullptr))
                    {
                        jsonCondition.insert("iop_name", iopValueCondition->agentIOP()->name());
                        jsonCondition.insert("operator", ValueComparisonTypes::staticEnumToKey(iopValueCondition->valueComparisonType()));
                        jsonCondition.insert("value", iopValueCondition->comparisonValue());

                        jsonFilled = true;
                    }

                    break;
                }
                case ActionConditionTypes::AGENT:
                {
                    ConditionOnAgentM* conditionOnAgent = qobject_cast<ConditionOnAgentM*>(actionCondition);
                    if (conditionOnAgent != nullptr)
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


        //
        // EFFECTS
        //
        QJsonArray jsonEffectsArray;

        for (ActionEffectVM* effectVM : actionM->effectsList()->toList())
        {
            ActionEffectM* actionEffect = effectVM->modelM();

            jsonFilled = false;

            if (actionEffect != nullptr)
            {
                QJsonObject jsonEffect;
                jsonEffect.insert("type", ActionEffectTypes::staticEnumToKey(effectVM->effectType()));

                switch (effectVM->effectType())
                {
                case ActionEffectTypes::AGENT:
                {
                    EffectOnAgentM* effectOnAgent = qobject_cast<EffectOnAgentM*>(actionEffect);
                    if ((effectOnAgent != nullptr) && (effectOnAgent->agent() != nullptr))
                    {
                        jsonEffect.insert("agent_name", effectOnAgent->agent()->name());
                        jsonEffect.insert("value", AgentEffectValues::staticEnumToKey(effectOnAgent->agentEffectValue()));

                        jsonFilled = true;
                    }
                    break;
                }
                case ActionEffectTypes::VALUE:
                {
                    IOPValueEffectM* iopValueEffect = qobject_cast<IOPValueEffectM*>(actionEffect);
                    if ((iopValueEffect != nullptr) && (iopValueEffect->agent() != nullptr) && (iopValueEffect->agentIOP() != nullptr) && (iopValueEffect->agentIOP()->firstModel() != nullptr))
                    {
                        jsonEffect.insert("agent_name", iopValueEffect->agent()->name());
                        jsonEffect.insert("iop_type", AgentIOPTypes::staticEnumToKey(iopValueEffect->agentIOP()->firstModel()->agentIOPType()));
                        jsonEffect.insert("iop_name", iopValueEffect->agentIOP()->name());
                        jsonEffect.insert("value", iopValueEffect->value());

                        jsonFilled = true;
                    }

                    break;
                }
                case ActionEffectTypes::MAPPING:
                {
                    MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(actionEffect);
                    if ((mappingEffect != nullptr) && (mappingEffect->agent() != nullptr) && (mappingEffect->input() != nullptr)
                            && (mappingEffect->outputAgent() != nullptr)  && (mappingEffect->output() != nullptr))
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
        if (actionInPalette->modelM() != nullptr)
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
        if ((actionVM != nullptr) && (actionVM->modelM() != nullptr) && (actionVM->startTime() >= 0))
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
            for (QJsonValue jsonValue : recordsValue.toArray())
            {
                if (jsonValue.isObject())
                {
                    QJsonObject jsonRecord = jsonValue.toObject();

                    QJsonValue jsonId = jsonRecord.value("id");
                    QJsonValue jsonName = jsonRecord.value("name_record");
                    QJsonValue jsonBeginDateTime = jsonRecord.value("time_beg");
                    QJsonValue jsonEndDateTime = jsonRecord.value("time_end");

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
 * @brief Create a model of agent Input/Output/Parameter from JSON object
 * @param jsonObject
 * @param agentIOPType
 * @return
 */
AgentIOPM* JsonHelper::_createModelOfAgentIOP(QJsonObject jsonObject, AgentIOPTypes::Value agentIOPType)
{
    AgentIOPM* agentIOP = nullptr;

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
                if (jsonValue.isDouble())
                {
                    int value = jsonValue.toInt();

                    agentIOP->setdefaultValue(QVariant(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not an int";
                }
                break;

            case AgentIOPValueTypes::DOUBLE:
                if (jsonValue.isDouble())
                {
                    double value = jsonValue.toDouble();

                    agentIOP->setdefaultValue(QVariant(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a double";
                }
                break;

            case AgentIOPValueTypes::STRING:
                if (jsonValue.isString())
                {
                    QString value = jsonValue.toString();

                    agentIOP->setdefaultValue(QVariant(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a string";
                }
                break;

            case AgentIOPValueTypes::BOOL:
                if (jsonValue.isString())
                {
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
                /*if (jsonValue.isDouble())
                {
                    //QByteArray value = QByteArray();
                    int value = jsonValue.toInt();

                    agentIOP->setdefaultValue(QVariant(value));
                }
                else*/ if (jsonValue.isString())
                {
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

    if (agentIOP != nullptr)
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
    ElementMappingM* mappingElement = nullptr;

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
 * @param hashFromNameToAgentsGrouped
 * @return
 */
ActionEffectVM* JsonHelper::_parseEffectVMFromJson(QJsonObject jsonEffect,
                                                   QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped)
{
    ActionEffectVM* actionEffectVM = nullptr;

    QJsonValue jsonValue = jsonEffect.value("type");
    if (jsonValue.isString())
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
                    QJsonValue jsonIOPType = jsonEffect.value("iop_type");
                    QJsonValue jsonIOPName = jsonEffect.value("iop_name");

                    if (jsonAgentName.isString() && jsonIOPType.isString() && jsonIOPName.isString())
                    {
                        QString agentName = jsonAgentName.toString();

                        AgentIOPTypes::Value agentIOPType = AgentIOPTypes::INPUT;
                        int nAgentIOPType = AgentIOPTypes::staticEnumFromKey(jsonIOPType.toString());
                        if (nAgentIOPType > -1) {
                            agentIOPType = static_cast<AgentIOPTypes::Value>(nAgentIOPType);
                        }

                        QString agentIOPName = jsonIOPName.toString();

                        if (hashFromNameToAgentsGrouped.contains(agentName))
                        {
                            AgentsGroupedByNameVM* agent = hashFromNameToAgentsGrouped.value(agentName);

                            if (agent != nullptr)
                            {
                                AgentIOPVM* agentIOP = nullptr;

                                switch (agentIOPType)
                                {
                                case AgentIOPTypes::INPUT:
                                {
                                    QList<InputVM*> inputsWithSameName = agent->getInputsListFromName(agentIOPName);
                                    if (!inputsWithSameName.isEmpty()) {
                                        agentIOP = inputsWithSameName.at(0);
                                    }
                                    break;
                                }
                                case AgentIOPTypes::OUTPUT:
                                {
                                    QList<OutputVM*> outputsWithSameName = agent->getOutputsListFromName(agentIOPName);
                                    if (!outputsWithSameName.isEmpty()) {
                                        agentIOP = outputsWithSameName.at(0);
                                    }
                                    break;
                                }
                                case AgentIOPTypes::PARAMETER:
                                {
                                    QList<ParameterVM*> parametersWithSameName = agent->getParametersListFromName(agentIOPName);
                                    if (!parametersWithSameName.isEmpty()) {
                                        agentIOP = parametersWithSameName.at(0);
                                    }
                                    break;
                                }
                                default:
                                    break;
                                }

                                if (agentIOP != nullptr)
                                {
                                    // Create model
                                    IOPValueEffectM* iopValueEffect = new IOPValueEffectM();

                                    // Create view model
                                    actionEffectVM = new ActionEffectVM();
                                    actionEffectVM->seteffectType(ActionEffectTypes::VALUE);

                                    actionEffectVM->setmodelM(iopValueEffect);

                                    // set value
                                    jsonValue = jsonEffect.value("value");
                                    if (jsonValue.isString()) {
                                        iopValueEffect->setvalue(jsonValue.toString());
                                    }

                                    // set agent and agent I/O/P
                                    iopValueEffect->setagent(agent);
                                    iopValueEffect->setagentIOP(agentIOP);
                                }
                            }
                        }
                    }
                    break;
                }
                // AGENT
                case ActionEffectTypes::AGENT:
                {
                    QJsonValue jsonAgentName = jsonEffect.value("agent_name");

                    if (jsonAgentName.isString())
                    {
                        // Check agent name and iop name exists
                        QString agentName = jsonAgentName.toString();

                        if (hashFromNameToAgentsGrouped.contains(agentName))
                        {
                            AgentsGroupedByNameVM* agent = hashFromNameToAgentsGrouped.value(agentName);

                            if (agent != nullptr)
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
                        QString outputAgentName = jsonOutputAgentName.toString();
                        QString outputName = jsonOutputName.toString();
                        QString inputAgentName = jsonInputAgentName.toString();
                        QString inputName = jsonInputName.toString();

                        AgentsGroupedByNameVM* outputAgent = nullptr;
                        AgentIOPVM* output = nullptr;
                        AgentsGroupedByNameVM* inputAgent = nullptr;
                        AgentIOPVM* input = nullptr;

                        // Output agent and output
                        if (hashFromNameToAgentsGrouped.contains(outputAgentName))
                        {
                            outputAgent = hashFromNameToAgentsGrouped.value(outputAgentName);
                            if (outputAgent != nullptr)
                            {
                                QList<OutputVM*> outputsWithSameName = outputAgent->getOutputsListFromName(outputName);
                                if (!outputsWithSameName.isEmpty()) {
                                    output = outputsWithSameName.at(0);
                                }
                            }
                        }

                        // Input agent and input
                        if (hashFromNameToAgentsGrouped.contains(inputAgentName))
                        {
                            inputAgent = hashFromNameToAgentsGrouped.value(inputAgentName);
                            if (inputAgent != nullptr)
                            {
                                QList<InputVM*> inputsWithSameName = inputAgent->getInputsListFromName(inputName);
                                if (!inputsWithSameName.isEmpty()) {
                                    input = inputsWithSameName.at(0);
                                }
                            }
                        }

                        if ((inputAgent != nullptr) && (input != nullptr) && (outputAgent != nullptr) && (output != nullptr))
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
 * @param hashFromNameToAgentsGrouped
 * @return
 */
ActionConditionVM* JsonHelper::_parseConditionsVMFromJson(QJsonObject jsonCondition,
                                                          QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped)
{
    ActionConditionVM* actionConditionVM = nullptr;

    QJsonValue jsonValue = jsonCondition.value("type");
    if (jsonValue.isString())
    {
        int conditionType = ActionConditionTypes::staticEnumFromKey(jsonValue.toString().toUpper());
        if (conditionType >= 0)
        {
            switch (conditionType)
            {
                case ActionConditionTypes::VALUE:
                {
                    QJsonValue jsonAgentName = jsonCondition.value("agent_name");
                    QJsonValue jsonIOPName = jsonCondition.value("iop_name");

                    if (jsonAgentName.isString() && jsonIOPName.isString())
                    {
                        QString agentName = jsonAgentName.toString();

                        /*AgentIOPTypes::Value agentIOPType = AgentIOPTypes::INPUT;
                        int nAgentIOPType = AgentIOPTypes::staticEnumFromKey(jsonIOPType.toString());
                        if (nAgentIOPType > -1) {
                            agentIOPType = static_cast<AgentIOPTypes::Value>(nAgentIOPType);
                        }*/

                        QString agentIOPName = jsonIOPName.toString();

                        if (hashFromNameToAgentsGrouped.contains(agentName))
                        {
                            AgentsGroupedByNameVM* agent = hashFromNameToAgentsGrouped.value(agentName);

                            if (agent != nullptr)
                            {
                                AgentIOPVM* agentIOP = nullptr;

                                /*switch (agentIOPType)
                                {
                                case AgentIOPTypes::INPUT:
                                {
                                    QList<InputVM*> inputsWithSameName = agent->getInputsListFromName(agentIOPName);
                                    if (!inputsWithSameName.isEmpty()) {
                                        agentIOP = inputsWithSameName.at(0);
                                    }
                                    break;
                                }
                                case AgentIOPTypes::OUTPUT:
                                {
                                    QList<OutputVM*> outputsWithSameName = agent->getOutputsListFromName(agentIOPName);
                                    if (!outputsWithSameName.isEmpty()) {
                                        agentIOP = outputsWithSameName.at(0);
                                    }
                                    break;
                                }
                                case AgentIOPTypes::PARAMETER:
                                {
                                    QList<ParameterVM*> parametersWithSameName = agent->getParametersListFromName(agentIOPName);
                                    if (!parametersWithSameName.isEmpty()) {
                                        agentIOP = parametersWithSameName.at(0);
                                    }
                                    break;
                                }
                                default:
                                    break;
                                }*/

                                QList<OutputVM*> outputsWithSameName = agent->getOutputsListFromName(agentIOPName);
                                if (!outputsWithSameName.isEmpty()) {
                                    agentIOP = outputsWithSameName.at(0);
                                }

                                if (agentIOP != nullptr)
                                {
                                    // Create model
                                    IOPValueConditionM* iopValueCondition = new IOPValueConditionM();

                                    // Create view model
                                    actionConditionVM = new ActionConditionVM();
                                    actionConditionVM->setconditionType(ActionConditionTypes::VALUE);

                                    actionConditionVM->setmodelM(iopValueCondition);

                                    // set the operator (value comparison type)
                                    jsonValue = jsonCondition.value("operator");
                                    if (jsonValue.isString())
                                    {
                                        int nValueComparisonType = ValueComparisonTypes::staticEnumFromKey(jsonValue.toString().toUpper());
                                        iopValueCondition->setvalueComparisonType(static_cast<ValueComparisonTypes::Value>(nValueComparisonType));
                                    }

                                    // set the comparison value
                                    jsonValue = jsonCondition.value("value");
                                    if (jsonValue.isString()) {
                                        iopValueCondition->setcomparisonValue(jsonValue.toString());
                                    }

                                    // set agent and agent I/O/P
                                    iopValueCondition->setagent(agent);
                                    iopValueCondition->setagentIOP(agentIOP);
                                }
                            }
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

                        if (hashFromNameToAgentsGrouped.contains(agentName))
                        {
                            AgentsGroupedByNameVM* agent = hashFromNameToAgentsGrouped.value(agentName);

                            if (agent != nullptr)
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
                                conditionOnAgent->setagent(agent);
                            }
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
