/*
 *	IngeScape Assessments
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

#ifndef INGESCAPEASSESSMENTSCONTROLLER_H
#define INGESCAPEASSESSMENTSCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2PropertyHelpers.h>

#include <controller/assessmentsmodelmanager.h>
#include <controller/networkcontroller.h>
#include <controller/licensescontroller.h>
#include <controller/experimentationslistcontroller.h>
#include <controller/experimentationcontroller.h>
#include <controller/subject/subjectscontroller.h>
#include <controller/protocol/protocolscontroller.h>
#include <controller/export/exportcontroller.h>

#include <misc/terminationsignalwatcher.h>


/**
 * @brief The IngeScapeAssessmentsController class defines the main controller of the IngeScape Assessments application
 */
class IngeScapeAssessmentsController : public QObject
{
    Q_OBJECT

    // Network settings - network device
    I2_QML_PROPERTY(QString, networkDevice)

    // Network settings - ip address
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Network settings - port
    I2_QML_PROPERTY(uint, port)

    // Path to the directory with IngeScape licenses
    I2_QML_PROPERTY_READONLY(QString, licensesPath)

    // Snapshot Directory
    I2_QML_PROPERTY_READONLY(QString, snapshotDirectory)

    // Controller for network communication
    I2_QML_PROPERTY_READONLY(NetworkController*, networkC)

    // Controller to manage IngeScape licenses
    I2_QML_PROPERTY_READONLY(LicensesController*, licensesC)

    // Controller to manage the list of experimentations
    I2_QML_PROPERTY_READONLY(ExperimentationsListController*, experimentationsListC)

    // Controller to manage the current experimentation
    I2_QML_PROPERTY_READONLY(ExperimentationController*, experimentationC)

    // Controller to manage the subjects of the current experimentation
    I2_QML_PROPERTY_READONLY(SubjectsController*, subjectsC)

    // Controller to manage the protocols of the current experimentation
    I2_QML_PROPERTY_READONLY(ProtocolsController*, protocolsC)

    // Controller to export data from the database
    I2_QML_PROPERTY_READONLY(ExportController*, exportC)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeAssessmentsController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeAssessmentsController();


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
     * @brief If _networkDevice not available will try to auto select another one
     */
    Q_INVOKABLE bool startIngeScape();

    Q_INVOKABLE void stopIngeScape(bool hasToClearPlatform);
    Q_INVOKABLE bool restartIngeScape(bool hasToClearPlatform);


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
     * @brief Slot called when the flag "Is Connected to Database" changed
     * @param isConnectedToDatabase
     */
    void _onIsConnectedToDatabaseChanged(bool isConnectedToDatabase);


    /**
     * @brief Slot called when the current experimentation changed
     * @param value
     */
    void _onCurrentExperimentationChanged(ExperimentationM* currentExperimentation);


private:

    // To subscribe to termination signals
    TerminationSignalWatcher *_terminationSignalWatcher;

    // Agent license id needed for the app to work
    static const char * AGENT_LICENSE_ID_NEEDED;
};

QML_DECLARE_TYPE(IngeScapeAssessmentsController)

#endif // INGESCAPEASSESSMENTSCONTROLLER_H
