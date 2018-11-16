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
 * @param outputAgent The link starts from this agent
 * @param linkOutput The link starts from this output of the output agent
 * @param inputAgent The link ends to this agent
 * @param linkInput The link ends to this input of the input agent
 * @param isVirtual
 * @param parent
 */
LinkVM::LinkVM(AgentInMappingVM* outputAgent,
               LinkOutputVM* linkOutput,
               AgentInMappingVM* inputAgent,
               LinkInputVM* linkInput,
               bool isVirtual,
               QObject *parent) : QObject(parent),
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
        qInfo() << "Create new Link between" << _outputAgent->name() << "." << _linkOutput->name() << "-->" << _inputAgent->name() << "." << _linkInput->name();

        // Identifier with all names: [outputAgent##linkOutput-->inputAgent##linkInput]
        _id = QString("%1%2%3-->%4%2%5").arg(_outputAgent->name(), SEPARATOR_AGENT_NAME_AND_IOP, _linkOutput->name(), _inputAgent->name(), _linkInput->name());
    }
}


/**
 * @brief Destructor
 */
LinkVM::~LinkVM()
{
    if ((_outputAgent != nullptr) && (_linkOutput != nullptr) && (_inputAgent != nullptr) && (_linkInput != nullptr))
    {
        qInfo() << "Delete Link between" << _outputAgent->name() << "." << _linkOutput->name() << "-->" << _inputAgent->name() << "." << _linkInput->name();
    }

    setoutputAgent(nullptr);
    setlinkOutput(nullptr);
    setinputAgent(nullptr);
    setlinkInput(nullptr);
}
