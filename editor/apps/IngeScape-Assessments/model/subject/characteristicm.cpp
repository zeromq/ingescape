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

/**
 * @brief Constructor
 * @param name
 * @param valueType
 * @param parent
 */
CharacteristicM::CharacteristicM(CassUuid cassUuid,
                                 QString name,
                                 CharacteristicValueTypes::Value valueType,
                                 QObject *parent) : QObject(parent),
    _name(name),
    _valueType(valueType),
    _isSubjectId(false),
    _enumValues(QStringList()),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Characteristic" << _name << "of type" << CharacteristicValueTypes::staticEnumToString(_valueType);

    if (_name == CHARACTERISTIC_SUBJECT_ID) {
        _isSubjectId = true;
    }
}


/**
 * @brief Destructor
 */
CharacteristicM::~CharacteristicM()
{
    qInfo() << "Delete Model of Characteristic" << _name << "of type" << CharacteristicValueTypes::staticEnumToString(_valueType);

}


/**
 * @brief Get the unique identifier in Cassandra Data Base
 * @return
 */
CassUuid CharacteristicM::getCassUuid()
{
    return _cassUuid;
}
