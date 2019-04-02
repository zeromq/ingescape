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

#include "subjectscontroller.h"


/**
 * @brief Constructor
 * @param parent
 */
SubjectsController::SubjectsController(IngeScapeModelManager* modelManager,
                                       QObject *parent) : QObject(parent),
    _characteristicValueTypeEnum(CharacteristicValueTypes::CHARACTERISTIC_ENUM),
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

        qInfo() << "Create new characteristic" << characteristicName << "of type" << CharacteristicValueTypes::staticEnumToString(characteristicValueType);

        // Create the new characteristic
        CharacteristicM* characteristic = new CharacteristicM(characteristicName, characteristicValueType, nullptr);

        // Add the characteristic to the experimentation
        _modelManager->currentExperimentation()->allCharacteristics()->append(characteristic);
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
        // Remove from the experimentation
        _modelManager->currentExperimentation()->allCharacteristics()->remove(characteristic);

        // Free memory
        delete characteristic;
    }
}
