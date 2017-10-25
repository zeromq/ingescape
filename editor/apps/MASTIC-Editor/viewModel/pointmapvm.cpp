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

PointMapVM::PointMapVM(QString nameAgent, QObject *parent) : QObject(parent),
    _nameAgent(nameAgent),
    _position(QPointF())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "Create new PointMap VM" << _nameAgent;
}


PointMapVM::~PointMapVM()
{
    qInfo() << "Delete PointMap VM" << _nameAgent;
}
