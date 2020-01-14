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

#ifndef EVENTM_H
#define EVENTM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <cassandra.h>

/**
 * @brief The EventM class defines a model of event as in database
 */
class EventM : public QObject
{
    Q_OBJECT

    // Agent of the event
    I2_CPP_PROPERTY(QString, agent)

    // Output of the event
    I2_CPP_PROPERTY(QString, output)

    // Date of the event execution
    I2_CPP_PROPERTY(QDateTime, executionDateTime)

    // Type of the event
    I2_CPP_PROPERTY(int, type)


public:

    /**
     * @brief Constructor
     */
    explicit EventM(CassUuid recordCassUuid,
                    CassUuid timeUuid,
                    QString agent,
                    QString output,
                    QDateTime executionDateTime,
                    int type,
                    QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~EventM();


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
    static EventM* createFromCassandraRow(const CassRow* row);


    /**
     * @brief Accessor for the time's Cassandra UUID of this entry
     * @return
     */
    CassUuid getTimeCassUuid() const { return _timeUuid; }


private:
    CassUuid _recordCassUuid;
    CassUuid _timeUuid;
};

QML_DECLARE_TYPE(EventM)

#endif // EVENTM_H
