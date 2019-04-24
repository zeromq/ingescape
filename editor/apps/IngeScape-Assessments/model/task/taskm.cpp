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

/**
 * @brief Constructor
 * @param name
 * @param parent
 */
TaskM::TaskM(QString name,
             QObject *parent) : QObject(parent),
    _name(name),
    _platformFileUrl(QUrl()),
    _platformFileName(QString("")),
    _agentNamesList(QStringList())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Task" << _name;

    //
    // FIXME for tests
    //
    for (int i = 1; i < 5; i++)
    {
        IndependentVariableM* independentVariable = new IndependentVariableM(QString("VI %1").arg(i), QString("description of VI %1").arg(i), IndependentVariableValueTypes::DOUBLE);

        _independentVariables.append(independentVariable);
    }
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
        if (_platformFileUrl.isValid()) {
            setplatformFileName(_platformFileUrl.fileName());
        }
        else {
            setplatformFileName("");
        }

        Q_EMIT platformFileUrlChanged(value);
    }
}


/**
 * @brief Update the list of agent names and the hash table from an agent name to the list of its outputs names
 * (in the platform of our task)
 * @param agentNamesList
 * @param hashFromAgentNameToOutputNamesList
 */
void TaskM::updateAgentNamesAndOutputNames(QStringList agentNamesList, QHash<QString, QStringList> hashFromAgentNameToOutputNamesList)
{
    setagentNamesList(agentNamesList);

    _hashFromAgentNameToOutputNamesList = hashFromAgentNameToOutputNamesList;

    // For each existing dependent variable
    for (DependentVariableM* dependentVariable : _dependentVariables.toList())
    {
        if (dependentVariable != nullptr)
        {
            // Update the list of output names for this agent name (of the dependent variable)
            _updateOutputNamesListOfDependentVariable(dependentVariable, "");
        }
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
        connect(dependentVariable, &DependentVariableM::agentNameChanged, this, &TaskM::_onAgentNameOfDependentVariableChanged);

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
    disconnect(dependentVariable, nullptr, this, nullptr);

    // Remove from the list
    _dependentVariables.remove(dependentVariable);
}


/**
 * @brief Slot called when the agent name of a dependent variable changed
 * @param agentName
 */
void TaskM::_onAgentNameOfDependentVariableChanged(QString agentName)
{
    DependentVariableM* dependentVariable = qobject_cast<DependentVariableM*>(sender());
    if (dependentVariable != nullptr)
    {
        // Update the list of output names for this agent name (of the dependent variable)
        _updateOutputNamesListOfDependentVariable(dependentVariable, agentName);
    }
}


/**
 * @brief Update the list of output names for an agent name (of a dependent variable)
 * @param dependentVariable
 * @param agentName
 */
void TaskM::_updateOutputNamesListOfDependentVariable(DependentVariableM* dependentVariable, QString agentName)
{
    if (dependentVariable != nullptr)
    {
        //qDebug() << "Agent" << agentName << "selected in" << dependentVariable->name();

        if (agentName.isEmpty())
        {
            dependentVariable->setoutputNamesList(QStringList());
        }
        else
        {
            if (_hashFromAgentNameToOutputNamesList.contains(agentName))
            {
                QStringList outputNamesList = _hashFromAgentNameToOutputNamesList.value(agentName);

                dependentVariable->setoutputNamesList(outputNamesList);
            }
        }
    }
}
