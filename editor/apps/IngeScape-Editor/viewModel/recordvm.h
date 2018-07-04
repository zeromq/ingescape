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
 *      Bruno Lemenicier <lemenicier@ingenuity.io>
 *
 */

#ifndef RECORDVM_H
#define RECORDVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include "model/recordm.h"
#include "model/agentm.h"

/**
 * @brief The RecordVM class defines a view model of record in the supervision list
 */
class RecordVM : public QObject
{
    Q_OBJECT

    // FIXME: modelM instead of recordModel
    // Model of our record
    I2_QML_PROPERTY_DELETE_PROOF(RecordM*, recordModel)

public:

    /**
     * @brief HostVM Default constructor
     * @param parent
     */
    explicit RecordVM(RecordM* model, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~RecordVM();


Q_SIGNALS:

//    /**
//     * @brief Signal emitted when a command must be sent on the network to a launcher
//     * @param command
//     * @param hostname
//     * @param commandLine
//     */
//    void commandAskedToHost(QString command, QString hostname, QString commandLine);

};

QML_DECLARE_TYPE(RecordVM)

#endif // RECORDVM_H
