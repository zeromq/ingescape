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

        Q_EMIT newDefinitionInAgentMapping(this->agentName());
    }else
    {
        qInfo()<<"The definition of the agent named '"<<_agentName<<"' could not be add to the agent mapping VM named '"<<newDefinition->name()<<"'";
    }
}

void AgentInMappingVM::addPointMapInInternalList(DefinitionM *newDefinition,
                                                 I2CustomItemListModel<PointMapVM> *list)
{
    //TODOESTIA : voir si on peut passer direct la list plutot que I2CustomItemListModel<PointMapVM>

    if(newDefinition != NULL)
    {
        //
        // Create the list of Point Map
        //
        QList<PointMapVM*> listOfPointMapTemp;
        I2CustomItemListModel<AgentIOPM> *definitionIopList ;
        if(list->objectName() == _inputsList.objectName())
        {
            definitionIopList  = newDefinition->inputsList();
        }else
        {
            //TODOESTIA : voir avec Vincent pourquoi OutputM
//            definitionIopList  = newDefinition->outputsList();
            definitionIopList  = newDefinition->inputsList();
        }

        foreach (AgentIOPM* iopM, definitionIopList->toList())
        {
            if (iopM != NULL)
            {


                //Check if it's not alreafy exist in the list & add it
                if(!checkIfAlreadyInList(list->toList(),
                                         _agentName,
                                         iopM->name()))
                {
                    PointMapVM* pointMapVM = new PointMapVM(_agentName, iopM, this);

                    listOfPointMapTemp.append(pointMapVM);

                    //add to the map
                    if(list->objectName() == _inputsList.objectName())
                    {
                        _mapOfInputsFromInputName.insert(iopM->name(),
                                                         pointMapVM);
                    }else
                    {
                        _mapOfOutputsFromOutputName.insert(iopM->name(),
                                                         pointMapVM);
                    }

                    qInfo()<<"Add the new point in the list : "<<list->objectName();
                }else
                {
                    qInfo() << "This point map : " << _agentName << "." << iopM->name()<<" is already present in the list.";
                }
            }
        }
        list->append(listOfPointMapTemp);
    }
}

bool AgentInMappingVM::checkIfAlreadyInList(QList<PointMapVM *> list,
                                            QString agentName,
                                            QString iopName)
{
    foreach (PointMapVM* iterator, list)
    {
        // If this VM contains our model of agent
        if ( (iterator != NULL)
             &&
             // Same agent name
             (iterator->nameAgent() == agentName)
             &&
             // Same Input name
             (iterator->iopModel()->name() == iopName)
             )
        {

            // Exactly the same point ('agent name' & 'iop name')
            return true;
        }
    }

    return false;
}


PointMapVM * AgentInMappingVM::getPointMapFromInputName(QString inputName)
{
    return _mapOfInputsFromInputName.value(inputName);
}

PointMapVM * AgentInMappingVM::getPointMapFromOutputName(QString outputName)
{
    return _mapOfOutputsFromOutputName.value(outputName);
}

AgentInMappingVM::~AgentInMappingVM()
{
    qInfo() << "Delete View Model of AgentInMapping" << _agentName;

    //Clear the list of definition
    _definitionModelList.clear();

    //Delete element in the previous list Input & Output
    _mapOfInputsFromInputName.clear();
    _mapOfOutputsFromOutputName.clear();

    //Delete element in the list Input & Output
    _inputsList.deleteAllItems();
    _outputsList.deleteAllItems();
}
