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
 * @brief Manage a "Shouted" message
 * @param peerId
 * @param peerName
 * @param zMessage
 */
void NetworkController::_onShoutedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage)
{
    QString message = zmsg_popstr(zMessage);

    qDebug() << "Not yet managed SHOUTED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
}


/**
 * @brief Manage a "Whispered" message
 * @param peerId
 * @param peerName
 * @param zMessage
 */
void NetworkController::_onWhisperedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage)
{
    QString message = zmsg_popstr(zMessage);

    qDebug() << "Not yet managed WHISPERED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
}

