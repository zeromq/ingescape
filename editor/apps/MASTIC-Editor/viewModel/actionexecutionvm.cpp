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
 * @brief Get the list of commands for the list of effects (and eventually init the list of reverse commands)
 * @param effectsList
 * @return
 */
QList<QPair<QString, QStringList>> ActionExecutionVM::getCommandsForEffectsAndInitReverseCommands(QList<ActionEffectVM*> effectsList)
{
    // List of pairs <agent name, command (and parameters)>
    QList<QPair<QString, QStringList>> commandsForAgents;

    foreach (ActionEffectVM* effectVM, effectsList)
    {
        if ((effectVM != NULL) && (effectVM->modelM() != NULL) && (effectVM->modelM()->agent() != NULL))
        {
            ActionEffectM* effectModel = effectVM->modelM();
            QString agentName = effectModel->agent()->name();

            QPair<QString, QStringList> pairCommandAndParameters;
            QPair<QString, QStringList> pairReverseCommandAndParameters;

            pairCommandAndParameters.first = agentName;
            pairReverseCommandAndParameters.first = agentName;

            QStringList commandAndParameters;
            QStringList reverseCommandAndParameters;

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
                        commandAndParameters << "RUN";
                        reverseCommandAndParameters << "DIE";
                        break;
                    }
                    case AgentEffectValues::OFF: {
                        commandAndParameters << "DIE";
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

                    commandAndParameters << command << iopValueEffect->agentIOP()->name();
                    reverseCommandAndParameters << command << iopValueEffect->agentIOP()->name();

                    switch (iopValueEffect->agentIOP()->agentIOPValueType())
                    {
                    case AgentIOPValueTypes::BOOL: {
                        if (iopValueEffect->agentIOP()->currentValue().toBool()) {
                            commandAndParameters << "false";
                            reverseCommandAndParameters << "true";
                        }
                        else {
                            commandAndParameters << "true";
                            reverseCommandAndParameters << "false";
                        }
                        break;
                    }
                    case AgentIOPValueTypes::INTEGER: {
                        commandAndParameters << iopValueEffect->value();

                        // FIXME check if conversion succeeded
                        //reverseCommandAndParameters << QString::number(iopValueEffect->agentIOP()->currentValue().toInt());
                        reverseCommandAndParameters << iopValueEffect->agentIOP()->displayableCurrentValue();
                        break;
                    }
                    case AgentIOPValueTypes::DOUBLE: {
                        commandAndParameters << iopValueEffect->value();

                        // FIXME check if conversion succeeded
                        //reverseCommandAndParameters << QString::number(iopValueEffect->agentIOP()->currentValue().toDouble());
                        reverseCommandAndParameters << iopValueEffect->agentIOP()->displayableCurrentValue();
                        break;
                    }
                    case AgentIOPValueTypes::STRING: {
                        commandAndParameters << iopValueEffect->value();
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
                        command = "MAP";
                        reverseCommand = "UNMAP";
                        break;
                    }
                    case MappingEffectValues::UNMAPPED: {
                        command = "UNMAP";
                        reverseCommand = "MAP";
                        break;
                    }
                    default:
                        break;
                    }

                    commandAndParameters << command << mappingEffect->input()->name() << mappingEffect->outputAgent()->name() << mappingEffect->output()->name();
                    reverseCommandAndParameters << reverseCommand << mappingEffect->input()->name() << mappingEffect->outputAgent()->name() << mappingEffect->output()->name();
                }
                break;
            }
            default:
                break;
            }

            // Command (and parameters) are defined
            if (commandAndParameters.count() > 0)
            {
                qDebug() << pairCommandAndParameters.first << "Command (and parameters):" << commandAndParameters;

                pairCommandAndParameters.second = commandAndParameters;

                commandsForAgents.append(pairCommandAndParameters);
            }

            // Reverse command (and parameters) are defined
            if (_shallRevert && reverseCommandAndParameters.count() > 0)
            {
                qDebug() << pairReverseCommandAndParameters.first << "Reverse Command (and parameters):" << reverseCommandAndParameters;

                pairReverseCommandAndParameters.second = reverseCommandAndParameters;

                _reverseCommandsForAgents.append(pairReverseCommandAndParameters);
            }
        }
    }

    return commandsForAgents;
}


/**
 * @brief Get the list of Reverse commands
 * @return
 */
QList<QPair<QString, QStringList>> ActionExecutionVM::getReverseCommands()
{
    return _reverseCommandsForAgents;
}


