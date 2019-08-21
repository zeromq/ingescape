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

#include "characteristicm.h"

#include "controller/assessmentsmodelmanager.h"
#include "model/experimentationm.h"

const QString CHARACTERISTIC_SUBJECT_ID = "ID";

// Experimentation table name
const QString CharacteristicM::table = "ingescape.characteristic";

/**
 * @brief Constructor
 * @param name
 * @param valueType
 * @param parent
 */
CharacteristicM::CharacteristicM(CassUuid cassUuid,
                                 CassUuid experimentationUuid,
                                 const QString& name,
                                 CharacteristicValueTypes::Value valueType,
                                 const QStringList& enumValues,
                                 QObject *parent) : QObject(parent),
    _name(name),
    _valueType(valueType),
    _isSubjectId(name == CHARACTERISTIC_SUBJECT_ID),
    _enumValues(enumValues),
    _experimentationCassUuid(experimentationUuid),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Characteristic" << _name << "of type" << CharacteristicValueTypes::staticEnumToString(_valueType) << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ")";
}


/**
 * @brief Destructor
 */
CharacteristicM::~CharacteristicM()
{
    qInfo() << "Delete Model of Characteristic" << _name << "of type" << CharacteristicValueTypes::staticEnumToString(_valueType) << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ")";

}


/**
 * @brief Static factory method to create a characteristic from a CassandraDB record
 * @param row
 * @return
 */
CharacteristicM* CharacteristicM::createCharacteristicFromCassandraRow(const CassRow* row)
{
    CharacteristicM* characteristic = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, characteristicUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &characteristicUuid);

        const char *chrTaskName = "";
        size_t nameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrTaskName, &nameLength);
        QString characteristicName = QString::fromUtf8(chrTaskName, static_cast<int>(nameLength));

        QStringList enumValues;
        CassIterator* enumValuesIterator = cass_iterator_from_collection(cass_row_get_column_by_name(row, "enum_values"));
        if (enumValuesIterator != nullptr) {
            while(cass_iterator_next(enumValuesIterator)) {
                const char *chrEnumValue = "";
                size_t enumValueLength = 0;
                cass_value_get_string(cass_iterator_get_value(enumValuesIterator), &chrEnumValue, &enumValueLength);
                enumValues.append(QString::fromUtf8(chrEnumValue, static_cast<int>(enumValueLength)));
            }

            cass_iterator_free(enumValuesIterator);
            enumValuesIterator = nullptr;
        }

        int8_t i8ValueType = 0;
        cass_value_get_int8(cass_row_get_column_by_name(row, "value_type"), &i8ValueType);
        CharacteristicValueTypes::Value valueType = static_cast<CharacteristicValueTypes::Value>(i8ValueType);

        characteristic = new CharacteristicM(characteristicUuid, experimentationUuid, characteristicName, valueType, enumValues);
    }

    return characteristic;
}

/**
 * @brief Delete a characteristic from Cassandra DB
 * @param characteristic The characteristic to delete
 * @param experimentation The experimentation it's associated with for back reference to subjects
 */
void CharacteristicM::deleteCharacteristicFromCassandra(const CharacteristicM& characteristic, ExperimentationM* experimentation)
{
    QString queryStr = "DELETE FROM " + CharacteristicM::table + " WHERE id_experimentation = ? AND id = ?;";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 2);
    cass_statement_bind_uuid(cassStatement, 0, characteristic.getExperimentationCassUuid());
    cass_statement_bind_uuid(cassStatement, 1, characteristic.getCassUuid());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "Characteristic" << characteristic.name() << "has been successfully deleted from the DB";

        _deleteCharacteristicValuesForCharacteristic(characteristic, experimentation);
    }
    else {
        qCritical() << "Could not delete the characteristic" << characteristic.name() << "from the DB:" << cass_error_desc(cassError);
    }

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);
}

/**
 * @brief Delete every characteric value associated with the given characteristic
 * @param characteristic The characteristic to delete
 * @param experimentation The experimentation it's associated with for back reference to subjects
 */
void CharacteristicM::_deleteCharacteristicValuesForCharacteristic(const CharacteristicM& characteristic, ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        for (auto subjectIt = experimentation->allSubjects()->begin() ; subjectIt != experimentation->allSubjects()->end() ; ++subjectIt)
        {
            SubjectM* subject = *subjectIt;
            if (subject != nullptr)
            {
                QString queryStr = "DELETE FROM " + CharacteristicValueM::table + " WHERE id_experimentation = ? AND id_subject = ? AND id_characteristic = ;";
                CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 3);
                cass_statement_bind_uuid(cassStatement, 0, characteristic.getExperimentationCassUuid());
                cass_statement_bind_uuid(cassStatement, 1, subject->getCassUuid());
                cass_statement_bind_uuid(cassStatement, 2, characteristic.getCassUuid());

                // Execute the query or bound statement
                CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
                CassError cassError = cass_future_error_code(cassFuture);
                if (cassError == CASS_OK)
                {
                    qInfo() << "Characteristic values for characteristic" << characteristic.name() << "has been successfully deleted from the DB";
                }
                else {
                    qCritical() << "Could not delete the characteristic values for characteristic" << characteristic.name() << "from the DB:" << cass_error_desc(cassError);
                }

                // Clean-up cassandra objects
                cass_future_free(cassFuture);
                cass_statement_free(cassStatement);
            }
        }
    }
}
