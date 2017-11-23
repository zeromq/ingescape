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
TimeTickM::TimeTickM(int relativeDay, int hour, int minute, TimeTickTypes::Value type, QObject *parent):
    QObject(parent),
    _relativeDay(relativeDay),
    _hour(hour),
    _minute(minute),
    _type(type)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Set our time in seconds
    _timeInSeconds = relativeDay * 86400 + hour * 3600 + minute * 60;

    // Set our flags
    _isBigTick = (type == TimeTickTypes::BIG_TICK);
    _isSmallTick = (type == TimeTickTypes::SMALL_TICK);

    // Set our label
    _label = QString("%1:%2").arg(_hour, 2, 10, QChar('0')).arg(_minute, 2, 10, QChar('0'));
}
