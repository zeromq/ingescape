/*
 *	IngeScape Editor
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
#include <QFileDialog>


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
    _clonesNumber(0),
    _canBeRestarted(false),
    _state(""),
    _hasLogInStream(false),
    _hasLogInFile(false),
    _logFilePath(""),
    _definitionFilePath(""),
    _mappingFilePath("")
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
        if (previousValue != NULL)
        {
            // DIS-connect from signals from the previous definition
            disconnect(previousValue, &DefinitionM::commandAskedForOutput, this, &AgentVM::_onCommandAskedToAgentAboutOutput);
            disconnect(previousValue, &DefinitionM::openValuesHistoryOfAgent, this, &AgentVM::_onOpenValuesHistoryOfAgent);
        }

        _definition = value;

        // New value
        if (_definition != NULL)
        {
            // Connect to signal from the new definition
            connect(_definition, &DefinitionM::commandAskedForOutput, this, &AgentVM::_onCommandAskedToAgentAboutOutput);
            connect(_definition, &DefinitionM::openValuesHistoryOfAgent, this, &AgentVM::_onOpenValuesHistoryOfAgent);
        }

        // Emit default signal for QML
        Q_EMIT definitionChanged(value);

        // Emit the signal "Definition Changed" with previous and new values
        Q_EMIT definitionChangedWithPreviousAndNewValues(previousValue, value);
    }
}


/**
 * @brief Change the state of our agent
 */
void AgentVM::changeState()
{
    // is ON --> Kill all agents
    if (_isON) {
        Q_EMIT commandAskedToAgent(_peerIdsList, command_StopAgent);
    }
    // is OFF --> Execute all agents
    else
    {
        foreach (AgentM* model, _models.toList())
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
void AgentVM::changeMuteAllOutputs()
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
void AgentVM::changeFreeze()
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
void AgentVM::loadDefinition()
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
void AgentVM::loadMapping()
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
void AgentVM::downloadDefinition()
{
    // "File Dialog" to get the file path to save
    QString definitionFilePath = QFileDialog::getSaveFileName(NULL,
                                                              "Save definition",
                                                              "",
                                                              "JSON (*.json)");

    if (!definitionFilePath.isEmpty() && (_definition != NULL)) {
        Q_EMIT downloadAgentDefinitionToPath(_definition, definitionFilePath);
    }
}


/**
 * @brief Download the current mapping
 */
void AgentVM::downloadMapping()
{
    // "File Dialog" to get the file path to save
    QString mappingFilePath = QFileDialog::getSaveFileName(NULL,
                                                           "Save mapping",
                                                           "",
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
void AgentVM::changeLogInStream()
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
void AgentVM::changeLogInFile()
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
void AgentVM::saveFilesPaths(QString definitionFilePath, QString mappingFilePath, QString logFilePath)
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
void AgentVM::saveDefinitionToPath()
{
    Q_EMIT commandAskedToAgent(_peerIdsList, command_SaveDefinitionToPath);
}


/**
 * @brief Save the mapping of our agent to its path
 */
void AgentVM::saveMappingToPath()
{
    Q_EMIT commandAskedToAgent(_peerIdsList, command_SaveMappingToPath);
}


/**
 * @brief Open the "Log Stream" of our agent
 */
void AgentVM::openLogStream()
{
    // FIXME TODO
    qDebug() << "Open the 'Log Stream' of" << _name;

    for (AgentM* model : _models.toList())
    {
        if ((model != NULL) && model->isON())
        {
            QString subscriberAddress = QString("tcp://%1:%2").arg(model->address(), model->loggerPort());

            qDebug() << "Address:" << model->address() << "+ Logger Port:" << model->loggerPort() << "--> Subscriber Address:" << subscriberAddress;
        }
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

                // Connect to signals of the model
                connect(model, &AgentM::isONChanged, this, &AgentVM::_onIsONofModelChanged, Qt::UniqueConnection);
                connect(model, &AgentM::canBeRestartedChanged, this, &AgentVM::_onCanBeRestartedOfModelChanged, Qt::UniqueConnection);
                connect(model, &AgentM::isMutedChanged, this, &AgentVM::_onIsMutedOfModelChanged, Qt::UniqueConnection);
                connect(model, &AgentM::isFrozenChanged, this, &AgentVM::_onIsFrozenOfModelChanged, Qt::UniqueConnection);
                connect(model, &AgentM::definitionChanged, this, &AgentVM::_onDefinitionOfModelChanged, Qt::UniqueConnection);
                //connect(model, &AgentM::definitionChangedWithPreviousAndNewValues, this, &AgentVM::_onDefinitionOfModelChangedWithPreviousAndNewValues, Qt::UniqueConnection);
                connect(model, &AgentM::stateChanged, this, &AgentVM::_onStateOfModelChanged, Qt::UniqueConnection);

                connect(model, &AgentM::hasLogInStreamChanged, this, &AgentVM::_onHasLogInStreamOfModelChanged);
                connect(model, &AgentM::hasLogInFileChanged, this, &AgentVM::_onHasLogInFileOfModelChanged);
                connect(model, &AgentM::logFilePathChanged, this, &AgentVM::_onLogFilePathOfModelChanged);
                connect(model, &AgentM::definitionFilePathChanged, this, &AgentVM::_onDefinitionFilePathOfModelChanged);
                connect(model, &AgentM::mappingFilePathChanged, this, &AgentVM::_onMappingFilePathOfModelChanged);
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

                // DIS-connect from signals of the model
                disconnect(model, 0, this, 0);
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
 * @brief Slot when the flag "can Be Restarted" of a model changed
 * @param canBeRestarted
 */
void AgentVM::_onCanBeRestartedOfModelChanged(bool canBeRestarted)
{
    Q_UNUSED(canBeRestarted)

    // Update the flag "can Be Restarted" in function of flags of all models
    _updateCanBeRestarted();
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
    if ((_models.count() > 1) && (_definition != NULL) && (definition != NULL))
    {
        // The modified definition is different from the definition of our view model of agent
        if (!DefinitionM::areIdenticals(_definition, definition))
        {
            AgentM* agent = qobject_cast<AgentM*>(sender());
            if (agent != NULL)
            {
                // Emit the signal "Different Definition Detected on this model of Agent"
                Q_EMIT differentDefinitionDetectedOnModelOfAgent(agent);
            }
        }
        else {
            // Update with the definition of first model
            _updateWithDefinitionOfFirstModel();
        }
    }
    else {
        // Update with the definition of first model
        _updateWithDefinitionOfFirstModel();
    }
}


/**
 * @brief Slot when the state of a model changed
 * @param state
 */
void AgentVM::_onStateOfModelChanged(QString state)
{
    Q_UNUSED(state)

    // Update with the state of the first model
    _updateWithStateOfFirstModel();
}


/**
 * @brief Slot called when the flag "has Log in Stream" of a model changed
 * @param hasLogInStream
 */
void AgentVM::_onHasLogInStreamOfModelChanged(bool hasLogInStream)
{
    Q_UNUSED(hasLogInStream)

    // Update the flag "has Log in Stream" in function of all models
    _updateHasLogInStream();
}


/**
 * @brief Slot called when the flag "has Log in File" of a model changed
 * @param hasLogInFile
 */
void AgentVM::_onHasLogInFileOfModelChanged(bool hasLogInFile)
{
    Q_UNUSED(hasLogInFile)

    // Update the flag "has Log in File" in function of all models
    _updateHasLogInFile();
}


/**
 * @brief Slot called when the path of "Log File" of a model changed
 * @param logFilePath
 */
void AgentVM::_onLogFilePathOfModelChanged(QString logFilePath)
{
    Q_UNUSED(logFilePath)

    // Update with the log file path of the first model
    _updateWithLogFilePathOfFirstModel();
}


/**
 * @brief Slot called when the path of "Definition File" of a model changed
 * @param definitionFilePath
 */
void AgentVM::_onDefinitionFilePathOfModelChanged(QString definitionFilePath)
{
    Q_UNUSED(definitionFilePath)

    // Update with the definition file path of the first model
    _updateWithDefinitionFilePathOfFirstModel();
}


/**
 * @brief Slot called when the path of "Mapping File" of a model changed
 * @param mappingFilePath
 */
void AgentVM::_onMappingFilePathOfModelChanged(QString mappingFilePath)
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

    foreach (AgentM* model, _models.toList())
    {
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
    _updateCanBeRestarted();
    _updateIsMuted();
    _updateIsFrozen();
    _updateHasLogInStream();
    _updateHasLogInFile();

    // Update with the first model
    _updateWithDefinitionOfFirstModel();
    _updateWithStateOfFirstModel();
    _updateWithLogFilePathOfFirstModel();
    _updateWithDefinitionFilePathOfFirstModel();
    _updateWithMappingFilePathOfFirstModel();
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
 * @brief Update the flag "can Be Restarted" in function of flags of models
 */
void AgentVM::_updateCanBeRestarted()
{
    bool globalCanBeRestarted = false;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && model->canBeRestarted()) {
            globalCanBeRestarted = true;
            break;
        }
    }
    setcanBeRestarted(globalCanBeRestarted);
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
 * @brief Update the flag "has Log in Stram" in function of flags of models
 */
void AgentVM::_updateHasLogInStream()
{
    bool globalHasLogInStream = false;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && model->hasLogInStream()) {
            globalHasLogInStream = true;
            break;
        }
    }
    sethasLogInStream(globalHasLogInStream);
}


/**
 * @brief Update the flag "has Log in File" in function of flags of models
 */
void AgentVM::_updateHasLogInFile()
{
    bool globalHasLogInFile = false;

    foreach (AgentM* model, _models.toList()) {
        if ((model != NULL) && model->hasLogInFile()) {
            globalHasLogInFile = true;
            break;
        }
    }
    sethasLogInFile(globalHasLogInFile);
}


/**
 * @brief Update with the definition of the first model
 */
void AgentVM::_updateWithDefinitionOfFirstModel()
{
    DefinitionM* definition = NULL;
    if (!_models.isEmpty())
    {
         AgentM* model = _models.at(0);
         if (model != NULL) {
             definition = model->definition();
         }
    }
    setdefinition(definition);
}


/**
 * @brief Update with the state of the first model
 */
void AgentVM::_updateWithStateOfFirstModel()
{
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if (model != NULL) {
            setstate(model->state());
        }
    }
}


/**
 * @brief Update with the log file path of the first model
 */
void AgentVM::_updateWithLogFilePathOfFirstModel()
{
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if (model != NULL) {
            setlogFilePath(model->logFilePath());
        }
    }
}


/**
 * @brief Update with the definition file path of the first model
 */
void AgentVM::_updateWithDefinitionFilePathOfFirstModel()
{
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if (model != NULL) {
            setdefinitionFilePath(model->definitionFilePath());
        }
    }
}


/**
 * @brief Update with the mapping file path of the first model
 */
void AgentVM::_updateWithMappingFilePathOfFirstModel()
{
    if (!_models.isEmpty())
    {
        AgentM* model = _models.at(0);
        if (model != NULL) {
            setmappingFilePath(model->mappingFilePath());
        }
    }
}
