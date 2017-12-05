/*
 *	MASTIC Editor
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

}


/**
 * @brief Initialize the reverse command (and parameters) for each effect
 * @param effectsList
 */
void ActionExecutionVM::initReverseCommandsForEffects(QList<ActionEffectVM*> effectsList)
{
    foreach (ActionEffectVM* effectVM, effectsList)
    {
        if ((effectVM != NULL) && (effectVM->modelM() != NULL))
        {
            // Get the pair with the agent name and the reverse command (with parameters) of the effect
            QPair<QString, QStringList> pairAgentNameAndReverseCommand = effectVM->modelM()->getAgentNameAndReverseCommandWithParameters();

            qDebug() << "Reverse Command (and parameters):" << pairAgentNameAndReverseCommand.second << "for agent" << pairAgentNameAndReverseCommand.first;

            _reverseCommandsForAgents.append(pairAgentNameAndReverseCommand);

            // FIXME TO REMOVE
            /*switch (effectVM->effectType())
            {
            // AGENT
            case ActionEffectTypes::AGENT:
            {
                // Try to cast as "Effect on Agent"
                EffectOnAgentM* effectOnAgent = qobject_cast<EffectOnAgentM*>(effectModel);
                if (effectOnAgent != NULL)
                {
                    switch (effectOnAgent->agentEffectValue())
                    {
                    case AgentEffectValues::ON: {
                        reverseCommandAndParameters << "STOP";
                        break;
                    }
                    case AgentEffectValues::OFF: {
                        reverseCommandAndParameters << "RUN";
                        break;
                    }
                    default:
                        break;
                    }
                }
                break;
            }
            // VALUE
            case ActionEffectTypes::VALUE:
            {
                // Try to cast as IOP Value Effect
                IOPValueEffectM* iopValueEffect = qobject_cast<IOPValueEffectM*>(effectModel);
                if ((iopValueEffect != NULL) && (iopValueEffect->agentIOP() != NULL))
                {
                    // SET_INPUT / SET_OUTPUT / SET_PARAMETER
                    QString command = QString("SET_%1").arg(AgentIOPTypes::staticEnumToString(iopValueEffect->agentIOP()->agentIOPType()));

                    reverseCommandAndParameters << command << iopValueEffect->agentIOP()->name();

                    switch (iopValueEffect->agentIOP()->agentIOPValueType())
                    {
                    case AgentIOPValueTypes::BOOL: {
                        if (iopValueEffect->agentIOP()->currentValue().toBool()) {
                            reverseCommandAndParameters << "true";
                        }
                        else {
                            reverseCommandAndParameters << "false";
                        }
                        break;
                    }
                    case AgentIOPValueTypes::INTEGER: {
                        // FIXME check if conversion succeeded
                        //reverseCommandAndParameters << QString::number(iopValueEffect->agentIOP()->currentValue().toInt());
                        reverseCommandAndParameters << iopValueEffect->agentIOP()->displayableCurrentValue();
                        break;
                    }
                    case AgentIOPValueTypes::DOUBLE: {
                        // FIXME check if conversion succeeded
                        //reverseCommandAndParameters << QString::number(iopValueEffect->agentIOP()->currentValue().toDouble());
                        reverseCommandAndParameters << iopValueEffect->agentIOP()->displayableCurrentValue();
                        break;
                    }
                    case AgentIOPValueTypes::STRING: {
                        reverseCommandAndParameters << iopValueEffect->agentIOP()->currentValue().toString();
                        break;
                    }
                    default:
                        qInfo() << "The Input/Output/Parameter has value type DATA or IMPULSION thus the effect is irreversible!";
                        break;
                    }
                }
                break;
            }
            // MAPPING
            case ActionEffectTypes::MAPPING:
            {
                // Try to cast as Mapping Effect
                MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(effectModel);
                if ((mappingEffect != NULL) && (mappingEffect->input() != NULL) && (mappingEffect->outputAgent() != NULL) && (mappingEffect->output() != NULL))
                {
                    QString command = "";
                    QString reverseCommand = "";

                    switch (mappingEffect->mappingEffectValue())
                    {
                    case MappingEffectValues::MAPPED: {
                        reverseCommand = "UNMAP";
                        break;
                    }
                    case MappingEffectValues::UNMAPPED: {
                        reverseCommand = "MAP";
                        break;
                    }
                    default:
                        break;
                    }

                    reverseCommandAndParameters << reverseCommand << mappingEffect->input()->name() << mappingEffect->outputAgent()->name() << mappingEffect->output()->name();
                }
                break;
            }
            default:
                break;
            }*/
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


