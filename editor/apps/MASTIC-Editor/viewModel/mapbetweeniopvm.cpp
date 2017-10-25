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
 * @brief Constructor
 * @param pointFrom
 * @param pointTo
 * @param parent
 */
MapBetweenIOPVM::MapBetweenIOPVM(PointMapVM *pointFrom, PointMapVM *pointTo, QObject *parent) : QObject(parent),
    _pointFrom(NULL),
    _pointTo(NULL),
    _isNewValueOnOutput(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setpointFrom(pointFrom);
    setpointTo(pointTo);

    if ((_pointFrom != NULL) && (_pointTo != NULL))
    {
        qInfo() << "Map Between "<<_pointFrom->iopModel()->name()<<"->"<<_pointTo->iopModel()->name()<<"created";
    }
    else
    {
        qWarning() << "Invalid map";
    }
}


/**
 * @brief Constructor
 * @param pointFrom
 * @param parent
 */
/*MapBetweenIOPVM::MapBetweenIOPVM(PointMapVM *pointFrom, QObject *parent) : QObject(parent),
    _pointFrom(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setpointFrom(pointFrom);

    if (_pointFrom != NULL)
    {
        qInfo() << "Map Between "<<_pointFrom->iopModel()->name()<<"created";
    }
    else
    {
        qWarning() << "Invalid map";
    }
}*/


/**
 * @brief Default constructor
 * @param agentFrom The link starts from this agent
 * @param pointFrom The link starts from this output of the agentFrom
 * @param agentTo The link ends to this agent
 * @param pointTo The link ends to this input of the agentTo
 * @param parent
 */
MapBetweenIOPVM::MapBetweenIOPVM(AgentInMappingVM* agentFrom,
                                 PointMapVM *pointFrom,
                                 AgentInMappingVM* agentTo,
                                 PointMapVM *pointTo,
                                 QObject *parent) : QObject(parent),
    _agentFrom(NULL),
    _pointFrom(NULL),
    _agentTo(NULL),
    _pointTo(NULL),
    _isNewValueOnOutput(false)
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
        //qInfo() << "New Map Between" << _agentFrom->agentName() << "." << _pointFrom->iopModel()->name() << "-->" << _agentTo->agentName() << "." << _pointTo->iopModel()->name();
        qInfo() << "New Map Between" << _agentFrom->agentName() << "." << "TODO _pointFrom OutputVM" << "-->" << _agentTo->agentName() << "." << "TODO _pointTo InputVM";
    }
}


/**
 * @brief Destructor
 */
MapBetweenIOPVM::~MapBetweenIOPVM()
{
    qInfo() << "Map destroyed";

    setagentFrom(NULL);
    setpointFrom(NULL);
    setagentTo(NULL);
    setpointTo(NULL);
}
