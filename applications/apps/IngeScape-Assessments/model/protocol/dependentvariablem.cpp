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
                                       , CassUuid protocolUuid
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
    , _protocolCassUuid(protocolUuid)
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
 * @brief Static factory method to create an dependent variable from a CassandraDB record
 * @param row
 * @return
 */
DependentVariableM* DependentVariableM::createFromCassandraRow(const CassRow* row)
{
    DependentVariableM* dependentVariable = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, protocolUuid, independentVarUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &protocolUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &independentVarUuid);

        QString variableName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QString variableDescription(AssessmentsModelManager::getStringValueFromColumnName(row, "description"));
        QString agentName(AssessmentsModelManager::getStringValueFromColumnName(row, "agent_name"));
        QString outputName(AssessmentsModelManager::getStringValueFromColumnName(row, "output_name"));

        dependentVariable = new DependentVariableM(experimentationUuid,
                                                   protocolUuid,
                                                   independentVarUuid,
                                                   variableName,
                                                   variableDescription,
                                                   agentName,
                                                   outputName);
    }

    return dependentVariable;
}

/**
 * @brief Create a CassStatement to insert an DependentVariableM into the DB.
 * The statement contains the values from the given dependentVariable.
 * Passed dependentVariable must have a valid and unique UUID.
 * @param dependentVariable
 * @return
 */
CassStatement* DependentVariableM::createBoundInsertStatement(const DependentVariableM& dependentVariable)
{
    QString queryStr = "INSERT INTO " + DependentVariableM::table + " (id_experimentation, id_task, id, name, description, agent_name, output_name) VALUES (?, ?, ?, ?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 7);
    cass_statement_bind_uuid  (cassStatement, 0, dependentVariable.getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 1, dependentVariable.getProtocolCassUuid());
    cass_statement_bind_uuid  (cassStatement, 2, dependentVariable.getCassUuid());
    cass_statement_bind_string(cassStatement, 3, dependentVariable.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 4, dependentVariable.description().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 5, dependentVariable.agentName().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 6, dependentVariable.outputName().toStdString().c_str());
    return cassStatement;
}

/**
 * @brief Create a CassStatement to update a DependentVariableM into the DB.
 * The statement contains the values from the given dependentVariable.
 * Passed dependentVariable must have a valid and unique UUID.
 * @param dependentVariable
 * @return
 */
CassStatement* DependentVariableM::createBoundUpdateStatement(const DependentVariableM& dependentVariable)
{
    QString queryStr = "UPDATE " + DependentVariableM::table + " SET name = ?, description = ?, agent_name = ?, output_name = ? WHERE id_experimentation = ? AND id_task = ? AND id = ?;";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 7);
    cass_statement_bind_string(cassStatement, 0, dependentVariable.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 1, dependentVariable.description().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 2, dependentVariable.agentName().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 3, dependentVariable.outputName().toStdString().c_str());
    cass_statement_bind_uuid  (cassStatement, 4, dependentVariable.getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 5, dependentVariable.getProtocolCassUuid());
    cass_statement_bind_uuid  (cassStatement, 6, dependentVariable.getCassUuid());
    return cassStatement;
}
