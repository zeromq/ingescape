/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef TIMETICKM_H
#define TIMETICKM_H

#include <QObject>
#include <QtQml>
#include <QTime>
#include <QDateTime>

#include "I2PropertyHelpers.h"



/**
  * @ brief Types of time ticks
  */
I2_ENUM(TimeTickTypes, NORMAL_TICK, BIG_TICK, SMALL_TICK)



/**
 * @brief The TimeTickM class represents a given time tick on our timeline
 */
class TimeTickM : public QObject
{
    Q_OBJECT


    // The hour part (0 to 23) of the time associated to our tick
    I2_QML_PROPERTY_READONLY(int, hour)

    // The minute part (0 to 59) of the time associated to our tick
    I2_QML_PROPERTY_READONLY(int, minute)

    // The second part (0 to 59) of the time associated to our tick
    I2_QML_PROPERTY_READONLY(int, second)

    // The millisecond part (0 to 99) of the time associated to our tick
    I2_QML_PROPERTY_READONLY(int, millisecond)

    // Type of ticks
    I2_QML_PROPERTY_READONLY(TimeTickTypes::Value, type)

    // Flag indicating if our tick is a big tick (type == BIG_TICK)
    // NB: to avoid useless computations in QML
    I2_QML_PROPERTY_READONLY(bool, isBigTick)

    // Flag indicating if our tick is a small tick (type == SMALL_TICK)
    // NB: to avoid useless computations in QML
    I2_QML_PROPERTY_READONLY(bool, isSmallTick)

    // Relative time in seconds associated to our tick (0: today at 00:00:00)
    I2_QML_PROPERTY_READONLY(int, timeInMilliSeconds)

    // Textual representation (label) of our time value
    // NB: to avoid useless computations in QML
    I2_QML_PROPERTY_READONLY(QString, label)

public:
    /**
     * @brief Default constructor
     *
     * @param relativeDay
     * @param milliseconds
     * @param type
     * @param parent
     */
    explicit TimeTickM(int milliseconds, TimeTickTypes::Value type, QObject *parent = 0);
};

QML_DECLARE_TYPE(TimeTickM)

#endif // TIMETICKM_H
