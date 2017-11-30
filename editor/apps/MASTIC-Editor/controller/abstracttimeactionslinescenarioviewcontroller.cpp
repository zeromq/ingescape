
// Margin in minutes (left and right side of our view) to ensure that labels are not truncated
#define TIME_MARGIN_IN_PIXELS 40

// The viewport of our analysis view is approximately 1335 px
#define DEFAULT_VIEWPORT_WIDTH 1400.0

// Minimum and maximum values to constrain "zoom" levels
#define MINIMUM_TIME_RANGE 1.0
#define MAXIMUM_TIME_RANGE 30.0

// Default "time range" in minutes
#define DEFAULT_TIME_RANGE 2.0


// Total length of timeline in hours
#define LENGTH_TIME_LINE 2.0

#include "abstracttimeactionslinescenarioviewcontroller.h"

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
    _pixelsPerMinute(DEFAULT_VIEWPORT_WIDTH/DEFAULT_TIME_RANGE),
    _timeTicksTotalWidth(1.0),
    _viewportX(-1),
    _viewportY(-1),
    _viewportWidth(0),
    _viewportHeight(0),
    _viewportContentScaleX(1.0),
    _timeMarginInPixels(TIME_MARGIN_IN_PIXELS),
    _minPixelsPerMinute(DEFAULT_VIEWPORT_WIDTH/MAXIMUM_TIME_RANGE),
    _maxPixelsPerMinute(DEFAULT_VIEWPORT_WIDTH/MINIMUM_TIME_RANGE),
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

    _endTime = QTime(LENGTH_TIME_LINE, 0, 0);
    _endRelativeTimeInSeconds = 3600 * LENGTH_TIME_LINE;

    // Today - from 00:00 to 23:55
    for (int hours = 0; hours < LENGTH_TIME_LINE; hours++)
    {
        for (int minutes = 0; minutes < 60; minutes += 1)
        {
            for (int seconds = 0; seconds < 60; seconds += 5)
            {
                // Check if we have a big tick
                bool isBigTick = (seconds == 0 || seconds == 30);
                TimeTickTypes::Value type = (isBigTick ? TimeTickTypes::BIG_TICK : (TimeTickTypes::NORMAL_TICK));

                // Create a new tick and save it
                TimeTickM* timeTick = new TimeTickM(hours, minutes, seconds, type);
                timeTicksList.append(timeTick);
            }
        }
    }

    // - Add a last time tick (for decoration purposes only)
    TimeTickM* timeTick = new TimeTickM(LENGTH_TIME_LINE, 0, 0, TimeTickTypes::BIG_TICK);
    timeTicksList.append(timeTick);

    // Save our list of time ticks
    _timeTicks.append(timeTicksList);

    // Compute time period in minutes
    // NB: we can not rely on _endDateTime and _startDateTime because _currentReferenceDate can be invalid
    _totalTimePeriodInMinutes = LENGTH_TIME_LINE * 60.0;


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
    qreal deltaSeconds = (timeInMilliSeconds/1000.0 - _startRelativeTimeInSeconds);

    // Round value to avoid rendering artefacts
    return (qRound(_pixelsPerMinute * (deltaSeconds/60.0)) + _timeMarginInPixels);
}


/**
 * @brief Convert a given X value (abscissa) of our coordinate system into a time
 *
 * @param X value
 * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
 *
 * @return
 */
int AbstractTimeActionslineScenarioViewController::convertAbscissaInCoordinateSystemToTimeInMilliseconds(qreal xValue, qreal extraQmlUpdateField)
{
    Q_UNUSED(extraQmlUpdateField)

    qreal timeSeconds = ((xValue- _timeMarginInPixels)/_pixelsPerMinute) * 60.0;

    return ((timeSeconds + _startRelativeTimeInSeconds) * 1000.0);
}

/**
 * @brief Convert a given X value (abscissa) of our coordinate system into a QTime object
 *
 * @param X value
 * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
 *
 * @return
 */
QTime AbstractTimeActionslineScenarioViewController::convertAbscissaInCoordinateSystemToQTime(qreal xValue, qreal extraQmlUpdateField)
{
    int millisecondsTime = convertAbscissaInCoordinateSystemToTimeInMilliseconds(xValue,extraQmlUpdateField);

    return QTime::fromMSecsSinceStartOfDay(millisecondsTime);
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
 * @brief Convert a given duration in milliseconds into a length value in our coordinate system
 * @param durationInMilliseconds
 * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
 * @return
 */
qreal AbstractTimeActionslineScenarioViewController::convertDurationInMillisecondsToLengthInCoordinateSystem(int durationInMilliseconds, qreal extraQmlUpdateField)
{
    Q_UNUSED(extraQmlUpdateField)

    // Round value to avoid rendering artefacts
    return qRound( static_cast<qreal>(durationInMilliseconds) * _pixelsPerMinute/60000.0);
}



/**
  * @brief Called when the abscissa axis of our coordinate system needs to be updated
  */
void AbstractTimeActionslineScenarioViewController::_updateCoordinateSystemAbscissaAxis()
{
    // Compute the new total width of our coordinate system
    qreal newTimeTicksTotalWidth = _pixelsPerMinute * (_totalTimePeriodInMinutes) + 2 * _timeMarginInPixels;
    settimeTicksTotalWidth(newTimeTicksTotalWidth);

    Q_EMIT coordinateSystemAbscissaAxisChanged();
    Q_EMIT coordinateSystemAbscissaAndOrdinateAxesChanged();
}



