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
void SubjectsController::createNewCharacteristic(QString characteristicName, int nCharacteristicValueType)
{
    if (!characteristicName.isEmpty() && (nCharacteristicValueType > -1) && (_currentExperimentation != nullptr))
    {
        // FIXME TODO createNewCharacteristic

        /*CharacteristicValueTypes::Value characteristicValueType = static_cast<CharacteristicValueTypes::Value>(nCharacteristicValueType);

        //qInfo() << "Create new characteristic" << characteristicName << "of type" << CharacteristicValueTypes::staticEnumToString(characteristicValueType);

        // Create the new characteristic
        CharacteristicM* characteristic = new CharacteristicM(characteristicName, characteristicValueType);

        // Add the characteristic to the current experimentation
        _currentExperimentation->addCharacteristic(characteristic);*/
    }
}


/**
 * @brief Create a new characteristic of type enum
 * @param characteristicName
 * @param enumValues
 */
void SubjectsController::createNewCharacteristicEnum(QString characteristicName, QStringList enumValues)
{
    if (!characteristicName.isEmpty() && !enumValues.isEmpty() && (_currentExperimentation != nullptr))
    {
        // FIXME TODO createNewCharacteristicEnum

        //qInfo() << "Create new characteristic" << characteristicName << "of type" << CharacteristicValueTypes::staticEnumToString(CharacteristicValueTypes::CHARACTERISTIC_ENUM) << "with values:" << enumValues;

        /*// Create the new characteristic
        CharacteristicM* characteristic = new CharacteristicM(characteristicName, CharacteristicValueTypes::CHARACTERISTIC_ENUM);
        characteristic->setenumValues(enumValues);

        // Add the characteristic to the current experimentation
        _currentExperimentation->addCharacteristic(characteristic);*/
    }
}


/**
 * @brief Delete a characteristic
 * @param characteristic
 */
void SubjectsController::deleteCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && (_currentExperimentation != nullptr))
    {
        // Remove the characteristic from the current experimentation
        _currentExperimentation->removeCharacteristic(characteristic);

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

        CassUuid subjectUid;
        cass_uuid_gen_time(AssessmentsModelManager::Instance()->getCassUuidGen(), &subjectUid);

        // Create a new subject
        SubjectM* subject = new SubjectM(subjectUid, displayedId, nullptr);

        // For each existing characteristic
        for (CharacteristicM* characteristic : _currentExperimentation->allCharacteristics()->toList())
        {
            if (characteristic != nullptr)
            {
                subject->addCharacteristic(characteristic);
            }
        }

        // Add the subject to the current experimentation
        _currentExperimentation->addSubject(subject);
    }
}


/**
 * @brief Delete a subject
 * @param subject
 */
void SubjectsController::deleteSubject(SubjectM* subject)
{
    if ((subject != nullptr) && (_currentExperimentation != nullptr))
    {
        // Remove the subject from the current experimentation
        _currentExperimentation->removeSubject(subject);

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
                        CharacteristicM* characteristic = new CharacteristicM(characteristicUid, characteristicName, characteristicValueType, nullptr);

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
                        char chrCharacteristicUid[CASS_UUID_STRING_LENGTH];
                        cass_uuid_string(characteristicUid, chrCharacteristicUid);

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
                            characteristic = new CharacteristicM(characteristicUid, characteristicName, characteristicValueType, nullptr);

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

