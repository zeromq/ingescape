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

#include "subjectscontroller.h"

#include <controller/assessmentsmodelmanager.h>


/**
 * @brief Constructor
 * @param modelManager
 * @param parent
 */
SubjectsController::SubjectsController(QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Subjects Controller";

    // Fill without type "UNKNOWN"
    _allCharacteristicValueTypes.fillWithAllEnumValues();
    _allCharacteristicValueTypes.removeEnumValue(CharacteristicValueTypes::UNKNOWN);

}


/**
 * @brief Destructor
 */
SubjectsController::~SubjectsController()
{
    qInfo() << "Delete Subjects Controller";

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr)
    {
        setcurrentExperimentation(nullptr);
    }

    /*if (_modelManager != nullptr)
    {
        //disconnect(_modelManager, nullptr, this, nullptr);

        _modelManager = nullptr;
    }*/
}


/**
 * @brief Setter for property "Current Experimentation"
 * @param value
 */
void SubjectsController::setcurrentExperimentation(ExperimentationM *value)
{
    if (_currentExperimentation != value)
    {
        _currentExperimentation = value;

        // Manage changes
        _onCurrentExperimentationChanged(_currentExperimentation);

        Q_EMIT currentExperimentationChanged(value);
    }
}


/**
 * @brief Return true if the user can create a characteristic with the name
 * Check if the name is not empty and if a characteristic with the same name does not already exist
 * @param characteristicName
 * @return
 */
bool SubjectsController::canCreateCharacteristicWithName(QString characteristicName)
{
    //NOTE Oneliner
//    const QList<CharacteristicM*>& characteristicList = _currentExperimentation->allCharacteristics()->toList();
//    return !characteristicName.isEmpty() && (_currentExperimentation != nullptr)
//            && std::none_of(characteristicList.begin(), characteristicList.end(),
//                            [characteristicName](CharacteristicM* characteristic){
//        return (characteristic != nullptr) && (characteristic->name() == characteristicName);
//    });

    if (!characteristicName.isEmpty() && (_currentExperimentation != nullptr))
    {
        for (CharacteristicM* characteristic : _currentExperimentation->allCharacteristics()->toList())
        {
            if ((characteristic != nullptr) && (characteristic->name() == characteristicName))
            {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
}


/**
 * @brief Create a new characteristic
 * @param characteristicName
 * @param nCharacteristicValueType
 */
void SubjectsController::createNewCharacteristic(const QString& characteristicName, int nCharacteristicValueType, const QStringList& enumValues)
{
    if (!characteristicName.isEmpty() && (nCharacteristicValueType > -1) && (_currentExperimentation != nullptr))
    {
        CharacteristicValueTypes::Value characteristicValueType = static_cast<CharacteristicValueTypes::Value>(nCharacteristicValueType);
        qInfo() << "Create new characteristic" << characteristicName << "of type" << CharacteristicValueTypes::staticEnumToString(characteristicValueType);

        // Create the new characteristic
        CharacteristicM* characteristic = _insertCharacteristicIntoDB(_currentExperimentation->getCassUuid(), characteristicName, characteristicValueType, enumValues);

        if (characteristic != nullptr)
        {
            // Add characteristic to every existing agent and add the corresponding DB entry
            // Avoid trying to add an existing characteristic to the DB when one exists with the same name
            bool notFound = std::none_of(_currentExperimentation->allCharacteristics()->begin(),
                                         _currentExperimentation->allCharacteristics()->end(),
                                         [characteristicName](CharacteristicM* characteristic) { return (characteristic != nullptr) && (characteristic->name() == characteristicName); });
            if (notFound)
            {
                for (auto subjectIt = _currentExperimentation->allSubjects()->begin() ; subjectIt != _currentExperimentation->allSubjects()->end() ; ++subjectIt)
                {
                    SubjectM* subject = *subjectIt;
                    if (subject != nullptr)
                    {
                        _insertCharacteristicValueForSubjectIntoDB(subject, characteristic);
                        subject->addCharacteristic(characteristic);
                    }
                }
            }

            // Add the characteristic to the current experimentation
            _currentExperimentation->addCharacteristic(characteristic);
        }
    }
}


/**
 * @brief Create a new characteristic of type enum
 * @param characteristicName
 * @param enumValues
 */
void SubjectsController::createNewCharacteristicEnum(QString characteristicName, QStringList enumValues)
{
    createNewCharacteristic(characteristicName, CharacteristicValueTypes::CHARACTERISTIC_ENUM, enumValues);
}


/**
 * @brief Delete the given characteristic from the current experimentation and from the Cassandra DB
 * @param characteristic
 */
void SubjectsController::deleteCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && (_currentExperimentation != nullptr))
    {
        // Remove the characteristic from the current experimentation
        _currentExperimentation->removeCharacteristic(characteristic);

        // Remove characteristic from DB
        const char* query = "DELETE FROM ingescape.characteristic WHERE id_experimentation = ? AND id = ?;";
        CassStatement* cassStatement = cass_statement_new(query, 2);
        cass_statement_bind_uuid(cassStatement, 0, characteristic->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, characteristic->getCassUuid());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Characteristic" << characteristic->name() << "has been successfully deleted from the DB";
        }
        else {
            qCritical() << "Could not delete the characteristic" << characteristic->name() << "from the DB:" << cass_error_desc(cassError);
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);

        // Free memory
        delete characteristic;
    }
}


/**
 * @brief Create a new subject
 */
void SubjectsController::createNewSubject()
{
    if ((_currentExperimentation != nullptr) && (AssessmentsModelManager::Instance() != nullptr))
    {
        QDateTime now = QDateTime::currentDateTime();

        QString displayedId = now.toString("S-yyMMdd-hhmmss-zzz");

        // Create a new subject
        SubjectM* subject = _insertSubjectIntoDB(_currentExperimentation->getCassUuid(), displayedId);

        if (subject != nullptr)
        {
            // For each existing characteristic
            for (CharacteristicM* characteristic : _currentExperimentation->allCharacteristics()->toList())
            {
                if (characteristic != nullptr)
                {
                    _insertCharacteristicValueForSubjectIntoDB(subject, characteristic);
                    subject->addCharacteristic(characteristic);
                    if (characteristic->name() == CHARACTERISTIC_SUBJECT_ID)
                    {
                        subject->setCharacteristicValue(characteristic, displayedId);
                    }
                }
            }

            // Add the subject to the current experimentation
            _currentExperimentation->addSubject(subject);
        }
    }
}


/**
 * @brief Delete the given subject from the current experimentation and from the Cassandra DB
 * @param subject
 */
void SubjectsController::deleteSubject(SubjectM* subject)
{
    if ((subject != nullptr) && (_currentExperimentation != nullptr))
    {
        // Remove the subject from the current experimentation
        _currentExperimentation->removeSubject(subject);

        // Remove subject from DB
        SubjectM::deleteSubjectFromCassandra(*subject);

        // Free memory
        delete subject;
    }
}


/**
 * @brief Slot called when the current experimentation changed
 * @param currentExperimentation
 */
void SubjectsController::_onCurrentExperimentationChanged(ExperimentationM* currentExperimentation)
{
    if ((currentExperimentation != nullptr) && (AssessmentsModelManager::Instance() != nullptr))
    {
        qDebug() << "Subjects Controller: on Current Experimentation changed" << currentExperimentation->name();

        // FIXME TODO: load data about this experimentation (subjects and characteristics)

        CassUuid uidExperimentation = currentExperimentation->getCassUuid();

        // Create the query
        QString queryGetCharacteristics = QString("SELECT * FROM ingescape.characteristic WHERE id_experimentation = ?;");

        // Creates the new query statement
        CassStatement* cassStatementGetCharacteristics = cass_statement_new(queryGetCharacteristics.toStdString().c_str(), 1);
        cass_statement_bind_uuid(cassStatementGetCharacteristics, 0, uidExperimentation);

        // Execute the query or bound statement
        CassFuture* cassFutureGetCharacteristics = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatementGetCharacteristics);

        CassError cassErrorGetCharacteristics = cass_future_error_code(cassFutureGetCharacteristics);
        if (cassErrorGetCharacteristics == CASS_OK)
        {
            qDebug() << "Get all characteristics succeeded";

            // Retrieve result set and iterate over the rows
            const CassResult* cassResult = cass_future_get_result(cassFutureGetCharacteristics);

            if (cassResult != nullptr)
            {
                size_t characteristicsNumber = cass_result_row_count(cassResult);
                if (characteristicsNumber == 0)
                {
                    qDebug() << "There is NO characteristic...create the special characteristic 'id'";

                    CassUuid characteristicUid;
                    cass_uuid_gen_time(AssessmentsModelManager::Instance()->getCassUuidGen(), &characteristicUid);

                    QString characteristicName = CHARACTERISTIC_SUBJECT_ID;
                    CharacteristicValueTypes::Value characteristicValueType = CharacteristicValueTypes::TEXT;

                    // Create the query
                    QString queryInsertCharacteristic = QString("INSERT INTO ingescape.characteristic (id, id_experimentation, name, value_type, enum_values) VALUES (?, ?, ?, ?, ?);");

                    // Creates the new query statement
                    CassStatement* cassStatementInsertCharacteristic = cass_statement_new(queryInsertCharacteristic.toStdString().c_str(), 5);
                    cass_statement_bind_uuid(cassStatementInsertCharacteristic, 0, characteristicUid);
                    cass_statement_bind_uuid(cassStatementInsertCharacteristic, 1, uidExperimentation);
                    cass_statement_bind_string(cassStatementInsertCharacteristic, 2, characteristicName.toStdString().c_str());
                    cass_statement_bind_int8(cassStatementInsertCharacteristic, 3, characteristicValueType);
                    cass_statement_bind_string(cassStatementInsertCharacteristic, 4, "");

                    // Execute the query or bound statement
                    CassFuture* cassFutureInsertCharacteristic = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatementInsertCharacteristic);

                    CassError cassErrorInsertCharacteristic = cass_future_error_code(cassFutureInsertCharacteristic);
                    if (cassErrorInsertCharacteristic == CASS_OK)
                    {
                        qInfo() << "CharacteristicM" << characteristicName << "inserted into the DataBase";

                        // Create the new characteristic
                        CharacteristicM* characteristic = new CharacteristicM(characteristicUid, currentExperimentation->getCassUuid(), characteristicName, characteristicValueType);

                        // Add the characteristic to the current experimentation
                        _currentExperimentation->addCharacteristic(characteristic);
                    }
                    else {
                        qCritical() << "Could not insert the experimentation" << characteristicName << "into the DataBase:" << cass_error_desc(cassErrorInsertCharacteristic);
                    }

                    cass_statement_free(cassStatementInsertCharacteristic);
                    cass_future_free(cassFutureInsertCharacteristic);
                }
                else
                {
                    qDebug() << "There are" << characteristicsNumber << "characteristics";

                    CassIterator* cassIterator = cass_iterator_from_result(cassResult);

                    while(cass_iterator_next(cassIterator))
                    {      
                        const CassRow* row = cass_iterator_get_row(cassIterator);

                        CassUuid characteristicUid;
                        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &characteristicUid);
                        QString chrCharacteristicUid = AssessmentsModelManager::cassUuidToQString(characteristicUid);

                        // Get the characteristic from its UID
                        CharacteristicM* characteristic = currentExperimentation->getCharacteristicFromUID(chrCharacteristicUid);

                        // It does not yet exist
                        if (characteristic == nullptr)
                        {
                            const char *chrCharacteristicName = "";
                            size_t nameLength;
                            cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrCharacteristicName, &nameLength);
                            QString characteristicName = QString::fromUtf8(chrCharacteristicName, static_cast<int>(nameLength));

                            CharacteristicValueTypes::Value characteristicValueType = CharacteristicValueTypes::UNKNOWN;
                            int8_t type;
                            cass_value_get_int8(cass_row_get_column_by_name(row, "value_type"), &type);
                            characteristicValueType = static_cast<CharacteristicValueTypes::Value>(type);

                            // Create the characteristic
                            characteristic = new CharacteristicM(characteristicUid, currentExperimentation->getCassUuid(), characteristicName, characteristicValueType);

                            // Add the characteristic to the current experimentation
                            _currentExperimentation->addCharacteristic(characteristic);
                        }
                        // FIXME Update the existing characteristic ?
                        else
                        {

                        }
                    }

                    cass_iterator_free(cassIterator);
                }
            }
        }
        else {
            qCritical() << "Could not get all characteristics from the DataBase:" << cass_error_desc(cassErrorGetCharacteristics);
        }
    }
}


/**
 * @brief Creates a new characteristic with the given parameters and insert it into the Cassandra DB
 * A nullptr is returned if the operation failed.
 * @param experimentationUuid
 * @param name
 * @param valueType
 * @param enumValues
 */
CharacteristicM* SubjectsController::_insertCharacteristicIntoDB(CassUuid experimentationUuid, const QString& name, CharacteristicValueTypes::Value valueType, const QStringList& enumValues)
{
    CharacteristicM* characteristic = nullptr;

    CassUuid characteristicUuid;
    cass_uuid_gen_time(AssessmentsModelManager::Instance()->getCassUuidGen(), &characteristicUuid);

    const char* query = "INSERT INTO ingescape.characteristic (id_experimentation, id, name, value_type, enum_values) VALUES (?, ?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(query, 5);
    cass_statement_bind_uuid  (cassStatement, 0, experimentationUuid);
    cass_statement_bind_uuid  (cassStatement, 1, characteristicUuid);
    cass_statement_bind_string(cassStatement, 2, name.toStdString().c_str());
    cass_statement_bind_int8  (cassStatement, 3, static_cast<int8_t>(valueType));
    cass_statement_bind_string(cassStatement, 4, enumValues.join(";").toStdString().c_str());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "New characteristic inserted into the DB";

        // Create the new task
        characteristic = new CharacteristicM(characteristicUuid, experimentationUuid, name, valueType, enumValues);

    }
    else {
        qCritical() << "Could not insert the new characteristic into the DB:" << cass_error_desc(cassError);
    }

    cass_statement_free(cassStatement);
    cass_future_free(cassFuture);

    return characteristic;
}


/**
 * @brief Creates a new subject with the given parameters an insert it into the Cassandra DB
 * a nullptr is returned if the operation fails
 * @param experimentationUuid
 * @param name
 * @return
 */
SubjectM* SubjectsController::_insertSubjectIntoDB(CassUuid experimentationUuid, const QString& displayed_id)
{
    SubjectM* subject = nullptr;

    CassUuid subjectUuid;
    cass_uuid_gen_time(AssessmentsModelManager::Instance()->getCassUuidGen(), &subjectUuid);

    const char* query = "INSERT INTO ingescape.subject (id_experimentation, id, displayed_id) VALUES (?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(query, 3);
    cass_statement_bind_uuid  (cassStatement, 0, experimentationUuid);
    cass_statement_bind_uuid  (cassStatement, 1, subjectUuid);
    cass_statement_bind_string(cassStatement, 2, displayed_id.toStdString().c_str());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "New subject inserted into the DB";

        // Create the new task
        subject = new SubjectM(experimentationUuid, subjectUuid, displayed_id);
    }
    else {
        qCritical() << "Could not insert the new subject into the DB:" << cass_error_desc(cassError);
    }

    cass_statement_free(cassStatement);
    cass_future_free(cassFuture);

    return subject;
}


/**
 * @brief Insert a new value for the given subject and characteristic into the DB
 * A default value is written in DB according to the characterystic's type
 * @param subject
 * @param characteristic
 */
void SubjectsController::_insertCharacteristicValueForSubjectIntoDB(SubjectM* subject, CharacteristicM* characteristic)
{
    if ((subject != nullptr) && (characteristic != nullptr))
    {
        const char* query = "INSERT INTO ingescape.characteristic_value_of_subject (id_experimentation, id_subject, id_characteristic, characteristic_value) VALUES (?, ?, ?, ?);";
        CassStatement* cassStatement = cass_statement_new(query, 4);
        cass_statement_bind_uuid  (cassStatement, 0, subject->getExperimentationCassUuid());
        cass_statement_bind_uuid  (cassStatement, 1, subject->getCassUuid());
        cass_statement_bind_uuid  (cassStatement, 2, characteristic->getCassUuid());
        const char* value;
        switch(characteristic->valueType())
        {
            case CharacteristicValueTypes::INTEGER:
                value = "0";
                break;
            case CharacteristicValueTypes::DOUBLE:
                value = "0.0";
                break;
            case CharacteristicValueTypes::TEXT:
            case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
                value = "";
                break;
            default:
                // Unknown characteristic value type
                value = "";
                break;
        }
        cass_statement_bind_string(cassStatement, 3, value);

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "New characteristic value inserted into the DB";
        }
        else {
            qCritical() << "Could not insert the new subject into the DB:" << cass_error_desc(cassError);
        }

        cass_statement_free(cassStatement);
        cass_future_free(cassFuture);
    }
}


/**
 * @brief Delete evert characteris value assciated with the given characteristic
 * FIXME Sending a request for each subject does not seem very efficient...
 *       It would be nive if we could just have a WHERE clause on id_experimentation and id_characteristic!
 * @param characteristic
 */
void SubjectsController::_deleteCharacteristicValuesForCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && (_currentExperimentation != nullptr))
    {
        for (auto subjectIt = _currentExperimentation->allSubjects()->begin() ; subjectIt != _currentExperimentation->allSubjects()->end() ; ++subjectIt)
        {
            SubjectM* subject = *subjectIt;
            if (subject != nullptr)
            {
                const char* query = "DELETE FROM ingescape.characteristic_value_of_subject WHERE id_experimentation = ? AND id_subject = ? AND id_characteristic = ;";
                CassStatement* cassStatement = cass_statement_new(query, 3);
                cass_statement_bind_uuid(cassStatement, 0, characteristic->getExperimentationCassUuid());
                cass_statement_bind_uuid(cassStatement, 1, subject->getCassUuid());
                cass_statement_bind_uuid(cassStatement, 2, characteristic->getCassUuid());

                // Execute the query or bound statement
                CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
                CassError cassError = cass_future_error_code(cassFuture);
                if (cassError == CASS_OK)
                {
                    qInfo() << "Characteristic values for characteristic" << characteristic->name() << "has been successfully deleted from the DB";
                }
                else {
                    qCritical() << "Could not delete the characteristic values for characteristic" << characteristic->name() << "from the DB:" << cass_error_desc(cassError);
                }

                // Clean-up cassandra objects
                cass_future_free(cassFuture);
                cass_statement_free(cassStatement);
            }
        }
    }
}

