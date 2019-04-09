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
    //I2_QML_PROPERTY(QString, uid)

    // Name of our characteristic
    I2_QML_PROPERTY(QString, name)

    // Description of our characteristic
    //I2_QML_PROPERTY(QString, description)

    // Type of our characteristic value
    I2_QML_PROPERTY_READONLY(CharacteristicValueTypes::Value, valueType)

    // Details if valueType == CHARACTERISTIC_ENUM ?
    // Link with CharacteristicEnumM ?

    // List of possible values if the value type is "CHARACTERISTIC_ENUM"
    I2_QML_PROPERTY(QStringList, enumValues)


public:

    /**
     * @brief Constructor
     * @param name
     * @param valueType
     * @param parent
     */
    explicit CharacteristicM(QString name,
                             CharacteristicValueTypes::Value valueType,
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
