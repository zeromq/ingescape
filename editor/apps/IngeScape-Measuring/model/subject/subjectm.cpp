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

#include "subjectm.h"

/**
 * @brief Constructor
 * @param parent
 */
SubjectM::SubjectM(QObject *parent) : QObject(parent),
    _name("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Subject" << _name;

    _mapFromCharacteristicIdToValue.insert("age", QVariant(35));
    _mapFromCharacteristicIdToValue.insert("type", QVariant("HOMME"));
}


/**
 * @brief Destructor
 */
SubjectM::~SubjectM()
{
    qInfo() << "Delete Model of Subject" << _name;

}
