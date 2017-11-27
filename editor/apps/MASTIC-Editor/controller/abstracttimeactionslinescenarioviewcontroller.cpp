#include "abstracttimeactionslinescenarioviewcontroller.h"


// Margin in minutes (left and right side of our Analysis view) to ensure that labels are not truncated
#define TIME_MARGIN_IN_MINUTES 2.0


#include <QQmlEngine>
#include <QtGlobal>
#include <QDate>
#include <QtMath>

/**
 * @brief Default constructor
 * @param currentReferenceDate
 * @param parent
 */
AbstractTimeActionslineScenarioViewController::AbstractTimeActionslineScenarioViewController(QObject *parent) :
    QObject(parent),
    _pixelsPerMinute(20.0),
    _timeTicksTotalWidth(1.0),
    _viewportX(-1),
    _viewportY(-1),
    _viewportWidth(0),
    _viewportHeight(0),
    _viewportContentScaleX(1.0),
    _timeMarginInMinutes(TIME_MARGIN_IN_MINUTES),
    _totalTimePeriodInMinutes(0.0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


    //----------------------
    //
    // X-axis: timeline
    //
    //----------------------
    //
    // Time ticks
    //

    // Build our list of ticks
    QList<TimeTickM*> timeTicksList;

    // NB: We start at 00:00:00
    _startTime = QTime(0, 0, 0);
    _startRelativeTimeInSeconds = 0;

    _endTime = QTime(24, 0, 0);
    _endRelativeTimeInSeconds = 3600 * 24;

    // Today - from 00:00 to 23:55
    for (int hours = 0; hours < 24; hours++)
    {
        for (int minutes = 0; minutes < 60; minutes += 1)
        {
            // Check if we have a big tick
            bool isBigTick = ((minutes%5) == 0);
            TimeTickTypes::Value type = (isBigTick ? TimeTickTypes::BIG_TICK : (TimeTickTypes::NORMAL_TICK));

            // Create a new tick and save it
            TimeTickM* timeTick = new TimeTickM(0, hours, minutes, type);
            timeTicksList.append(timeTick);
        }
    }

    // - Add a last time tick (for decoration purposes only)
    // (TIME_RANGE_LAST_HOUR_OF_DAY_PLUS_ONE):00
    TimeTickM* timeTick = new TimeTickM(1, 24, 0, TimeTickTypes::BIG_TICK);
    timeTicksList.append(timeTick);

    // Save our list of time ticks
    _timeTicks.append(timeTicksList);

    // Compute time period in minutes
    // NB: we can not rely on _endDateTime and _startDateTime because _currentReferenceDate can be invalid
    _totalTimePeriodInMinutes = 24 * 60.0;


    // Updata our X axis
    _updateCoordinateSystemAbscissaAxis();
}


/**
  * @brief Destructor
  */
AbstractTimeActionslineScenarioViewController::~AbstractTimeActionslineScenarioViewController()
{
    // Clean-up our list of time ticks
    _timeTicks.deleteAllItems();
}


/**
 * @brief Set the size in pixels of a minute
 * @param value
 */
void AbstractTimeActionslineScenarioViewController::setpixelsPerMinute(qreal value)
{
    if (!qFuzzyCompare(_pixelsPerMinute, value))
    {
        // Save value
        _pixelsPerMinute = value;

        // Updata our X axis
        _updateCoordinateSystemAbscissaAxis();

        // Notify change
        Q_EMIT pixelsPerMinuteChanged(value);
    }
}



/**
 * @brief Set the scale factor applied to the content of our viewport (X-axis)
 * @param value
 */
void AbstractTimeActionslineScenarioViewController::setviewportContentScaleX(qreal value)
{
    if (!qFuzzyCompare(_viewportContentScaleX, value))
    {
        // Save new value
        _viewportContentScaleX = value;

        // Notify change
        Q_EMIT viewportContentScaleXChanged(value);
    }
}


/**
 * @brief Set the width of all time ticks
 * @param value
 */
void AbstractTimeActionslineScenarioViewController::settimeTicksTotalWidth(qreal value)
{
    // Guard to avoid QML issues
    value = qMax(1.0, value);

    if (!qFuzzyCompare(_timeTicksTotalWidth, value))
    {
        // Save our new value
        _timeTicksTotalWidth = value;

        // Notify change
        Q_EMIT timeTicksTotalWidthChanged(value);
    }
}


/**
 * @brief Convert a given time value into a X value (abscissa) of our coordinate system
 *
 * @param timeInMilliSeconds Number of seconds since 00:00:00 of our current date
 * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
 *
 * @return
 */
qreal AbstractTimeActionslineScenarioViewController::convertTimeInMillisecondsToAbscissaInCoordinateSystem(int timeInMilliSeconds, qreal extraQmlUpdateField)
{
    Q_UNUSED(extraQmlUpdateField)

    // Compute delta in seconds between this date and our origin
    int deltaSeconds = (timeInMilliSeconds/1000 - _startRelativeTimeInSeconds);

    // Round value to avoid rendering artefacts
    return qRound(_pixelsPerMinute * (_timeMarginInMinutes + deltaSeconds/60.0));
}

/**
 * @brief Convert a given QTime object into a X value (abscissa) of our coordinate system
 *
 * @param QTime of our current time
 * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
 *
 * @return
 */
qreal AbstractTimeActionslineScenarioViewController::convertQTimeToAbscissaInCoordinateSystem(QTime time, qreal extraQmlUpdateField)
{
    return convertTimeInMillisecondsToAbscissaInCoordinateSystem(time.msecsSinceStartOfDay(),extraQmlUpdateField);
}


/**
 * @brief Convert a given duration in seconds into a length value in our coordinate system
 * @param durationInSeconds
 * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
 * @return
 */
qreal AbstractTimeActionslineScenarioViewController::convertDurationInSecondsToLengthInCoordinateSystem(int durationInSeconds, qreal extraQmlUpdateField)
{
    Q_UNUSED(extraQmlUpdateField)

    // Round value to avoid rendering artefacts
    return qRound( static_cast<qreal>(durationInSeconds) * _pixelsPerMinute/60.0);
}



/**
  * @brief Called when the abscissa axis of our coordinate system needs to be updated
  */
void AbstractTimeActionslineScenarioViewController::_updateCoordinateSystemAbscissaAxis()
{
    // Compute the new total width of our coordinate system
    qreal newTimeTicksTotalWidth = _pixelsPerMinute * (_totalTimePeriodInMinutes + 2 * _timeMarginInMinutes);
    settimeTicksTotalWidth(newTimeTicksTotalWidth);

    Q_EMIT coordinateSystemAbscissaAxisChanged();
    Q_EMIT coordinateSystemAbscissaAndOrdinateAxesChanged();
}



