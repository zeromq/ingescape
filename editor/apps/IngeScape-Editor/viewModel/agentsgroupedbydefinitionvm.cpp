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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "agentsgroupedbydefinitionvm.h"


/**
 * @brief Constructor
 * @param model
 * @param parent
 */
AgentsGroupedByDefinitionVM::AgentsGroupedByDefinitionVM(AgentM* model,
                                                         DefinitionM* definition,
                                                         QObject *parent) : QObject(parent),
    _name(""),
    _definition(definition),
    _isON(false),
    _hostnames(""),
    _canBeRestarted(false),
    _isMuted(false),
    _canBeFrozen(false),
    _isFrozen(false),
    _clonesNumber(0),
    _state(""),
    _hasLogInStream(false),
    _hasLogInFile(false),
    _logFilePath(""),
    _definitionFilePath(""),
    _mappingFilePath(""),
    _isEnabledViewLogStream(false),
    _peerIdsList(QStringList())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (model != NULL)
    {
        // Init the name
        _name = model->name();

        if (_definition != nullptr) {
            qInfo() << "New View Model of Agents grouped by definition" << _definition->name() << "(and name" << _name << ")";
        }
        else {
            qInfo() << "New View Model of Agents grouped by definition 'NULL'" << "(and name" << _name << ")";
        }

        // Connect to signal "Count Changed" from the list of models
        connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentsGroupedByDefinitionVM::_onModelsChanged);

        // Add to the list
        _models.append(model);
    }
}


/**
 * @brief Destructor
 */
AgentsGroupedByDefinitionVM::~AgentsGroupedByDefinitionVM()
{
    if (_definition != nullptr) {
        qInfo() << "Delete View Model of Agents grouped by definition" << _definition->name() << "(and name" << _name << ")";

        // Reset and delete the definition
        DefinitionM* temp = _definition;
        setdefinition(NULL);
        delete temp;
    }
    else {
        qInfo() << "Delete View Model of Agents grouped by definition 'NULL'" << "(and name" << _name << ")";
    }

    // Connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentsGroupedByDefinitionVM::_onModelsChanged);

    // Clear the previous list of models
    _previousAgentsList.clear();

    // Free the memory elsewhere
    //_models.deleteAllItems();
    _models.clear();
}


/**
 * @brief Setter for property "Definition"
 * @param value
 */
void AgentsGroupedByDefinitionVM::setdefinition(DefinitionM* value)
{
    if (_definition != value)
    {
        //DefinitionM* previousValue = _definition;

        // Previous value
        /*if (previousValue != NULL)
        {
            // DIS-connect from signals from the previous definition
            disconnect(previousValue, &DefinitionM::commandAskedForOutput, this, &AgentVM::_onCommandAskedToAgentAboutOutput);
            disconnect(previousValue, &DefinitionM::openValuesHistoryOfAgent, this, &AgentVM::_onOpenValuesHistoryOfAgent);
        }*/

        _definition = value;

        // New value
        /*if (_definition != NULL)
        {
            // Connect to signal from the new definition
            connect(_definition, &DefinitionM::commandAskedForOutput, this, &AgentVM::_onCommandAskedToAgentAboutOutput);
            connect(_definition, &DefinitionM::openValuesHistoryOfAgent, this, &AgentVM::_onOpenValuesHistoryOfAgent);
        }*/

        // Emit default signal for QML
        Q_EMIT definitionChanged(value);

        // Emit the signal "Definition Changed" with previous and new values
        //Q_EMIT definitionChangedWithPreviousAndNewValues(previousValue, value);
    }
}


/**
 * @brief Get the list of agent models on a host
 * @param hostname
 * @return
 */
QList<AgentM*> AgentsGroupedByDefinitionVM::getModelsOnHost(QString hostname)
{
    if (_hashFromHostnameToModels.contains(hostname)) {
        return _hashFromHostnameToModels.value(hostname);
    }
    else {
        return QList<AgentM*>();
    }
}


/**
 * @brief Slot called when the list of models changed
 */
void AgentsGroupedByDefinitionVM::_onModelsChanged()
{
    QList<AgentM*> newAgentsList = _models.toList();

    // Model of agent added
    if (_previousAgentsList.count() < newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> ADD --> " << newAgentsList.count();

        for (AgentM* model : newAgentsList)
        {
            if ((model != NULL) && !_previousAgentsList.contains(model))
            {
                //qDebug() << "New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals of the model
                connect(model, &AgentM::hostnameChanged, this, &AgentsGroupedByDefinitionVM::_onHostnameOfModelChanged);
                connect(model, &AgentM::isONChanged, this, &AgentsGroupedByDefinitionVM::_onIsONofModelChanged);
                /*connect(model, &AgentM::canBeRestartedChanged, this, &AgentsGroupedByDefinitionVM::_onCanBeRestartedOfModelChanged);
                connect(model, &AgentM::isMutedChanged, this, &AgentsGroupedByDefinitionVM::_onIsMutedOfModelChanged);
                connect(model, &AgentM::canBeFrozenChanged, this, &AgentsGroupedByDefinitionVM::_onCanBeFrozenOfModelChanged);
                connect(model, &AgentM::isFrozenChanged, this, &AgentsGroupedByDefinitionVM::_onIsFrozenOfModelChanged);
                connect(model, &AgentM::definitionChanged, this, &AgentsGroupedByDefinitionVM::_onDefinitionOfModelChanged);
                connect(model, &AgentM::stateChanged, this, &AgentsGroupedByDefinitionVM::_onStateOfModelChanged);
                connect(model, &AgentM::loggerPortChanged, this, &AgentsGroupedByDefinitionVM::_onLoggerPortOfModelChanged);

                connect(model, &AgentM::hasLogInStreamChanged, this, &AgentsGroupedByDefinitionVM::_onHasLogInStreamOfModelChanged);
                connect(model, &AgentM::hasLogInFileChanged, this, &AgentsGroupedByDefinitionVM::_onHasLogInFileOfModelChanged);
                connect(model, &AgentM::logFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onLogFilePathOfModelChanged);
                connect(model, &AgentM::definitionFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onDefinitionFilePathOfModelChanged);
                connect(model, &AgentM::mappingFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onMappingFilePathOfModelChanged);*/
            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList)
        {
            if ((model != NULL) && !newAgentsList.contains(model))
            {
                //qDebug() << "Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals of the model
                disconnect(model, 0, this, 0);
            }
        }
    }

    _previousAgentsList = newAgentsList;

    if (_models.isEmpty())
    {
        // There is no more model, our VM is useless
        Q_EMIT noMoreModelAndUseless();
    }
    else {
        // Update with all models
        _updateWithAllModels();
    }
}


/**
 * @brief Slot called when the hostname of a model changed
 * @param hostname
 */
void AgentsGroupedByDefinitionVM::_onHostnameOfModelChanged(QString hostname)
{
    Q_UNUSED(hostname)

    // Update with all models of agents
    // Allows to update "_peerIdsList", "_hashFromHostnameToModels" and "hostnames"
    _updateWithAllModels();
}


/**
 * @brief Slot called when the flag "is ON" of a model changed
 * @param isON
 */
void AgentsGroupedByDefinitionVM::_onIsONofModelChanged(bool isON)
{
    Q_UNUSED(isON)

    // Update the flag "is ON" in function of flags of all models
    _updateIsON();
}


/**
 * @brief Update with all models of agents
 */
void AgentsGroupedByDefinitionVM::_updateWithAllModels()
{
    if (_definition != nullptr) {
        qDebug() << "Grouped by definition" << _definition->name() << ": Update with all (" << _models.count() << ") models of" << _name << "(" << this << ")";
    }
    else {
        qDebug() << "Grouped by definition 'NULL': Update with all (" << _models.count() << ") models of" << _name << "(" << this << ")";
    }

    _peerIdsList.clear();
    QStringList temporaryListOfHostnames;
    _hashFromHostnameToModels.clear();

    for (AgentM* model : _models.toList())
    {
        if (model != NULL)
        {
            if (!model->peerId().isEmpty()) {
                _peerIdsList.append(model->peerId());
            }

            if (!temporaryListOfHostnames.contains(model->hostname())) {
                temporaryListOfHostnames.append(model->hostname());
            }

            QList<AgentM*> modelsOnHost = getModelsOnHost(model->hostname());
            modelsOnHost.append(model);
            _hashFromHostnameToModels.insert(model->hostname(), modelsOnHost);
        }
    }

    QString globalHostnames = "";
    for (QString hostname : temporaryListOfHostnames)
    {
        if (hostname != HOSTNAME_NOT_DEFINED)
        {
            if (globalHostnames.isEmpty()) {
                globalHostnames = hostname;
            }
            else {
                globalHostnames = QString("%1, %2").arg(globalHostnames, hostname);
            }
        }

        // FIXME: Print for debug
        QList<AgentM*> modelsOnHost = getModelsOnHost(hostname);
        qDebug() << hostname << ":" << modelsOnHost.count() << "agent(s)";
    }

    sethostnames(globalHostnames);

    // Update flags in function of models
    _updateIsON();
    /*_updateCanBeRestarted();
    _updateIsMuted();
    _updateCanBeFrozen();
    _updateIsFrozen();
    _updateHasLogInStream();
    _updateIsEnabledViewLogStream();
    _updateHasLogInFile();

    // Update with the first model
    _updateWithDefinitionOfFirstModel();
    _updateWithStateOfFirstModel();
    _updateWithLogFilePathOfFirstModel();
    _updateWithDefinitionFilePathOfFirstModel();
    _updateWithMappingFilePathOfFirstModel();*/
}


/**
 * @brief Update the flag "is ON" in function of flags of models
 */
void AgentsGroupedByDefinitionVM::_updateIsON()
{
    bool globalIsON = false;
    int clonesNumber = 0;

    for (AgentM* model : _models.toList())
    {
        if ((model != NULL) && model->isON())
        {
            globalIsON = true;
            clonesNumber++;
        }
    }

    setisON(globalIsON);

    // Update the clones number
    if (clonesNumber > 1) {
        setclonesNumber(clonesNumber);
    }
    else {
        setclonesNumber(0);
    }
}

