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
 * @param parent
 */
TasksController::TasksController(QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr),
    _selectedTask(nullptr),
    _temporaryIndependentVariable(nullptr),
    _temporaryDependentVariable(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Tasks Controller";

    // Init temporary independent and dependant variable as empty variables
    _temporaryIndependentVariable = new IndependentVariableM(CassUuid(), CassUuid(), CassUuid(), "", "", IndependentVariableValueTypes::UNKNOWN);
    _temporaryDependentVariable = new DependentVariableM(CassUuid(), CassUuid(), CassUuid(), "", "", "", "");

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

    // Reset the model of the temporary independant variable
    if (_temporaryIndependentVariable != nullptr)
    {
        IndependentVariableM* tmp = _temporaryIndependentVariable;
        settemporaryIndependentVariable(nullptr);
        delete tmp;
    }

    // Reset the model of the temporary dependant variable
    if (_temporaryDependentVariable != nullptr)
    {
        DependentVariableM* tmp = _temporaryDependentVariable;
        settemporaryDependentVariable(nullptr);
        delete tmp;
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
    if ((task != nullptr) && (_currentExperimentation != nullptr) && (AssessmentsModelManager::instance() != nullptr))
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
 * @brief Create a new independent variable from the Independent Variable currently edited (stored in _temporaryIndependentVariable)
 */
void TasksController::createNewIndependentVariableFromTemporary()
{
    if ((_temporaryIndependentVariable != nullptr) && (!_temporaryIndependentVariable->name().isEmpty()) && (_selectedTask != nullptr))
    {
        // Create and insert the new independent variable
        IndependentVariableM* independentVariable = _insertIndependentVariableIntoDB(_selectedTask->getExperimentationCassUuid(), _selectedTask->getCassUuid(), _temporaryIndependentVariable->name(),
                                                                                     _temporaryIndependentVariable->description(), _temporaryIndependentVariable->valueType(), _temporaryIndependentVariable->enumValues());
        if (independentVariable != nullptr)
        {
            // Add the independent variable to the selected task
            _selectedTask->addIndependentVariable(independentVariable);
        }
    }
}


/**
 * @brief Save the modifications of the Independent Variable currently edited (stored in _temporaryIndependentVariable)
 * @param independentVariableCurrentlyEdited
 */
void TasksController::saveModificationsOfIndependentVariableFromTemporary(IndependentVariableM* independentVariableCurrentlyEdited)
{
    if ((independentVariableCurrentlyEdited != nullptr) && (_temporaryIndependentVariable != nullptr) && (!_temporaryIndependentVariable->name().isEmpty())
            && (AssessmentsModelManager::instance() != nullptr))
    {
        independentVariableCurrentlyEdited->setname(_temporaryIndependentVariable->name());
        independentVariableCurrentlyEdited->setdescription(_temporaryIndependentVariable->description());
        independentVariableCurrentlyEdited->setvalueType(_temporaryIndependentVariable->valueType());
        independentVariableCurrentlyEdited->setenumValues(_temporaryIndependentVariable->enumValues());

        AssessmentsModelManager::update(*independentVariableCurrentlyEdited);
    }
}


/**
 * @brief Delete an independent variable
 * @param independentVariable
 */
void TasksController::deleteIndependentVariable(IndependentVariableM* independentVariable)
{
    if ((independentVariable != nullptr) && (_selectedTask != nullptr) && (AssessmentsModelManager::instance() != nullptr))
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
 * @brief Create a new dependent variable from the Dependent Variable currently edited (stored in _temporaryDependentVariable)
 */
void TasksController::createNewDependentVariableFromTemporary()
{
    if ((_temporaryDependentVariable != nullptr) && (!_temporaryDependentVariable->name().isEmpty()) && (!_temporaryDependentVariable->outputName().isEmpty())
            && (_selectedTask != nullptr) && (AssessmentsModelManager::instance() != nullptr))
    {
        // Create and insert the new dependent variable
        DependentVariableM* dependentVariable = _insertDependentVariableIntoDB(_selectedTask->getExperimentationCassUuid(),
                                                                               _selectedTask->getCassUuid(),
                                                                               _temporaryDependentVariable->name(),
                                                                               _temporaryDependentVariable->description(),
                                                                               _temporaryDependentVariable->agentName(),
                                                                               _temporaryDependentVariable->outputName());

        if (dependentVariable != nullptr)
        {
            // Add the dependent variable to the selected task
            _selectedTask->addDependentVariable(dependentVariable);
        }
    }
}


/**
 * @brief Save the modifications of the Dependent Variable currently edited (stored in _temporaryDependentVariable)
 * @param independentVariableCurrentlyEdited
 */
void TasksController::saveModificationsOfDependentVariableFromTemporary(DependentVariableM* dependentVariableCurrentlyEdited)
{
    if ((dependentVariableCurrentlyEdited != nullptr) && (_temporaryDependentVariable != nullptr) && (!_temporaryDependentVariable->name().isEmpty()))
    {
        dependentVariableCurrentlyEdited->setname(_temporaryDependentVariable->name());
        dependentVariableCurrentlyEdited->setdescription(_temporaryDependentVariable->description());
        dependentVariableCurrentlyEdited->setagentName(_temporaryDependentVariable->agentName());
        dependentVariableCurrentlyEdited->setoutputName(_temporaryDependentVariable->outputName());

        AssessmentsModelManager::update(*dependentVariableCurrentlyEdited);
    }
}



/**
 * @brief Delete a dependent variable
 * @param dependentVariable
 */
void TasksController::deleteDependentVariable(DependentVariableM* dependentVariable)
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
}


/**
 * @brief Create a new protocol with an IngeScape platform file URL
 * @param protocolName
 * @param platformFileUrl
 * @return
 */
TaskM* TasksController::_createNewProtocolWithIngeScapePlatformFileUrl(QString protocolName, QUrl platformFileUrl)
{
    TaskM* task = nullptr;

    if (!protocolName.isEmpty() && platformFileUrl.isValid() && (_currentExperimentation != nullptr) && (AssessmentsModelManager::instance() != nullptr))
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

    if (!variableName.isEmpty() && (AssessmentsModelManager::instance() != nullptr))
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

/**
 * @brief Initialize the temporary independent variable with the given independent variable
 * @param baseVariable, if null, init empty temporary independent variable
 */
void TasksController::initTemporaryIndependentVariable(IndependentVariableM* baseVariable)
{
    if (_temporaryIndependentVariable != nullptr)
    {
        if (baseVariable != nullptr)
        {
            _temporaryIndependentVariable->setname(baseVariable->name());
            _temporaryIndependentVariable->setdescription(baseVariable->description());
            _temporaryIndependentVariable->setvalueType(baseVariable->valueType());
            _temporaryIndependentVariable->setenumValues(baseVariable->enumValues());
        }
        else {
            _temporaryIndependentVariable->setname("");
            _temporaryIndependentVariable->setdescription("");
            _temporaryIndependentVariable->setvalueType(IndependentVariableValueTypes::UNKNOWN);
            _temporaryIndependentVariable->setenumValues({});
        }
    }
}


/**
 * @brief Initialize the temporary dependent variable with the given dependent variable
 * @param baseVariable, if null, init empty temporary dependent variable
 */
void TasksController::initTemporaryDependentVariable(DependentVariableM* baseVariable)
{
    if (_temporaryDependentVariable != nullptr)
    {
        if (baseVariable != nullptr)
        {
            qDebug() << "ON INIT " << baseVariable->name();
            _temporaryDependentVariable->setname(baseVariable->name());
            _temporaryDependentVariable->setdescription(baseVariable->description());
            _temporaryDependentVariable->setagentName(baseVariable->agentName());
            _temporaryDependentVariable->setoutputName(baseVariable->outputName());
        }
        else {
            qDebug() << "ON INIT IS EMPTY";
            _temporaryDependentVariable->setname("");
            _temporaryDependentVariable->setdescription("");
            _temporaryDependentVariable->setagentName("");
            _temporaryDependentVariable->setoutputName("");
        }
    }
}
