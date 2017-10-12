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
    _isFictitious(true),
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
            _isFictitious = true;
            qInfo() << "New View Model of FICTITIOUS Agent" << _name;
        }
        else {
            _isFictitious = false;
            qInfo() << "New View Model of Agent" << _name << "with peer id" << model->peerId();
        }

        // Connect to signal "Count Changed" from the list of models
        connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentVM::onModelsChanged);

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
        qDebug() << "Rename agent" << _name << "to" << value;

        _name = value;

        // TODO: update the name of models

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
            _inputsList.deleteAllItems();
            _outputsList.deleteAllItems();
            _parametersList.deleteAllItems();
        }

        _definition = value;

        // New value
        if (_definition != NULL)
        {
            //
            // Create the list of VM of inputs
            //
            QList<AgentIOPVM*> listOfInputVMs;
            foreach (AgentIOPM* inputM, _definition->inputsList()->toList())
            {
                if (inputM != NULL)
                {
                    AgentIOPVM* inputVM = new AgentIOPVM(inputM, this);
                    listOfInputVMs.append(inputVM);
                }
            }
            _inputsList.append(listOfInputVMs);


            //
            // Create the list of VM of outputs
            //
            QList<AgentIOPVM*> listOfOutputVMs;
            foreach (AgentIOPM* outputM, _definition->outputsList()->toList())
            {
                if (outputM != NULL)
                {
                    AgentIOPVM* outputVM = new AgentIOPVM(outputM, this);
                    listOfOutputVMs.append(outputVM);
                }
            }
            _outputsList.append(listOfOutputVMs);


            //
            // Create the list of VM of parameters
            //
            QList<AgentIOPVM*> listOfParameterVMs;
            foreach (AgentIOPM* parameterM, _definition->parametersList()->toList())
            {
                if (parameterM != NULL)
                {
                    AgentIOPVM* parameterVM = new AgentIOPVM(parameterM, this);
                    listOfParameterVMs.append(parameterVM);
                }
            }
            _parametersList.append(listOfParameterVMs);
        }

        Q_EMIT definitionChanged(value);
    }
}


/**
 * @brief Mute our agent
 */
void AgentVM::mute()
{
    qDebug() << "Mute" << _name;
}


/**
 * @brief Freeze our agent
 */
void AgentVM::freeze()
{
    qDebug() << "Freeze" << _name;
}


/**
 * @brief Slot when the list of models changed
 */
void AgentVM::onModelsChanged()
{
    QList<AgentM*> newAgentsList = _models.toList();

    // Model of agent added
    if (_previousAgentsList.count() < newAgentsList.count())
    {
        qDebug() << _previousAgentsList.count() << "--> ADD --> " << newAgentsList.count();

        for (AgentM* model : newAgentsList) {
            if ((model != NULL) && !_previousAgentsList.contains(model))
            {
                // Connect to signal "Status Changed" from a model
                connect(model, &AgentM::statusChanged, this, &AgentVM::onModelStatusChanged);
            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList) {
            if ((model != NULL) && !newAgentsList.contains(model))
            {
                // DIS-connect from signal "Status Changed" from a model
                disconnect(model, &AgentM::statusChanged, this, &AgentVM::onModelStatusChanged);
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
void AgentVM::onModelStatusChanged(AgentStatus::Value status)
{
    Q_UNUSED(status)

    // Update the status in function of status of models
    _updateStatus();
}


/**
 * @brief Update with the list of models
 */
void AgentVM::_updateWithModels()
{
    QStringList addressesList;
    QString globalAddresses = "";
    bool globalCanBeFrozen = false;

    foreach (AgentM* model, _models.toList()) {
        if (model != NULL)
        {
            /*if (!addressesList.contains(model->address())) {
                addressesList.append(model->address());
            }*/
            if (!addressesList.contains(model->hostname())) {
                addressesList.append(model->hostname());
            }

            if (model->canBeFrozen()) {
                globalCanBeFrozen = true;
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

    // Update the status in function of status of models
    _updateStatus();
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
