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


#include "masticplaygroundcontroller.h"

#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QFileDialog>
#include <QDirIterator>

#include "MasticQuick.h"


// Maximum number of recent files
#define MAXIMUM_NUMBER_OF_RECENT_FILES 5


// Embedded directory of examples
#define QRC_EXAMPLES_DIRECTORY ":/resources/examples/"



//------------------------------------------------------------
//
// MasticPlaygroundController
//
//------------------------------------------------------------


/**
 * @brief Default constructor
 *
 * @param engine
 * @param scriptEngine
 * @param parent
 */
MasticPlaygroundController::MasticPlaygroundController(QQmlEngine* engine, QJSEngine* scriptEngine, QObject *parent)
    : QObject(parent),
      _qmlEngine(NULL),
      _autoSave(true),
      _autoRestartMastic(false),
      _needToRestartMastic(false),
      _needToResetQmlAndMastic(false)
{
    Q_UNUSED(scriptEngine)

    // Set our QML engine
    setqmlEngine(engine);


     //-------------------------------
    //
    // View modes
    //
    //-------------------------------
    _viewModeCodeOnly = tr("Code");
    _viewModeBoth = tr("Both");
    _viewModeViewerOnly = tr("Viewer");
    _currentViewMode = _viewModeBoth;
    _viewModes << _viewModeCodeOnly << _viewModeBoth << _viewModeViewerOnly;



    //-------------------------------
    //
    // List of examples
    //
    //-------------------------------

    // - V1: get all files in our examples directory
    /*
    QDir qrcExamplesDirectory(QRC_EXAMPLES_DIRECTORY);
    if (qrcExamplesDirectory.exists())
    {
        qrcExamplesDirectory.setNameFilters(QStringList() << "*.qml");
        QStringList examplesFiles = qrcExamplesDirectory.entryList(QDir::Files);
        qDebug() << "Examples:" << examplesFiles;
        // TODO
    }
    */

    // - V2: custom description for each example
    _examples
            << new PlaygroundExample("Clock.qml", "Clock: set time based on inputs")
            << new PlaygroundExample("DragNDrop.qml", "Drag-n-drop: send 2D absolute coordinates")
            << new PlaygroundExample("VirtualJoystick.qml", "Virtual joystick: send 2D relative coordinates")
            ;

}



/**
  * @brief Destructor
  */
MasticPlaygroundController::~MasticPlaygroundController()
{
    // Clean-up examples
    QList<PlaygroundExample*> tempExamples = _examples;
    _examples.clear();
    Q_EMIT examplesChanged();
    qDeleteAll(tempExamples);
    tempExamples.clear();
}



/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* MasticPlaygroundController::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    return new MasticPlaygroundController(engine, scriptEngine);
}



//------------------------------------------------------------
//
// Custom setters and getters
//
//------------------------------------------------------------


/**
 * @brief Set our autoSave flag
 * @param value
 */
void MasticPlaygroundController::setautoSave(bool value)
{
    if (_autoSave != value)
    {
        // Save our new value
        _autoSave = value;

        // Save if needed
        if (_autoSave)
        {
            _tryToAutoSave();
        }

        // Notify change
        Q_EMIT autoSaveChanged(value);
    }
}


/**
 * @brief Set URL of our current source file
 * @param value
 */
void MasticPlaygroundController::setcurrentSourceFile(QUrl value)
{
    if (_currentSourceFile != value)
    {
        // Clean-up
        _removeImportPathsForFile(_currentSourceFile);

        // Save our new value
        _currentSourceFile = value;

        // Update QQmlEngine and our file system watcher if needed
        _addImportPathsForFile(_currentSourceFile);

        // Notify change
        Q_EMIT currentSourceFileChanged(value);
    }
}


/**
 * @brief Set our list of recent files
 * @param value
 */
void MasticPlaygroundController::setrecentFiles(QVariantList value)
{
    // Remove all invalid values
    value.removeAll(QVariant());

    // Remove last items if needed
    while (value.count() > MAXIMUM_NUMBER_OF_RECENT_FILES)
    {
        value.removeLast();
    }

    if (_recentFiles != value)
    {
        _recentFiles = value;

        Q_EMIT recentFilesChanged( value );
    }
}


/**
 * @brief MasticPlaygroundController::seteditedSourceContent
 * @param value
 */
void MasticPlaygroundController::seteditedSourceContent(QString value)
{
    if (_editedSourceContent != value)
    {
        // Save change
        _editedSourceContent = value;

        // Trigger a reload
        _triggerReload();

        // Write content if needed
        if (_autoSave)
        {
            _tryToAutoSave();
        }

        // Notify change
        Q_EMIT editedSourceContentChanged(value);
    }
}



/**
 * @brief Get our list of examples
 * @return
 */
QQmlListProperty<PlaygroundExample> MasticPlaygroundController::examples()
{
    return QQmlListProperty<PlaygroundExample>(this, _examples);
}



//------------------------------------------------------------
//
// Public methods
//
//------------------------------------------------------------


/**
 * @brief Method used to force the creation of our singleton from QML
 */
void MasticPlaygroundController::forceCreation()
{
}


/**
 * @brief init
 */
void MasticPlaygroundController::init()
{
    // Check if we have at least one recent file
    if (_recentFiles.count() > 0)
    {
        // Open our last file
        _openFile(_recentFiles.at(0).toUrl());
    }
    else
    {
        // Open a new file
        newFile();
    }
}



/**
 * @brief Open a given file
 * @param url
 */
void MasticPlaygroundController::openFile(QUrl url)
{
    if ((url != _currentSourceFile) && url.isValid())
    {
        // Add it to recent files
        _addToRecentFiles(url);

        // Open it
        _openFile(url);
    }
    // Else: invalid URL => invalid file
    else
    {
        QString error = tr("Invalid URL: can not open file %1").arg(url.toString());
        Q_EMIT errorMessage(error);
    }
}


/**
 * @brief Create a new file
 */
void MasticPlaygroundController::newFile()
{
    // Open our template file
    // NB: we don't want to add it to our recent files
    _openFile(QUrl("qrc:/resources/NewFile.qml"));
}


/**
 * @brief Open a given example
 * @param example
 */
void MasticPlaygroundController::openExample(QString example)
{
    if (!example.isEmpty())
    {
        QString stringURL = QString("qrc%1%2").arg(QRC_EXAMPLES_DIRECTORY, example);

        openFile(QUrl(stringURL));
    }
}


/**
 * @brief Save current file
 */
void MasticPlaygroundController::saveCurrentFile()
{
    if (_currentSourceFile.isValid())
    {
        if (!_currentSourceFile.isLocalFile())
        {
            // Embedded file or remote file (http, ftp, etc.)

            Q_EMIT userMustDefineSaveFile();
        }
        else
        {
            // Local file

            // Create a watcher to monitor our async task
            QFutureWatcher<bool>* futureWatcher = new QFutureWatcher<bool>();
            connect(
                    futureWatcher, &QFutureWatcher<bool>::finished,
                    this, [this, futureWatcher]() {
                        // Get result of our future
                        QFuture<bool> future = futureWatcher->future();
                        bool result = future.result();

                        if (result)
                        {
                            //
                            // Success
                            //

                            QString success = tr("Source code saved");
                            Q_EMIT successMessage( success);
                        }
                        else
                        {
                            //
                            // Failure
                            //

                            QString error = tr("Write error: can not save source code. Source file %1 is read-only").arg(_currentSourceFile.toString());
                            Q_EMIT errorMessage(error);
                        }

                        // Clean-up
                        disconnect(futureWatcher, 0, this, 0);
                        futureWatcher->deleteLater();
                    });


            // Create our async task
            QFuture<bool> future = QtConcurrent::run(this, &MasticPlaygroundController::_asyncWriteContentToFile, _editedSourceContent, _currentSourceFile);
            futureWatcher->setFuture(future);
        }
    }
    // Else: invalid file => should not happen because we can not open an invalid file
    else
    {
        QString error = tr("Invalid URL: can not save source code. Source file %1 is invalid").arg(_currentSourceFile.toString());
        Q_EMIT errorMessage(error);
    }
}



/**
 * @brief Save current file in another file
 *
 * @param url
 */
void MasticPlaygroundController::saveCurrentFileAs(QUrl url)
{
    if (url.isValid())
    {
        // Create a watcher to monitor our async task
        QFutureWatcher<bool>* futureWatcher = new QFutureWatcher<bool>();
        connect(
                futureWatcher, &QFutureWatcher<bool>::finished,
                this, [this, futureWatcher, url]() {
                    // Get result of our future
                    QFuture<bool> future = futureWatcher->future();
                    bool result = future.result();

                    if (result)
                    {
                        //
                        // Success
                        //

                        QString success = tr("Source code saved as %1").arg(url.toString());
                        Q_EMIT successMessage( success);
                    }
                    else
                    {
                        //
                        // Failure
                        //

                        QString error = tr("Write error: failed to save as %1").arg(url.toString());
                        Q_EMIT errorMessage(error);
                    }

                    // Clean-up
                    disconnect(futureWatcher, 0, this, 0);
                    futureWatcher->deleteLater();
                });


        // Create our async task
        QFuture<bool> future = QtConcurrent::run(this, &MasticPlaygroundController::_asyncWriteContentToFile, _editedSourceContent, url);
        futureWatcher->setFuture(future);
    }
    else
    {
        QString error = tr("Invalid URL: failed to save as %1").arg(url.toString());
        Q_EMIT errorMessage(error);
    }
}



/**
 * @brief Set file for auto-save
 * @param url
 *
 * @remarks only called for embedded resources or remote (http, ftp, etc.) files
 */
void MasticPlaygroundController::setRequiredFileToSave(QUrl url)
{
    _autoSaveWithFile(url);
}



//------------------------------------------------------------
//
// Protected methods
//
//------------------------------------------------------------



/**
 * @brief Internal setter for editedSourceContent
 * @param value
 */
void MasticPlaygroundController::_seteditedSourceContent(QString value)
{
    if (_editedSourceContent != value)
    {
        // Save change
        _editedSourceContent = value;

        // Notify change
        Q_EMIT editedSourceContentChanged(value);
    }
}



/**
 * @brief Trigger a reload event
 */
void MasticPlaygroundController::_triggerReload()
{
    // Clean-up our live view
    Q_EMIT clearLiveView();


    // NB: QML needs a small delay to delete its content
    QTimer::singleShot(50, [=] {
        //
        // Clean-up Mastic and QML if needed
        //
        if (_needToResetQmlAndMastic)
        {
            // Clean Mastic quick
            MasticQuick* masticQuick = MasticQuick::instance();
            if (masticQuick != NULL)
            {
                // Set our needToRestartMastic flag
                setneedToRestartMastic( masticQuick->isStarted() && _autoRestartMastic );

                // Stop Mastic properly
                masticQuick->stop();

                // Clear definition
                masticQuick->clearDefinition();
            }

            // Clean-up QML
            if (_qmlEngine != NULL)
            {
                _qmlEngine->clearComponentCache();
            }
        }
        else
        {
            // It is our first call to _triggerReload()
            // We will need to reset QML and Mastic before rendering a second file
            _needToResetQmlAndMastic = true;

            // Set our needToRestartMastic flag
            setneedToRestartMastic( _autoRestartMastic );
        }

        // Reload our live view
        Q_EMIT reloadLiveView();
    });
}



/**
 * @brief Try to autosave our content
 */
void MasticPlaygroundController::_tryToAutoSave()
{
    // Check if we have a valid URL
    if (_currentSourceFile.isValid())
    {
        // Check if we have opened a local file
        if (!_currentSourceFile.isLocalFile())
        {
            // Embedded file (qrc://...) or remote file (http, ftp, etc.)
            // => Our end-user must define a local file to save changes
            Q_EMIT userMustDefineSaveFile();
        }
        else
        {
            // Local file
            // FIXME: we assume that our file is writable
            _autoSaveWithFile(_currentSourceFile);
        }
    }
    // Else: invalid file => should not happen because we can not open an invalid file
    else
    {
        QString error = tr("Invalid URL: can not auto-save source code. Source file %1 is invalid").arg(_currentSourceFile.toString());
        Q_EMIT errorMessage(error);
    }
}



/**
 * @brief Auto save our content in a given URL
 * @param url
 *
 */
void MasticPlaygroundController::_autoSaveWithFile(QUrl url)
{
    // Ensure that we have a valid URL
    if (url.isValid())
    {
        // Create a watcher to monitor our async task
        QFutureWatcher<bool>* futureWatcher = new QFutureWatcher<bool>();
        connect(
                futureWatcher, &QFutureWatcher<bool>::finished,
                this, [this, futureWatcher, url]() {
                        // Get result of our future
                        QFuture<bool> future = futureWatcher->future();
                        bool result = future.result();

                        if (result)
                        {
                            //
                            // Success
                            //

                            // Update our current file
                            setcurrentSourceFile(url);

                            // Add it to recent files
                            _addToRecentFiles(url);

                            // Notify that everything is ok
                            QString success = tr("Source code saved");
                            Q_EMIT successMessage(success);
                        }
                        else
                        {
                            //
                            // Failure
                            //

                            QString error = tr("Write error: can not save source code. Source file %1 is read-only").arg(url.toString());
                            Q_EMIT errorMessage(error);
                        }

                        // Clean-up
                        disconnect(futureWatcher, 0, this, 0);
                        futureWatcher->deleteLater();
                    });


        // Create our async task
        QFuture<bool> future = QtConcurrent::run(this, &MasticPlaygroundController::_asyncWriteContentToFile, _editedSourceContent, url);
        futureWatcher->setFuture(future);
    }
    // Else: invalid URL
}



/**
 * @brief Add an item to recent files
 * @param url
 */
void MasticPlaygroundController::_addToRecentFiles(QUrl url)
{
    // Ensure that we have a valid URL
    if (url.isValid())
    {
        QVariant variantURL = QVariant(url.toString());

        // Add it to our list of recent files
        // NB: we remove all previous occurrences to avoid multiple entries for the same file
        QVariantList newRecentFiles = QVariantList(_recentFiles);
        if (newRecentFiles.contains(variantURL))
        {
            newRecentFiles.removeAll(variantURL);
        }

        // The first item is the most recent file
        newRecentFiles.prepend(variantURL);

        setrecentFiles( newRecentFiles );
    }
    // Else: invalid URL
}


/**
* @brief Open a given file
* @param url
*/
void MasticPlaygroundController::_openFile(QUrl url)
{
    // Create a future wacther and subscribe to it
    QFutureWatcher<QPair<bool, QString>>* futureWatcher = new QFutureWatcher<QPair<bool, QString>>();
    connect(
            futureWatcher, &QFutureWatcher<QPair<bool, QString>>::finished,
            this, [this, futureWatcher, url]() {
                // Get result of our future
                QFuture<QPair<bool, QString>> future = futureWatcher->future();
                QPair<bool, QString> result = future.result();

                if (result.first)
                {
                    //
                    // Success
                    //

                    // Save our source file
                    setcurrentSourceFile(url);

                    // Set content of our source file
                    // NB: we use an internal setter because we don't want to save a new file
                    _seteditedSourceContent(result.second);

                    // Trigger a reload
                    _triggerReload();

                    // Notify that everything is ok
                    QString success = tr("Source file %1 opened").arg(url.toString());
                    Q_EMIT successMessage(success);
                }
                // Else: failure
                else
                {
                    QString error = tr("Read error: can not read content of source file %1").arg(url.toString());
                    Q_EMIT errorMessage(error);
                }


                // Clean-up
                disconnect(futureWatcher, 0, this, 0);
                futureWatcher->deleteLater();
            });


    // Create an async task and link it to our watcher
    QFuture<QPair<bool, QString>> future = QtConcurrent::run(this, &MasticPlaygroundController::_loadFile, url);
    futureWatcher->setFuture(future);
}



/**
 * @brief Load a given file
 * @param url
 * @return
 */
QPair<bool, QString> MasticPlaygroundController::_loadFile(QUrl url)
{
   QPair<bool, QString> result =  QPair<bool, QString>(false, "");

   // Check if we have a valid URL
   if (url.isValid())
   {
       QString filePath = _qurlToQString(url);

       // Check if we have a valid file path
       if (!(filePath.isEmpty()))
       {
           QFile file(filePath);
           //NB: we must check if file exists, otherwise QFile::open will try to create a new file before opening it
           if (file.exists())
           {
              if (file.open(QFile::ReadOnly | QFile::Text))
              {
                  result.first = true;
                  result.second = file.readAll();
                  file.close();
              }
              else
              {
                  qWarning() << "MasticPlaygroundController warning: can not read file " << filePath;
              }
           }
           else
           {
               qWarning() << "MasticPlaygroundController warning: file " << filePath << " does not exist";
           }
       }
   }

   return result;
}



/**
 * @brief Asynchronously write a given content to a given file
 *
 * @param content
 * @param url
 *
 * @return
 */
bool MasticPlaygroundController::_asyncWriteContentToFile(const QString &content, const QUrl &url)
{
    bool result = false;

    // Check if we have a valid URL
    if (url.isValid())
    {
        // Open our file
        QFile file(_qurlToQString(url));
        if (file.open(QIODevice::WriteOnly))
        {
            // Copy content
            QTextStream out(&file);
            out << content;

            // Close our file
            file.flush();
            file.close();

            result = true;
        }
        // Else: file is read-only
    }

    return result;
}



/**
 * @brief Convert a QURL into a QString
 * @param fileUrl
 * @return
 */
QString MasticPlaygroundController::_qurlToQString(const QUrl& fileUrl)
{
    QString result;

    if (!(fileUrl.isEmpty()) && (fileUrl.isValid()))
    {
        // Check if we have a local file
        if (fileUrl.isLocalFile())
        {
            result = fileUrl.toLocalFile();
        }
        else
        {
            QString filePath = fileUrl.toString();
            if (filePath.startsWith("qrc:"))
            {
                // A lot of Qt objects (QFileSystemWatcher, QFile, etc.) don't accept URLs
                // starting with 'qrc://'
                // We must replace 'qrc://' by a simple '://' to avoid issues
                filePath = filePath.mid(3);
            }

            result = filePath;
        }
    }
    // Else: empty or invalid URL

    return result;
}



/**
 * @brief Remove import paths associated to a given file
 * @param url
 */
void MasticPlaygroundController::_removeImportPathsForFile(const QUrl& url)
{
    if (url.isValid())
    {
        // Check if we have a local file
        if (url.isLocalFile())
        {
            QString stringURL = _qurlToQString(url);
            QFileInfo fileInfo(stringURL);
            QDir directory = fileInfo.absoluteDir();

            // QQmlEngine
            if (_qmlEngine != NULL)
            {
                QStringList importPathsList = _qmlEngine->importPathList();
                importPathsList.removeAll( directory.absolutePath());
                _qmlEngine->setImportPathList(importPathsList);
            }

            // File system watcher
            // TODO
        }
        else if (QString::compare(url.scheme(), "qrc", Qt::CaseInsensitive) == 0)
        {
            // Embedded file
            // => Nothing to do
            //    We MUST remove nothing because its import path is the one of our application
        }
        else
        {
            // Remote file (http, ftp, etc.)

            // TODO: remove import paths if we add import paths
        }
    }
    // Else: invalid URL => nothing to remove
}


/**
 * @brief Add import paths associated to a given file
 * @param url
 */
void MasticPlaygroundController::_addImportPathsForFile(const QUrl& url)
{
    if (url.isValid())
    {
        // Check if we have a local file
        if (url.isLocalFile())
        {
            QString stringURL = _qurlToQString(url);
            QFileInfo fileInfo(stringURL);
            QDir directory = fileInfo.absoluteDir();
qDebug() << "stringurl" << stringURL;
            // QQmlEngine
            if (_qmlEngine != NULL)
            {
                _qmlEngine->addImportPath(directory.absolutePath());
                qDebug() << "After " << _qmlEngine->importPathList();
            }

            // File system watcher
            // TODO
        }
        else if (QString::compare(url.scheme(), "qrc", Qt::CaseInsensitive) == 0)
        {
            // Embedded file
            // => Nothing to do
            //    We MUST add nothing because its import path is the one of our application
        }
        else
        {
            // Remote file (http, ftp, etc.)

            // TODO: add import paths if needed
        }
    }
    // Else: invalid URL => nothing to do
}




/**
 * @brief Call when our file system watcher detects a file change
 */
void MasticPlaygroundController::_onFileSystemWatcherFileChanged(const QString &path)
{
    Q_UNUSED(path)
}


/**
 * @brief Call when our file system watcher detects a directory change
 */
void MasticPlaygroundController::_onFileSystemWatcherDirectoryChanged(const QString &path)
{
    Q_UNUSED(path)
}

