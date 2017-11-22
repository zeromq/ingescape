#ifndef ACTIONEXECUTIONVM_H
#define ACTIONEXECUTIONVM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QPair>

#include <I2PropertyHelpers.h>
#include <viewModel/actioneffectvm.h>

/**
  * @brief Execution mode
  * Types:
  * - Normal: The process stops after the end of the action execution
  * - Revert: The process will try to revert the effects of the action.
  */
I2_ENUM_CUSTOM(ExecutionMode, NORMAL, REVERT)

/**
 * @brief The ActionExecutionVM class is the view model that support the representation of the action in the scenario
 */
class ActionExecutionVM
{
     Q_OBJECT

    // Execution mode
    I2_QML_PROPERTY(ExecutionMode::Value, executionMode)

    // Flag is waiting for the reverse effect.
    I2_QML_PROPERTY(bool, isWaitingRevert)

    // Flag is triggered when action has been executed
    I2_QML_PROPERTY(bool, isTriggered)

    // Time T3: execution time in millisecond.
    I2_QML_PROPERTY(int, executionTime)

    // Time T4: reverse time in millisecond.
    I2_QML_PROPERTY(int, reverseTime)

public:
    /**
      * @brief Constructor by default
      */
    ActionExecutionVM(ExecutionMode::Value executionMode, int executionTime, int reverseTime, QObject *parent = NULL);

    /**
      * @brief Destructor by default
      */
    ~ActionExecutionVM();

public:

    /**
     * @brief Add a new pair (peerID,parameters) to reverse the action's effect.
     * @param peerId of the target agent
     * @param effectToReverse: VM of the effect we want to reverse
     */
    void addReverseEffectsList(QString peerIdTargetAgent, ActionEffectVM* effectToReverseVM);
    
private:
    // List of the reverse effects to execute... QPair <peerId, parameters>.
    QList<QPair<QString, QString>> _reverseEffectsList;



};

QML_DECLARE_TYPE(ActionExecutionVM)

#endif // ACTIONEXECUTIONVM_H
