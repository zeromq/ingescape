#include "pointmapvm.h"

PointMapVM::PointMapVM(AgentIOPM *model, QObject *parent) : QObject(parent),
    _nameAgent(""),
    _modelM(model),
    _x_center(0),
    _y_center(0),
    _radius(10)
{
    // Force ownership of our object, it will prevent Qml from stealing it
       QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

       qInfo() << "Create VM of PointMap" << _modelM->name() << " of " << _nameAgent << "." << _modelM->name() << "'";

}


PointMapVM::~PointMapVM()
{
    qInfo() << "Delete VM of PointMap" << _modelM->name() << " of " << _nameAgent << "." << _modelM->name() << "'";
    setmodelM(NULL);
}
