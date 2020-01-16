/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef SESSION_M_H
#define SESSION_M_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/subject/subjectm.h>
#include <model/protocol/protocolm.h>
#include <model/assessmentsenums.h>
#include <model/recordassessmentm.h>


/**
 * @brief The SessionM class defines a model of session
 */
class SessionM : public QObject
{
    Q_OBJECT

    // Name of our session
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, name)

    // User comments
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, comments)

    // Subject of our session
    I2_QML_PROPERTY_DELETE_PROOF(SubjectM*, subject)

    // Protocol of our session
    I2_QML_PROPERTY_DELETE_PROOF(ProtocolM*, protocol)

    // Start date and time of our session
    I2_QML_PROPERTY(QDateTime, startDateTime)

    // End date and time of our session
    I2_QML_PROPERTY_CUSTOM_SETTER(QDateTime, endDateTime)

    // Duration of our session
    I2_QML_PROPERTY_QTime(duration)
    //I2_QML_PROPERTY(QDateTime, duration)

    // Values of the independent variables of the protocol
    // "Qml Property Map" allows to set key-value pairs that can be used in QML bindings
    I2_QML_PROPERTY_READONLY(QQmlPropertyMap*, mapIndependentVariableValues)

    // List of records of our session
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(RecordAssessmentM, recordsList)

    // Flag indicating if our session is recorded
    I2_QML_PROPERTY(bool, isRecorded)


public:

    /**
     * @brief Constructor
     * @param experimentationUuid
     * @param cassUuid
     * @param name
     * @param comments
     * @param subjectUuid
     * @param protocolUuid
     * @param startDateTime
     * @param parent
     */
    explicit SessionM(CassUuid experimentationUuid,
                      CassUuid cassUuid,
                      QString name,
                      QString comments,
                      CassUuid subjectUuid,
                      CassUuid protocolUuid,
                      QDateTime startDateTime,
                      QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~SessionM();

    /**
     * @brief Session table name
     */
    static const QString table;

    /**
     * @brief Session table column names
     */
    static const QStringList columnNames;

    /**
     * @brief Session table primary keys IN ORDER
     */
    static const QStringList primaryKeys;

    /**
     * @brief Accessor for the Cassandra UUID of this entry
     * @return
     */
    CassUuid getCassUuid() const { return _cassUuid; }

    /**
     * @brief Accessor for the experimentation's Cassandra UUID of this entry
     * @return
     */
    CassUuid getExperimentationCassUuid() const { return _experimentationCassUuid; }

    /**
     * @brief Accessor for the protocol's Cassandra UUID of this entry
     * @return
     */
    CassUuid getProtocolCassUuid() const { return _protocolUuid; }

    /**
     * @brief Accessor for the subject's Cassandra UUID of this entry
     * @return
     */
    CassUuid getSubjectCassUuid() const { return _subjectUuid; }

    /**
     * @brief Set the value of the given independent variable into the QQmlPropertyMap
     * @param indepVar
     * @param value
     */
    void setIndependentVariableValue(IndependentVariableM* indepVar, const QString& value);

    /**
     * @brief Static factory method to create a session from a CassandraDB record
     * @param row
     * @return
     */
    static SessionM* createFromCassandraRow(const CassRow* row);

    /**
     * @brief Delete the given session from Cassandra DB
     * @param session
     */
    static void deleteSessionFromCassandra(const SessionM& session);

    /**
     * @brief Create a CassStatement to insert a SessionM into the DB.
     * The statement contains the values from the given session.
     * Passed session must have a valid and unique UUID.
     * @param session
     * @return
     */
    static CassStatement* createBoundInsertStatement(const SessionM& session);

    /**
     * @brief Create a CassStatement to update a SessionM into the DB.
     * The statement contains the values from the given session.
     * Passed session must have a valid and unique UUID.
     * @param session
     * @return
     */
    static CassStatement* createBoundUpdateStatement(const SessionM& session);


private Q_SLOTS:
    /**
     * @brief Slot called when a value of the Qml Property Map "map Independent Variable Values" changed
     * @param key
     * @param value
     */
    void _onIndependentVariableValueChanged(const QString& key, const QVariant& value);


private:
    /**
     * @brief For debug purpose: Print the value of all independent variables
     */
    void _printIndependentVariableValues();


private:
    QHash<QString, IndependentVariableM*> _mapIndependentVarByName;
    CassUuid _experimentationCassUuid;
    CassUuid _subjectUuid;
    CassUuid _protocolUuid;

    CassUuid _cassUuid;


};

QML_DECLARE_TYPE(SessionM)

#endif // SESSION_M_H
