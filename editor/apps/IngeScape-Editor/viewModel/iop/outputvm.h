/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#ifndef OUTPUTVM_H
#define OUTPUTVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/iop/outputm.h>
#include <viewModel/iop/agentiopvm.h>


/**
 * @brief The OutputVM class defines a view model of output
 */
class OutputVM : public AgentIOPVM
{
    Q_OBJECT

    // First model of our agent Output
    I2_QML_PROPERTY_READONLY_DELETE_PROOF(OutputM*, firstModel)

    // Models of our agent output
    I2_QOBJECT_LISTMODEL(OutputM, models)

    // FIXME: rather in LinkOutputVM than in OutputVM
    // Flag indicating if a new value is published on our output
    I2_QML_PROPERTY_READONLY(bool, isPublishedNewValue)


public:
    /**
     * @brief Constructor
     * @param outputName
     * @param outputId
     * @param modelM
     * @param parent
     */
    explicit OutputVM(QString outputName,
                      QString outputId,
                      OutputM* modelM,
                      QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~OutputVM();


    // FIXME: rather in LinkOutputVM than in OutputVM
    /**
     * @brief Simulate that the current value of model changed: allows to highlight the corresponding link(s)
     */
    void simulateCurrentValueOfModelChanged();


Q_SIGNALS:


public Q_SLOTS:


private Q_SLOTS:
    /**
     * @brief Slot when the list of models changed
     */
    void _onModelsChanged();


    /**
     * @brief Slot when the current value (of a model) changed
     * @param value
     */
    void _onCurrentValueOfModelChanged(QVariant value);


    /**
     * @brief Slot when the timer time out
     */
    void _onTimeout();


private:
    // Previous list of models
    QList<OutputM*> _previousModelsList;

    // Timer to reset the flag "is Published New Value"
    // Allows to play an animation when the value changed
    QTimer _timer;

};

QML_DECLARE_TYPE(OutputVM)

#endif // OUTPUTVM_H
