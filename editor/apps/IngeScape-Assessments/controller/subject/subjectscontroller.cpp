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
#include "model/subject/characteristicvaluem.h"


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

    // Fill without type "UNKNOWN" and "ENUM"
    _allCharacteristicValueTypes.fillWithAllEnumValues();
    _allCharacteristicValueTypes.removeEnumValue(CharacteristicValueTypes::UNKNOWN);
    _allCharacteristicValueTypes.removeEnumValue(CharacteristicValueTypes::CHARACTERISTIC_ENUM);

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
        // Remove the characteristic values linked to soon-to-be-removed characteistic
        _deleteCharacteristicValuesForCharacteristic(characteristic);

        // Remove the characteristic from the current experimentation
        _currentExperimentation->removeCharacteristic(characteristic);

        // Remove characteristic from DB
        AssessmentsModelManager::deleteEntry<CharacteristicM>({ _currentExperimentation->getCassUuid(), characteristic->getCassUuid() });

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

        // Remove task instances related to the subject
        QList<CassUuid> taskUuidList;
        for (TaskM* task : *(_currentExperimentation->allTasks())) {
            if (task != nullptr)
            {
                taskUuidList.append(task->getCassUuid());
            }
        }

        AssessmentsModelManager::deleteEntry<TaskInstanceM>({ { _currentExperimentation->getCassUuid() }, { subject->getCassUuid() }, taskUuidList });


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

        // Retrieve the experimentation's characteristics
        QList<CharacteristicM*> characteristicList = AssessmentsModelManager::select<CharacteristicM>({ currentExperimentation->getCassUuid() });

        if (characteristicList.isEmpty())
        {
            qDebug() << "There is NO characteristic...create the special characteristic 'id'";

            // Create the new characteristic
            CharacteristicM* subjectIdCharacteristic = new CharacteristicM(AssessmentsModelManager::genCassUuid(), currentExperimentation->getCassUuid(), CHARACTERISTIC_SUBJECT_ID, CharacteristicValueTypes::TEXT);

            if (subjectIdCharacteristic != nullptr)
            {
                // Insert the new characteristic into DB
                AssessmentsModelManager::insert(*subjectIdCharacteristic);

                // Add the characteristic to the current experimentation
                _currentExperimentation->addCharacteristic(subjectIdCharacteristic);
            }
        }
        else
        {
            qDebug() << "There are" << characteristicList.size() << "characteristics";

            for (CharacteristicM* characteristic : characteristicList)
            {
                // It does not yet exist
                if (characteristic != nullptr)
                {
                    // Add the characteristic to the current experimentation
                    _currentExperimentation->addCharacteristic(characteristic);
                }
            }
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
    CharacteristicM* characteristic = new CharacteristicM(AssessmentsModelManager::genCassUuid(), experimentationUuid, name, valueType, enumValues);
    if (characteristic == nullptr || !AssessmentsModelManager::insert(*characteristic))
    {
        delete characteristic;
        characteristic = nullptr;
    }

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
    SubjectM* subject = new SubjectM(experimentationUuid, AssessmentsModelManager::genCassUuid(), displayed_id);
    if (subject == nullptr || !AssessmentsModelManager::insert(*subject))
    {
        delete subject;
        subject = nullptr;
    }

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
        QString valueString;
        switch(characteristic->valueType())
        {
            case CharacteristicValueTypes::INTEGER:
                valueString = "0";
                break;
            case CharacteristicValueTypes::DOUBLE:
                valueString = "0.0";
                break;
            case CharacteristicValueTypes::TEXT:
            case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
                valueString = "";
                break;
            default:
                // Unknown characteristic value type
                valueString = "";
                break;
        }

        AssessmentsModelManager::insert(CharacteristicValueM(subject->getExperimentationCassUuid(),
                                                             subject->getCassUuid(),
                                                             characteristic->getCassUuid(),
                                                             valueString));
    }
}


/**
 * @brief Delete evert characteris value assciated with the given characteristic
 * @param characteristic
 */
void SubjectsController::_deleteCharacteristicValuesForCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && (_currentExperimentation != nullptr))
    {
        // Remove task instances related to the task
        QList<CassUuid> subjectUuidList;
        for (SubjectM* subject : _currentExperimentation->allSubjects()->toList()) {
            if (subject != nullptr)
            {
                subjectUuidList.append(subject->getCassUuid());
            }
        }

        AssessmentsModelManager::deleteEntry<CharacteristicValueM>({ { _currentExperimentation->getCassUuid() }, subjectUuidList, { characteristic->getCassUuid() } });
    }
}

