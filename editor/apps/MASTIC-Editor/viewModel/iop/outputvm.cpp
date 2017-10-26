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

#include "outputvm.h"

OutputVM::OutputVM(QString agentName,
                   OutputM* modelM,
                   QObject *parent) : PointMapVM(agentName,
                                                 parent),
    _modelM(modelM)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setmodelM(modelM);

    if (_modelM != NULL) {
        qInfo() << "New Output VM" << _nameAgent << "." << _modelM->name();
    }
}


/**
 * @brief Destructor
 */
OutputVM::~OutputVM()
{
    if (_modelM != NULL) {
        qInfo() << "Delete Output VM" << _nameAgent << "." << _modelM->name();
    }

    setmodelM(NULL);
}
