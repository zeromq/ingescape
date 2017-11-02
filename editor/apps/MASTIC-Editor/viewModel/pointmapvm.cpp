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
 *
 */

#include "pointmapvm.h"

PointMapVM::PointMapVM(QString iopName, QObject *parent) : QObject(parent),
    _iopName(iopName),
    _position(QPointF())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "Create new PointMap VM" << _iopName;
}


PointMapVM::~PointMapVM()
{
    qInfo() << "Delete PointMap VM" << _iopName;
}


/**
 * @brief Return true if the type of the output is compatible with the type of the input
 * @param outputValueType
 * @param inputValueType
 * @return
 */
bool PointMapVM::_canLinkOutputToInput(AgentIOPValueTypes::Value outputValueType, AgentIOPValueTypes::Value inputValueType)
{
    bool result = false;

    // Identical
    if (outputValueType == inputValueType) {
        result = true;
    }
    // Implicit conversions
    else
    {
        switch (outputValueType)
        {
        case AgentIOPValueTypes::INTEGER: {
            if (inputValueType != AgentIOPValueTypes::DATA) {
                result = true;
            }
            break;
        }

        case AgentIOPValueTypes::DOUBLE: {
            if (inputValueType != AgentIOPValueTypes::DATA) {
                result = true;
            }
            break;
        }

        case AgentIOPValueTypes::BOOL: {
            if (inputValueType != AgentIOPValueTypes::DATA) {
                result = true;
            }
            break;
        }

        case AgentIOPValueTypes::STRING: {
            if (inputValueType == AgentIOPValueTypes::IMPULSION) {
                result = true;
            }
            break;
        }

        case AgentIOPValueTypes::IMPULSION: {
            if (inputValueType != AgentIOPValueTypes::DATA) {
                result = true;
            }
            break;
        }

        /*case AgentIOPValueTypes::DATA: {
            break;
        }*/

        default:
            break;
        }
    }
    return result;
}
