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


static const QString CHARACTERISTIC_UID = "UID";
static const QString CHARACTERISTIC_NAME = "Name";


/**
 * @brief The SubjectM class defines a model of subject
 */
class SubjectM : public QObject
{
    Q_OBJECT

    // Unique identifier of our subject
    I2_QML_PROPERTY(QString, uid)

    // Name of our subject
    I2_QML_PROPERTY(QString, name)

    // Flag indicating ...
    //I2_QML_PROPERTY(bool, isCurrentlyEditing)

    // Values of the characteristics of our subject
    // Hash table from a (unique) id of characteristic to the characteristic value
    //I2_QOBJECT_HASHMODEL(QVariant, mapFromCharacteristicIdToValue)

    //I2_QML_PROPERTY(QHash<QString, QVariant>, mapFromCharacteristicIdToValue)

    // FIXME for tests
    //I2_QML_PROPERTY(QList<QObject*>, temp)
    //I2_QML_PROPERTY(QHash<QString, QObject*>, temp)

    // Values of the characteristics of our subject
    //I2_QML_PROPERTY(QList<QVariant>, characteristicValues)


public:

    /**
     * @brief Constructor
     * @param uid
     * @param name
     * @param parent
     */
    explicit SubjectM(QString uid,
                      QString name = "",
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


    /**
     * @brief Get the value of a characteristic
     * @param characteristicName
     * @return
     */
    Q_INVOKABLE QString getValueOfCharacteristic(QString characteristicName);


    /**
     * @brief Set the value of a characteristic
     * @param characteristicValue
     * @param characteristic
     */
    Q_INVOKABLE void setValueOfCharacteristic(QString characteristicValue, CharacteristicM* characteristic);



Q_SIGNALS:


public Q_SLOTS:


private:

    // FIXME accessible for QML
    // Values of the characteristics of our subject
    // Hash table from a (unique) id of characteristic to the characteristic value
    QHash<QString, QVariant> _mapFromCharacteristicIdToValue;

};

QML_DECLARE_TYPE(SubjectM)

#endif // SUBJECTM_H
