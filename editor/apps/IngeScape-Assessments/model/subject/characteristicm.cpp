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
#include "model/subject/characteristicvaluem.h"

const QString CHARACTERISTIC_SUBJECT_ID = "ID";

/**
 * @brief Characteristic table name
 */
const QString CharacteristicM::table = "ingescape.characteristic";

/**
 * @brief Characteristic table column names
 */
const QStringList CharacteristicM::columnNames = {
      "id_experimentation"
    , "id"
    , "enum_values"
    , "name"
    , "value_type"
};

/**
 * @brief Characteristic table primary keys IN ORDER
 */
const QStringList CharacteristicM::primaryKeys = {
      "id_experimentation"
    , "id"
};

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
CharacteristicM* CharacteristicM::createFromCassandraRow(const CassRow* row)
{
    CharacteristicM* characteristic = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, characteristicUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &characteristicUuid);

        QString characteristicName = AssessmentsModelManager::getStringValueFromColumnName(row, "name");
        QStringList enumValues = AssessmentsModelManager::getStringListFromColumnName(row, "enum_values");

        int8_t i8ValueType = 0;
        cass_value_get_int8(cass_row_get_column_by_name(row, "value_type"), &i8ValueType);
        CharacteristicValueTypes::Value valueType = static_cast<CharacteristicValueTypes::Value>(i8ValueType);

        characteristic = new CharacteristicM(characteristicUuid, experimentationUuid, characteristicName, valueType, enumValues);
    }

    return characteristic;
}
