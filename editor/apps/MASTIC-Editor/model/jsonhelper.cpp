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
QList<QPair<QString, DefinitionM*>> JsonHelper::initAgentsList(QByteArray byteArrayOfJson)
{
    QList<QPair<QString, DefinitionM*>> agentsListToImport;

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
                    DefinitionM* definition = _createModelOfDefinitionFromJSON(jsonDefinition.toObject());
                    if (definition != NULL)
                    {
                        qDebug() << "Initialize agent" << jsonName.toString() << "with definition" << definition->name();

                        // Create a pair with agent name and definition
                        QPair<QString, DefinitionM*> pair;
                        pair.first = jsonName.toString();
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
 * @brief Create a model of agent definition from JSON file
 * @param byteArrayOfJson
 * @return
 */
DefinitionM* JsonHelper::createModelOfDefinition(QByteArray byteArrayOfJson)
{
    DefinitionM* definition = NULL;

    QJsonDocument jsonAgentDefinition = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonAgentDefinition.isObject())
    {
        QJsonObject jsonObject = jsonAgentDefinition.object();

        QJsonValue jsonDefinition = jsonObject.value("definition");
        if (jsonDefinition.isObject())
        {
            // Create a model of agent definition from JSON object
            definition = _createModelOfDefinitionFromJSON(jsonDefinition.toObject());
        }
    }

    return definition;
}


/**
 * @brief Export the agents list
 * @param agentsListToExport list of pairs <agent name, definition>
 */
QByteArray JsonHelper::exportAgentsList(QList<QPair<QString, DefinitionM*>> agentsListToExport)
{
    QJsonArray jsonArray;

    for (int i = 0; i < agentsListToExport.count(); i++)
    {
        QPair<QString, DefinitionM*> pair = agentsListToExport.at(i);
        QString agentName = pair.first;
        DefinitionM* definition = pair.second;

        if (!agentName.isEmpty() && (definition != NULL))
        {
            QJsonObject jsonAgent;
            jsonAgent.insert("agentName", agentName);

            QJsonObject jsonDefinition;
            jsonDefinition.insert("name", definition->name());
            jsonDefinition.insert("version", definition->version());
            jsonDefinition.insert("description", definition->description());

            QJsonArray jsonInputs;
            foreach (AgentIOPM* agentIOP, definition->inputsList()->toList()) {
                if (agentIOP != NULL) {
                    // Get JSON object from the agent Input/Output/Parameter
                    QJsonObject jsonAgentIOP = _getJsonFromAgentIOP(agentIOP);

                    jsonInputs.append(jsonAgentIOP);
                }
            }

            QJsonArray jsonOutputs;
            foreach (AgentIOPM* agentIOP, definition->outputsList()->toList()) {
                if (agentIOP != NULL) {
                    // Get JSON object from the agent Input/Output/Parameter
                    QJsonObject jsonAgentIOP = _getJsonFromAgentIOP(agentIOP);

                    jsonOutputs.append(jsonAgentIOP);
                }
            }

            QJsonArray jsonParameters;
            foreach (AgentIOPM* agentIOP, definition->parametersList()->toList()) {
                if (agentIOP != NULL) {
                    // Get JSON object from the agent Input/Output/Parameter
                    QJsonObject jsonAgentIOP = _getJsonFromAgentIOP(agentIOP);

                    jsonParameters.append(jsonAgentIOP);
                }
            }

            jsonDefinition.insert("inputs", jsonInputs);
            jsonDefinition.insert("outputs", jsonOutputs);
            jsonDefinition.insert("parameters", jsonParameters);

            jsonAgent.insert("definition", jsonDefinition);

            jsonArray.append(jsonAgent);
        }
    }

    QJsonDocument jsonDocument = QJsonDocument(jsonArray);

    return jsonDocument.toJson();
}


/**
 * @brief Create a model of agent mapping with JSON and the input agent name corresponding
 * @param inputAgentName, byteArrayOfJson
 * @return
 */
AgentMappingM* JsonHelper::createModelOfAgentMapping(QString inputAgentName, QByteArray byteArrayOfJson)
{
    AgentMappingM* agentMapping = NULL;

    QJsonDocument jsonAgentMapping = QJsonDocument::fromJson(byteArrayOfJson);

    if (jsonAgentMapping.isObject())
    {
        QJsonObject jsonObject = jsonAgentMapping.object();

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
                    foreach (QJsonValue jsonMap, jsonMappingOut.toArray()) {
                        if (jsonMap.isObject())
                        {
                            ElementMappingM* elementMapping = _createModelOfElementMapping(inputAgentName, jsonMap.toObject());
                            if (elementMapping != NULL) {
                                agentMapping->elementMappingsList()->append(elementMapping);
                            }
                        }
                    }
                }
            }
        }
    }

    return agentMapping;
}


/**
 * @brief Create a model of agent definition from JSON object
 * @param jsonDefinition
 * @return
 */
DefinitionM* JsonHelper::_createModelOfDefinitionFromJSON(QJsonObject jsonDefinition)
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
                    int value = (int)jsonValue.toDouble();

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
    ElementMappingM* elementMapping = NULL;

    QJsonValue jsonInputName = jsonObject.value("input_name");
    QJsonValue jsonAgentName = jsonObject.value("agent_name");
    QJsonValue jsonOutputName = jsonObject.value("output_name");

    //All the members need to be completed
    if (jsonInputName.isString() && jsonAgentName.isString() && jsonOutputName.isString())
    {
        elementMapping = new ElementMappingM(inputAgentName,
                                             jsonInputName.toString(),
                                             jsonAgentName.toString(),
                                             jsonOutputName.toString());
    }

    return elementMapping;
}


/**
 * @brief Initialize actions list from JSON file
 * @param byteArrayOfJson
 * @return
 */
QPair< QPair< QList<ActionM*>, QList<ActionInPaletteVM*> > , QList<ActionVM*> > JsonHelper::initActionsList(QByteArray byteArrayOfJson, QList<AgentInMappingVM*> listAgentsInMapping)
{
    QPair< QPair< QList<ActionM*>, QList<ActionInPaletteVM*> > , QList<ActionVM*> > parsedResult;
    QList<ActionM*> actionsListToImport;
    QList<ActionInPaletteVM*> actionsInPalette;
    QList<ActionVM*> actionsInTimeLine;

    QHash<QString, ActionM*> mapActionsMFromActionName;

    QJsonDocument jsonFileRoot = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonFileRoot.isObject())
    {
        QJsonObject jsonActionsRoot = jsonFileRoot.object();

        // ------
        // Actions in table list
        // ------
        QJsonValue jsonActionsList = jsonActionsRoot.value("actions");
        if(jsonActionsList.isArray())
        {
            foreach (QJsonValue jsonTmp, jsonActionsList.toArray())
            {
                if (jsonTmp.isObject())
                {
                    ActionM* actionM = NULL;

                    QJsonObject jsonAction = jsonTmp.toObject();
                    QJsonValue jsonName = jsonAction.value("name");
                    if(jsonName.isString())
                    {
                        // Create the model
                        actionM = new ActionM(jsonName.toString());

                        QJsonValue jsonValue = jsonAction.value("validity_duration_type");
                        if(jsonValue.isString())
                        {
                            int validationDurationType = ValidationDurationType::staticEnumFromKey(jsonValue.toString());
                            actionM->setvalidityDurationType((ValidationDurationType::Value)validationDurationType);
                        }

                        jsonValue = jsonAction.value("validity_duration_value");
                        if(jsonValue.isString())
                        {
                            actionM->setvalidityDurationString(jsonValue.toString());
                        }

                        jsonValue = jsonAction.value("shall_revert");
                        if(jsonValue.isBool())
                        {
                            actionM->setshallRevert(jsonValue.toBool());
                        }

                        jsonValue = jsonAction.value("shall_revert_at_validity_end");
                        if(jsonValue.isBool())
                        {
                            actionM->setshallRevertWhenValidityIsOver(jsonValue.toBool());
                        }

                        jsonValue = jsonAction.value("shall_revert_after_time");
                        if(jsonValue.isBool())
                        {
                            actionM->setshallRevertAfterTime(jsonValue.toBool());
                        }

                        jsonValue = jsonAction.value("shall_rearm");
                        if(jsonValue.isBool())
                        {
                            actionM->setshallRearm(jsonValue.toBool());
                        }

                        jsonValue = jsonAction.value("revert_after_time");
                        if(jsonValue.isString())
                        {
                            actionM->setrevertAfterTimeString(jsonValue.toString());
                        }

                        QJsonValue jsonEffectsList = jsonAction.value("effects");
                        if(jsonEffectsList.isArray())
                        {
                            foreach (QJsonValue jsonEffect, jsonEffectsList.toArray())
                            {
                                if (jsonEffect.isObject())
                                {
                                    QJsonObject jsonEffectObj = jsonEffect.toObject();
                                    ActionEffectVM* effectVM = _parseEffectVMFromJson(jsonEffectObj, listAgentsInMapping);

                                    if(effectVM != NULL)
                                    {
                                        actionM->effectsList()->append(effectVM);
                                    }
                                }
                            }
                        }

                        QJsonValue jsonConditionsList = jsonAction.value("conditions");
                        if(jsonConditionsList.isArray())
                        {
                            foreach (QJsonValue jsonCondition, jsonConditionsList.toArray())
                            {
                                if (jsonCondition.isObject())
                                {
                                    QJsonObject jsonConditionObj = jsonCondition.toObject();
                                    ActionConditionVM* conditionVM = _parseConditionsVMFromJson(jsonConditionObj, listAgentsInMapping);

                                    if(conditionVM != NULL)
                                    {
                                        actionM->conditionsList()->append(conditionVM);
                                    }
                                }
                            }
                        }

                    }

                    if(actionM != NULL)
                    {
                        actionsListToImport.append(actionM);
                        mapActionsMFromActionName.insert(actionM->name(),actionM);
                    }
                }
            }
        }


        // ------
        // Actions in palette
        // ------
        QJsonValue jsonActionsInPaletteList = jsonActionsRoot.value("actions_palette");
        if(jsonActionsInPaletteList.isArray())
        {
            foreach (QJsonValue jsonTmp, jsonActionsInPaletteList.toArray())
            {
                if (jsonTmp.isObject())
                {
                    QJsonObject jsonActionInPalette = jsonTmp.toObject();
                    QJsonValue jsonActionName = jsonActionInPalette.value("action_name");
                    QJsonValue jsonActionIndex = jsonActionInPalette.value("index");
                    if(jsonActionName.isString())
                    {
                        QString actionName = jsonActionName.toString();
                        if(mapActionsMFromActionName.contains(actionName))
                        {
                            ActionM * actionM = mapActionsMFromActionName.value(actionName);
                            if(actionM != NULL)
                            {
                                int index = jsonActionIndex.toInt();
                                if(index >= 0 && index < 9)
                                {
                                    // Add action in palette
                                    actionsInPalette.append(new ActionInPaletteVM(actionM,index));
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
        QJsonValue jsonActionsInTimelineList = jsonActionsRoot.value("actions_timeline");
        if(jsonActionsInTimelineList.isArray())
        {
            foreach (QJsonValue jsonTmp, jsonActionsInTimelineList.toArray())
            {
                if (jsonTmp.isObject())
                {
                    QJsonObject jsonActionInTimeline = jsonTmp.toObject();
                    QJsonValue jsonActionName = jsonActionInTimeline.value("action_name");
                    QJsonValue jsonActionStartTime = jsonActionInTimeline.value("start_time");
                    QJsonValue jsonActionColor = jsonActionInTimeline.value("color");
                    QJsonValue jsonActionLineInTimeline = jsonActionInTimeline.value("line_number");
                    if(jsonActionName.isString() && jsonActionStartTime.isDouble())
                    {
                        QString actionName = jsonActionName.toString();
                        if(mapActionsMFromActionName.contains(actionName))
                        {
                            ActionM * actionM = mapActionsMFromActionName.value(actionName);
                            if(actionM != NULL)
                            {
                                ActionVM * actionVM = new ActionVM(actionM,jsonActionStartTime.toDouble());
                                if(jsonActionColor.isString())
                                {
                                    actionVM->setcolor(jsonActionColor.toString());
                                }

                                // Set line in timeline
                                actionVM->setlineInTimeLine(jsonActionLineInTimeline.toInt());

                                // Add our action from timeline
                                actionsInTimeLine.append(actionVM);
                            }
                        }

                    }
                }
            }
        }
    }

    // Set results
    parsedResult.first.first.append(actionsListToImport);
    parsedResult.first.second.append(actionsInPalette);
    parsedResult.second.append(actionsInTimeLine);

    return parsedResult;
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
        if(effectType >= 0)
        {
            switch (effectType)
            {
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
                            if(iterator->name() == agentName)
                            {
                                agent = iterator;
                                // Go through the inputs
                                foreach (InputVM* inputVM, iterator->inputsList()->toList())
                                {
                                    if(found == false && inputVM->name() == agentIOPName)
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
                                foreach (OutputVM* outputVM, iterator->outputsList()->toList())
                                {
                                    if(found == false && outputVM->name() == agentIOPName)
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

                        if(agent != NULL && iopAgentM != NULL)
                        {
                            // Create model
                            IOPValueEffectM* iopEffectM = new IOPValueEffectM();

                            // Create view model
                            actionEffectVM = new ActionEffectVM();
                            actionEffectVM->seteffectType(ActionEffectTypes::VALUE);

                            actionEffectVM->setmodelM(iopEffectM);

                            // set value
                            jsonValue = jsonEffect.value("value");
                            if(jsonValue.isString())
                            {
                                iopEffectM->setvalue(jsonValue.toString());
                            }

                            // Set the list of agent iop
                            iopEffectM->iopMergedList()->append(listIOPAgents);

                            // set agent
                            iopEffectM->setagent(agent);
                            iopEffectM->setagentIOP(iopAgentM);
                        }
                    }

                    break;
                }
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
                            if (iterator->name() == agentName)
                            {
                                agent = iterator;
                                break;
                            }
                        }

                        if(agent != NULL)
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
                                int agentEffectValue = AgentEffectValues::staticEnumFromKey(jsonValue.toString().toUpper());
                                effectOnAgent->setagentEffectValue(static_cast<AgentEffectValues::Value>(agentEffectValue));
                            }

                            // set agent
                            effectOnAgent->setagent(agent);

                        }
                    }
                    break;
                }
                case ActionEffectTypes::MAPPING:
                {
                    QJsonValue jsonFromAgentName = jsonEffect.value("from_agent_name");
                    QJsonValue jsonFromIOPName = jsonEffect.value("from_iop_name");
                    QJsonValue jsonToAgentName = jsonEffect.value("to_agent_name");
                    QJsonValue jsonToIOPName = jsonEffect.value("to_iop_name");

                    if (jsonFromAgentName.isString() && jsonFromIOPName.isString()
                            && jsonToAgentName.isString() && jsonToIOPName.isString())
                    {
                        // Check agent name and iop name exists
                        QString fromAgentName = jsonFromAgentName.toString();
                        QString fromAgentIOPName = jsonFromIOPName.toString();
                        QString toAgentName = jsonToAgentName.toString();
                        QString toAgentIOPName = jsonToIOPName.toString();

                        AgentInMappingVM* fromAgentM = NULL;
                        AgentIOPM* output = NULL;
                        AgentInMappingVM* toAgentM = NULL;
                        AgentIOPM* input = NULL;
                        bool found = false;

                        QList<AgentIOPM*> outputsList;
                        QList<AgentIOPM*> inputsList;

                        foreach (AgentInMappingVM* agent, listAgentsInMapping)
                        {
                            if (agent->name() == fromAgentName)
                            {
                                fromAgentM = agent;
                                found = false;

                                // Go through the inputs
                                foreach (InputVM* inputVM, agent->inputsList()->toList())
                                {
                                    if(found == false && inputVM->name() == fromAgentIOPName)
                                    {
                                        output = inputVM->firstModel();
                                        found = true;
                                    }

                                    if (inputVM->firstModel() != NULL) {
                                        outputsList.append(inputVM->firstModel());
                                    }
                                }

                                // Go through the outputs
                                foreach (OutputVM* outputVM, agent->outputsList()->toList())
                                {
                                    if(found == false && outputVM->name() == fromAgentIOPName)
                                    {
                                        output = outputVM->firstModel();
                                        found = true;
                                    }

                                    if (outputVM->firstModel() != NULL) {
                                        outputsList.append(outputVM->firstModel());
                                    }
                                }
                            }

                            if (agent->name() == toAgentName)
                            {
                                toAgentM = agent;
                                found = false;

                                // Go through the inputs
                                foreach (InputVM* inputVM, agent->inputsList()->toList())
                                {
                                    if(found == false && inputVM->name() == toAgentIOPName)
                                    {
                                        input = inputVM->firstModel();
                                        found = true;
                                    }

                                    if (inputVM->firstModel() != NULL) {
                                        inputsList.append(inputVM->firstModel());
                                    }
                                }

                                // Go through the outputs
                                foreach (OutputVM* outputVM, agent->outputsList()->toList())
                                {
                                    if(found == false && outputVM->name() == toAgentIOPName)
                                    {
                                        input = outputVM->firstModel();
                                        found = true;
                                    }

                                    if (outputVM->firstModel() != NULL) {
                                        inputsList.append(outputVM->firstModel());
                                    }
                                }
                            }
                        }

                        if ((fromAgentM != NULL) && (output != NULL) && (toAgentM != NULL) && (input != NULL))
                        {
                            // Create model
                            MappingEffectM* mappingEffectM = new MappingEffectM();

                            // Create view model
                            actionEffectVM = new ActionEffectVM();
                            actionEffectVM->seteffectType(ActionEffectTypes::MAPPING);
                            actionEffectVM->setmodelM(mappingEffectM);

                            // set from agent
                            mappingEffectM->setagent(fromAgentM);
                            mappingEffectM->setoutput(output);
                            mappingEffectM->setinputAgent(toAgentM);
                            mappingEffectM->setinput(input);

                            // set value
                            jsonValue = jsonEffect.value("value");
                            if (jsonValue.isString()) {
                                int mappingEffectValue = MappingEffectValues::staticEnumFromKey(jsonValue.toString().toUpper());
                                mappingEffectM->setmappingEffectValue(static_cast<MappingEffectValues::Value>(mappingEffectValue));
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
        int conditionType = ActionConditionType::staticEnumFromKey(jsonValue.toString().toUpper());
        if(conditionType >= 0)
        {
            switch (conditionType)
            {
                case ActionConditionType::VALUE:
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
                                    if(found == false && inputVM->name() == agentIOPName)
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
                                    if(found == false && outputVM->name() == agentIOPName)
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

                        if(agentM != NULL && iopAgentM != NULL)
                        {
                            // Create model
                            IOPValueConditionM* iopConditionM = new IOPValueConditionM();

                            // Create view model
                            actionConditionVM = new ActionConditionVM();
                            actionConditionVM->setconditionType(ActionConditionType::VALUE);

                            actionConditionVM->setcondition(iopConditionM);

                            // set operator
                            jsonValue = jsonCondition.value("operator");
                            if(jsonValue.isString())
                            {
                                iopConditionM->setcomparison((ActionComparisonValueType::Value)ActionComparisonValueType::staticEnumFromKey(jsonValue.toString().toUpper()));
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
                            iopConditionM->setagentModel(agentM);
                            iopConditionM->setagentIOP(iopAgentM);                            
                        }
                    }

                    break;
                }
                case ActionConditionType::AGENT:
                {
                    QJsonValue jsonAgentName = jsonCondition.value("agent_name");
                    if(jsonAgentName.isString())
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

                        if(agentM != NULL)
                        {
                            // Create model
                            ActionConditionM* actionConditionM = new ActionConditionM();

                            // Create view model
                            actionConditionVM = new ActionConditionVM();
                            actionConditionVM->setconditionType(ActionConditionType::AGENT);
                            actionConditionVM->setcondition(actionConditionM);

                            // set value
                            jsonValue = jsonCondition.value("value");
                            if(jsonValue.isString())
                            {
                                actionConditionM->setcomparison((ActionComparisonValueType::Value)ActionComparisonValueType::staticEnumFromKey(jsonValue.toString().toUpper()));
                            }

                            // set agent
                            actionConditionM->setagentModel(agentM);
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

/**
 * @brief Create an action condition VM from JSON object
 * @param jsonObject
 * @param list of agents in mapping
 * @return
 */
QByteArray JsonHelper::exportScenario(QList<ActionM*> actionsList, QList<ActionInPaletteVM*> actionsInPaletteList, QList<ActionVM*> actionsInTimeLine)
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
        jsonAgent.insert("name",actionM->name());
        jsonAgent.insert("validity_duration_type",ValidationDurationType::staticEnumToKey(actionM->validityDurationType()));
        jsonAgent.insert("validity_duration_value",actionM->validityDurationString());
        jsonAgent.insert("shall_revert",actionM->shallRevert());
        jsonAgent.insert("shall_revert_at_validity_end",actionM->shallRevertWhenValidityIsOver());
        jsonAgent.insert("shall_revert_after_time",actionM->shallRevertAfterTime());
        jsonAgent.insert("revert_after_time",actionM->revertAfterTimeString());
        jsonAgent.insert("shall_rearm",actionM->shallRearm());

        bool jsonFilled = false;

        QJsonArray jsonConditionsArray;
        // Create conditions view models
        foreach (ActionConditionVM* conditionVM, actionM->conditionsList()->toList())
        {
            ActionConditionM* actionCondition = conditionVM->condition();
            jsonFilled = false;

            if(actionCondition != NULL)
            {
                QJsonObject jsonCondition;
                jsonCondition.insert("type",ActionConditionType::staticEnumToKey(conditionVM->conditionType()));
                jsonCondition.insert("agent_name", actionCondition->agentModel()->name());

                switch (conditionVM->conditionType())
                {
                case ActionConditionType::VALUE:
                {
                    IOPValueConditionM* iopCondition = qobject_cast<IOPValueConditionM*>(actionCondition);
                    if(iopCondition != NULL && iopCondition->agentIOP() != NULL)
                    {
                        jsonCondition.insert("iop_name", iopCondition->agentIOP()->name());
                        jsonCondition.insert("operator", ActionComparisonValueType::staticEnumToKey(iopCondition->comparison()));
                        jsonCondition.insert("value", iopCondition->value());

                        jsonFilled = true;
                    }

                    break;
                }
                case ActionConditionType::AGENT:
                {
                    if(actionCondition->agentModel() != NULL)
                    {
                        jsonCondition.insert("value", ActionComparisonValueType::staticEnumToKey(actionCondition->comparison()));

                        jsonFilled = true;
                    }
                    break;
                }
                default:
                    break;
                }

                if(jsonFilled == true)
                {
                    jsonConditionsArray.append(jsonCondition);
                }

            }
        }

        // Insert the list of conditions
        jsonAgent.insert("conditions",jsonConditionsArray);

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
                    if ((mappingEffect != NULL) && (mappingEffect->agent() != NULL)
                            && (mappingEffect->inputAgent() != NULL) && (mappingEffect->input() != NULL) && (mappingEffect->output() != NULL))
                    {
                        jsonEffect.insert("from_agent_name", mappingEffect->agent()->name());
                        jsonEffect.insert("from_iop_name", mappingEffect->output()->name());
                        jsonEffect.insert("to_agent_name", mappingEffect->inputAgent()->name());
                        jsonEffect.insert("to_iop_name", mappingEffect->input()->name());
                        jsonEffect.insert("value", MappingEffectValues::staticEnumToKey(mappingEffect->mappingEffectValue()));

                        jsonFilled = true;
                    }
                    break;
                }
                default:
                    break;
                }

                if(jsonFilled == true)
                {
                    jsonEffectsArray.append(jsonEffect);
                }
            }
        }

        // Insert the list of effects
        jsonAgent.insert("effects",jsonEffectsArray);

        jsonActionsArray.append(jsonAgent);
    }
    jsonScenario.insert("actions",jsonActionsArray);

    // ----
    // Actions list in palette
    // ----
    QJsonArray jsonActionsInPaletteArray;
    foreach (ActionInPaletteVM* actionInPalette, actionsInPaletteList)
    {
        if(actionInPalette->actionModel() != NULL)
        {
            QJsonObject jsonActionsInPalette;
            jsonActionsInPalette.insert("index",actionInPalette->indexInPanel());
            jsonActionsInPalette.insert("action_name",actionInPalette->actionModel()->name());

            jsonActionsInPaletteArray.append(jsonActionsInPalette);
        }
    }
    jsonScenario.insert("actions_palette",jsonActionsInPaletteArray);

    // ----
    // Actions list in timeline
    // ----
    QJsonArray jsonActionsInTimelineArray;
    foreach (ActionVM* actionVM, actionsInTimeLine)
    {
        if(actionVM->actionModel() != NULL && actionVM->startTime() >= 0)
        {
            QJsonObject jsonActionsInTimeLine;
            jsonActionsInTimeLine.insert("action_name",actionVM->actionModel()->name());
            jsonActionsInTimeLine.insert("start_time",actionVM->startTime());
            jsonActionsInTimeLine.insert("color",actionVM->color().name());
            jsonActionsInTimeLine.insert("line_number",actionVM->lineInTimeLine());

            jsonActionsInTimelineArray.append(jsonActionsInTimeLine);
        }

    }
    jsonScenario.insert("actions_timeline",jsonActionsInTimelineArray);

    QJsonDocument jsonDocument = QJsonDocument(jsonScenario);

    return jsonDocument.toJson();
}
