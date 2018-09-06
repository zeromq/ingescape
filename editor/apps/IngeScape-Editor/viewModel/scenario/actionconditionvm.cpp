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

#include "actionconditionvm.h"

#include <QDebug>

/**
 * @brief Enum "ActionConditionTypes" to string
 * @param value
 * @return
 */
QString ActionConditionTypes::enumToString(int value)
{
    switch (value)
    {
    case ActionConditionTypes::VALUE:
        return tr("Value");

    case ActionConditionTypes::AGENT:
        return tr("Agent");

    default:
        return "";
    }
}


//--------------------------------------------------------------
//
// ActionConditionVM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionConditionVM::ActionConditionVM(QObject *parent) : QObject(parent),
    _modelM(NULL),
    _conditionType(ActionConditionTypes::VALUE)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
ActionConditionVM::~ActionConditionVM()
{
    if (_modelM != NULL)
    {
        ActionConditionM* tmp = _modelM;
        setmodelM(NULL);
        delete tmp;
        tmp = NULL;
    }
}


/**
 * @brief Custom setter on the condition type
 */
void ActionConditionVM::setconditionType(ActionConditionTypes::Value value)
{
    if(_conditionType != value)
    {
        _conditionType = value;

        // configure the new type
        _configureToType(value);

        Q_EMIT conditionTypeChanged(value);
    }
}


/**
 * @brief Configure action condition VM into a specific type
 */
void ActionConditionVM::_configureToType(ActionConditionTypes::Value value)
{
    AgentInMappingVM* agent = NULL;

    // Delete the old condition if exists
    if (_modelM != NULL)
    {
        // Save the agent
        agent = _modelM->agent();

        ActionConditionM* tmp = _modelM;
        setmodelM(NULL);
        delete tmp;
        tmp = NULL;
    }

    // Create the new model of condition
    switch (value)
    {
        // AGENT
        case ActionConditionTypes::AGENT:
        {
            setmodelM(new ConditionOnAgentM());
            _modelM->setagent(agent);
            break;
        }
        // VALUE
        case ActionConditionTypes::VALUE:
        {
            setmodelM(new IOPValueConditionM());
            _modelM->setagent(agent);
            break;
        }

        default:
            break;
    }
}


