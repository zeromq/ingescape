#include "actionexecutionvm.h"

/**
 * @brief Constructor
 * @param hasRevert
 * @param executionTime
 * @param reverseTime
 * @param parent
 */
ActionExecutionVM::ActionExecutionVM(bool hasRevert,
                                     int executionTime,
                                     int reverseTime,
                                     QObject *parent) : QObject(parent),
    _hasRevert(hasRevert),
    _isWaitingRevert(false),
    _isTriggered(false),
    _executionTime(executionTime),
    _reverseTime(reverseTime)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
  * @brief Destructor
  */
ActionExecutionVM::~ActionExecutionVM()
{

}


/**
 * @brief Get the list of commands for the list of effects (and eventually init the list of reverse commands)
 * @param effectsList
 * @return
 */
QList<QPair<QString, QString>> ActionExecutionVM::getCommandsForEffectsAndInitReverseCommands(QList<ActionEffectVM*> effectsList)
{
    // List of pairs <agent name, command>
    QList<QPair<QString, QString>> commandsForAgents;

    foreach (ActionEffectVM* effectVM, effectsList)
    {
        if ((effectVM != NULL) && (effectVM->modelM() != NULL) && (effectVM->modelM()->agent() != NULL))
        {
            ActionEffectM* effectModel = effectVM->modelM();
            QString agentName = effectModel->agent()->name();

            QPair<QString, QString> pairCommand;
            QPair<QString, QString> pairReverseCommand;

            pairCommand.first = agentName;
            pairReverseCommand.first = agentName;

            QString commandAndParameters;
            QString reverseCommandAndParameters;

            switch (effectVM->effectType())
            {
            case ActionEffectTypes::AGENT:
            {
                switch (effectModel->agentEffectValue())
                {
                case AgentEffectValues::ON: {
                    commandAndParameters = "RUN";
                    reverseCommandAndParameters = "DIE";
                    break;
                }
                case AgentEffectValues::OFF: {
                    commandAndParameters = "DIE";
                    reverseCommandAndParameters = "RUN";
                    break;
                }
                default:
                    break;
                }

                break;
            }
            case ActionEffectTypes::VALUE:
            {

                break;
            }
            case ActionEffectTypes::MAPPING:
            {

                break;
            }
            default:
                break;
            }

            // Command (and parameters) are defined
            if (!commandAndParameters.isEmpty())
            {
                pairCommand.second = commandAndParameters;

                commandsForAgents.append(pairCommand);
            }

            // Reverse command (and parameters) are defined
            if (_hasRevert && !reverseCommandAndParameters.isEmpty())
            {
                pairReverseCommand.second = reverseCommandAndParameters;

                _reverseCommandsForAgents.append(pairReverseCommand);
            }
        }
    }

    return commandsForAgents;
}


/**
 * @brief Get the list of Reverse commands
 * @return
 */
QList<QPair<QString, QString>> ActionExecutionVM::getReverseCommands()
{
    return _reverseCommandsForAgents;
}


/**
 * @brief Add a new pair (peerID, parameters) to reverse the action's effect.
 * @param peerId of the target agent
 * @param effectToReverse: VM of the effect we want to reverse
 */
/*void ActionExecutionVM::addReverseEffectsList(QString peerIdTargetAgent, ActionEffectVM* effectToReverseVM)
{
    QPair<QString, QString> newReverseEffectElement;

    if (!peerIdTargetAgent.isEmpty() && (effectToReverseVM != NULL) && (effectToReverseVM->modelM() != NULL))
    {
        // Get the model of effect
        ActionEffectM* effectToReverseM = effectToReverseVM->modelM();

        // Create first
        newReverseEffectElement.first = peerIdTargetAgent;

        // Create second
        QString parameters;

        switch (effectToReverseVM->effectType())
        { 
            case ActionEffectTypes::AGENT:
            {
                switch (effectToReverseM->agentEffectValue())
                {
                    case AgentEffectValues::ON:
                        parameters = "DIE";
                        break;

                    case AgentEffectValues::OFF:
                        parameters = "RUN";
                        break;

                    default:
                        break;
                }
                break;
            }
            break;

            case ActionEffectTypes::VALUE:
            {
                //Try to cast as IOPValueEffect.
                IOPValueEffectM* valueEffectToReverseM = dynamic_cast<IOPValueEffectM*>(effectToReverseM);
                if(valueEffectToReverseM != NULL)
                {
                    // Depending of IOP value type will reset value to default once effect apply.
                    switch (valueEffectToReverseM->agentIOP()->agentIOPValueType())
                    {
                        case AgentIOPValueTypes::BOOL:
                            if (valueEffectToReverseM->agentIOP()->currentValue().toBool()) {
                                 parameters = "true";
                            }
                            else {
                                parameters = "false";
                            }
                            break;

                        case AgentIOPValueTypes::INTEGER:
                            parameters = QString::number(valueEffectToReverseM->agentIOP()->currentValue().toInt());
                            break;

                        case AgentIOPValueTypes::DOUBLE:
                            parameters = QString::number(valueEffectToReverseM->agentIOP()->currentValue().toDouble());
                            break;

                        case AgentIOPValueTypes::STRING:
                            parameters = valueEffectToReverseM->agentIOP()->currentValue().toString();
                            break;

                        default:
                            qInfo() << "The iop is of value type DATA or IMPULSION thus the effect is irreversible!";
                            break;
                    }
                }
            }
            break;

            case ActionEffectTypes::MAPPING:
            {
                // Try to cast as Mapping
                MappingEffectM* mappingEffectToReverseM = dynamic_cast<MappingEffectM*>(effectToReverseM);
                if(mappingEffectToReverseM != NULL)
                {
                    switch (mappingEffectToReverseM->mappingEffectValue())
                    {
                        case MappingEffectValues::MAPPED:
                            parameters = QString("%s %s %s %s").arg("UNMAP",
                                                                    mappingEffectToReverseM->toAgentIOP()->name(),
                                                                    mappingEffectToReverseM->agent()->name(),
                                                                    mappingEffectToReverseM->fromAgentIOP()->name());
                            break;

                        case MappingEffectValues::UNMAPPED:
                            parameters = QString("%s %s %s %s").arg("MAP",
                                                                    mappingEffectToReverseM->toAgentIOP()->name(),
                                                                    mappingEffectToReverseM->agent()->name(),
                                                                    mappingEffectToReverseM->fromAgentIOP()->name());
                            break;

                        default:
                            break;
                    }
                }
            }
            break;
        }

        if (!parameters.isEmpty())
        {
            //
            newReverseEffectElement.second = parameters;

            //add the pair in internal list
            _reverseEffectsList.append(newReverseEffectElement);
        }
    }
}*/



