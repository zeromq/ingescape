/*
 *	IngeScape Editor
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
 * @param ipAddress
 * @param parent
 */
AgentM::AgentM(QString name,
               QString peerId,
               QString ipAddress,
               QObject *parent) : QObject(parent),
    _name(name),
    _peerId(peerId),
    _address(ipAddress),
    _hostname(HOSTNAME_NOT_DEFINED),
    _commandLine(""),
    _isON(false),
    _canBeRestarted(false),
    _isMuted(false),
    _canBeFrozen(false),
    _loggerPort(""),
    _isFrozen(false),
    _definition(NULL),
    _mapping(NULL),
    _state(""),
    _hasLogInStream(false),
    _hasLogInFile(false),
    _logFilePath(""),
    _definitionFilePath(""),
    _mappingFilePath("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent" << _name << "(" << _peerId << ") at" << _address;
}


/**
 * @brief Destructor
 */
AgentM::~AgentM()
{
    qInfo() << "Delete Model of Agent" << _name << "(" << _peerId << ") on" << _hostname << "(" << _address << ")";

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
 * @brief Setter for property "is Muted"
 * @param value
 */
void AgentM::setisMuted(bool value)
{
    if (_isMuted != value)
    {
        _isMuted = value;

        if (_definition != NULL) {
            _definition->setisMutedOfAllOutputs(_isMuted);
        }

        Q_EMIT isMutedChanged(value);
    }
}


/**
 * @brief Setter for property "Definition"
 * @param value
 */
void AgentM::setdefinition(DefinitionM *value)
{
    if (_definition != value)
    {
        DefinitionM* previousValue = _definition;

        _definition = value;

        // Emit default signal for QML
        Q_EMIT definitionChanged(value);

        // Emit the signal "Definition Changed" with previous and new values
        Q_EMIT definitionChangedWithPreviousAndNewValues(previousValue, value);
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


/**
 * @brief Clear the data about the network
 */
void AgentM::clearNetworkData()
{
    Q_EMIT networkDataWillBeCleared(_peerId);

    setpeerId("");
    setaddress("");
    sethostname(HOSTNAME_NOT_DEFINED);
    setcommandLine("");
    setisON(false);
    setcanBeRestarted(false);

    // Reset other properties
    setisMuted(false);
    setcanBeFrozen(false);
    setloggerPort("");
    setisFrozen(false);
    setstate("");
    sethasLogInStream(false);
    sethasLogInFile(false);
    setlogFilePath("");
    setdefinitionFilePath("");
    setmappingFilePath("");

    // Keep only the definition and the mapping
    //setdefinition(NULL);
    //setmapping(NULL);
}
