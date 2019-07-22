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
    explicit SubjectM(CassUuid cassUuid,
                      CassUuid experimentationUuid,
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
    CassUuid getCassUuid() { return _cassUuid; }


    /**
     * @brief Get the experimentation's unique identifier in Cassandra Data Base
     * @return
     */
    CassUuid getExperimentationCassUuid() { return _experimentationCassUuid; }


    /**
     * @brief Add the characteristic to our experimentation
     * @param characteristic
     */
    void addCharacteristic(CharacteristicM* characteristic);


    /**
     * @brief Remove the characteristic from our experimentation
     * @param characteristic
     */
    void removeCharacteristic(CharacteristicM* characteristic);


Q_SIGNALS:


public Q_SLOTS:


private Q_SLOTS:

    /**
     * @brief Slot called when the value of a characteristic changed
     * @param key
     * @param value
     */
    void _onCharacteristicValueChanged(const QString &key, const QVariant &value);


private:

    // Experimentation's unique identifier in Cassandra Data Base
    CassUuid _experimentationCassUuid;

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;

};

QML_DECLARE_TYPE(SubjectM)

#endif // SUBJECTM_H
