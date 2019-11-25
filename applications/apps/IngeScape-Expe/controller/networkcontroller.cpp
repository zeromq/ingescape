/*
 *	IngeScape Expe
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "networkcontroller.h"


static const QString prefix_LoadPlatformFile = "LOAD_PLATFORM_FROM_PATH";
static const QString prefix_TimeLineState = "TIMELINE_STATE=";


//--------------------------------------------------------------
//
// Network Controller
//
//--------------------------------------------------------------

/**
 * @brief Constructor
 * @param parent
 */
NetworkController::NetworkController(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Add  header to declare ourselves as expe
    igs_busAddServiceDescription("isExpe", "1");

    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    if (ingeScapeNetworkC != nullptr)
    {
        // We don't see itself
        ingeScapeNetworkC->setnumberOfExpes(1);

        /*connect(ingeScapeNetworkC, SIGNAL(shoutedMessageReceived(QString, QString, QString)),
                this, SLOT(_onShoutedMessageReceived(QString, QString, QString)));
        connect(ingeScapeNetworkC, SIGNAL(shoutedMessageReceived(QString, QString, QString, QStringList)),
                this, SLOT(_onShoutedMessageReceived(QString, QString, QString, QStringList)));*/

        connect(ingeScapeNetworkC, SIGNAL(whisperedMessageReceived(QString, QString, QString)),
                this, SLOT(_onWhisperedMessageReceived(QString, QString, QString)));
        /*connect(ingeScapeNetworkC, SIGNAL(whisperedMessageReceived(QString, QString, QString, QStringList)),
                this, SLOT(_onWhisperedMessageReceived(QString, QString, QString, QStringList)));*/
    }
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    if (ingeScapeNetworkC != nullptr)
    {
        // DIS-connect from the IngeScape Network Controller
        disconnect(ingeScapeNetworkC, nullptr, this, nullptr);
    }
}


/**
 * @brief Slot called when "Whispered" message (with one part) has been received
 * @param peerId
 * @param peerName
 * @param message
 */
void NetworkController::_onWhisperedMessageReceived(QString peerId, QString peerName, QString message)
{
    // Status of command "LOAD PLATFORM FROM PATH"
    if (message.startsWith(prefix_LoadPlatformFile))
    {
        // Starts with the prefix, followed by parameters
        // Ends with white space followed by "STATUS=" and a digit
        QString pattern = QString("^%1=(.*)\\sSTATUS=(\\d)$").arg(prefix_LoadPlatformFile);
        QRegularExpression regExp(pattern);
        QRegularExpressionMatch regExpMatch = regExp.match(message);

        if (regExpMatch.hasMatch())
        {
            //QString matched = regExpMatch.captured(0);
            QString commandParameters = regExpMatch.captured(1);
            QString strStatus = regExpMatch.captured(2);

            bool successConvertStringToInt = false;
            int status = strStatus.toInt(&successConvertStringToInt);

            if (successConvertStringToInt)
            {
                // Emit the signal "Status Received about LoadPlatformFile"
                Q_EMIT statusReceivedAbout_LoadPlatformFile(static_cast<bool>(status), commandParameters);
            }
        }
    }
    // The state of the TimeLine updated (in Editor app)
    else if (message.startsWith(prefix_TimeLineState))
    {
        QString parameters = message.remove(0, prefix_TimeLineState.length());

        Q_EMIT timeLineStateUpdated(parameters);
    }
    // Unknown
    else
    {
        qDebug() << "Not yet managed WHISPERED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
    }
}

