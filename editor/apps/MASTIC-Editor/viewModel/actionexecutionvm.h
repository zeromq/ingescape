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

    // Flag indicating if we shall revert effect(s) of our action
    I2_QML_PROPERTY(bool, shallRevert)

    // Flag indicating if we are waiting to revert effect(s) of our action
    I2_QML_PROPERTY(bool, isWaitingRevert)

    // Flag indicating if our action has been executed
    I2_QML_PROPERTY(bool, isExecuted)

    // Execution time in millisecond
    I2_QML_PROPERTY(int, executionTime)

    // Reverse time in millisecond.
    I2_QML_PROPERTY(int, reverseTime)


public:
    /**
     * @brief Constructor
     * @param shallRevert
     * @param executionTime
     * @param reverseTime
     * @param parent
     */
    ActionExecutionVM(bool shallRevert,
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
    

private:

    // List of pairs <agent name, reverse command>
    QList<QPair<QString, QString>> _reverseCommandsForAgents;

};

QML_DECLARE_TYPE(ActionExecutionVM)

#endif // ACTIONEXECUTIONVM_H
