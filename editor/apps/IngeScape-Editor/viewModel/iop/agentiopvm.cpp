/*
 *	IngeScape Editor
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

#include "agentiopvm.h"

/**
 * @brief Constructor
 * @param name
 * @param id
 * @param parent
 */
AgentIOPVM::AgentIOPVM(QString name,
                       QString id,
                       QObject *parent) : QObject(parent),
    _name(name),
    _id(id)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Agent I/O/P VM" << _name << "(" << _id << ")";
}


/**
 * @brief Destructor
 */
AgentIOPVM::~AgentIOPVM()
{
    //qInfo() << "Delete Agent I/O/P VM" << _name << "(" << _id << ")";
}
