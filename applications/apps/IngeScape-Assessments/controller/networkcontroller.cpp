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
NetworkController::NetworkController(QObject *parent) : IngeScapeNetworkController(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Add  header to declare ourselves as assessments
    igs_busAddServiceDescription("isAssessments", "1");

    // We don't see itself
    setnumberOfAssessments(1);
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    // Mother class is automatically called
    //IngeScapeNetworkController::~IngeScapeNetworkController();
}


/**
 * @brief Manage a "Shouted" message
 * @param peerId
 * @param peerName
 * @param zMessage
 */
/*void NetworkController::manageShoutedMessage(QString peerId, QString peerName, zmsg_t* zMessage)
{

}*/


/**
 * @brief Manage a "Whispered" message
 * @param peerId
 * @param peerName
 * @param zMessage
 */
void NetworkController::manageWhisperedMessage(QString peerId, QString peerName, zmsg_t* zMessage)
{
    std::unique_ptr<char> zmsg_str(zmsg_popstr(zMessage));
    QString message(zmsg_str.get());

    // An agent DEFINITION has been received
    if (message.startsWith(prefix_Definition))
    {
        QString definitionJSON = message.remove(0, prefix_Definition.length());

        Q_EMIT definitionReceived(peerId, peerName, definitionJSON);
    }
    // An agent MAPPING has been received
    else if (message.startsWith(prefix_Mapping))
    {
        QString mappingJSON = message.remove(0, prefix_Mapping.length());

        Q_EMIT mappingReceived(peerId, peerName, mappingJSON);
    }
    // Unknown
    else
    {
        qDebug() << "Not yet managed WHISPERED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
    }
}
