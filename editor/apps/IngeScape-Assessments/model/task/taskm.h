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

    // List of agent names in the IngeScape platform of our task
    I2_QML_PROPERTY_READONLY(QStringList, agentNamesList)


public:
    /**
     * @brief Constructor
     * @param name
     * @param parent
     */
    explicit TaskM(QString name,
                   QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TaskM();


    /**
     * @brief Update the list of agent names and the hash table from an agent name to the list of its outputs names
     * (in the platform of our task)
     * @param agentNamesList
     * @param hashFromAgentNameToOutputNamesList
     */
    void updateAgentNamesAndOutputNames(QStringList agentNamesList, QHash<QString, QStringList> hashFromAgentNameToOutputNamesList);


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


Q_SIGNALS:


private Q_SLOTS:

    /**
     * @brief Slot called when the agent name of a dependent variable changed
     * @param agentName
     */
    void _onAgentNameOfDependentVariableChanged(QString agentName);


private:

    /**
     * @brief Update the list of output names for an agent name (of a dependent variable)
     * @param dependentVariable
     * @param agentName
     */
    void _updateOutputNamesListOfDependentVariable(DependentVariableM* dependentVariable, QString agentName);


    /**
     * @brief Update the list of agents from a platform file path
     * Update the hash table from an agent name to a (simplified) model of agent with its name and its outputs
     * @param platformFilePath
     */
    void _updateAgentsFromPlatformFilePath(QString platformFilePath);


private:

    // Hash table from an agent name to the list of its outputs names (in the platform of our task)
    QHash<QString, QStringList> _hashFromAgentNameToOutputNamesList;

};

QML_DECLARE_TYPE(TaskM)

#endif // TASKM_H
