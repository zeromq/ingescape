/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Bruno Lemenicier   <lemenicier@ingenuity.io>
 *
 */

#ifndef RECORDSSUPERVISIONCONTROLLER_H
#define RECORDSSUPERVISIONCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <viewModel/recordvm.h>
#include <model/recordm.h>


/**
 * @brief The RecordsSupervisionController class defines the controller for Records supervision
 */
class RecordsSupervisionController : public QObject
{
    Q_OBJECT

    // Sorted list of Record
    I2_QOBJECT_LISTMODEL(RecordVM, recordsList)

    // Selected Record in the Records list
    I2_QML_PROPERTY_DELETE_PROOF(RecordVM*, selectedRecord)

public:
    /**
     * @brief Default constructor
     * @param modelManager
     * @param parent
     */
    explicit RecordsSupervisionController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordsSupervisionController();




Q_SIGNALS:




public Q_SLOTS:


    /**
     * @brief Slot when a model of agent will be deleted
     * @param records
     */
    void onRecordsListChanged(QList<RecordM*> records);


private:

    // Map from record name to a list of view models of record
    QHash<RecordM*, RecordVM*> _mapFromRecordModelToViewModel;
};

QML_DECLARE_TYPE(RecordsSupervisionController)

#endif // RECORDSSUPERVISIONCONTROLLER_H
