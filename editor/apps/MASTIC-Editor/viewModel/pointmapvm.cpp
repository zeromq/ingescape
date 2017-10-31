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

#include "pointmapvm.h"

PointMapVM::PointMapVM(QString iopName, QObject *parent) : QObject(parent),
    _iopName(iopName),
    _position(QPointF())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "Create new PointMap VM" << _iopName;
}


PointMapVM::~PointMapVM()
{
    qInfo() << "Delete PointMap VM" << _iopName;
}
