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
 *      Bruno Lemenicier <lemenicier@ingenuity.io>
 *      Alexandre Lemort <lemort@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "recordvm.h"

#include <QQmlEngine>


/**
 * @brief Constructor
 * @param model
 * @param parent
 */
RecordVM::RecordVM(RecordM* model,
                   QObject *parent) : QObject(parent),
    _modelM(model)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelM != NULL) {
        qDebug() << "New View Model of Record" << _modelM->name();
    }
}


/**
 * @brief Destructor
 */
RecordVM::~RecordVM()
{
    if (_modelM != NULL) {
        qDebug() << "Delete View Model of Record" << _modelM->name();
        setmodelM(nullptr);
    }
}

