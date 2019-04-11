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

#include "characteristicm.h"

/**
 * @brief Constructor
 * @param name
 * @param valueType
 * @param canBeDeleted
 * @param parent
 */
CharacteristicM::CharacteristicM(QString name,
                                 CharacteristicValueTypes::Value valueType,
                                 bool canBeDeleted,
                                 QObject *parent) : QObject(parent),
    _name(name),
    _valueType(valueType),
    _canBeDeleted(canBeDeleted),
    _enumValues(QStringList())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Characteristic" << _name << "of type" << CharacteristicValueTypes::staticEnumToString(_valueType);
}


/**
 * @brief Destructor
 */
CharacteristicM::~CharacteristicM()
{
    qInfo() << "Delete Model of Characteristic" << _name << "of type" << CharacteristicValueTypes::staticEnumToString(_valueType);

}
