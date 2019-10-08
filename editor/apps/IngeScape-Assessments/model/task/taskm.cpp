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
 * @brief Task table name
 */
const QString TaskM::table = "ingescape.task";

const QStringList TaskM::columnNames = {
    "id_experimentation",
    "id",
    "name",
    "platform_file",
};

const QStringList TaskM::primaryKeys = {
    "id_experimentation",
    "id",
};

/**
 * @brief Constructor
 * @param name
 * @param parent
 */
TaskM::TaskM(const CassUuid& experimentationUuid, const CassUuid& uid, const QString& name, const QUrl& platformFile, QObject *parent)
    : QObject(parent)
    , _name(name)
    , _platformFileUrl(QUrl())
    , _platformFileName("")
    , _temporaryIndependentVariable(nullptr)
    , _temporaryDependentVariable(nullptr)
    , _cassExperimentationUuid(experimentationUuid)
    , _cassUuid(uid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setplatformFileUrl(platformFile);

    // Create the temporary Independent & Dependent variables
    _temporaryIndependentVariable = new IndependentVariableM(CassUuid(), CassUuid(), CassUuid(), "", "", IndependentVariableValueTypes::UNKNOWN);
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

    if (_temporaryIndependentVariable != nullptr)
    {
        IndependentVariableM* tmp = _temporaryIndependentVariable;
        settemporaryIndependentVariable(nullptr);
        delete tmp;
    }
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
 * @brief Return the independent variable instance corresponding to the given UUID
 * @param cassUuid
 * @return
 */
IndependentVariableM* TaskM::getIndependentVariableFromUuid(const CassUuid& cassUuid) const
{
    IndependentVariableM* indepVar = nullptr;
    auto indepVarIterator = std::find_if(_independentVariables.begin(),
                                         _independentVariables.end(),
                                         [&cassUuid](IndependentVariableM* indepVar)
    {
            return AssessmentsModelManager::cassUuidToQString(cassUuid) == AssessmentsModelManager::cassUuidToQString(indepVar->getCassUuid());
    });

    if (indepVarIterator != _independentVariables.end())
    {
        indepVar = *indepVarIterator;
    }

    return indepVar;
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
TaskM* TaskM::createFromCassandraRow(const CassRow* row)
{
    TaskM* task = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, taskUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &taskUuid);

        QString protocolName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QUrl platformUrl(AssessmentsModelManager::getStringValueFromColumnName(row, "platform_file"));

        task = new TaskM(experimentationUuid, taskUuid, protocolName, platformUrl);
    }

    return task;
}

/**
 * @brief Delete the given task from the Cassandra DB
 * @param task
 */
void TaskM::deleteTaskFromCassandraRow(const TaskM& task)
{

    // Remove independent_var from DB
    AssessmentsModelManager::deleteEntry<IndependentVariableM>({ task.getExperimentationCassUuid(), task.getCassUuid() });

    // Remove dependent_var from DB
    AssessmentsModelManager::deleteEntry<DependentVariableM>({ task.getExperimentationCassUuid(), task.getCassUuid() });

    // Remove task from DB
    AssessmentsModelManager::deleteEntry<TaskM>({ task.getExperimentationCassUuid(), task.getCassUuid() });
}


/**
 * @brief Create a CassStatement to insert an TaskM into the DB.
 * The statement contains the values from the given task.
 * Passed task must have a valid and unique UUID.
 * @param task
 * @return
 */
CassStatement* TaskM::createBoundInsertStatement(const TaskM& task)
{
    QString queryStr = "INSERT INTO " + TaskM::table + " (id_experimentation, id, name, platform_file) VALUES (?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 4);
    cass_statement_bind_uuid  (cassStatement, 0, task.getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 1, task.getCassUuid());
    cass_statement_bind_string(cassStatement, 2, task.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 3, task.platformFileUrl().toString().toStdString().c_str());
    return cassStatement;
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
 * @brief Initialize the temporary independent variable with the given independent variable
 * @param baseVariable
 */
void TaskM::initTemporaryIndependentVariable(IndependentVariableM* baseVariable)
{
    if ((_temporaryIndependentVariable != nullptr) && (baseVariable != nullptr))
    {
        _temporaryIndependentVariable->setname(baseVariable->name());
        _temporaryIndependentVariable->setdescription(baseVariable->description());
        _temporaryIndependentVariable->setvalueType(baseVariable->valueType());
        _temporaryIndependentVariable->setenumValues(baseVariable->enumValues());
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

        AssessmentsModelManager::update(*variableToUpdate);
    }
}

/**
 * @brief Delete the given dependent variable from the task and from the Cassandra DB
 * @param variableToDelete
 */
void TaskM::deleteDependentVariable(DependentVariableM* variableToDelete)
{
    if (variableToDelete != nullptr)
    {
        _dependentVariables.remove(variableToDelete);

        AssessmentsModelManager::deleteEntry<DependentVariableM>({ variableToDelete->getExperimentationCassUuid(),
                                                                   variableToDelete->getTaskCassUuid(),
                                                                   variableToDelete->getCassUuid() });

        // Free memory
        delete variableToDelete;
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
