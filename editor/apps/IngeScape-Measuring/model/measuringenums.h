/*
 *	IngeScape Measuring
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

#ifndef MEASURINGENUMS_H
#define MEASURINGENUMS_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
  * Types of the value for a characteristic
  */
I2_ENUM(CharacteristicValueTypes, UNKNOWN, TEXT, INTEGER, DOUBLE, CHARACTERISTIC_ENUM)


/**
  * Types of the value for an independent variable
  */
I2_ENUM(IndependentVariableValueTypes, UNKNOWN, TEXT, INTEGER, DOUBLE, INDEPENDENT_VARIABLE_ENUM)


/**
 * @brief The MeasuringEnums class is a helper for enumerations and constants specific to the "Measuring" application
 */
class MeasuringEnums : public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit MeasuringEnums(QObject *parent = nullptr);


};

QML_DECLARE_TYPE(MeasuringEnums)

#endif // MEASURINGENUMS_H
