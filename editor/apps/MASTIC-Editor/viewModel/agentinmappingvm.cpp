#include "agentinmappingvm.h"

/**
     * @brief Default constructor
     * @param agentModelList. At least one agentM is need and the first agentM of list is the reference agentM.
     * Typically passing during the drag-drop from the list of agent on the left side.
     * @param position Position of the box
     * @param parent
     */
AgentInMappingVM::AgentInMappingVM(QList<AgentM*> agentModelList,
                                   QPointF position,
                                   QObject *parent) : QObject(parent),
    _agentName(""),
    _position(position),
    _isON(false),
    _isReduced(false),
    _reducedMapValueTypeInInput(AgentIOPValueTypes::MIXED),
    _reducedMapValueTypeInOutput(AgentIOPValueTypes::MIXED),
    _isGhost(false),
    _areIdenticalsAllDefinitions(true)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if(!agentModelList.isEmpty())
    {
        if(agentModelList.first() != NULL)
        {

            //Get the name of the AgentInMappingVM
            _agentName = agentModelList.first()->name();

            // Set activity. // FIXME VICENT : ESTIA
            setisON(agentModelList.first()->isON());

            //Process all definitions in the list
            foreach (AgentM* currentAgentM, agentModelList)
            {
                addAgentToInternalList(currentAgentM);
            }

            // At least one agent model
            setisGhost(false);
        }
        else {
            qCritical() << "No agent model for the agent mapping is available !";
        }
    }
}

/**
     * @brief Ghost Constructor: AgentMList is empty. The agent is an empty shell only defined by a name.
     * @param agentName
     * @param parent
     */
AgentInMappingVM::AgentInMappingVM(QString agentName, QObject *parent) : AgentInMappingVM(QList<AgentM*>(), QPointF(), parent)
{
    setagentName(agentName);
    setisGhost(true);
    qInfo() << "New ghost agentInMappingVM as been made." << _agentName;
}

/**
     * @brief Add agent dynamically to the internal list
     * @param newAgent The definition to add
     */
void AgentInMappingVM::addAgentToInternalList(AgentM* newAgentM)
{
    //Check if agent model and agent in mapping share the same name
    if(newAgentM->name() == _agentName)
    {
        if(_agentModelList.isEmpty()) // First model of agent to be ever submitted.
        {
            qInfo() << "Add initial agent model.";

            // Create the list of input (PointMapVM)
            addPointMapInInternalInputList(newAgentM->definition());

            // Create the list of output (PointMapVM)
            addPointMapInInternalOutputList(newAgentM->definition());

            // Pull the creation of newMapBetweenIOPVM...
            Q_EMIT newDefinitionInAgentMapping(this);
        }
        else // Another model of agent is provided. Must show "patte blanche".
        {
            //Check if agentM has same definition as the first agent of internal agentList().
            if(!DefinitionM::areIdenticals(_agentModelList.toList().first()->definition(), newAgentM->definition()))
            {
                // Should handle the difference by creating the missing INPUT / OUTPUT. More work is done at their creation.
                qInfo() << "Add initial agent model but defintion is different.";

                // Create the list of input (PointMapVM)
                addPointMapInInternalInputList(newAgentM->definition());

                // Create the list of output (PointMapVM)
                addPointMapInInternalOutputList(newAgentM->definition());

                // Pull the creation of newMapBetweenIOPVM...
                Q_EMIT newDefinitionInAgentMapping(this);
            }
        }

        //Add new agent model
        _agentModelList.append(newAgentM);

    }
    else {
        qInfo()<<"The agent model named '" << _agentName << "' could not be add to the agent mapping VM named '" << newAgentM->name() << "'";
    }
}


/**
     * @brief Add new points Map to the inputs list from a definition model
     * @param newDefinition The definition model
     */
void AgentInMappingVM::addPointMapInInternalInputList(DefinitionM *newDefinition)
{
    if(newDefinition != NULL)
    {
        // Create the list of Point Map to add in the agent in mapping
        QList<InputVM*> listOfPointMapTemp;

        foreach (AgentIOPM* iopM, newDefinition->inputsList()->toList())
        {
            if (iopM != NULL)
            {
                // New pointMapVM (from agent and input model
                InputVM* inputPoint = new InputVM(iopM->name(), iopM, this);

                //Check if it's not alreafy exist in the list & add it
                if(!checkIfAlreadyInInputList(inputPoint))
                {
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
                // New pointMapVM (from agent and input model
                OutputVM* outputPoint = new OutputVM(iopM->name(), iopM, this);

                //Check if it's not alreafy exist in the list & add it
                if(!checkIfAlreadyInOutputList(outputPoint))
                {
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

/**
     * @brief This function check if the InputVM already exist in the input list
     * @param currentInput The newly created Input VM
     */
bool AgentInMappingVM::checkIfAlreadyInInputList(InputVM* currentInput)
{
    foreach (InputVM* iterator, _inputsList.toList())
    {
        if( (currentInput != NULL) && (iterator != NULL))
        {
            if(currentInput->modelM() != NULL && iterator->modelM() != NULL )
            {
                if( currentInput->modelM()->id() == iterator->modelM()->id() )
                {
                    // Exactly the same point
                    return true;
                }
            }
        }
    }
    return false;
}

/**
     * @brief This function check if the OutputVM already exist in the input list
     * @param currentOuput The newly created OutputVM
     */
bool AgentInMappingVM::checkIfAlreadyInOutputList(OutputVM* currentOuput)
{
    foreach (OutputVM* iterator, _outputsList.toList())
    {
        if( (currentOuput != NULL) && (iterator != NULL))
        {
            if(currentOuput->modelM() != NULL && iterator->modelM() != NULL )
            {
                if( currentOuput->modelM()->id() == iterator->modelM()->id() )
                {
                    // Exactly the same point
                    return true;
                }
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
    _agentModelList.clear();

    //Delete element in the previous list Input & Output
    _mapOfInputsFromInputName.clear();
    _mapOfOutputsFromOutputName.clear();

    //Delete element in the list Input & Output
    _inputsList.deleteAllItems();
    _outputsList.deleteAllItems();
}
