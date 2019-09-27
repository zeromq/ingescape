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

#include "I2PropertyHelpers.h"


/**
 * @brief The IngescapeApplication class defines a custom QApplication that supports file associations
 */
class IngescapeApplication : public QApplication
{
    Q_OBJECT

    // Flag indicating if we have a pending "open file" request
    I2_QML_PROPERTY_READONLY(bool, hasPendingOpenFileRequest)

public:
    /**
     * @brief Default constructor
     * @param argc
     * @param argv
     */
    explicit IngescapeApplication(int &argc, char **argv);


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


Q_SIGNALS:
    /**
     * @brief Triggered when our application receives an "open file" request
     * @param url
     * @param filePath
     */
    void openFileRequest(QUrl fileUrl, QString filePath);


protected:
    /**
     * @brief Override QApplication::event
     * @param event
     * @return
     */
    bool event(QEvent *event) Q_DECL_OVERRIDE;


protected:
    // Pending "open file" request
    QUrl _pendingOpenFileRequestUrl;
    QString _pendingOpenFileRequestFilePath;
};

#endif // INGESCAPEAPPLICATION_H
