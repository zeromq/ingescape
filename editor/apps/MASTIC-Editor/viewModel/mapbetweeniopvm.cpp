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

#include "mapbetweeniopvm.h"


#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param outputAgent The link starts from this agent
 * @param output The link starts from this output of the output agent
 * @param inputAgent The link ends to this agent
 * @param input The link ends to this input of the input agent
 * @param isVirtual
 * @param parent
 */
MapBetweenIOPVM::MapBetweenIOPVM(AgentInMappingVM* outputAgent,
                                 OutputVM* output,
                                 AgentInMappingVM* inputAgent,
                                 InputVM* input,
                                 bool isVirtual,
                                 QObject *parent) : QObject(parent),
    _outputAgent(NULL),
    _output(NULL),
    _inputAgent(NULL),
    _input(NULL),
    _isVirtual(isVirtual)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Allow to benefit of "DELETE PROOF"
    setoutputAgent(outputAgent);
    setoutput(output);
    setinputAgent(inputAgent);
    setinput(input);

    if ((_outputAgent != NULL) && (_output != NULL) && (_inputAgent != NULL) && (_input != NULL))
    {
        //qInfo() << "Create new Map Between IOP VM" << _outputAgent->name() << "." << _output->name() << "-->" << _inputAgent->name() << "." << _input->name();
    }
}


/**
 * @brief Destructor
 */
MapBetweenIOPVM::~MapBetweenIOPVM()
{
    if ((_outputAgent != NULL) && (_output != NULL) && (_inputAgent != NULL) && (_input != NULL))
    {
        //qInfo() << "Delete Map Between IOP VM" << _outputAgent->name() << "." << _output->name() << "-->" << _inputAgent->name() << "." << _input->name();
    }

    if (_outputAgent != NULL)
    {
        AgentInMappingVM* temp = _outputAgent;
        setoutputAgent(NULL);

        // Handle ghost agent
        if (temp->isGhost()) {
            delete temp;
        }
    }

    if (_output != NULL)
    {
        OutputVM* temp = _output;
        setoutput(NULL);

        // Handle ghost output
        if (temp->isGhost()) {
            delete temp;
        }
    }

    setinputAgent(NULL);
    setinput(NULL);
}

