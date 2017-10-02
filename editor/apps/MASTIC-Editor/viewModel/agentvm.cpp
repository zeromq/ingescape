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
    _x(0),
    _y(0),
    _isMuted(false),
    _isFrozen(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelM != NULL)
    {
        qInfo() << "New View Model of Agent" << _modelM->name() << "with version" << _modelM->version() << "and description" << _modelM->description();

        //
        // Create the list of VM of inputs
        //
        QList<AgentIOPVM*> listOfInputVMs;
        foreach (AgentIOPM* inputM, _modelM->inputsList()->toList())
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
        foreach (AgentIOPM* outputM, _modelM->outputsList()->toList())
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
        foreach (AgentIOPM* parameterM, _modelM->parametersList()->toList())
        {
            if (parameterM != NULL)
            {
                AgentIOPVM* parameterVM = new AgentIOPVM(parameterM, this);
                listOfParameterVMs.append(parameterVM);
            }
        }
        _parametersList.append(listOfParameterVMs);
    }
}


/**
 * @brief Destructor
 */
AgentVM::~AgentVM()
{
    if (_modelM != NULL)
    {
        qInfo() << "Delete View Model of Agent" << _modelM->name() << "with version" << _modelM->version() << "and description" << _modelM->description();

        setmodelM(NULL);
    }
}
