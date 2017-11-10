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
        qInfo() << "New Output VM" << _name << "(" << _id << ")";
    }
    else {
        _isGhost = true;
        qInfo() << "New ghost of Output VM" << _name;
    }

    // Connect to signal "Count Changed" from the list of models
    connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &OutputVM::_onModelsChanged);

    _models.append(modelM);
}


/**
 * @brief Destructor
 */
OutputVM::~OutputVM()
{
    if (_firstModel != NULL) {
        qInfo() << "Delete Output VM" << _name << "(" << _id << ")";

        setfirstModel(NULL);
    }
    else {
        qInfo() << "Delete ghost of Output VM" << _name;
    }

    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &OutputVM::_onModelsChanged);

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
    if (_models.count() > 0) {
        setfirstModel(_models.at(0));
    }
    else {
        setfirstModel(NULL);
    }

    /**QList<OutputM*> newModelsList = _models.toList();

    // Model of output added
    if (_previousModelsList.count() < newModelsList.count())
    {
        qDebug() << _previousModelsList.count() << "--> ADD --> " << newModelsList.count();

        for (OutputM* model : newModelsList) {
            if ((model != NULL) && !_previousModelsList.contains(model))
            {
                qDebug() << "New model" << model->name() << "ADDED";
            }
        }
    }
    // Model of output removed
    else if (_previousModelsList.count() > newModelsList.count())
    {
        qDebug() << _previousModelsList.count() << "--> REMOVE --> " << newModelsList.count();

        for (OutputM* model : _previousModelsList) {
            if ((model != NULL) && !newModelsList.contains(model))
            {
                qDebug() << "Old model" << model->name() << "REMOVED";
            }
        }
    }

    _previousModelsList = newModelsList;*/
}
