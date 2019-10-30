/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

#include "ingescapeutils.h"

#include <QQmlEngine>
#include <QDebug>


// Biggest unique id of action model
static int BIGGEST_UID_OF_ACTION_MODEL = -1;

// Biggest unique id of action in mapping view model
static int BIGGEST_UID_OF_ACTION_IN_MAPPING_VIEW_MODEL = -1;


//--------------------------------------------------------------
//
// IngeScape Utils
//
//--------------------------------------------------------------


/**
  * @brief Create a directory if it does not exist
  * @param directoryPath
  */
void IngeScapeUtils::createDirectoryIfNotExist(QString directoryPath)
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
QString IngeScapeUtils::getRootPath()
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
            IngeScapeUtils::createDirectoryIfNotExist(rootDirectoryPath);
        }
    }

    return rootDirectoryPath;
}


/**
 * @brief Get (and create if needed) the settings path of our application
 * "[DocumentsLocation]/IngeScape/settings/"
 * @return
 */
QString IngeScapeUtils::getSettingsPath()
{
    return IngeScapeUtils::_getSubDirectoryPath("settings");
}


/**
 * @brief Get (and create if needed) the logs path of our application
 * "[DocumentsLocation]/IngeScape/logs/"
 * @return
 */
QString IngeScapeUtils::getLogsPath()
{
    return IngeScapeUtils::_getSubDirectoryPath("logs");
}


/**
 * @brief Get (and create if needed) the snapshots path of our application
 * "[DocumentsLocation]/IngeScape/snapshots/"
 * @return
 */
QString IngeScapeUtils::getSnapshotsPath()
{
    return IngeScapeUtils::_getSubDirectoryPath("snapshots");
}


/**
 * @brief Get (and create if needed) the path with files about platforms
 * "[DocumentsLocation]/IngeScape/platforms/"
 * @return
 */
QString IngeScapeUtils::getPlatformsPath()
{
    return IngeScapeUtils::_getSubDirectoryPath("platforms");
}


/**
 * @brief Get (and create if needed) the path with files about exports
 * "[DocumentsLocation]/IngeScape/exports/"
 * @return
 */
QString IngeScapeUtils::getExportsPath()
{
    return IngeScapeUtils::_getSubDirectoryPath("exports");
}


/**
 * @brief Get (and create if needed) the path with files about licenses
 * "[DocumentsLocation]/IngeScape/licenses/"
 * @return
 */
QString IngeScapeUtils::getLicensesPath()
{
    return IngeScapeUtils::_getSubDirectoryPath("licenses");
}


/**
 * @brief Get an UID for a new model of action
 * @return
 */
int IngeScapeUtils::getUIDforNewActionM()
{
    BIGGEST_UID_OF_ACTION_MODEL++;

    return BIGGEST_UID_OF_ACTION_MODEL;
}


/**
 * @brief Free an UID of a model of action
 * @param uid
 */
void IngeScapeUtils::freeUIDofActionM(int uid)
{
    // Decrement only if the uid correspond to the biggest one
    if (uid == BIGGEST_UID_OF_ACTION_MODEL)
    {
        BIGGEST_UID_OF_ACTION_MODEL--;
    }
}


/**
 * @brief Book an UID for a new model of action
 * @param uid
 */
void IngeScapeUtils::bookUIDforActionM(int uid)
{
    if (uid > BIGGEST_UID_OF_ACTION_MODEL) {
        BIGGEST_UID_OF_ACTION_MODEL = uid;
    }
}


/**
 * @brief Get an UID for a new view model of action in mapping
 * @return
 */
QString IngeScapeUtils::getUIDforNewActionInMappingVM()
{
    BIGGEST_UID_OF_ACTION_IN_MAPPING_VIEW_MODEL++;

    return QString::number(BIGGEST_UID_OF_ACTION_IN_MAPPING_VIEW_MODEL);
}


/**
 * @brief Free an UID of a view model of action in mapping
 * @param uid
 */
void IngeScapeUtils::freeUIDofActionInMappingVM(QString uid)
{
    bool success = false;
    int nUID = uid.toInt(&success);

    // Decrement only if the uid correspond to the biggest one
    if (success && (nUID == BIGGEST_UID_OF_ACTION_IN_MAPPING_VIEW_MODEL))
    {
        BIGGEST_UID_OF_ACTION_IN_MAPPING_VIEW_MODEL--;
    }
}


/**
 * @brief Book an UID for a new view model of action in mapping
 * @param uid
 */
void IngeScapeUtils::bookUIDforActionInMappingVM(QString uid)
{
    bool success = false;
    int nUID = uid.toInt(&success);

    // This UID is greater than the static variable, update it
    if (success && (nUID > BIGGEST_UID_OF_ACTION_IN_MAPPING_VIEW_MODEL)) {
        BIGGEST_UID_OF_ACTION_IN_MAPPING_VIEW_MODEL = nUID;
    }
}


/**
 * @brief Get (and create if needed) the fullpath of a given sub-directory
 * @param subDirectory
 * @return
 */
QString IngeScapeUtils::_getSubDirectoryPath(QString subDirectory)
{
    QString subDirectoryPath = QString("%1%3%2").arg(IngeScapeUtils::getRootPath(), QDir::separator(), subDirectory);

    // Create this directory if it does not exist
    IngeScapeUtils::createDirectoryIfNotExist(subDirectoryPath);

    return subDirectoryPath;
}
