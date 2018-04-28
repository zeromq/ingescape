#include "timetickm.h"

#include <QQmlEngine>



/**
 * @brief Default constructor
 *
 * @param relativeDay
 * @param milliseconds
 * @param type
 * @param parent
 */
TimeTickM::TimeTickM(int milliseconds, TimeTickTypes::Value type, QObject *parent):
    QObject(parent),
    _type(type)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Set our time in seconds
    _timeInMilliSeconds = milliseconds;

    // set hour minute and second
    QTime time = QTime::fromMSecsSinceStartOfDay(_timeInMilliSeconds);

    _hour = time.hour();
    _minute = time.minute();
    _second = time.second();

    // Set our flags
    _isBigTick = (type == TimeTickTypes::BIG_TICK);
    _isSmallTick = (type == TimeTickTypes::SMALL_TICK);

    // Set our label
    _label = QString("%1:%2:%3").arg(_hour, 2, 10, QChar('0')).arg(_minute, 2, 10, QChar('0')).arg(_second, 2, 10, QChar('0'));
}
