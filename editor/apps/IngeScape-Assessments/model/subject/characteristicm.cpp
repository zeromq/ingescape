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

const QString CHARACTERISTIC_SUBJECT_ID = "ID";

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
    _uid(""),
    _name(name),
    _valueType(valueType),
    _isSubjectId(name == CHARACTERISTIC_SUBJECT_ID),
    _enumValues(enumValues),
    _experimentationCassUuid(experimentationUuid),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    char chrCassUid[CASS_UUID_STRING_LENGTH];
    cass_uuid_string(_cassUuid, chrCassUid);
    _uid = QString(chrCassUid);

    qInfo() << "New Model of Characteristic" << _name << "of type" << CharacteristicValueTypes::staticEnumToString(_valueType) << "(" << _uid << ")";
}


/**
 * @brief Destructor
 */
CharacteristicM::~CharacteristicM()
{
    qInfo() << "Delete Model of Characteristic" << _name << "of type" << CharacteristicValueTypes::staticEnumToString(_valueType) << "(" << _uid << ")";

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

        const char *chrEnumValues = "";
        size_t enumValuesLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "enum_values"), &chrEnumValues, &enumValuesLength);
        QStringList enumValues(QString::fromUtf8(chrEnumValues, static_cast<int>(enumValuesLength)).split(";"));

        int8_t i8ValueType = 0;
        cass_value_get_int8(cass_row_get_column_by_name(row, "value_type"), &i8ValueType);
        CharacteristicValueTypes::Value valueType = static_cast<CharacteristicValueTypes::Value>(i8ValueType);

        characteristic = new CharacteristicM(characteristicUuid, experimentationUuid, characteristicName, valueType, enumValues);
    }

    return characteristic;
}
