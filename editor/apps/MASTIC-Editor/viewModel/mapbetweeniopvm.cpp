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
    _pointTo(NULL)
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
MapBetweenIOPVM::MapBetweenIOPVM(PointMapVM *pointFrom, QObject *parent) : QObject(parent),
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
}



/**
 * @brief Destructor
 */
MapBetweenIOPVM::~MapBetweenIOPVM()
{
    qInfo() << "Map destroyed";
}
