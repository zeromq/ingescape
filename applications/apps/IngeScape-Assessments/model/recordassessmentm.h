/*
 *	IngeScape Editor
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Natanael Vaugien    <vaugien@ingenuity.io>
 *
 */

#ifndef RECORDASSESSMENTM_H
#define RECORDASSESSMENTM_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/recordm.h>
#include <model/enums.h>
#include <cassandra.h>

class RecordAssessmentM : public RecordM
{
    Q_OBJECT

    // Start time in milliseconds of our record in timeline
    I2_QML_PROPERTY(int, startTimeInTimeline)

    // End time in milliseconds of our record in timeline
    I2_QML_PROPERTY(qint64, endTimeInTimeline)

public:
    explicit RecordAssessmentM(QString uid,
                      QString name,
                      QDateTime begin,
                      QDateTime end,
                      int startTimeInTimeline,
                      QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~RecordAssessmentM();

    /**
     * @brief Static factory method to create a record from a CassandraDB record
     * @param row
     * @return
     */
    static RecordAssessmentM* createFromCassandraRow(const CassRow* row);


    /**
     * @brief Record table name
     */
    static const QString table;

    /**
     * @brief Record table column names
     */
    static const QStringList columnNames;

    /**
     * @brief Record table primary keys IN ORDER
     */
    static const QStringList primaryKeys;
};

QML_DECLARE_TYPE(RecordAssessmentM)

#endif // RECORDASSESSMENTM_H
