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
    _addresses(""),
    _definition(NULL),
    _hasOnlyDefinition(true),
    _status(AgentStatus::OFF),
    _state(""),
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

        if (model->peerId().isEmpty()) {
            _hasOnlyDefinition = true;
            qInfo() << "New View Model of Agent" << _name << "with Only Definition";
        }
        else {
            _hasOnlyDefinition = false;
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

    if (_definition != NULL)
    {   
        setdefinition(NULL);
    }

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
void AgentVM::setdefinition(DefinitionM *value)
{
    if (_definition != value)
    {
        // Previous value
        if (_definition != NULL) {
            // Delete all previous Inputs / Outputs / Parameters
            //_inputsList.deleteAllItems();
            //_outputsList.deleteAllItems();
            //_parametersList.deleteAllItems();

            // DIS-connect from signal "Command Asked" from the previous definition
            disconnect(_definition, &DefinitionM::commandAsked, this, &AgentVM::onCommandAskedForOutput);
        }

        _definition = value;

        // New value
        if (_definition != NULL)
        {
            //
            // Create the list of VM of inputs
            //
            /*QList<AgentIOPVM*> listOfInputVMs;
            foreach (AgentIOPM* inputM, _definition->inputsList()->toList())
            {
                if (inputM != NULL)
                {
                    AgentIOPVM* inputVM = new AgentIOPVM(inputM, this);
                    listOfInputVMs.append(inputVM);
                }
            }
            _inputsList.append(listOfInputVMs);*/


            //
            // Create the list of VM of outputs
            //
            /*QList<AgentIOPVM*> listOfOutputVMs;
            foreach (AgentIOPM* outputM, _definition->outputsList()->toList())
            {
                if (outputM != NULL)
                {
                    AgentIOPVM* outputVM = new AgentIOPVM(outputM, this);
                    listOfOutputVMs.append(outputVM);
                }
            }
            _outputsList.append(listOfOutputVMs);*/


            //
            // Create the list of VM of parameters
            //
            /*QList<AgentIOPVM*> listOfParameterVMs;
            foreach (AgentIOPM* parameterM, _definition->parametersList()->toList())
            {
                if (parameterM != NULL)
                {
                    AgentIOPVM* parameterVM = new AgentIOPVM(parameterM, this);
                    listOfParameterVMs.append(parameterVM);
                }
            }
            _parametersList.append(listOfParameterVMs);*/

            // Connect to signal "Command Asked" from the new definition
            connect(_definition, &DefinitionM::commandAsked, this, &AgentVM::onCommandAskedForOutput);
        }

        Q_EMIT definitionChanged(value);
    }
}


/**
 * @brief Mute/UN-mute all outputs of our agent
 * @param muteAllOutputs
 */
void AgentVM::updateMuteAllOutputs(bool muteAllOutputs)
{
    if (muteAllOutputs) {
        Q_EMIT commandAsked("MUTE_ALL", _peerIdsList);
    }
    else {
        Q_EMIT commandAsked("UNMUTE_ALL", _peerIdsList);
    }
}


/**
 * @brief Freeze/UN-freeze our agent
 * @param freeze
 */
void AgentVM::updateFreeze(bool freeze)
{
    if (freeze) {
        Q_EMIT commandAsked("FREEZE", _peerIdsList);
    }
    else {
        Q_EMIT commandAsked("UNFREEZE", _peerIdsList);
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
                connect(model, &AgentM::statusChanged, this, &AgentVM::_onStatusOfModelChanged);
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
 * @brief Slot when the "Status" of a model changed
 * @param status
 */
void AgentVM::_onStatusOfModelChanged(AgentStatus::Value status)
{
    Q_UNUSED(status)

    // Update the status in function of status of models
    _updateStatus();
}


/**
 * @brief Slot when the flag "Is Muted" of a model changed
 * @param isMuted
 */
void AgentVM::_onIsMutedOfModelChanged(bool isMuted)
{
    Q_UNUSED(isMuted)

    // Update the flag "Is Muted" in function of models
    _updateIsMuted();
}


/**
 * @brief Slot when the flag "Is Frozen" of a model changed
 * @param isMuted
 */
void AgentVM::_onIsFrozenOfModelChanged(bool isFrozen)
{
    Q_UNUSED(isFrozen)

    // Update the flag "Is Frozen" in function of models
    _updateIsFrozen();
}


/**
 * @brief Update with the list of models
 */
void AgentVM::_updateWithModels()
{
    _peerIdsList.clear();
    QStringList addressesList;
    QString globalAddresses = "";
    bool globalCanBeFrozen = true;

    foreach (AgentM* model, _models.toList()) {
        if (model != NULL)
        {
            if (!model->peerId().isEmpty()) {
                _peerIdsList.append(model->peerId());
            }

            /*if (!addressesList.contains(model->address())) {
                addressesList.append(model->address());
            }*/
            if (!addressesList.contains(model->hostname())) {
                addressesList.append(model->hostname());
            }

            if (!model->canBeFrozen()) {
                globalCanBeFrozen = false;
            }
        }
    }

    for (int i = 0; i < addressesList.count(); i++) {
        if (i == 0) {
            globalAddresses = addressesList.at(i);
        }
        else {
            globalAddresses = QString("%1, %2").arg(globalAddresses, addressesList.at(i));
        }
    }

    setaddresses(globalAddresses);
    setcanBeFrozen(globalCanBeFrozen);

    // Update the status and flags in function of models
    _updateStatus();
    _updateIsMuted();
    _updateIsFrozen();
}


/**
 * @brief Update the status in function of status of models
 */
void AgentVM::_updateStatus()
{
    AgentStatus::Value globalStatus = AgentStatus::OFF;

    foreach (AgentM* model, _models.toList()) {
        if (model != NULL)
        {
            // Use switch case if we need to manage transition status "..._ASKED"

            if (model->status() == AgentStatus::ON) {
                globalStatus = AgentStatus::ON;
            }
        }
    }

    setstatus(globalStatus);
}


/**
 * @brief Update the flag "Is Muted" in function of models
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
 * @brief Update the flag "Is Frozen" in function of models
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
