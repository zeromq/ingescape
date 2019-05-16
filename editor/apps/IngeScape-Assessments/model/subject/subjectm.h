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


static const QString CHARACTERISTIC_SUBJECT_NAME = "Name";


/**
 * @brief The SubjectM class defines a model of subject
 */
class SubjectM : public QObject
{
    Q_OBJECT

    // Unique identifier of our subject
    I2_QML_PROPERTY_READONLY(QString, uid)

    // Name of our subject
    I2_QML_PROPERTY(QString, name)

    // Values of the characteristics of our subject
    // "Qml Property Map" allows to set key-value pairs that can be used in QML bindings
    I2_QML_PROPERTY_READONLY(QQmlPropertyMap*, mapCharacteristicValues)

    // Hash table from a (unique) id of characteristic to the characteristic value
    //I2_QOBJECT_HASHMODEL(QVariant, hashFromCharacteristicIdToValue)


public:

    /**
     * @brief Constructor
     * @param uid
     * @param name
     * @param parent
     */
    explicit SubjectM(QString uid,
                      //QString name = "",
                      QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~SubjectM();


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


};

QML_DECLARE_TYPE(SubjectM)

#endif // SUBJECTM_H
