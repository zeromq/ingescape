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

#include "characteristicm.h"

/**
 * @brief Constructor
 * @param parent
 */
CharacteristicM::CharacteristicM(QObject *parent) : QObject(parent),
    _name(""),
    _valueType(CharacteristicValueTypes::UNKNOWN),
    _enumValues(QStringList())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Characteristic" << _name;
}


/**
 * @brief Destructor
 */
CharacteristicM::~CharacteristicM()
{
    qInfo() << "Delete Model of Characteristic" << _name;

}
