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


/**
  * Types of the value for a characteristic
  */
I2_ENUM_CUSTOM(CharacteristicValueTypes, UNKNOWN, INTEGER, DOUBLE, TEXT, CHARACTERISTIC_ENUM)


/**
  * Types of the value for an independent variable
  */
I2_ENUM(IndependentVariableValueTypes, UNKNOWN, INTEGER, DOUBLE, TEXT, INDEPENDENT_VARIABLE_ENUM)


// Date date of our application
static const QDate APPLICATION_START_DATE = QDate::currentDate();


/*!
 * Define a QTime and a QDateTime: Manage a date in addition to the time
 * to prevent a delta in hours between JS (QML) and C++.
 * The QDateTime is readable/writable from QML.
 * The date used is the current date.
 */
#define I2_QML_PROPERTY_QTime(name) \
        Q_PROPERTY (QDateTime name READ qmlGet##name WRITE qmlSet##name NOTIFY name##Changed) \
    public: \
        QDateTime qmlGet##name () const { \
            return QDateTime(_##name##_Date, _##name); \
        } \
        QTime name () const { \
            return _##name; \
        } \
        virtual bool qmlSet##name (QDateTime value) { \
            bool hasChanged = false; \
            if (_##name != value.time()) { \
                _##name = value.time(); \
                _##name##_Date = value.date(); \
                hasChanged = true; \
                Q_EMIT name##Changed(value); \
            } \
            return hasChanged; \
        } \
        void set##name(QTime value) { \
            if (_##name != value) { \
                _##name = value; \
                Q_EMIT name##Changed(QDateTime(_##name##_Date, _##name)); \
            } \
        } \
    Q_SIGNALS: \
        void name##Changed (QDateTime value); \
    protected: \
        QTime _##name; \
        QDate _##name##_Date = APPLICATION_START_DATE;



/**
 * @brief The AssessmentsEnums class is a helper for enumerations and constants specific to the "Measuring" application
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
