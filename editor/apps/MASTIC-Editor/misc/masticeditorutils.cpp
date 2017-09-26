/*
 *	MasticEditorUtils
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *
 */

#include "masticeditorutils.h"


#include <QDebug>



//--------------------------------------------------------------
//
// MasticEditorUtils
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
MasticEditorUtils::MasticEditorUtils(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
MasticEditorUtils::~MasticEditorUtils()
{
}

/**
  * @brief Create a directory if it does not exist
  * @param directoryPath
  */
void MasticEditorUtils::createDirectoryIfNotExist(QString directoryPath)
{
    // Check if the directory path is not empty
    if (!directoryPath.isEmpty())
    {
        QDir dir(directoryPath);

        // Check if the directory exists
        // NB: It should be useless because a directory can create its parent directories
        //     BUT it allows us to know exactly which directory can not be created and thus
        //     to identify permission issues
        if (!dir.exists())
        {
            if (!dir.mkpath(".")) {
                qCritical() << "ERROR: could not create directory at '" << directoryPath << "' !";
                qFatal("ERROR: could not create directory");
            }
        }
    }
}

/**
 * @brief Get (and create if needed) the root path of our application
 * "[DocumentsLocation]/MASTIC/"
 * @return
 */
QString MasticEditorUtils::getRootPath()
{
    static QString RootDirectoryPath;

    if (RootDirectoryPath.isEmpty())
    {
        QStringList documentsLocation = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        if (documentsLocation.count() > 0)
        {
            QString documentsDirectoryPath = documentsLocation.first();

            RootDirectoryPath = QString("%1%2MASTIC").arg(documentsDirectoryPath, QDir::separator());

            // Create a directory if it does not exist
            MasticEditorUtils::createDirectoryIfNotExist(RootDirectoryPath);
        }
    }

    return RootDirectoryPath;
}

/**
 * @brief Get (and create if needed) the settings path of our application
 * "[DocumentsLocation]/MASTIC/Settings/"
 * @return
 */
QString MasticEditorUtils::getSettingsPath()
{
    QString settingsDirectoryPath = QString("%1%2Settings%2").arg(MasticEditorUtils::getRootPath(), QDir::separator());

    // Create a directory if it does not exist
    MasticEditorUtils::createDirectoryIfNotExist(settingsDirectoryPath);

    return settingsDirectoryPath;
}


/**
 * @brief Get (and create if needed) the data path of our application
 * "[DocumentsLocation]/MASTIC/Data/"
 * @return
 */
QString MasticEditorUtils::getDataPath()
{
    QString resourcesDirectoryPath = QString("%1%2Data%2").arg(MasticEditorUtils::getRootPath(), QDir::separator());

    // Create a directory if it does not exist
    MasticEditorUtils::createDirectoryIfNotExist(resourcesDirectoryPath);

    return resourcesDirectoryPath;
}

/**
 * @brief Get (and create if needed) the snapshots path of our application
 * "[DocumentsLocation]/MASTIC/Snapshots/"
 * @return
 */
QString MasticEditorUtils::getSnapshotsPath()
{
    QString snapshotsDirectoryPath = QString("%1%2Snapshots%2").arg(MasticEditorUtils::getRootPath(), QDir::separator());

    // Create a directory if it does not exist
    MasticEditorUtils::createDirectoryIfNotExist(snapshotsDirectoryPath);

    return snapshotsDirectoryPath;
}

/**
 * @brief Get (and create if needed) the agents definitions path of our application
 * "[DocumentsLocation]/MASTIC/AgentsDefinitions/"
 * @return
 */
QString MasticEditorUtils::getAgentsDefinitionsPath()
{
    QString snapshotsDirectoryPath = QString("%1%2AgentsDefinitions%2").arg(MasticEditorUtils::getRootPath(), QDir::separator());

    // Create a directory if it does not exist
    MasticEditorUtils::createDirectoryIfNotExist(snapshotsDirectoryPath);

    return snapshotsDirectoryPath;
}


