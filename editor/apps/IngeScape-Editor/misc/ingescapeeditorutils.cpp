/*
 *	IngeScape Editor
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Deliencourt <deliencourt@ingenuity.io>
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *
 */

#include "ingescapeeditorutils.h"

#include <QQmlEngine>
#include <QDebug>



//--------------------------------------------------------------
//
// IngeScapeEditorUtils
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
IngeScapeEditorUtils::IngeScapeEditorUtils(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
IngeScapeEditorUtils::~IngeScapeEditorUtils()
{
}


/**
  * @brief Create a directory if it does not exist
  * @param directoryPath
  */
void IngeScapeEditorUtils::createDirectoryIfNotExist(QString directoryPath)
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
            if (!dir.mkpath("."))
            {
                qCritical() << "ERROR: could not create directory at '" << directoryPath << "' !";
                qFatal("ERROR: could not create directory");
            }
        }
    }
}


/**
 * @brief Get (and create if needed) the root path of our application
 * "[DocumentsLocation]/IngeScape/"
 * @return
 */
QString IngeScapeEditorUtils::getRootPath()
{
    static QString rootDirectoryPath;

    if (rootDirectoryPath.isEmpty())
    {
        QStringList documentsLocation = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        if (documentsLocation.count() > 0)
        {
            QString documentsDirectoryPath = documentsLocation.first();

            rootDirectoryPath = QString("%1%2IngeScape%2").arg(documentsDirectoryPath, QDir::separator());

            // Create a directory if it does not exist
            IngeScapeEditorUtils::createDirectoryIfNotExist(rootDirectoryPath);
        }
    }

    return rootDirectoryPath;
}


/**
 * @brief Get (and create if needed) the settings path of our application
 * "[DocumentsLocation]/IngeScape/Settings/"
 * @return
 */
QString IngeScapeEditorUtils::getSettingsPath()
{
    return IngeScapeEditorUtils::_getSubDirectoryPath("Settings");
}


/**
 * @brief Get (and create if needed) the data path of our application
 * "[DocumentsLocation]/IngeScape/Data/"
 * @return
 */
QString IngeScapeEditorUtils::getDataPath()
{
    return IngeScapeEditorUtils::_getSubDirectoryPath("Data");
}


/**
 * @brief Get (and create if needed) the snapshots path of our application
 * "[DocumentsLocation]/IngeScape/Snapshots/"
 * @return
 */
QString IngeScapeEditorUtils::getSnapshotsPath()
{
    return IngeScapeEditorUtils::_getSubDirectoryPath("Snapshots");
}


/**
 * @brief Get (and create if needed) the path with files about agents list
 * "[DocumentsLocation]/IngeScape/AgentsList/"
 * @return
 */
QString IngeScapeEditorUtils::getAgentsListPath()
{
    return IngeScapeEditorUtils::_getSubDirectoryPath("AgentsList");
}


/**
 * @brief Get (and create if needed) the path with files about agents mappings
 * "[DocumentsLocation]/IngeScape/AgentsMappings/"
 * @return
 */
QString IngeScapeEditorUtils::getAgentsMappingsPath()
{
    return IngeScapeEditorUtils::_getSubDirectoryPath("AgentsMappings");
}

/**
 * @brief Get (and create if needed) the path with files about scenarios
 * "[DocumentsLocation]/IngeScape/Scenarios/"
 * @return
 */
QString IngeScapeEditorUtils::getScenariosPath()
{
    return IngeScapeEditorUtils::_getSubDirectoryPath("Scenarios");
}

/**
 * @brief Get (and create if needed) the path with files about platforms
 * "[DocumentsLocation]/IngeScape/Platforms/"
 * @return
 */
QString IngeScapeEditorUtils::getPlatformsPath()
{
    return IngeScapeEditorUtils::_getSubDirectoryPath("Platforms");
}

/**
 * @brief Get (and create if needed) the fullpath of a given sub-directory
 * @param subDirectory
 * @return
 */
QString IngeScapeEditorUtils::_getSubDirectoryPath(QString subDirectory)
{
    QString subDirectoryPath = QString("%1%3%2").arg(IngeScapeEditorUtils::getRootPath(), QDir::separator(), subDirectory);

    // Create this directory if it does not exist
    IngeScapeEditorUtils::createDirectoryIfNotExist(subDirectoryPath);

    return subDirectoryPath;
}
