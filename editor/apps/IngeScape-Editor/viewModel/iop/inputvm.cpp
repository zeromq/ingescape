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

#include "inputvm.h"
#include <viewModel/iop/outputvm.h>

/**
 * @brief Constructor
 * @param inputName
 * @param inputId
 * @param modelM
 * @param parent
 */
InputVM::InputVM(QString inputName,
                 QString inputId,
                 AgentIOPM* modelM,
                 QObject *parent) : PointMapVM(inputName,
                                               inputId,
                                               parent),
    _firstModel(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Input VM" << _name << "(" << _id << ")";

    // Connect to signal "Count Changed" from the list of models
    connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &InputVM::_onModelsChanged);

    _models.append(modelM);
}


/**
 * @brief Destructor
 */
InputVM::~InputVM()
{
    //qInfo() << "Delete Input VM" << _name << "(" << _id << ")";

    setfirstModel(NULL);

    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &InputVM::_onModelsChanged);

    _models.clear();
}


/**
 * @brief Return true if our input can link with the output (types are compatible)
 * @param pointMap
 * @return
 */
bool InputVM::canLinkWith(PointMapVM* pointMap)
{
    OutputVM* output = qobject_cast<OutputVM*>(pointMap);
    if ((output != NULL) && (output->firstModel() != NULL)
            && (_firstModel != NULL))
    {
        // Call parent class function
        return _canLinkOutputToInput(output->firstModel()->agentIOPValueType(), _firstModel->agentIOPValueType());
    }
    else {
        return false;
    }
}


/**
 * @brief Slot when the list of models changed
 */
void InputVM::_onModelsChanged()
{
    // Update the first model
    if (!_models.isEmpty()) {
        setfirstModel(_models.at(0));
    }
    else {
        setfirstModel(NULL);
    }
}
