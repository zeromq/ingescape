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

#ifndef ASSESSMENTSENUMS_H
#define ASSESSMENTSENUMS_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/enums.h>


/**
  * Types of the value for a characteristic
  */
I2_ENUM_CUSTOM(CharacteristicValueTypes, UNKNOWN, INTEGER, DOUBLE, TEXT, CHARACTERISTIC_ENUM)


/**
  * Types of the value for an independent variable
  */
I2_ENUM_CUSTOM(IndependentVariableValueTypes, UNKNOWN, INTEGER, DOUBLE, TEXT, INDEPENDENT_VARIABLE_ENUM)


/**
 * @brief The AssessmentsEnums class is a helper for enumerations and constants specific to the "Assessments" application
 */
class AssessmentsEnums : public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit AssessmentsEnums(QObject *parent = nullptr);


};

QML_DECLARE_TYPE(AssessmentsEnums)

#endif // ASSESSMENTSENUMS_H
