/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */


#ifndef INGESCAPEAPPLICATION_H
#define INGESCAPEAPPLICATION_H

#include <QObject>
#include <QApplication>
#include <QUrl>
#include <QScopedPointer>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQuickWindow>

#include "I2PropertyHelpers.h"



/**
 * @brief The IngescapeApplicationPrivate class defines the private API of IngescapeApplicationPrivate
 */
class IngescapeApplicationPrivate;



/**
 * @brief The IngescapeApplication class defines a custom QApplication that supports file associations
 */
class IngescapeApplication : public QApplication
{
    Q_OBJECT

    // Flag indicating if we have a pending "open file" request
    I2_QML_PROPERTY_READONLY(bool, hasPendingOpenFileRequest)

    // Current window
    Q_PROPERTY(QQuickWindow* currentWindow READ currentWindow WRITE setcurrentWindow NOTIFY currentWindowChanged)


public:
    /**
     * @brief Default constructor
     * @param argc
     * @param argv
     */
    explicit IngescapeApplication(int &argc, char **argv);


    /**
     * @brief Destructor
     */
    ~IngescapeApplication();


    /**
    * @brief get our current window
    * @return
    */
    QQuickWindow* currentWindow() const;


    /**
     * @brief Set our current window
     * @param value
     * @return
     */
    bool setcurrentWindow (QQuickWindow* value);


    /**
     * @brief Returns a pointer to the application's casted as an IngescapeApplication
     * @return
     */
    static IngescapeApplication* instance();


    /**
     * @brief Get our pending "open file" request
     * @return
     */
    QPair<QUrl, QString> getPendingOpenFileRequest();


    /**
     * @brief Add a pending "open file" request
     *
     * @param url
     * @param filePath
     */
    void addPendingOpenFileRequest(QUrl url, QString filePath);


    /**
     * @brief Register a file type
     *
     * @param documentId    e.g. ApplicationName.myextension
     * @param fileTypeName  e.g. My file type
     * @param fileExtension e.g. .myextension
     * @param appIconIndex
     */
    void registerFileType(const QString &documentId, const QString &fileTypeName, const QString &fileExtension, qint32 appIconIndex = 0);


Q_SIGNALS:
    /**
     * @brief Triggered when our currentWindow property has changed
     * @param value
     */
    void currentWindowChanged(QQuickWindow* value);


    /**
     * @brief Triggered when our application receives an "open file" request
     * @param url
     * @param filePath
     */
    void openFileRequest(QUrl fileUrl, QString filePath);


private Q_SLOTS:
    /**
    * @brief Called when our current window is destroyed
    */
   void _oncurrentWindowDestroyed(QObject*);



protected:
    /**
     * @brief Override QApplication::event
     * @param event
     * @return
     */
    bool event(QEvent *event) Q_DECL_OVERRIDE;


protected:
    /**
     * @brief Subscribe to our current window
     */
    void _subscribeToCurrentWindow(QQuickWindow* window);


    /**
     * @brief Unsubscribe to our current Window
     */
    void _unsubscribeToCurrentWindow(QQuickWindow* window);


private:
    // Current window
    QQuickWindow* _currentWindow;

    // Pending "open file" request
    QUrl _pendingOpenFileRequestUrl;
    QString _pendingOpenFileRequestFilePath;

    // Private API
    QScopedPointer<IngescapeApplicationPrivate> _privateAPI;
};

#endif // INGESCAPEAPPLICATION_H
