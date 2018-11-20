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
 *
 */

#include "parametervm.h"

/**
 * @brief Constructor
 * @param parameterName
 * @param parameterId
 * @param modelM
 * @param parent
 */
ParameterVM::ParameterVM(QString parameterName,
                         QString parameterId,
                         AgentIOPM* modelM,
                         QObject *parent) : AgentIOPVM(parameterName,
                                                       parameterId,
                                                       modelM,
                                                       parent)
    //_firstModel(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Parameter VM" << _name << "(" << _id << ")";

    // Connect to signal "Count Changed" from the list of models
    //connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &ParameterVM::_onModelsChanged);

    //_models.append(modelM);
}


/**
 * @brief Destructor
 */
ParameterVM::~ParameterVM()
{
    //qInfo() << "Delete Parameter VM" << _name << "(" << _id << ")";

    //setfirstModel(nullptr);

    // DIS-connect to signal "Count Changed" from the list of models
    //disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &ParameterVM::_onModelsChanged);

    //_models.clear();
}


/**
 * @brief Slot when the list of models changed
 */
/*void ParameterVM::_onModelsChanged()
{
    // Update the first model
    if (!_models.isEmpty()) {
        setfirstModel(_models.at(0));
    }
    else {
        setfirstModel(nullptr);
    }
}*/
