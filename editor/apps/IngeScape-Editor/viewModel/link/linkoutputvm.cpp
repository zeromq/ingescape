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
    _output(output),
    _isPublishedNewValue(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Agent
    if (_output != nullptr)
    {
        setname(_output->name());
        setuid(_output->uid());

        // Connect to signals from the view model of output
        connect(_output, &OutputVM::currentValueChanged, this, &LinkOutputVM::_oncurrentValueChanged);
    }
    // Action
    else
    {
        setname(ACTION_LINK_OUTPUT_NAME);
        setuid(QString("%1%2%3").arg(_name, SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE, AgentIOPValueTypes::staticEnumToString(AgentIOPValueTypes::IMPULSION)));
    }

    // Init the timer to reset the flag "is Published New Value"
    // Allows to play an animation when the value changed
    _timer.setInterval(500);
    connect(&_timer, &QTimer::timeout, this, &LinkOutputVM::_onTimeout);
}


/**
 * @brief Destructor
 */
LinkOutputVM::~LinkOutputVM()
{
    //
    // Stop and clean the timer
    //
    _timer.stop();
    disconnect(&_timer, nullptr, this, nullptr);


    if (_output != nullptr)
    {
        // DIS-connect to signals from the view model of output
        disconnect(_output, &OutputVM::currentValueChanged, this, &LinkOutputVM::_oncurrentValueChanged);

        setoutput(nullptr);
    }
}


/**
 * @brief Simulate that the current value of model changed: allows to highlight the corresponding link(s)
 */
void LinkOutputVM::simulateCurrentValueOfModelChanged()
{
    _oncurrentValueChanged(QVariant());
}


/**
 * @brief Return true if our output can link with the input (types are compatible)
 * @param linkConnector
 * @return
 */
bool LinkOutputVM::canLinkWith(LinkConnectorVM* linkConnector)
{
    bool canLink = false;

    LinkInputVM* linkInput = qobject_cast<LinkInputVM*>(linkConnector);
    if (linkInput != nullptr)
    {
        // If the input OR the output is null, we try to link an action
        if ((linkInput->input() == nullptr) || (_output == nullptr))
        {
            canLink = true;
        }
        //
        else if ((linkInput->input() != nullptr) && (linkInput->input()->firstModel() != nullptr)
                && (_output != nullptr) && (_output->firstModel() != nullptr))
        {
            // Call our mother class
            canLink = _canLinkOutputToInput(_output->firstModel()->agentIOPValueType(), linkInput->input()->firstModel()->agentIOPValueType());
        }
    }

    return canLink;
}


/**
 * @brief Slot called when the current value (of our output) changed
 * @param value
 */
void LinkOutputVM::_oncurrentValueChanged(QVariant value)
{
    Q_UNUSED(value)

    /*if (_output != nullptr) {
        qDebug() << "On Current Value of Output Changed" << _name << value.toString();
    }*/

    // Check that the flag is not already to true
    if (!_isPublishedNewValue)
    {
        setisPublishedNewValue(true);

        // Start the timer to reset the flag "is Published New Value"
        _timer.start();
    }
}


/**
 * @brief Slot called when the timer times out
 */
void LinkOutputVM::_onTimeout()
{
    // Stop the timer
    _timer.stop();

    // Reset the flag
    setisPublishedNewValue(false);
}

