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


#include "IngescapeApplication.h"

#include <QtGlobal>
#include <QDebug>
#include <QFileOpenEvent>



/**
 * @brief Default constructor
 * @param argc
 * @param argv
 */
IngescapeApplication::IngescapeApplication(int &argc, char **argv)
    : QApplication(argc, argv),
      _hasPendingOpenFileRequest(false),
      _pendingOpenFileRequestUrl(""),
      _pendingOpenFileRequestFilePath("")
{
}


/**
 * @brief Returns a pointer to the application's casted as an IngescapeApplication
 * @return
 */
IngescapeApplication* IngescapeApplication::instance()
{
    return dynamic_cast<IngescapeApplication*>(QApplication::instance());
}


/**
 * @brief Override QApplication::event
 * @param event
 * @return
 */
bool IngescapeApplication::event(QEvent *event)
{
    if (
        (event != nullptr)
        &&
        (event->type() == QEvent::FileOpen)
        )
    {qDebug() << "APPLICATION openFileRequest";
        QFileOpenEvent *fileOpenEvent = dynamic_cast<QFileOpenEvent*>(event);
        if (fileOpenEvent != nullptr)
        {
            sethasPendingOpenFileRequest(true);
            _pendingOpenFileRequestUrl = fileOpenEvent->url();
            _pendingOpenFileRequestFilePath = fileOpenEvent->file();

             Q_EMIT openFileRequest(_pendingOpenFileRequestUrl, _pendingOpenFileRequestFilePath);
        }
        // Else: should not happen

        return true;
    }

    return QApplication::event(event);
}


/**
 * @brief Get our pending "open file" request
 * @return
 */
QPair<QUrl, QString> IngescapeApplication::getPendingOpenFileRequest()
{
    QPair<QUrl, QString> result = QPair<QUrl, QString>(_pendingOpenFileRequestUrl, _pendingOpenFileRequestFilePath);

    sethasPendingOpenFileRequest(false);
    _pendingOpenFileRequestUrl = QUrl();
    _pendingOpenFileRequestFilePath = "";

    return result;
}

