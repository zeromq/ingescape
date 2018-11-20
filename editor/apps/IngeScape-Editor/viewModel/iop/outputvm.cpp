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

#include "outputvm.h"

/**
 * @brief Constructor
 * @param outputName
 * @param outputId
 * @param modelM
 * @param parent
 */
OutputVM::OutputVM(QString outputName,
                   QString outputId,
                   OutputM* modelM,
                   QObject *parent) : AgentIOPVM(outputName,
                                                 outputId,
                                                 modelM,
                                                 parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Output VM" << _name << "(" << _id << ")";
}


/**
 * @brief Destructor
 */
OutputVM::~OutputVM()
{
    //qInfo() << "Delete Output VM" << _name << "(" << _id << ")";

}
