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
#include <model/definitionm.h>


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
     * @brief Create a model of agent definition with JSON
     * @param byteArrayOfJson
     * @return
     */
    DefinitionM* createModelOfDefinition(QByteArray byteArrayOfJson);


Q_SIGNALS:

public Q_SLOTS:

private:

    /**
     * @brief Create a model of agent Input/Output/Parameter with JSON
     * @param jsonObject
     * @return
     */
    AgentIOPM* _createModelOfAgentIOP(QJsonObject jsonObject);
};

#endif // JSONHELPER_H
