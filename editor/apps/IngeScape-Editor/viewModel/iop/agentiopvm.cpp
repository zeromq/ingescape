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
 *
 */

#include "agentiopvm.h"

/**
 * @brief Constructor
 * @param name
 * @param id
 * @param modelM
 * @param parent
 */
AgentIOPVM::AgentIOPVM(QString name,
                       QString id,
                       AgentIOPM* modelM,
                       QObject *parent) : QObject(parent),
    _name(name),
    _id(id),
    _firstModel(nullptr),
    _isDefinedInAllDefinitions(true)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Agent I/O/P VM" << _name << "(" << _id << ")";

    // Connect to signal "Count Changed" from the list of models
    connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentIOPVM::_onModelsChanged);

    _models.append(modelM);
}


/**
 * @brief Destructor
 */
AgentIOPVM::~AgentIOPVM()
{
    //qInfo() << "Delete Agent I/O/P VM" << _name << "(" << _id << ")";

    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentIOPVM::_onModelsChanged);

    // Reset the first model
    setfirstModel(nullptr);

    // Models are deleted elsewhere
    _models.clear();
}


/**
 * @brief Slot called when the list of models changed
 */
void AgentIOPVM::_onModelsChanged()
{
    // Update the first model
    if (!_models.isEmpty()) {
        setfirstModel(_models.at(0));
    }
    else {
        setfirstModel(nullptr);
    }
}
