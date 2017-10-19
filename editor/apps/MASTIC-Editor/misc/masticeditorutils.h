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

#ifndef MASTICEDITORUTILS_H
#define MASTICEDITORUTILS_H

#include <QObject>
#include <QtQml>




/**
 * @brief The MasticEditorUtils class defines the main controller of our application
 */
class MasticEditorUtils: public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticEditorUtils(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~MasticEditorUtils();


    /**
      * @brief Create a directory if it does not exist
      * @param directoryPath
      */
    static void createDirectoryIfNotExist(QString directoryPath);


    /**
     * @brief Get (and create if needed) the root path of our application
     * "[DocumentsLocation]/MASTIC/"
     * @return
     */
    static QString getRootPath();


    /**
     * @brief Get (and create if needed) the settings path of our application
     * "[DocumentsLocation]/MASTIC/settings/"
     * @return
     */
    static QString getSettingsPath();


    /**
     * @brief Get (and create if needed) the data path of our application
     * "[DocumentsLocation]/MASTIC/data/"
     * @return
     */
    static QString getDataPath();


    /**
     * @brief Get (and create if needed) the snapshots path of our application
     * "[DocumentsLocation]/MASTIC/snapshots/"
     * @return
     */
    static QString getSnapshotsPath();


    /**
     * @brief Get (and create if needed) the agents (Definitions and Mappings) path of our application
     * "[DocumentsLocation]/MASTIC/Agents/"
     * @return
     */
    static QString getAgentsDefinitionsAndMappingsPath();


protected:
    /**
     * @brief Get (and create if needed) the fullpath of a given sub-directory
     * @param subDirectory
     * @return
     */
    static QString _getSubDirectoryPath(QString subDirectory);
};

QML_DECLARE_TYPE(MasticEditorUtils)

#endif // MASTICEDITORUTILS_H
