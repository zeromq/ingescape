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

#include "inputvm.h"

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
                 QObject *parent) : AgentIOPVM(inputName,
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

    setfirstModel(nullptr);

    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &InputVM::_onModelsChanged);

    _models.clear();
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
        setfirstModel(nullptr);
    }
}
