
// Margin in minutes (left and right side of our view) to ensure that labels are not truncated
#define TIME_MARGIN_IN_PIXELS 40

// The viewport of our timeline view is approximately 1335 px
#define DEFAULT_VIEWPORT_WIDTH 1400.0

// Minimum and maximum values to constrain "zoom" levels
#define MINIMUM_TIME_RANGE 0.1
#define MAXIMUM_TIME_RANGE 60.0

// Default "time range" in minutes
#define DEFAULT_TIME_RANGE 2.0


// Total length of timeline in hours
#define LENGTH_TIME_LINE 2.0

// time ranges number
#define TIME_RANGES_COUNT 8
// time ranges for time ticks
int TIME_RANGES[TIME_RANGES_COUNT] = {1, 2, 5, 10, 30, 60, 120, 300};



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

    // Configure our filtered list of "time ticks"
    _filteredListTimeTicks.setSourceModel(&_timeTicks);


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

    // Compute time period in minutes
    _totalTimePeriodInMinutes = LENGTH_TIME_LINE * 60.0;

    // range per default
    _timeRangeBetweenTimeTicksInMilliSeconds = TIME_RANGES[4]*1000;

    for (int milliseconds = 0; milliseconds < (_totalTimePeriodInMinutes*60000.0); milliseconds += _timeRangeBetweenTimeTicksInMilliSeconds/3)
    {
        // Check if we have a normal tick
        bool isBigTick = ((milliseconds%_timeRangeBetweenTimeTicksInMilliSeconds) == 0);
        TimeTickTypes::Value type = (isBigTick ? TimeTickTypes::BIG_TICK : TimeTickTypes::NORMAL_TICK);

        // Create a new tick and save it
        TimeTickM* timeTick = new TimeTickM(milliseconds, type);
        timeTicksList.append(timeTick);
    }

    // - Add a last time tick (for decoration purposes only)
    TimeTickM* timeTick = new TimeTickM((_totalTimePeriodInMinutes*60000.0), TimeTickTypes::BIG_TICK);
    timeTicksList.append(timeTick);

    // Save our list of time ticks
    _timeTicks.append(timeTicksList);

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
 * @brief Update time coordinates of X axis according to zoom levels
 * @return
 */
void AbstractTimeActionslineScenarioViewController::updateTimeCoordinatesOfTimeTicks() {

    // change time coordinates if needed
    qreal viewportTimeRangeInSeconds =  DEFAULT_VIEWPORT_WIDTH/(_pixelsPerMinute/60.0);

    qreal minDeltaBetweenTicks = viewportTimeRangeInSeconds/12.0;
    qreal previousTimeRange = _timeRangeBetweenTimeTicksInMilliSeconds;

    for (int index = 0; index < TIME_RANGES_COUNT; index ++)
    {
        if (minDeltaBetweenTicks < TIME_RANGES[index])
        {
            _timeRangeBetweenTimeTicksInMilliSeconds = TIME_RANGES[index]*1000;
            break;
        }
    }

    // change time ticks list if the range has changed
    if (_timeRangeBetweenTimeTicksInMilliSeconds != previousTimeRange)
    {
        // clear the ticks list
        _timeTicks.deleteAllItems();

        // Build our list of ticks
        QList<TimeTickM*> timeTicksList;

        // the number of small ticks is different according to the time range between big ticks
        int rangeForSmallTicks;
        if ( (_timeRangeBetweenTimeTicksInMilliSeconds/1000) == 5 || (_timeRangeBetweenTimeTicksInMilliSeconds/1000) == 300) {
            rangeForSmallTicks = _timeRangeBetweenTimeTicksInMilliSeconds/5;
        }
        else if ((_timeRangeBetweenTimeTicksInMilliSeconds/1000) == 30)  {
            rangeForSmallTicks = _timeRangeBetweenTimeTicksInMilliSeconds/3;
        }
        else {
            rangeForSmallTicks = _timeRangeBetweenTimeTicksInMilliSeconds/2;
        }

        for (int milliseconds = 0; milliseconds < (_totalTimePeriodInMinutes*60000.0); milliseconds += rangeForSmallTicks)
        {
            // Check if we have a normal tick or a big one
            bool isBigTick = ((milliseconds%_timeRangeBetweenTimeTicksInMilliSeconds) == 0);
            TimeTickTypes::Value type = (isBigTick ? TimeTickTypes::BIG_TICK : TimeTickTypes::NORMAL_TICK);

            // Create a new tick and save it
            TimeTickM* timeTick = new TimeTickM(milliseconds, type);
            timeTicksList.append(timeTick);
        }

        // - Add a last time tick (for decoration purposes only)
        TimeTickM* timeTick = new TimeTickM((_totalTimePeriodInMinutes*60000.0), TimeTickTypes::BIG_TICK);
        timeTicksList.append(timeTick);

        // Save our new list of time ticks
        _timeTicks.append(timeTicksList);
    }


    // Updata our X axis
    _updateCoordinateSystemAbscissaAxis();
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

        // Update the time tick filtered time range
        _updateFilteredTimeTicksListTimeRange();

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
    int millisecondsTime = convertAbscissaInCoordinateSystemToTimeInMilliseconds(xValue, extraQmlUpdateField);

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
    return convertTimeInMillisecondsToAbscissaInCoordinateSystem(time.msecsSinceStartOfDay(), extraQmlUpdateField);
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

/**
  * @brief Custom setter on viewport X
  */
void AbstractTimeActionslineScenarioViewController::setviewportX(int value)
{
    if(_viewportX != value)
    {
        _viewportX = value;

        // Update time ticks list time range
        _updateFilteredTimeTicksListTimeRange();

        Q_EMIT viewportXChanged(value);
    }
}

/**
  * @brief Custom setter on viewport width
  */
void AbstractTimeActionslineScenarioViewController::setviewportWidth(int value)
{
    if(_viewportWidth != value)
    {
        _viewportWidth = value;

        // Update time ticks list time range
        _updateFilteredTimeTicksListTimeRange();

        Q_EMIT viewportWidthChanged(value);
    }
}

/**
  * @brief Update the filtered list of time ticks by computing the visible windows
  */
void AbstractTimeActionslineScenarioViewController::_updateFilteredTimeTicksListTimeRange()
{
    // Compute the viewport range in misslisecond with right and left margin of 20 pixels
    qreal viewportTimeRangeStartInMilliseconds =  (_viewportX-_timeMarginInPixels-20)/(_pixelsPerMinute/60000.0);
    qreal viewportTimeRangeEndInMilliseconds =  (_viewportX+_viewportWidth+20)/(_pixelsPerMinute/60000.0);

    // Set the new time range
    _filteredListTimeTicks.setTimeRange(viewportTimeRangeStartInMilliseconds,viewportTimeRangeEndInMilliseconds);

    // Emit the change for actions view models
    Q_EMIT timeRangeChanged(viewportTimeRangeStartInMilliseconds,viewportTimeRangeEndInMilliseconds);
}




