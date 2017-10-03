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

#include <model/agentm.h>


/**
 * @brief The JsonHelper class defines a helper to manage JSON definitions of agents
 */
class JsonHelper : public QObject
{
    Q_OBJECT

public:
    explicit JsonHelper(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~JsonHelper();


    /**
     * @brief Create an agent with a definition
     * @param jsonObject
     * @return
     */
    AgentM* createAgentWithRawDefinition(QByteArray jsonByteArray);


signals:

public slots:

private:

    /**
     * @brief Create an agent Input/Output/Parameter
     * @param jsonObject
     * @return
     */
    AgentIOPM* _createAgentIOP(QJsonObject jsonObject);
};

#endif // JSONHELPER_H
