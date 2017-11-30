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
    _neverAppearedOnNetwork(true),
    _isON(false),
    _isMuted(false),
    _canBeFrozen(false),
    _isFrozen(false),
    _definition(NULL),
    _clonesNumber(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (model != NULL)
    {
        // Init the name
        _name = model->name();
        _neverAppearedOnNetwork = model->neverAppearedOnNetwork();

        if (_neverAppearedOnNetwork) {
            qInfo() << "New View Model of Agent" << _name << "which never yet appeared on the network";
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

    // Reset the definition
    setdefinition(NULL);

    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentVM::_onModelsChanged);

    // Clear the previous list of models
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
void AgentVM::setdefinition(DefinitionM* value)
{
    if (_definition != value)
    {
        DefinitionM* previousValue = _definition;

        // Previous value
        if (previousValue != NULL) {
            // DIS-connect from signals from the previous definition
            disconnect(previousValue, &DefinitionM::commandAskedForOutput, this, &AgentVM::_onCommandAskedToAgentAboutOutput);
            disconnect(previousValue, &DefinitionM::openValuesHistoryOfAgent, this, &AgentVM::_onOpenValuesHistoryOfAgent);
        }

        _definition = value;

        // New value
        if (_definition != NULL) {
            // Connect to signal from the new definition
            connect(_definition, &DefinitionM::commandAskedForOutput, this, &AgentVM::_onCommandAskedToAgentAboutOutput);
            connect(_definition, &DefinitionM::openValuesHistoryOfAgent, this, &AgentVM::_onOpenValuesHistoryOfAgent);
        }

        // Emit simple signal for QML
        Q_EMIT definitionChanged(value);

        // Emit signal with previous value
        Q_EMIT definitionChangedWithPreviousValue(previousValue, _definition);
    }
}


/**
 * @brief Change the state of our agent
 */
void AgentVM::changeState()
{
    // is ON --> Kill all agents
    if (_isON) {
        Q_EMIT commandAskedToAgent(_peerIdsList, "STOP");
    }
    // is OFF --> Execute all agents
    else {
        foreach (AgentM* model, _models.toList())
        {
            // Check if the model has a hostname
            if ((model != NULL) && !model->hostname().isEmpty())
            {
                Q_EMIT commandAskedToLauncher("RUN", model->hostname(), model->commandLine());
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
        Q_EMIT commandAskedToAgent(_peerIdsList, "UNMUTE_ALL");
    }
    else {
        Q_EMIT commandAskedToAgent(_peerIdsList, "MUTE_ALL");
    }
}


/**
 * @brief Freeze / UN-freeze our agent
 */
void AgentVM::changeFreeze()
{
    if (_isFrozen) {
        Q_EMIT commandAskedToAgent(_peerIdsList, "UNFREEZE");
    }
    else {
        Q_EMIT commandAskedToAgent(_peerIdsList, "FREEZE");
    }
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
                //qDebug() << "New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals from a model
                connect(model, &AgentM::isONChanged, this, &AgentVM::_onIsONofModelChanged);
                connect(model, &AgentM::isMutedChanged, this, &AgentVM::_onIsMutedOfModelChanged);
                connect(model, &AgentM::isFrozenChanged, this, &AgentVM::_onIsFrozenOfModelChanged);
                connect(model, &AgentM::definitionChanged, this, &AgentVM::_onDefinitionOfModelChanged);
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
                //qDebug() << "Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals from a model
                disconnect(model, &AgentM::isONChanged, this, &AgentVM::_onIsONofModelChanged);
                disconnect(model, &AgentM::isMutedChanged, this, &AgentVM::_onIsMutedOfModelChanged);
                disconnect(model, &AgentM::isFrozenChanged, this, &AgentVM::_onIsFrozenOfModelChanged);
                disconnect(model, &AgentM::definitionChanged, this, &AgentVM::_onDefinitionOfModelChanged);
            }
        }
    }

    _previousAgentsList = newAgentsList;

    // Update with all models
    _updateWithAllModels();
}


/**
 * @brief Slot when the flag "is ON" of a model changed
 * @param isON
 */
void AgentVM::_onIsONofModelChanged(bool isON)
{
    Q_UNUSED(isON)

    // Update the flag "is ON" in function of flags of all models
    _updateIsON();
}


/**
 * @brief Slot when the flag "is Muted" of a model changed
 * @param isMuted
 */
void AgentVM::_onIsMutedOfModelChanged(bool isMuted)
{
    Q_UNUSED(isMuted)

    // Update the flag "is Muted" in function of all models
    _updateIsMuted();
}


/**
 * @brief Slot when the flag "is Frozen" of a model changed
 * @param isMuted
 */
void AgentVM::_onIsFrozenOfModelChanged(bool isFrozen)
{
    Q_UNUSED(isFrozen)

    // Update the flag "is Frozen" in function of all models
    _updateIsFrozen();
}


/**
 * @brief Slot when the definition of a model changed
 * @param definition
 */
void AgentVM::_onDefinitionOfModelChanged(DefinitionM* definition)
{
    Q_UNUSED(definition)

    // Update with the definition of first model
    _updateWithDefinitionOfFirstModel();
}


/**
 * @brief Slot when a command must be sent on the network to an agent about one of its output
 * @param command
 * @param outputName
 */
void AgentVM::_onCommandAskedToAgentAboutOutput(QString command, QString outputName)
{
    Q_EMIT commandAskedToAgentAboutOutput(_peerIdsList, command, outputName);
}


/**
 * @brief Slot when we have to open the values history of our agent
 */
void AgentVM::_onOpenValuesHistoryOfAgent()
{
    Q_EMIT openValuesHistoryOfAgent(_name);

    qDebug() << "Open Values History of Agent" << _name;
}


/**
 * @brief Update with all models of agents
 */
void AgentVM::_updateWithAllModels()
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

    // Update with the definition of first model
    _updateWithDefinitionOfFirstModel();
}


/**
 * @brief Update the flag "is ON" in function of flags of models
 */
void AgentVM::_updateIsON()
{
    bool globalIsON = false;
    int clonesNumber = 0;

    foreach (AgentM* model, _models.toList())
    {
        if ((model != NULL) && model->isON())
        {
            globalIsON = true;
            //break;
            clonesNumber++;
        }
    }

    setisON(globalIsON);

    if (clonesNumber > 1) {
        setclonesNumber(clonesNumber);
    }
    else {
        setclonesNumber(0);
    }
}


/**
 * @brief Update the flag "is Muted" in function of flags of models
 */
void AgentVM::_updateIsMuted()
{
    bool globalIsMuted = false;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && model->isMuted()) {
            globalIsMuted = true;
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
    bool globalIsFrozen = false;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && model->isFrozen()) {
            globalIsFrozen = true;
            break;
        }
    }
    setisFrozen(globalIsFrozen);
}


/**
 * @brief Update with the definition of first model
 */
void AgentVM::_updateWithDefinitionOfFirstModel()
{
    DefinitionM* definition = NULL;
    if (_models.count() > 0) {
         AgentM* model = _models.at(0);
         if (model != NULL) {
             definition = model->definition();
         }
    }
    setdefinition(definition);
}
