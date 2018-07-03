#ifndef ABSTRACTTIMEACTIONSLINESCENARIOVIEWCONTROLLER_H
#define ABSTRACTTIMEACTIONSLINESCENARIOVIEWCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <QTime>
#include <QDateTime>

#include "I2PropertyHelpers.h"

#include <model/scenario/timetickm.h>
#include <sortFilter/abstracttimerangefilter.h>


/**
 * @brief The AbstractTimeActionslineScenarioViewController class define the base class of
 *        controllers used to display a time line based on (time, actions line) data
 */
class AbstractTimeActionslineScenarioViewController : public QObject
{
    Q_OBJECT

    //--------------------------------------
    //
    // DATA
    //
    //--------------------------------------
    // List of time ticks
    I2_QOBJECT_LISTMODEL(TimeTickM, timeTicks)

    // List of "time ticks" filtered with a given time range
    Q_PROPERTY(AbstractTimeRangeFilter* filteredListTimeTicks READ filteredListTimeTicks CONSTANT)


    //--------------------------------------
    //
    // Coordinate system
    //
    //--------------------------------------

    // Size in pixels of a minute
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, pixelsPerMinute)

    // Width of all time ticks
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(qreal, timeTicksTotalWidth)

    // Viewport start abscissa in pixels (left-side of our viewport)
    I2_QML_PROPERTY_CUSTOM_SETTER(int, viewportX)

    // Viewpot start ordinate in pixels (left-side of our viewport)
    I2_QML_PROPERTY(int, viewportY)

    // Viewport width in pixels
    I2_QML_PROPERTY_CUSTOM_SETTER(int, viewportWidth)

    // Viewport height in pixels
    I2_QML_PROPERTY(int, viewportHeight)

    // Scale factor applied to the content of our viewport (X-axis)
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, viewportContentScaleX)

    // Time margin in pixels
    I2_QML_PROPERTY(int, timeMarginInPixels)

    // Minimum size in pixels of a minute
    I2_QML_PROPERTY_READONLY_FUZZY_COMPARE(qreal, minPixelsPerMinute)

    // Maximum size in pixels of a minute
    I2_QML_PROPERTY_READONLY_FUZZY_COMPARE(qreal, maxPixelsPerMinute)


public:
        /**
         * @brief Default constructor
         * @param currentReferenceDate
         * @param parent
         */
        explicit AbstractTimeActionslineScenarioViewController(QObject *parent = 0);


        /**
          * @brief Destructor
          */
        ~AbstractTimeActionslineScenarioViewController();

        /**
        * @brief Get our filtered list of "time ticks"
        * @return
        */
        AbstractTimeRangeFilter* filteredListTimeTicks()
        {
            return &_filteredListTimeTicks;
        }


    public Q_SLOTS:

        /**
         * @brief Update time coordinates of X axis according to zoom levels
         * @return
         */
        void updateTimeCoordinatesOfTimeTicks();

        /**
         * @brief Convert a given time value into a X value (abscissa) of our coordinate system
         *
         * @param timeInMilliSeconds Number of seconds since 00:00:00 of our current date
         * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
         *
         * @return
         */
        qreal convertTimeInMillisecondsToAbscissaInCoordinateSystem(int timeInMilliSeconds, qreal extraQmlUpdateField);


        /**
         * @brief Convert a given X value (abscissa) of our coordinate system into a time
         *
         * @param X value
         * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
         *
         * @return
         */
        int convertAbscissaInCoordinateSystemToTimeInMilliseconds(qreal xValue, qreal extraQmlUpdateField);

        /**
         * @brief Convert a given X value (abscissa) of our coordinate system into a QTime object
         *
         * @param X value
         * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
         *
         * @return
         */
        QTime convertAbscissaInCoordinateSystemToQTime(qreal xValue, qreal extraQmlUpdateField);


        /**
         * @brief Convert a given QTime object into a X value (abscissa) of our coordinate system
         *
         * @param QTime of our current time
         * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
         *
         * @return
         */
        qreal convertQTimeToAbscissaInCoordinateSystem(QTime time, qreal extraQmlUpdateField);

        /**
         * @brief Convert a given duration in milliseconds into a length value in our coordinate system
         * @param durationInMilliseconds
         * @param extraQmlUpdateField Extra QML field used to recall this function when needed (binding)
         * @return
         */
        qreal convertDurationInMillisecondsToLengthInCoordinateSystem(int durationInMilliseconds, qreal extraQmlUpdateField);

    Q_SIGNALS:
       /**
        * @brief Will be triggered when the abscissa axis of our coordinate system changes
        * (time scale update)
        */
       void coordinateSystemAbscissaAxisChanged();

       /**
        * @brief Will be triggered when both the abscissa axis and the ordinate axis of our coordinate system change
        */
       void coordinateSystemAbscissaAndOrdinateAxesChanged();

       /**
        * @brief Emitted when the visible time range of our timeline has changed
        */
       void timeRangeChanged(int viewportTimeRangeStartInMilliseconds, int viewportTimeRangeEndInMilliseconds);


    protected:
        /**
          * @brief Called when the abscissa axis of our coordinate system needs to be updated
          */
       void _updateCoordinateSystemAbscissaAxis();

       /**
         * @brief Update the filtered list of time ticks by computing the visible windows
         */
       void _updateFilteredTimeTicksListTimeRange();


    protected:
       // Our origin date-time value (T0) i.e. the first time tick
       QTime _startTime;

       // Relative time in seconds associated to our origin (T0) i.e. the first time tick
       int _startRelativeTimeInSeconds;

       // Our last date-time value (Tend) i.e. the last time tick
       QTime _endTime;

       // Total time period in minutes (Tend - T0)
       qreal _totalTimePeriodInMinutes;


       // Time in seconds between two time ticks in X axis
       int _timeRangeBetweenTimeTicksInMilliSeconds;

       // List of time ticks filtered with a given time range in milliseconds
       AbstractTimeRangeFilter _filteredListTimeTicks;


    };


    QML_DECLARE_TYPE(AbstractTimeActionslineScenarioViewController)


#endif // ABSTRACTTIMEACTIONSLINESCENARIOVIEWCONTROLLER_H
