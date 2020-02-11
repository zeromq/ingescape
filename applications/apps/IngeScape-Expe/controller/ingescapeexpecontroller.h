/*
 *	IngeScape Expe
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

#ifndef INGESCAPEEXPECONTROLLER_H
#define INGESCAPEEXPECONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2PropertyHelpers.h>

#include <controller/networkcontroller.h>
#include <controller/expemodelmanager.h>

#include <misc/terminationsignalwatcher.h>


/**
  * States of the TimeLine
  */
I2_ENUM(TimeLineStates, STOPPED, PLAYING, PAUSED)


/**
 * @brief The IngeScapeExpeController class defines the main controller of the IngeScape Expe application
 */
class IngeScapeExpeController : public QObject
{
    Q_OBJECT

    // Network settings - network device
    I2_QML_PROPERTY_READONLY(QString, networkDevice)

    // Network settings - ip address
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Network settings - port
    I2_QML_PROPERTY_READONLY(uint, port)

    // Path to the directory with IngeScape licenses
    I2_QML_PROPERTY_READONLY(QString, licensesPath)

    // Error message when a connection attempt fails
    I2_QML_PROPERTY_READONLY(QString, errorMessageWhenConnectionFailed)

    // Snapshot Directory
    I2_QML_PROPERTY_READONLY(QString, snapshotDirectory)

    // Manager for the data model of our IngeScape Expe application
    I2_QML_PROPERTY_READONLY(ExpeModelManager*, modelManager)

    // Controller for network communication
    I2_QML_PROPERTY_READONLY(NetworkController*, networkC)

    // Flag indicating if the timeline (scenario) is currently playing in the "Editor app"
    //I2_QML_PROPERTY_READONLY(bool, isPlayingTimeLine)

    // Current state of the TimeLine
    I2_QML_PROPERTY_READONLY(TimeLineStates::Value, timeLineState)

    // Flag indicating if the "Recorder app" is currently recording
    //I2_QML_PROPERTY_READONLY(bool, isRecording)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeExpeController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeExpeController();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
    static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


    /**
      * @brief Actions to perform before the application closing
      */
    Q_INVOKABLE void processBeforeClosing();


    /**
     * @brief Re-Start the network with a port and a network device
     * @param strPort
     * @param networkDevice
     * @param hasToClearPlatform
     * @return true when success
     */
    //Q_INVOKABLE bool restartNetwork(QString strPort, QString networkDevice, bool hasToClearPlatform);


    /**
     * @brief Select a directory
     */
    Q_INVOKABLE void selectDirectory();


    /**
     * @brief Open a platform
     * @param platform
     */
    Q_INVOKABLE void openPlatform(PlatformM* platform);


    /**
     * @brief Play the TimeLine and Start to record if flag "withRecord" is true
     * OR only resume the TimeLine
     * @param withRecord
     */
    Q_INVOKABLE void playOrResumeTimeLine(bool withRecord);


    /**
     * @brief Pause the TimeLine
     */
    Q_INVOKABLE void pauseTimeLine();


    /**
     * @brief Stop the TimeLine
     */
    Q_INVOKABLE void stopTimeLine();


public Q_SLOTS:

    /**
      * @brief Method used to force the creation of our singleton from QML
      */
    void forceCreation();


private Q_SLOTS:

    /**
     * @brief Called when our network device is not available
     */
    void _onNetworkDeviceIsNotAvailable();


    /**
     * @brief Called when our network device is available again
     */
    void _onNetworkDeviceIsAvailableAgain();


    /**
     * @brief Called when our network device has a new IP address
     */
    void _onNetworkDeviceIpAddressHasChanged();


    /**
     * @brief Called when our machine will go to sleep
     */
    void _onSystemSleep();


    /**
     * @brief Called when our machine did wake from sleep
     */
    void _onSystemWake();


    /**
     * @brief Called when a network configuration is added, removed or changed
     */
    void _onSystemNetworkConfigurationsUpdated();


    /**
     * @brief Slot called when the state of the TimeLine updated (in Editor app)
     * @param parameters
     */
    void _onTimeLineStateUpdated(QString parameters);


private:

    /**
     * @brief Start IngeScape
     *
     * @param checkAvailableNetworkDevices
     *
     * @return
     */
    bool _startIngeScape(bool checkAvailableNetworkDevices);


    /**
     * @brief Restart IngeScape
     *
     * @param hasToClearPlatform
     * @param checkAvailableNetworkDevices
     *
     *
     * @return true if success
     */
    bool _restartIngeScape(bool hasToClearPlatform, bool checkAvailableNetworkDevices = false);


    /**
     * @brief Stop IngeScape
     *
     * @param hasToClearPlatform
     */
    void _stopIngeScape(bool hasToClearPlatform);


    /**
     * @brief Start Recording
     */
    void _startRecording();


    /**
     * @brief Stop Recording
     */
    void _stopRecording();


private:

    // To subscribe to termination signals
    TerminationSignalWatcher *_terminationSignalWatcher;

    // States of our mapping when our network was stopped
    bool _beforeNetworkStop_isMappingConnected;

    // Path to the directory containing JSON files about platforms
    QString _platformDirectoryPath;

    // Flag indicating if the timeline is played with record or not
    bool _withRecord;

};

QML_DECLARE_TYPE(IngeScapeExpeController)

#endif // INGESCAPEEXPECONTROLLER_H
