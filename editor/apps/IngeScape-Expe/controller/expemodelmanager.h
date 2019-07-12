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

#ifndef EXPEMODELMANAGER_H
#define EXPEMODELMANAGER_H

#include <QObject>
#include <I2PropertyHelpers.h>
//#include <model/expeenums.h>
#include <controller/ingescapemodelmanager.h>
#include <model/platformm.h>


/**
 * @brief The ExpeModelManager class defines the manager for the data model of our IngeScape Expe application
 */
class ExpeModelManager : public IngeScapeModelManager
{
    Q_OBJECT

    // Peer id of the editor
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerIdOfEditor)

    // Peer name of the editor
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerNameOfEditor)

    // Flag indicating is there is an editor with state ON
    I2_QML_PROPERTY_READONLY(bool, isEditorON)

    // Current directory path
    I2_QML_PROPERTY_READONLY(QString, currentDirectoryPath)

    // List of platforms
    //I2_QOBJECT_LISTMODEL(PlatformM, platformsList)
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(PlatformM, platformsList)

    // Current loaded platform
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(PlatformM*, currentLoadedPlatform)


public:

    /**
     * @brief Constructor
     * @param jsonHelper
     * @param rootDirectoryPath
     * @param parent
     */
    explicit ExpeModelManager(JsonHelper* jsonHelper,
                              QString rootDirectoryPath,
                              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExpeModelManager() Q_DECL_OVERRIDE;


    /**
     * @brief List all IngeScape platforms in a directory
     * @param directoryPath
     */
    void listPlatformsInDirectory(QString directoryPath);


    /**
     * @brief Randomize the list of platforms
     */
    Q_INVOKABLE void randomizePlatformsList();


    /**
     * @brief Sort the list of platforms in alphabetical order
     */
    Q_INVOKABLE void sortPlatformsListInAlphabeticOrder();


Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Slot called when an editor enter the network
     * @param peerId
     * @param peerName
     * @param ipAddress
     * @param hostname
     */
    void onEditorEntered(QString peerId, QString peerName, QString ipAddress, QString hostname);


    /**
     * @brief Slot called when an editor quit the network
     * @param peerId
     * @param peerName
     */
    void onEditorExited(QString peerId, QString peerName);


    /**
     * @brief Slot called when an editor reply to our command "Load Platform File" with a status
     * @param commandStatus
     * @param commandParameters
     */
    void onStatusReceivedAbout_LoadPlatformFile(bool commandStatus, QString commandParameters);


private Q_SLOTS:


private:

    /**
     * @brief Get a list of random indexes
     * @param max
     * @return
     */
    QList<int> _getRandomIndexes(int max);


private:


};

QML_DECLARE_TYPE(ExpeModelManager)

#endif // EXPEMODELMANAGER_H
