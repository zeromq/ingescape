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
 *      Mathieu Soum     <soum@ingenuity.io>
 *
 */

#include "independentvariablevaluem.h"

#include "controller/assessmentsmodelmanager.h"

/**
 * @brief Independent variable value table name
 */
const QString IndependentVariableValueM::table = "ingescape.independent_var_value";

/**
 * @brief Independent variable value table column names
 */
const QStringList IndependentVariableValueM::columnNames = {
    "id_experimentation",
    "id_task_instance",
    "id_independent_var",
    "independent_var_value",
};

/**
 * @brief Independent variable value table primary keys IN ORDER
 */
const QStringList IndependentVariableValueM::primaryKeys = {
    "id_experimentation",
    "id_task_instance",
    "id_independent_var",
};

/**
 * @brief Constructor setting all parameters
 * @param experimentationUuid
 * @param taskInstanceUuid
 * @param independentVariableUuid
 * @param valueString
 */
IndependentVariableValueM::IndependentVariableValueM(const CassUuid& experimentationUuid, const CassUuid& taskInstanceUuid, const CassUuid& independentVariableUuid, const QString& valueString)
    : experimentationUuid(experimentationUuid)
    , taskInstanceUuid(taskInstanceUuid)
    , independentVariableUuid(independentVariableUuid)
    , valueString(valueString) {}

/**
 * @brief Static factory method to create an independent variable value from a CassandraDB record
 * @param row
 * @return
 */

IndependentVariableValueM* IndependentVariableValueM::createFromCassandraRow(const CassRow* row)
{
    IndependentVariableValueM* indeVarValue = nullptr;

    if (row != nullptr)
    {
        // Get independent variable uuid
        CassUuid experimentationUuid, taskInstanceUuid, indeVarUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task_instance"), &taskInstanceUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_independent_var"), &indeVarUuid);
        QString valueString = AssessmentsModelManager::getStringValueFromColumnName(row, "independent_var_value");

        indeVarValue = new IndependentVariableValueM(experimentationUuid, taskInstanceUuid, indeVarUuid, valueString);
    }

    return indeVarValue;
}

/**
 * @brief Create a CassStatement to insert an IndependentVariableValueM into the DB.
 * The statement contains the values from the given independent variable value.
 * Passed independent variable value must have a valid and unique UUID.
 * @param independentVariableValue
 * @return
 */

CassStatement* IndependentVariableValueM::createBoundInsertStatement(const IndependentVariableValueM& independentVariableValue)
{
    QString queryStr = "INSERT INTO " + IndependentVariableValueM::table + " (id_experimentation, id_task_instance, id_independent_var, independent_var_value) VALUES (?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 4);
    cass_statement_bind_uuid  (cassStatement, 0, independentVariableValue.experimentationUuid);
    cass_statement_bind_uuid  (cassStatement, 1, independentVariableValue.taskInstanceUuid);
    cass_statement_bind_uuid  (cassStatement, 2, independentVariableValue.independentVariableUuid);
    cass_statement_bind_string(cassStatement, 3, independentVariableValue.valueString.toStdString().c_str());
    return cassStatement;
}
