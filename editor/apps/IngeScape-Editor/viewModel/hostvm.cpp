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
 * @param model
 * @param parent
 */
HostVM::HostVM(HostM* model, QObject *parent) : QObject(parent),
    _name(""),
    _modelM(model),
    _canProvideStream(false),
    _isStreaming(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Agents are sorted on their name (alphabetical order)
    //_agentsList.setSortProperty("name");
    _agentsList.setSortProperty("isON");
    _agentsList.setSortOrder(Qt::DescendingOrder);

    if (_modelM != nullptr)
    {
        _name = _modelM->name();

        qInfo() << "New View Model of Host" << _name;

        if (!_modelM->streamingPort().isEmpty())
        {
            _canProvideStream = true;
        }
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
    _agentsList.clear();
}


/**
 * @brief Change the state of our agent
 */
void HostVM::changeState()
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

