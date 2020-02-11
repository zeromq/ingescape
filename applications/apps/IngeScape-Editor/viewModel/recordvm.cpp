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
 * @param name
 * @param model
 * @param parent
 */
RecordVM::RecordVM(//QString name,
                   RecordM* model,
                   QObject *parent) : QObject(parent),
    _name(""),
    _modelM(model),
    _elapsedTime(QTime::fromMSecsSinceStartOfDay(0))
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelM != nullptr)
    {
        _name = _modelM->name();
    }
    qDebug() << "New View Model of Record" << _name;
}


/**
 * @brief Destructor
 */
RecordVM::~RecordVM()
{
    qDebug() << "Delete View Model of Record" << _name;

    if (_modelM != nullptr)
    {
        // The model of record is deleted elsewhere

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

