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

#ifndef INGESCAPEUTILS_H
#define INGESCAPEUTILS_H

#include <QObject>
#include <QtQml>


/**
 * @brief The IngeScapeUtils class defines the IngeScape utils
 */
class IngeScapeUtils: public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit IngeScapeUtils(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~IngeScapeUtils();


    /**
      * @brief Create a directory if it does not exist
      * @param directoryPath
      */
    static void createDirectoryIfNotExist(QString directoryPath);


    /**
     * @brief Get (and create if needed) the root path of our application
     * "[DocumentsLocation]/INGESCAPE/"
     * @return
     */
    static QString getRootPath();


    /**
     * @brief Get (and create if needed) the settings path of our application
     * "[DocumentsLocation]/INGESCAPE/settings/"
     * @return
     */
    static QString getSettingsPath();


    /**
     * @brief Get (and create if needed) the snapshots path of our application
     * "[DocumentsLocation]/INGESCAPE/snapshots/"
     * @return
     */
    static QString getSnapshotsPath();


    /**
     * @brief Get (and create if needed) the path with files about platforms
     * "[DocumentsLocation]/INGESCAPE/platforms/"
     * @return
     */
    static QString getPlatformsPath();


    /**
     * @brief Get an UID for a new model of action
     * @return
     */
    static int getUIDforNewActionM();


    /**
     * @brief Free an UID of a model of action
     * @param uid
     */
    static void freeUIDofActionM(int uid);


    /**
     * @brief Book an UID for a new model of action
     * @param uid
     */
    static void bookUIDforActionM(int uid);


    /**
     * @brief Get an UID for a new view model of action in mapping
     * @return
     */
    static QString getUIDforNewActionInMappingVM();


    /**
     * @brief Free an UID of a view model of action in mapping
     * @param uid
     */
    static void freeUIDofActionInMappingVM(QString uid);


    /**
     * @brief Book an UID for a new view model of action in mapping
     * @param uid
     */
    static void bookUIDforActionInMappingVM(QString uid);


protected:
    /**
     * @brief Get (and create if needed) the fullpath of a given sub-directory
     * @param subDirectory
     * @return
     */
    static QString _getSubDirectoryPath(QString subDirectory);
};

QML_DECLARE_TYPE(IngeScapeUtils)

#endif // INGESCAPEUTILS_H
