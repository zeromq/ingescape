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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "mapbetweeniopvm.h"


#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param agentFrom The link starts from this agent
 * @param pointFrom The link starts from this output of the agentFrom
 * @param agentTo The link ends to this agent
 * @param pointTo The link ends to this input of the agentTo
 * @param parent
 */
MapBetweenIOPVM::MapBetweenIOPVM(AgentInMappingVM* agentFrom,
                                 OutputVM *pointFrom,
                                 AgentInMappingVM* agentTo,
                                 InputVM *pointTo,
                                 QObject *parent) : QObject(parent),
    _agentFrom(NULL),
    _pointFrom(NULL),
    _agentTo(NULL),
    _pointTo(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Allow to benefit of "DELETE PROOF"
    setagentFrom(agentFrom);
    setpointFrom(pointFrom);
    setagentTo(agentTo);
    setpointTo(pointTo);

    if ((_agentFrom != NULL) && (_pointFrom != NULL) && (_agentTo != NULL) && (_pointTo != NULL))
    {
        qInfo() << "Create new MapBetweenIOPVM " << _agentFrom->name() << "." << _pointFrom->name() << "-->" << _agentTo->name() << "." << _pointTo->name();
    }
}


/**
 * @brief Destructor
 */
MapBetweenIOPVM::~MapBetweenIOPVM()
{
    if ((_agentFrom != NULL) && (_pointFrom != NULL) && (_agentTo != NULL) && (_pointTo != NULL))
    {
        qInfo() << "Delete MapBetweenIOPVM " << _agentFrom->name() << "." << _pointFrom->name() << "-->" << _agentTo->name() << "." << _pointTo->name();
    }

    if (_agentFrom != NULL)
    {
        AgentInMappingVM* temp = _agentFrom;
        setagentFrom(NULL);

        // Handle ghost agent
        if (temp->isGhost())
        {
            delete temp;
        }
    }

    if (_pointFrom != NULL)
    {
        OutputVM* temp = _pointFrom;
        setpointFrom(NULL);

        // Handle ghost output
        if (temp->isGhost())
        {
            delete temp;
        }
    }

    setagentTo(NULL);
    setpointTo(NULL);
}

