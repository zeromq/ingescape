/*
 *	IngeScape Measuring
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

#ifndef INGESCAPEMEASURINGCONTROLLER_H
#define INGESCAPEMEASURINGCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2PropertyHelpers.h>

#include <controller/networkcontroller.h>
#include <controller/ingescapemodelmanager.h>

#include <misc/terminationsignalwatcher.h>


/**
 * @brief The IngeScapeMeasuringController class defines the main controller of the IngeScape measuring application
 */
class IngeScapeMeasuringController : public QObject
{
    Q_OBJECT

    // Network settings - network device
    I2_QML_PROPERTY_READONLY(QString, networkDevice)

    // Network settings - ip address
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Network settings - port
    I2_QML_PROPERTY_READONLY(int, port)

    // Error message when a connection attempt fails
    I2_QML_PROPERTY_READONLY(QString, errorMessageWhenConnectionFailed)

    // Snapshot Directory
    I2_QML_PROPERTY_READONLY(QString, snapshotDirectory)

    // Manager for the data model of our IngeScape measuring application
    I2_QML_PROPERTY_READONLY(IngeScapeModelManager*, modelManager)

    // Controller for network communication
    I2_QML_PROPERTY_READONLY(NetworkController*, networkC)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeMeasuringController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeMeasuringController();


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


public Q_SLOTS:

    /**
      * @brief Method used to force the creation of our singleton from QML
      */
    void forceCreation();


private:

    // To subscribe to termination signals
    TerminationSignalWatcher *_terminationSignalWatcher;

    // Helper to manage JSON files
    //JsonHelper* _jsonHelper;

    // Path to the directory containing JSON files about platforms
    //QString _platformDirectoryPath;
    //QString _platformDefaultFilePath;

};

QML_DECLARE_TYPE(IngeScapeMeasuringController)

#endif // INGESCAPEMEASURINGCONTROLLER_H
