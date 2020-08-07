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
#include <sortFilter/platformagentsortfilter.h>


/**
 * @brief The ExpeModelManager class defines the manager for the data model of our IngeScape Expe application
 */
class ExpeModelManager : public QObject
{
    Q_OBJECT

    // Peer of the editor
    I2_CPP_NOSIGNAL_PROPERTY(PeerM*, peerOfEditor)

    // Flag indicating is there is an editor with state ON
    I2_QML_PROPERTY_READONLY(bool, isEditorON)

    // Peer of the recorder
    I2_CPP_NOSIGNAL_PROPERTY(PeerM*, peerOfRecorder)

    // Flag indicating is there is a recorder with state ON
    I2_QML_PROPERTY_READONLY(bool, isRecorderON)

    // Current directory path
    I2_QML_PROPERTY_READONLY(QString, currentDirectoryPath)

    // List of platforms
    //I2_QOBJECT_LISTMODEL(PlatformM, platformsList)
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(PlatformM, platformsList)

    // Current loaded platform
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(PlatformM*, currentLoadedPlatform)

    // List of filtered (and sorted) platform agents
    Q_PROPERTY(PlatformAgentSortFilter* filteredPlatformAgents READ filteredPlatformAgents CONSTANT)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit ExpeModelManager(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExpeModelManager();


    /**
     * @brief Get our filtered list of platform agents
     * @return
     */
    PlatformAgentSortFilter* filteredPlatformAgents()
    {
        return &_filteredPlatformAgents;
    }


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

    // Slot called when an "IngeScape Editor" enter/quit the network
    void onEditorEntered(PeerM* peer);
    void onEditorExited(PeerM* peer);


    // Slot called when an "IngeScape Recorder" enter/quit the network
    void onRecorderEntered(PeerM* peer);
    void onRecorderExited(PeerM* peer);


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

    // List of filtered (and sorted) platform agents
    PlatformAgentSortFilter _filteredPlatformAgents;

};

QML_DECLARE_TYPE(ExpeModelManager)

#endif // EXPEMODELMANAGER_H
