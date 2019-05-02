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

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
TasksController::TasksController(AssessmentsModelManager* modelManager,
                                 JsonHelper* jsonHelper,
                                 QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr),
    _selectedTask(nullptr),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Tasks Controller";

    // Fill without type "UNKNOWN"
    _allIndependentVariableValueTypes.fillWithAllEnumValues();
    _allIndependentVariableValueTypes.removeEnumValue(IndependentVariableValueTypes::UNKNOWN);

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

    // Reset pointers
    _modelManager = nullptr;
    _jsonHelper = nullptr;
}


/**
 * @brief Setter for property "Selected Task"
 * @param value
 */
void TasksController::setselectedTask(TaskM *value)
{
    if (_selectedTask != value)
    {
        // Previous selected task was defined
        if (_selectedTask != nullptr)
        {
            // Clear the list of agents of the unselected task
            _clearAgentsOfUnselectedTask(_selectedTask);
        }

        // Update the selected task
        _selectedTask = value;

        // New selected task is defined
        if (_selectedTask != nullptr)
        {
            // Fill the list of agents of the selected task
            _fillAgentsOfSelectedTask(_selectedTask);
        }

        Q_EMIT selectedTaskChanged(value);
    }
}


/**
 * @brief Return true if the user can create a task with the name
 * Check if the name is not empty and if a task with the same name does not already exist
 * @param taskName
 * @return
 */
bool TasksController::canCreateTaskWithName(QString taskName)
{
    if (!taskName.isEmpty() && (_currentExperimentation != nullptr))
    {
        for (TaskM* task : _currentExperimentation->allTasks()->toList())
        {
            if ((task != nullptr) && (task->name() == taskName))
            {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
}


/**
 * @brief Create a new task with an IngeScape platform file
 * @param taskName
 * @param platformFilePath
 */
void TasksController::createNewTaskWithIngeScapePlatformFile(QString taskName, QString platformFilePath)
{
    if (!taskName.isEmpty() && !platformFilePath.isEmpty() && (_currentExperimentation != nullptr))
    {
        qInfo() << "Create new task" << taskName << "with file" << platformFilePath;

        QUrl platformFileUrl = QUrl(platformFilePath);

        if (platformFileUrl.isValid())
        {
            // Create the new task
            TaskM* task = new TaskM(taskName);

            // Set the URL of the IngeScape platform file (JSON)
            task->setplatformFileUrl(platformFileUrl);

            // Add the task to the current experimentation
            _currentExperimentation->addTask(task);

            // Select this new task
            setselectedTask(task);
        }
        else {
            qWarning() << "Failed to create the task" << taskName << "because the URL" << platformFilePath << "is wrong !";
        }
    }
}


/**
 * @brief Delete a task
 * @param task
 */
void TasksController::deleteTask(TaskM* task)
{
    if ((task != nullptr) && (_currentExperimentation != nullptr))
    {
        if (task == _selectedTask) {
            setselectedTask(nullptr);
        }

        // Remove the task from the current experimentation
        _currentExperimentation->removeTask(task);

        // Free memory
        delete task;
    }
}


/**
 * @brief Return true if the user can create an independent variable with the name
 * Check if the name is not empty and if a independent variable with the same name does not already exist
 * @param independentVariableName
 * @return
 */
bool TasksController::canCreateIndependentVariableWithName(QString independentVariableName)
{
    if (!independentVariableName.isEmpty() && (_selectedTask != nullptr))
    {
        for (IndependentVariableM* independentVariable : _selectedTask->independentVariables()->toList())
        {
            if ((independentVariable != nullptr) && (independentVariable->name() == independentVariableName))
            {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
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
    if ((independentVariableCurrentlyEdited != nullptr) && !independentVariableName.isEmpty() && (_selectedTask != nullptr))
    {
        for (IndependentVariableM* independentVariable : _selectedTask->independentVariables()->toList())
        {
            if ((independentVariable != nullptr) && (independentVariable != independentVariableCurrentlyEdited) && (independentVariable->name() == independentVariableName))
            {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
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

        // Create the new independent variable
        IndependentVariableM* independentVariable = new IndependentVariableM(independentVariableName, independentVariableDescription, independentVariableValueType);

        // Add the independent variable to the selected task
        _selectedTask->addIndependentVariable(independentVariable);
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
        IndependentVariableM* independentVariable = new IndependentVariableM(independentVariableName, independentVariableDescription, IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM);
        independentVariable->setenumValues(enumValues);

        // Add the independent variable to the selected task
        _selectedTask->addIndependentVariable(independentVariable);
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
    if ((independentVariable != nullptr) && (_selectedTask != nullptr))
    {
        // Remove the independent variable from the selected task
        _selectedTask->removeIndependentVariable(independentVariable);

        // Free memory
        delete independentVariable;
    }
}


/**
 * @brief Create a new dependent variable
 */
void TasksController::createNewDependentVariable()
{
    if (_selectedTask != nullptr)
    {
        qDebug() << "Create a new dependent variable";

        DependentVariableM* dependentVariable = new DependentVariableM();

        dependentVariable->setname("VD");
        //dependentVariable->setdescription("");
        //dependentVariable->setagentName("");
        //dependentVariable->setoutputName("");

        // Add the dependent variable to the selected task
        _selectedTask->addDependentVariable(dependentVariable);
    }
}


/**
 * @brief Clear the list of agents of the unselected task
 * @param task
 */
void TasksController::_clearAgentsOfUnselectedTask(TaskM* task)
{
    if (task != nullptr)
    {
        // FIXME TODO _clearAgentsOfUnselectedTask
        qDebug() << "Clear the list of agents of the unselected task" << task->name();
    }
}


/**
 * @brief Fill the list of agents of the selected task
 * @param task
 */
void TasksController::_fillAgentsOfSelectedTask(TaskM* task)
{
    if ((task != nullptr) && (_modelManager != nullptr))
    {
        qDebug() << "Fill the list of agents of the selected task" << task->name();

        if (task->platformFileUrl().isValid())
        {
            QString platformFilePath = task->platformFileUrl().path();

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
                        // Version
                        QString versionJsonPlatform = "";
                        if (jsonRoot.contains("version"))
                        {
                            versionJsonPlatform = jsonRoot.value("version").toString();

                            qDebug() << "Version of JSON platform is" << versionJsonPlatform;
                        }
                        else {
                            qDebug() << "UNDEFINED version of JSON platform";
                        }

                        // Import the agents list from a json byte content
                        _modelManager->importAgentsListFromJson(jsonRoot.value("agents").toArray(), versionJsonPlatform);


                        //
                        // Update the list of agent names and the hash table from an agent name to the list of its outputs names
                        // in the platform of the (selected) task
                        //
                        QStringList agentNamesList;
                        QHash<QString, QStringList> hashFromAgentNameToOutputNamesList;

                        for (AgentsGroupedByNameVM* agentsGroupedByName : _modelManager->allAgentsGroupsByName()->toList())
                        {
                            if (agentsGroupedByName != nullptr)
                            {
                                agentNamesList.append(agentsGroupedByName->name());

                                QStringList outputNamesList;
                                for (OutputVM* output : agentsGroupedByName->outputsList()->toList())
                                {
                                    if (output != nullptr)
                                    {
                                        outputNamesList.append(output->name());
                                    }
                                }
                                hashFromAgentNameToOutputNamesList.insert(agentsGroupedByName->name(), outputNamesList);
                            }
                        }

                        // Update the list of agent names and the hash table from an agent name to the list of its outputs names
                        task->updateAgentNamesAndOutputNames(agentNamesList, hashFromAgentNameToOutputNamesList);

                        qDebug() << "Agents of the selected task:" << agentNamesList << "(" << hashFromAgentNameToOutputNamesList << ")";

                        // FIXME TODO: clean Models and VM of agents
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
        else {
            qWarning() << "The URL of platform" << task->platformFileUrl() << "is not valid";
        }
    }
}
