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

/**
 * @brief Independent variable table name
 */
const QString IndependentVariableM::table = "ingescape.independent_var";

/**
 * @brief Independent variable table column names
 */
const QStringList IndependentVariableM::columnNames = {
    "id_experimentation",
    "id_task",
    "id",
    "description",
    "enum_value",
    "name",
    "value_type",
};

/**
 * @brief Independent variable table primary keys IN ORDER
 */
const QStringList IndependentVariableM::primaryKeys = {
    "id_experimentation",
    "id_task",
    "id",
};

/**
 * @brief Constructor
 * @param experimentationUuid
 * @param protocolUuid
 * @param uuid
 * @param name
 * @param description
 * @param valueType
 * @param enumValues
 * @param parent
 */
IndependentVariableM::IndependentVariableM(CassUuid experimentationUuid,
                                           CassUuid protocolUuid,
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
    _protocolCassUuid(protocolUuid),
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
IndependentVariableM* IndependentVariableM::createFromCassandraRow(const CassRow* row)
{
    IndependentVariableM* independentVariable = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, protocolUuid, independentVarUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &protocolUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &independentVarUuid);

        QString variableName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QString variableDescription(AssessmentsModelManager::getStringValueFromColumnName(row, "description"));

        int8_t i8ValueType = 0;
        cass_value_get_int8(cass_row_get_column_by_name(row, "value_type"), &i8ValueType);
        IndependentVariableValueTypes::Value valueType = static_cast<IndependentVariableValueTypes::Value>(i8ValueType);

        QStringList enumValues = AssessmentsModelManager::getStringListFromColumnName(row, "enum_values");

        independentVariable = new IndependentVariableM(experimentationUuid,
                                                       protocolUuid,
                                                       independentVarUuid,
                                                       variableName,
                                                       variableDescription,
                                                       valueType,
                                                       enumValues);
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
    AssessmentsModelManager::deleteEntry<IndependentVariableM>({ independentVariable.getExperimentationCassUuid(),
                                                                 independentVariable.getProtocolCassUuid(),
                                                                 independentVariable.getCassUuid() });
}


/**
 * @brief Create a CassStatement to insert an IndependentVariableM into the DB.
 * The statement contains the values from the given independentVariable.
 * Passed independentVariable must have a valid and unique UUID.
 * @param independentVariable
 * @return
 */
CassStatement* IndependentVariableM::createBoundInsertStatement(const IndependentVariableM& independentVariable)
{
    QString queryStr = "INSERT INTO " + IndependentVariableM::table + " (id_experimentation, id_task, id, name, description, value_type, enum_values) VALUES (?, ?, ?, ?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 7);
    cass_statement_bind_uuid  (cassStatement, 0, independentVariable.getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 1, independentVariable.getProtocolCassUuid());
    cass_statement_bind_uuid  (cassStatement, 2, independentVariable.getCassUuid());
    cass_statement_bind_string(cassStatement, 3, independentVariable.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 4, independentVariable.description().toStdString().c_str());
    cass_statement_bind_int8  (cassStatement, 5, static_cast<int8_t>(independentVariable.valueType()));

    CassCollection* enumValuesCassList = cass_collection_new(CASS_COLLECTION_TYPE_LIST, static_cast<size_t>(independentVariable.enumValues().size()));
    for (QString enumValue : independentVariable.enumValues()) {
        cass_collection_append_string(enumValuesCassList, enumValue.toStdString().c_str());
    }
    cass_statement_bind_collection(cassStatement, 6, enumValuesCassList);
    cass_collection_free(enumValuesCassList);

    return cassStatement;
}


/**
 * @brief Create a CassStatement to update an IndependentVariableM into the DB.
 * The statement contains the values from the given independentVariable.
 * Passed independentVariable must have a valid and unique UUID.
 * @param independentVariable
 * @return
 */
CassStatement* IndependentVariableM::createBoundUpdateStatement(const IndependentVariableM& independentVariable)
{
    QString queryStr = "UPDATE " + IndependentVariableM::table + " SET name = ?, description = ?, value_type = ?, enum_values = ? WHERE id_experimentation = ? AND id_task = ? AND id = ?;";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 7);
    cass_statement_bind_string(cassStatement, 0, independentVariable.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 1, independentVariable.description().toStdString().c_str());
    cass_statement_bind_int8  (cassStatement, 2, static_cast<int8_t>(independentVariable.valueType()));

    CassCollection* enumValuesCassList = cass_collection_new(CASS_COLLECTION_TYPE_LIST, static_cast<size_t>(independentVariable.enumValues().size()));
    for (QString enumValue : independentVariable.enumValues()) {
        cass_collection_append_string(enumValuesCassList, enumValue.toStdString().c_str());
    }
    cass_statement_bind_collection(cassStatement, 3, enumValuesCassList);
    cass_collection_free(enumValuesCassList);

    cass_statement_bind_uuid  (cassStatement, 4, independentVariable.getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 5, independentVariable.getProtocolCassUuid());
    cass_statement_bind_uuid  (cassStatement, 6, independentVariable.getCassUuid());
    return cassStatement;
}
