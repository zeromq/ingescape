/*
 *	MASTIC Editor
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

#include "outputvm.h"
#include <viewModel/iop/inputvm.h>

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
                   QObject *parent) : PointMapVM(outputName,
                                                 outputId,
                                                 parent),
    _firstModel(NULL),
    _isGhost(false),
    _isPublishedNewValue(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (modelM != NULL) {
        //qInfo() << "New Output VM" << _name << "(" << _id << ")";
    }
    else {
        _isGhost = true;
        qInfo() << "New ghost of Output VM" << _name;
    }

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

    // Reset the first model
    if (_firstModel != NULL) {
        //qInfo() << "Delete Output VM" << _name << "(" << _id << ")";

        setfirstModel(NULL);
    }
    else {
        qInfo() << "Delete ghost of Output VM" << _name;
    }

    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &OutputVM::_onModelsChanged);

    _previousModelsList.clear();
    _models.clear();
}


/**
 * @brief Return true if our output can link with the input (types are compatible)
 * @param pointMap
 * @return
 */
bool OutputVM::canLinkWith(PointMapVM* pointMap)
{
    InputVM* input = qobject_cast<InputVM*>(pointMap);
    if ((input != NULL) && (input->firstModel() != NULL)
            && (_firstModel != NULL))
    {
        // Call parent class function
        return _canLinkOutputToInput(_firstModel->agentIOPValueType(), input->firstModel()->agentIOPValueType());
    }
    else {
        return false;
    }
}


/**
 * @brief Slot when the list of models changed
 */
void OutputVM::_onModelsChanged()
{
    // Update the first model
    if (_models.count() > 0) {
        setfirstModel(_models.at(0));
    }
    else {
        setfirstModel(NULL);
    }

    //
    QList<OutputM*> newModelsList = _models.toList();

    // Model of output added
    if (_previousModelsList.count() < newModelsList.count())
    {
        //qDebug() << _previousModelsList.count() << "--> ADD --> " << newModelsList.count();

        for (OutputM* model : newModelsList) {
            if ((model != NULL) && !_previousModelsList.contains(model))
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
            if ((model != NULL) && !newModelsList.contains(model))
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
    if (model != NULL) {
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
