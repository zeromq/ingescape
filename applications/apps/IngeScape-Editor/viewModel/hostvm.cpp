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
#include <controller/ingescapenetworkcontroller.h>


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
    _nbAgentsOn(0),
    _canProvideStream(false),
    _isStreaming(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Allows to sort agents executed on our host
    _sortedAgents.setSourceModel(&_agentsList);

    //qInfo() << "New View Model of Host" << _name;

    if ((_modelM != nullptr) && (_modelM->peer() != nullptr))
    {
        _isON = true;

        qInfo() << "New View Model of Host" << _name << "ON (connected to IngeScape)";

        if (_name != _modelM->name()) {
            qCritical() << "The name of the view model of host" << _name << "does not correspond to the name of the model" << _modelM->name();
        }

        if (!_modelM->peer()->streamingPort().isEmpty())
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
    //    _agentsList.clear();
    // To disconnect signals ...
    for (AgentM* agent : _agentsList)
    {
        removeAgentModelFromList(agent);
    }


    _nbAgentsOn = 0;
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
    if ((agent != nullptr) && (agent->peer() != nullptr)
            && (_modelM != nullptr) && (_modelM->peer() != nullptr))
    {
        QStringList message = {
            command_StartPeer,
            agent->peer()->commandLine()
        };

        // Send the message "Start Agent" to this host (IngeScape Launcher)
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_modelM->peer()->uid(), message);
    }
}


/**
 * @brief Stop an agent
 * @param agent
 */
void HostVM::stopAgent(AgentM* agent)
{
    if ((agent != nullptr) && (agent->peer() != nullptr))
    {
        // Send the message "STOP PEER" to this host (IngeScape Launcher)
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(agent->peer()->uid(), command_StopPeer);
    }
}


/**
 * @brief Add an agent model to list and connect to its isONChanged signal
 * @param agent
 */
void HostVM::addAgentModelToList(AgentM* agent)
{
    if (!_agentsList.contains(agent))
    {
        // Update flag about number of agents ON
        if (agent->isON()) {
            setnbAgentsOn(_nbAgentsOn + 1);
        }

        // Connect to isON changed signal of our new agent
        connect(agent, &AgentM::isONChanged, this, &HostVM::_onStatusONChanged);

        // Add this agent to the host
        _agentsList.append(agent);

        qDebug() << "Add agent" << agent->name() << "to host" << _name;
    }
}


/**
 * @brief Remove an agent model from list and disconnect from its isONChanged signal
 * @param agent
 */
void HostVM::removeAgentModelFromList(AgentM* agent)
{
    if (_agentsList.contains(agent))
    {
        // Update flag about number of agents ON
        if (agent->isON()) {
            setnbAgentsOn(_nbAgentsOn - 1);
        }

        // Disconnect from isON changed signal of our agent
        disconnect(agent, &AgentM::isONChanged, this, &HostVM::_onStatusONChanged);

        _agentsList.remove(agent);

        qDebug() << "Remove agent" << agent->name() << "from host" << _name;
    }
}


/**
 * @brief Slot called when the status ON changed to OFF in one agent model of _agentsList
 */
void HostVM::_onStatusONChanged(bool isOn) {
    // Update flag about number of agents ON
    isOn ? setnbAgentsOn(_nbAgentsOn + 1) : setnbAgentsOn(_nbAgentsOn - 1);
}
