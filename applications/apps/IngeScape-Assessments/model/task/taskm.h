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

#ifndef TASKM_H
#define TASKM_H

#include <QObject>
#include "I2PropertyHelpers.h"

#include <model/task/dependentvariablem.h>
#include <model/task/independentvariablem.h>
#include <model/task/agentnameandoutputsm.h>

#include "cassandra.h"


/**
 * @brief The TaskM class defines a model of task
 */
class TaskM : public QObject
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

    // Temporary independent variable used for edition rollbacks
    I2_QML_PROPERTY(IndependentVariableM*, temporaryIndependentVariable)

    // Temporary dependent variable used for edition rollbacks
    I2_QML_PROPERTY(DependentVariableM*, temporaryDependentVariable)


public:
    /**
     * @brief Constructor
     * @param experimentationUuid
     * @param uid
     * @param name
     * @param platformFile
     * @param parent
     */
    explicit TaskM(const CassUuid& experimentationUuid,
                   const CassUuid& uid,
                   const QString& name,
                   const QUrl& platformFile,
                   QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TaskM();


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
    static TaskM* createFromCassandraRow(const CassRow* row);

    /**
     * @brief Delete the given task from the Cassandra DB
     * @param task
     */
    static void deleteTaskFromCassandraRow(const TaskM& task);

    /**
     * @brief Create a CassStatement to insert an TaskM into the DB.
     * The statement contains the values from the given task.
     * Passed task must have a valid and unique UUID.
     * @param task
     * @return
     */
    static CassStatement* createBoundInsertStatement(const TaskM& task);

    /**
     * @brief Returns true if the agent name is in the hashTable hashFromAgentNameToSimplifiedAgent
     * @param agentName
     * @return
     */
    bool isAgentNameInProtocol(QString agentName);

    /**
     * @brief Initialize the temporary dependent variable with the given dependent variable
     * @param baseVariable
     */
    Q_INVOKABLE void initTemporaryDependentVariable(DependentVariableM* baseVariable);


    /**
     * @brief Initialize the temporary independent variable with the given independent variable
     * @param baseVariable
     */
    Q_INVOKABLE void initTemporaryIndependentVariable(IndependentVariableM* baseVariable);


    /**
     * @brief Apply the values from the temporary dependent variable to the given dependent variable.
     * Update said dependent variable into the Cassandra DB
     * @param variableToUpdate
     */
    Q_INVOKABLE void applyTemporaryDependentVariable(DependentVariableM* variableToUpdate);


    /**
     * @brief Apply the values from the temporary independent variable to the given independent variable.
     * Update said independent variable into the Cassandra DB
     * @param variableToUpdate
     */
    Q_INVOKABLE void applyTemporaryIndependentVariable(IndependentVariableM* variableToUpdate);


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

QML_DECLARE_TYPE(TaskM)

#endif // TASKM_H
