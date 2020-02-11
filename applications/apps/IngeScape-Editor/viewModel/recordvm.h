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

#include <model/recordm.h>
#include <model/enums.h>


/**
 * @brief The RecordVM class defines a view model of record in the supervision list
 */
class RecordVM : public QObject
{
    Q_OBJECT

    // Name of our record
    I2_QML_PROPERTY_READONLY(QString, name)

    // Model of our record
    I2_QML_PROPERTY_DELETE_PROOF(RecordM*, modelM)

    // Elapsed time (from the beginning of the current replay)
    // Define a QTime and a QDateTime: Manage a date in addition to the time to prevent a delta in hours between JS (QML) and C++
    I2_QML_PROPERTY_QTime(elapsedTime)

    // The value will never change (get from the model)
    Q_PROPERTY(QDateTime endDateTime READ endDateTime CONSTANT)


public:

    /**
     * @brief Constructor
     * @param name
     * @param model
     * @param parent
     */
    explicit RecordVM(//QString name,
                      RecordM* model,
                      QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordVM();


    /**
     * @brief Get "End Date Time" from model
     * @return
     */
    QDateTime endDateTime() const;


Q_SIGNALS:


};

QML_DECLARE_TYPE(RecordVM)

#endif // RECORDVM_H
