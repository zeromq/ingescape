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

#include "clonedagentvm.h"

/**
 * @brief Constructor
 * @param name
 * @param parent
 */
ClonedAgentVM::ClonedAgentVM(QString name,
                             QObject *parent) : AgentVM(NULL, parent),
    _name(name)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New View Model of CLONED Agent" << _name;
}


/**
 * @brief Destructor
 */
ClonedAgentVM::~ClonedAgentVM()
{
    qInfo() << "Delete View Model of CLONED Agent" << _name;

    _models.clear();
}
