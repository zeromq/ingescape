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

#include <model/experimentationm.h>
#include <model/task/independentvariablem.h>


/**
 * @brief The TasksController class defines the controller to manage the tasks of the current experimentation
 */
class TasksController : public QObject
{
    Q_OBJECT

    // List of all types for independent variable value (without "UNKNOWN")
    I2_ENUM_LISTMODEL(IndependentVariableValueTypes, allIndependentVariableValueTypes)

    // List of types for independent variable value without "ENUM" (and without "UNKNOWN")
    I2_ENUM_LISTMODEL(IndependentVariableValueTypes, independentVariableValueTypesWithoutEnum)

    // Model of the current experimentation
    I2_QML_PROPERTY_READONLY(ExperimentationM*, currentExperimentation)

    // Model of the selected task
    I2_QML_PROPERTY_DELETE_PROOF(TaskM*, selectedTask)

    // Temporary independent variable used for edition rollbacks
    I2_QML_PROPERTY(IndependentVariableM*, temporaryIndependentVariable)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit TasksController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TasksController();


    /**
     * @brief Return true if the user can create a protocol with the name
     * Check if the name is not empty and if a protocol with the same name does not already exist
     * @param protocolName
     * @return
     */
    Q_INVOKABLE bool canCreateProtocolWithName(QString protocolName);


    /**
     * @brief Create a new protocol with an IngeScape platform file path
     * @param protocolName
     * @param platformFilePath
     */
    Q_INVOKABLE void createNewProtocolWithIngeScapePlatformFilePath(QString protocolName, QString platformFilePath);


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
     * Check if the name is not empty and if an independent variable with the same name does not already exist
     * @param independentVariableName
     * @return
     */
    Q_INVOKABLE bool canCreateIndependentVariableWithName(QString independentVariableName);


    /**
     * @brief Return true if the user can create a dependent variable with the name
     * Check if the name is not empty and if a dependent variable with the same name does not already exist
     * @param dependentVariableName
     * @return
     */
    Q_INVOKABLE bool canCreateDependentVariableWithName(QString dependentVariableName);


    /**
     * @brief Return true if the user can edit an independent variable with the name
     * Check if the name is not empty and if a independent variable with the same name does not already exist
     * @param independentVariableCurrentlyEdited
     * @param independentVariableName
     * @return
     */
    Q_INVOKABLE bool canEditIndependentVariableWithName(IndependentVariableM* independentVariableCurrentlyEdited, QString independentVariableName);


    /**
     * @brief Create a new independent variable from the Independent Variable currently edited (stored in _temporaryIndependentVariable)
     */
    Q_INVOKABLE void createNewIndependentVariableFromTemporary();


    /**
     * @brief Save the modifications of the Independent Variable currently edited (stored in _temporaryIndependentVariable)
     * @param independentVariableCurrentlyEdited
     */
    Q_INVOKABLE void saveModificationsOfIndependentVariableFromTemporary(IndependentVariableM* independentVariableCurrentlyEdited);


    /**
     * @brief Delete an independent variable
     * @param independentVariable
     */
    Q_INVOKABLE void deleteIndependentVariable(IndependentVariableM* independentVariable);


    /**
     * @brief Create a new dependent variable
     * @param dependentVariableName
     * @param dependentVariableDescription
     * @param agentName
     * @param outputName
     */
    Q_INVOKABLE void createNewDependentVariable(QString dependentVariableName,
                                                QString dependentVariableDescription,
                                                QString agentName,
                                                QString outputName);


    /**
     * @brief Delete an dependent variable
     * @param dependentVariable
     */
    // FIXME Unused
    //Q_INVOKABLE void deleteDependentVariable(DependentVariableM* dependentVariable);


    /**
     * @brief Initialize the temporary independent variable with the given independent variable
     * @param baseVariable
     */
    Q_INVOKABLE void initTemporaryIndependentVariable(IndependentVariableM* baseVariable);

private:

    /**
     * @brief Create a new protocol with an IngeScape platform file URL
     * @param protocolName
     * @param platformFileUrl
     * @return
     */
    TaskM* _createNewProtocolWithIngeScapePlatformFileUrl(QString protocolName, QUrl platformFileUrl);


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
    IndependentVariableM* _insertIndependentVariableIntoDB(CassUuid experimentationUuid, CassUuid taskUuid, const QString& name, const QString& description, IndependentVariableValueTypes::Value valueType, const QStringList& enumValues = {});


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
    DependentVariableM* _insertDependentVariableIntoDB(CassUuid experimentationUuid, CassUuid taskUuid, const QString& name, const QString& description, const QString& agentName, const QString& outputName);
};

QML_DECLARE_TYPE(TasksController)

#endif // TASKSCONTROLLER_H
