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
 *      Bruno Lemenicier   <lemenicier@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "recordssupervisioncontroller.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param modelManager
 * @param parent
 */
RecordsSupervisionController::RecordsSupervisionController(QObject *parent) : QObject(parent),
    _selectedRecord(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


}

/**
 * @brief RecordsSupervisionController::onRecordsListChanged
 * @param records
 */
void RecordsSupervisionController::onRecordsListChanged(QList<RecordM*> records)
{
    _recordsList.clear();

    foreach (RecordM* model, records) {
        RecordVM* vm = new RecordVM(model);
        _recordsList.append(vm);
    }
}


/**
 * @brief Destructor
 */
RecordsSupervisionController::~RecordsSupervisionController()
{
    // Clean-up current selection
    setselectedRecord(NULL);

    _mapFromRecordModelToViewModel.clear();

    // Delete all VM of host
    _recordsList.deleteAllItems();
}



