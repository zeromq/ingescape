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

        connect(ingeScapeNetworkC, &IngeScapeNetworkController::shoutedMessageReceived,
                this, &NetworkController::_onShoutedMessageReceived);
        connect(ingeScapeNetworkC, &IngeScapeNetworkController::whisperedMessageReceived,
                this, &NetworkController::_onWhisperedMessageReceived);
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
 * @brief Slot called when a "Shouted" message has been received
 */
void NetworkController::_onShoutedMessageReceived(PeerM* peer, QString messageType, QStringList messageParameters)
{
    if (peer != nullptr)
    {
        QString peerId = peer->uid();

        qWarning() << "Not yet managed SHOUTED message '" << messageType << "' with parameters" << messageParameters << "from peer" << peer->name() << "(" << peer->uid() << ")";
    }
}


/**
 * @brief Slot called when a "Whispered" message has been received
 */
void NetworkController::_onWhisperedMessageReceived(PeerM* peer, QString messageType, QStringList messageParameters)
{
    if (peer != nullptr)
    {
        QString peerId = peer->uid();

        // The "Recorder app" Started to record
        if (messageType.startsWith(prefix_RecordStarted))
        {
            qInfo() << prefix_RecordStarted;

            Q_EMIT recordStartedReceived(peerId);
        }
        else if (messageType.startsWith(prefix_RecordStopped))
        {
            qInfo() << prefix_RecordStopped;

            Q_EMIT recordStoppedReceived(peerId);
        }
        else if (messageType.startsWith(prefix_AddedRecord))
        {
            messageType.remove(0, prefix_AddedRecord.length());

            Q_EMIT addedRecordReceived(messageType);
        }
        else if (messageType.startsWith(prefix_DeletedRecord))
        {
            messageType.remove(0, prefix_DeletedRecord.length());

            Q_EMIT deletedRecordReceived(messageType);
        }
        else if (messageType.startsWith(prefix_RecordExported))
        {
            Q_EMIT exportedRecordReceived();
        }
        else
        {
            qWarning() << "Not yet managed WHISPERED message '" << messageType << "' with parameters" << messageParameters << "from peer" << peer->name() << "(" << peerId << ")";
        }
    }
}

