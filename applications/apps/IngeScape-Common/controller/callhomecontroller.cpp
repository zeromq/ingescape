/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum     <soum@ingenuity.io>
 *
 */

#include "callhomecontroller.h"

/**
 * @brief Url to call to call home
 */
const QString CallHomeController::URL_TO_CALL_HOME = "https://services.ingescape.com/editor/stats";

/**
 * @brief Basic constructor
 * @param callHomeRemoteUrl
 * @param parent
 */
CallHomeController::CallHomeController(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    _accessManager = new QNetworkAccessManager();
    connect(_accessManager, &QNetworkAccessManager::finished,
            this, &CallHomeController::replyFinished);
}

/**
 * @brief Destructor
 */
CallHomeController::~CallHomeController()
{
    if (_accessManager != nullptr)
    {
        disconnect(_accessManager, nullptr, nullptr, nullptr);
        _accessManager->deleteLater();
    }
}

/**
 * @brief Method called when the editor is launched to notify our server.
 * The given license information as identification.
 */
void CallHomeController::editorLaunched(LicenseInformationM* license)
{
    if ((_accessManager != nullptr) && (license != nullptr))
    {
        // Build JSON Body
        QByteArray jsonString = "{\"user_id\":\"";
        jsonString.append(license->licenseId());
        jsonString.append("\"}");

        // Prepare request
        QNetworkRequest request(QUrl(this->URL_TO_CALL_HOME));

        // Add the headers
        request.setRawHeader("Content-Type", "application/json");
        QByteArray postDataSize = QByteArray::number(jsonString.size());
        request.setRawHeader("Content-Length", postDataSize);

        _accessManager->post(request, jsonString);
    }
}

/**
 * @brief Slot called when the call home finishes
 * @param reply
 */
void CallHomeController::replyFinished(QNetworkReply* reply)
{
    // Do nothing if failed
    qDebug() << "Reply arrived with error code:" << reply->error();
}
