/*
 *	IngeScape Assessments
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



//--------------------------------------------------------------
//
// NetworkController
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

    // Add  header to declare ourselves as assessments
    igs_busAddServiceDescription("isAssessments", "1");

    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    if (ingeScapeNetworkC != nullptr)
    {
        // We don't see itself
        ingeScapeNetworkC->setnumberOfAssessments(1);

        connect(ingeScapeNetworkC, SIGNAL(shoutedMessageReceived(QString, QString, QString)),
                this, SLOT(_onShoutedMessageReceived(QString, QString, QString)));
        connect(ingeScapeNetworkC, SIGNAL(shoutedMessageReceived(QString, QString, QString, QStringList)),
                this, SLOT(_onShoutedMessageReceived(QString, QString, QString, QStringList)));

        connect(ingeScapeNetworkC, SIGNAL(whisperedMessageReceived(QString, QString, QString)),
                this, SLOT(_onWhisperedMessageReceived(QString, QString, QString)));
        connect(ingeScapeNetworkC, SIGNAL(whisperedMessageReceived(QString, QString, QString, QStringList)),
                this, SLOT(_onWhisperedMessageReceived(QString, QString, QString, QStringList)));
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
 * @brief Slot called when a "Shouted" message (with one part) has been received
 * @param peerId
 * @param peerName
 * @param message
 */
void NetworkController::_onShoutedMessageReceived(QString peerId, QString peerName, QString message)
{
    qDebug() << "Not yet managed SHOUTED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
}


/**
 * @brief Slot called when a "Shouted" message (with several parts) has been received
 * @param peerId
 * @param peerName
 * @param messagePart1
 * @param messageOthersParts
 */
void NetworkController::_onShoutedMessageReceived(QString peerId, QString peerName, QString messagePart1, QStringList messageOthersParts)
{
    qDebug() << "Not yet managed SHOUTED message '" << messagePart1 << "+" << messageOthersParts << "' for agent" << peerName << "(" << peerId << ")";
}


/**
 * @brief Slot called when "Whispered" message (with one part) has been received
 * @param peerId
 * @param peerName
 * @param message
 */
void NetworkController::_onWhisperedMessageReceived(QString peerId, QString peerName, QString message)
{

    /// The "Recorder app" Started to record
    if (message.startsWith(prefix_RecordStarted))
    {
        qInfo() << prefix_RecordStarted;

        Q_EMIT recordStartedReceived(peerId);
    }
    else if (message.startsWith(prefix_RecordStopped))
    {
        qInfo() << prefix_RecordStopped;

        Q_EMIT recordStoppedReceived(peerId);
    }
    else if (message.startsWith(prefix_AddedRecord))
    {
        message.remove(0, prefix_AddedRecord.length());

        Q_EMIT addedRecordReceived(message);
    }
    else if (message.startsWith(prefix_DeletedRecord))
    {
        message.remove(0, prefix_DeletedRecord.length());

        Q_EMIT deletedRecordReceived(message);
    }
    else if (message.startsWith(prefix_RecordExported))
    {
        Q_EMIT exportedRecordReceived();
    }
    else {
        qDebug() << "Not yet managed WHISPERED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
    }
}



/**
 * @brief Slot called when "Whispered" message (with several parts) has been received
 * @param peerId
 * @param peerName
 * @param messagePart1
 * @param messageOthersParts
 */
void NetworkController::_onWhisperedMessageReceived(QString peerId, QString peerName, QString messagePart1, QStringList messageOthersParts)
{
    qDebug() << "Not yet managed WHISPERED message '" << messagePart1 << "+" << messageOthersParts << "' for agent" << peerName << "(" << peerId << ")";
}


