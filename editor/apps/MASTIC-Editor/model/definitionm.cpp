/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "definitionm.h"

/**
 * @brief Constructor
 * @param name
 * @param version
 * @param description
 * @param parent
 */
DefinitionM::DefinitionM(QString name,
                         QString version,
                         QString description,
                         QObject *parent) : QObject(parent),
    _name(name),
    _version(version),
    _description(description),
    _variant("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent Definition" << _name << "with version" << _version << "about" << _description;
}


/**
 * @brief Destructor
 */
DefinitionM::~DefinitionM()
{
    qInfo() << "Delete Model of Agent Definition" << _name;
}
