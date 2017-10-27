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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "agentvm.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param model
 * @param parent
 */
AgentVM::AgentVM(AgentM* model, QObject *parent) : QObject(parent),
    _name(""),
    _hostnames(""),
    _hasNeverAppearedOnNetwork(true),
    _isON(false),
    _isMuted(false),
    _canBeFrozen(false),
    _isFrozen(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (model != NULL)
    {
        // Init the name
        _name = model->name();
        _hasNeverAppearedOnNetwork = model->hasNeverAppearedOnNetwork();

        if (_hasNeverAppearedOnNetwork) {
            qInfo() << "New View Model of Agent" << _name << "which has never yet appeared on the network";
        }
        else {
            qInfo() << "New View Model of Agent" << _name << "with peer id" << model->peerId();
        }

        // Connect to signal "Count Changed" from the list of models
        connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentVM::_onModelsChanged);

        // Add to the list
        _models.append(model);
    }
    else
    {
        qCritical() << "The model of agent is NULL !";
    }
}


/**
 * @brief Destructor
 */
AgentVM::~AgentVM()
{
    qInfo() << "Delete View Model of Agent" << _name;

    // Clear the lists of models
    _previousAgentsList.clear();

    // Free the memory elsewhere
    //_models.deleteAllItems();
    _models.clear();
}


/**
 * @brief Setter for property "Name"
 * @param value
 */
void AgentVM::setname(QString value)
{
    if (_name != value)
    {
        QString previousName = _name;

        _name = value;

        // TODO: update the name of models
        qDebug() << "Rename agent from" << previousName << "to" << _name;

        Q_EMIT nameChanged(value);
    }
}


/**
 * @brief Setter for property "Definition"
 * @param value
 */
/*void AgentVM::setdefinition(DefinitionM *value)
{
    if (_definition != value)
    {
        // Previous value
        if (_definition != NULL) {
            // DIS-connect from signals from the previous definition
            disconnect(_definition, 0, this, 0);
        }

        _definition = value;

        // New value
        if (_definition != NULL) {
            // Connect to signal "Command Asked" from the new definition
            connect(_definition, &DefinitionM::commandAsked, this, &AgentVM::onCommandAskedForOutput);
        }

        Q_EMIT definitionChanged(value);
    }
}*/


/**
 * @brief Change the state of our agent
 */
void AgentVM::changeState()
{
    // is ON --> Kill all agents
    if (_isON) {
        Q_EMIT commandAsked("DIE", _peerIdsList);
    }
    // is OFF --> Execute all agents
    else {
        foreach (AgentM* model, _models.toList())
        {
            // Check if the model has a hostname
            if ((model != NULL) && !model->hostname().isEmpty()) {
                Q_EMIT commandAskedToLauncher("RUN", model->hostname(), model->executionPath());
            }
        }
    }
}


/**
 * @brief Mute / UN-mute all outputs of our agent
 */
void AgentVM::changeMuteAllOutputs()
{
    if (_isMuted) {
        Q_EMIT commandAsked("UNMUTE_ALL", _peerIdsList);
    }
    else {
        Q_EMIT commandAsked("MUTE_ALL", _peerIdsList);
    }
}


/**
 * @brief Freeze / UN-freeze our agent
 */
void AgentVM::changeFreeze()
{
    if (_isFrozen) {
        Q_EMIT commandAsked("UNFREEZE", _peerIdsList);
    }
    else {
        Q_EMIT commandAsked("FREEZE", _peerIdsList);
    }
}


/**
 * @brief Slot when a command from an output must be sent on the network
 * @param command
 * @param outputName
 */
void AgentVM::onCommandAskedForOutput(QString command, QString outputName)
{
    Q_EMIT commandAskedForOutput(command, outputName, _peerIdsList);
}


/**
 * @brief Slot when the list of models changed
 */
void AgentVM::_onModelsChanged()
{
    QList<AgentM*> newAgentsList = _models.toList();

    // Model of agent added
    if (_previousAgentsList.count() < newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> ADD --> " << newAgentsList.count();

        for (AgentM* model : newAgentsList) {
            if ((model != NULL) && !_previousAgentsList.contains(model))
            {   
                // Connect to signals from a model
                connect(model, &AgentM::isONChanged, this, &AgentVM::_onIsONofModelChanged);
                connect(model, &AgentM::isMutedChanged, this, &AgentVM::_onIsMutedOfModelChanged);
                connect(model, &AgentM::isFrozenChanged, this, &AgentVM::_onIsFrozenOfModelChanged);
            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList) {
            if ((model != NULL) && !newAgentsList.contains(model))
            {
                // DIS-connect from signals from a model
                disconnect(model, 0, this, 0);
            }
        }
    }

    _previousAgentsList = newAgentsList;

    // Update with the list of models
    _updateWithModels();
}


/**
 * @brief Slot when the flag "is ON" of a model changed
 * @param isON
 */
void AgentVM::_onIsONofModelChanged(bool isON)
{
    Q_UNUSED(isON)

    // Update the flag "is ON" in function of flags of models
    _updateIsON();
}


/**
 * @brief Slot when the flag "is Muted" of a model changed
 * @param isMuted
 */
void AgentVM::_onIsMutedOfModelChanged(bool isMuted)
{
    Q_UNUSED(isMuted)

    // Update the flag "is Muted" in function of models
    _updateIsMuted();
}


/**
 * @brief Slot when the flag "is Frozen" of a model changed
 * @param isMuted
 */
void AgentVM::_onIsFrozenOfModelChanged(bool isFrozen)
{
    Q_UNUSED(isFrozen)

    // Update the flag "is Frozen" in function of models
    _updateIsFrozen();
}


/**
 * @brief Update with the list of models
 */
void AgentVM::_updateWithModels()
{
    _peerIdsList.clear();
    QStringList hostnamesList;
    bool globalCanBeFrozen = true;

    foreach (AgentM* model, _models.toList()) {
        if (model != NULL)
        {
            if (!model->peerId().isEmpty()) {
                _peerIdsList.append(model->peerId());
            }

            if (!hostnamesList.contains(model->hostname())) {
                hostnamesList.append(model->hostname());
            }

            if (!model->canBeFrozen()) {
                globalCanBeFrozen = false;
            }
        }
    }

    QString globalHostnames = "";
    for (int i = 0; i < hostnamesList.count(); i++) {
        if (i == 0) {
            globalHostnames = hostnamesList.at(i);
        }
        else {
            globalHostnames = QString("%1, %2").arg(globalHostnames, hostnamesList.at(i));
        }
    }

    sethostnames(globalHostnames);
    setcanBeFrozen(globalCanBeFrozen);

    // Update flags in function of models
    _updateIsON();
    _updateIsMuted();
    _updateIsFrozen();
}


/**
 * @brief Update the flag "is ON" in function of flags of models
 */
void AgentVM::_updateIsON()
{
    bool globalIsON = true;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && !model->isON()) {
            globalIsON = false;
            break;
        }
    }

    setisON(globalIsON);
}


/**
 * @brief Update the flag "is Muted" in function of flags of models
 */
void AgentVM::_updateIsMuted()
{
    bool globalIsMuted = true;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && !model->isMuted()) {
            globalIsMuted = false;
            break;
        }
    }

    setisMuted(globalIsMuted);
}


/**
 * @brief Update the flag "is Frozen" in function of flags of models
 */
void AgentVM::_updateIsFrozen()
{
    bool globalIsFrozen = true;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && !model->isFrozen()) {
            globalIsFrozen = false;
            break;
        }
    }

    setisFrozen(globalIsFrozen);
}
