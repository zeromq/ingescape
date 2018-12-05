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
    _id(""),
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

        // Identifier with agents names and Input/Output ids: "outputAgent##output::outputType-->inputAgent##input::inputType"

        // Get the id (with format "outputAgent##output::outputType-->inputAgent##input::inputType")
        // of our link from agent names and Input/Output ids
        _id = LinkVM::getIdFromAgentNamesAndIOids(_outputAgent->name(), _linkOutput->id(), _inputAgent->name(), _linkInput->id());

        qInfo() << "New Link" << _name << "(" << _id << ")";
    }
}


/**
 * @brief Destructor
 */
LinkVM::~LinkVM()
{
    qInfo() << "Delete Link" << _name << "(" << _id << ")";

    //setmappingElement(nullptr);

    setoutputAgent(nullptr);
    setlinkOutput(nullptr);
    setinputAgent(nullptr);
    setlinkInput(nullptr);
}


/**
 * @brief Get the id (with format "outputAgent##output::outputType-->inputAgent##input::inputType")
 * of a link from agent names and Input/Output ids
 * @param inputAgent
 * @param input
 * @param outputAgent
 * @param output
 * @return id with format "outputAgent##output::outputType-->inputAgent##input::inputType"
 */
QString LinkVM::getIdFromAgentNamesAndIOids(QString inputAgent, QString inputId, QString outputAgent, QString outputId)
{
    if (!inputAgent.isEmpty() && !inputId.isEmpty() && !outputAgent.isEmpty() && !outputId.isEmpty())
    {
        // outputAgent##output::outputType-->inputAgent##input::inputType
        return QString("%1%2%3%4%5%2%6").arg(outputAgent, SEPARATOR_AGENT_NAME_AND_IOP, outputId, SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT, inputAgent, inputId);
    }
    else {
        return "";
    }
}
