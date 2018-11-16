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
    LinkOutputVM* linkOutput = qobject_cast<LinkOutputVM*>(linkConnector);
    if ((linkOutput != nullptr) && (linkOutput->output() != nullptr) && (linkOutput->output()->firstModel() != nullptr)
            && (_input != nullptr) && (_input->firstModel() != nullptr))
    {
        // Call parent class function
        return _canLinkOutputToInput(linkOutput->output()->firstModel()->agentIOPValueType(), _input->firstModel()->agentIOPValueType());
    }
    else {
        return false;
    }
}
