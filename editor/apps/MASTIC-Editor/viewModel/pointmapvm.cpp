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

/**
 * @brief Default constructor
 * @param name
 * @param id
 * @param parent
 */
PointMapVM::PointMapVM(QString name,
                       QString id,
                       QObject *parent) : QObject(parent),
    _name(name),
    _id(id),
    _position(QPointF())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

   // qInfo() << "New Point Map VM" << _name << "(" << _id << ")";
}


PointMapVM::~PointMapVM()
{
   // qInfo() << "Delete Point Map VM" << _name << "(" << _id << ")";
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
