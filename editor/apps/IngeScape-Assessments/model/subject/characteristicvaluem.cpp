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

#include "characteristicvaluem.h"
#include "model/subject/characteristicm.h"
#include "controller/assessmentsmodelmanager.h"

/**
 * @brief Characteristic value table name
 */
const QString CharacteristicValueM::table = "ingescape.characteristic_value";

/**
 * @brief Characteristic value table column names
 */
const QStringList CharacteristicValueM::columnNames = {
    "id_experimentation",
    "id_subject",
    "id_characteristic",
    "characteristic_value",
};

/**
 * @brief Characteristic value table primary keys IN ORDER
 */
const QStringList CharacteristicValueM::primaryKeys = {
    "id_experimentation",
    "id",
};

/**
 * @brief Constructor setting all parameters
 * @param experimentationUuid
 * @param subjectUuid
 * @param characteristicUuid
 * @param valueString
 */
CharacteristicValueM::CharacteristicValueM(const CassUuid& experimentationUuid, const CassUuid& subjectUuid, const CassUuid& characteristicUuid, const QString& characteristicValue)
    : experimentationUuid(experimentationUuid)
    , subjectUuid(subjectUuid)
    , characteristicUuid(characteristicUuid)
    , valueString(characteristicValue) {}


/**
 * @brief Static factory method to create a characteristic value from a CassandraDB record
 * @param row
 * @return
 */
CharacteristicValueM* CharacteristicValueM::createFromCassandraRow(const CassRow* row)
{
    CharacteristicValueM* characValue = nullptr;

    if (row != nullptr)
    {
        // Get characteristic id
        CassUuid experimentationUuid, subjectUuid, characteristicUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_subject"), &subjectUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_characteristic"), &characteristicUuid);

        // Get characteristic value as a string
        QString valueString = AssessmentsModelManager::getStringValueFromColumnName(row, "characteristic_value");

        characValue = new CharacteristicValueM(experimentationUuid, subjectUuid, characteristicUuid, valueString);
    }

    return characValue;
}

/**
 * @brief Create a CassStatement to insert an CharacteristicValueM into the DB.
 * The statement contains the values from the given characteristicValue.
 * Passed characteristicValue must have a valid and unique UUID.
 * @param characteristicValue
 * @return
 */
CassStatement* CharacteristicValueM::createBoundInsertStatement(const CharacteristicValueM& characteristicValue)
{
    // Create the query
    QString query = "INSERT INTO " + CharacteristicValueM::table + " (id_experimentation, id_subject, id_characteristic, characteristic_value) VALUES (?, ?, ?, ?);";

    // Creates the new query statement
    CassStatement* cassStatement = cass_statement_new(query.toStdString().c_str(), 4);
    cass_statement_bind_uuid  (cassStatement, 0, characteristicValue.experimentationUuid);
    cass_statement_bind_uuid  (cassStatement, 1, characteristicValue.subjectUuid);
    cass_statement_bind_uuid  (cassStatement, 2, characteristicValue.characteristicUuid);
    cass_statement_bind_string(cassStatement, 3, characteristicValue.valueString.toStdString().c_str());

    return cassStatement;
}
