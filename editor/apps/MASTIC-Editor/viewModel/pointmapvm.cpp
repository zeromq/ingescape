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
