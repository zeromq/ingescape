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

    if (_output != nullptr)
    {
        setname(_output->name());
        setuid(_output->id());

        // Connect to signals from the view model of output
        connect(_output, &OutputVM::currentValueChanged, this, &LinkOutputVM::_oncurrentValueChanged);
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
    disconnect(&_timer, 0, this, 0);


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
 * @brief Slot when the timer time out
 */
void LinkOutputVM::_onTimeout()
{
    // Stop the timer
    _timer.stop();

    // Reset the flag
    setisPublishedNewValue(false);
}

