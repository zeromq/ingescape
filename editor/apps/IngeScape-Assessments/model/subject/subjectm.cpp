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

#include "controller/assessmentsmodelmanager.h"


/**
 * @brief Constructor
 * @param experimentationUuid
 * @param cassUuid
 * @param displayedId
 * @param parent
 */
SubjectM::SubjectM(CassUuid experimentationUuid,
                   CassUuid cassUuid,
                   QString displayedId,
                   QObject *parent) : QObject(parent),
    _uid(AssessmentsModelManager::cassUuidToQString(cassUuid)),
    _displayedId(displayedId),
    _mapCharacteristicValues(nullptr),
    _tempMapCharacteristicValues(nullptr),
    _experimentationCassUuid(experimentationUuid),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Subject" << _displayedId << "(" << _uid << ")";

    // Create the "Qml Property Map" that allows to set key-value pairs that can be used in QML bindings
    _mapCharacteristicValues = new QQmlPropertyMap(this);
    _tempMapCharacteristicValues = new QQmlPropertyMap(this);

    // Connect to signal "Value Changed" fro the "Qml Property Map"
    connect(_mapCharacteristicValues, &QQmlPropertyMap::valueChanged, this, &SubjectM::_onCharacteristicValueChanged);
}


/**
 * @brief Destructor
 */
SubjectM::~SubjectM()
{
    qInfo() << "Delete Model of Subject" << _displayedId << "(" << _uid << ")";

    // Clean-up characteristic map. No deletion.
    _mapCharacteristicsByName.clear();

    // Free memory
    if (_mapCharacteristicValues != nullptr)
    {
        QQmlPropertyMap* temp = _mapCharacteristicValues;
        setmapCharacteristicValues(nullptr);
        delete temp;
    }
    if (_tempMapCharacteristicValues != nullptr)
    {
        QQmlPropertyMap* temp = _tempMapCharacteristicValues;
        settempMapCharacteristicValues(nullptr);
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

        _mapCharacteristicsByName.insert(characteristic->name(), characteristic);
    }

    qDebug() << "Subject" << _displayedId << "has characteristics:" << _mapCharacteristicValues;
}

void SubjectM::setCharacteristicValue(CharacteristicM* characteristic, const QVariant& value)
{
    if (characteristic != nullptr)
    {
        _mapCharacteristicValues->insert(characteristic->name(), value);
        _mapCharacteristicsByName.insert(characteristic->name(), characteristic);

        // Need to call manually the slot because the valueChanged signal is only emitted from QML
        _onCharacteristicValueChanged(characteristic->name(), value);
    }
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

        // Remove the characteristic from the name map
        _mapCharacteristicsByName.remove(characteristic->name());
    }
}


/**
 * @brief Static factory method to create a subject from a CassandraDB record
 * @param row
 * @return
 */
SubjectM* SubjectM::createSubjectFromCassandraRow(const CassRow* row)
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
 * @brief Delete the given subject from Cassandra DB
 * @param subject
 */
void SubjectM::deleteSubjectFromCassandra(const SubjectM& subject)
{
    // Remove subject from DB
    const char* query = "DELETE FROM ingescape.subject WHERE id_experimentation = ? AND id = ?;";
    CassStatement* cassStatement = cass_statement_new(query, 2);
    cass_statement_bind_uuid(cassStatement, 0, subject.getExperimentationCassUuid());
    cass_statement_bind_uuid(cassStatement, 1, subject.getCassUuid());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "Subject" << subject.displayedId() << "has been successfully deleted from the DB";

        // Delete the characteristic values associated with the subject
        _deleteCharacteristicValuesForSubject(subject);
    }
    else {
        qCritical() << "Could not delete the subject" << subject.displayedId() << "from the DB:" << cass_error_desc(cassError);
    }

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);
}

/**
 * @brief Restore the temporary map values wih the ones from the actual value map
 */
void SubjectM::resetTemporaryPropertyValues()
{
    for(const QString& propertyName : _mapCharacteristicValues->keys())
    {
        _tempMapCharacteristicValues->insert(propertyName, _mapCharacteristicValues->value(propertyName));
    }
}

/**
 * @brief Apply the values from the temporary to the actual value map
 * The DB will be update on the fly
 */
void SubjectM::applyTemporaryPropertyValues()
{
    for(const QString& propertyName : _tempMapCharacteristicValues->keys())
    {
        const QVariant& oldValue = _mapCharacteristicValues->value(propertyName);
        const QVariant& value = _tempMapCharacteristicValues->value(propertyName);

        if (oldValue != value)
        {
            _mapCharacteristicValues->insert(propertyName, value);

            // Need to call manually the slot because the valueChanged signal is only emitted from QML
            _onCharacteristicValueChanged(propertyName, value);
        }
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

    if (key == CHARACTERISTIC_SUBJECT_ID)
    {
        setdisplayedId(value.toString());
    }

    CharacteristicM* characteristic = _mapCharacteristicsByName.value(key, nullptr);
    if (characteristic != nullptr)
    {
        const char* query = "UPDATE ingescape.characteristic_value_of_subject SET characteristic_value = ? WHERE id_experimentation = ? AND id_subject = ? AND id_characteristic = ?;";
        CassStatement* cassStatement = cass_statement_new(query, 4);
        cass_statement_bind_string(cassStatement, 0, value.toString().toStdString().c_str());
        cass_statement_bind_uuid  (cassStatement, 1, _experimentationCassUuid);
        cass_statement_bind_uuid  (cassStatement, 2, _cassUuid);
        cass_statement_bind_uuid  (cassStatement, 3, characteristic->getCassUuid());
        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);

        if (cassError != CASS_OK)
        {
            qCritical() << "Could not update the characteristic value of" << characteristic->name() << "for subject" << displayedId();
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);
    }
}


/**
 * @brief Delete every characteristic value associated with the given subject
 * @param subject
 */
void SubjectM::_deleteCharacteristicValuesForSubject(const SubjectM& subject)
{
    const char* query = "DELETE FROM ingescape.characteristic_value_of_subject WHERE id_experimentation = ? AND id_subject = ?;";
    CassStatement* cassStatement = cass_statement_new(query, 2);
    cass_statement_bind_uuid(cassStatement, 0, subject.getExperimentationCassUuid());
    cass_statement_bind_uuid(cassStatement, 1, subject.getCassUuid());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "Characteristics for subject" << subject.displayedId() << "has been successfully deleted from the DB";
    }
    else {
        qCritical() << "Could not delete the characteristics for subject" << subject.displayedId() << "from the DB:" << cass_error_desc(cassError);
    }

    // Clean-up cassandra objects
    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);
}

