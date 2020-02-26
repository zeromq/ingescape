/*
 *	IngeScape Assessments
 *
 *  Copyright © 2020 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Chloé Roumieu   <roumieu@ingenuity.io>
 */

#ifndef ACTIONASSESSMENT_H
#define ACTIONASSESSMENT_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <cassandra.h>

/**
 * @brief The ActionAssessmentM class defines a model of action as in database
 */
class ActionAssessmentM : public QObject
{
    Q_OBJECT

    // Action's id
    I2_CPP_PROPERTY(int, actionId)

    // Action's line in timeline
    I2_QML_PROPERTY_READONLY(int, timelineLine)

    I2_QML_PROPERTY(int, executionTime)


public:

    /**
     * @brief Constructor
     */
    explicit ActionAssessmentM(CassUuid recordCassUuid,
                               CassUuid timeUuid,
                               int actionId,
                               int timelineLine,
                               QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ActionAssessmentM();


    /**
     * @brief Event table name
     */
    static const QString table;

    /**
     * @brief Event table column names
     */
    static const QStringList columnNames;

    /**
     * @brief Event table primary keys IN ORDER
     */
    static const QStringList primaryKeys;


    /**
     * @brief Static factory method to create an event from a CassandraDB record
     * @param row
     * @return
     */
    static ActionAssessmentM* createFromCassandraRow(const CassRow* row);


private:
    CassUuid _recordCassUuid;
    CassUuid _timeUuid;
};

QML_DECLARE_TYPE(ActionAssessmentM)

#endif // ACTIONASSESSMENT_H
