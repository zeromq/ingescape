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
 *
 */

#include "elementmappingm.h"

/**
 * @brief Constructor with input and output agent and IOP
 * @param inputAgent
 * @param input
 * @param outputAgent
 * @param output
 * @param parent
 */
ElementMappingM::ElementMappingM(QString inputAgent,
                                 QString input,
                                 QString outputAgent,
                                 QString output,
                                 QObject *parent) : QObject(parent),
    _inputAgent(inputAgent),
    _input(input),
    _outputAgent(outputAgent),
    _output(output)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Model of Element Mapping between Agent input: " << _inputAgent << "." << _input << " and Agent output: " << _outputAgent << "." << _output;
}


/**
 * @brief Destructor
 */
ElementMappingM::~ElementMappingM()
{
    //qInfo() << "Delete Model of Element Mapping between Agent input: " << _inputAgent << "." << _input << " and Agent output: " << _outputAgent << "." << _output;
}
