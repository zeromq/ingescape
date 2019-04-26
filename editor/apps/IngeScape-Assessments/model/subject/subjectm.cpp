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
                   //QString name,
                   QObject *parent) : QObject(parent),
    _uid(uid),
    _name(""), //_name(name),
    _propertyMap(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Subject" << _name << "(" << _uid << ")";

    // Create the "Qml Property Map" that allows to set key-value pairs that can be used in QML bindings
    _propertyMap = new QQmlPropertyMap(this);

    // Connect to signal "Value Changed" fro the "Qml Property Map"
    connect(_propertyMap, &QQmlPropertyMap::valueChanged, this, &SubjectM::_onCharacteristicValueChanged);
}


/**
 * @brief Destructor
 */
SubjectM::~SubjectM()
{
    qInfo() << "Delete Model of Subject" << _name << "(" << _uid << ")";

    // Free memory
    if (_propertyMap != nullptr)
    {
        /*// Clear each value
        for (QString key : _propertyMap->keys())
        {
            _propertyMap->clear(key);
        }*/

        QQmlPropertyMap* temp = _propertyMap;
        setpropertyMap(nullptr);
        delete temp;
    }
}


/**
 * @brief Add the characteristic to our experimentation
 * @param characteristic
 */
void SubjectM::addCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && (_propertyMap != nullptr))
    {
        // It is not possible to remove keys from the map;
        // once a key has been added, you can only modify or clear its associated value
        // --> Do not test if the map contains this key
        //if (!_propertyMap->contains(characteristic->name()))

        /*switch (characteristic->valueType())
        {
        case CharacteristicValueTypes::INTEGER:
            _propertyMap->insert(characteristic->name(), QVariant(0));
            break;

        case CharacteristicValueTypes::DOUBLE:
            _propertyMap->insert(characteristic->name(), QVariant(0.0));
            break;

        case CharacteristicValueTypes::TEXT:
            _propertyMap->insert(characteristic->name(), QVariant(""));
            break;

        case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
            _propertyMap->insert(characteristic->name(), QVariant(""));
            break;

        default:
            qWarning() << "We cannot add the characteristic" << characteristic->name() << "because the type" <<  characteristic->valueType() << "is wrong !";
            break;
        }*/

        // Insert an (invalid) not initialized QVariant
        _propertyMap->insert(characteristic->name(), QVariant());
    }
}


/**
 * @brief Remove the characteristic from our experimentation
 * @param characteristic
 */
void SubjectM::removeCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && (_propertyMap != nullptr))
    {
        // Clears the value (if any) associated with key
        _propertyMap->clear(characteristic->name());
    }
}


/**
 * @brief Slot called when the value of a characteristic changed
 * @param key
 * @param value
 */
void SubjectM::_onCharacteristicValueChanged(const QString &key, const QVariant &value)
{
    qDebug() << key << "-->" << value.toString();

    if (key == CHARACTERISTIC_SUBJECT_NAME)
    {
        setname(value.toString());
    }
}

