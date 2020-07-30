/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
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
 * @brief Constructor
 * @param name
 * @param peerId
 * @param ipAddress
 * @param hostname
 * @param commandLine
 * @param parent
 */
AgentM::AgentM(QString name,
               QString peerId,
               QString ipAddress,
               QString hostname,
               QString commandLine,
               bool isON,
               QObject *parent) : QObject(parent),
    _name(name),
    _peerId(peerId),
    _address(ipAddress),
    _hostname(hostname),
    _commandLine(commandLine),
    _isON(isON),
    _canBeRestarted(false),
    _isMuted(false),
    _isFrozen(false),
    _definition(nullptr),
    _mapping(nullptr),
    _state(""),
    _loggerPort(""),
    _hasLogInStream(false),
    _hasLogInFile(false),
    _logFilePath(""),
    _definitionFilePath(""),
    _mappingFilePath("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent" << _name << "(" << _peerId << ") on" << _hostname << "(" << _address << ") and command" << _commandLine;
}


/**
 * @brief Destructor
 */
AgentM::~AgentM()
{
    qInfo() << "Delete Model of Agent" << _name << "(" << _peerId << ") on" << _hostname << "(" << _address << ") and command" << _commandLine;

    // Delete our agent definition
    if (_definition != nullptr) {
        //disconnect(_definition);

        DefinitionM* temp = _definition;
        setdefinition(nullptr);
        delete temp;
    }

    // Delete our agent mapping
    if (_mapping != nullptr) {
        //disconnect(_mapping);

        AgentMappingM* temp = _mapping;
        setmapping(nullptr);
        delete temp;
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

        if (_definition != nullptr) {
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

        if (previousValue != nullptr) {
            disconnect(previousValue, &DefinitionM::isMutedOutputChanged, this, &AgentM::isMutedOutputChanged);
        }

        _definition = value;

        if (_definition != nullptr) {
            connect(_definition, &DefinitionM::isMutedOutputChanged, this, &AgentM::isMutedOutputChanged);
        }

        // Emit the default signal for QML
        Q_EMIT definitionChanged(value);

        // Emit the signal "Definition changed" with previous and new values
        Q_EMIT definitionChangedWithPreviousAndNewValues(previousValue, value);
    }
}


/**
 * @brief Setter for property "Mapping"
 * @param value
 */
void AgentM::setmapping(AgentMappingM *value)
{
    if (_mapping != value)
    {
        AgentMappingM* previousValue = _mapping;

        _mapping = value;

        // Emit the default signal for QML
        Q_EMIT mappingChanged(value);

        // Emit the signal "Mapping changed" with previous and new values
        Q_EMIT mappingChangedWithPreviousAndNewValues(previousValue, value);
    }
}


/**
 * @brief Set the flag "is Muted" of an Output of our agent
 * @param isMuted
 * @param outputName
 */
void AgentM::setisMutedOfOutput(bool isMuted, QString outputName)
{
    if (_definition != nullptr) {
        _definition->setisMutedOutput(isMuted, outputName);
    }
}


/**
 * @brief Clear the data about the network of our agent
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
    setisFrozen(false);
    setstate("");
    setloggerPort("");
    sethasLogInStream(false);
    sethasLogInFile(false);
    setlogFilePath("");
    setdefinitionFilePath("");
    setmappingFilePath("");

    // Keep only the definition and the mapping
    //setdefinition(nullptr);
    //setmapping(nullptr);
}
