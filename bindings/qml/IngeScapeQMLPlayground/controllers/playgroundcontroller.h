/*
 *  ingeScape - QML playground
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


#ifndef PLAYGROUNDCONTROLLER_H
#define PLAYGROUNDCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QFileSystemWatcher>


#include "models/playgroundexample.h"

#include "utils/propertieshelpers.h"



/**
 * @brief The MasticPlaygroundController class defines the main controller of our playground application
 */
class PlaygroundController : public QObject
{
    Q_OBJECT

    // QML engine
    PLAYGROUND_QML_PROPERTY_READONLY_DELETE_PROOF(QQmlEngine*, qmlEngine)

    // Auto-save
    PLAYGROUND_QML_PROPERTY_CUSTOM_SETTER(bool, autoSave)

    // Auto-restart Mastic
    PLAYGROUND_QML_PROPERTY(bool, autoRestartMastic)

    // Flag indicating if we need to restart Mastic
    PLAYGROUND_QML_PROPERTY_READONLY(bool, needToRestartMastic)

    // Current view mode
    PLAYGROUND_QML_PROPERTY(QString, currentViewMode)

    // View modes
    PLAYGROUND_QML_PROPERTY_CONSTANT(QString, viewModeCodeOnly)
    PLAYGROUND_QML_PROPERTY_CONSTANT(QString, viewModeBoth)
    PLAYGROUND_QML_PROPERTY_CONSTANT(QString, viewModeViewerOnly)
    PLAYGROUND_QML_PROPERTY_READONLY(QStringList, viewModes)

    // Current file
    PLAYGROUND_QML_PROPERTY_READONLY_CUSTOM_SETTER(QUrl, currentSourceFile)

    // Content of our edited source file
    PLAYGROUND_QML_PROPERTY_CUSTOM_SETTER(QString, editedSourceContent)

    // Recent files
    PLAYGROUND_QML_PROPERTY_CUSTOM_SETTER(QVariantList, recentFiles)

    // Examples
     Q_PROPERTY(QQmlListProperty<PlaygroundExample> examples READ examples NOTIFY examplesChanged)


public:
    /**
     * @brief Default constructor
     *
     * @param engine
     * @param scriptEngine
     * @param parent
     */
    explicit PlaygroundController(QQmlEngine* engine, QJSEngine* scriptEngine, QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~PlaygroundController();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


     /**
      * @brief Get our list of examples
      * @return
      */
     QQmlListProperty<PlaygroundExample> examples();




Q_SIGNALS:
     /**
      * @brief Triggered when our example property changes
      */
     void examplesChanged();


     /**
      * @brief Triggered to clear our live view
      */
     void clearLiveView();


     /**
      * @brief Triggered to reload our live view
      */
     void reloadLiveView();


     /**
      * @brief Triggered when our user must define a save file
      */
     void userMustDefineSaveFile();


     /**
      * @brief Triggered when an action has succeeded
      * @param message
      */
     void successMessage(QString message);


     /**
      * @brief Triggered when we have an error
      * @param error
      */
     void errorMessage(QString error);


public Q_SLOTS:
     /**
      * @brief Method used to force the creation of our singleton from QML
      */
     void forceCreation();


     /**
      * @brief init
      */
     void init();


     /**
      * @brief Open a given file
      * @param file
      */
     void openFile(QUrl file);


     /**
      * @brief Create a new file
      */
     void newFile();


     /**
      * @brief Open a given example
      * @param example
      */
     void openExample(QString example);


     /**
      * @brief Save current file
      */
     void saveCurrentFile();


     /**
      * @brief Save current file in another file
      *
      * @param url
      */
     void saveCurrentFileAs(QUrl url);


     /**
      * @brief Auto-save our content with a given URL
      * @param url
      *
      * @remarks only called for embedded resources or remote (http, ftp, etc.) files
      */
     void setRequiredFileToSave(QUrl url);


protected:
     /**
      * @brief Internal setter for editedSourceContent
      * @param value
      */
     void _seteditedSourceContent(QString value);


     /**
      * @brief Trigger a reload event
      */
     void _triggerReload();


     /**
      * @brief Try to autosave our content
      */
     void _tryToAutoSave();


     /**
      * @brief Auto save our content in a given URL
      * @param url
      *
      */
     void _autoSaveWithFile(QUrl url);


     /**
      * @brief Add an item to recent files
      * @param file
      */
     void _addToRecentFiles(QUrl file);


    /**
     * @brief Open a given file
     * @param file
     */
    void _openFile(QUrl file);


    /**
     * @brief Load a given file
     * @param url
     * @return
     */
    QPair<bool, QString> _loadFile(QUrl url);


    /**
     * @brief Asynchronously write a given content to a given file
     *
     * @param content
     * @param url
     *
     * @return
     */
    bool _asyncWriteContentToFile(const QString &content, const QUrl &url);



    /**
     * @brief Convert a QURL into a QString
     * @param fileUrl
     * @return
     */
    QString _qurlToQString(const QUrl& fileUrl);



    /**
     * @brief Remove import paths associated to a given file
     * @param url
     */
    void _removeImportPathsForFile(const QUrl& url);


    /**
     * @brief Add import paths associated to a given file
     * @param url
     */
    void _addImportPathsForFile(const QUrl& url);


protected Q_SLOTS:
    /**
     * @brief Called when our file system watcher detects a file change
     */
    void _onFileSystemWatcherFileChanged(const QString &path);


    /**
     * @brief Called when our file system watcher detects a directory change
     */
    void _onFileSystemWatcherDirectoryChanged(const QString &path);


    /**
     * @brief Called when our _timerReloadOnFileSystemChanges triggers a timeout
     */
    void _onTimerReloadOnFileSystemChangesTimeout();


protected:
    // Flag indicating if we need to reset QML and Mastic
    bool _needToResetQmlAndMastic;

    // File path of our current file
    QString _currentFilePath;

    // Directory of our current file
    QDir _currentFileDirectory;

    // File watcher to detect external changes i.e. if the source code is edited in an IDE such as Qt Creator, Xcode, Visual Studio, etc.
    QFileSystemWatcher _fileSystemWatcher;

    // Timer used to reload our content when a file or a directory changes
    QTimer _timerReloadOnFileSystemChanges;

    // List of examples
    QList<PlaygroundExample*> _examples;
};


QML_DECLARE_TYPE(PlaygroundController)


#endif // PLAYGROUNDCONTROLLER_H
