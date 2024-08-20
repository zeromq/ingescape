/*  =========================================================================
Enum.cs

Copyright (c) the Contributors as noted in the AUTHORS file.
This file is part of Ingescape, see https://github.com/zeromq/ingescape.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
=========================================================================
*/


namespace Ingescape
{
    public enum IopType
    {
        Input = 1,
        Output,
        Parameter
    };

    public enum IopValueType
    {
        Integer = 1,
        Double,
        String,
        Bool,
        Impulsion,
        Data,
        Unknown
    };

    /// <summary>
    /// <inheritdoc cref="Igs.Log"/>
    /// </summary>
    public enum LogLevel
    {
        LogTrace = 0,
        LogDebug,
        LogInfo,
        LogWarn,
        LogError,
        LogFatal
    };

    public enum Result
    {
        Failure = -1,
        Success = 0
    };

    /// <summary>
    /// IGS_PEER_ENTERED: a new peer has entered the network<br />
    /// IGS_PEER_EXITED: a peer has left the network(clean exit or timeout)<br />
    /// IGS_AGENT_ENTERED: an agent carried by a peer has entered the network<br />
    /// IGS_AGENT_UPDATED_DEFINITION: an agent has updated its definition<br />
    /// IGS_AGENT_KNOWS_US: an agent is fully ready to communicate with us(services can be called)<br />
    /// IGS_AGENT_EXITED: an agent has left the netork(peer has left or agent has been deactivated)<br />
    /// IGS_AGENT_UPDATED_MAPPING: an agent has updated its mapping<br />
    /// IGS_AGENT_WON_ELECTION: THIS AGENT has won an election(via its peer)<br />
    /// IGS_AGENT_LOST_ELECTION: THIS AGENT has lost an election(via its peer)<br />
    /// </summary>
    public enum AgentEvent
    {
        PeerEntered = 1,
        PeerExited,
        AgentEntered,
        AgentUpdatedDefinition,
        AgentKnowsUs,
        AgentExited,
        AgentUpdatedMapping,
        AgentWonElection,
        AgentLostElection
    };

    public enum MonitorEvent
    {
        NetworkOk = 1,
        NetworkDeviceNotAvailable,
        NetworkAddressChanged,
        NetworkOkAfterManualRestart
    }

    /// <summary>
    /// parse a JSON string or file based on parsing events and a callback
    /// </summary>
    public enum JsonValueType
    {
        JsonString = 1,
        JsonNumber, //int or double
        JsonMap,
        JsonArray,
        JsonTrue, //not used in parsing callback
        JsonFalse, //not used in parsing callback
        JsonNull,
        JsonKey = 9, //not used in tree queries
        JsonMapEnd, //not used in tree queries
        JsonArrayEnd, //not used in tree queries
        JsonBool //not used in tree queries
    }
}
