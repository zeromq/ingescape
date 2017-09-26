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
 * @brief Create an agent with a definition
 * @param jsonObject
 * @return
 */
AgentM* JsonHelper::createAgentWithDefinition(QJsonObject jsonObject)
{
    AgentM* agent = NULL;

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

        if (jsonName.isString())
        {
            // Create the agent
            agent = new AgentM();

            agent->setname(jsonName.toString());

            if (!jsonDescription.isNull() && jsonDescription.isString()) {
                agent->setdescription(jsonDescription.toString());
            }

            if (!jsonVersion.isNull() && jsonVersion.isString()) {
                agent->setversion(jsonVersion.toString());
            }

            //qDebug() << agent->name() << "with version" << agent->version() << "and description" << agent->description();

            if (jsonParameters.isArray()) {
                foreach (QJsonValue jsonParameter, jsonParameters.toArray()) {
                    if (jsonParameter.isObject())
                    {
                        // Create an agent Parameter
                        AgentIOPM* agentParameter = _createAgentIOP(jsonParameter.toObject());
                        if (agentParameter != NULL) {
                            agentParameter->setagentIOPType(AgentIOPTypes::PARAMETER);
                            agent->parametersList()->append(agentParameter);
                        }
                    }
                }
            }

            if (jsonInputs.isArray()) {
                foreach (QJsonValue jsonInput, jsonInputs.toArray()) {
                    if (jsonInput.isObject())
                    {
                        // Create an agent Input
                        AgentIOPM* agentInput = _createAgentIOP(jsonInput.toObject());
                        if (agentInput != NULL) {
                            agentInput->setagentIOPType(AgentIOPTypes::INPUT);
                            agent->inputsList()->append(agentInput);
                        }
                    }
                }
            }

            if (jsonOutputs.isArray()) {
                foreach (QJsonValue jsonOutput, jsonOutputs.toArray()) {
                    if (jsonOutput.isObject())
                    {
                        // Create an agent Output
                        AgentIOPM* agentOutput = _createAgentIOP(jsonOutput.toObject());
                        if (agentOutput != NULL) {
                            agentOutput->setagentIOPType(AgentIOPTypes::OUTPUT);
                            agent->outputsList()->append(agentOutput);
                        }
                    }
                }
            }
        }
    }

    return agent;
}


/**
 * @brief Create an agent Input/Output/Parameter
 * @param jsonObject
 * @return
 */
AgentIOPM* JsonHelper::_createAgentIOP(QJsonObject jsonObject)
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
