#include "pointmapvm.h"

PointMapVM::PointMapVM(QString nameAgent, AgentIOPM *iopModel, QObject *parent) : QObject(parent),
    _nameAgent(nameAgent),
    _iopModel(iopModel),
    _position(QPointF()),
    _radius(10)
{
    // Force ownership of our object, it will prevent Qml from stealing it
       QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

       qInfo() << "Create new PointMap VM" << _nameAgent << "." << _iopModel->name();

}


PointMapVM::~PointMapVM()
{
    qInfo() << "Delete PointMap VM" << _nameAgent << "." << _iopModel->name();
}
