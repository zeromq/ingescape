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


/**
 * @brief Constructor
 * @param modelManager
 * @param parent
 */
SubjectsController::SubjectsController(IngeScapeModelManager* modelManager,
                                       QObject *parent) : QObject(parent),
    _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Subjects Controller";

    // Fill without type "UNKNOWN"
    _allCharacteristicValueTypes.fillWithAllEnumValues();
    _allCharacteristicValueTypes.removeEnumValue(CharacteristicValueTypes::UNKNOWN);

    /*if (_modelManager != nullptr)
    {
        connect(_modelManager, &IngeScapeModelManager::currentExperimentationChanged, this, &SubjectsController::onCurrentExperimentationChanged);
    }*/
}


/**
 * @brief Destructor
 */
SubjectsController::~SubjectsController()
{
    qInfo() << "Delete Subjects Controller";

    if (_modelManager != nullptr)
    {
        //disconnect(_modelManager, nullptr, this, nullptr);

        _modelManager = nullptr;
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
    if (!characteristicName.isEmpty()
            && (_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        for (CharacteristicM* characteristic : _modelManager->currentExperimentation()->allCharacteristics()->toList())
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
    if (!characteristicName.isEmpty() && (nCharacteristicValueType > -1)
            && (_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        CharacteristicValueTypes::Value characteristicValueType = static_cast<CharacteristicValueTypes::Value>(nCharacteristicValueType);

        //qInfo() << "Create new characteristic" << characteristicName << "of type" << CharacteristicValueTypes::staticEnumToString(characteristicValueType);

        // Create the new characteristic
        CharacteristicM* characteristic = new CharacteristicM(characteristicName, characteristicValueType);

        // Add the characteristic to the experimentation
        _modelManager->currentExperimentation()->addCharacteristic(characteristic);
    }
}


/**
 * @brief Create a new characteristic of type enum
 * @param characteristicName
 * @param enumValues
 */
void SubjectsController::createNewCharacteristicEnum(QString characteristicName, QStringList enumValues)
{
    if (!characteristicName.isEmpty() && !enumValues.isEmpty()
            && (_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        qInfo() << "Create new characteristic" << characteristicName << "of type" << CharacteristicValueTypes::staticEnumToString(CharacteristicValueTypes::CHARACTERISTIC_ENUM) << "with values:" << enumValues;

        // Create the new characteristic
        CharacteristicM* characteristic = new CharacteristicM(characteristicName, CharacteristicValueTypes::CHARACTERISTIC_ENUM);
        characteristic->setenumValues(enumValues);

        // Add the characteristic to the experimentation
        _modelManager->currentExperimentation()->addCharacteristic(characteristic);
    }
}


/**
 * @brief Delete a characteristic
 * @param characteristic
 */
void SubjectsController::deleteCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr)
            && (_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        // Remove the characteristic from the experimentation
        _modelManager->currentExperimentation()->removeCharacteristic(characteristic);

        // Free memory
        delete characteristic;
    }
}


/**
 * @brief Create a new subject
 */
void SubjectsController::createNewSubject()
{
    if ((_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        QDateTime now = QDateTime::currentDateTime();

        QString subjectUID = now.toString("S-yyMMdd-hhmmss-zzz");

        // Create a new subject
        SubjectM* subject = new SubjectM(subjectUID, nullptr);

        // For each existing characteristic
        for (CharacteristicM* characteristic : _modelManager->currentExperimentation()->allCharacteristics()->toList())
        {
            if (characteristic != nullptr)
            {
                subject->addCharacteristic(characteristic);
            }
        }

        _modelManager->currentExperimentation()->allSubjects()->append(subject);
    }
}


/**
 * @brief Delete a subject
 * @param subject
 */
void SubjectsController::deleteSubject(SubjectM* subject)
{
    if ((subject != nullptr)
            && (_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        // Remove from the experimentation
        _modelManager->currentExperimentation()->allSubjects()->remove(subject);

        // Free memory
        delete subject;
    }
}
