/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
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

#include <viewModel/pointmapvm.h>
#include <model/iop/outputm.h>


/**
 * @brief The OutputVM class defines a view model of output
 */
class OutputVM : public PointMapVM
{
    Q_OBJECT

    // First model of our agent Output
    I2_QML_PROPERTY_READONLY_DELETE_PROOF(OutputM*, firstModel)

    // Models of our agent output
    I2_QOBJECT_LISTMODEL(OutputM, models)

    // Point map name (AgentIOP name) but needed for ghost input/output
    //I2_QML_PROPERTY_READONLY(bool, isGhost)

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
                      QString outputId = "",
                      OutputM* modelM = NULL,
                      QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~OutputVM();


Q_SIGNALS:

public Q_SLOTS:

    /**
     * @brief Return true if our output can link with the input (types are compatible)
     * @param pointMap
     * @return
     */
    bool canLinkWith(PointMapVM* pointMap) Q_DECL_OVERRIDE;


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
