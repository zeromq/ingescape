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
 * @param uid
 * @param modelM
 * @param parent
 */
AgentIOPVM::AgentIOPVM(QString name,
                       QString uid,
                       AgentIOPM* modelM,
                       QObject *parent) : QObject(parent),
    _name(name),
    _uid(uid),
    _firstModel(nullptr),
    _isDefinedInAllDefinitions(true)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Agent I/O/P VM" << _name << "(" << _uid << ")";

    // Connect to signal "Count Changed" from the list of models
    connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentIOPVM::_onModelsChanged);

    if (modelM != nullptr) {
        _models.append(modelM);
    }
}


/**
 * @brief Destructor
 */
AgentIOPVM::~AgentIOPVM()
{
    //qInfo() << "Delete Agent I/O/P VM" << _name << "(" << _uid << ")";

    // DIS-connect to signal "Count Changed" from the list of models
    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentIOPVM::_onModelsChanged);

    // Reset the first model
    setfirstModel(nullptr);

    // Models are deleted elsewhere
    _previousModels.clear();
    _models.clear();
}


/**
 * @brief Slot called when the list of models changed
 */
void AgentIOPVM::_onModelsChanged()
{
    QList<AgentIOPM*> newModelsList = _models.toList();

    // Model of I/O/P added
    if (_previousModels.count() < newModelsList.count())
    {
        //qDebug() << _previousModels.count() << "--> ADD --> " << newModelsList.count();

        for (AgentIOPM* model : newModelsList)
        {
            if ((model != nullptr) && !_previousModels.contains(model))
            {
                //qDebug() << "Agent IOP VM: New model" << model->name() << "ADDED";

                // Connect to signals from this new model
                //connect(model, &AgentIOPM::currentValueChanged, this, &AgentIOPVM::_onCurrentValueOfModelChanged);
                connect(model, &AgentIOPM::currentValueChanged, this, &AgentIOPVM::currentValueChanged);
            }
        }
    }
    // Model of I/O/P removed
    else if (_previousModels.count() > newModelsList.count())
    {
        //qDebug() << _previousModels.count() << "--> REMOVE --> " << newModelsList.count();

        for (AgentIOPM* model : _previousModels)
        {
            if ((model != nullptr) && !newModelsList.contains(model))
            {
                //qDebug() << "Agent IOP VM: Old model" << model->name() << "REMOVED";

                // DIS-connect to signals from this previous model
                //disconnect(model, &AgentIOPM::currentValueChanged, this, &AgentIOPVM::_onCurrentValueOfModelChanged);
                disconnect(model, &AgentIOPM::currentValueChanged, this, &AgentIOPVM::currentValueChanged);
            }
        }
    }

    _previousModels = newModelsList;

    // Update the first model
    _updateFirstModel();
}


/**
 * @brief Slot called when the current value (of a model) changed
 * @param value
 */
/*void AgentIOPVM::_onCurrentValueOfModelChanged(QVariant value)
{
    AgentIOPM* model = qobject_cast<AgentIOPM*>(sender());
    if (model != nullptr) {
        qDebug() << "On Current Value of Model Changed" << model->name() << value.toString();
    }

    // ?
    //setcurrentValue(value);

    Q_EMIT currentValueChanged(value);
}*/


/**
 * @brief Update the first model
 */
void AgentIOPVM::_updateFirstModel()
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