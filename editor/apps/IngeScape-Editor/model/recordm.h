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
    I2_QML_PROPERTY_READONLY(QString, beginDate)

    // Time of the beginning of the record
    I2_QML_PROPERTY_READONLY(QString, beginTime)

    // Date of the end of the record
    I2_QML_PROPERTY_READONLY(QString, endDate)

    // Time of the end of the record
    I2_QML_PROPERTY_READONLY(QString, endTime)



public:
    explicit RecordM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordM();

};

QML_DECLARE_TYPE(RecordM)

#endif // RECORDM_H
