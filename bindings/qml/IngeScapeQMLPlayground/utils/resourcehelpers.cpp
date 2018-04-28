/*
 *  IngeScape - QML playground
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#include "resourcehelpers.h"


#include <QDebug>
#include <QFile>
#include <QDir>



/**
  * @brief Dump a given embedded resource if needed
  * @param resourceFilePath File path of our embedded resource (e.g. :/data/myfile.txt)
  * @param targetFilePath Target file path (e.g. /home/data/myfile.txt)
  * @return true if everything is ok, false otherwise
  */
bool ResourceHelpers::dumpEmbeddedResourceFileIfNeeded(QString resourceFilePath, QString targetFilePath)
{
    bool isOk = false;

    // Check if both file paths are valid
    if (!resourceFilePath.isEmpty() && !targetFilePath.isEmpty())
    {
        // Check if our resource file exists
        QFile resourceFile(resourceFilePath);
        if (resourceFile.exists())
        {
            // Check if our target file already exists
            QFile targetFile(targetFilePath);
            if (!targetFile.exists())
            {
                // Ensure that all parent directories exist
                QFileInfo targetFileInfo(targetFile);
                QDir targetDirectory = targetFileInfo.absoluteDir();
                if (!targetDirectory.exists())
                {
                    if (!targetDirectory.mkpath("."))
                    {
                        qCritical() << Q_FUNC_INFO << ": the target directory" << targetDirectory.absolutePath() << "can not be created";
                    }
                }

                if (targetDirectory.exists())
                {
                    if (QFile::copy(resourceFilePath, targetFilePath))
                    {
                        // Update permissions of our new file
                        QFileDevice::Permissions filePermissions =  QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ReadGroup | QFileDevice::WriteGroup;
                        if (!QFile::setPermissions(targetFilePath, filePermissions))
                        {
                            qWarning() << Q_FUNC_INFO << ": failed to set read-write permissions of" << targetFilePath;
                        }
                        else
                        {
                            // Everything is ok
                            isOk = true;
                        }
                    }
                    else
                    {
                        qCritical() << Q_FUNC_INFO << ": failed to copy embedded file" << resourceFilePath << "to" << targetFilePath;
                    }
                }
                else
                {
                    qCritical() << Q_FUNC_INFO << ": failed to copy embedded file" << resourceFilePath << "to" << targetFilePath << "because the parent directory can not be created";
                }
            }
            else
            {
                // Else: nothing to do
                isOk = true;
            }
        }
        else
        {
            qWarning() << Q_FUNC_INFO << ": embedded file " << resourceFilePath << "does not exist";
        }
    }

    return isOk;
}

