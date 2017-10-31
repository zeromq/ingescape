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
    _isON(false),
    _isReduced(false),
    _reducedMapValueTypeInInput(AgentIOPValueTypes::MIXED),
    _reducedMapValueTypeInOutput(AgentIOPValueTypes::MIXED),
    _isGhost(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //Set the agent name
    if(definitionModel != NULL){

        //Get the name of the Agent
        _agentName = definitionModel->name();

        //Process the definition model to add the point Map
        addDefinitionInInternalList(definitionModel);
    }
    else {
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
    _isON(false),
    _isReduced(false),
    _reducedMapValueTypeInInput(AgentIOPValueTypes::MIXED),
    _reducedMapValueTypeInOutput(AgentIOPValueTypes::MIXED),
    _isGhost(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //Set the agent name
    if(definitionModelList.first() != NULL) {

        //Get the name of the AgentInMappingVM
        _agentName = definitionModelList.first()->name();

        //Process all definitions in the list
        foreach (DefinitionM * currentDefinition, definitionModelList) {
            addDefinitionInInternalList(currentDefinition);
        }
    }
    else {
        qCritical() << "The model of agent mapping is NULL !";
    }
}


/**
     * @brief Ghost Constructor: Definition model is empty. The agent is an empty shell only defined by a name.
     * @param agentName
     * @param parent
     */
AgentInMappingVM::AgentInMappingVM(QString agentName, QObject *parent) : QObject(parent),
    _agentName(agentName),
    _position(QPointF()),
    _isON(false),
    _isReduced(false),
    _reducedMapValueTypeInInput(AgentIOPValueTypes::MIXED),
    _reducedMapValueTypeInOutput(AgentIOPValueTypes::MIXED),
    _isGhost(true)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

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
         addPointMapInInternalOutputList(newDefinition);

        Q_EMIT newDefinitionInAgentMapping(this);
    }
    else {
        qInfo()<<"The definition of the agent named '" << _agentName << "' could not be add to the agent mapping VM named '" << newDefinition->name() << "'";
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
        QList<InputVM*> listOfPointMapTemp;

        foreach (AgentIOPM* iopM, newDefinition->inputsList()->toList())
        {
            if (iopM != NULL)
            {
                //Check if it's not alreafy exist in the list & add it
                if(!checkIfAlreadyInInputOutputList(iopM->name(), iopM->agentIOPType()))
                {
                    // New pointMapVM (from agent and input model
                    InputVM* inputPoint = new InputVM(iopM->name(), iopM, this);

                    // Add to the list of newly created pointMap
                    listOfPointMapTemp.append(inputPoint);

                    // Update the hash table
                    _mapOfInputsFromInputName.insert(iopM->name(), inputPoint);

                    qInfo()<<"Add new point map " << _agentName << "." << iopM->name() << " in the inputs list.";
                }
                else {
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
        // Create the list of Point Map to add in the agent in mapping
        QList<OutputVM*> listOfPointMapTemp;

        foreach (OutputM* iopM, newDefinition->outputsList()->toList())
        {
            if (iopM != NULL)
            {
                //Check if it's not alreafy exist in the list & add it
                if(!checkIfAlreadyInInputOutputList(iopM->name(), iopM->agentIOPType()))
                {
                    // New pointMapVM (from agent and input model
                    OutputVM* outputPoint = new OutputVM(iopM->name(), iopM, this);

                    // Add to the list of newly created pointMap
                    listOfPointMapTemp.append(outputPoint);

                    // Update the hash table
                    _mapOfOutputsFromOutputName.insert(iopM->name(), outputPoint);

                    qInfo()<<"Add new point map " << _agentName << "." << iopM->name() << " in the inputs list.";
                }
                else {
                    qInfo() << "Point map : " << _agentName << "." << iopM->name() << " is already present in the list.";
                }
            }
        }
        _outputsList.append(listOfPointMapTemp);
    }
}


// TODO ESTIA Sera sans doute supprimer. Attendre de voir le multidefinition
/**
     * @brief This function check if the point map already exist in the input list
     * @param iopName The name of the iop
     * @param typeofIOP The type of iop
     */
bool AgentInMappingVM::checkIfAlreadyInInputOutputList(QString iopName, AgentIOPTypes::Value typeofIOP)
{
    if(typeofIOP == AgentIOPTypes::INPUT)
    {
        foreach (InputVM* iterator, _inputsList.toList())
        {
            // If this VM contains our model of agent
            if ( (iterator != NULL)
                 &&

                 // Same Input name
                 (iterator->modelM()->name() == iopName)
                 )
            {
                // Exactly the same point ('agent name' & 'iop name')
                return true;
            }
        }
    }
    else if(typeofIOP == AgentIOPTypes::OUTPUT)
    {
        foreach (OutputVM* iterator, _outputsList.toList())
        {
            // If this VM contains our model of agent
            if ( (iterator != NULL)
                 &&

                 // Same Input name
                 (iterator->modelM()->name() == iopName)
                 )
            {
                // Exactly the same point ('agent name' & 'iop name')
                return true;
            }
        }
    }

    return false;
}


/**
     * @brief Return the corresponding PointMap from the input IOP name
     * @param inputName
     */
InputVM * AgentInMappingVM::getPointMapFromInputName(QString inputName)
{
    // Doc QT: If the hash contains no item with the given key, the function returns defaultValue.
    // Suggestion pour être sur: if (contains) ... else { return NULL; }
    return _mapOfInputsFromInputName.value(inputName);
}

/**
     * @brief Return the corresponding PointMap from the output IOP name
     * @param outputName
     */
OutputVM * AgentInMappingVM::getPointMapFromOutputName(QString outputName)
{
    // Doc QT: If the hash contains no item with the given key, the function returns defaultValue.
    // Suggestion pour être sur: if (contains) ... else { return NULL; }
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
