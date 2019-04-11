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


/**
 * @brief The CharacteristicM class defines a model of characteristic
 */
class CharacteristicM : public QObject
{
    Q_OBJECT

    // Unique identifier of our characteristic
    //I2_QML_PROPERTY_READONLY(QString, uid)

    // Name of our characteristic
    I2_QML_PROPERTY_READONLY(QString, name)

    // Description of our characteristic
    //I2_QML_PROPERTY_READONLY(QString, description)

    // Type of our characteristic value
    I2_QML_PROPERTY_READONLY(CharacteristicValueTypes::Value, valueType)

    // Flag indicating if our characteristic can be deleted
    I2_QML_PROPERTY_READONLY(bool, canBeDeleted)

    // Details if valueType == CHARACTERISTIC_ENUM ?
    // Link with CharacteristicEnumM ?

    // List of possible values if the value type is "CHARACTERISTIC_ENUM"
    I2_QML_PROPERTY_READONLY(QStringList, enumValues)


public:

    /**
     * @brief Constructor
     * @param name
     * @param valueType
     * @param canBeDeleted
     * @param parent
     */
    explicit CharacteristicM(QString name,
                             CharacteristicValueTypes::Value valueType,
                             bool canBeDeleted = true,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~CharacteristicM();


Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(CharacteristicM)

#endif // CHARACTERISTICM_H
