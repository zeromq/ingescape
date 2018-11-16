/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
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
#include <model/enums.h>

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
    _name(""),
    _inputAgent(inputAgent),
    _input(input),
    _outputAgent(outputAgent),
    _output(output)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Name with all names formatted: "outputAgent##output-->inputAgent##input"
    _name = QString("%1%2%3-->%4%2%5").arg(_outputAgent, SEPARATOR_AGENT_NAME_AND_IOP, _output, _inputAgent, _input);

    //qInfo() << "New Model of Element Mapping" << _name;
}


/**
 * @brief Destructor
 */
ElementMappingM::~ElementMappingM()
{
    //qInfo() << "Delete Model of Element Mapping" << _name;
}
