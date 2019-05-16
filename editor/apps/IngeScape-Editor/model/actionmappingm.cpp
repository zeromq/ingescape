/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "actionmappingm.h"


/**
 * @brief Constructor
 * @param namesListOfOutputAgentInMappingAndOuputName
 * @param uidsListOfOutputActionsInMapping
 * @param parent
 */
ActionMappingM::ActionMappingM(QStringList namesListOfOutputAgentInMappingAndOuputName,
                               QList<int> uidsListOfOutputActionsInMapping,
                               QObject *parent) : QObject(parent),
    _namesListOfOutputAgentInMappingAndOuputName(namesListOfOutputAgentInMappingAndOuputName),
    _uidsListOfOutputActionsInMapping(uidsListOfOutputActionsInMapping)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}
