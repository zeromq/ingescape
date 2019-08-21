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

#include "independentvariablem.h"

#include "controller/assessmentsmodelmanager.h"

const QString IndependentVariableM::table = "ingescape.independent_var";

/**
 * @brief Constructor
 * @param name
 * @param description
 * @param valueType
 * @param parent
 */
IndependentVariableM::IndependentVariableM(CassUuid experimentationUuid,
                                           CassUuid taskUuid,
                                           CassUuid uuid,
                                           QString name,
                                           QString description,
                                           IndependentVariableValueTypes::Value valueType,
                                           QStringList enumValues,
                                           QObject *parent) : QObject(parent),
    _name(name),
    _description(description),
    _valueType(valueType),
    _enumValues(enumValues),
    _experimentationCassUuid(experimentationUuid),
    _taskCassUuid(taskUuid),
    _cassUuid(uuid)

{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Independent Variable" << _name << "of type" << IndependentVariableValueTypes::staticEnumToString(_valueType);
}


/**
 * @brief Destructor
 */
IndependentVariableM::~IndependentVariableM()
{
    qInfo() << "Delete Model of Independent Variable" << _name << "of type" << IndependentVariableValueTypes::staticEnumToString(_valueType);

}


/**
 * @brief Static factory method to create an independent variable from a CassandraDB record
 * @param row
 * @return
 */
IndependentVariableM* IndependentVariableM::createIndependentVariableFromCassandraRow(const CassRow* row)
{
    IndependentVariableM* independentVariable = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, taskUuid, independentVarUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &taskUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &independentVarUuid);

        QString variableName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QString variableDescription(AssessmentsModelManager::getStringValueFromColumnName(row, "description"));

        int8_t i8ValueType = 0;
        cass_value_get_int8(cass_row_get_column_by_name(row, "value_type"), &i8ValueType);
        IndependentVariableValueTypes::Value valueType = static_cast<IndependentVariableValueTypes::Value>(i8ValueType);

        QStringList enumValues = AssessmentsModelManager::getStringListFromColumnName(row, "enum_values");

        independentVariable = new IndependentVariableM(experimentationUuid, taskUuid, independentVarUuid, variableName, variableDescription, valueType, enumValues);
    }

    return independentVariable;
}


/**
 * @brief Delete the given independent variable from Cassandra DB
 * @param independentVariable
 * @return
 */
void IndependentVariableM::deleteIndependentVariableFromCassandra(const IndependentVariableM& independentVariable)
{
    // Remove independent variable from DB
    QString queryStr = "DELETE FROM " + IndependentVariableM::table + " WHERE id_experimentation = ? AND id_task = ? AND id = ?;";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 3);
    cass_statement_bind_uuid(cassStatement, 0, independentVariable.getExperimentationCassUuid());
    cass_statement_bind_uuid(cassStatement, 1, independentVariable.getTaskCassUuid());
    cass_statement_bind_uuid(cassStatement, 2, independentVariable.getCassUuid());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "Independent variable" << independentVariable.name() << "has been successfully deleted from the DB";
    }
    else {
        qCritical() << "Could not delete the independent variable" << independentVariable.name() << "from the DB:" << cass_error_desc(cassError);
    }

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);
}
