/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "sessioncontroller.h"

#include <controller/assessmentsmodelmanager.h>
#include "model/eventm.h"
#include "model/actionassessmentm.h"


/**
 * @brief Constructor
 * @param parent
 */
SessionController::SessionController(QObject *parent) : QObject(parent),
    _timeLineC(nullptr),
    _scenarioC(nullptr),
    _currentSession(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Record Controller";

    // Create the controller to manage the time-line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);

    // Create the controller for scenario management
    _scenarioC = new AbstractScenarioController(this);

    // List of agents present in current Platform
    _agentsGroupedByNameInCurrentPlatform.setSourceModel(IngeScapeModelManager::instance()->allAgentsGroupsByName());

    // Connect to the signal "time range changed" from the time line
    // to the scenario controller to filter the action view models
    connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &AbstractScenarioController::onTimeRangeChanged);
}


/**
 * @brief Destructor
 */
SessionController::~SessionController()
{
    qInfo() << "Delete Session Controller";


    // Reset the model of the current session
    if (_currentSession != nullptr)
    {
        setcurrentSession(nullptr);
    }


    //
    // Clean-up sub-controllers
    //

    if (_timeLineC != nullptr)
    {
        //disconnect(_timeLineC);

        AbstractTimeActionslineScenarioViewController* temp = _timeLineC;
        settimeLineC(nullptr);
        delete temp;
    }

    if (_scenarioC != nullptr)
    {
        //disconnect(_scenarioC);

        AbstractScenarioController* temp = _scenarioC;
        setscenarioC(nullptr);
        delete temp;
        temp = nullptr;
    }
}


/**
 * @brief Setter for property "Current Session"
 * @param value
 */
void SessionController::setcurrentSession(SessionM *value)
{
    if (_currentSession != value)
    {
        SessionM *previousSession = _currentSession;

        _currentSession = value;

        // Manage changes
        _oncurrentSessionChanged(previousSession, _currentSession);

        Q_EMIT currentSessionChanged(value);
    }
}


/**
 * @brief Adds the given URLs as attachements for this record
 * @param urlList
 */
void SessionController::addNewAttachements(const QList<QUrl>& urlList)
{
    for (QUrl url : urlList)
    {
        qDebug() << "New attachement:" << url.toString();
    }
}


/**
 * @brief Slot called when the current session changed
 * @param previousSession
 * @param currentSession
 */
void SessionController::_oncurrentSessionChanged(SessionM* previousSession, SessionM* currentSession)
{
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();
    if ((ingeScapeModelManager != nullptr) && (_scenarioC != nullptr))
    {
        //
        // Clean the previous session
        //
        if (previousSession != nullptr)
        {
            if (!previousSession->isRecorded() && !previousSession->recordsList()->isEmpty())
            {
                previousSession->setisRecorded(true);
            }

            // Clear the filter
            _agentsGroupedByNameInCurrentPlatform.setcurrentProtocol(nullptr);

            // Clear the previous mapping
            //clearMapping();

            // Clear the previous scenario
            _scenarioC->clearScenario();

            // Delete all published values
            ingeScapeModelManager->deleteAllPublishedValues();

            // Delete all (models of) actions
            ingeScapeModelManager->deleteAllActions();

            // Delete agents OFF
            QStringList namesListOfAgentsON = ingeScapeModelManager->deleteAgentsOFF();
            qDebug() << "Remaining agents ON:" << namesListOfAgentsON;

            // DIS-connect to the signal "Agent Model ON has been Added" of each agent
            for (AgentsGroupedByNameVM* agentsGroupedByName : IngeScapeModelManager::instance()->allAgentsGroupsByName()->toList())
            {
                if (agentsGroupedByName != nullptr) {
                    disconnect(agentsGroupedByName, &AgentsGroupedByNameVM::agentModelONhasBeenAdded, this, &SessionController::_onAgentModelONhasBeenAdded);
                }
            }
        }


        //
        // Manage the new (current) session
        //
        if ((currentSession != nullptr) && (currentSession->protocol() != nullptr))
        {
            //
            // Load session's protocol
            //
            ProtocolM* protocol = currentSession->protocol();

            if (protocol->platformFileUrl().isValid())
            {
                QString platformFilePath = protocol->platformFileUrl().toLocalFile();

                QFile jsonFile(platformFilePath);
                if (jsonFile.exists())
                {
                    if (jsonFile.open(QIODevice::ReadOnly))
                    {
                        QByteArray byteArrayOfJson = jsonFile.readAll();
                        jsonFile.close();

                        QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);
                        if (jsonDocument.isObject())
                        {
                            QJsonObject jsonRoot = jsonDocument.object();

                            // Version
                            QString versionJsonPlatform = "";
                            if (jsonRoot.contains("version"))
                            {
                                versionJsonPlatform = jsonRoot.value("version").toString();

                                qDebug() << "Version of JSON platform is" << versionJsonPlatform;
                            }
                            else {
                                qDebug() << "UNDEFINED version of JSON platform";
                            }

                            // Import the agents list from JSON
                            if (jsonRoot.contains("agents"))
                            {
                                ingeScapeModelManager->importAgentsListFromJson(jsonRoot.value("agents").toArray(), versionJsonPlatform);
                            }

                            // Import the scenario from JSON
                            if (jsonRoot.contains("scenario"))
                            {
                                _scenarioC->importScenarioFromJson(jsonRoot.value("scenario").toObject());
                            }

                            // Import the global mapping (of agents) from JSON
                            if (jsonRoot.contains("mapping"))
                            {
                                _importMappingFromJson(jsonRoot.value("mapping").toArray());
                            }

                            // Connect to the signal "Agent Model ON has been Added" of each agent
                            // Allows to force an agent (model) to load the wanted mapping (in the protocol) when it appears on the network
                            for (AgentsGroupedByNameVM* agentsGroupedByName : IngeScapeModelManager::instance()->allAgentsGroupsByName()->toList())
                            {
                                if (agentsGroupedByName != nullptr) {
                                    connect(agentsGroupedByName, &AgentsGroupedByNameVM::agentModelONhasBeenAdded, this, &SessionController::_onAgentModelONhasBeenAdded);
                                }
                            }
                        }
                    }
                    else {
                        qCritical() << "Can not open file" << platformFilePath;
                    }
                }
                else {
                    qWarning() << "There is no file" << platformFilePath;
                }

                // Update the filter
                _agentsGroupedByNameInCurrentPlatform.setcurrentProtocol(protocol);
                _agentsGroupedByNameInCurrentPlatform.forceUpdate();
            }
            else {
                qWarning() << "The URL of platform" << protocol->platformFileUrl() << "is not valid";
            }


            //
            // Load session's records if exist
            //
            if (currentSession->recordsList()->isEmpty())
            {
                // Get the list of record
                QList<RecordAssessmentM*> recordList = AssessmentsModelManager::select<RecordAssessmentM>({currentSession->getCassUuid() });
                currentSession->recordsList()->append(recordList);
            }

            // If records list is empty when we load it from DB : session has never been recorded
            currentSession->setisRecorded(!currentSession->recordsList()->isEmpty());


            //
            // Load session's executed actions if session is already recorded
            //
            if (currentSession->isRecorded())
            {
                for (RecordAssessmentM* record : currentSession->recordsList()->toList())
                {
                    CassUuid cassUuidRecord = AssessmentsModelManager::qStringToCassUuid(record->uid());

                    record->executionsList()->clear();
                    QList<EventM*> eventsList = AssessmentsModelManager::select<EventM>({ cassUuidRecord });
                    for (EventM* event : eventsList)
                    {
                        if (event->type() == 9) // 9 is for REC_ACTION_T event type
                        {
                            QList<ActionAssessmentM*> actionsList = AssessmentsModelManager::select<ActionAssessmentM> ({ cassUuidRecord, event->getTimeCassUuid()});

                            // Add executed actions in our record
                            for (ActionAssessmentM* action : actionsList)
                            {
                                action->setexecutionTime(static_cast<int>(record->beginDateTime().msecsTo(event->executionDateTime())) + record->startTimeInTimeline());
                                record->executionsList()->append(action);
                            }
                        }
                    }
                }
            }
        }
    }
}


/**
 * @brief Slot called when a model of agent "ON" has been added to an agent(s grouped by name)
 * @param model
 */
void SessionController::_onAgentModelONhasBeenAdded(AgentM* model)
{
    // Model of Agent ON
    if ((model != nullptr) && model->isON() && !model->name().isEmpty() && !model->peerId().isEmpty()
            && (_currentSession != nullptr) && (_currentSession->protocol() != nullptr)
            && (IngeScapeModelManager::instance() != nullptr) && (IngeScapeNetworkController::instance() != nullptr))
    {
        QString agentName = model->name();

        // The agent is in the current protocol
        if (_currentSession->protocol()->isAgentNameInProtocol(agentName))
        {
            qDebug() << agentName << "is ON and in the protocol --> LOAD the MAPPING !";

            AgentsGroupedByNameVM* agentsGroupedByName = IngeScapeModelManager::instance()->getAgentsGroupedForName(agentName);
            if (agentsGroupedByName != nullptr)
            {
                // Get a string from the JSON
                QString jsonOfMapping = JsonHelper::getJsonOfAgentMapping(agentsGroupedByName->currentMapping(),
                                                                          QJsonDocument::Compact);

                QString message = QString("%1%2").arg(command_LoadMapping, jsonOfMapping);

                // Send the message "LOAD THIS MAPPING" to this agent
                // FIXME: JSON can be too big for a string (use Z-Frame instead ?)
                IngeScapeNetworkController::instance()->sendStringMessageToAgent(model->peerId(), message);
            }
        }
        // The agent is NOT in the current protocol
        /*else
        {
            qDebug() << agentName << "is ON but NOT in the protocol --> CLEAR its MAPPING !";

            // Send the message "Clear Mapping" to this agent
            IngeScapeNetworkController::instance()->sendStringMessageToAgent(model->peerId(), command_ClearMapping);
        }*/
    }
}


/**
 * @brief Import the global mapping (of agents) from JSON
 * @param jsonArrayOfAgentsInMapping
 */
void SessionController::_importMappingFromJson(QJsonArray jsonArrayOfAgentsInMapping)
{
    if ((IngeScapeModelManager::instance() != nullptr) && (IngeScapeNetworkController::instance() != nullptr))
    {
        for (QJsonValue jsonValue : jsonArrayOfAgentsInMapping)
        {
            if (jsonValue.isObject())
            {
                QJsonObject jsonObjectInMapping = jsonValue.toObject();

                if (jsonObjectInMapping.contains("agentName"))
                {
                    // Get values for key "agentName" and "mapping"
                    QJsonValue jsonName = jsonObjectInMapping.value("agentName");
                    QJsonValue jsonMapping = jsonObjectInMapping.value("mapping");

                    if (jsonName.isString() && jsonMapping.isObject())
                    {
                        QString agentName = jsonName.toString();

                        // Get the (view model of) agents grouped for this name
                        AgentsGroupedByNameVM* agentsGroupedByName = IngeScapeModelManager::instance()->getAgentsGroupedForName(agentName);

                        // Create the agent mapping from JSON
                        AgentMappingM* agentMapping = JsonHelper::createModelOfAgentMappingFromJSON(agentName, jsonMapping.toObject());

                        if ((agentsGroupedByName != nullptr) && (agentMapping != nullptr))
                        {
                            // Agent is ON
                            if (agentsGroupedByName->isON())
                            {
                                // Get a string from the JSON
                                QString jsonOfMapping = JsonHelper::getJsonOfAgentMapping(agentMapping,
                                                                                          QJsonDocument::Compact);

                                QString message = QString("%1%2").arg(command_LoadMapping, jsonOfMapping);

                                // Send the message to the agent (list of models of agent)
                                // FIXME: JSON can be too big for a string (use Z-Frame instead ?)
                                IngeScapeNetworkController::instance()->sendStringMessageToAgents(agentsGroupedByName->peerIdsList(), message);
                            }
                            // Agent is OFF
                            else
                            {
                                // Force its current mapping
                                agentsGroupedByName->setcurrentMapping(agentMapping);
                            }
                        }
                    }
                }
            }
        }
    }
}


