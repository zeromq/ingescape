/*
 *	I2Quick
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *      Luc Sorignet <sorignet@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef I2UTILS_H
#define I2UTILS_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

// To export symbols
#include "i2quick_global.h"

/**
 * i2_qlist_cast cast a Qlist<T> to a QList<T2>
 * (cast each element)
 * T and T2 must be pointer
 * T and T2 must be castable
 */
 template <class T,class T2> QList<T2> i2_qlist_cast(QList<T> list)
{
     QList<T2> listAnyClass = QList<T2>();
       for( int i = 0; i < list.length(); i++ )
       {
           T2 pAnyClass = qobject_cast<T2>(list.at(i));

           if(pAnyClass)
           {
               listAnyClass.append(pAnyClass);
           }
       }
       return listAnyClass;
}

/**
 * @brief The I2Utils class defines a set of utility functions
 */
class I2QUICK_EXPORT I2Utils : public QObject
{
    Q_OBJECT

    // Flag indicating if our application is launched on a mobile device (iOS, Android) or not
    I2_QML_PROPERTY_READONLY(bool, isMobileAplication)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit I2Utils(QObject *parent = 0);


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
    static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


    /**
     * @brief Get all the directories associated to QStandardPaths::DesktopLocation (i.e. user's desktop)
     * @return
     */
    Q_INVOKABLE QStringList getStandardDesktopLocations();


    /**
     * @brief Get all the directories associated to QStandardPaths::DocumentsLocation (i.e. directories containing user document files)
     * @return
     */
    Q_INVOKABLE QStringList getStandardDocumentsLocations();


    /**
      * @brief Create a directory if it does not exist
      * @param directoryPath
      */
    static void createDirectoryIfNotExist(QString directoryPath);


    /**
     * @brief DEPRECATED get the root path where are stored all app data or create it if not exist
     * @param appRootDirName name of the application (or name of the dir)
     * @return
     */
    static QString getOrCreateAppRootPathInDocumentDir(QString appRootDirName="App");


    /**
     * @brief Dump app ressource dir embedded as resources if needed
     * @param ressourceDirPath Source directory to dump
     * @param targetDirectoryPath Target directory for our embedded model data
     * @return true if everything is ok, false otherwise
     */
    static bool dumpAppResourcesDirIfNeeded(QString ressourceDirPath, QString targetDirectoryPath);


    /**
     * @brief Dump a given embedded resource if needed
     * @param resourceFilePath File path of our embedded resource (e.g. :/data/myfile.txt)
     * @param targetFilePath Target file path (e.g. /home/data/myfile.txt)
     * @return true if everything is ok, false otherwise
     */
    static bool dumpEmbeddedResourceFileIfNeeded(QString resourceFilePath, QString targetFilePath);


    /**
     * @brief Get (and create if needed) the root path of the current application
     * "[Documents Location]/[Root Directory Name]/"
     * @param rootDirectoryName
     * @return
     */
    static QString getRootPathInDocuments(QString rootDirectoryName = "App");


    /**
     * @brief Get (and create if needed) the path of the sub directory of the current application
     * "[Documents Location]/[Root Directory Name]/[Sub Directory Name]"
     * @param subDirectoryName
     * @return
     */
    static QString getSubDirectoryPathInDocuments(QString subDirectoryName);

};

QML_DECLARE_TYPE(I2Utils)

#endif // I2UTILS_H
