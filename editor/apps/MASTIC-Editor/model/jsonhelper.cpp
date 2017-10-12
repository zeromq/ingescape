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
 * @brief Create a model of agent definition with JSON
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
            QJsonObject jsonSubObject = jsonDefinition.toObject();

            QJsonValue jsonName = jsonSubObject.value("name");
            QJsonValue jsonDescription = jsonSubObject.value("description");
            QJsonValue jsonVersion = jsonSubObject.value("version");
            QJsonValue jsonParameters = jsonSubObject.value("parameters");
            QJsonValue jsonInputs = jsonSubObject.value("inputs");
            QJsonValue jsonOutputs = jsonSubObject.value("outputs");

            if (jsonName.isString() && jsonDescription.isString() && jsonVersion.isString())
            {
                // Create the agent definition
                definition = new DefinitionM(jsonName.toString(), jsonVersion.toString(), jsonDescription.toString());

                if (jsonParameters.isArray()) {
                    foreach (QJsonValue jsonParameter, jsonParameters.toArray()) {
                        if (jsonParameter.isObject())
                        {
                            // Create a model of agent Parameter
                            AgentIOPM* agentParameter = _createModelOfAgentIOP(jsonParameter.toObject());
                            if (agentParameter != NULL) {
                                agentParameter->setagentIOPType(AgentIOPTypes::PARAMETER);
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
                            AgentIOPM* agentInput = _createModelOfAgentIOP(jsonInput.toObject());
                            if (agentInput != NULL) {
                                agentInput->setagentIOPType(AgentIOPTypes::INPUT);
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
                            AgentIOPM* agentOutput = _createModelOfAgentIOP(jsonOutput.toObject());
                            if (agentOutput != NULL) {
                                agentOutput->setagentIOPType(AgentIOPTypes::OUTPUT);
                                definition->outputsList()->append(agentOutput);
                            }
                        }
                    }
                }

                // Generate md5 value for the definition string
                QString md5Hash = QString(QCryptographicHash::hash(byteArrayOfJson, QCryptographicHash::Md5).toHex());
                definition->setmd5Hash(md5Hash);

                //qDebug() << "md5:" << md5Hash;
                //qDebug() << "json:" << jsonDefinition;
            }
        }
    }

    return definition;
}

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
                            ElementMappingM* elementMapping = _createModelOfElementMapping (jsonMap.toObject());
                            if (elementMapping != NULL) {
                                elementMapping->setinputAgent(inputAgentName);
                                agentMapping->elementMappingsList()->append(elementMapping);
                            }
                        }
                    }
                }

                // Generate md5 value for the definition string
                QString md5Hash = QString(QCryptographicHash::hash(byteArrayOfJson, QCryptographicHash::Md5).toHex());
                agentMapping->setmd5Hash(md5Hash);

                //qDebug() << "md5:" << md5Hash;
                //qDebug() << "json:" << jsonMapping;
            }
        }
    }

    return agentMapping;
}


/**
 * @brief Create a model of agent Input/Output/Parameter with JSON
 * @param jsonObject
 * @return
 */
AgentIOPM* JsonHelper::_createModelOfAgentIOP(QJsonObject jsonObject)
{
    AgentIOPM* agentIOP = NULL;

    QJsonValue jsonName = jsonObject.value("name");
    QJsonValue jsonType = jsonObject.value("type");
    QJsonValue jsonValue = jsonObject.value("value");

    if (jsonName.isString() && jsonType.isString())
    {
        // Create the agent Input/Output/Parameter
        agentIOP = new AgentIOPM();

        agentIOP->setname(jsonName.toString());

        int nAgentIOPValueType = AgentIOPValueTypes::staticEnumFromKey(jsonType.toString());
        if (nAgentIOPValueType > -1) {
            agentIOP->setagentIOPValueType(static_cast<AgentIOPValueTypes::Value>(nAgentIOPValueType));

            switch (agentIOP->agentIOPValueType())
            {
            case AgentIOPValueTypes::INTEGER:
                if (jsonValue.isDouble()) {
                    int value = (int)jsonValue.toDouble();

                    agentIOP->setdefaultValue(QVariant(value));
                    agentIOP->setdisplayableDefaultValue(QString::number(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not an int";
                }
                break;

            case AgentIOPValueTypes::DOUBLE:
                if (jsonValue.isDouble()) {
                    double value = jsonValue.toDouble();

                    agentIOP->setdefaultValue(QVariant(value));
                    agentIOP->setdisplayableDefaultValue(QString::number(value));
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a double";
                }
                break;

            case AgentIOPValueTypes::STRING:
                if (jsonValue.isString()) {
                    QString value = jsonValue.toString();

                    agentIOP->setdefaultValue(QVariant(value));
                    agentIOP->setdisplayableDefaultValue(value);
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
                        agentIOP->setdisplayableDefaultValue(strValue);
                    }
                    else {
                        qCritical() << "IOP '" << agentIOP->name() << "': The value '" << strValue << "' is not a bool";
                    }
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a bool";
                }
                break;

            /*case AgentIOPValueTypes::IMPULSION:
                // Nothing to do
                break;*/

            case AgentIOPValueTypes::DATA:
                if (jsonValue.isString()) {
                    QString strValue = jsonValue.toString();
                    QByteArray value = strValue.toLocal8Bit();
                    //QByteArray value = strValue.toUtf8();

                    agentIOP->setdefaultValue(QVariant(value));
                    agentIOP->setdisplayableDefaultValue(strValue);
                }
                else {
                    qCritical() << "IOP '" << agentIOP->name() << "': The value '" << jsonValue << "' is not a data of bytes";
                }
                break;

            default:
                break;
            }

            //qDebug() << agentIOP->name() << "(" << AgentIOPValueTypes::staticEnumToString(agentIOP->agentIOPValueType()) << ")" << agentIOP->displayableDefaultValue();
        }
        else {
            qCritical() << "IOP '" << agentIOP->name() << "': The value type '" << jsonType.toString() << "' is wrong (must be INTEGER, DOUBLE, STRING, BOOL, IMPULSION or DATA)";
        }
    }

    return agentIOP;
}

ElementMappingM* JsonHelper::_createModelOfElementMapping(QJsonObject jsonObject)
{
    ElementMappingM* elementMapping = NULL;

    QJsonValue jsonInputName = jsonObject.value("input_name");
    QJsonValue jsonAgentName = jsonObject.value("agent_name");
    QJsonValue jsonOutputName = jsonObject.value("output_name");

    //All the members need to be completed
    if (jsonInputName.isString() && jsonAgentName.isString() && jsonOutputName.isString())
    {
        elementMapping = new ElementMappingM(NULL,
                                             jsonInputName.toString(),
                                             jsonAgentName.toString(),
                                             jsonOutputName.toString());


    }

    return elementMapping;
}
