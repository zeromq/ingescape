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

#include "experimentationcontroller.h"

#include <ctime>

#include <controller/assessmentsmodelmanager.h>
#include "model/subject/characteristicvaluem.h"
#include "model/task/independentvariablevaluem.h"

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
ExperimentationController::ExperimentationController(JsonHelper* jsonHelper,
                                                     QObject *parent) : QObject(parent),
    _taskInstanceC(nullptr),
    _currentExperimentation(nullptr),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentation Controller";

    // Create the controller to manage a record of the current experimentation
    _taskInstanceC = new TaskInstanceController(_jsonHelper, this);
}


/**
 * @brief Destructor
 */
ExperimentationController::~ExperimentationController()
{
    qInfo() << "Delete Experimentation Controller";

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr)
    {
        setcurrentExperimentation(nullptr);
    }

    if (_taskInstanceC != nullptr)
    {
        disconnect(_taskInstanceC);

        TaskInstanceController* temp = _taskInstanceC;
        settaskInstanceC(nullptr);
        delete temp;
        temp = nullptr;
    }

    // Reset pointers
    _jsonHelper = nullptr;
}


/**
 * @brief Setter for property "Current Experimentation"
 * @param value
 */
void ExperimentationController::setcurrentExperimentation(ExperimentationM *value)
{
    if (_currentExperimentation != value)
    {
        if (_currentExperimentation != nullptr)
        {
            // Unload any previous experimentation
            _currentExperimentation->clearData();
        }

        _currentExperimentation = value;

        // Manage changes
        _onCurrentExperimentationChanged(_currentExperimentation);

        Q_EMIT currentExperimentationChanged(value);
    }
}


/**
 * @brief Create a new session for a subject and a protocol
 * @param subject
 * @param protocol
 */
void ExperimentationController::createNewSessionForSubjectAndProtocol(SubjectM* subject, TaskM* protocol)
{
    if ((subject != nullptr) && (protocol != nullptr) && (_currentExperimentation != nullptr))
    {
        QString sessionName = QString("%1 - %2 - %3").arg(protocol->name(), subject->displayedId(), QDate::currentDate().toString("dd/MM/yy"));

        qInfo() << "Create new session" << sessionName << "for subject" << subject->displayedId() << "and protocol" << protocol->name();

        // Create a new (experimentation) task instance
        TaskInstanceM* session = _insertTaskInstanceIntoDB(sessionName, subject, protocol);

        if (session != nullptr)
        {
            // Add the task insatnce to the current experimentation
            _currentExperimentation->addTaskInstance(session);

            // Open this new task instance
            openTaskInstance(session);
        }
    }
}


/**
 * @brief Open a task instance
 * @param record
 */
void ExperimentationController::openTaskInstance(TaskInstanceM* taskInstance)
{
    if ((taskInstance != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Open the record" << taskInstance->name() << "of the experimentation" << _currentExperimentation->name();

        // Update the current task instance
        _taskInstanceC->setcurrentTaskInstance(taskInstance);
    }
}


/**
 * @brief Delete a task instance
 * @param record
 */
void ExperimentationController::deleteTaskInstance(TaskInstanceM* taskInstance)
{
    if ((taskInstance != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Delete the record" << taskInstance->name() << "of the experimentation" << _currentExperimentation->name();

        // Delete task instance from DB
        TaskInstanceM::deleteTaskInstanceFromCassandra(*taskInstance);

        // Remove the task instance from the current experimentation
        _currentExperimentation->removeTaskInstance(taskInstance);

        // Free memory
        delete taskInstance;
    }
}


/**
 * @brief Slot called when the current experimentation changed
 * @param currentExperimentation
 */
void ExperimentationController::_onCurrentExperimentationChanged(ExperimentationM* currentExperimentation)
{
    if (currentExperimentation != nullptr && AssessmentsModelManager::Instance() != nullptr)
    {
        qDebug() << "_on Current Experimentation Changed" << currentExperimentation->name();

        _retrieveTasksForExperimentation(currentExperimentation);

        _retrieveSubjectsForExperimentation(currentExperimentation);

        _retrieveCharacteristicsForExperimentation(currentExperimentation);

        _retrieveCharacteristicValuesForSubjectsInExperimentation(currentExperimentation);

        _retrieveTaskInstancesForExperimentation(currentExperimentation);

        _retrieveIndependentVariableValuesForTaskInstancesInExperimentation(currentExperimentation);
    }
}

/**
 * @brief Create and insert a new task instance into the DB.
 * A nullptr is returned if the task instance could not be created
 * @param recordName
 * @param subject
 * @param task
 * @return
 */
TaskInstanceM* ExperimentationController::_insertTaskInstanceIntoDB(const QString& taskInstanceName, SubjectM* subject, TaskM* task)
{
    TaskInstanceM* taskInstance = nullptr;

    if ((_currentExperimentation != nullptr) && (subject != nullptr) && (task != nullptr))
    {
        // Create the new task instance
        taskInstance = new TaskInstanceM(_currentExperimentation->getCassUuid(), AssessmentsModelManager::genCassUuid(), taskInstanceName, "", subject->getCassUuid(), task->getCassUuid(), QDateTime::currentDateTime());
        if (taskInstance != nullptr)
        {
            taskInstance->settask(task);
            taskInstance->setsubject(subject);

            if (AssessmentsModelManager::insert(*taskInstance))
            {
                for (auto indepVarIterator = task->independentVariables()->begin() ; indepVarIterator != task->independentVariables()->end() ; ++indepVarIterator)
                {
                    IndependentVariableM* independentVar = *indepVarIterator;
                    if (independentVar != nullptr)
                    {
                        IndependentVariableValueM indepVarValue(subject->getExperimentationCassUuid(), taskInstance->getCassUuid(), independentVar->getCassUuid(), "");
                        AssessmentsModelManager::insert(indepVarValue);
                    }
                }
            }
        }
    }

    return taskInstance;
}

/**
 * @brief Retrieve all independent variables from the Cassandra DB for the given task.
 * The task will be updated by this method
 * @param task
 */
void ExperimentationController::_retrieveIndependentVariableForTask(TaskM* task)
{
    if (AssessmentsModelManager::Instance() != nullptr)
    {
        QList<IndependentVariableM*> indepVarList = AssessmentsModelManager::select<IndependentVariableM>({ task->getExperimentationCassUuid(), task->getCassUuid() });
        for (IndependentVariableM* independentVariable : indepVarList) {
            task->addIndependentVariable(independentVariable);
        }
    }
}

/**
 * @brief Retrieve all dependent variables from the Cassandra DB for the given task.
 * The task will be updated by this method.
 * @param task
 */
void ExperimentationController::_retrieveDependentVariableForTask(TaskM* task)
{
    if ((task != nullptr) && (AssessmentsModelManager::Instance() != nullptr))
    {
        QList<DependentVariableM*> depVarList = AssessmentsModelManager::select<DependentVariableM>({ task->getExperimentationCassUuid(), task->getCassUuid() });
        for (DependentVariableM* dependentVariable : depVarList) {
            task->addDependentVariable(dependentVariable);
        }
    }
}


/**
 * @brief Retrieve all subjects from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveSubjectsForExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<SubjectM*> subejctList = AssessmentsModelManager::select<SubjectM>({ experimentation->getCassUuid() });
        for (SubjectM* subject : subejctList) {
            experimentation->addSubject(subject);
        }
    }
}


/**
 * @brief Retrieve all characteristics from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveCharacteristicsForExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<CharacteristicM*> characteristicList = AssessmentsModelManager::select<CharacteristicM>({ experimentation->getCassUuid() });
        for (CharacteristicM* characteristic : characteristicList) {
            experimentation->addCharacteristic(characteristic);
        }
    }
}


/**
 * @brief Retrieve all tasks from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveTasksForExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<TaskM*> taskList = AssessmentsModelManager::select<TaskM>({ experimentation->getCassUuid() });
        for (TaskM* task : taskList) {
            // Independent variables
            _retrieveIndependentVariableForTask(task);

            // Dependent variables
            _retrieveDependentVariableForTask(task);

            // Add the task to the experimentation
            experimentation->addTask(task);
        }
    }
}

/**
 * @brief Retrieve all task instances from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveTaskInstancesForExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<TaskInstanceM*> taskInstanceList = AssessmentsModelManager::select<TaskInstanceM>({ experimentation->getCassUuid() });
        for (TaskInstanceM* taskInstance : taskInstanceList) {
            experimentation->addTaskInstance(taskInstance);

            // Set pointers to Task & Subject
            taskInstance->settask(experimentation->getTaskFromUID(taskInstance->getTaskCassUuid()));
            taskInstance->setsubject(experimentation->getSubjectFromUID(taskInstance->getSubjectCassUuid()));
        }
    }
}

/**
 * @brief Retrieve all characteristic values from the Cassandra DB for each subjects in the given experimentation
 * The subjects will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveCharacteristicValuesForSubjectsInExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<CharacteristicValueM*> characValueList = AssessmentsModelManager::select<CharacteristicValueM>({experimentation->getCassUuid()});
        for (CharacteristicValueM* characValue : characValueList)
        {
            if (characValue != nullptr)
            {
                SubjectM* subject = experimentation->getSubjectFromUID(characValue->subjectUuid);
                CharacteristicM* characteristic = experimentation->getCharacteristicFromUID(characValue->characteristicUuid);

                // Get characteristic value type
                if ((subject != nullptr) && (characteristic != nullptr))
                {
                    switch (characteristic->valueType()) {
                        case CharacteristicValueTypes::INTEGER:
                            subject->setCharacteristicValue(characteristic, characValue->valueString.toInt());
                            break;
                        case CharacteristicValueTypes::DOUBLE:
                            subject->setCharacteristicValue(characteristic, characValue->valueString.toDouble());
                            break;
                        case CharacteristicValueTypes::TEXT:
                            subject->setCharacteristicValue(characteristic, characValue->valueString);
                            break;
                        case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
                            subject->setCharacteristicValue(characteristic, characValue->valueString);
                            break;
                        default: // UNKNOWN
                            break;
                    }
                }
            }
        }
        qDeleteAll(characValueList);
        characValueList.clear();
    }
}

/**
 * @brief Retrieve all independent variable values Cassandra DB for each task instance in the given experimentation.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveIndependentVariableValuesForTaskInstancesInExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<IndependentVariableValueM*> indepVarValueList = AssessmentsModelManager::select<IndependentVariableValueM>({ experimentation->getCassUuid() });
        for (IndependentVariableValueM* indepVarValue : indepVarValueList)
        {
            if (indepVarValue != nullptr)
            {
                TaskInstanceM* taskInstance = experimentation->getTaskInstanceFromUID(indepVarValue->taskInstanceUuid);
                IndependentVariableM* indepVar = taskInstance->task()->getIndependentVariableFromUuid(indepVarValue->independentVariableUuid);
                if ((taskInstance != nullptr) && (indepVar != nullptr))
                {
                    taskInstance->setIndependentVariableValue(indepVar, indepVarValue->valueString);
                }
            }
        }
        qDeleteAll(indepVarValueList);
        indepVarValueList.clear();
    }
}
