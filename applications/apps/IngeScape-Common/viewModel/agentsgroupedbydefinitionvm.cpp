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
#include <QFileDialog>
#include <model/enums.h>
#include <controller/ingescapemodelmanager.h>
#include <controller/ingescapenetworkcontroller.h>


/**
 * @brief Constructor
 * @param agentName
 * @param definition
 * @param parent
 */
AgentsGroupedByDefinitionVM::AgentsGroupedByDefinitionVM(QString agentName,
                                                         DefinitionM* definition,
                                                         QObject *parent) : QObject(parent),
    _name(agentName),
    _definition(definition),
    _peerIdsList(QStringList()),
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
    _isEnabledViewLogStream(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_definition != nullptr) {
        qInfo() << "New View Model of Agents grouped by definition" << _definition->name() << "(agent name" << _name << ")";

        connect(_definition, &DefinitionM::commandAskedForOutput, this, &AgentsGroupedByDefinitionVM::_onCommandAskedToAgentAboutOutput);
        connect(_definition, &DefinitionM::openValuesHistoryOfAgent, this, &AgentsGroupedByDefinitionVM::_onOpenValuesHistoryOfAgent);
    }
    else {
        qInfo() << "New View Model of Agents grouped by definition 'NULL'" << "(agent name" << _name << ")";
    }

    // Connect to signal "Count Changed" from the list of models
    connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentsGroupedByDefinitionVM::_onModelsChanged);
}


/**
 * @brief Destructor
 */
AgentsGroupedByDefinitionVM::~AgentsGroupedByDefinitionVM()
{
    if (_definition != nullptr)
    {
        qInfo() << "Delete View Model of Agents grouped by definition" << _definition->name() << "(and name" << _name << ")";

        disconnect(_definition, nullptr, this, nullptr);

        // Reset and delete the definition
        DefinitionM* temp = _definition;
        setdefinition(nullptr);
        delete temp;
    }
    else {
        qInfo() << "Delete View Model of Agents grouped by definition 'NULL'" << "(and name" << _name << ")";
    }


    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentsGroupedByDefinitionVM::_onModelsChanged);

    // DIS-connect from signals of each model
    for (AgentM* model : _models)
    {
        if (model != nullptr) {
            disconnect(model, nullptr, this, nullptr);
        }
    }

    // Clear the previous list of models
    _previousAgentsList.clear();

    // Models are deleted elsewhere (the signal "agentModelHasToBeDeleted" is emitted for each of them
    // in the method "AgentsGroupedByNameVM::deleteAgentsGroupedByDefinition" just before our destructor)
    //_models.deleteAllItems();
    _models.clear();
}


/**
 * @brief Setter for property "is Muted"
 * @param value
 */
void AgentsGroupedByDefinitionVM::setisMuted(bool value)
{
    if (_isMuted != value)
    {
        _isMuted = value;

        // Update our definition which is not directly linked to a real model of agent
        if (_definition != nullptr) {
            _definition->setisMutedOfAllOutputs(value);
        }

        Q_EMIT isMutedChanged(value);
    }
}


/**
 * @brief Change the state of our agent
 */
void AgentsGroupedByDefinitionVM::changeState()
{
    // is ON --> Kill all agents
    if (_isON)
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_StopAgent);
    }
    // is OFF --> Execute all agents
    else
    {
        for (AgentM* model : _models.toList())
        {
            // Check if the model has a hostname
            if ((model != nullptr) && !model->hostname().isEmpty())
            {
                // Get the peer id of the Launcher on this host
                QString peerIdOfLauncher = IngeScapeModelManager::instance()->getPeerIdOfLauncherOnHost(model->hostname());
                if (!peerIdOfLauncher.isEmpty())
                {
                    QStringList message = {
                        command_StartAgent,
                        model->commandLine()
                    };

                    // Send the message "Start Agent" to the IngeScape Launcher
                    IngeScapeNetworkController::instance()->sendMessageToAgent(peerIdOfLauncher, message);
                }
            }
        }
    }
}


/**
 * @brief Mute / UN-mute all outputs of our agent
 */
void AgentsGroupedByDefinitionVM::changeMuteAllOutputs()
{
    if (_isMuted)
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_UnmuteAgent);
    }
    else
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_MuteAgent);
    }
}


/**
 * @brief Freeze / UN-freeze our agent
 */
void AgentsGroupedByDefinitionVM::changeFreeze()
{
    if (_isFrozen)
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_UnfreezeAgent);
    }
    else
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_FreezeAgent);
    }
}


/**
 * @brief Load a (new) definition
 */
void AgentsGroupedByDefinitionVM::loadDefinition()
{
    // "File Dialog" to get the file path to open
    QString definitionFilePath = QFileDialog::getOpenFileName(nullptr,
                                                              tr("Open definition"),
                                                              "",
                                                              tr("Definition (*.igsdefinition *.json)")
                                                              );

    if (!definitionFilePath.isEmpty()) {
        Q_EMIT loadAgentDefinitionFromPath(_peerIdsList, definitionFilePath);
    }

}


/**
 * @brief Load a (new) mapping
 */
void AgentsGroupedByDefinitionVM::loadMapping()
{
    // "File Dialog" to get the file path to open
    QString mappingFilePath = QFileDialog::getOpenFileName(nullptr,
                                                           tr("Open mapping"),
                                                           "",
                                                           tr("Mapping (*.json)")
                                                           );

    if (!mappingFilePath.isEmpty()) {
        Q_EMIT loadAgentMappingFromPath(_peerIdsList, mappingFilePath);
    }
}


/**
 * @brief Download the current definition
 */
void AgentsGroupedByDefinitionVM::downloadDefinition()
{
    QString defaultDefinitionName = QString("Definition_of_%1.igsdefinition").arg(_name);
    if (_definition != nullptr) {
        defaultDefinitionName = QString("%1.igsdefinition").arg(_definition->name());
    }

    // "File Dialog" to get the file path to save
    QString definitionFilePath = QFileDialog::getSaveFileName(nullptr,
                                                              tr("Save definition"),
                                                              defaultDefinitionName,
                                                              tr("Definition (*.igsdefinition)")
                                                              );

    if (!definitionFilePath.isEmpty() && (_definition != nullptr)) {
        Q_EMIT downloadAgentDefinitionToPath(_definition, definitionFilePath);
    }
}


/**
 * @brief Download the current mapping
 */
void AgentsGroupedByDefinitionVM::downloadMapping()
{
    QString defaultMappingName = QString("Mapping_of_%1.json").arg(_name);
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if ((model != nullptr) && (model->mapping() != nullptr)) {
            defaultMappingName = QString("%1.json").arg(model->mapping()->name());
        }
    }

    // "File Dialog" to get the file path to save
    QString mappingFilePath = QFileDialog::getSaveFileName(nullptr,
                                                           tr("Save mapping"),
                                                           defaultMappingName,
                                                           tr("Mapping (*.json)")
                                                           );

    if (!mappingFilePath.isEmpty() && !_models.isEmpty())
    {
        // Get the mapping of an active agent
        AgentMappingM* agentMapping = nullptr;
        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && model->isON() && (model->mapping() != nullptr))
            {
                agentMapping = model->mapping();
                break;
            }
        }
        if (agentMapping != nullptr) {
            Q_EMIT downloadAgentMappingToPath(agentMapping, mappingFilePath);
        }
    }
}


/**
 * @brief Change the flag "(has) Log in Stream"
 */
void AgentsGroupedByDefinitionVM::changeLogInStream()
{
    if (_hasLogInStream)
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_DisableLogStream);
    }
    else
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_EnableLogStream);
    }
}


/**
 * @brief Change the flag "(has) Log in File"
 */
void AgentsGroupedByDefinitionVM::changeLogInFile()
{
    if (_hasLogInFile)
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_DisableLogFile);
    }
    else
    {
        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_EnableLogFile);
    }
}


/**
 * @brief Save the "files paths" of our agent
 * @param definitionFilePath
 * @param mappingFilePath
 * @param logFilePath
 */
void AgentsGroupedByDefinitionVM::saveFilesPaths(QString definitionFilePath, QString mappingFilePath, QString logFilePath)
{
    // Path for definition file changed
    if (definitionFilePath != _definitionFilePath)
    {
        qDebug() << "Path of definition changed to" << definitionFilePath;

        QString message = QString("%1 %2").arg(command_SetDefinitionPath, definitionFilePath);

        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, message);
    }

    // Path for mapping file changed
    if (mappingFilePath != _mappingFilePath)
    {
        qDebug() << "Path of mapping changed to" << mappingFilePath;

        QString message = QString("%1 %2").arg(command_SetMappingPath, mappingFilePath);

        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, message);
    }

    // Path for log file changed
    if (logFilePath != _logFilePath)
    {
        qDebug() << "Path of log changed to" << logFilePath;

        QString message = QString("%1 %2").arg(command_SetLogPath, logFilePath);

        IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, message);
    }
}


/**
 * @brief Save the definition of our agent to its path
 */
void AgentsGroupedByDefinitionVM::saveDefinitionToPath()
{
    IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_SaveDefinitionToPath);
}


/**
 * @brief Save the mapping of our agent to its path
 */
void AgentsGroupedByDefinitionVM::saveMappingToPath()
{
    IngeScapeNetworkController::instance()->sendMessageToAgents(_peerIdsList, command_SaveMappingToPath);
}


/**
 * @brief Open the "Log Stream" of our agent
 */
void AgentsGroupedByDefinitionVM::openLogStream()
{
    //qDebug() << "Open the 'Log Stream' of" << _name;

    Q_EMIT openLogStreamOfAgents(_models.toList());
}


/**
 * @brief Get the list of agent models on a host
 * @param hostname
 * @return
 */
QList<AgentM*> AgentsGroupedByDefinitionVM::getModelsOnHost(QString hostname)
{
    return _hashFromHostnameToModels.value(hostname, QList<AgentM*>());
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
            if ((model != nullptr) && !_previousAgentsList.contains(model))
            {
                //qDebug() << "New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals of the model
                connect(model, &AgentM::hostnameChanged, this, &AgentsGroupedByDefinitionVM::_onHostnameOfModelChanged);
                connect(model, &AgentM::isONChanged, this, &AgentsGroupedByDefinitionVM::_onIsONofModelChanged);
                connect(model, &AgentM::canBeRestartedChanged, this, &AgentsGroupedByDefinitionVM::_onCanBeRestartedOfModelChanged);
                connect(model, &AgentM::isMutedChanged, this, &AgentsGroupedByDefinitionVM::_onIsMutedOfModelChanged);
                connect(model, &AgentM::canBeFrozenChanged, this, &AgentsGroupedByDefinitionVM::_onCanBeFrozenOfModelChanged);
                connect(model, &AgentM::isFrozenChanged, this, &AgentsGroupedByDefinitionVM::_onIsFrozenOfModelChanged);
                connect(model, &AgentM::loggerPortChanged, this, &AgentsGroupedByDefinitionVM::_onLoggerPortOfModelChanged);
                connect(model, &AgentM::hasLogInStreamChanged, this, &AgentsGroupedByDefinitionVM::_onHasLogInStreamOfModelChanged);
                connect(model, &AgentM::hasLogInFileChanged, this, &AgentsGroupedByDefinitionVM::_onHasLogInFileOfModelChanged);
                connect(model, &AgentM::stateChanged, this, &AgentsGroupedByDefinitionVM::_onStateOfModelChanged);
                connect(model, &AgentM::logFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onLogFilePathOfModelChanged);
                connect(model, &AgentM::definitionFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onDefinitionFilePathOfModelChanged);
                connect(model, &AgentM::mappingFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onMappingFilePathOfModelChanged);
                connect(model, &AgentM::isMutedOutputChanged, this, &AgentsGroupedByDefinitionVM::_onIsMutedOutputOfModelChanged);
            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList)
        {
            if ((model != nullptr) && !newAgentsList.contains(model))
            {
                //qDebug() << "Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals of the model
                disconnect(model, nullptr, this, nullptr);
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
    // Note: hostname is never empty (default value is HOSTNAME_NOT_DEFINED)

    _peerIdsList.clear();
    _hashFromHostnameToModels.clear();

    QString globalHostnames = "";

    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        AgentM* model = _models.at(0);
        if (model != nullptr)
        {
            if (!model->peerId().isEmpty()) {
                _peerIdsList = QStringList(model->peerId());
            }

            QList<AgentM*> modelsOnHost;
            modelsOnHost.append(model);
            _hashFromHostnameToModels.insert(hostname, modelsOnHost);

            if (hostname != HOSTNAME_NOT_DEFINED) {
                globalHostnames = hostname;
            }
        }
    }
    // Several models
    else
    {
        QStringList temporaryListOfHostnames;

        // Update with all models of agents
        for (AgentM* model : _models.toList())
        {
            if (model != nullptr)
            {
                if (!model->peerId().isEmpty()) {
                    _peerIdsList.append(model->peerId());
                }

                QList<AgentM*> modelsOnHost = getModelsOnHost(model->hostname());
                modelsOnHost.append(model);
                _hashFromHostnameToModels.insert(model->hostname(), modelsOnHost);

                if (!temporaryListOfHostnames.contains(model->hostname())) {
                    temporaryListOfHostnames.append(model->hostname());
                }
            }
        }

        // Hostname(s) on the network of our agent(s)
        for (QString iterator : temporaryListOfHostnames)
        {
            if (iterator != HOSTNAME_NOT_DEFINED)
            {
                if (globalHostnames.isEmpty()) {
                    globalHostnames = iterator;
                }
                else {
                    globalHostnames = QString("%1, %2").arg(globalHostnames, iterator);
                }
            }
        }
    }

    sethostnames(globalHostnames);
}


/**
 * @brief Slot called when the flag "is ON" of a model changed
 * @param isON
 */
void AgentsGroupedByDefinitionVM::_onIsONofModelChanged(bool isON)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setisON(isON);
        setclonesNumber(0);
    }
    // Several models
    else
    {
        bool globalIsON = false;
        int clonesNumber = 0;

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && model->isON())
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
}


/**
 * @brief Slot called when the flag "can Be Restarted" of a model changed
 * @param canBeRestarted
 */
void AgentsGroupedByDefinitionVM::_onCanBeRestartedOfModelChanged(bool canBeRestarted)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setcanBeRestarted(canBeRestarted);
    }
    // Several models
    else
    {
        bool globalCanBeRestarted = false;

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && model->canBeRestarted())
            {
                globalCanBeRestarted = true;
                break;
            }
        }
        setcanBeRestarted(globalCanBeRestarted);
    }
}


/**
 * @brief Slot called when the flag "is Muted" of a model changed
 * @param isMuted
 */
void AgentsGroupedByDefinitionVM::_onIsMutedOfModelChanged(bool isMuted)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setisMuted(isMuted);
    }
    // Several models
    else
    {
        bool globalIsMuted = false;

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && model->isMuted())
            {
                globalIsMuted = true;
                break;
            }
        }
        setisMuted(globalIsMuted);
    }
}


/**
 * @brief Slot called when the flag "can be Frozen" of a model changed
 * @param canBeFrozen
 */
void AgentsGroupedByDefinitionVM::_onCanBeFrozenOfModelChanged(bool canBeFrozen)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setcanBeFrozen(canBeFrozen);
    }
    // Several models
    else
    {
        bool globalCanBeFrozen = false;

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && model->canBeFrozen())
            {
                globalCanBeFrozen = true;
                break;
            }
        }
        setcanBeFrozen(globalCanBeFrozen);
    }
}


/**
 * @brief Slot called when the flag "is Frozen" of a model changed
 * @param isMuted
 */
void AgentsGroupedByDefinitionVM::_onIsFrozenOfModelChanged(bool isFrozen)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setisFrozen(isFrozen);
    }
    // Several models
    else
    {
        bool globalIsFrozen = false;

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && model->isFrozen())
            {
                globalIsFrozen = true;
                break;
            }
        }
        setisFrozen(globalIsFrozen);
    }
}


/**
 * @brief Slot called when the "Logger Port" of a model changed
 * @param loggerPort
 */
void AgentsGroupedByDefinitionVM::_onLoggerPortOfModelChanged(QString loggerPort)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        // Check that its logger port is defined
        setisEnabledViewLogStream(!loggerPort.isEmpty());
    }
    // Several models
    else
    {
        bool globalIsEnabledViewLogStream = false;

        for (AgentM* model : _models.toList())
        {
            // Check that its logger port is defined
            if ((model != nullptr) && !model->loggerPort().isEmpty())
            {
                globalIsEnabledViewLogStream = true;
                break;
            }
        }
        setisEnabledViewLogStream(globalIsEnabledViewLogStream);
    }
}


/**
 * @brief Slot called when the flag "has Log in Stream" of a model changed
 * @param hasLogInStream
 */
void AgentsGroupedByDefinitionVM::_onHasLogInStreamOfModelChanged(bool hasLogInStream)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        sethasLogInStream(hasLogInStream);
    }
    // Several models
    else
    {
        bool globalHasLogInStream = false;

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && model->hasLogInStream())
            {
                globalHasLogInStream = true;
                break;
            }
        }
        sethasLogInStream(globalHasLogInStream);
    }
}


/**
 * @brief Slot called when the flag "has Log in File" of a model changed
 * @param hasLogInFile
 */
void AgentsGroupedByDefinitionVM::_onHasLogInFileOfModelChanged(bool hasLogInFile)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        sethasLogInFile(hasLogInFile);
    }
    // Several models
    else
    {
        bool globalHasLogInFile = false;

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && model->hasLogInFile())
            {
                globalHasLogInFile = true;
                break;
            }
        }
        sethasLogInFile(globalHasLogInFile);
    }
}


/**
 * @brief Slot called when the state of a model changed
 * @param state
 */
void AgentsGroupedByDefinitionVM::_onStateOfModelChanged(QString state)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setstate(state);
    }
    // Several models
    else
    {
        QString globalStates = "";

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && !model->state().isEmpty())
            {
                globalStates = model->state();
                break;
            }
        }
        setstate(globalStates);
    }
}


/**
 * @brief Slot called when the path of "Log File" of a model changed
 * @param logFilePath
 */
void AgentsGroupedByDefinitionVM::_onLogFilePathOfModelChanged(QString logFilePath)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setlogFilePath(logFilePath);
    }
    // Several models
    else
    {
        QString globalLogFilePath = "";

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && !model->logFilePath().isEmpty())
            {
                globalLogFilePath = model->logFilePath();
                break;
            }
        }
        setlogFilePath(globalLogFilePath);
    }
}


/**
 * @brief Slot called when the path of "Definition File" of a model changed
 * @param definitionFilePath
 */
void AgentsGroupedByDefinitionVM::_onDefinitionFilePathOfModelChanged(QString definitionFilePath)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setdefinitionFilePath(definitionFilePath);
    }
    // Several models
    else
    {
        QString globalDefinitionFilePath = "";

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && !model->definitionFilePath().isEmpty())
            {
                globalDefinitionFilePath = model->definitionFilePath();
                break;
            }
        }
        setdefinitionFilePath(globalDefinitionFilePath);
    }
}


/**
 * @brief Slot called when the path of "Mapping File" of a model changed
 * @param mappingFilePath
 */
void AgentsGroupedByDefinitionVM::_onMappingFilePathOfModelChanged(QString mappingFilePath)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setmappingFilePath(mappingFilePath);
    }
    // Several models
    else
    {
        QString globalMappingFilePath = "";

        for (AgentM* model : _models.toList())
        {
            if ((model != nullptr) && !model->mappingFilePath().isEmpty())
            {
                globalMappingFilePath = model->mappingFilePath();
                break;
            }
        }
        setmappingFilePath(globalMappingFilePath);
    }
}


/**
 * @brief Slot called when the flag "is Muted Output" of an output (of a model) changed
 * @param isMutedOutput
 * @param outputName
 */
void AgentsGroupedByDefinitionVM::_onIsMutedOutputOfModelChanged(bool isMutedOutput, QString outputName)
{
    if (_definition != nullptr)
    {
        // Most of the time, there is only one model
        if (_models.count() == 1)
        {
            _definition->setisMutedOutput(isMutedOutput, outputName);
        }
        // Several models
        else
        {
            bool globalIsMutedOutput = false;

            for (AgentM* model : _models.toList())
            {
                if ((model != nullptr) && (model->definition() != nullptr) && model->definition()->getIsMutedOutput(outputName))
                {
                    globalIsMutedOutput = true;
                    break;
                }
            }
            _definition->setisMutedOutput(globalIsMutedOutput, outputName);
        }
    }
}


/**
 * @brief Slot called when a command must be sent on the network to agent(s) about one of its output
 * @param command
 * @param outputName
 */
void AgentsGroupedByDefinitionVM::_onCommandAskedToAgentAboutOutput(QString command, QString outputName)
{
    Q_EMIT commandAskedToAgentAboutOutput(_peerIdsList, command, outputName);
}


/**
 * @brief Slot called when we have to open the values history of our agent
 */
void AgentsGroupedByDefinitionVM::_onOpenValuesHistoryOfAgent()
{
    Q_EMIT openValuesHistoryOfAgent(_name);

    qDebug() << "Open Values History of Agent" << _name;
}


/**
 * @brief Update with all models of agents
 */
void AgentsGroupedByDefinitionVM::_updateWithAllModels()
{
    /*if (_definition != nullptr) {
        qDebug() << "Grouped by definition" << _definition->name() << ": Update with all (" << _models.count() << ") models of" << _name;
    }
    else {
        qDebug() << "Grouped by definition 'NULL': Update with all (" << _models.count() << ") models of" << _name;
    }*/

    // Note: hostname is never empty (default value is HOSTNAME_NOT_DEFINED)
    _peerIdsList.clear();
    _hashFromHostnameToModels.clear();

    QString globalHostnames = "";
    bool globalIsON = false;
    int clonesNumber = 0;
    bool globalCanBeRestarted = false;
    bool globalIsMuted = false;
    bool globalCanBeFrozen = false;
    bool globalIsFrozen = false;
    bool globalIsEnabledViewLogStream = false;
    bool globalHasLogInStream = false;
    bool globalHasLogInFile = false;
    QString globalState = "";
    QString globalLogFilePath = "";
    QString globalDefinitionFilePath = "";
    QString globalMappingFilePath = "";

    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        AgentM* model = _models.at(0);
        if (model != nullptr)
        {
            if (!model->peerId().isEmpty()) {
                _peerIdsList = QStringList(model->peerId());
            }

            QList<AgentM*> modelsOnHost;
            modelsOnHost.append(model);
            _hashFromHostnameToModels.insert(model->hostname(), modelsOnHost);

            if (model->hostname() != HOSTNAME_NOT_DEFINED) {
                globalHostnames = model->hostname();
            }

            globalIsON = model->isON();
            //clonesNumber = 1;
            globalCanBeRestarted = model->canBeRestarted();
            globalIsMuted = model->isMuted();
            globalCanBeFrozen = model->canBeFrozen();
            globalIsFrozen = model->isFrozen();
            globalIsEnabledViewLogStream = !model->loggerPort().isEmpty();
            globalHasLogInStream = model->hasLogInStream();
            globalHasLogInFile = model->hasLogInFile();
            globalState = model->state();
            globalLogFilePath = model->logFilePath();
            globalDefinitionFilePath = model->definitionFilePath();
            globalMappingFilePath = model->mappingFilePath();
        }
    }
    // Several models
    else
    {
        QStringList temporaryListOfHostnames;

        // Update with all models of agents
        for (AgentM* model : _models.toList())
        {
            if (model != nullptr)
            {
                if (!model->peerId().isEmpty()) {
                    _peerIdsList.append(model->peerId());
                }

                QList<AgentM*> modelsOnHost = getModelsOnHost(model->hostname());
                modelsOnHost.append(model);
                _hashFromHostnameToModels.insert(model->hostname(), modelsOnHost);

                if (!temporaryListOfHostnames.contains(model->hostname())) {
                    temporaryListOfHostnames.append(model->hostname());
                }

                if (model->isON())
                {
                    //if (!globalIsON) {
                    globalIsON = true;
                    //}

                    clonesNumber++;
                }

                if (!globalCanBeRestarted && model->canBeRestarted()) {
                    globalCanBeRestarted = true;
                }

                if (!globalIsMuted && model->isMuted()) {
                    globalIsMuted = true;
                }

                if (!globalCanBeFrozen && model->canBeFrozen()) {
                    globalCanBeFrozen = true;
                }

                if (!globalIsFrozen && model->isFrozen()) {
                    globalIsFrozen = true;
                }

                if (!globalIsEnabledViewLogStream && !model->loggerPort().isEmpty()) {
                    globalIsEnabledViewLogStream = true;
                }

                if (!globalHasLogInStream && model->hasLogInStream()) {
                    globalHasLogInStream = true;
                }

                if (!globalHasLogInFile && model->hasLogInFile()) {
                    globalHasLogInFile = true;
                }

                if (globalState.isEmpty() && !model->state().isEmpty()) {
                    globalState = model->state();
                }

                if (globalLogFilePath.isEmpty() && !model->logFilePath().isEmpty()) {
                    globalLogFilePath = model->logFilePath();
                }

                if (globalDefinitionFilePath.isEmpty() && !model->definitionFilePath().isEmpty()) {
                    globalDefinitionFilePath = model->definitionFilePath();
                }

                if (globalMappingFilePath.isEmpty() && !model->mappingFilePath().isEmpty()) {
                    globalMappingFilePath = model->mappingFilePath();
                }
            }
        }

        // Hostname(s) on the network of our agent(s)
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
        }
    }


    //
    // Update properties
    //
    sethostnames(globalHostnames);
    setisON(globalIsON);

    // Update the clones number
    if (clonesNumber > 1) {
        setclonesNumber(clonesNumber);
    }
    else {
        setclonesNumber(0);
    }

    setcanBeRestarted(globalCanBeRestarted);
    setisMuted(globalIsMuted);
    setcanBeFrozen(globalCanBeFrozen);
    setisFrozen(globalIsFrozen);
    setisEnabledViewLogStream(globalIsEnabledViewLogStream);
    sethasLogInStream(globalHasLogInStream);
    sethasLogInFile(globalHasLogInFile);
    setstate(globalState);
    setlogFilePath(globalLogFilePath);
    setdefinitionFilePath(globalDefinitionFilePath);
    setmappingFilePath(globalMappingFilePath);
}
