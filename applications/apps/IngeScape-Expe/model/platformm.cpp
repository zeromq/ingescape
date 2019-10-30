/*
 *	IngeScape Expe
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "platformm.h"

/**
 * @brief Constructor
 * @param name
 * @param filePath
 * @param parent
 */
PlatformM::PlatformM(QString name,
                     QString filePath,
                     int indexOfAlphabeticOrder,
                     QObject *parent) : QObject(parent),
    _name(name),
    _filePath(filePath),
    _agentNamesList(QStringList()),
    _isLoaded(false),
    _recordState(RecordStates::NOT_RECORDED),
    _currentIndex(indexOfAlphabeticOrder),
    _indexOfAlphabeticOrder(indexOfAlphabeticOrder)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Platform" << _name << "at" << _filePath << "[" << _currentIndex << "]";

    // Initialize the list of agent names from our platform file path
    _initAgentNamesList();

    qDebug() << _agentNamesList.count() << "agents:" << _agentNamesList;
}


/**
 * @brief Destructor
 */
PlatformM::~PlatformM()
{
    qInfo() << "Delete Model of Platform" << _name << "at" << _filePath << "[" << _currentIndex << "]";

}


/**
 * @brief Initialize the list of agent names from our platform file path
 */
void PlatformM::_initAgentNamesList()
{
    QFile jsonFile(_filePath);
    if (jsonFile.exists())
    {
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();

            QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

            QJsonObject jsonRoot = jsonDocument.object();

            // List of agents
            if (jsonRoot.contains("agents"))
            {
                for (QJsonValue jsonIteratorAgent : jsonRoot.value("agents").toArray())
                {
                    if (jsonIteratorAgent.isObject())
                    {
                        QJsonObject jsonAgentsGroupedByName = jsonIteratorAgent.toObject();

                        // Agent name
                        if (jsonAgentsGroupedByName.contains("agentName"))
                        {
                            QJsonValue jsonAgentName = jsonAgentsGroupedByName.value("agentName");
                            if (jsonAgentName.isString()) {
                                _agentNamesList.append(jsonAgentName.toString());
                            }
                        }
                        else {
                            qCritical() << "Missing agent name in file" << _filePath;
                        }
                    }
                }
            }
        }
        else {
            qCritical() << "Can not open file" << _filePath;
        }
    }
    else {
        qWarning() << "There is no file" << _filePath;
    }
}
