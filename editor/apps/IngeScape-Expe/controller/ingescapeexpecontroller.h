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

    // Current directory path
    I2_QML_PROPERTY_READONLY(QString, currentDirectoryPath)

    // List of platform names
    I2_QML_PROPERTY_READONLY(QStringList, platformNamesList)

    // Peer id of the editor
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerIdOfEditor)

    // Peer name of the editor
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerNameOfEditor)

    // Flag indicating is there is an editor with state ON
    I2_QML_PROPERTY_READONLY(bool, isEditorON)


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
     * @brief Open a platform (at index)
     * @param index
     */
    Q_INVOKABLE void openPlatform(int index);


public Q_SLOTS:

    /**
      * @brief Method used to force the creation of our singleton from QML
      */
    void forceCreation();


private Q_SLOTS:

    /**
     * @brief Slot called when an editor enter the network
     * @param peerId
     * @param peerName
     * @param ipAddress
     * @param hostname
     */
    void _onEditorEntered(QString peerId, QString peerName, QString ipAddress, QString hostname);


    /**
     * @brief Slot called when an editor quit the network
     * @param peerId
     * @param peerName
     */
    void _onEditorExited(QString peerId, QString peerName);


    /**
     * @brief Slot called when an editor reply to our command "Load Platform File" with a status
     * @param commandStatus
     * @param commandParameters
     */
    void _onStatusReceivedAbout_LoadPlatformFile(bool commandStatus, QString commandParameters);


private:

    // To subscribe to termination signals
    TerminationSignalWatcher *_terminationSignalWatcher;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Path to the directory containing JSON files about platforms
    QString _platformDirectoryPath;

    // List of platform paths
    QStringList _platformPathsList;

};

QML_DECLARE_TYPE(IngeScapeExpeController)

#endif // INGESCAPEEXPECONTROLLER_H
