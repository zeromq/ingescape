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
 * @brief Default constructor
 * @param parent
 */
MapBetweenIOPVM::MapBetweenIOPVM(AgentIOPVM *pointFrom, AgentIOPVM *pointTo, QObject *parent) : QObject(parent),
    _pointFrom(pointFrom),
    _pointTo(pointTo)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


    qInfo() << "Map created";
}



/**
 * @brief Destructor
 */
MapBetweenIOPVM::~MapBetweenIOPVM()
{
    qInfo() << "Map destroyed";
}
