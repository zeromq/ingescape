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

#ifndef SUBJECTM_H
#define SUBJECTM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/subject/characteristicm.h>


/**
 * @brief The SubjectM class defines a model of subject
 */
class SubjectM : public QObject
{
    Q_OBJECT

    // Displayed identifier of our subject
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, displayedId)

    // Values of the characteristics of our subject
    // "Qml Property Map" allows to set key-value pairs that can be used in QML bindings
    I2_QML_PROPERTY_READONLY(QQmlPropertyMap*, mapCharacteristicValues)

    // Temporary map holding edited values of the characteristics of our subject
    // User modifications are stored here before applying them to the actual value map
    // in case a rollback is requested
    I2_QML_PROPERTY_READONLY(QQmlPropertyMap*, tempMapCharacteristicValues)

    // Hash table from a (unique) id of characteristic to the characteristic value
    //I2_QOBJECT_HASHMODEL(QVariant, hashFromCharacteristicIdToValue)


public:

    /**
     * @brief Constructor
     * @param experimentationUuid
     * @param cassUuid
     * @param displayedId
     * @param parent
     */
    explicit SubjectM(CassUuid experimentationUuid,
                      CassUuid cassUuid,
                      QString displayedId,
                      QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~SubjectM();


    /**
     * @brief Subject table name
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
     * @brief Add the characteristic to our experimentation
     * @param characteristic
     */
    void addCharacteristic(CharacteristicM* characteristic);


    void setCharacteristicValue(CharacteristicM* characteristic, const QVariant& value);


    /**
     * @brief Remove the characteristic from our experimentation
     * @param characteristic
     */
    void removeCharacteristic(CharacteristicM* characteristic);


    /**
     * @brief Static factory method to create a subject from a CassandraDB record
     * @param row
     * @return
     */
    static SubjectM* createSubjectFromCassandraRow(const CassRow* row);


    /**
     * @brief Delete the given subject from Cassandra DB
     * @param subject
     */
    static void deleteSubjectFromCassandra(const SubjectM& subject);

    /**
     * @brief Restore the temporary map values wih the ones from the actual value map
     */
    Q_INVOKABLE void resetTemporaryPropertyValues();

    /**
     * @brief Apply the values from the temporary to the actual value map
     * The DB will be update on the fly
     */
    Q_INVOKABLE void applyTemporaryPropertyValues();


private Q_SLOTS:

    /**
     * @brief Slot called when the value of a characteristic changed
     * @param key
     * @param value
     */
    void _onCharacteristicValueChanged(const QString &key, const QVariant &value);

private: // Methods

    /**
     * @brief Delete every characteristic value associated with the given subject
     * @param subject
     */
    static void _deleteCharacteristicValuesForSubject(const SubjectM& subject);


private:
    QHash<QString, CharacteristicM*> _mapCharacteristicsByName;

    // Experimentation's unique identifier in Cassandra Data Base
    CassUuid _experimentationCassUuid;

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;

};

QML_DECLARE_TYPE(SubjectM)

#endif // SUBJECTM_H
