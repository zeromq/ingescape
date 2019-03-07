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
IngeScapeModelManager::IngeScapeModelManager(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Model Manager";

    // Agents grouped are sorted on their name (alphabetical order)
    //_allAgentsGroupsByName.setSortProperty("name");


    //
    // FIXME TESTS
    //

    CharacteristicM *characteristicAge = new CharacteristicM(this);
    characteristicAge->setname("Age");
    characteristicAge->setvalueType(CharacteristicValueTypes::INTEGER);


    QStringList enumBool = { tr("FALSE"), tr("TRUE") };
    //QStringList enumBool_French = { "FAUX", "VRAI" };

    QStringList enumYesNo = { tr("NO"), tr("YES") };
    //QStringList enumYesNo_French = { "NON", "OUI" };

    QStringList enumGender = { tr("NEUTRAL"), tr("WOMAN"), tr("MAN") };
    //QStringList enumGender_French = { "NEUTRE", "FEMME", "HOMME" };

    CharacteristicM *characteristicBool = new CharacteristicM(this);
    characteristicBool->setname("Booléen");
    characteristicBool->setvalueType(CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicBool->setenumValues(enumBool);

    CharacteristicM *characteristicYesNo = new CharacteristicM(this);
    characteristicYesNo->setname("Oui/Non");
    characteristicYesNo->setvalueType(CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicYesNo->setenumValues(enumYesNo);

    CharacteristicM *characteristicGender = new CharacteristicM(this);
    characteristicGender->setname("Genre");
    characteristicGender->setvalueType(CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicGender->setenumValues(enumGender);
}


/**
 * @brief Destructor
 */
IngeScapeModelManager::~IngeScapeModelManager()
{
    qInfo() << "Delete IngeScape Model Manager";

}
