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

    // Unique identifier in Cassandra Data Base converted to string
    I2_CPP_NOSIGNAL_PROPERTY(QString, uid)

    // Displayed identifier of our subject
    I2_QML_PROPERTY(QString, displayedId)

    // Values of the characteristics of our subject
    // "Qml Property Map" allows to set key-value pairs that can be used in QML bindings
    I2_QML_PROPERTY_READONLY(QQmlPropertyMap*, mapCharacteristicValues)

    // Hash table from a (unique) id of characteristic to the characteristic value
    //I2_QOBJECT_HASHMODEL(QVariant, hashFromCharacteristicIdToValue)


public:

    /**
     * @brief Constructor
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
    // Experimentation's unique identifier in Cassandra Data Base
    CassUuid _experimentationCassUuid;

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;

};

QML_DECLARE_TYPE(SubjectM)

#endif // SUBJECTM_H
