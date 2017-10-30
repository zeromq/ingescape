/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "agentm.h"

#include <QQmlEngine>
#include <QDebug>

/**
 * @brief Constructor without peer id and address
 * @param name
 * @param parent
 */
AgentM::AgentM(QString name,
               QObject *parent) : AgentM(name,
                                         "",
                                         "",
                                         parent)
{
}


/**
 * @brief Constructor with peer id and address
 * @param name
 * @param peerId
 * @param address
 * @param parent
 */
AgentM::AgentM(QString name,
               QString peerId,
               QString address,
               QObject *parent) : QObject(parent),
    _name(name),
    _peerId(peerId),
    _address(address),
    _hostname(""),
    _executionPath(""),
    _pid(0),
    _isON(false),
    _isMuted(false),
    _canBeFrozen(false),
    _isFrozen(false),
    _definition(NULL),
    _mapping(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Init flag indicating if our agent never yet appeared on the network
    _neverAppearedOnNetwork = _peerId.isEmpty();

    qInfo() << "New Model of Agent" << _name << "(" << _peerId << ") at" << _address;
}


/**
 * @brief Destructor
 */
AgentM::~AgentM()
{
    qInfo() << "Delete Model of Agent" << _name << "(" << _peerId << ") at" << _address;

    // Delete our agent definition
    if (_definition != NULL) {
        //disconnect(_definition);

        //DefinitionM* temp = _definition;
        setdefinition(NULL);
        //delete temp;
        //temp = NULL;
    }

    // Delete our agent mapping
    if (_mapping != NULL) {
        //disconnect(_mapping);

        //AgentMappingM* temp = _mapping;
        setmapping(NULL);
        //delete temp;
        //temp = NULL;
    }
}


/**
 * @brief Set the flag "is Muted" of an Output of our agent
 * @param isMuted
 * @param outputName
 */
void AgentM::setisMutedOfOutput(bool isMuted, QString outputName)
{
    if (_definition != NULL) {
        _definition->setisMutedOfOutput(isMuted, outputName);
    }
}
