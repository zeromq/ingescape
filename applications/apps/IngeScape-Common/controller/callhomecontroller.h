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

#ifndef CALLHOMECONTROLLER_H
#define CALLHOMECONTROLLER_H

#include <QObject>

#include <I2PropertyHelpers.h>
#include "model/licenseinformationm.h"

class CallHomeController : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Basic constructor
     * @param callHomeRemoteUrl
     * @param parent
     */
    explicit CallHomeController(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    virtual ~CallHomeController();

    /**
     * @brief Method called when the editor is launched to notify our server.
     * The given license information as identification.
     * @param licenseObject
     */
    Q_INVOKABLE void editorLaunched(LicenseInformationM* licenseObject);


private Q_SLOTS:
    /**
     * @brief Slot called when the call home finishes
     * @param reply
     */
    void replyFinished(QNetworkReply *reply);


private:
    /**
     * @brief Access manager handling GET/POST requests to the server
     */
    QNetworkAccessManager* _accessManager;

    /**
     * @brief Url to call to call home
     */
    static const QString URL_TO_CALL_HOME;
};

#endif // CALLHOMECONTROLLER_H
