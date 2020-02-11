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

#include "protocolm.h"

#include "controller/assessmentsmodelmanager.h"


/**
 * @brief Protocol table name
 */
const QString ProtocolM::table = "ingescape.protocol";

const QStringList ProtocolM::columnNames = {
    "id_experimentation",
    "id",
    "name",
    "platform_file",
};

const QStringList ProtocolM::primaryKeys = {
    "id_experimentation",
    "id",
};

/**
 * @brief Constructor
 * @param name
 * @param parent
 */
ProtocolM::ProtocolM(const CassUuid& experimentationUuid, const CassUuid& uid, const QString& name, const QUrl& platformFile, QObject *parent)
    : QObject(parent)
    , _name(name)
    , _platformFileUrl(QUrl())
    , _platformFileName("")
    , _cassExperimentationUuid(experimentationUuid)
    , _cassUuid(uid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setplatformFileUrl(platformFile);

    qInfo() << "New Model of Protocol" << _name;
}


/**
 * @brief Destructor
 */
ProtocolM::~ProtocolM()
{
    qInfo() << "Delete Model of Protocol" << _name;

    // Free memory
    _independentVariables.deleteAllItems();
    _dependentVariables.deleteAllItems();
    _hashFromAgentNameToSimplifiedAgent.deleteAllItems();
}


/**
 * @brief Setter for property "Platform File Url"
 * @param value
 */
void ProtocolM::setplatformFileUrl(QUrl value)
{
    if (_platformFileUrl != value)
    {
        _platformFileUrl = value;

        // Update file name
        if (_platformFileUrl.isValid())
        {
            setplatformFileName(_platformFileUrl.fileName());

            // Update the hash table from an agent name to a (simplified) model of agent with its name and its outputs
            _updateAgentsFromPlatformFilePath(_platformFileUrl.toLocalFile());
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
 * @brief Add an Independent Variable to our protocol
 * @param independentVariable
 */
void ProtocolM::addIndependentVariable(IndependentVariableM* independentVariable)
{
    if (independentVariable != nullptr)
    {
        // Add to the list
        _independentVariables.append(independentVariable);
    }
}


/**
 * @brief Remove an Independent Variable from our protocol
 * @param independentVariable
 */
void ProtocolM::removeIndependentVariable(IndependentVariableM* independentVariable)
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
IndependentVariableM* ProtocolM::getIndependentVariableFromUuid(const CassUuid& cassUuid) const
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
 * @brief Add a Dependent Variable to our protocol
 * @param dependentVariable
 */
void ProtocolM::addDependentVariable(DependentVariableM* dependentVariable)
{
    if (dependentVariable != nullptr)
    {
        // Add to the list
        _dependentVariables.append(dependentVariable);
    }
}


/**
 * @brief Remove a Dependent Variable from our protocol
 * @param dependentVariable
 */
void ProtocolM::removeDependentVariable(DependentVariableM* dependentVariable)
{
    // Remove from the list
    _dependentVariables.remove(dependentVariable);
}


/**
 * @brief Static factory method to create a protocol from a CassandraDB record
 * @param row
 * @return
 */
ProtocolM* ProtocolM::createFromCassandraRow(const CassRow* row)
{
    ProtocolM* protocol = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, protocolUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &protocolUuid);

        QString protocolName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QUrl platformUrl(AssessmentsModelManager::getStringValueFromColumnName(row, "platform_file"));

        protocol = new ProtocolM(experimentationUuid,
                                 protocolUuid,
                                 protocolName,
                                 platformUrl);
    }

    return protocol;
}

/**
 * @brief Delete the given protocol from the Cassandra DB
 * @param protocol
 */
void ProtocolM::deleteProtocolFromCassandraRow(const ProtocolM& protocol)
{
    // Remove independent_var from DB
    AssessmentsModelManager::deleteEntry<IndependentVariableM>({ protocol.getExperimentationCassUuid(), protocol.getCassUuid() });

    // Remove dependent_var from DB
    AssessmentsModelManager::deleteEntry<DependentVariableM>({ protocol.getExperimentationCassUuid(), protocol.getCassUuid() });

    // Remove protocol from DB
    AssessmentsModelManager::deleteEntry<ProtocolM>({ protocol.getExperimentationCassUuid(), protocol.getCassUuid() });
}


/**
 * @brief Create a CassStatement to insert an ProtocolM into the DB.
 * The statement contains the values from the given protocol.
 * Passed protocol must have a valid and unique UUID.
 * @param protocol
 * @return
 */
CassStatement* ProtocolM::createBoundInsertStatement(const ProtocolM& protocol)
{
    QString queryStr = "INSERT INTO " + ProtocolM::table + " (id_experimentation, id, name, platform_file) VALUES (?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 4);
    cass_statement_bind_uuid  (cassStatement, 0, protocol.getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 1, protocol.getCassUuid());
    cass_statement_bind_string(cassStatement, 2, protocol.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 3, protocol.platformFileUrl().toString().toStdString().c_str());
    return cassStatement;
}

/**
 * @brief Returns true if the agent name is in the hashFromAgentNameToSimplifiedAgent
 * @param agentName
 * @return
 */
bool ProtocolM::isAgentNameInProtocol(QString agentName){
    return _hashFromAgentNameToSimplifiedAgent.containsKey(agentName);
}


/**
 * @brief Delete the given dependent variable from the protocol and from the Cassandra DB
 * @param variableToDelete
 */
void ProtocolM::deleteDependentVariable(DependentVariableM* variableToDelete)
{
    if (variableToDelete != nullptr)
    {
        _dependentVariables.remove(variableToDelete);

        AssessmentsModelManager::deleteEntry<DependentVariableM>({ variableToDelete->getExperimentationCassUuid(),
                                                                   variableToDelete->getProtocolCassUuid(),
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
void ProtocolM::_updateAgentsFromPlatformFilePath(QString platformFilePath)
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

                            if (!_hashFromAgentNameToSimplifiedAgent.containsKey(agentName) && !outputNamesList.isEmpty())
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
