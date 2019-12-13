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
#include <cassandra.h>

class RecordAssessmentM : public RecordM
{
    Q_OBJECT

public:
    explicit RecordAssessmentM(QString uid,
                      QString name,
                      QDateTime begin,
                      QDateTime end,
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
