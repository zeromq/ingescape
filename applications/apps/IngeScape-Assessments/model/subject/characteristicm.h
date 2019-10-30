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

#ifndef CHARACTERISTICM_H
#define CHARACTERISTICM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/assessmentsenums.h>

#include "cassandra.h"

// Forward declaration
class ExperimentationM;


extern const QString CHARACTERISTIC_SUBJECT_ID;


/**
 * @brief The CharacteristicM class defines a model of characteristic for a subject
 */
class CharacteristicM : public QObject
{
    Q_OBJECT

    // Name of our characteristic
    I2_QML_PROPERTY_READONLY(QString, name)

    // Description of our characteristic
    //I2_QML_PROPERTY_READONLY(QString, description)

    // Type of our characteristic value
    I2_QML_PROPERTY_READONLY(CharacteristicValueTypes::Value, valueType)

    // Flag indicating if our characteristic is the subject id
    I2_QML_PROPERTY_READONLY(bool, isSubjectId)

    // List of possible values if the value type is "CHARACTERISTIC_ENUM"
    I2_QML_PROPERTY_READONLY(QStringList, enumValues)


public:

    /**
     * @brief Constructor
     * @param cassUuid
     * @param name
     * @param valueType
     * @param parent
     */
    explicit CharacteristicM(CassUuid cassUuid,
                             CassUuid experimentationUuid,
                             const QString& name,
                             CharacteristicValueTypes::Value valueType,
                             const QStringList& enumValues = {},
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~CharacteristicM();


    /**
     * @brief Characteristic table name
     */
    static const QString table;

    /**
     * @brief Characteristic table column names
     */
    static const QStringList columnNames;

    /**
     * @brief Characteristic table primary keys IN ORDER
     */
    static const QStringList primaryKeys;


    /**
     * @brief Get the unique identifier in Cassandra Data Base
     * @return
     */
    CassUuid getCassUuid() const { return _cassUuid; }


    /**
     * @brief Get the experimentation's unique identifier in Cassandra Data Base
     * @return
     */
    CassUuid getExperimentationCassUuid() const { return _experimentationCassUuid; }


    /**
     * @brief Static factory method to create a characteristic from a CassandraDB record
     * @param row
     * @return
     */
    static CharacteristicM* createFromCassandraRow(const CassRow* row);


    /**
     * @brief Create a CassStatement to insert an CharacteristicM into the DB.
     * The statement contains the values from the given characteristic.
     * Passed characteristic must have a valid and unique UUID.
     * @param characteristic
     * @return
     */
    static CassStatement* createBoundInsertStatement(const CharacteristicM& characteristic);

private:
    // Experimentation's unique identifier in Cassandra Data Base
    CassUuid _experimentationCassUuid;

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;

};

QML_DECLARE_TYPE(CharacteristicM)

#endif // CHARACTERISTICM_H
