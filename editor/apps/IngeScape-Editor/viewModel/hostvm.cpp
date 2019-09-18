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
 *      Bruno Lemenicier <lemenicier@ingenuity.io>
 *      Alexandre Lemort <lemort@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "hostvm.h"

#include <QQmlEngine>


/**
 * @brief Constructor
 * @param name
 * @param model
 * @param parent
 */
HostVM::HostVM(QString name,
               HostM* model,
               QObject *parent) : QObject(parent),
    _name(name),
    _modelM(model),
    _isON(false),
    _canProvideStream(false),
    _isStreaming(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Agents are sorted on their name (alphabetical order)
    //_agentsList.setSortProperty("name");
    //_agentsList.setSortProperty("isON");
    //_agentsList.setSortOrder(Qt::DescendingOrder);
    _sortedAgents.setSourceModel(_agentsList);

    //qInfo() << "New View Model of Host" << _name;

    if (_modelM != nullptr)
    {
        _isON = true;

        qInfo() << "New View Model of Host" << _name << "ON (connected to IngeScape)";

        if (_name != _modelM->name()) {
            qCritical() << "The name of the view model of host" << _name << "does not correspond to the name of the model" << _modelM->name();
        }

        if (!_modelM->streamingPort().isEmpty())
        {
            _canProvideStream = true;
        }
    }
    else {
        qInfo() << "New View Model of Host" << _name << "OFF (dis-connected from IngeScape)";
    }
}


/**
 * @brief Destructor
 */
HostVM::~HostVM()
{
    qInfo() << "Delete View Model of Host" << _name;

    if (_modelM != nullptr) {
        setmodelM(nullptr);
    }

    // Clear the list of agents
    //_agentsList.clear();
    _sortedAgents.clear();
}


/**
 * @brief Setter for proprty "model"
 * @param value
 */
void HostVM::setmodelM(HostM *value)
{
    if (_modelM != value)
    {
        _modelM = value;

        // The real host enter the network
        if (_modelM != nullptr) {
            setisON(true);
        }
        // The real host leave the network
        else {
            setisON(false);
        }

        Q_EMIT modelMChanged(value);
    }
}


/**
 * @brief Change the stream state of our host
 */
void HostVM::changeStreamState()
{
    // is streaming => request streaming end
    if (_isStreaming)
    {
        //Q_EMIT commandAskedToHost("STOP_STREAMING", _name, "");
        setisStreaming(false);
    }
    // is not streaming => request streaming
    else
    {
        //Q_EMIT commandAskedToHost("START_STREAMING", _name, "");
        setisStreaming(true);
    }
}


/**
 * @brief Start an agent
 * @param agent
 */
void HostVM::startAgent(AgentM* agent)
{
    if ((agent != nullptr) && (_modelM != nullptr))
    {
         Q_EMIT commandAskedToLauncher(_modelM->peerId(), command_StartAgent, agent->commandLine());
    }
}


/**
 * @brief Stop an agent
 * @param agent
 */
void HostVM::stopAgent(AgentM* agent)
{
    if (agent != nullptr)
    {
        QStringList peerIdsList = QStringList(agent->peerId());

        Q_EMIT commandAskedToAgent(peerIdsList, command_StopAgent);
    }
}

