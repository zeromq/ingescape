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
 * @param uid
 * @param parent
 */
SubjectM::SubjectM(QString uid,
                   QObject *parent) : QObject(parent),
    _uid(uid),
    _name("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Subject" << _uid;

    _mapFromCharacteristicIdToValue.insert("age", QVariant(35));
    _mapFromCharacteristicIdToValue.insert("type", QVariant("HOMME"));
}


/**
 * @brief Destructor
 */
SubjectM::~SubjectM()
{
    qInfo() << "Delete Model of Subject" << _uid;

}
