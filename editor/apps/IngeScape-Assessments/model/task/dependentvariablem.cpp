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

#include "dependentvariablem.h"

#include <controller/assessmentsmodelmanager.h>

/**
 * @brief Constructor
 * @param parent
 */
DependentVariableM::DependentVariableM(CassUuid experimentationUuid
                                       , CassUuid taskUuid
                                       , CassUuid cassUuid
                                       , const QString& name
                                       , const QString& description
                                       , const QString& agentName
                                       , const QString& outputName
                                       , QObject *parent)
    : QObject(parent)
    , _name(name)
    , _description(description)
    , _agentName(agentName)
    , _outputName(outputName)
    , _experimentationCassUuid(experimentationUuid)
    , _taskCassUuid(taskUuid)
    , _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Dependent Variable" << _name;
}


/**
 * @brief Destructor
 */
DependentVariableM::~DependentVariableM()
{
    qInfo() << "Delete Model of Dependent Variable" << _name;

}

/**
 * @brief Custom setter for the 'name' property that also update the DB entry
 * @param value
 */
void DependentVariableM::setname(QString value)
{
    if ((value != _name) && (AssessmentsModelManager::Instance() != nullptr))
    {
        CassError cassError = _updateDBEntry(value, "name");
        if (cassError != CASS_OK) {
            qCritical() << "Could not update dependent variable" << _name << "in the DB:" << cass_error_desc(cassError);
        } else {
            _name = value;
            Q_EMIT nameChanged(value);
        }
    }
}


/**
 * @brief Custom setter for the 'description' property that also update the DB entry
 * @param value
 */
void DependentVariableM::setdescription(QString value)
{
    if ((value != _name) && (AssessmentsModelManager::Instance() != nullptr))
    {
        CassError cassError = _updateDBEntry(value, "description");
        if (cassError != CASS_OK) {
            qCritical() << "Could not update dependent variable" << _name << "in the DB:" << cass_error_desc(cassError);
        } else {
            _description = value;
            Q_EMIT descriptionChanged(value);
        }
    }
}


/**
 * @brief Custom setter for the 'agentName' property that also update the DB entry
 * @param value
 */
void DependentVariableM::setagentName(QString value)
{
    if ((value != _name) && (AssessmentsModelManager::Instance() != nullptr))
    {
        CassError cassError = _updateDBEntry(value, "agent_name");
        if (cassError != CASS_OK) {
            qCritical() << "Could not update dependent variable" << _name << "in the DB:" << cass_error_desc(cassError);
        } else {
            _agentName = value;
            Q_EMIT agentNameChanged(value);
        }
    }
}


/**
 * @brief Custom setter for the 'outputName' property that also update the DB entry
 * @param value
 */
void DependentVariableM::setoutputName(QString value)
{
    if ((value != _name) && (AssessmentsModelManager::Instance() != nullptr))
    {
        CassError cassError = _updateDBEntry(value, "output_name");
        if (cassError != CASS_OK) {
            qCritical() << "Could not update dependent variable" << _name << "in the DB:" << cass_error_desc(cassError);
        } else {
            _outputName = value;
            Q_EMIT outputNameChanged(value);
        }
    }
}


/**
 * @brief Update the given field with the given value in the corresponding DB entry
 * @param value
 * @param dbField
 * @return
 */
CassError DependentVariableM::_updateDBEntry(const QString& value, const QString& dbField)
{
    QString query = QString("UPDATE ingescape.dependent_var SET %1 = ? WHERE id_experimentation = ? AND id_task = ? AND id = ?;").arg(dbField);
    CassStatement* cassStatement = cass_statement_new(query.toStdString().c_str(), 4);
    cass_statement_bind_string(cassStatement, 0, value.toStdString().c_str());
    cass_statement_bind_uuid  (cassStatement, 1, _experimentationCassUuid);
    cass_statement_bind_uuid  (cassStatement, 2, _taskCassUuid);
    cass_statement_bind_uuid  (cassStatement, 3, _cassUuid);
    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);

    return cassError;
}


/**
 * @brief Static factory method to create an dependent variable from a CassandraDB record
 * @param row
 * @return
 */
DependentVariableM* DependentVariableM::createDependentVariableFromCassandraRow(const CassRow* row)
{
    DependentVariableM* dependentVariable = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, taskUuid, independentVarUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &taskUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &independentVarUuid);

        const char *chrVariableName = "";
        size_t varNameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrVariableName, &varNameLength);
        QString variableName = QString::fromUtf8(chrVariableName, static_cast<int>(varNameLength));

        const char *chrVariableDescription = "";
        size_t varDescriptionLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "description"), &chrVariableDescription, &varDescriptionLength);
        QString variableDescription(QString::fromUtf8(chrVariableDescription, static_cast<int>(varDescriptionLength)));

        const char *chrAgentName = "";
        size_t agentNameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "agent_name"), &chrAgentName, &agentNameLength);
        QString agentName(QString::fromUtf8(chrAgentName, static_cast<int>(agentNameLength)));

        const char *chrOutputName = "";
        size_t outputNameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "output_name"), &chrOutputName, &outputNameLength);
        QString outputName(QString::fromUtf8(chrOutputName, static_cast<int>(outputNameLength)));

        dependentVariable = new DependentVariableM(experimentationUuid, taskUuid, independentVarUuid, variableName, variableDescription, agentName, outputName);
    }

    return dependentVariable;
}
