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
 * @brief Dependent variable table name
 */
const QString DependentVariableM::table = "ingescape.dependent_var";

/**
 * @brief Dependent variable table column names
 */
const QStringList DependentVariableM::columnNames = {
    "id_experimentation",
    "id_task",
    "id",
    "agent_name",
    "description",
    "name",
    "output_name"
};

/**
 * @brief Dependent variable table primary keys IN ORDER
 */
const QStringList DependentVariableM::primaryKeys = {
    "id_experimentation",
    "id_task",
    "id",
};


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
 * @brief Update the given Dependent variable the Cassandra DB
 * @param entry
 * @return
 */
bool DependentVariableM::updateDependentVariableIntoCassandraDB(const DependentVariableM& entry)
{
    QString queryStr = "UPDATE " + DependentVariableM::table + " SET name = ?, description = ?, agent_name = ?, output_name = ? WHERE id_experimentation = ? AND id_task = ? AND id = ?;";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 7);
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
DependentVariableM* DependentVariableM::createFromCassandraRow(const CassRow* row)
{
    DependentVariableM* dependentVariable = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, taskUuid, independentVarUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &taskUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &independentVarUuid);

        QString variableName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QString variableDescription(AssessmentsModelManager::getStringValueFromColumnName(row, "description"));
        QString agentName(AssessmentsModelManager::getStringValueFromColumnName(row, "agent_name"));
        QString outputName(AssessmentsModelManager::getStringValueFromColumnName(row, "output_name"));

        dependentVariable = new DependentVariableM(experimentationUuid, taskUuid, independentVarUuid, variableName, variableDescription, agentName, outputName);
    }

    return dependentVariable;
}

/**
 * @brief Delete the given dependent variable from Cassandra DB
 * @param row
 * @return
 */
void DependentVariableM::deleteDependentVariableFromCassandraDB(const DependentVariableM& entry)
{
    // Remove dependent_var from DB
    AssessmentsModelManager::deleteEntry<DependentVariableM>({ entry.getExperimentationCassUuid(), entry.getTaskCassUuid(), entry.getCassUuid() });
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
