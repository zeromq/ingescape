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

#include "ingescapemodelmanager.h"

/**
 * @brief Constructor
 * @param parent
 */
IngeScapeModelManager::IngeScapeModelManager(QObject *parent) : QObject(parent),
    _currentExperimentationsGroup(nullptr),
    _currentExperimentation(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Model Manager";

    // Agents grouped are sorted on their name (alphabetical order)
    //_allAgentsGroupsByName.setSortProperty("name");


    //
    // FIXME TESTS
    //

    //CharacteristicM *characteristicAge = new CharacteristicM("Age", CharacteristicValueTypes::INTEGER, this);


    QStringList enumBool = { tr("FALSE"), tr("TRUE") };
    //QStringList enumBool_French = { "FAUX", "VRAI" };

    QStringList enumYesNo = { tr("NO"), tr("YES") };
    //QStringList enumYesNo_French = { "NON", "OUI" };

    QStringList enumGender = { tr("NEUTRAL"), tr("WOMAN"), tr("MAN") };
    //QStringList enumGender_French = { "NEUTRE", "FEMME", "HOMME" };

    CharacteristicM *characteristicBool = new CharacteristicM("Booléen", CharacteristicValueTypes::CHARACTERISTIC_ENUM, this);
    characteristicBool->setenumValues(enumBool);

    CharacteristicM *characteristicYesNo = new CharacteristicM("Oui/Non", CharacteristicValueTypes::CHARACTERISTIC_ENUM, this);
    characteristicYesNo->setenumValues(enumYesNo);

    CharacteristicM *characteristicGender = new CharacteristicM("Genre", CharacteristicValueTypes::CHARACTERISTIC_ENUM, this);
    characteristicGender->setenumValues(enumGender);
}


/**
 * @brief Destructor
 */
IngeScapeModelManager::~IngeScapeModelManager()
{
    qInfo() << "Delete IngeScape Model Manager";

    if (_currentExperimentationsGroup != nullptr) {
        setcurrentExperimentationsGroup(nullptr);
    }

    if (_currentExperimentation != nullptr) {
        setcurrentExperimentation(nullptr);
    }

}
