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
 * @brief The IngeScapeUtils namespace defines the IngeScape utils
 */
namespace IngeScapeUtils
{


    /**
      * @brief Create a directory if it does not exist
      * @param directoryPath
      */
    void createDirectoryIfNotExist(QString directoryPath);


    /**
     * @brief Get (and create if needed) the root path of our application
     * "[DocumentsLocation]/IngeScape/"
     * @return
     */
    QString getRootPath();


    /**
     * @brief Get (and create if needed) the settings path of our application
     * "[DocumentsLocation]/IngeScape/settings/"
     * @return
     */
    QString getSettingsPath();


    /**
     * @brief Get (and create if needed) the logs path of our application
     * "[DocumentsLocation]/IngeScape/logs/"
     * @return
     */
    QString getLogsPath();


    /**
     * @brief Get (and create if needed) the snapshots path of our application
     * "[DocumentsLocation]/IngeScape/snapshots/"
     * @return
     */
    QString getSnapshotsPath();


    /**
     * @brief Get (and create if needed) the path with files about platforms
     * "[DocumentsLocation]/IngeScape/platforms/"
     * @return
     */
    QString getPlatformsPath();


    /**
     * @brief Get (and create if needed) the path with files about exports
     * "[DocumentsLocation]/IngeScape/exports/"
     * @return
     */
    QString getExportsPath();


    /**
     * @brief Get (and create if needed) the path with files about licenses
     * "[DocumentsLocation]/IngeScape/licenses/"
     * @return
     */
    QString getLicensesPath();


    /**
     * @brief Get an UID for a new model of action
     * @return
     */
    int getUIDforNewActionM();


    /**
     * @brief Free an UID of a model of action
     * @param uid
     */
    void freeUIDofActionM(int uid);


    /**
     * @brief Book an UID for a new model of action
     * @param uid
     */
    void bookUIDforActionM(int uid);


    /**
     * @brief Get an UID for a new view model of action in mapping
     * @return
     */
    QString getUIDforNewActionInMappingVM();


    /**
     * @brief Free an UID of a view model of action in mapping
     * @param uid
     */
    void freeUIDofActionInMappingVM(QString uid);


    /**
     * @brief Book an UID for a new view model of action in mapping
     * @param uid
     */
    void bookUIDforActionInMappingVM(QString uid);


    /**
     * @brief Compare the values pointed by the elements of both lists
     * instead of comparing the pointers.
     */
    template<class T>
    bool arePointerListsEqual(const QList<T*> & left, const QList<T*>& right)
    {
        bool equality = true;

        // Compare the refs lists
        if (left.length() != right.length())
        {
            equality = false;
        }
        else
        {
            for(T* leftItem : left)
            {
                // Check if any element of 'right' is equal to 'leftItem'
                equality = std::any_of(right.begin(), right.end(), [leftItem](T* rightItem) {
                        return (leftItem == nullptr && rightItem == nullptr)
                        || (*leftItem == *rightItem);
                });

                if (!equality)
                {
                    break;
                }
            }
        }

        return equality;
    }

    /**
     * @brief Get (and create if needed) the fullpath of a given sub-directory
     * @param subDirectory
     * @return
     */
    QString _getSubDirectoryPath(QString subDirectory);

};

#endif // INGESCAPEUTILS_H
