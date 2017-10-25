#include "agentinmappingvm.h"

/**
     * @brief Default constructor
     * @param definitionModel The first definition model needed to instanciate an agent mapping VM.
     * Typically passing during the drag-drop from the list of agent on the left side.
     * @param position Position of the box
     * @param parent
     */
AgentInMappingVM::AgentInMappingVM(DefinitionM *definitionModel,
                                   QPointF position,
                                   QObject *parent) : QObject(parent),
    _agentName(""),
    _position(position),
    _width(150),
    _height(70),
    _isON(false),
    _isReduced(false),
    _reducedMapValueTypeInInput(AgentIOPValueTypes::MIXED),
    _reducedMapValueTypeInOutput(AgentIOPValueTypes::MIXED)
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

/**
     * @brief Second constructor to instanciate from a list of definition
     * @param definitionModelList The definition model list needed to instanciate an agent mapping VM.
     * Typically passing during the drag-drop from the list of agent on the left side.
     * @param position Position of the box
     * @param parent
     */
AgentInMappingVM::AgentInMappingVM(QList<DefinitionM *> definitionModelList,
                                   QPointF position,
                                   QObject *parent) : QObject(parent),
    _agentName(""),
    _position(position),
    _width(150),
    _height(70),
    _isON(false),
    _isReduced(false),
    _reducedMapValueTypeInInput(AgentIOPValueTypes::MIXED),
    _reducedMapValueTypeInOutput(AgentIOPValueTypes::MIXED)
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

/**
     * @brief Add definition dynamically to the internal list
     * @param newDefinition The definition to add
     */
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
        addPointMapInInternalInputList(newDefinition);

        //
        // Create the list of output (PointMapVM)
        //
        // addPointMapInInternalOutputList(newDefinition);

        Q_EMIT newDefinitionInAgentMapping(this->agentName());
    }else
    {
        qInfo()<<"The definition of the agent named '"<<_agentName<<"' could not be add to the agent mapping VM named '"<<newDefinition->name()<<"'";
    }
}


/**
     * @brief Add new points Map to the inputs list from a definition model
     * @param newDefinition The definition model
     */
void AgentInMappingVM::addPointMapInInternalInputList(DefinitionM *newDefinition)
{
    //TODOESTIA : voir si on peut passer direct la list plutot que I2CustomItemListModel<PointMapVM>

    if(newDefinition != NULL)
    {
        // Create the list of Point Map to add in the agent in mapping
        QList<PointMapVM*> listOfPointMapTemp;

        foreach (AgentIOPM* iopM, newDefinition->inputsList()->toList())
        {
            if (iopM != NULL)
            {
                //Check if it's not alreafy exist in the list & add it
                if(!checkIfAlreadyInList(_inputsList.toList(), _agentName, iopM->name()))
                {
                    // New pointMapVM (from agent and input model
                    PointMapVM* pointMapVM = new PointMapVM(_agentName, iopM, this);

                    // Add to the list of newly created pointMap
                    listOfPointMapTemp.append(pointMapVM);

                    // Update the hash table
                    _mapOfInputsFromInputName.insert(iopM->name(), pointMapVM);

                    qInfo()<<"Add new point map " << _agentName << "." << iopM->name() << " in the inputs list.";

                }else
                {
                    qInfo() << "Point map : " << _agentName << "." << iopM->name() << " is already present in the list.";
                }
            }
        }
        _inputsList.append(listOfPointMapTemp);
    }
}


/**
     * @brief Add new points Map to the outputs list from a definition model
     * @param newDefinition The definition model
     */
void AgentInMappingVM::addPointMapInInternalOutputList(DefinitionM *newDefinition)
{
    if(newDefinition != NULL)
    {

        // TODO ESTIA : AWAITING FOR TWO NEW DAUGHTERS CLASSES FROM POINTMAPVM

//        // Create the list of Point Map to add in the agent in mapping
//        QList<PointMapVM*> listOfPointMapTemp;

//        // Get the input list from the model
//        I2CustomItemListModel<AgentIOPM> *definitionIopList = newDefinition->outputsList();

//        foreach (AgentIOPM* iopM, definitionIopList->toList())
//        {
//            if (iopM != NULL)
//            {
//                //Check if it's not alreafy exist in the list & add it
//                if(!checkIfAlreadyInList(list->toList(), _agentName, iopM->name()))
//                {
//                    // New pointMapVM (from agent and input model
//                    PointMapVM* pointMapVM = new PointMapVM(_agentName, iopM, this);

//                    // Add to the list of newly created pointMap
//                    listOfPointMapTemp.append(pointMapVM);

//                    // Update the hash table
//                    _mapOfInputsFromInputName.insert(iopM->name(), pointMapVM);

//                    qInfo()<<"Add new point map " << _agentName << "." << iopM->name() << " in the inputs list.";

//                }else
//                {
//                    qInfo() << "Point map : " << _agentName << "." << iopM->name() << " is already present in the list.";
//                }
//            }
//        }
//        _outputsList->append(listOfPointMapTemp);
    }
}


/**
     * @brief This function check if the point map already exist in the list (input/output list)
     * @param list The list where to add the points map (input/output list)
     * @param agentName The name of the agent mapping VM
     * @param iopName The name of the input/output to add
     */
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

/**
     * @brief Return the corresponding PointMap from the input IOP name
     * @param inputName
     */
PointMapVM * AgentInMappingVM::getPointMapFromInputName(QString inputName)
{
    return _mapOfInputsFromInputName.value(inputName);
}

/**
     * @brief Return the corresponding PointMap from the output IOP name
     * @param outputName
     */
PointMapVM * AgentInMappingVM::getPointMapFromOutputName(QString outputName)
{
    return _mapOfOutputsFromOutputName.value(outputName);
}

/**
 * @brief Destructor
 */
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
