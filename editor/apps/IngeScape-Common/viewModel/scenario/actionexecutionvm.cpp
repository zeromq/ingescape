/*
 *	IngeScape Editor
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

#include "actionexecutionvm.h"

/**
 * @brief Constructor
 * @param shallRevert
 * @param executionTime
 * @param reverseTime
 * @param parent
 */
ActionExecutionVM::ActionExecutionVM(bool shallRevert,
                                     int executionTime,
                                     int reverseTime,
                                     QObject *parent) : QObject(parent),
    _shallRevert(shallRevert),
    _isExecuted(false),
    _executionTime(executionTime),
    _reverseTime(reverseTime),
    _neverExecuted(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_shallRevert) {
        qDebug() << "New Action Execution with REVERT at" << _executionTime << "seconds";
    }
    else {
        qDebug() << "New Action Execution (without revert) at" << _executionTime << "seconds";
    }
}


/**
  * @brief Destructor
  */
ActionExecutionVM::~ActionExecutionVM()
{
    if (_shallRevert) {
        qDebug() << "Delete Action Execution with REVERT at" << _executionTime << "seconds";
    }
    else {
        qDebug() << "Delete Action Execution (without revert) at" << _executionTime << "seconds";
    }
}


/**
 * @brief Initialize the reverse command (and parameters) for each effect
 * @param effectsList
 */
void ActionExecutionVM::initReverseCommandsForEffects(QList<ActionEffectVM*> effectsList)
{
    for (ActionEffectVM* effectVM : effectsList)
    {
        if ((effectVM != nullptr) && (effectVM->modelM() != nullptr))
        {
            // Get the pair with the agent name and the reverse command (with parameters) of the effect
            QPair<QString, QStringList> pairAgentNameAndReverseCommand = effectVM->modelM()->getAgentNameAndReverseCommandWithParameters();

            qDebug() << "Reverse Command (and parameters):" << pairAgentNameAndReverseCommand.second << "for agent" << pairAgentNameAndReverseCommand.first;

            _reverseCommandsForAgents.append(pairAgentNameAndReverseCommand);
        }
    }
}


/**
 * @brief Get the list of Reverse commands
 * @return
 */
QList<QPair<QString, QStringList>> ActionExecutionVM::getReverseCommands()
{
    return _reverseCommandsForAgents;
}


