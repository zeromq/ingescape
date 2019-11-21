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

#include "taskscontroller.h"

#include <controller/assessmentsmodelmanager.h>
#include "model/task/independentvariablevaluem.h"

/**
 * @brief Constructor
 * @param modelManager
 * @param parent
 */
TasksController::TasksController(QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr),
    _selectedTask(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Tasks Controller";

    // Fill without type "UNKNOWN"
    _allIndependentVariableValueTypes.fillWithAllEnumValues();
    _allIndependentVariableValueTypes.removeEnumValue(IndependentVariableValueTypes::UNKNOWN);

    // Fill without type "ENUM" and "UNKNOWN"
    _independentVariableValueTypesWithoutEnum.fillWithAllEnumValues();
    _independentVariableValueTypesWithoutEnum.removeEnumValue(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM);
    _independentVariableValueTypesWithoutEnum.removeEnumValue(IndependentVariableValueTypes::UNKNOWN);

}


/**
 * @brief Destructor
 */
TasksController::~TasksController()
{
    qInfo() << "Delete Tasks Controller";

    if (_selectedTask != nullptr) {
        setselectedTask(nullptr);
    }

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr)
    {
        setcurrentExperimentation(nullptr);
    }
}


/**
 * @brief Return true if the user can create a protocol with the name
 * Check if the name is not empty and if a protocol with the same name does not already exist
 * @param protocolName
 * @return
 */
bool TasksController::canCreateProtocolWithName(QString protocolName)
{
    const QList<TaskM*>& taskList = _currentExperimentation->allTasks()->toList();
    auto hasGivenName = [protocolName](TaskM* task) {
        return (task != nullptr) && (task->name() == protocolName);
    };

    return !protocolName.isEmpty() && (_currentExperimentation != nullptr)
            && std::none_of(taskList.begin(), taskList.end(), hasGivenName);
}


/**
 * @brief Create a new protocol with an IngeScape platform file path
 * @param protocolName
 * @param platformFilePath
 */
void TasksController::createNewProtocolWithIngeScapePlatformFilePath(QString protocolName, QString platformFilePath)
{
    if (!protocolName.isEmpty() && !platformFilePath.isEmpty())
    {
        qInfo() << "Create new protocol" << protocolName << "with file" << platformFilePath;

        QUrl platformFileUrl = QUrl(platformFilePath);

        if (platformFileUrl.isValid())
        {
            // Create a new protocol with an IngeScape platform file URL
            _createNewProtocolWithIngeScapePlatformFileUrl(protocolName, platformFileUrl);
        }
        else {
            qWarning() << "Failed to create the protocol" << protocolName << "because the path" << platformFilePath << "is wrong !";
        }
    }
}


/**
 * @brief Delete a task
 * @param task
 */
void TasksController::deleteTask(TaskM* task)
{
    if ((task != nullptr) && (_currentExperimentation != nullptr) && (AssessmentsModelManager::Instance() != nullptr))
    {
        if (task == _selectedTask) {
            setselectedTask(nullptr);
        }

        // Remove task instances related to the task
        QList<CassUuid> subjectUuidList;
        for (SubjectM* subject : *(_currentExperimentation->allSubjects())) {
            if (subject != nullptr)
            {
                subjectUuidList.append(subject->getCassUuid());
            }
        }

        AssessmentsModelManager::deleteEntry<TaskInstanceM>({ { _currentExperimentation->getCassUuid() }, subjectUuidList, { task->getCassUuid() } });

        // Remove from DB
        TaskM::deleteTaskFromCassandraRow(*task);

        // Remove the task from the current experimentation
        _currentExperimentation->removeTask(task);

        // Free memory
        delete task;
    }
}


/**
 * @brief Duplicate a task
 * @param task
 */
void TasksController::duplicateTask(TaskM* task)
{
    if (task != nullptr)
    {
        QString protocolName = QString("%1_copy").arg(task->name());

        // Create a new protocol with an IngeScape platform file URL
        TaskM* newTask = _createNewProtocolWithIngeScapePlatformFileUrl(protocolName, task->platformFileUrl());

        if (newTask != nullptr)
        {
            // Copy each independent variables
            for (IndependentVariableM* independentVariable : task->independentVariables()->toList())
            {
                if (independentVariable != nullptr)
                {
                    // Create the new independent variable
                    IndependentVariableM* newIndependentVariable = _insertIndependentVariableIntoDB(newTask->getExperimentationCassUuid()
                                                                                                    , newTask->getCassUuid()
                                                                                                    , independentVariable->name()
                                                                                                    , independentVariable->description()
                                                                                                    , independentVariable->valueType()
                                                                                                    , independentVariable->enumValues()
                                                                                                    );

                    if (newIndependentVariable != nullptr)
                    {
                        // Add the independent variable to the new task
                        newTask->addIndependentVariable(newIndependentVariable);
                    }
                }
            }

            // Copy each dependent variables
            for (DependentVariableM* dependentVariable : task->dependentVariables()->toList())
            {
                if (dependentVariable != nullptr)
                {
                    // Create the new dependent variable
                    DependentVariableM* newDependentVariable = _insertDependentVariableIntoDB(newTask->getExperimentationCassUuid()
                                                                                              , newTask->getCassUuid()
                                                                                              , dependentVariable->name()
                                                                                              , dependentVariable->description()
                                                                                              , dependentVariable->agentName()
                                                                                              , dependentVariable->outputName()
                                                                                              );

                    if (newDependentVariable != nullptr)
                    {
                        // Add the dependent variable to the new task
                        newTask->addDependentVariable(newDependentVariable);
                    }

                }
            }
        }
    }
}


/**
 * @brief Return true if the user can create an independent variable with the name
 * Check if the name is not empty and if an independent variable with the same name does not already exist
 * @param independentVariableName
 * @return
 */
bool TasksController::canCreateIndependentVariableWithName(QString independentVariableName)
{
    const QList<IndependentVariableM*>& varList = _selectedTask->independentVariables()->toList();
    auto hasGivenName = [independentVariableName](IndependentVariableM* independentVariable) {
        return (independentVariable != nullptr) && (independentVariable->name() == independentVariableName);
    };

    return !independentVariableName.isEmpty() && (_selectedTask != nullptr) && std::none_of(varList.begin(), varList.end(), hasGivenName);
}


/**
 * @brief Return true if the user can create a dependent variable with the name
 * Check if the name is not empty and if a dependent variable with the same name does not already exist
 * @param dependentVariableName
 * @return
 */
bool TasksController::canCreateDependentVariableWithName(QString dependentVariableName)
{
    const QList<DependentVariableM*>& varList = _selectedTask->dependentVariables()->toList();
    auto hasGivenName = [dependentVariableName](DependentVariableM* dependentVariable) {
        return (dependentVariable != nullptr) && (dependentVariable->name() == dependentVariableName);
    };

    return !dependentVariableName.isEmpty() && (_selectedTask != nullptr) && std::none_of(varList.begin(), varList.end(), hasGivenName);
}


/**
 * @brief Return true if the user can edit an independent variable with the name
 * Check if the name is not empty and if a independent variable with the same name does not already exist
 * @param independentVariableCurrentlyEdited
 * @param independentVariableName
 * @return
 */
bool TasksController::canEditIndependentVariableWithName(IndependentVariableM* independentVariableCurrentlyEdited, QString independentVariableName)
{
    const QList<IndependentVariableM*>& varList = _selectedTask->independentVariables()->toList();
    auto hasGivenNameAndIsNotEdited = [independentVariableName, independentVariableCurrentlyEdited](IndependentVariableM* independentVariable){
        return (independentVariable != nullptr) && (independentVariable != independentVariableCurrentlyEdited) && (independentVariable->name() == independentVariableName);
    };

    return (independentVariableCurrentlyEdited != nullptr) && !independentVariableName.isEmpty() && (_selectedTask != nullptr)
            && std::none_of(varList.begin(), varList.end(), hasGivenNameAndIsNotEdited);
}


/**
 * @brief Create a new independent variable
 * @param independentVariableName
 * @param independentVariableDescription
 * @param nIndependentVariableValueType
 */
void TasksController::createNewIndependentVariable(QString independentVariableName,
                                                   QString independentVariableDescription,
                                                   int nIndependentVariableValueType)
{
    if (!independentVariableName.isEmpty() && (nIndependentVariableValueType > -1) && (_selectedTask != nullptr))
    {
        IndependentVariableValueTypes::Value independentVariableValueType = static_cast<IndependentVariableValueTypes::Value>(nIndependentVariableValueType);

        qInfo() << "Create new independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(independentVariableValueType);

        // Create and insert the new independent variable
        IndependentVariableM* independentVariable = _insertIndependentVariableIntoDB(_selectedTask->getExperimentationCassUuid(), _selectedTask->getCassUuid(), independentVariableName, independentVariableDescription, independentVariableValueType, {});
        if (independentVariable != nullptr)
        {
            // Add the independent variable to the selected task
            _selectedTask->addIndependentVariable(independentVariable);
        }
    }
}


/**
 * @brief Create a new independent variable of type enum
 * @param independentVariableName
 * @param independentVariableDescription
 * @param enumValues
 */
void TasksController::createNewIndependentVariableEnum(QString independentVariableName,
                                                       QString independentVariableDescription,
                                                       QStringList enumValues)
{
    if (!independentVariableName.isEmpty() && !enumValues.isEmpty() && (_selectedTask != nullptr))
    {
        qInfo() << "Create new independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM) << "with values:" << enumValues;

        // Create the new independent variable
        IndependentVariableM* independentVariable = _insertIndependentVariableIntoDB(_selectedTask->getExperimentationCassUuid(), _selectedTask->getCassUuid(), independentVariableName, independentVariableDescription, IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM, enumValues);
        if (independentVariable != nullptr)
        {
            // Add the independent variable to the selected task
            _selectedTask->addIndependentVariable(independentVariable);
        }
    }
}


/**
 * @brief Save the modifications of the Independent Variable currently edited
 * @param independentVariableCurrentlyEdited
 * @param independentVariableName
 * @param independentVariableDescription
 * @param nIndependentVariableValueType
 */
void TasksController::saveModificationsOfIndependentVariable(IndependentVariableM* independentVariableCurrentlyEdited,
                                                             QString independentVariableName,
                                                             QString independentVariableDescription,
                                                             int nIndependentVariableValueType)
{
    if ((independentVariableCurrentlyEdited != nullptr) && !independentVariableName.isEmpty() && (nIndependentVariableValueType > -1) && (_selectedTask != nullptr))
    {
        IndependentVariableValueTypes::Value independentVariableValueType = static_cast<IndependentVariableValueTypes::Value>(nIndependentVariableValueType);

        qInfo() << "Edit independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(independentVariableValueType);

        independentVariableCurrentlyEdited->setname(independentVariableName);
        independentVariableCurrentlyEdited->setdescription(independentVariableDescription);
        independentVariableCurrentlyEdited->setvalueType(independentVariableValueType);
        independentVariableCurrentlyEdited->setenumValues(QStringList());
    }
}


/**
 * @brief Save the modifications of the Independent Variable (of type enum) currently edited
 * @param independentVariableCurrentlyEdited
 * @param independentVariableName
 * @param independentVariableDescription
 * @param enumValues
 */
void TasksController::saveModificationsOfIndependentVariableEnum(IndependentVariableM* independentVariableCurrentlyEdited,
                                                                 QString independentVariableName,
                                                                 QString independentVariableDescription,
                                                                 QStringList enumValues)
{
    if ((independentVariableCurrentlyEdited != nullptr) && !independentVariableName.isEmpty() && !enumValues.isEmpty() && (_selectedTask != nullptr))
    {
        qInfo() << "Edit independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM) << "with values:" << enumValues;

        independentVariableCurrentlyEdited->setname(independentVariableName);
        independentVariableCurrentlyEdited->setdescription(independentVariableDescription);
        independentVariableCurrentlyEdited->setvalueType(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM);
        independentVariableCurrentlyEdited->setenumValues(enumValues);
    }
}


/**
 * @brief Delete an independent variable
 * @param independentVariable
 */
void TasksController::deleteIndependentVariable(IndependentVariableM* independentVariable)
{
    if ((independentVariable != nullptr) && (_selectedTask != nullptr) && (AssessmentsModelManager::Instance() != nullptr))
    {
        // Delete independent variable values from Cassandra DB
        QList<CassUuid> taskInstanceUuidList;
        for (TaskInstanceM* taskInstance : _currentExperimentation->allTaskInstances()->toList())
        {
            if (taskInstance != nullptr)
            {
                taskInstanceUuidList.append(taskInstance->getCassUuid());
            }
        }

        AssessmentsModelManager::deleteEntry<IndependentVariableValueM>({ { _currentExperimentation->getCassUuid() },
                                                                          taskInstanceUuidList,
                                                                          { independentVariable->getCassUuid() } });

        // Delete independent variable from Cassandra DB
        IndependentVariableM::deleteIndependentVariableFromCassandra(*independentVariable);

        // Remove the independent variable from the selected task
        _selectedTask->removeIndependentVariable(independentVariable);

        // Free memory
        delete independentVariable;
    }
}


/**
 * @brief Create a new dependent variable
 * @param dependentVariableName
 * @param dependentVariableDescription
 * @param agentName
 * @param outputName
 */
void TasksController::createNewDependentVariable(QString dependentVariableName,
                                                 QString dependentVariableDescription,
                                                 QString agentName,
                                                 QString outputName)
{
    if (!dependentVariableName.isEmpty() && !agentName.isEmpty() && !outputName.isEmpty()
            && (_selectedTask != nullptr) && (AssessmentsModelManager::Instance() != nullptr))
    {
        qInfo() << "Create a new dependent variable" << dependentVariableName << "on output" << outputName << "of agent" << agentName;

        CassUuid dependentVarUuid;
        cass_uuid_gen_time(AssessmentsModelManager::Instance()->getCassUuidGen(), &dependentVarUuid);

        DependentVariableM* dependentVariable = _insertDependentVariableIntoDB(_selectedTask->getExperimentationCassUuid(),
                                                                               _selectedTask->getCassUuid(),
                                                                               dependentVariableName,
                                                                               dependentVariableDescription,
                                                                               agentName,
                                                                               outputName);

        if (dependentVariable != nullptr)
        {
            // Add the dependent variable to the selected task
            _selectedTask->addDependentVariable(dependentVariable);
        }
    }
}


/**
 * @brief Delete a dependent variable
 * @param dependentVariable
 */
// FIXME Unused
/*void TasksController::deleteDependentVariable(DependentVariableM* dependentVariable)
{
    if ((dependentVariable != nullptr) && (_selectedTask != nullptr))
    {
        // Remove the dependent variable from the selected task
        _selectedTask->removeDependentVariable(dependentVariable);

        // Remove from DB
        AssessmentsModelManager::deleteEntry<DependentVariableM>({ _selectedTask->getExperimentationCassUuid(),
                                                                   _selectedTask->getCassUuid() });

        // Free memory
        delete dependentVariable;
    }
}*/


/**
 * @brief Create a new protocol with an IngeScape platform file URL
 * @param protocolName
 * @param platformFileUrl
 * @return
 */
TaskM* TasksController::_createNewProtocolWithIngeScapePlatformFileUrl(QString protocolName, QUrl platformFileUrl)
{
    TaskM* task = nullptr;

    if (!protocolName.isEmpty() && platformFileUrl.isValid() && (_currentExperimentation != nullptr) && (AssessmentsModelManager::Instance() != nullptr))
    {
        // Create the new task
        task = new TaskM(_currentExperimentation->getCassUuid(), AssessmentsModelManager::genCassUuid(), protocolName, platformFileUrl);
        if (task == nullptr || !AssessmentsModelManager::insert(*task)) {
            delete task;
            task = nullptr;
        }
        else {
            // Add the task to the current experimentation
            _currentExperimentation->addTask(task);

            // Select this new task
            setselectedTask(task);
        }
    }
    else {
        qWarning() << "Cannot create new task because name is empty (" << task->name() << ") or group is null !";
    }

    return task;
}


/**
 * @brief Creates a new independent variable with the given parameters and insert it into the Cassandra DB
 * A nullptr is returned if the operation failed.
 * @param experimentationUuid
 * @param taskUuid
 * @param name
 * @param description
 * @param valueType
 * @param enumValues
 * @return
 */
IndependentVariableM* TasksController::_insertIndependentVariableIntoDB(CassUuid experimentationUuid, CassUuid taskUuid, const QString& variableName, const QString& variableDescription, IndependentVariableValueTypes::Value valueType, const QStringList& enumValues)
{
    IndependentVariableM* independentVariable = nullptr;

    if (!variableName.isEmpty() && (AssessmentsModelManager::Instance() != nullptr))
    {
        independentVariable = new IndependentVariableM(experimentationUuid, taskUuid, AssessmentsModelManager::genCassUuid(), variableName, variableDescription, valueType, enumValues);
        if (independentVariable == nullptr || !AssessmentsModelManager::insert(*independentVariable))
        {
            delete independentVariable;
            independentVariable = nullptr;
        }
    }

    return independentVariable;
}


/**
 * @brief Creates a new dependent variable with the given parameters and insert it into the Cassandra DB
 * A nullptr is returned if the operation failed.
 * @param experimentationUuid
 * @param taskUuid
 * @param name
 * @param description
 * @param valueType
 * @param enumValues
 * @return
 */
DependentVariableM* TasksController::_insertDependentVariableIntoDB(CassUuid experimentationUuid,
                                                                    CassUuid taskUuid,
                                                                    const QString& name,
                                                                    const QString& description,
                                                                    const QString& agentName,
                                                                    const QString& outputName)
{
    DependentVariableM* dependentVariable = new DependentVariableM(experimentationUuid,
                                                                   taskUuid,
                                                                   AssessmentsModelManager::genCassUuid(),
                                                                   name,
                                                                   description,
                                                                   agentName,
                                                                   outputName);
    if (dependentVariable == nullptr || !AssessmentsModelManager::insert(*dependentVariable))
    {
        delete dependentVariable;
        dependentVariable = nullptr;
    }

    return dependentVariable;
}
