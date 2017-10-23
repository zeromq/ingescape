#include "agentinmappingvm.h"


AgentInMappingVM::AgentInMappingVM(DefinitionM *definitionModel,
                                   QPointF position,
                                   QObject *parent) : QObject(parent),
    _agentName(""),
    _position(position),
    _width(150),
    _height(70)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //Set the agent name
    if(definitionModel != NULL){

        //Get the name of the Agent
        _agentName = definitionModel->name();

        //Process the definition model to add the point Map
        addDefinitionInInternalList(definitionModel);
    }else
    {
        qCritical() << "The model of agent mapping is NULL !";
    }
}

AgentInMappingVM::AgentInMappingVM(QList<DefinitionM *> definitionModelList,
                                   QPointF position,
                                   QObject *parent) : QObject(parent),
    _agentName(""),
    _position(position),
    _width(150),
    _height(70)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //Set the agent name
    if(definitionModelList.first() != NULL){

        //Get the name of the AgentInMappingVM
        _agentName = definitionModelList.first()->name();

        //Process all definitions in the list
        foreach (DefinitionM * currentDefinition, definitionModelList) {
            addDefinitionInInternalList(currentDefinition);
        }
    }else
    {
        qCritical() << "The model of agent mapping is NULL !";
    }
}

void AgentInMappingVM::addDefinitionInInternalList(DefinitionM *newDefinition)
{
    //If it is the same agent in mapping
    if(newDefinition->name() == _agentName)
    {
        //Set the definition model
        _definitionModelList.append(newDefinition);

        //
        // Create the list of input (PointMapVM)
        //
        addPointMapInInternalList(newDefinition, &_inputsList);

        //
        // Create the list of output (PointMapVM)
        //
        addPointMapInInternalList(newDefinition, &_outputsList);
    }else
    {
        qInfo()<<"The definition of the agent named '"<<_agentName<<"' could not be add to the agent mapping VM named '"<<newDefinition->name()<<"'";
    }
}

void AgentInMappingVM::addPointMapInInternalList(DefinitionM *newDefinition,
                                                 I2CustomItemListModel<PointMapVM> *list)
{
    if(newDefinition != NULL)
    {
        //
        // Create the list of Point Map
        //
        QList<PointMapVM*> listOfPointMapTemp;
        foreach (AgentIOPM* inputM, newDefinition->inputsList()->toList())
        {
            if (inputM != NULL)
            {
                PointMapVM* inputPtMapVM = new PointMapVM(_agentName, inputM, this);

                //Check if it's not alreafy exist in the list & add it
                if(!checkIfAlreadyInList(list->toList(), inputPtMapVM))
                {
                    listOfPointMapTemp.append(inputPtMapVM);
                }
            }
        }
        list->append(listOfPointMapTemp);
    }
}

bool AgentInMappingVM::checkIfAlreadyInList(QList<PointMapVM *> list,
                                            PointMapVM *newPointMapVM)
{
    foreach (PointMapVM* iterator, list)
    {
        // If this VM contains our model of agent
        if ( (iterator != NULL)
             &&
             // Same agent name
             (iterator->nameAgent() == newPointMapVM->nameAgent())
             &&
             // Same Input name
             (iterator->iopModel()->name() == newPointMapVM->iopModel()->name())
             )
        {
            qDebug() << "There is exactly the same point map for agent name : " << newPointMapVM->nameAgent() << "and input name : " << newPointMapVM->iopModel()->name();

            // Exactly the same point ('agent name' & 'iop name')
            return true;
        }
    }

    return false;
}

AgentInMappingVM::~AgentInMappingVM()
{
    qInfo() << "Delete View Model of AgentInMapping" << _agentName;

    //Clear the list of definition
    _definitionModelList.clear();

    //Delete element in the previous list Input & Output
    _previousInputsList.clear();
    _previousOutputsList.clear();

    //Delete element in the list Input & Output
    _inputsList.deleteAllItems();
    _outputsList.deleteAllItems();
}
