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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
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

    // Model of our record
    I2_QML_PROPERTY_DELETE_PROOF(RecordM*, modelM)


public:

    /**
     * @brief Constructor
     * @param model
     * @param parent
     */
    explicit RecordVM(RecordM* model, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordVM();


Q_SIGNALS:


};

QML_DECLARE_TYPE(RecordVM)

#endif // RECORDVM_H
