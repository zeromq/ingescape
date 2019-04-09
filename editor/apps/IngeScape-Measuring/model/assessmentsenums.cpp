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

#include "assessmentsenums.h"

/**
 * @brief Enum "LogTypes" to string
 * @param value
 * @return
 */
QString CharacteristicValueTypes::enumToString(int value)
{
    switch (value)
    {
    case CharacteristicValueTypes::UNKNOWN:
        return tr("Unknown");

    case CharacteristicValueTypes::INTEGER:
        return tr("Integer");

    case CharacteristicValueTypes::DOUBLE:
        return tr("Double");

    case CharacteristicValueTypes::TEXT:
        return tr("Text");

    case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
        return tr("Enum");

    default:
        return "";
    }
}


//--------------------------------------------------------------
//
//  Measuring Enums
//
//--------------------------------------------------------------

/**
 * @brief Constructor
 * @param parent
 */
AssessmentsEnums::AssessmentsEnums(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}
