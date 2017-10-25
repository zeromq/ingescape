#include "pointmapvm.h"

PointMapVM::PointMapVM(QString nameAgent, AgentIOPM *iopModel, QObject *parent) : QObject(parent),
    _nameAgent(nameAgent),
    _iopModel(NULL),
    _position(QPointF()),
    _radius(10)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Allow to benefit of "DELETE PROOF"
    setiopModel(iopModel);

    if (_iopModel != NULL) {
        qInfo() << "Create new PointMap VM" << _nameAgent << "." << _iopModel->name();
    }

}


PointMapVM::~PointMapVM()
{
    if (_iopModel != NULL) {
        qInfo() << "Delete PointMap VM" << _nameAgent << "." << _iopModel->name();
    }
}
