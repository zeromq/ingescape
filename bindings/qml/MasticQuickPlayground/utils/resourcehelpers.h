/*
 *  Mastic - QML playground
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

#ifndef RESOURCEHELPERS_H
#define RESOURCEHELPERS_H

#include <QObject>


/**
 * @brief The ResourceHelpers class defines a set of utility functions
 */
class ResourceHelpers : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Dump a given embedded resource if needed
     * @param resourceFilePath File path of our embedded resource (e.g. :/data/myfile.txt)
     * @param targetFilePath Target file path (e.g. /home/data/myfile.txt)
     * @return true if everything is ok, false otherwise
     */
    static bool dumpEmbeddedResourceFileIfNeeded(QString resourceFilePath, QString targetFilePath);

};

#endif // RESOURCEHELPERS_H
