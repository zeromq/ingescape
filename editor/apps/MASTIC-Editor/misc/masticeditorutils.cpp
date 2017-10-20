/*
 *	MASTIC Editor
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

#include "masticeditorutils.h"

#include <QQmlEngine>
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
 * "[DocumentsLocation]/MASTIC/"
 * @return
 */
QString MasticEditorUtils::getRootPath()
{
    static QString rootDirectoryPath;

    if (rootDirectoryPath.isEmpty())
    {
        QStringList documentsLocation = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        if (documentsLocation.count() > 0)
        {
            QString documentsDirectoryPath = documentsLocation.first();

            rootDirectoryPath = QString("%1%2MASTIC%2").arg(documentsDirectoryPath, QDir::separator());

            // Create a directory if it does not exist
            MasticEditorUtils::createDirectoryIfNotExist(rootDirectoryPath);
        }
    }

    return rootDirectoryPath;
}


/**
 * @brief Get (and create if needed) the settings path of our application
 * "[DocumentsLocation]/MASTIC/Settings/"
 * @return
 */
QString MasticEditorUtils::getSettingsPath()
{
    return MasticEditorUtils::_getSubDirectoryPath("Settings");
}


/**
 * @brief Get (and create if needed) the data path of our application
 * "[DocumentsLocation]/MASTIC/Data/"
 * @return
 */
QString MasticEditorUtils::getDataPath()
{
    return MasticEditorUtils::_getSubDirectoryPath("Data");
}


/**
 * @brief Get (and create if needed) the snapshots path of our application
 * "[DocumentsLocation]/MASTIC/Snapshots/"
 * @return
 */
QString MasticEditorUtils::getSnapshotsPath()
{
    return MasticEditorUtils::_getSubDirectoryPath("Snapshots");
}


/**
 * @brief Get (and create if needed) the path with files about agents list
 * "[DocumentsLocation]/MASTIC/AgentsList/"
 * @return
 */
QString MasticEditorUtils::getAgentsListPath()
{
    return MasticEditorUtils::_getSubDirectoryPath("AgentsList");
}


/**
 * @brief Get (and create if needed) the path with files about agents mappings
 * "[DocumentsLocation]/MASTIC/AgentsMappings/"
 * @return
 */
QString MasticEditorUtils::getAgentsMappingsPath()
{
    return MasticEditorUtils::_getSubDirectoryPath("AgentsMappings");
}


/**
 * @brief Get (and create if needed) the fullpath of a given sub-directory
 * @param subDirectory
 * @return
 */
QString MasticEditorUtils::_getSubDirectoryPath(QString subDirectory)
{
    QString subDirectoryPath = QString("%1%3%2").arg(MasticEditorUtils::getRootPath(), QDir::separator(), subDirectory);

    // Create this directory if it does not exist
    MasticEditorUtils::createDirectoryIfNotExist(subDirectoryPath);

    return subDirectoryPath;
}
