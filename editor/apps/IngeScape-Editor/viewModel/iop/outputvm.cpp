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

#include "outputvm.h"

/**
 * @brief Constructor
 * @param outputName
 * @param outputId
 * @param modelM
 * @param parent
 */
OutputVM::OutputVM(QString outputName,
                   QString outputId,
                   OutputM* modelM,
                   QObject *parent) : AgentIOPVM(outputName,
                                                 outputId,
                                                 parent),
    _firstModel(NULL),
    _isPublishedNewValue(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Output VM" << _name << "(" << _id << ")";

    // Init the timer to reset the flag "is Published New Value"
    // Allows to play an animation when the value changed
    _timer.setInterval(500);
    connect(&_timer, &QTimer::timeout, this, &OutputVM::_onTimeout);


    // Connect to signal "Count Changed" from the list of models
    connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &OutputVM::_onModelsChanged);

    _models.append(modelM);
}


/**
 * @brief Destructor
 */
OutputVM::~OutputVM()
{
    //
    // Stop and clean the timer
    //
    _timer.stop();
    disconnect(&_timer, 0, this, 0);

    //qInfo() << "Delete Output VM" << _name << "(" << _id << ")";

    setfirstModel(nullptr);

    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &OutputVM::_onModelsChanged);

    _previousModelsList.clear();
    _models.clear();
}


// FIXME: rather in LinkOutputVM than in OutputVM
/**
 * @brief Simulate that the current value of model changed: allows to highlight the corresponding link(s)
 */
void OutputVM::simulateCurrentValueOfModelChanged()
{
    _onCurrentValueOfModelChanged(QVariant());
}


/**
 * @brief Slot when the list of models changed
 */
void OutputVM::_onModelsChanged()
{
    // Update the first model
    if (!_models.isEmpty()) {
        setfirstModel(_models.at(0));
    }
    else {
        setfirstModel(nullptr);
    }

    //
    QList<OutputM*> newModelsList = _models.toList();

    // Model of output added
    if (_previousModelsList.count() < newModelsList.count())
    {
        //qDebug() << _previousModelsList.count() << "--> ADD --> " << newModelsList.count();

        for (OutputM* model : newModelsList) {
            if ((model != nullptr) && !_previousModelsList.contains(model))
            {
                //qDebug() << "Output VM: New model" << model->name() << "ADDED";

                // Connect to signals from this new model
                connect(model, &OutputM::currentValueChanged, this, &OutputVM::_onCurrentValueOfModelChanged);
            }
        }
    }
    // Model of output removed
    else if (_previousModelsList.count() > newModelsList.count())
    {
        //qDebug() << _previousModelsList.count() << "--> REMOVE --> " << newModelsList.count();

        for (OutputM* model : _previousModelsList) {
            if ((model != nullptr) && !newModelsList.contains(model))
            {
                //qDebug() << "Output VM: Old model" << model->name() << "REMOVED";

                // DIS-connect to signals from this previous model
                disconnect(model, &OutputM::currentValueChanged, this, &OutputVM::_onCurrentValueOfModelChanged);
            }
        }
    }

    _previousModelsList = newModelsList;
}


/**
 * @brief Slot when the current value (of a model) changed
 * @param value
 */
void OutputVM::_onCurrentValueOfModelChanged(QVariant value)
{
    Q_UNUSED(value)

    /*OutputM* model = qobject_cast<OutputM*>(sender());
    if (model != nullptr) {
        qDebug() << "On Current Value of Model Changed" << model->name() << value.toString();
    }*/

    // Check that the flag is not already to true
    if (!_isPublishedNewValue) {
        setisPublishedNewValue(true);

        // Start the timer to reset the flag "is Published New Value"
        _timer.start();
    }
}


/**
 * @brief Slot when the timer time out
 */
void OutputVM::_onTimeout()
{
    // Stop the timer
    _timer.stop();

    if (_isPublishedNewValue) {
        // Reset the flag
        setisPublishedNewValue(false);
    }
}
