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

#ifndef PROTOCOLM_H
#define PROTOCOLM_H

#include <QObject>
#include "I2PropertyHelpers.h"

#include <model/protocol/dependentvariablem.h>
#include <model/protocol/independentvariablem.h>
#include <model/protocol/agentnameandoutputsm.h>

#include "cassandra.h"


/**
 * @brief The ProtocolM class defines a model of task
 */
class ProtocolM : public QObject
{
    Q_OBJECT

    // Name of our task
    I2_QML_PROPERTY(QString, name)

    // URL of the IngeScape platform (JSON file) of our task
    I2_CPP_PROPERTY_CUSTOM_SETTER(QUrl, platformFileUrl)

    // Name of the IngeScape platform (JSON file) of our task
    I2_QML_PROPERTY_READONLY(QString, platformFileName)

    // List of independent variables of our task
    I2_QOBJECT_LISTMODEL(IndependentVariableM, independentVariables)

    // List of dependent variables of our task
    I2_QOBJECT_LISTMODEL(DependentVariableM, dependentVariables)

    // Hash table from an agent name to a (simplified) model of agent with its name and its outputs
    // Found in the platform (JSON file) of our task
    I2_QOBJECT_HASHMODEL(AgentNameAndOutputsM, hashFromAgentNameToSimplifiedAgent)


public:
    /**
     * @brief Constructor
     * @param experimentationUuid
     * @param uid
     * @param name
     * @param platformFile
     * @param parent
     */
    explicit ProtocolM(const CassUuid& experimentationUuid,
                   const CassUuid& uid,
                   const QString& name,
                   const QUrl& platformFile,
                   QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ProtocolM();


    /**
     * @brief Accessor for this task UUID in the Cassandra DB
     * @return
     */
    CassUuid getCassUuid() const { return _cassUuid; }


    /**
     * @brief Accessor for the task's experimentation UUID in the Cassandra DB
     * @return
     */
    CassUuid getExperimentationCassUuid() const { return _cassExperimentationUuid; }


    /**
     * @brief Add an Independent Variable to our task
     * @param independentVariable
     */
    void addIndependentVariable(IndependentVariableM* independentVariable);


    /**
     * @brief Remove an Independent Variable from our task
     * @param independentVariable
     */
    void removeIndependentVariable(IndependentVariableM* independentVariable);


    /**
     * @brief Return the independent variable instance corresponding to the given UUID
     * @param cassUuid
     * @return
     */
    IndependentVariableM* getIndependentVariableFromUuid(const CassUuid& cassUuid) const;


    /**
     * @brief Add a Dependent Variable to our task
     * @param dependentVariable
     */
    void addDependentVariable(DependentVariableM* dependentVariable);


    /**
     * @brief Remove a Dependent Variable from our task
     * @param dependentVariable
     */
    void removeDependentVariable(DependentVariableM* dependentVariable);


    /**
     * @brief Task table name
     */
    static const QString table;

    static const QStringList columnNames;

    static const QStringList primaryKeys;

    /**
     * @brief Static factory method to create a task from a CassandraDB record
     * @param row
     * @return
     */
    static ProtocolM* createFromCassandraRow(const CassRow* row);

    /**
     * @brief Delete the given protocol from the Cassandra DB
     * @param protocol
     */
    static void deleteProtocolFromCassandraRow(const ProtocolM& protocol);

    /**
     * @brief Create a CassStatement to insert an ProtocolM into the DB.
     * The statement contains the values from the given task.
     * Passed task must have a valid and unique UUID.
     * @param task
     * @return
     */
    static CassStatement* createBoundInsertStatement(const ProtocolM& task);

    /**
     * @brief Returns true if the agent name is in the hashTable hashFromAgentNameToSimplifiedAgent
     * @param agentName
     * @return
     */
    bool isAgentNameInProtocol(QString agentName);


    /**
     * @brief Delete the given dependent variable from the task and from the Cassandra DB
     * @param variableToUpdate
     */
    Q_INVOKABLE void deleteDependentVariable(DependentVariableM* variableToDelete);


private:

    // Experimentation's UUID from Cassandra DB
    CassUuid _cassExperimentationUuid;

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;

    /**
     * @brief Update the list of agents from a platform file path
     * Update the hash table from an agent name to a (simplified) model of agent with its name and its outputs
     * @param platformFilePath
     */
    void _updateAgentsFromPlatformFilePath(QString platformFilePath);


private:


};

QML_DECLARE_TYPE(ProtocolM)

#endif // PROTOCOLM_H
