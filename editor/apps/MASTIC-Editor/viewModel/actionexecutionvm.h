#ifndef ACTIONEXECUTIONVM_H
#define ACTIONEXECUTIONVM_H

#include <QObject>
#include <QtQml>
#include <QPair>

#include <I2PropertyHelpers.h>
#include <viewModel/actioneffectvm.h>

/**
 * @brief The ActionExecutionVM class is the view model that support the representation of the action in the scenario
 */
class ActionExecutionVM : public QObject
{
     Q_OBJECT

    // Flag indicating if our action has a revert effect
    I2_QML_PROPERTY(bool, hasRevert)

    // Flag indicating if we are waiting for the revert effect
    I2_QML_PROPERTY(bool, isWaitingRevert)

    // Flag indicating if our action has been executed
    I2_QML_PROPERTY(bool, isTriggered)

    // Execution time in millisecond
    I2_QML_PROPERTY(int, executionTime)

    // Reverse time in millisecond.
    I2_QML_PROPERTY(int, reverseTime)


public:
    /**
     * @brief Constructor
     * @param hasRevert
     * @param executionTime
     * @param reverseTime
     * @param parent
     */
    ActionExecutionVM(bool hasRevert,
                      int executionTime,
                      int reverseTime,
                      QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ActionExecutionVM();


    /**
     * @brief Get the list of commands for the list of effects (and eventually init the list of reverse commands)
     * @param effectsList
     * @return
     */
    QList<QPair<QString, QString>> getCommandsForEffectsAndInitReverseCommands(QList<ActionEffectVM*> effectsList);


    /**
     * @brief Get the list of Reverse commands
     * @return
     */
    QList<QPair<QString, QString>> getReverseCommands();


    /**
     * @brief Add a new pair (peerID,parameters) to reverse the action's effect.
     * @param peerId of the target agent
     * @param effectToReverse: VM of the effect we want to reverse
     */
    //void addReverseEffectsList(QString peerIdTargetAgent, ActionEffectVM* effectToReverseVM);
    

private:

    // List of pairs <agent name, reverse command>
    QList<QPair<QString, QString>> _reverseCommandsForAgents;

};

QML_DECLARE_TYPE(ActionExecutionVM)

#endif // ACTIONEXECUTIONVM_H
