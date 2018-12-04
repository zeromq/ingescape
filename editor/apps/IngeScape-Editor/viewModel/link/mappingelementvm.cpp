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

#include "mappingelementvm.h"

/**
 * @brief Constructor
 * @param name
 * @param modelM
 * @param parent
 */
MappingElementVM::MappingElementVM(QString name,
                                   ElementMappingM* modelM,
                                   QObject *parent) : QObject(parent),
    _name(name),
    _firstModel(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Mapping Element VM" << _name;

    // Connect to signal "Count Changed" from the list of models
    connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &MappingElementVM::_onModelsChanged);

    if (modelM != nullptr) {
        _models.append(modelM);
    }
}


/**
 * @brief Destructor
 */
MappingElementVM::~MappingElementVM()
{
    qInfo() << "Delete Mapping Element VM" << _name;

    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &MappingElementVM::_onModelsChanged);

    // Reset the first model
    setfirstModel(nullptr);

    // Models are deleted elsewhere
    _models.clear();
}


/**
 * @brief Slot called when the list of models changed
 */
void MappingElementVM::_onModelsChanged()
{
    if (!_models.isEmpty())
    {
        setfirstModel(_models.at(0));
    }
    else
    {
        setfirstModel(nullptr);
    }
}
