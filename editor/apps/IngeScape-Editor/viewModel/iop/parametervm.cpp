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
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Parameter VM" << _name << "(" << _id << ")";

}


/**
 * @brief Destructor
 */
ParameterVM::~ParameterVM()
{
    //qInfo() << "Delete Parameter VM" << _name << "(" << _id << ")";

}
