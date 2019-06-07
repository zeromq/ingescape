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

#ifndef TASKSCONTROLLER_H
#define TASKSCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <controller/assessmentsmodelmanager.h>
#include <model/jsonhelper.h>
#include <model/experimentationm.h>


/**
 * @brief The TasksController class defines the controller to manage the tasks of the current experimentation
 */
class TasksController : public QObject
{
    Q_OBJECT

    // List of all types for independent variable value
    I2_ENUM_LISTMODEL(IndependentVariableValueTypes, allIndependentVariableValueTypes)

    // Model of the current experimentation
    I2_QML_PROPERTY_READONLY(ExperimentationM*, currentExperimentation)

    // Model of the selected task
    I2_QML_PROPERTY(TaskM*, selectedTask)


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param jsonHelper
     * @param parent
     */
    explicit TasksController(//AssessmentsModelManager* modelManager,
                             //JsonHelper* jsonHelper,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TasksController();


    /**
     * @brief Return true if the user can create a task with the name
     * Check if the name is not empty and if a task with the same name does not already exist
     * @param taskName
     * @return
     */
    Q_INVOKABLE bool canCreateTaskWithName(QString taskName);


    /**
     * @brief Create a new task with an IngeScape platform file path
     * @param taskName
     * @param platformFilePath
     */
    Q_INVOKABLE void createNewTaskWithIngeScapePlatformFilePath(QString taskName, QString platformFilePath);


    /**
     * @brief Delete a task
     * @param task
     */
    Q_INVOKABLE void deleteTask(TaskM* task);


    /**
     * @brief Duplicate a task
     * @param task
     */
    Q_INVOKABLE void duplicateTask(TaskM* task);


    /**
     * @brief Return true if the user can create an independent variable with the name
     * Check if the name is not empty and if a independent variable with the same name does not already exist
     * @param independentVariableName
     * @return
     */
    Q_INVOKABLE bool canCreateIndependentVariableWithName(QString independentVariableName);


    /**
     * @brief Return true if the user can edit an independent variable with the name
     * Check if the name is not empty and if a independent variable with the same name does not already exist
     * @param independentVariableCurrentlyEdited
     * @param independentVariableName
     * @return
     */
    Q_INVOKABLE bool canEditIndependentVariableWithName(IndependentVariableM* independentVariableCurrentlyEdited, QString independentVariableName);


    /**
     * @brief Create a new independent variable
     * @param independentVariableName
     * @param independentVariableDescription
     * @param nIndependentVariableValueType
     */
    Q_INVOKABLE void createNewIndependentVariable(QString independentVariableName,
                                                  QString independentVariableDescription,
                                                  int nIndependentVariableValueType);


    /**
     * @brief Create a new independent variable of type enum
     * @param independentVariableName
     * @param independentVariableDescription
     * @param enumValues
     */
    Q_INVOKABLE void createNewIndependentVariableEnum(QString independentVariableName,
                                                      QString independentVariableDescription,
                                                      QStringList enumValues);


    /**
     * @brief Save the modifications of the Independent Variable currently edited
     * @param independentVariableCurrentlyEdited
     * @param independentVariableName
     * @param independentVariableDescription
     * @param nIndependentVariableValueType
     */
    Q_INVOKABLE void saveModificationsOfIndependentVariable(IndependentVariableM* independentVariableCurrentlyEdited,
                                                            QString independentVariableName,
                                                            QString independentVariableDescription,
                                                            int nIndependentVariableValueType);


    /**
     * @brief Save the modifications of the Independent Variable (of type enum) currently edited
     * @param independentVariableCurrentlyEdited
     * @param independentVariableName
     * @param independentVariableDescription
     * @param enumValues
     */
    Q_INVOKABLE void saveModificationsOfIndependentVariableEnum(IndependentVariableM* independentVariableCurrentlyEdited,
                                                                QString independentVariableName,
                                                                QString independentVariableDescription,
                                                                QStringList enumValues);


    /**
     * @brief Delete an independent variable
     * @param independentVariable
     */
    Q_INVOKABLE void deleteIndependentVariable(IndependentVariableM* independentVariable);


    /**
     * @brief Create a new dependent variable
     */
    Q_INVOKABLE void createNewDependentVariable();


    /**
     * @brief Delete an dependent variable
     * @param dependentVariable
     */
    Q_INVOKABLE void deleteDependentVariable(DependentVariableM* dependentVariable);



Q_SIGNALS:


public Q_SLOTS:


private:

    /**
     * @brief Create a new task with an IngeScape platform file URL
     * @param taskName
     * @param platformFileUrl
     * @return
     */
    TaskM* _createNewTaskWithIngeScapePlatformFileUrl(QString taskName, QUrl platformFileUrl);


private:

    // Manager for the data model of our IngeScape Assessments application
    //AssessmentsModelManager* _modelManager;

    // Helper to manage JSON files
    //JsonHelper* _jsonHelper;

};

QML_DECLARE_TYPE(TasksController)

#endif // TASKSCONTROLLER_H
