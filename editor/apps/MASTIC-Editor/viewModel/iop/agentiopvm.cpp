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

#include "agentiopvm.h"

#include <QQmlEngine>
#include <QDebug>



/**
 * @brief Default constrcutor
 * @param model
 * @param parent
 */
AgentIOPVM::AgentIOPVM(AgentIOPM* model, QObject *parent) : QObject(parent),
    _modelM(model),
    _mappingValue(QVariant()),
    _displayableMappingValue(""),
    _currentValue(QVariant()),
    _displayableCurrentValue(""),
    _x(0),
    _y(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelM != NULL)
    {
        qInfo() << "New VM of agent" << AgentIOPTypes::staticEnumToString(_modelM->agentIOPType()) << _modelM->name()
                << "(" << AgentIOPValueTypes::staticEnumToString(_modelM->agentIOPValueType()) << ")" << _modelM->displayableDefaultValue();
    }
}


/**
 * @brief Destructor
 */
AgentIOPVM::~AgentIOPVM()
{
    if (_modelM != NULL)
    {
        qInfo() << "Delete VM of Agent" << AgentIOPTypes::staticEnumToString(_modelM->agentIOPType()) << _modelM->name()
                << "(" << AgentIOPValueTypes::staticEnumToString(_modelM->agentIOPValueType()) << ")" << _modelM->displayableDefaultValue();

        setmodelM(NULL);
    }
}

