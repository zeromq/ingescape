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

MapBetweenIOPVM::MapBetweenIOPVM(PointMapVM *pointFrom, PointMapVM *pointTo, QObject *parent) : QObject(parent),
    _pointFrom(pointFrom),
    _pointTo(pointTo)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "Map Between "<<_pointFrom->iopModel()->name()<<"->"<<_pointTo->iopModel()->name()<<"created";
}

MapBetweenIOPVM::MapBetweenIOPVM(PointMapVM *pointFrom, QObject *parent) : QObject(parent),
    _pointFrom(pointFrom)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "Map Between "<<_pointFrom->iopModel()->name()<<"created";
}

MapBetweenIOPVM::~MapBetweenIOPVM()
{
    qInfo() << "Map destroyed";
}
