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
 * @brief The TimeTickM class represents a given time tick on our analysis view
 */
class TimeTickM : public QObject
{
    Q_OBJECT

    // The relative day (-1: yesterday, 0: today, 1: tomorrow, etc.) associated to our tick
    I2_QML_PROPERTY_READONLY(int, relativeDay)

    // The hour part (0 to 23) of the time associated to our tick
    I2_QML_PROPERTY_READONLY(int, hour)

    // The minute part (0 to 59) of the time associated to our tick
    I2_QML_PROPERTY_READONLY(int, minute)

    // Type of ticks
    I2_QML_PROPERTY_READONLY(TimeTickTypes::Value, type)

    // Flag indicating if our tick is a big tick (type == BIG_TICK)
    // NB: to avoid useless computations in QML
    I2_QML_PROPERTY_READONLY(bool, isBigTick)

    // Flag indicating if our tick is a small tick (type == SMALL_TICK)
    // NB: to avoid useless computations in QML
    I2_QML_PROPERTY_READONLY(bool, isSmallTick)

    // Relative time in seconds associated to our tick (0: today at 00:00:00)
    I2_QML_PROPERTY_READONLY(int, timeInSeconds)

    // Textual representation (label) of our time value
    // NB: to avoid useless computations in QML
    I2_QML_PROPERTY_READONLY(QString, label)

public:
    /**
     * @brief Default constructor
     *
     * @param relativeDay
     * @param hour
     * @param minute
     * @param type
     * @param parent
     */
    explicit TimeTickM(int relativeDay, int hour, int minute, TimeTickTypes::Value type, QObject *parent = 0);
};

QML_DECLARE_TYPE(TimeTickM)

#endif // TIMETICKM_H
