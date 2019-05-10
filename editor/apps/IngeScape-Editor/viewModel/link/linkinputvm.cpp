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

#include "linkinputvm.h"
#include <viewModel/link/linkoutputvm.h>


/**
 * @brief Constructor
 * @param input
 * @param parent
 */
LinkInputVM::LinkInputVM(InputVM* input,
                         QObject *parent) : LinkConnectorVM(parent),
    _input(input)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_input != nullptr) {
        setname(_input->name());
        setuid(_input->uid());
    }
    else {
        setname("in");
        setuid(QString("%1%2%3").arg(_name, SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE, AgentIOPValueTypes::staticEnumToString(AgentIOPValueTypes::IMPULSION)));
    }
}


/**
 * @brief Destructor
 */
LinkInputVM::~LinkInputVM()
{
    if (_input != nullptr) {
        setinput(nullptr);
    }
}


/**
 * @brief Return true if our input can link with the output (types are compatible)
 * @param linkConnector
 * @return
 */
bool LinkInputVM::canLinkWith(LinkConnectorVM* linkConnector)
{
    bool canLink = false;

    LinkOutputVM* linkOutput = qobject_cast<LinkOutputVM*>(linkConnector);
    if (linkOutput != nullptr)
    {
        // If the input OR the output is null, we try to link an action
        if ((linkOutput->output() == nullptr) || (_input == nullptr))
        {
            canLink = true;
        }
        //
        else if ((linkOutput != nullptr) && (linkOutput->output() != nullptr) && (linkOutput->output()->firstModel() != nullptr)
                 && (_input != nullptr) && (_input->firstModel() != nullptr))
         {
             // Call our mother class
             canLink = _canLinkOutputToInput(linkOutput->output()->firstModel()->agentIOPValueType(), _input->firstModel()->agentIOPValueType());
         }
    }

    return canLink;
}
