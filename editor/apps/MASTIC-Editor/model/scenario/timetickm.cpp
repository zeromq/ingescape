#include "timetickm.h"

#include <QQmlEngine>



/**
 * @brief Default constructor
 *
 * @param relativeDay
 * @param hour
 * @param minute
 * @param type
 * @param parent
 */
TimeTickM::TimeTickM(int hour, int minute, int second, TimeTickTypes::Value type, QObject *parent):
    QObject(parent),
    _hour(hour),
    _minute(minute),
    _second(second),
    _type(type)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Set our time in seconds
    _timeInSeconds = hour * 3600 + minute * 60 + second;

    // Set our flags
    _isBigTick = (type == TimeTickTypes::BIG_TICK);
    _isSmallTick = (type == TimeTickTypes::SMALL_TICK);

    // Set our label
    _label = QString("%1:%2:%3").arg(_hour, 2, 10, QChar('0')).arg(_minute, 2, 10, QChar('0')).arg(_second, 2, 10, QChar('0'));
}
