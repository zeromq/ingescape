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
 * @param parent
 */
ClonedAgentVM::ClonedAgentVM(QObject *parent) : AgentVM(NULL, parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}



/**
 * @brief Destructor
 */
ClonedAgentVM::~ClonedAgentVM()
{

}
