/*  =========================================================================
IgsAgent.cs

Copyright (c) the Contributors as noted in the AUTHORS file.
This file is part of Ingescape, see https://github.com/zeromq/ingescape.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
=========================================================================
*/

using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Ingescape
{
    public class Agent : IDisposable
    {
        internal IntPtr _pAgent;
        private string _defaultAgentName = "NewAgent";

        #region Agent logging
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_log(LogLevel logLevel, IntPtr function, IntPtr agent, IntPtr message);
        public void Trace(string message, [CallerMemberName] string memberName = "")
        {
            if (_pAgent == IntPtr.Zero)
                throw new NullReferenceException("Agent pointer is null");

            IntPtr memberNameAsPtr = Igs.StringToUTF8Ptr(memberName);
            IntPtr messageAsPtr = Igs.StringToUTF8Ptr(message);
            igsagent_log(LogLevel.LogTrace, memberNameAsPtr, _pAgent, messageAsPtr);
            Marshal.FreeHGlobal(memberNameAsPtr);
            Marshal.FreeHGlobal(messageAsPtr);
        }
        public void Debug(string message, [CallerMemberName] string memberName = "")
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");

            IntPtr memberNameAsPtr = Igs.StringToUTF8Ptr(memberName);
            IntPtr messageAsPtr = Igs.StringToUTF8Ptr(message);
            igsagent_log(LogLevel.LogDebug, memberNameAsPtr, _pAgent, messageAsPtr);
            Marshal.FreeHGlobal(memberNameAsPtr);
            Marshal.FreeHGlobal(messageAsPtr);
        }
        public void Info(string message, [CallerMemberName] string memberName = "")
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");

            IntPtr memberNameAsPtr = Igs.StringToUTF8Ptr(memberName);
            IntPtr messageAsPtr = Igs.StringToUTF8Ptr(message);
            igsagent_log(LogLevel.LogInfo, memberNameAsPtr, _pAgent, messageAsPtr);
            Marshal.FreeHGlobal(memberNameAsPtr);
            Marshal.FreeHGlobal(messageAsPtr);

        }
        public void Warn(string message, [CallerMemberName] string memberName = "")
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");

            IntPtr memberNameAsPtr = Igs.StringToUTF8Ptr(memberName);
            IntPtr messageAsPtr = Igs.StringToUTF8Ptr(message);
            igsagent_log(LogLevel.LogWarn, memberNameAsPtr, _pAgent, messageAsPtr);
            Marshal.FreeHGlobal(memberNameAsPtr);
            Marshal.FreeHGlobal(messageAsPtr);

        }
        public void Error(string message, [CallerMemberName] string memberName = "")
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");

            IntPtr memberNameAsPtr = Igs.StringToUTF8Ptr(memberName);
            IntPtr messageAsPtr = Igs.StringToUTF8Ptr(message);
            igsagent_log(LogLevel.LogError, memberNameAsPtr, _pAgent, messageAsPtr);
            Marshal.FreeHGlobal(memberNameAsPtr);
            Marshal.FreeHGlobal(messageAsPtr);

        }
        public void Fatal(string message, [CallerMemberName] string memberName = "")
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");

            IntPtr memberNameAsPtr = Igs.StringToUTF8Ptr(memberName);
            IntPtr messageAsPtr = Igs.StringToUTF8Ptr(message);
            igsagent_log(LogLevel.LogFatal, memberNameAsPtr, _pAgent, messageAsPtr);
            Marshal.FreeHGlobal(memberNameAsPtr);
            Marshal.FreeHGlobal(messageAsPtr);

        }
        #endregion

        #region Agent creation/destruction/activationd
        public Agent()
        {
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(_defaultAgentName);
            _pAgent = igsagent_new(nameAsPtr, false);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_new(IntPtr name, bool activateImmediately);
        public Agent(string name, bool activateImmediately)
        {
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            _pAgent = igsagent_new(nameAsPtr, activateImmediately);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        ~Agent()
        {
            Dispose();
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_destroy(ref IntPtr agent);
        public void Destroy()
        {
            Dispose();
        }

        public void Dispose()
        {
            Dispose(disposing: true);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                igsagent_destroy(ref _pAgent);
                Marshal.FreeHGlobal(_pAgent);
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_activate(IntPtr agent);
        public Result Activate()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_activate(_pAgent);

        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_deactivate(IntPtr agent);
        public Result Deactivate()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_deactivate(_pAgent);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_is_activated(IntPtr agent);
        public bool IsActivated
        {
            get
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                return igsagent_is_activated(_pAgent);
            }
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void AgentFunctionC(IntPtr agent, bool isActivated, IntPtr myData);
        public delegate void AgentFunction(Agent Agent, bool isActivated, object myData);
        private AgentFunctionC _OnActivateCallback;
        private void OnActivateCallback(IntPtr agent, bool isActivated, IntPtr myData)
        {
            Tuple<AgentFunction, object> tupleData = (Tuple<AgentFunction, object>)GCHandle.FromIntPtr(myData).Target;
            AgentFunction cSharpFunction = tupleData.Item1;
            object data = tupleData.Item2;
            cSharpFunction(this, isActivated, data);
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_observe(IntPtr agent, AgentFunctionC cb, IntPtr myData);
        public void ObserveActivate(AgentFunction callback, object myData)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Tuple<AgentFunction, object> tupleData = new Tuple<AgentFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnActivateCallback == null)
                _OnActivateCallback = OnActivateCallback;
            
            igsagent_observe(_pAgent, _OnActivateCallback, data);
        }
        #endregion

        #region Agent initialization, control and events
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_set_name(IntPtr agent, IntPtr name);
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_name(IntPtr agent);
        public string Name
        {
            get
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                return Igs.PtrToStringFromUTF8(igsagent_name(_pAgent));
            }

            set
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                IntPtr nameAsPtr = Igs.StringToUTF8Ptr(value);
                igsagent_set_name(_pAgent, nameAsPtr);
                Marshal.FreeHGlobal(nameAsPtr);
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_set_family(IntPtr agent, IntPtr name);
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_family(IntPtr agent);
        public string Family
        {
            get
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                return Igs.PtrToStringFromUTF8(igsagent_family(_pAgent));
            }

            set
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                IntPtr nameAsPtr = Igs.StringToUTF8Ptr(value);
                igsagent_set_family(_pAgent, nameAsPtr);
                Marshal.FreeHGlobal(nameAsPtr);
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_uuid(IntPtr agent);
        public string UUID
        {
            get
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                else
                    return Igs.PtrToStringFromUTF8(igsagent_uuid(_pAgent));
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_set_state(IntPtr agent, IntPtr state);
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_state(IntPtr agent);
        public string State
        {
            get
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                else
                    return Igs.PtrToStringFromUTF8(igsagent_state(_pAgent));
            }
            set
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                
                IntPtr stateAsPtr = Igs.StringToUTF8Ptr(value);
                igsagent_set_state(_pAgent, stateAsPtr);
                Marshal.FreeHGlobal(stateAsPtr);
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_mute(IntPtr agent);
        public void Mute()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            igsagent_mute(_pAgent);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_unmute(IntPtr agent);
        public void Unmute()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            igsagent_unmute(_pAgent);
        }


        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_is_muted(IntPtr agent);
        public bool IsMuted
        {
            get
            {
                if (_pAgent == IntPtr.Zero)
                    throw new NullReferenceException("Agent pointer is null");                
                return igsagent_is_muted(_pAgent);
            }
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void MuteFunctionC(IntPtr agent, bool AgentIsMuted, IntPtr myData);
        public delegate void MuteFunction(Agent agent, bool AgentIsMuted, object myData);
        private MuteFunctionC _OnMuteCallback;
        private void OnMuteCallback(IntPtr agent, bool AgentIsMuted, IntPtr myData)
        {
            Tuple<MuteFunction, object> tupleData = (Tuple<MuteFunction, object>)GCHandle.FromIntPtr(myData).Target;
            MuteFunction cSharpFunction = tupleData.Item1;
            object data = tupleData.Item2;
            cSharpFunction(this, AgentIsMuted, data);
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_observe_mute(IntPtr agent, MuteFunctionC cb, IntPtr myData);
        public void ObserveMute(MuteFunction callback, object myData)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Tuple<Agent.MuteFunction, object> tupleData = new Tuple<Agent.MuteFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnMuteCallback == null)
                _OnMuteCallback = OnMuteCallback;
            
            igsagent_observe_mute(_pAgent, OnMuteCallback, data);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void AgentEventsFunctionC(IntPtr agent, AgentEvent agentEvent, IntPtr uuid, IntPtr name, IntPtr eventData, IntPtr myData);
        public delegate void AgentEventsFunction(Agent agent, AgentEvent agentEvent, string uuid, string name, object eventData, object myData);
        private AgentEventsFunctionC _OnAgentEventCallback;
        private void OnAgentEventCallback(IntPtr agent, AgentEvent agentEvent, IntPtr uuid, IntPtr name, IntPtr eventData, IntPtr myData)
        {
            Tuple<AgentEventsFunction, object> tupleData = (Tuple<AgentEventsFunction, object>)GCHandle.FromIntPtr(myData).Target;
            AgentEventsFunction cSharpFunction = tupleData.Item1;
            object data = tupleData.Item2;
            object eventDataAsObject = null;
            if (eventData != IntPtr.Zero && agentEvent != AgentEvent.PeerEntered)
                eventDataAsObject = Igs.PtrToStringFromUTF8(eventData);

            cSharpFunction(this, agentEvent, Igs.PtrToStringFromUTF8(uuid), Igs.PtrToStringFromUTF8(name), eventDataAsObject, data);
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_observe_agent_events(IntPtr Agent, AgentEventsFunctionC cb, IntPtr myData);
        public void ObserveAgentEvents(AgentEventsFunction callback, object myData)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Tuple<Agent.AgentEventsFunction, object> tupleData = new Tuple<Agent.AgentEventsFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnAgentEventCallback == null)
                _OnAgentEventCallback = OnAgentEventCallback;
            
            igsagent_observe_agent_events(_pAgent, _OnAgentEventCallback, data);
        }
        #endregion

        #region Editing & inspecting definitions, adding and removing inputs/outputs/parameters
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_definition_load_str(IntPtr agent, IntPtr json_str);
        public Result DefinitionLoadStr(string json)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr jsonAsPtr = Igs.StringToUTF8Ptr(json);
            Result res = igsagent_definition_load_str(_pAgent, jsonAsPtr);
            Marshal.FreeHGlobal(jsonAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_definition_load_file(IntPtr agent, IntPtr file_path);
        public Result DefinitionLoadFile(string file_path)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr pathAsPtr = Igs.StringToUTF8Ptr(file_path);
            Result res = igsagent_definition_load_file(_pAgent, pathAsPtr);
            Marshal.FreeHGlobal(pathAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_clear_definition(IntPtr agent);
        public void ClearDefinition()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            igsagent_clear_definition(_pAgent);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_definition_json(IntPtr agent);
        public string DefinitionJson()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return Igs.PtrToStringFromUTF8(igsagent_definition_json(_pAgent));
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_definition_description(IntPtr agent);
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_definition_set_description(IntPtr agent, IntPtr description);
        public string DefinitionDescription
        {
            get
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                else
                    return Igs.PtrToStringFromUTF8(igsagent_definition_description(_pAgent));
            }

            set
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                IntPtr descriptionAsPtr = Igs.StringToUTF8Ptr(value);
                igsagent_definition_set_description(_pAgent, descriptionAsPtr);
                Marshal.FreeHGlobal(descriptionAsPtr);
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_definition_version(IntPtr agent);
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_definition_set_version(IntPtr agent, IntPtr Version);
        public string DefinitionVersion
        {
            get
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                else
                    return Igs.PtrToStringFromUTF8(igsagent_definition_version(_pAgent));
            }

            set
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                IntPtr versionAsPtr = Igs.StringToUTF8Ptr(value);
                igsagent_definition_set_version(_pAgent, Igs.StringToUTF8Ptr(value));
                Marshal.FreeHGlobal(versionAsPtr);
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_create(IntPtr agent, IntPtr name, IopValueType value_type, IntPtr value, uint size);
        public Result InputCreate(string name, IopValueType type, object value = null)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            if (value != null)
            {
                uint size;
                IntPtr valuePtr = Igs.ObjectToPtr(value, out size);
                if (valuePtr == null)
                {
                    Igs.Error(string.Format("Invalid value type for the input {0}", name));
                    return Result.Failure;
                }

                Result res = igsagent_input_create(_pAgent, nameAsPtr, type, valuePtr, size);
                Marshal.FreeHGlobal(nameAsPtr);
                Marshal.FreeHGlobal(valuePtr);
                return res;
            }
            else
            {
                Result res = igsagent_input_create(_pAgent, nameAsPtr, type, IntPtr.Zero, 0);
                Marshal.FreeHGlobal(nameAsPtr);
                return res;
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_create(IntPtr agent, IntPtr name, IopValueType type, IntPtr value, uint size);
        public Result OutputCreate(string name, IopValueType type, object value = null)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            if (value != null)
            {
                uint size;
                IntPtr valuePtr = Igs.ObjectToPtr(value, out size);
                if (valuePtr == null)
                {
                    Igs.Error(string.Format("Invalid value type for the output {0}", name));
                    return Result.Failure;
                }

                Result res = igsagent_output_create(_pAgent, nameAsPtr, type, valuePtr, size);
                Marshal.FreeHGlobal(nameAsPtr);
                Marshal.FreeHGlobal(valuePtr);
                return res;
            }
            else
            {
                Result res = igsagent_output_create(_pAgent, nameAsPtr, type, IntPtr.Zero, 0);
                Marshal.FreeHGlobal(nameAsPtr);
                return res;
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_create(IntPtr agent, IntPtr name, IopValueType type, IntPtr value, uint size);
        public Result ParameterCreate(string name, IopValueType type, object value = null)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            if (value != null)
            {
                uint size;
                IntPtr valuePtr = Igs.ObjectToPtr(value, out size);
                if (valuePtr == null)
                {
                    Igs.Error(string.Format("Invalid value type for the parameter {0}", name));
                    return Result.Failure;
                }

                Result res = igsagent_parameter_create(_pAgent, nameAsPtr, type, valuePtr, size);
                Marshal.FreeHGlobal(nameAsPtr);
                Marshal.FreeHGlobal(valuePtr);
                return res;
            }
            else
            {
                Result res = igsagent_parameter_create(_pAgent, nameAsPtr, type, IntPtr.Zero, 0);
                Marshal.FreeHGlobal(nameAsPtr);
                return res;
            }
        }


        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_remove(IntPtr agent, IntPtr name);
        public Result InputRemove(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            Result res = igsagent_input_remove(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_remove(IntPtr agent, IntPtr name);
        public Result OutputRemove(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            Result res = igsagent_output_remove(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_remove(IntPtr agent, IntPtr name);
        public Result ParameterRemove(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            Result res = igsagent_parameter_remove(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IopValueType igsagent_input_type(IntPtr agent, IntPtr name);
        public IopValueType InputType(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IopValueType res = igsagent_input_type(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IopValueType igsagent_output_type(IntPtr agent, IntPtr name);
        public IopValueType OutputType(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IopValueType res = igsagent_output_type(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IopValueType igsagent_parameter_type(IntPtr agent, IntPtr name);
        public IopValueType ParameterType(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IopValueType res = igsagent_parameter_type(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_input_count(IntPtr agent);
        public uint InputCount()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_input_count(_pAgent);
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_output_count(IntPtr agent);
        public uint OutputCount()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_output_count(_pAgent);
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_parameter_count(IntPtr agent);
        public uint ParameterCount()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_parameter_count(_pAgent);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_input_list(IntPtr agent, ref uint nbOfElements);
        public string[] InputList()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            uint nb = 0;
            IntPtr pInputList = igsagent_input_list(_pAgent, ref nb);
            IntPtr[] pArrayinputList = new IntPtr[nb];
            string[] inputList = new string[nb];
            Marshal.Copy(pInputList, pArrayinputList, 0, (int)nb);
            for (int i = 0; i < nb; i++)
                inputList[i] = Marshal.PtrToStringAnsi(pArrayinputList[i]);
            
            Igs.igs_free_iop_list(pInputList, (int)nb);
            return inputList;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_output_list(IntPtr agent, ref uint nbOfElements);
        public string[] OutputList()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            uint nb = 0;
            IntPtr pInputList = igsagent_output_list(_pAgent, ref nb);
            IntPtr[] pArrayinputList = new IntPtr[nb];
            string[] inputList = new string[nb];
            Marshal.Copy(pInputList, pArrayinputList, 0, (int)nb);
            for (int i = 0; i < nb; i++)
                inputList[i] = Marshal.PtrToStringAnsi(pArrayinputList[i]);
            
            Igs.igs_free_iop_list(pInputList, (int)nb);
            return inputList;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_parameter_list(IntPtr agent, ref uint nbOfElements);
        public string[] ParameterList()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            uint nb = 0;
            IntPtr pInputList = igsagent_parameter_list(_pAgent, ref nb);
            IntPtr[] pArrayinputList = new IntPtr[nb];
            string[] inputList = new string[nb];
            Marshal.Copy(pInputList, pArrayinputList, 0, (int)nb);
            for (int i = 0; i < nb; i++)
                inputList[i] = Marshal.PtrToStringAnsi(pArrayinputList[i]);
            
            Igs.igs_free_iop_list(pInputList, (int)nb);
            return inputList;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_input_exists(IntPtr agent, IntPtr name);
        public bool InputExists(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            bool res = igsagent_input_exists(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_output_exists(IntPtr agent, IntPtr name);
        public bool OutputExists(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            bool res = igsagent_output_exists(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_parameter_exists(IntPtr agent, IntPtr name);
        public bool ParameterExists(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            bool res = igsagent_parameter_exists(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        #endregion

        #region Reading and writing inputs/outputs/parameters, a.k.a IOPs
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_input_bool(IntPtr agent, IntPtr name);
        public bool InputBool(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            bool res = igsagent_input_bool(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igsagent_input_int(IntPtr agent, IntPtr name);
        public int InputInt(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            int res = igsagent_input_int(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igsagent_input_double(IntPtr agent, IntPtr name);
        public double InputDouble(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            double res = igsagent_input_int(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_input_string(IntPtr agent, IntPtr name);
        public string InputString(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            string res = Igs.PtrToStringFromUTF8(igsagent_input_string(_pAgent, nameAsPtr));
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_data(IntPtr agent, IntPtr name, ref IntPtr data, ref uint size);
        public byte[] InputData(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            uint size = 0;
            byte[] data = null;
            IntPtr ptr = IntPtr.Zero;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            Result result = igsagent_input_data(_pAgent, nameAsPtr, ref ptr, ref size);
            Marshal.FreeHGlobal(nameAsPtr);
            if (result == Result.Success)
            {
                data = new byte[size];
                if (ptr != IntPtr.Zero)
                    Marshal.Copy(ptr, data, 0, (int)size);
            }
            return data;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_output_bool(IntPtr agent, IntPtr name);
        public bool OutputBool(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            bool res = igsagent_output_bool(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igsagent_output_int(IntPtr agent, IntPtr name);
        public int OutputInt(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            int res = igsagent_output_int(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igsagent_output_double(IntPtr agent, IntPtr name);
        public double OutputDouble(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            double res = igsagent_output_double(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_output_string(IntPtr agent, IntPtr name);
        public string OutputString(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            string res = Igs.PtrToStringFromUTF8(igsagent_output_string(_pAgent, nameAsPtr));
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_data(IntPtr agent, IntPtr name, ref IntPtr data, ref uint size);
        public byte[] OutputData(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            uint size = 0;
            byte[] data = null;
            IntPtr ptr = IntPtr.Zero;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            Result result = igsagent_output_data(_pAgent, nameAsPtr, ref ptr, ref size);
            Marshal.FreeHGlobal(nameAsPtr);
            if (result == Result.Success)
            {
                data = new byte[size];
                if (ptr != IntPtr.Zero)
                    Marshal.Copy(ptr, data, 0, (int)size);
            }
            return data;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_parameter_bool(IntPtr agent, IntPtr name);
        public bool ParameterBool(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            bool res = igsagent_parameter_bool(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igsagent_parameter_int(IntPtr agent, IntPtr name);
        public int ParameterInt(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            int res = igsagent_parameter_int(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igsagent_parameter_double(IntPtr agent, IntPtr name);
        public double ParameterDouble(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            double res = igsagent_parameter_double(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_parameter_string(IntPtr agent, IntPtr name);
        public string ParameterString(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            string res = Igs.PtrToStringFromUTF8(igsagent_parameter_string(_pAgent, nameAsPtr));
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_data(IntPtr agent, IntPtr name, ref IntPtr data, ref uint size);
        public byte[] ParameterData(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            uint size = 0;
            byte[] data = null;
            IntPtr ptr = IntPtr.Zero;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            Result result = igsagent_parameter_data(_pAgent, nameAsPtr, ref ptr, ref size);
            Marshal.FreeHGlobal(nameAsPtr);
            if (result == Result.Success)
            {
                data = new byte[size];
                if (ptr != IntPtr.Zero)
                    Marshal.Copy(ptr, data, 0, (int)size);
            }
            return data;
        }

        #region set input
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_set_bool(IntPtr agent, IntPtr name, bool value);
        public Result InputSetBool(string name, bool value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_input_set_bool(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_set_int(IntPtr agent, IntPtr name, int value);
        public Result InputSetInt(string name, int value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_input_set_int(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_set_double(IntPtr agent, IntPtr name, double value);
        public Result InputSetDouble(string name, double value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_input_set_double(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_set_string(IntPtr agent, IntPtr name, IntPtr value);
        public Result InputSetString(string name, string value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr valueAsPtr = Igs.StringToUTF8Ptr(value);
            result = igsagent_input_set_string(_pAgent, nameAsPtr, valueAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_set_impulsion(IntPtr agent, IntPtr name);
        public Result InputSetImpulsion(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_input_set_impulsion(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_set_data(IntPtr agent, IntPtr name, IntPtr value, uint size);
        public Result InputSetData(string name, byte[] value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            uint size = Convert.ToUInt32(((byte[])value).Length);
            IntPtr valueAsPtr = Marshal.AllocHGlobal((int)size);
            Marshal.Copy(value, 0, valueAsPtr, (int)size);
            result = igsagent_input_set_data(_pAgent, nameAsPtr, valueAsPtr, size);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }
        #endregion

        #region set output
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_set_bool(IntPtr agent, IntPtr name, bool value);
        public Result OutputSetBool(string name, bool value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_output_set_bool(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_set_int(IntPtr agent, IntPtr name, int value);
        public Result OutputSetInt(string name, int value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_output_set_int(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_set_double(IntPtr agent, IntPtr name, double value);
        public Result OutputSetDouble(string name, double value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_output_set_double(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_set_string(IntPtr agent, IntPtr name, IntPtr value);
        public Result OutputSetString(string name, string value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr valueAsPtr = Igs.StringToUTF8Ptr(value);
            result = igsagent_output_set_string(_pAgent, nameAsPtr, valueAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_set_impulsion(IntPtr agent, IntPtr name);
        public Result OutputSetImpulsion(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_input_set_impulsion(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_set_data(IntPtr agent, IntPtr name, IntPtr value, uint size);
        public Result OutputSetData(string name, byte[] value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            uint size = Convert.ToUInt32(((byte[])value).Length);
            IntPtr valueAsPtr = Marshal.AllocHGlobal((int)size);
            Marshal.Copy(value, 0, valueAsPtr, (int)size);
            result = igsagent_output_set_data(_pAgent, nameAsPtr, valueAsPtr, size);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }
        #endregion

        #region set parameter
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_set_bool(IntPtr agent, IntPtr name, bool value);
        public Result ParameterSetBool(string name, bool value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_parameter_set_bool(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_set_int(IntPtr agent, IntPtr name, int value);
        public Result ParameterSetInt(string name, int value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_parameter_set_int(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_set_double(IntPtr agent, IntPtr name, double value);
        public Result ParameterSetDouble(string name, double value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            result = igsagent_parameter_set_double(_pAgent, nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_set_string(IntPtr agent, IntPtr name, IntPtr value);
        public Result ParameterSetString(string name, string value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr valueAsPtr = Igs.StringToUTF8Ptr(value);
            result = igsagent_parameter_set_string(_pAgent, nameAsPtr, valueAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_set_data(IntPtr agent, IntPtr name, IntPtr value, uint size);
        public Result ParameterSetData(string name, byte[] value)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            uint size = Convert.ToUInt32(((byte[])value).Length);
            IntPtr valueAsPtr = Marshal.AllocHGlobal((int)size);
            Marshal.Copy(value, 0, valueAsPtr, (int)size);
            result = igsagent_parameter_set_data(_pAgent, nameAsPtr, valueAsPtr, size);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }
        #endregion


        //Constraints on IOPs
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_constraints_enforce(IntPtr agent, bool enforce);//default is false, i.e. disabled
        public  void ConstraintsEnforce(bool enforce) 
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            igsagent_constraints_enforce(_pAgent, enforce); 
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_input_add_constraint(IntPtr agent, IntPtr name, IntPtr constraint);        
        public Result InputAddConstraint(string name, string constraint)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr constraintAsPtr = Igs.StringToUTF8Ptr(constraint);
            result = igsagent_input_add_constraint(_pAgent, nameAsPtr, constraintAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(constraintAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_output_add_constraint(IntPtr agent, IntPtr name, IntPtr constraint);
        public Result OutputAddConstraint(string name, string constraint)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr constraintAsPtr = Igs.StringToUTF8Ptr(constraint);
            result = igsagent_output_add_constraint(_pAgent, nameAsPtr, constraintAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(constraintAsPtr);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_parameter_add_constraint(IntPtr agent, IntPtr name, IntPtr constraint);
        public Result ParameterAddConstraint(string name, string constraint)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Result result = Result.Failure;
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr constraintAsPtr = Igs.StringToUTF8Ptr(constraint);
            result = igsagent_parameter_add_constraint(_pAgent, nameAsPtr, constraintAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(constraintAsPtr);
            return result;
        }

        //IOP descriptions
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_input_set_description(IntPtr agent, IntPtr name, IntPtr description);
        public void InputSetDescription(string name, string description)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr descriptionAsPtr = Igs.StringToUTF8Ptr(description);
            igsagent_input_set_description(_pAgent, nameAsPtr, descriptionAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(descriptionAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_output_set_description(IntPtr agent, IntPtr name, IntPtr description);
        public void OutputSetDescription(string name, string description)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr descriptionAsPtr = Igs.StringToUTF8Ptr(description);
            igsagent_output_set_description(_pAgent, nameAsPtr, descriptionAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(descriptionAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_parameter_set_description(IntPtr agent, IntPtr name, IntPtr description);
        public void ParameterSetDescription(string name, string description)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            IntPtr descriptionAsPtr = Igs.StringToUTF8Ptr(description);
            igsagent_parameter_set_description(_pAgent, nameAsPtr, descriptionAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(descriptionAsPtr);
        }

        /*These two functions enable sending and receiving DATA on
         inputs/outputs by using zmsg_t structures. zmsg_t structures
         offer advanced functionalities for data serialization.
         More can be found here: http://czmq.zeromq.org/manual:zmsg */
        // No c# wrapping of zmsg_t structure exist.
        //FIXME: INGESCAPE_EXPORT igs_result_t igsagent_output_set_zmsg(const char *name, zmsg_t *msg);
        //FIXME: INGESCAPE_EXPORT igs_result_t igsagent_input_zmsg(const char *name, zmsg_t **msg); //msg is owned by caller


        //clear IOP data in memory without having to write an empty value
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_clear_input(IntPtr agent, IntPtr name);
        public void ClearInput(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            igsagent_clear_input(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_clear_output(IntPtr agent, IntPtr name);
        public void ClearOutput(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            igsagent_clear_output(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_clear_parameter(IntPtr agent, IntPtr name);
        public void ClearParameter(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            igsagent_clear_parameter(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        public delegate void IopFunction(Agent agent, IopType iopType, string name, IopValueType valueType, object value, object myData);

        private delegate void igsAgent_observeCallbackC(IntPtr agent, IopType iopType, IntPtr name, IopValueType valueType, IntPtr value, uint valueSize, IntPtr myData);
        private igsAgent_observeCallbackC _OnAgentObserveCallback;

        private void OnAgentObserveCallback(IntPtr agent, IopType iopType, IntPtr name, IopValueType valueType, IntPtr value, uint valueSize, IntPtr myData)
        {
            Tuple<Agent.IopFunction, object> tupleData = (Tuple<Agent.IopFunction, object>)GCHandle.FromIntPtr(myData).Target;
            Agent.IopFunction cSharpFunction = tupleData.Item1;
            object data = tupleData.Item2;
            object newValue = null;
            switch (valueType)
            {
                case IopValueType.Bool:
                    newValue = Igs.PtrToBool(value);
                    break;
                case IopValueType.Data:
                    newValue = Igs.PtrToData(value, (int)valueSize);
                    break;
                case IopValueType.Double:
                    newValue = Igs.PtrToDouble(value);
                    break;
                case IopValueType.Impulsion:
                    break;
                case IopValueType.Integer:
                    newValue = Igs.PtrToInt(value);
                    break;
                case IopValueType.String:
                    newValue = Igs.PtrToStringFromUTF8(value);
                    break;
            }
            cSharpFunction(this, iopType, Igs.PtrToStringFromUTF8(name), valueType, newValue, data);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_observe_input(IntPtr agent, IntPtr name, igsAgent_observeCallbackC cb, IntPtr myData);
        public void ObserveInput(string name, IopFunction callback, object myData)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Tuple<Agent.IopFunction, object> tupleData = new Tuple<Agent.IopFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnAgentObserveCallback == null)
                _OnAgentObserveCallback = OnAgentObserveCallback;
            
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            igsagent_observe_input(_pAgent, nameAsPtr, _OnAgentObserveCallback, data);
            Marshal.FreeHGlobal(nameAsPtr);
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_observe_output(IntPtr agent, IntPtr name, igsAgent_observeCallbackC cb, IntPtr myData);
        public void ObserveOutput(string name, IopFunction callback, object myData)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Tuple<Agent.IopFunction, object> tupleData = new Tuple<Agent.IopFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnAgentObserveCallback == null)
                _OnAgentObserveCallback = OnAgentObserveCallback;
            
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            igsagent_observe_output(_pAgent, nameAsPtr, _OnAgentObserveCallback, data);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_observe_parameter(IntPtr agent, IntPtr name, igsAgent_observeCallbackC cb, IntPtr myData);
        public void ObserveParameter(string name, IopFunction callback, object myData)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Tuple<Agent.IopFunction, object> tupleData = new Tuple<Agent.IopFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnAgentObserveCallback == null)
                _OnAgentObserveCallback = OnAgentObserveCallback;
            
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            igsagent_observe_parameter(_pAgent, nameAsPtr, _OnAgentObserveCallback, data);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_output_mute(IntPtr agent, IntPtr name);
        public void OutputMute(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            igsagent_output_mute(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_output_unmute(IntPtr agent, IntPtr name);
        public void OutputUnmute(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            igsagent_output_unmute(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_output_is_muted(IntPtr agent, IntPtr name);
        public bool OutputIsMuted(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            bool res = igsagent_output_is_muted(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }
        #endregion

        #region Mapping edition & inspection
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_mapping_load_str(IntPtr agent, IntPtr json_str);
        public Result MappingLoadStr(string json_str)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr jsonAsPtr = Igs.StringToUTF8Ptr(json_str);
            Result res = igsagent_mapping_load_str(_pAgent, jsonAsPtr);
            Marshal.FreeHGlobal(jsonAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_mapping_load_file(IntPtr agent, IntPtr file_path);
        public Result MappingLoadFile(string file_path)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr pathAsPtr = Igs.StringToUTF8Ptr(file_path);
            Result res = igsagent_mapping_load_file(_pAgent, pathAsPtr);
            Marshal.FreeHGlobal(pathAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_mapping_json(IntPtr agent); //returns json string, must be freed by caller
        public string MappingJson()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return Igs.PtrToStringFromUTF8(igsagent_mapping_json(_pAgent));
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_mapping_count(IntPtr agent);
        public uint MappingCount()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_mapping_count(_pAgent);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_clear_mappings(IntPtr agent);
        public void ClearMappings()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            igsagent_clear_mappings(_pAgent);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_clear_mappings_with_agent(IntPtr agent, IntPtr agentName);
        public void ClearMappingsWithAgent(string agentName)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr agentNameAsPtr = Igs.StringToUTF8Ptr(agentName);
            igsagent_clear_mappings_with_agent(_pAgent, agentNameAsPtr);
            Marshal.FreeHGlobal(agentNameAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_mapping_add(IntPtr agent, IntPtr fromOurInput, IntPtr toAgent, IntPtr withOutput);
        public uint MappingAdd(string fromOurInput, string toAgent, string withOutput)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr fromOurInputAsPtr = Igs.StringToUTF8Ptr(fromOurInput);
            IntPtr toAgentAsPtr = Igs.StringToUTF8Ptr(toAgent);
            IntPtr withOutputAsPtr = Igs.StringToUTF8Ptr(withOutput);
            uint res = igsagent_mapping_add(_pAgent, fromOurInputAsPtr, toAgentAsPtr, withOutputAsPtr);
            Marshal.FreeHGlobal(fromOurInputAsPtr);
            Marshal.FreeHGlobal(toAgentAsPtr);
            Marshal.FreeHGlobal(withOutputAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_mapping_remove_with_id(IntPtr agent, uint theId);
        public Result MappingRemoveWithId(uint theId)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_mapping_remove_with_id(_pAgent, theId);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_mapping_remove_with_name(IntPtr agent, IntPtr fromOurInput, IntPtr toAgent, IntPtr withOutput);
        public Result MappingRemoveWithName(string fromOurInput, string toAgent, string withOutput)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr fromOurInputAsPtr = Igs.StringToUTF8Ptr(fromOurInput);
            IntPtr toAgentAsPtr = Igs.StringToUTF8Ptr(toAgent);
            IntPtr withOutputAsPtr = Igs.StringToUTF8Ptr(withOutput);
            Result res = igsagent_mapping_remove_with_name(_pAgent, fromOurInputAsPtr, toAgentAsPtr, withOutputAsPtr);
            Marshal.FreeHGlobal(fromOurInputAsPtr);
            Marshal.FreeHGlobal(toAgentAsPtr);
            Marshal.FreeHGlobal(withOutputAsPtr);
            return res;
        }

        //split
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_split_count(IntPtr agent);
        public uint SplitCount
        {
            get
            {
                if (_pAgent == IntPtr.Zero) 
                    throw new NullReferenceException("Agent pointer is null");
                return igsagent_split_count(_pAgent);
            }
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_split_add(IntPtr agent, IntPtr fromOurInput, IntPtr toAgent, IntPtr withOutput);
        public uint SplitAdd(string fromOurInput, string toAgent, string withOutput)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr ptrFromOurInput = Igs.StringToUTF8Ptr(fromOurInput);
            IntPtr ptrToAgent = Igs.StringToUTF8Ptr(toAgent);
            IntPtr ptrWithOutput = Igs.StringToUTF8Ptr(withOutput);
            uint result = igsagent_split_add(_pAgent, ptrFromOurInput, ptrToAgent, ptrWithOutput);
            Marshal.FreeHGlobal(ptrFromOurInput);
            Marshal.FreeHGlobal(ptrToAgent);
            Marshal.FreeHGlobal(ptrWithOutput);
            return result;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_split_remove_with_id(IntPtr agent, uint id);
        public Result SplitRemoveWithId(uint id)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_split_remove_with_id(_pAgent, id);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_split_remove_with_name(IntPtr agent, IntPtr fromOurInput, IntPtr toAgent, IntPtr withOutput);
        public Result SplitRemoveWithName(string fromOurInput, string toAgent, string withOutput)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr ptrFromOurInput = Igs.StringToUTF8Ptr(fromOurInput);
            IntPtr ptrToAgent = Igs.StringToUTF8Ptr(toAgent);
            IntPtr ptrWithOutput = Igs.StringToUTF8Ptr(withOutput);
            Result result = igsagent_split_remove_with_name(_pAgent, ptrFromOurInput, ptrToAgent, ptrWithOutput);
            Marshal.FreeHGlobal(ptrFromOurInput);
            Marshal.FreeHGlobal(ptrToAgent);
            Marshal.FreeHGlobal(ptrWithOutput);
            return result;
        }


        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_mapping_set_outputs_request(IntPtr agent, bool notify);
        public void MappingSetOutputsRequest(bool notify)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            igsagent_mapping_set_outputs_request(_pAgent, notify);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_mapping_outputs_request(IntPtr agent);
        public bool MappingOutputsRequest()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_mapping_outputs_request(_pAgent);
        }
        #endregion

        #region Services edition & inspection
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_service_call(IntPtr agent, IntPtr agentNameOrUUID, IntPtr serviceName,
                                     ref IntPtr list, IntPtr token);
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_service_call(IntPtr agent, IntPtr agentNameOrUUID, IntPtr serviceName,
                                     IntPtr list, IntPtr token);

        public Result ServiceCall(string agentNameOrUUID, string serviceName, object[] arguments, string token = "")
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");;
            IntPtr agentNameOrUUIDAsPtr = Igs.StringToUTF8Ptr(agentNameOrUUID);
            IntPtr serviceNameAsPtr = Igs.StringToUTF8Ptr(serviceName);
            IntPtr tokenAsPtr = Igs.StringToUTF8Ptr(token);
            IntPtr ptr = IntPtr.Zero;
            Result success = Result.Failure;
            if (arguments != null)
            {
                int i = 0;
                foreach (object argument in arguments)
                {
                    i++;
                    if (argument != null)
                    {
                        if (argument.GetType() == typeof(bool))
                            Igs.igs_service_args_add_bool(ref ptr, Convert.ToBoolean(argument));
                        else if (argument.GetType() == typeof(byte[]))
                        {
                            byte[] data = (byte[])argument;
                            Igs.igs_service_args_add_data(ref ptr, data, (uint)data.Length);
                        }
                        else if (argument.GetType() == typeof(double) || argument.GetType() == typeof(float))
                            Igs.igs_service_args_add_double(ref ptr, Convert.ToDouble(argument));
                        else if (argument.GetType() == typeof(int))
                            Igs.igs_service_args_add_int(ref ptr, Convert.ToInt32(argument));
                        else if (argument.GetType() == typeof(string))
                        {
                            IntPtr argAsPtr = Igs.StringToUTF8Ptr(Convert.ToString(argument));
                            Igs.igs_service_args_add_string(ref ptr, argAsPtr);
                            Marshal.FreeHGlobal(argAsPtr);
                        }
                    }
                    else
                    {
                        Igs.Error(string.Format("argument at {0} is null. Cannot call service {1}", i.ToString(), serviceName));
                        Igs.igs_service_args_destroy(ref ptr);
                        return Result.Failure;
                    }
                }
                success = igsagent_service_call(_pAgent, agentNameOrUUIDAsPtr, serviceNameAsPtr, ref ptr, tokenAsPtr);
                Igs.igs_service_args_destroy(ref ptr);
            }
            else
            {
                success = igsagent_service_call(_pAgent, agentNameOrUUIDAsPtr, serviceNameAsPtr, ptr, tokenAsPtr);
                Igs.igs_service_args_destroy(ref ptr);
            }

            Marshal.FreeHGlobal(agentNameOrUUIDAsPtr);
            Marshal.FreeHGlobal(serviceNameAsPtr);
            Marshal.FreeHGlobal(tokenAsPtr);
            return success;
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void ServiceFunctionC(IntPtr agent, IntPtr senderAgentName,
                                            IntPtr senderAgentUUID,
                                            IntPtr serviceName,
                                            IntPtr firstArgument,
                                            uint nbArgs,
                                            IntPtr token,
                                            IntPtr myData);
        public delegate void ServiceFunction(Agent agent, string senderAgentName, string senderAgentUUID, string serviceName, List<ServiceArgument> arguments, string token, object myData);
        private ServiceFunctionC _OnAgentServiceCallback;

        void OnAgentServiceCallback(IntPtr agent, IntPtr senderAgentName,
                                  IntPtr senderAgentUUID,
                                  IntPtr serviceName,
                                  IntPtr firstArgument,
                                  uint nbArgs,
                                  IntPtr token,
                                  IntPtr myData)
        {
            GCHandle gCHandle = GCHandle.FromIntPtr(myData);
            Tuple<ServiceFunction, object> tuple = (Tuple<ServiceFunction, object>)gCHandle.Target;
            object data = tuple.Item2;
            ServiceFunction cSharpFunction = tuple.Item1;
            string serviceNameAsString = Igs.PtrToStringFromUTF8(serviceName);
            List<ServiceArgument> serviceArguments = ServiceArgumentsList(serviceNameAsString);
            cSharpFunction(this, Igs.PtrToStringFromUTF8(senderAgentName), Igs.PtrToStringFromUTF8(senderAgentUUID), serviceNameAsString, serviceArguments, Igs.PtrToStringFromUTF8(token), data);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_service_init(IntPtr agent, IntPtr name, Agent.ServiceFunctionC cb, IntPtr myData);
        public Result ServiceInit(string name, ServiceFunction callback, object myData)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            Tuple<Agent.ServiceFunction, object> tupleData = new Tuple<Agent.ServiceFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnAgentServiceCallback == null)
                _OnAgentServiceCallback = OnAgentServiceCallback;
            
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            Result res = igsagent_service_init(_pAgent, nameAsPtr, _OnAgentServiceCallback, data);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_service_remove(IntPtr agent, IntPtr name);
        public Result ServiceRemove(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            Result res = igsagent_service_remove(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_service_arg_add(IntPtr agent, IntPtr serviceName, IntPtr argName, IopValueType type);
        public Result ServiceArgAdd(string serviceName, string argName, IopValueType type)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr serviceNameAsPtr = Igs.StringToUTF8Ptr(serviceName);
            IntPtr argNameAsPtr = Igs.StringToUTF8Ptr(argName);
            Result res = igsagent_service_arg_add(_pAgent, serviceNameAsPtr, argNameAsPtr, type);
            Marshal.FreeHGlobal(serviceNameAsPtr);
            Marshal.FreeHGlobal(argNameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_service_arg_remove(IntPtr agent, IntPtr serviceName, IntPtr argName); //removes first occurence with this name
        public Result ServiceArgRemove(string serviceName, string argName)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr serviceNameAsPtr = Igs.StringToUTF8Ptr(serviceName);
            IntPtr argNameAsPtr = Igs.StringToUTF8Ptr(argName);
            Result res = igsagent_service_arg_remove(_pAgent, serviceNameAsPtr, argNameAsPtr);
            Marshal.FreeHGlobal(serviceNameAsPtr);
            Marshal.FreeHGlobal(argNameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_service_count(IntPtr agent);
        public uint ServiceCount()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            return igsagent_service_count(_pAgent);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_service_exists(IntPtr agent, IntPtr name);
        public bool ServiceExists(string name)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr nameAsPtr = Igs.StringToUTF8Ptr(name);
            bool res = igsagent_service_exists(_pAgent, nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_service_list(IntPtr agent, ref uint nbOfElements); //returned string* shall be freed by caller
        public string[] ServiceList()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            uint nbOfElements = 0;

            IntPtr intPtr = igsagent_service_list(_pAgent, ref nbOfElements);
            if (nbOfElements != 0)
            {
                IntPtr[] intPtrArray = new IntPtr[nbOfElements];
                Marshal.Copy(intPtr, intPtrArray, 0, (int)nbOfElements);
                string[] list = new string[nbOfElements];
                for (int i = 0; i < nbOfElements; i++)
                    list[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);

                Igs.igs_free_services_list(intPtr, nbOfElements);
                return list;
            }
            else return null;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igsagent_service_args_first(IntPtr agent, IntPtr serviceName); // Use PtrToStrutucre        

        public List<ServiceArgument> ServiceArgumentsList(string name)
        {
            IntPtr ptrArgument = igsagent_service_args_first(_pAgent, Igs.StringToUTF8Ptr(name));
            List<ServiceArgument> serviceArgumentsList = new List<ServiceArgument>();
            if (ptrArgument != null)
            {
                while (ptrArgument != IntPtr.Zero)
                {
                    // Marshals data from an unmanaged block of memory to a newly allocated managed object of the type specified by a generic type parameter.
                    Igs.StructServiceArgument structArgument = Marshal.PtrToStructure<Igs.StructServiceArgument>(ptrArgument);

                    object value = null;

                    switch (structArgument.type)
                    {
                        case IopValueType.Bool:
                            value = structArgument.union.b;
                            break;

                        case IopValueType.Integer:
                            value = structArgument.union.i;
                            break;

                        case IopValueType.Double:
                            value = structArgument.union.d;
                            break;

                        case IopValueType.String:
                            value = Igs.PtrToStringFromUTF8(structArgument.union.c);
                            break;

                        case IopValueType.Data:
                            byte[] byteArray = new byte[structArgument.size];

                            // FIXME: size has type "uint" in language C. The corresponding type in C# is uint. But "Marshal.Copy(...)" does not accept uint for parameter "length"
                            if (structArgument.union.data != IntPtr.Zero)
                                Marshal.Copy(structArgument.union.data, byteArray, 0, (int)structArgument.size);                            
                            else
                                byteArray = null;

                            value = byteArray;
                            break;

                        default:
                            break;
                    }
                    ServiceArgument serviceArgument = new ServiceArgument(Igs.PtrToStringFromUTF8(structArgument.name), structArgument.type, value);
                    serviceArgumentsList.Add(serviceArgument);
                    ptrArgument = structArgument.next;
                }
                return serviceArgumentsList;
            }
            else
                return null;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igsagent_service_args_count(IntPtr agent, IntPtr serviceName);
        public uint ServiceArgsCount(string serviceName)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr serviceNameAsPtr = Igs.StringToUTF8Ptr(serviceName);
            uint res = igsagent_service_args_count(_pAgent, serviceNameAsPtr);
            Marshal.FreeHGlobal(serviceNameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igsagent_service_arg_exists(IntPtr agent, IntPtr serviceName, IntPtr argName);
        public bool ServiceArgExists(string serviceName, string argName)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr serviceNameAsPtr = Igs.StringToUTF8Ptr(serviceName);
            IntPtr argNameAsPtr = Igs.StringToUTF8Ptr(argName);
            bool res = igsagent_service_arg_exists(_pAgent, serviceNameAsPtr, argNameAsPtr);
            Marshal.FreeHGlobal(serviceNameAsPtr);
            Marshal.FreeHGlobal(argNameAsPtr);
            return res;
        }
        #endregion

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_election_join(IntPtr agent, IntPtr electionName);
        public Result ElectionJoin(string electionName)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr electionNameAsPtr = Igs.StringToUTF8Ptr(electionName);
            Result res = igsagent_election_join(_pAgent, electionNameAsPtr);
            Marshal.FreeHGlobal(electionNameAsPtr);
            return res;
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igsagent_election_leave(IntPtr agent, IntPtr electionName);
        public Result ElectionLeave(string electionName)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr electionNameAsPtr = Igs.StringToUTF8Ptr(electionName);
            Result res = igsagent_election_leave(_pAgent, electionNameAsPtr);
            Marshal.FreeHGlobal(electionNameAsPtr);
            return res;
        }

        #region Administration, logging, configuration and utilities
        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_definition_set_path(IntPtr agent, IntPtr path);
        public void DefinitionSetPath(string path)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr pathAsPtr = Igs.StringToUTF8Ptr(path);
            igsagent_definition_set_path(_pAgent, pathAsPtr);
            Marshal.FreeHGlobal(pathAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_mapping_set_path(IntPtr agent, IntPtr path);
        public void MappingSetPath(string path)
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            IntPtr pathAsPtr = Igs.StringToUTF8Ptr(path);
            igsagent_mapping_set_path(_pAgent, pathAsPtr);
            Marshal.FreeHGlobal(pathAsPtr);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_definition_save(IntPtr agent);
        public void DefinitionSave()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            igsagent_definition_save(_pAgent);
        }

        [DllImport(Igs.ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igsagent_mapping_save(IntPtr agent);
        public void MappingSave()
        {
            if (_pAgent == IntPtr.Zero) 
                throw new NullReferenceException("Agent pointer is null");
            igsagent_mapping_save(_pAgent);
        }
        #endregion
    };
}
