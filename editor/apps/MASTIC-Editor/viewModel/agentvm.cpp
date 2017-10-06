/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "agentvm.h"

#include <QQmlEngine>
#include <QDebug>



/**
 * @brief Default constructor
 * @param model
 * @param parent
 */
AgentVM::AgentVM(AgentM* model, QObject *parent) : QObject(parent),
    _modelM(model),
    _definition(NULL),
    _isFictitious(true),
    _status(AgentStatus::OFF),
    _state(""),
    _x(0),
    _y(0),
    _isMuted(false),
    _isFrozen(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelM != NULL)
    {
        if (_modelM->peerId().isEmpty()) {
            _isFictitious = true;
            qInfo() << "New View Model of FICTITIOUS Agent" << _modelM->name();
        }
        else {
            _isFictitious = false;
            qInfo() << "New View Model of Agent" << _modelM->name() << "with peer id" << _modelM->peerId();
        }
    }
}


/**
 * @brief Destructor
 */
AgentVM::~AgentVM()
{
    if (_definition != NULL)
    {
        setdefinition(NULL);
    }

    if (_modelM != NULL)
    {
        qInfo() << "Delete View Model of Agent" << _modelM->name() << "(" << _modelM->peerId() << ")";

        setmodelM(NULL);
    }

    // Delete our lists of agents VM
    //_listIdenticalAgentsVM.deleteAllItems();
    //_listSimilarAgentsVM.deleteAllItems();
}


/**
 * @brief Setter for property "Definition"
 * @param value
 */
void AgentVM::setdefinition(DefinitionM *value)
{
    if (_definition != value)
    {
        // Previous value
        if (_definition != NULL) {
            qWarning() << "Delete previous definition ?";

            // Delete all previous Inputs / Outputs / Parameters
            _inputsList.deleteAllItems();
            _outputsList.deleteAllItems();
            _parametersList.deleteAllItems();
        }

        _definition = value;

        // New value
        if (_definition != NULL)
        {
            //
            // Create the list of VM of inputs
            //
            QList<AgentIOPVM*> listOfInputVMs;
            foreach (AgentIOPM* inputM, _definition->inputsList()->toList())
            {
                if (inputM != NULL)
                {
                    AgentIOPVM* inputVM = new AgentIOPVM(inputM, this);
                    listOfInputVMs.append(inputVM);
                }
            }
            _inputsList.append(listOfInputVMs);


            //
            // Create the list of VM of outputs
            //
            QList<AgentIOPVM*> listOfOutputVMs;
            foreach (AgentIOPM* outputM, _definition->outputsList()->toList())
            {
                if (outputM != NULL)
                {
                    AgentIOPVM* outputVM = new AgentIOPVM(outputM, this);
                    listOfOutputVMs.append(outputVM);
                }
            }
            _outputsList.append(listOfOutputVMs);


            //
            // Create the list of VM of parameters
            //
            QList<AgentIOPVM*> listOfParameterVMs;
            foreach (AgentIOPM* parameterM, _definition->parametersList()->toList())
            {
                if (parameterM != NULL)
                {
                    AgentIOPVM* parameterVM = new AgentIOPVM(parameterM, this);
                    listOfParameterVMs.append(parameterVM);
                }
            }
            _parametersList.append(listOfParameterVMs);
        }

        Q_EMIT definitionChanged(value);
    }
}
