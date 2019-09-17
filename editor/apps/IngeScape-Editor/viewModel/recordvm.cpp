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
    _modelM(model),
    _elapsedTime(QTime::fromMSecsSinceStartOfDay(0))
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelM != nullptr) {
        qDebug() << "New View Model of Record" << _modelM->name();
    }
}


/**
 * @brief Destructor
 */
RecordVM::~RecordVM()
{
    if (_modelM != nullptr) {
        qDebug() << "Delete View Model of Record" << _modelM->name();
        setmodelM(nullptr);
    }
}


/**
 * @brief Get "End Date Time" from model
 * @return
 */
QDateTime RecordVM::endDateTime() const
{
    if (_modelM != nullptr) {
        return _modelM->endDateTime();
    }
    else {
         return QDateTime();
    }
}

