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
    if (_definition != nullptr)
    {
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
            disconnect(previousValue, &DefinitionM::commandAskedForOutput, this, &AgentsGroupedByDefinitionVM::_onCommandAskedToAgentAboutOutput);
            disconnect(previousValue, &DefinitionM::openValuesHistoryOfAgent, this, &AgentsGroupedByDefinitionVM::_onOpenValuesHistoryOfAgent);
        }*/

        _definition = value;

        // New value
        /*if (_definition != NULL)
        {
            // Connect to signal from the new definition
            connect(_definition, &DefinitionM::commandAskedForOutput, this, &AgentsGroupedByDefinitionVM::_onCommandAskedToAgentAboutOutput);
            connect(_definition, &DefinitionM::openValuesHistoryOfAgent, this, &AgentsGroupedByDefinitionVM::_onOpenValuesHistoryOfAgent);
        }*/

        // Emit default signal for QML
        Q_EMIT definitionChanged(value);
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
        Q_EMIT commandAskedToAgent(_peerIdsList, command_StopAgent);
    }
    // is OFF --> Execute all agents
    else
    {
        for (AgentM* model : _models.toList())
        {
            // Check if the model has a hostname
            if ((model != NULL) && !model->hostname().isEmpty())
            {
                Q_EMIT commandAskedToLauncher(command_StartAgent, model->hostname(), model->commandLine());
            }
        }
    }
}


/**
 * @brief Mute / UN-mute all outputs of our agent
 */
void AgentsGroupedByDefinitionVM::changeMuteAllOutputs()
{
    if (_isMuted) {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_UnmuteAgent);
    }
    else {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_MuteAgent);
    }
}


/**
 * @brief Freeze / UN-freeze our agent
 */
void AgentsGroupedByDefinitionVM::changeFreeze()
{
    if (_isFrozen) {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_UnfreezeAgent);
    }
    else {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_FreezeAgent);
    }
}


/**
 * @brief Load a (new) definition
 */
void AgentsGroupedByDefinitionVM::loadDefinition()
{
    // "File Dialog" to get the file path to open
    QString definitionFilePath = QFileDialog::getOpenFileName(NULL,
                                                              "Open definition",
                                                              "",
                                                              "JSON (*.json)");

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
    QString mappingFilePath = QFileDialog::getOpenFileName(NULL,
                                                           "Open mapping",
                                                           "",
                                                           "JSON (*.json)");

    if (!mappingFilePath.isEmpty()) {
        Q_EMIT loadAgentMappingFromPath(_peerIdsList, mappingFilePath);
    }
}


/**
 * @brief Download the current definition
 */
void AgentsGroupedByDefinitionVM::downloadDefinition()
{
    QString defaultDefinitionName = QString("Definition_of_%1.json").arg(_name);
    if (_definition != NULL) {
        defaultDefinitionName = QString("%1.json").arg(_definition->name());
    }

    // "File Dialog" to get the file path to save
    QString definitionFilePath = QFileDialog::getSaveFileName(NULL,
                                                              "Save definition",
                                                              defaultDefinitionName,
                                                              "JSON (*.json)");

    if (!definitionFilePath.isEmpty() && (_definition != NULL)) {
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
        if ((model != NULL) && (model->mapping() != NULL)) {
            defaultMappingName = QString("%1.json").arg(model->mapping()->name());
        }
    }

    // "File Dialog" to get the file path to save
    QString mappingFilePath = QFileDialog::getSaveFileName(NULL,
                                                           "Save mapping",
                                                           defaultMappingName,
                                                           "JSON (*.json)");

    if (!mappingFilePath.isEmpty() && !_models.isEmpty())
    {
        // Get the mapping of an active agent
        AgentMappingM* agentMapping = NULL;
        for (AgentM* model : _models.toList())
        {
            if ((model != NULL) && model->isON() && (model->mapping() != NULL))
            {
                agentMapping = model->mapping();
                break;
            }
        }
        if (agentMapping != NULL) {
            Q_EMIT downloadAgentMappingToPath(agentMapping, mappingFilePath);
        }
    }
}


/**
 * @brief Change the flag "(has) Log in Stream"
 */
void AgentsGroupedByDefinitionVM::changeLogInStream()
{
    if (_hasLogInStream) {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_DisableLogStream);
    }
    else {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_EnableLogStream);
    }
}


/**
 * @brief Change the flag "(has) Log in File"
 */
void AgentsGroupedByDefinitionVM::changeLogInFile()
{
    if (_hasLogInFile) {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_DisableLogFile);
    }
    else {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_EnableLogFile);
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

        QString command = QString("%1 %2").arg(command_SetDefinitionPath, definitionFilePath);

        Q_EMIT commandAskedToAgent(_peerIdsList, command);
    }

    // Path for mapping file changed
    if (mappingFilePath != _mappingFilePath)
    {
        qDebug() << "Path of mapping changed to" << mappingFilePath;

        QString command = QString("%1 %2").arg(command_SetMappingPath, mappingFilePath);

        Q_EMIT commandAskedToAgent(_peerIdsList, command);
    }

    // Path for log file changed
    if (logFilePath != _logFilePath)
    {
        qDebug() << "Path of log changed to" << logFilePath;

        QString command = QString("%1 %2").arg(command_SetLogPath, logFilePath);

        Q_EMIT commandAskedToAgent(_peerIdsList, command);
    }
}


/**
 * @brief Save the definition of our agent to its path
 */
void AgentsGroupedByDefinitionVM::saveDefinitionToPath()
{
    Q_EMIT commandAskedToAgent(_peerIdsList, command_SaveDefinitionToPath);
}


/**
 * @brief Save the mapping of our agent to its path
 */
void AgentsGroupedByDefinitionVM::saveMappingToPath()
{
    Q_EMIT commandAskedToAgent(_peerIdsList, command_SaveMappingToPath);
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
                connect(model, &AgentM::canBeRestartedChanged, this, &AgentsGroupedByDefinitionVM::_onCanBeRestartedOfModelChanged);
                connect(model, &AgentM::isMutedChanged, this, &AgentsGroupedByDefinitionVM::_onIsMutedOfModelChanged);
                connect(model, &AgentM::canBeFrozenChanged, this, &AgentsGroupedByDefinitionVM::_onCanBeFrozenOfModelChanged);
                connect(model, &AgentM::isFrozenChanged, this, &AgentsGroupedByDefinitionVM::_onIsFrozenOfModelChanged);
                connect(model, &AgentM::stateChanged, this, &AgentsGroupedByDefinitionVM::_onStateOfModelChanged);
                connect(model, &AgentM::loggerPortChanged, this, &AgentsGroupedByDefinitionVM::_onLoggerPortOfModelChanged);
                connect(model, &AgentM::hasLogInStreamChanged, this, &AgentsGroupedByDefinitionVM::_onHasLogInStreamOfModelChanged);
                connect(model, &AgentM::hasLogInFileChanged, this, &AgentsGroupedByDefinitionVM::_onHasLogInFileOfModelChanged);
                connect(model, &AgentM::logFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onLogFilePathOfModelChanged);
                connect(model, &AgentM::definitionFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onDefinitionFilePathOfModelChanged);
                connect(model, &AgentM::mappingFilePathChanged, this, &AgentsGroupedByDefinitionVM::_onMappingFilePathOfModelChanged);
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
 * @brief Slot called when the flag "can Be Frozen" of a model changed
 * @param canBeFrozen
 */
void AgentsGroupedByDefinitionVM::_onCanBeFrozenOfModelChanged(bool canBeFrozen)
{
    Q_UNUSED(canBeFrozen)

    // Update the flag "can Be Frozen" in function of flags of all models
    _updateCanBeFrozen();
}


/**
 * @brief Slot called when the flag "is Frozen" of a model changed
 * @param isMuted
 */
void AgentsGroupedByDefinitionVM::_onIsFrozenOfModelChanged(bool isFrozen)
{
    Q_UNUSED(isFrozen)

    // Update the flag "is Frozen" in function of all models
    _updateIsFrozen();
}


/**
 * @brief Slot called when the state of a model changed
 * @param state
 */
void AgentsGroupedByDefinitionVM::_onStateOfModelChanged(QString state)
{
    Q_UNUSED(state)

    // Update with the state of the first model
    _updateWithStateOfFirstModel();
}


/**
 * @brief Slot called when the "Logger Port" of a model changed
 * @param loggerPort
 */
void AgentsGroupedByDefinitionVM::_onLoggerPortOfModelChanged(QString loggerPort)
{
    Q_UNUSED(loggerPort)

    // Update the flag "is Enabled View Log Stram" in function of flags of models
    _updateIsEnabledViewLogStream();
}


/**
 * @brief Slot called when the flag "has Log in Stream" of a model changed
 * @param hasLogInStream
 */
void AgentsGroupedByDefinitionVM::_onHasLogInStreamOfModelChanged(bool hasLogInStream)
{
    Q_UNUSED(hasLogInStream)

    // Update the flag "has Log in Stream" in function of all models
    _updateHasLogInStream();
}


/**
 * @brief Slot called when the flag "has Log in File" of a model changed
 * @param hasLogInFile
 */
void AgentsGroupedByDefinitionVM::_onHasLogInFileOfModelChanged(bool hasLogInFile)
{
    Q_UNUSED(hasLogInFile)

    // Update the flag "has Log in File" in function of all models
    _updateHasLogInFile();
}


/**
 * @brief Slot called when the path of "Log File" of a model changed
 * @param logFilePath
 */
void AgentsGroupedByDefinitionVM::_onLogFilePathOfModelChanged(QString logFilePath)
{
    Q_UNUSED(logFilePath)

    // Update with the log file path of the first model
    _updateWithLogFilePathOfFirstModel();
}


/**
 * @brief Slot called when the path of "Definition File" of a model changed
 * @param definitionFilePath
 */
void AgentsGroupedByDefinitionVM::_onDefinitionFilePathOfModelChanged(QString definitionFilePath)
{
    Q_UNUSED(definitionFilePath)

    // Update with the definition file path of the first model
    _updateWithDefinitionFilePathOfFirstModel();
}


/**
 * @brief Slot called when the path of "Mapping File" of a model changed
 * @param mappingFilePath
 */
void AgentsGroupedByDefinitionVM::_onMappingFilePathOfModelChanged(QString mappingFilePath)
{
    Q_UNUSED(mappingFilePath)

    // Update with the mapping file path of the first model
    _updateWithMappingFilePathOfFirstModel();
}


/**
 * @brief Slot when a command must be sent on the network to an agent about one of its output
 * @param command
 * @param outputName
 */
/*void AgentsGroupedByDefinitionVM::_onCommandAskedToAgentAboutOutput(QString command, QString outputName)
{
    Q_EMIT commandAskedToAgentAboutOutput(_peerIdsList, command, outputName);
}*/


/**
 * @brief Slot when we have to open the values history of our agent
 */
/*void AgentsGroupedByDefinitionVM::_onOpenValuesHistoryOfAgent()
{
    Q_EMIT openValuesHistoryOfAgent(_name);

    qDebug() << "Open Values History of Agent" << _name;
}*/


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
    _hashFromHostnameToModels.clear();

    QString globalHostnames = "";
    bool globalIsON = false;
    int clonesNumber = 0;
    bool globalCanBeRestarted = false;
    bool globalIsMuted = false;

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

                if (model->isON()) {
                    globalIsON = true;
                    clonesNumber++;
                }

                if (model->canBeRestarted()) {
                    globalCanBeRestarted = true;
                }

                if (model->isMuted()) {
                    globalIsMuted = true;
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



    _updateCanBeFrozen();
    _updateIsFrozen();
    _updateHasLogInStream();
    _updateIsEnabledViewLogStream();
    _updateHasLogInFile();

    // Update with the first model
    _updateWithStateOfFirstModel();
    _updateWithLogFilePathOfFirstModel();
    _updateWithDefinitionFilePathOfFirstModel();
    _updateWithMappingFilePathOfFirstModel();
}


/**
 * @brief Update the flag "can be Frozen" in function of flags of models
 */
void AgentsGroupedByDefinitionVM::_updateCanBeFrozen()
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


/**
 * @brief Update the flag "is Frozen" in function of flags of models
 */
void AgentsGroupedByDefinitionVM::_updateIsFrozen()
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


/**
 * @brief Update the flag "has Log in Stram" in function of flags of models
 */
void AgentsGroupedByDefinitionVM::_updateHasLogInStream()
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


/**
 * @brief Update the flag "is Enabled View Log Stram" in function of flags of models
 */
void AgentsGroupedByDefinitionVM::_updateIsEnabledViewLogStream()
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


/**
 * @brief Update the flag "has Log in File" in function of flags of models
 */
void AgentsGroupedByDefinitionVM::_updateHasLogInFile()
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


/**
 * @brief Update with the state of the first model
 */
void AgentsGroupedByDefinitionVM::_updateWithStateOfFirstModel()
{
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if (model != nullptr) {
            setstate(model->state());
        }
    }
}


/**
 * @brief Update with the log file path of the first model
 */
void AgentsGroupedByDefinitionVM::_updateWithLogFilePathOfFirstModel()
{
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if (model != nullptr) {
            setlogFilePath(model->logFilePath());
        }
    }
}


/**
 * @brief Update with the definition file path of the first model
 */
void AgentsGroupedByDefinitionVM::_updateWithDefinitionFilePathOfFirstModel()
{
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if (model != nullptr) {
            setdefinitionFilePath(model->definitionFilePath());
        }
    }
}


/**
 * @brief Update with the mapping file path of the first model
 */
void AgentsGroupedByDefinitionVM::_updateWithMappingFilePathOfFirstModel()
{
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if (model != nullptr) {
            setmappingFilePath(model->mappingFilePath());
        }
    }
}


