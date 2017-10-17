#include "agentinmappingvm.h"


AgentInMappingVM::AgentInMappingVM(AgentMappingM *model, QObject *parent) : QObject(parent),
    _name(""),
    _x_center(0),
    _y_center(0),
    _width(150),
    _height(70)
{
    // Force ownership of our object, it will prevent Qml from stealing it
      QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

      if (model != NULL)
      {
          // Init the name
          _name = model->name();

//          QList<PointMapVM*> * temp = new QList<PointMapVM*>();
//          foreach (ElementMappingM * elmt, model->elementMappingsList()) {
//              PointMapVM* pointMap = new PointMapVM();
//          }
          // Add to the list
          _models.append(model);
      }
      else
      {
          qCritical() << "The model of agent mapping is NULL !";
      }
}

AgentInMappingVM::~AgentInMappingVM()
{
    _previousAgentsMappingList.clear();
    _models.clear();
}
