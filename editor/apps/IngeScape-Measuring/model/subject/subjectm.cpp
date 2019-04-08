/*
 *	IngeScape Measuring
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

#include "subjectm.h"


/**
 * @brief Constructor
 * @param uid
 * @param parent
 */
SubjectM::SubjectM(QString uid,
                   QObject *parent) : QObject(parent),
    _uid(uid),
    _name("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Subject" << _uid;

    // Add the default characteristic
    _mapFromCharacteristicIdToValue.insert(CHARACTERISTIC_UID, QVariant(_uid));

}


/**
 * @brief Destructor
 */
SubjectM::~SubjectM()
{
    qInfo() << "Delete Model of Subject" << _uid;

}


/**
 * @brief Add the characteristic to our experimentation
 * @param characteristic
 */
void SubjectM::addCharacteristic(CharacteristicM* characteristic)
{
    if (characteristic != nullptr)
    {
        if (!_mapFromCharacteristicIdToValue.contains(characteristic->name()))
        {
            // UNKNOWN, INTEGER, DOUBLE, TEXT, CHARACTERISTIC_ENUM
            switch (characteristic->valueType())
            {
            case CharacteristicValueTypes::INTEGER:
                _mapFromCharacteristicIdToValue.insert(characteristic->name(), QVariant(0));
                break;

            case CharacteristicValueTypes::DOUBLE:
                _mapFromCharacteristicIdToValue.insert(characteristic->name(), QVariant(0.0));
                break;

            case CharacteristicValueTypes::TEXT:
                _mapFromCharacteristicIdToValue.insert(characteristic->name(), QVariant(""));
                break;

            case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
                _mapFromCharacteristicIdToValue.insert(characteristic->name(), QVariant(""));
                break;

            default:
                qWarning() << "We cannot add the characteristic" << characteristic->name() << "because the type" <<  characteristic->valueType() << "is wrong !";
                break;
            }
        }
    }
}


/**
 * @brief Remove the characteristic from our experimentation
 * @param characteristic
 */
void SubjectM::removeCharacteristic(CharacteristicM* characteristic)
{
    if (characteristic != nullptr)
    {
        if (_mapFromCharacteristicIdToValue.contains(characteristic->name())) {
            _mapFromCharacteristicIdToValue.remove(characteristic->name());
        }
    }
}


/**
 * @brief Get the value of a characteristic
 * @param characteristicName
 * @return
 */
QString SubjectM::getValueOfCharacteristic(QString characteristicName)
{
    QString characteristicValue = "";

    if (_mapFromCharacteristicIdToValue.contains(characteristicName))
    {
        QVariant characteristicVariant = _mapFromCharacteristicIdToValue.value(characteristicName);

        characteristicValue = characteristicVariant.toString();
    }

    return characteristicValue;
}

