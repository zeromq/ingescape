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


// Special namespace to match the behavior of model classes without having a proper model class for this particular table.
namespace CharacteristicValueM {

const QString table = "ingescape.characteristic_value";

}


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
    static CharacteristicM* createCharacteristicFromCassandraRow(const CassRow* row);

    /**
     * @brief Delete a characteristic from Cassandra DB
     * @param characteristic The characteristic to delete
     * @param experimentation The experimentation it's associated with for back reference to subjects
     */
    static void deleteCharacteristicFromCassandra(const CharacteristicM& characteristic, ExperimentationM* experimentation);

private:

    /**
     * @brief Delete every characteric value associated with the given characteristic
     * FIXME Sending a request for each subject does not seem very efficient...
     *       It would be nice if we could just have a WHERE clause on id_experimentation and id_characteristic, wouldn't it?
     * NOTE Having a model manager handling the model instances would allow a characteristic to get its experimentation from
     *      the CassUuid (which it already knows) without having to pass a pointer to said experimentation.
     *      Another solution would be to create ViewModels that holds pointers to the model instance and models that only handle UUIDs.
     *      This way, models are a match for what's ion the Cassandra DB and view models have access the linked instances for display purposes
     *      (cf. ItemVM in ENEDIS)
     * @param characteristic The characteristic to delete
     * @param experimentation The experimentation it's associated with for back reference to subjects
     */
    static void _deleteCharacteristicValuesForCharacteristic(const CharacteristicM& characteristic, ExperimentationM* experimentation);

private:
    // Experimentation's unique identifier in Cassandra Data Base
    CassUuid _experimentationCassUuid;

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;

};

QML_DECLARE_TYPE(CharacteristicM)

#endif // CHARACTERISTICM_H
