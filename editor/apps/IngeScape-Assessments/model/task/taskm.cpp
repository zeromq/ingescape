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

#include "taskm.h"

#include "controller/assessmentsmodelmanager.h"

/**
 * @brief Constructor
 * @param name
 * @param parent
 */
TaskM::TaskM(const CassUuid& experimentationUuid, const CassUuid& uid, const QString& name, const QUrl& platformFile, QObject *parent)
    : QObject(parent)
    , _experimentationUuid(AssessmentsModelManager::cassUuidToQString(experimentationUuid))
    , _uid(AssessmentsModelManager::cassUuidToQString(uid))
    , _name(name)
    , _platformFileUrl(QUrl())
    , _platformFileName("")
    , _temporaryDependentVariable(nullptr)
    , _cassExperimentationUuid(experimentationUuid)
    , _cassUuid(uid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setplatformFileUrl(platformFile);

    _temporaryDependentVariable = new DependentVariableM(CassUuid(), CassUuid(), CassUuid(), "", "", "", "");

    qInfo() << "New Model of Task" << _name;
}


/**
 * @brief Destructor
 */
TaskM::~TaskM()
{
    qInfo() << "Delete Model of Task" << _name;

    // Free memory
    _independentVariables.deleteAllItems();
    _dependentVariables.deleteAllItems();
    _hashFromAgentNameToSimplifiedAgent.deleteAllItems();

    if (_temporaryDependentVariable != nullptr)
    {
        DependentVariableM* tmp = _temporaryDependentVariable;
        settemporaryDependentVariable(nullptr);
        delete tmp;
    }
}


/**
 * @brief Setter for property "Platform File Url"
 * @param value
 */
void TaskM::setplatformFileUrl(QUrl value)
{
    if (_platformFileUrl != value)
    {
        _platformFileUrl = value;

        // Update file name
        if (_platformFileUrl.isValid())
        {
            setplatformFileName(_platformFileUrl.fileName());

            // Update the hash table from an agent name to a (simplified) model of agent with its name and its outputs
            _updateAgentsFromPlatformFilePath(_platformFileUrl.path());
        }
        else
        {
            setplatformFileName("");

            // Free memory
            _hashFromAgentNameToSimplifiedAgent.deleteAllItems();
        }

        Q_EMIT platformFileUrlChanged(value);
    }
}


/**
 * @brief Add an Independent Variable to our task
 * @param independentVariable
 */
void TaskM::addIndependentVariable(IndependentVariableM* independentVariable)
{
    if (independentVariable != nullptr)
    {
        // Add to the list
        _independentVariables.append(independentVariable);
    }
}


/**
 * @brief Remove an Independent Variable from our task
 * @param independentVariable
 */
void TaskM::removeIndependentVariable(IndependentVariableM* independentVariable)
{
    if (independentVariable != nullptr)
    {
        // Remove from the list
        _independentVariables.remove(independentVariable);
    }
}


/**
 * @brief Add a Dependent Variable to our task
 * @param dependentVariable
 */
void TaskM::addDependentVariable(DependentVariableM* dependentVariable)
{
    if (dependentVariable != nullptr)
    {
        // Add to the list
        _dependentVariables.append(dependentVariable);
    }
}


/**
 * @brief Remove a Dependent Variable from our task
 * @param dependentVariable
 */
void TaskM::removeDependentVariable(DependentVariableM* dependentVariable)
{
    // Remove from the list
    _dependentVariables.remove(dependentVariable);
}


/**
 * @brief Static factory method to create a task from a CassandraDB record
 * @param row
 * @return
 */
TaskM* TaskM::createTaskFromCassandraRow(const CassRow* row)
{
    TaskM* task = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, taskUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &taskUuid);

        const char *chrTaskName = "";
        size_t nameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrTaskName, &nameLength);
        QString taskName = QString::fromUtf8(chrTaskName, static_cast<int>(nameLength));

        const char *chrPlatformUrl = "";
        size_t platformUrlLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "platform_file"), &chrPlatformUrl, &platformUrlLength);
        QUrl platformUrl(QString::fromUtf8(chrPlatformUrl, static_cast<int>(platformUrlLength)));

        task = new TaskM(experimentationUuid, taskUuid, taskName, platformUrl);
    }

    return task;
}

/**
 * @brief Delete the given task from the Cassandra DB
 * @param task
 */
void TaskM::deleteTaskFromCassandra(const TaskM& task)
{

    // Remove independent_var from DB
    const char* query = "DELETE FROM ingescape.independent_var WHERE id_experimentation = ? AND id_task = ?;";
    CassStatement* cassStatement = cass_statement_new(query, 2);
    cass_statement_bind_uuid(cassStatement, 0, task.getExperimentationCassUuid());
    cass_statement_bind_uuid(cassStatement, 1, task.getCassUuid());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "Independent variables for task" << task.name() << "has been successfully deleted from the DB";
    }
    else {
        qCritical() << "Could not delete the independent variables for the task" << task.name() << "from the DB:" << cass_error_desc(cassError);
    }

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);

    // Remove dependent_var from DB
    query = "DELETE FROM ingescape.dependent_var WHERE id_experimentation = ? AND id_task = ?;";
    cassStatement = cass_statement_new(query, 2);
    cass_statement_bind_uuid(cassStatement, 0, task.getExperimentationCassUuid());
    cass_statement_bind_uuid(cassStatement, 1, task.getCassUuid());

    // Execute the query or bound statement
    cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "Dependent variables for task" << task.name() << "has been successfully deleted from the DB";
    }
    else {
        qCritical() << "Could not delete the dependent variables for the task" << task.name() << "from the DB:" << cass_error_desc(cassError);
    }

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);

    // Remove task from DB
    query = "DELETE FROM ingescape.task WHERE id_experimentation = ? AND id = ?;";
    cassStatement = cass_statement_new(query, 2);
    cass_statement_bind_uuid(cassStatement, 0, task.getExperimentationCassUuid());
    cass_statement_bind_uuid(cassStatement, 1, task.getCassUuid());

    // Execute the query or bound statement
    cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "Task" << task.name() << "has been successfully deleted from the DB";
    }
    else {
        qCritical() << "Could not delete the task" << task.name() << "from the DB:" << cass_error_desc(cassError);
    }

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);
}

/**
 * @brief Initialize the temporary dependent variable with the given dependent variable
 * @param baseVariable
 */
void TaskM::initTemporaryDependentVariable(DependentVariableM* baseVariable)
{
    if ((_temporaryDependentVariable != nullptr) && (baseVariable != nullptr))
    {
        _temporaryDependentVariable->setname(baseVariable->name());
        _temporaryDependentVariable->setdescription(baseVariable->description());
        _temporaryDependentVariable->setagentName(baseVariable->agentName());
        _temporaryDependentVariable->setoutputName(baseVariable->outputName());
    }
}


/**
 * @brief Apply the values from the temporary dependent variable to the givend dependent variable.
 * Update said dependent variable into the Cassandra DB
 * @param variableToUpdate
 */
void TaskM::applyTemporaryDependentVariable(DependentVariableM* variableToUpdate)
{
    if ((variableToUpdate != nullptr) && (_temporaryDependentVariable != nullptr))
    {
        variableToUpdate->setname(_temporaryDependentVariable->name());
        variableToUpdate->setdescription(_temporaryDependentVariable->description());
        variableToUpdate->setagentName(_temporaryDependentVariable->agentName());
        variableToUpdate->setoutputName(_temporaryDependentVariable->outputName());

        DependentVariableM::updateDependentVariableIntoCassandraDB(*variableToUpdate);
    }
}

/**
 * @brief Delete the given dependent variable from the task and from the Cassandra DB
 * @param variableToUpdate
 */
void TaskM::deleteDependentVariable(DependentVariableM* variableToDelete)
{
    if (variableToDelete != nullptr)
    {
        _dependentVariables.remove(variableToDelete);

        DependentVariableM::deleteDependentVariableFromCassandraDB(*variableToDelete);
    }
}

/**
 * @brief Update the list of agents from a platform file path
 * Update the hash table from an agent name to a (simplified) model of agent with its name and its outputs
 * @param platformFilePath
 */
void TaskM::_updateAgentsFromPlatformFilePath(QString platformFilePath)
{
    // Clear the hash table
    _hashFromAgentNameToSimplifiedAgent.deleteAllItems();

    QFile jsonFile(platformFilePath);
    if (jsonFile.exists())
    {
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();

            QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

            QJsonObject jsonRoot = jsonDocument.object();

            // List of agents
            if (jsonRoot.contains("agents"))
            {
                for (QJsonValue jsonIteratorAgent : jsonRoot.value("agents").toArray())
                {
                    if (jsonIteratorAgent.isObject())
                    {
                        QJsonObject jsonAgentsGroupedByName = jsonIteratorAgent.toObject();

                        QJsonValue jsonAgentName = jsonAgentsGroupedByName.value("agentName");
                        QJsonValue jsonAgentDefinitions = jsonAgentsGroupedByName.value("definitions");

                        if (jsonAgentName.isString() && jsonAgentDefinitions.isArray())
                        {
                            // Init parameters
                            QString agentName = jsonAgentName.toString();
                            QStringList outputNamesList = QStringList();

                            // Array of definitions
                            for (QJsonValue jsonIteratorDefinition : jsonAgentDefinitions.toArray())
                            {
                                QJsonObject jsonAgentsGroupedByDefinition = jsonIteratorDefinition.toObject();

                                QJsonValue jsonDefinition = jsonAgentsGroupedByDefinition.value("definition");

                                // Definition
                                if (jsonDefinition.isObject())
                                {
                                    QJsonObject jsonObjectDefinition = jsonDefinition.toObject();

                                    QJsonValue jsonOutputs = jsonObjectDefinition.value("outputs");

                                    if (jsonOutputs.isArray())
                                    {
                                        // Array of outputs
                                        for (QJsonValue jsonOutput : jsonOutputs.toArray())
                                        {
                                            if (jsonOutput.isObject())
                                            {
                                                QJsonObject jsonObjectOutput = jsonOutput.toObject();

                                                QJsonValue jsonOutputName = jsonObjectOutput.value("name");
                                                //QJsonValue jsonType = jsonObjectOutput.value("type");

                                                if (jsonOutputName.isString() && !outputNamesList.contains(jsonOutputName.toString()))
                                                {
                                                    outputNamesList.append(jsonOutputName.toString());
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            if (!_hashFromAgentNameToSimplifiedAgent.containsKey(agentName))
                            {
                                // Create the (simplified) agent with a name and a list of output names
                                AgentNameAndOutputsM* agentNameAndOutputs = new AgentNameAndOutputsM(agentName, outputNamesList);
                                _hashFromAgentNameToSimplifiedAgent.insert(agentName, agentNameAndOutputs);
                            }
                            else {
                                qCritical() << "There is already a (simplified) agent with name" << agentName;
                            }
                        }
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
}
