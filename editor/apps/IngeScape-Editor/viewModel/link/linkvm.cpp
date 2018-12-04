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

#include "linkvm.h"


/**
 * @brief Constructor
 * @param mappingElement
 * @param outputAgent The link starts from this agent
 * @param linkOutput The link starts from this output of the output agent
 * @param inputAgent The link ends to this agent
 * @param linkInput The link ends to this input of the input agent
 * @param isVirtual
 * @param parent
 */
LinkVM::LinkVM(//MappingElementVM* mappingElement,
               AgentInMappingVM* outputAgent,
               LinkOutputVM* linkOutput,
               AgentInMappingVM* inputAgent,
               LinkInputVM* linkInput,
               bool isVirtual,
               QObject *parent) : QObject(parent),
    _name(""),
    //_id(""),
    //_mappingElement(mappingElement),
    _outputAgent(outputAgent),
    _linkOutput(linkOutput),
    _inputAgent(inputAgent),
    _linkInput(linkInput),
    _isVirtual(isVirtual)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if ((_outputAgent != nullptr) && (_linkOutput != nullptr) && (_inputAgent != nullptr) && (_linkInput != nullptr))
    {
        // Get the name (with format "outputAgent##output-->inputAgent##input")
        // of our link from the list of names (of each part)
        _name = ElementMappingM::getNameFromNamesList(_outputAgent->name(), _linkOutput->name(), _inputAgent->name(), _linkInput->name());

        // Identifier with agents names and Input/Output ids: [outputAgent##output::outputType-->inputAgent##input::inputType]
        //_id = QString("%1%2%3%4%5%2%6").arg(_outputAgent->name(), SEPARATOR_AGENT_NAME_AND_IOP, _linkOutput->id(), SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT, _inputAgent->name(), _linkInput->id());

        qInfo() << "New Link" << _name;
    }
}


/**
 * @brief Destructor
 */
LinkVM::~LinkVM()
{
    qInfo() << "Delete Link" << _name;

    //setmappingElement(nullptr);

    setoutputAgent(nullptr);
    setlinkOutput(nullptr);
    setinputAgent(nullptr);
    setlinkInput(nullptr);
}
