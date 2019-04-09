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

#include "subjectm.h"


/**
 * @brief Constructor
 * @param uid
 * @param name
 * @param parent
 */
SubjectM::SubjectM(QString uid,
                   QString name,
                   QObject *parent) : QObject(parent),
    _uid(uid),
    _name(name)
    //_isCurrentlyEditing(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Subject" << _name << "(" << _uid << ")";

    // Add the default characteristic "UID"
    _mapFromCharacteristicIdToValue.insert(CHARACTERISTIC_UID, QVariant(_uid));

    // Add the default characteristic "Name"
    _mapFromCharacteristicIdToValue.insert(CHARACTERISTIC_NAME, QVariant(_name));

}


/**
 * @brief Destructor
 */
SubjectM::~SubjectM()
{
    qInfo() << "Delete Model of Subject" << _name << "(" << _uid << ")";

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

        qDebug() << "getValueOfCharacteristic" << characteristicName << "-->" << characteristicValue;
    }

    return characteristicValue;
}


/**
 * @brief Set the value of a characteristic
 * @param characteristicValue
 * @param characteristic
 */
void SubjectM::setValueOfCharacteristic(QString characteristicValue, CharacteristicM* characteristic)
{
    if (characteristic != nullptr)
    {
        // UNKNOWN, INTEGER, DOUBLE, TEXT, CHARACTERISTIC_ENUM
        switch (characteristic->valueType())
        {
        case CharacteristicValueTypes::INTEGER:
        {
            bool success = false;
            int nValue = characteristicValue.toInt(&success);
            if (success) {
                qDebug() << "Set (INTEGER)" << nValue << "in" << characteristic->name() << "of" << _uid;
                _mapFromCharacteristicIdToValue.insert(characteristic->name(), QVariant(nValue));
            }
        }
            break;

        case CharacteristicValueTypes::DOUBLE:
        {
            bool success = false;
            double dValue = characteristicValue.toDouble(&success);
            if (success) {
                qDebug() << "Set (DOUBLE)" << dValue << "in" << characteristic->name() << "of" << _uid;
                _mapFromCharacteristicIdToValue.insert(characteristic->name(), QVariant(dValue));
            }
        }
            break;

        case CharacteristicValueTypes::TEXT:
        {
            qDebug() << "Set (TEXT)" << characteristicValue << "in" << characteristic->name() << "of" << _uid;
            _mapFromCharacteristicIdToValue.insert(characteristic->name(), QVariant(characteristicValue));
        }
            break;

        case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
        {
            if (!characteristic->enumValues().isEmpty() && !characteristicValue.isEmpty()
                    && characteristic->enumValues().contains(characteristicValue))
            {
                qDebug() << "Set (CHARACTERISTIC_ENUM)" << characteristicValue << "in" << characteristic->name() << "of" << _uid;
                _mapFromCharacteristicIdToValue.insert(characteristic->name(), QVariant(characteristicValue));
            }
        }
            break;

        default:
            qWarning() << "We cannot set the value" << characteristicValue << "of the characteristic" << characteristic->name() << "because the type" <<  characteristic->valueType() << "is wrong !";
            break;
        }
    }
}

