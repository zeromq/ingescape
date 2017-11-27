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
    _isWaitingRevert(false),
    _isExecuted(false),
    _executionTime(executionTime),
    _reverseTime(reverseTime)
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

                    QString iopName = iopValueEffect->agentIOP()->name();

                    switch (iopValueEffect->agentIOP()->agentIOPValueType())
                    {
                    case AgentIOPValueTypes::BOOL: {
                        if (iopValueEffect->agentIOP()->currentValue().toBool()) {
                            commandAndParameters = QString("%1 %2 %3").arg(command, iopName, "false");
                            reverseCommandAndParameters = QString("%1 %2 %3").arg(command, iopName, "true");
                        }
                        else {
                            commandAndParameters = QString("%1 %2 %3").arg(command, iopName, "true");
                            reverseCommandAndParameters = QString("%1 %2 %3").arg(command, iopName, "false");
                        }
                        break;
                    }
                    case AgentIOPValueTypes::INTEGER: {
                        commandAndParameters = QString("%1 %2 %3").arg(command, iopName, iopValueEffect->value());
                        // FIXME check if conversion succeeded
                        //reverseCommandAndParameters = QString("%1 %2 %3").arg(command, iopName, QString::number(iopValueEffect->agentIOP()->currentValue().toInt()));
                        reverseCommandAndParameters = QString("%1 %2 %3").arg(command, iopName, iopValueEffect->agentIOP()->displayableCurrentValue());
                        break;
                    }
                    case AgentIOPValueTypes::DOUBLE: {
                        commandAndParameters = QString("%1 %2 %3").arg(command, iopName, iopValueEffect->value());
                        // FIXME check if conversion succeeded
                        //reverseCommandAndParameters = QString("%1 %2 %3").arg(command, iopName, QString::number(iopValueEffect->agentIOP()->currentValue().toDouble()));
                        reverseCommandAndParameters = QString("%1 %2 %3").arg(command, iopName, iopValueEffect->agentIOP()->displayableCurrentValue());
                        break;
                    }
                    case AgentIOPValueTypes::STRING: {
                        commandAndParameters = QString("%1 %2 %3").arg(command, iopName, iopValueEffect->value());
                        reverseCommandAndParameters = QString("%1 %2 %3").arg(command, iopName, iopValueEffect->agentIOP()->currentValue().toString());
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
                if ((mappingEffect != NULL) && (mappingEffect->input() != NULL) && (mappingEffect->output() != NULL))
                {
                    switch (mappingEffect->mappingEffectValue())
                    {
                        case MappingEffectValues::MAPPED:
                            commandAndParameters = QString("%1 %2 %3 %4").arg("MAP", mappingEffect->input()->name(), agentName, mappingEffect->output()->name());
                            reverseCommandAndParameters = QString("%1 %2 %3 %4").arg("UNMAP", mappingEffect->input()->name(), agentName, mappingEffect->output()->name());
                            break;

                        case MappingEffectValues::UNMAPPED:
                            commandAndParameters = QString("%1 %2 %3 %4").arg("UNMAP", mappingEffect->input()->name(), mappingEffect->agent()->name(), mappingEffect->output()->name());
                            reverseCommandAndParameters = QString("%1 %2 %3 %4").arg("MAP", mappingEffect->input()->name(), agentName, mappingEffect->output()->name());
                            break;

                        default:
                            break;
                    }
                }
                break;
            }
            default:
                break;
            }

            // Command (and parameters) are defined
            if (!commandAndParameters.isEmpty())
            {
                qDebug() << pairCommand.first << "Command (and parameters):" << commandAndParameters;

                pairCommand.second = commandAndParameters;

                commandsForAgents.append(pairCommand);
            }

            // Reverse command (and parameters) are defined
            if (_shallRevert && !reverseCommandAndParameters.isEmpty())
            {
                qDebug() << pairCommand.first << "Reverse Command (and parameters):" << reverseCommandAndParameters;

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


