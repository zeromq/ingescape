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

    // Flag is
    I2_QML_PROPERTY(bool, hasRevert)

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
    ActionExecutionVM(bool hasRevert, int executionTime, int reverseTime, QObject *parent = 0);

    /**
      * @brief Destructor by default
      */
    ~ActionExecutionVM();

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
