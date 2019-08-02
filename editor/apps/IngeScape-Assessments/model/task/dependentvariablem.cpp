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
 * @brief Update the given DependentVariableM into the Cassandra DB
 * @param entry
 * @return
 */
bool DependentVariableM::updateDependentVariableIntoCassandraDB(const DependentVariableM& entry)
{
    const char* query = "UPDATE ingescape.dependent_var SET name = ?, description = ?, agent_name = ?, output_name = ? WHERE id_experimentation = ? AND id_task = ? AND id = ?;";
    CassStatement* cassStatement = cass_statement_new(query, 7);
    cass_statement_bind_string(cassStatement, 0, entry.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 1, entry.description().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 2, entry.agentName().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 3, entry.outputName().toStdString().c_str());
    cass_statement_bind_uuid  (cassStatement, 4, entry.getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 5, entry.getTaskCassUuid());
    cass_statement_bind_uuid  (cassStatement, 6, entry.getCassUuid());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);

    return cassError == CASS_OK;
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

/**
 * @brief Delete the given dependent variable from the Cassandra DB
 * @param row
 * @return
 */
void DependentVariableM::deleteDependentVariableFromCassandraDB(const DependentVariableM& entry)
{
    // Remove dependent_var from DB
    const char* query = "DELETE FROM ingescape.dependent_var WHERE id_experimentation = ? AND id_task = ? AND id = ?;";
    CassStatement* cassStatement = cass_statement_new(query, 3);
    cass_statement_bind_uuid(cassStatement, 0, entry.getExperimentationCassUuid());
    cass_statement_bind_uuid(cassStatement, 1, entry.getTaskCassUuid());
    cass_statement_bind_uuid(cassStatement, 2, entry.getCassUuid());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "Dependent variable" << entry.name() << "has been successfully deleted from the DB";
    }
    else {
        qCritical() << "Could not delete the dependent variable" << entry.name() << "from the DB:" << cass_error_desc(cassError);
    }

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);
}

/**
 * @brief Create a clone of the current object.
 * Return nullptr in case of failure.
 * The caller is in charge of freeing the returned instance (if any).
 * @return
 */
DependentVariableM* DependentVariableM::clone() const
{
    return new DependentVariableM(CassUuid(), CassUuid(), CassUuid(), _name, _description, _agentName, _outputName);
}
