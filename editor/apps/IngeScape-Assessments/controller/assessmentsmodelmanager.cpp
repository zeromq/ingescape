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

#include "assessmentsmodelmanager.h"

/**
 * @brief Constructor
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
AssessmentsModelManager::AssessmentsModelManager(JsonHelper* jsonHelper,
                                                 QString rootDirectoryPath,
                                                 QObject *parent) : IngeScapeModelManager(jsonHelper,
                                                                                          rootDirectoryPath,
                                                                                          parent),
    _currentExperimentationsGroup(nullptr),
    _currentExperimentation(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Assessments Model Manager";


    //
    // FIXME for tests
    //

    QStringList enumBool = { tr("FALSE"), tr("TRUE") };
    //QStringList enumBool_French = { "FAUX", "VRAI" };

    QStringList enumYesNo = { tr("NO"), tr("YES") };
    //QStringList enumYesNo_French = { "NON", "OUI" };

    QStringList enumGender = { tr("NEUTRAL"), tr("WOMAN"), tr("MAN") };
    //QStringList enumGender_French = { "NEUTRE", "FEMME", "HOMME" };

    CharacteristicM *characteristicBool = new CharacteristicM("Booléen", CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicBool->setenumValues(enumBool);

    CharacteristicM *characteristicYesNo = new CharacteristicM("Oui/Non", CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicYesNo->setenumValues(enumYesNo);

    CharacteristicM *characteristicGender = new CharacteristicM("Genre", CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicGender->setenumValues(enumGender);
}


/**
 * @brief Destructor
 */
AssessmentsModelManager::~AssessmentsModelManager()
{
    qInfo() << "Delete IngeScape Assessments Model Manager";

    if (_currentExperimentationsGroup != nullptr) {
        setcurrentExperimentationsGroup(nullptr);
    }

    if (_currentExperimentation != nullptr) {
        setcurrentExperimentation(nullptr);
    }

    // Mother class is automatically called
    //IngeScapeModelManager::~IngeScapeModelManager();
}
