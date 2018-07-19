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
 *
 */

#include "recordvm.h"

#include <QQmlEngine>


/**
 * @brief Default constructor
 * @param model
 * @param parent
 */
RecordVM::RecordVM(RecordM* model,
                   QObject *parent) : QObject(parent),
    _recordModel(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (model != NULL)
    {
        // Init model
        setrecordModel( model );
    }
}


/**
 * @brief Destructor
 */
RecordVM::~RecordVM()
{
    if (_recordModel != NULL)
    {
        qInfo() << "Delete View Model of Record" << _recordModel->name();
    }
    setrecordModel(NULL);
}

