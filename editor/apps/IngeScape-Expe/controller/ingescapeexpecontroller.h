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

    // Error message when a connection attempt fails
    I2_QML_PROPERTY_READONLY(QString, errorMessageWhenConnectionFailed)

    // Snapshot Directory
    I2_QML_PROPERTY_READONLY(QString, snapshotDirectory)

    // Manager for the data model of our IngeScape Expe application
    I2_QML_PROPERTY_READONLY(ExpeModelManager*, modelManager)

    // Controller for network communication
    I2_QML_PROPERTY_READONLY(NetworkController*, networkC)


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
     * @brief Select a directory
     */
    Q_INVOKABLE void selectDirectory();


    /**
     * @brief Open a platform
     * @param platform
     */
    Q_INVOKABLE void openPlatform(PlatformM* platform);


public Q_SLOTS:

    /**
      * @brief Method used to force the creation of our singleton from QML
      */
    void forceCreation();


private Q_SLOTS:


private:

    // To subscribe to termination signals
    TerminationSignalWatcher *_terminationSignalWatcher;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Path to the directory containing JSON files about platforms
    QString _platformDirectoryPath;

};

QML_DECLARE_TYPE(IngeScapeExpeController)

#endif // INGESCAPEEXPECONTROLLER_H
