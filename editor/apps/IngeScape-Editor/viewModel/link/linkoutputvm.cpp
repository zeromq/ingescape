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

#include "linkoutputvm.h"
#include <viewModel/link/linkinputvm.h>


/**
 * @brief Constructor
 * @param output
 * @param parent
 */
LinkOutputVM::LinkOutputVM(OutputVM* output,
                           QObject *parent) : LinkConnectorVM(parent),
    _output(output)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_output != nullptr) {
        setname(_output->name());
    }
}


/**
 * @brief Destructor
 */
LinkOutputVM::~LinkOutputVM()
{
    if (_output != nullptr) {
        setoutput(nullptr);
    }
}


/**
 * @brief Return true if our output can link with the input (types are compatible)
 * @param linkConnector
 * @return
 */
bool LinkOutputVM::canLinkWith(LinkConnectorVM* linkConnector)
{
    LinkInputVM* linkInput = qobject_cast<LinkInputVM*>(linkConnector);
    if ((linkInput != nullptr) && (linkInput->input() != nullptr) && (linkInput->input()->firstModel() != nullptr)
            && (_output != nullptr) && (_output->firstModel() != nullptr))
    {
        // Call parent class function
        return _canLinkOutputToInput(_output->firstModel()->agentIOPValueType(), linkInput->input()->firstModel()->agentIOPValueType());
    }
    else {
        return false;
    }
}
