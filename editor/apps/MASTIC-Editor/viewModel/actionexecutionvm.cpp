#include "actionexecutionvm.h"

/**
  * @brief Constructor by default
  */
ActionExecutionVM::ActionExecutionVM(bool hasRevert, int executionTime, int reverseTime, QObject *parent):QObject(parent),
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
  * @brief Destructor by default
  */
ActionExecutionVM::~ActionExecutionVM()
{

}

/**
 * @brief Add a new pair (peerID,parameters) to reverse the action's effect.
 * @param peerId of the target agent
 * @param effectToReverse: VM of the effect we want to reverse
 */
void ActionExecutionVM::addReverseEffectsList(QString peerIdTargetAgent, ActionEffectVM *effectToReverseVM)
{
    QPair<QString, QString> newReverseEffectElement;
    if(!peerIdTargetAgent.isEmpty() || !peerIdTargetAgent.isNull() || effectToReverseVM != NULL)
    {
        // Create first
        newReverseEffectElement.first = peerIdTargetAgent;

        // Create second
        QString parameters;

        //Get the effect
        ActionEffectM* effectToReverseM = effectToReverseVM->effect();
        switch (effectToReverseVM->effectType())
        { 
            case ActionEffectType::AGENT:
                switch (effectToReverseM->effect()) {
                    case ActionEffectValueType::ON:
                        parameters = "DIE";
                        break;
                    case ActionEffectValueType::OFF:
                        parameters = "RUN";
                        break;
                }
                break;

            case ActionEffectType::MAPPING:
                //Try to cast as Mapping
                MappingEffectM* mappingEffectToReverseM = dynamic_cast<MappingEffectM*>(effectToReverseM);
                if(mappingEffectToReverseM != NULL)
                {
                    switch (mappingEffectToReverseM->effect()) {
                    case ActionEffectValueType::ENABLE:
                        parameters = QString("%s %s %s %S").arg("UNMAP",
                                                                mappingEffectToReverseM->toAgentIOP()->name(),
                                                                mappingEffectToReverseM->agentModel()->name(),
                                                                mappingEffectToReverseM->fromAgentIOP()->name());
                        break;

                    case ActionEffectValueType::DISABLE:
                        parameters = QString("%s %s %s %S").arg("MAP",
                                                                mappingEffectToReverseM->toAgentIOP()->name(),
                                                                mappingEffectToReverseM->agentModel()->name(),
                                                                mappingEffectToReverseM->fromAgentIOP()->name());

                        break;
                    }
                }
                break;

//            case ActionEffectType::VALUE:
//                //Try to cast as Mapping
//                IOPValueEffectM* valueEffectToReverseM = dynamic_cast<IOPValueEffectM*>(effectToReverseM);
//                if(valueEffectToReverseM != NULL)
//                {
//                    switch (valueEffectToReverseM->agentIOP()->agentIOPValueType()) {
//                    case AgentIOPValueTypes::BOOL:
//                        if(valueEffectToReverseM->agentIOP()->defaultValue().toBool()) parameters = "true";
//                        else parameters = "false";
//                        break;
//                    case AgentIOPValueTypes::INTEGER:
//                        parameters = QString::number(valueEffectToReverseM->agentIOP()->defaultValue().toInt());
//                        break;
//                    case AgentIOPValueTypes::DOUBLE:
//                        parameters = QString::number(valueEffectToReverseM->agentIOP()->defaultValue().toDouble());
//                        break;
//                    case AgentIOPValueTypes::STRING:
//                        parameters = valueEffectToReverseM->agentIOP()->defaultValue().toString();
//                        break;
//                    }
//                }
//                break;
        }

        if(!parameters.isEmpty() || !parameters.isNull())
        {
            //
            newReverseEffectElement.second = parameters;

            //add the pair in internal list
            _reverseEffectsList.append(newReverseEffectElement);
        }
    }

}

