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

            //qDebug() << agentIOP->name() << "(" << AgentIOPValueTypes::staticEnumToString(agentIOP->agentIOPValueType()) << ")" << agentIOP->displayableDefaultValue();
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
        jsonAgentIOP.insert("type", AgentIOPValueTypes::staticEnumToString(agentIOP->agentIOPValueType()));

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
