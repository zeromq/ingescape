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
 *      Bruno Lemenicier    <lemenicier@ingenuity.io>
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

#ifndef RECORDM_H
#define RECORDM_H

#include <QObject>
#include <I2PropertyHelpers.h>

/**
 * @brief The RecordM class defines a model of record
 */
class RecordM : public QObject
{
    Q_OBJECT

    // Record id
    I2_QML_PROPERTY_READONLY(QString, id)

    // Record name
    I2_QML_PROPERTY_READONLY(QString, name)

    // Date of the beginning of the record
    I2_QML_PROPERTY_READONLY(QDateTime, beginDateTime)

    // Date of the end of the record
    I2_QML_PROPERTY_READONLY(QDateTime, endDateTime)

    // Duration of the record
    I2_QML_PROPERTY_READONLY(QDateTime, duration)



public:
    /**
     * @brief Constructor
     * @param id
     * @param name
     * @param begin
     * @param end
     * @param parent
     */
    explicit RecordM(QString id,
                     QString name,
                     QDateTime begin,
                     QDateTime end,
                     QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordM();

};

QML_DECLARE_TYPE(RecordM)

#endif // RECORDM_H
