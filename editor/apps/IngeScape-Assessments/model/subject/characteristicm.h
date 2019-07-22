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


extern const QString CHARACTERISTIC_SUBJECT_ID;


/**
 * @brief The CharacteristicM class defines a model of characteristic for a subject
 */
class CharacteristicM : public QObject
{
    Q_OBJECT

    // Unique identifier in Cassandra Data Base converted to string
    I2_CPP_NOSIGNAL_PROPERTY(QString, uid)

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
                             const QString& name,
                             CharacteristicValueTypes::Value valueType,
                             const QStringList& enumValues = {},
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~CharacteristicM();


    /**
     * @brief Get the unique identifier in Cassandra Data Base
     * @return
     */
    CassUuid getCassUuid();


Q_SIGNALS:


public Q_SLOTS:


private:

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;

};

QML_DECLARE_TYPE(CharacteristicM)

#endif // CHARACTERISTICM_H
