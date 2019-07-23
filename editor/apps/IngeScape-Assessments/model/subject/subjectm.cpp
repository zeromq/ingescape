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
SubjectM::SubjectM(CassUuid experimentationUuid,
                   CassUuid cassUuid,
                   QString displayedId,
                   QObject *parent) : QObject(parent),
    _uid(""),
    _displayedId(displayedId),
    _mapCharacteristicValues(nullptr),
    _experimentationCassUuid(experimentationUuid),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    char chrCassUid[CASS_UUID_STRING_LENGTH];
    cass_uuid_string(_cassUuid, chrCassUid);
    _uid = QString(chrCassUid);

    qInfo() << "New Model of Subject" << _displayedId << "(" << _uid << ")";

    // Create the "Qml Property Map" that allows to set key-value pairs that can be used in QML bindings
    _mapCharacteristicValues = new QQmlPropertyMap(this);

    // Connect to signal "Value Changed" fro the "Qml Property Map"
    connect(_mapCharacteristicValues, &QQmlPropertyMap::valueChanged, this, &SubjectM::_onCharacteristicValueChanged);
}


/**
 * @brief Destructor
 */
SubjectM::~SubjectM()
{
    qInfo() << "Delete Model of Subject" << _displayedId << "(" << _uid << ")";

    // Free memory
    if (_mapCharacteristicValues != nullptr)
    {
        /*// Clear each value
        for (QString key : _mapCharacteristicValues->keys())
        {
            _mapCharacteristicValues->clear(key);
        }*/

        QQmlPropertyMap* temp = _mapCharacteristicValues;
        setmapCharacteristicValues(nullptr);
        delete temp;
    }
}


/**
 * @brief Add the characteristic to our experimentation
 * @param characteristic
 */
void SubjectM::addCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && (_mapCharacteristicValues != nullptr))
    {
        switch (characteristic->valueType())
        {
            case CharacteristicValueTypes::INTEGER:
                _mapCharacteristicValues->insert(characteristic->name(), QVariant(0));
                break;

            case CharacteristicValueTypes::DOUBLE:
                _mapCharacteristicValues->insert(characteristic->name(), QVariant(0.0));
                break;

            case CharacteristicValueTypes::TEXT:
                _mapCharacteristicValues->insert(characteristic->name(), QVariant(""));
                break;

            case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
                _mapCharacteristicValues->insert(characteristic->name(), QVariant(""));
                break;

            default:
                qWarning() << "We cannot add the characteristic" << characteristic->name() << "because the type" <<  characteristic->valueType() << "is wrong !";
                break;
        }
    }

    qDebug() << "Subject" << _displayedId << "has characteristics:" << _mapCharacteristicValues;
}

void SubjectM::setCharacteristicValue(CharacteristicM* characteristic, const QVariant& value)
{
    qDebug() << "Setting" << characteristic->name() << "=" << value.toString();
    _mapCharacteristicValues->insert(characteristic->name(), value);
}


/**
 * @brief Remove the characteristic from our experimentation
 * @param characteristic
 */
void SubjectM::removeCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && (_mapCharacteristicValues != nullptr))
    {
        // Clears the value (if any) associated with key
        _mapCharacteristicValues->clear(characteristic->name());
    }
}


/**
 * @brief Static factory method to create a subject from a CassandraDB record
 * @param row
 * @return
 */
SubjectM* SubjectM::createTaskFromCassandraRow(const CassRow* row)
{
    SubjectM* subject = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, subjectUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &subjectUuid);

        const char *chrDisplayedId = "";
        size_t displayedIdLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "displayed_id"), &chrDisplayedId, &displayedIdLength);
        QString displayedId = QString::fromUtf8(chrDisplayedId, static_cast<int>(displayedIdLength));

        subject = new SubjectM(experimentationUuid, subjectUuid, displayedId);
    }

    return subject;
}


/**
 * @brief Slot called when the value of a characteristic changed
 * @param key
 * @param value
 */
void SubjectM::_onCharacteristicValueChanged(const QString &key, const QVariant &value)
{
    qDebug() << key << "-->" << value.toString();

    if (key == CHARACTERISTIC_SUBJECT_ID)
    {
        setdisplayedId(value.toString());
    }
}

