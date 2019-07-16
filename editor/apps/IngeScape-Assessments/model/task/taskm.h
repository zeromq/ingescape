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

    // Reference to experimentation's UUID from Cassandra DB (converted to string)
    I2_CPP_NOSIGNAL_PROPERTY(QString, experimentationUuid)

    // Unique identifier from Cassandra Data Base (converted to string)
    I2_CPP_NOSIGNAL_PROPERTY(QString, uid)

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
     * @param name
     * @param parent
     */
    explicit TaskM(const CassUuid& experimentationUuid, const CassUuid& uid, const QString& name, const QUrl& platformFile, QObject *parent = nullptr);


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
     * @brief Static factory method to create a task from a CassandraDB record
     * @param row
     * @return
     */
    static TaskM* createTaskFromCassandraRow(const CassRow* row);


Q_SIGNALS:


private Q_SLOTS:


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
