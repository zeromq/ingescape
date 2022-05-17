/*  =========================================================================
Ingescape.cs

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
    public partial class Igs
    {
        #region Path to library C IngeScape
#if RELEASE
        internal const string ingescapeDLLPath = "ingescape";
#elif DEBUG
        internal const string ingescapeDLLPath = "ingescaped";
#endif
        #endregion

        #region Agent initialization, control and events
        // Initialization and control
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_start_with_device(IntPtr device, uint port);
        public static Result StartWithDevice(string networkDevice, uint port)
        {
            // ingescape provide devices in Latin-1, need to start with Latin-1
            Result result = igs_start_with_device(StringToLatin1Ptr(networkDevice), port);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_start_with_ip(IntPtr ipAddress, uint port);
        public static Result StartWithIp(string ipAddress, uint port)
        {
            IntPtr ipAsPtr = StringToUTF8Ptr(ipAddress);
            Result res = igs_start_with_ip(ipAsPtr, port);
            Marshal.FreeHGlobal(ipAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_stop();
        public static void Stop() { igs_stop(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_is_started();
        public static bool IsStarted() { return igs_is_started(); }

        private static ForcedStopFunctionC _OnForcedStopCallback;
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void ForcedStopFunctionC(IntPtr myData);
        public delegate void ForcedStopFunction(object myData);

        static void OnForcedStopCallback(IntPtr myData)
        {
            Tuple<ForcedStopFunction, object> tupleData = (Tuple<ForcedStopFunction, object>)GCHandle.FromIntPtr(myData).Target;
            ForcedStopFunction cSharpFunction = tupleData.Item1;
            object data = tupleData.Item2;
            cSharpFunction(data);
        }

        //PUBLIC zsock_t* igs_pipe_to_ingescape(void); //TOFIX: zsock

        //register a callback when the agent is forced to Stop by the ingescape platform
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observe_forced_stop([MarshalAs(UnmanagedType.FunctionPtr)] ForcedStopFunctionC cb, IntPtr myData);
        public static void ObserveForcedStop(ForcedStopFunction callback, object myData)
        {
            Tuple<ForcedStopFunction, object> tupleData = new Tuple<ForcedStopFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnForcedStopCallback == null)
                _OnForcedStopCallback = OnForcedStopCallback;
            
            igs_observe_forced_stop(_OnForcedStopCallback, data);
        }

        // Agent name
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_agent_set_name(IntPtr name);
        public static void AgentSetName(string name)
        {
            IntPtr strPtr = StringToUTF8Ptr(name);
            igs_agent_set_name(strPtr);
            Marshal.FreeHGlobal(strPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_agent_name();
        public static string AgentName()
        {
            IntPtr ptr = igs_agent_name();
            return PtrToStringFromUTF8(ptr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_agent_uuid();
        public static string AgentUUID()
        {
            IntPtr ptr = igs_agent_uuid();
            return PtrToStringFromUTF8(ptr);
        }

        // Agent state
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_agent_set_state(IntPtr state);
        public static void AgentSetState(string state)
        {
            IntPtr stateAsPtr = StringToUTF8Ptr(state);
            igs_agent_set_state(stateAsPtr);
            Marshal.FreeHGlobal(stateAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_agent_state();
        public static string AgentState()
        {
            IntPtr ptr = igs_agent_state();
            return PtrToStringFromUTF8(ptr);
        }

        // Agent family - optional
        //32 characters canonical UUID format is commonly expected,
        //default is an empty string, max length is 64 characters*/
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_agent_set_family(IntPtr family);
        public static void AgentSetFamily(string family)
        {
            IntPtr familyAsPtr = StringToUTF8Ptr(family);
            igs_agent_set_family(familyAsPtr);
            Marshal.FreeHGlobal(familyAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_agent_family(); //string must be freed by caller
        public static string AgentFamily() { return PtrToStringFromUTF8(igs_agent_family()); }

        // Mute the agent ouputs
        private static MuteFunctionC _OnMutedCallback;
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void MuteFunctionC(bool AgentIsMuted, IntPtr myData);
        public delegate void MuteFunction(bool AgentIsMuted, object myData);

        static void OnMutedCallback(bool isMuted, IntPtr myData)
        {
            Tuple<MuteFunction, object> tupleData = (Tuple<MuteFunction, object>)GCHandle.FromIntPtr(myData).Target;
            MuteFunction cSharpFunction = tupleData.Item1;
            object data = tupleData.Item2;
            cSharpFunction(isMuted, data);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_agent_mute();
        public static int AgentMute() { return igs_agent_mute(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_agent_unmute();
        public static int AgentUnmute() { return igs_agent_unmute(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_agent_is_muted();
        public static bool AgentIsMuted() { return igs_agent_is_muted(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observe_mute([MarshalAs(UnmanagedType.FunctionPtr)] MuteFunctionC cb, IntPtr myData);
        public static void ObserveMute(MuteFunction callback, object myData)
        {
            Tuple<MuteFunction, object> tupleData = new Tuple<MuteFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnMutedCallback == null)
                _OnMutedCallback = OnMutedCallback;
            
            igs_observe_mute(_OnMutedCallback, data);
        }

        //Freeze and Unfreeze the agent
        //When freezed, agent will not send anything on its outputs and
        //its inputs are not reactive to external data.
        //NB: the internal semantics of Freeze and Unfreeze for a given agent
        //are up to the developer and can be controlled using callbacks and igs_observe_freeze
        private static FreezeFunctionC _OnFreezeCallback;
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void FreezeFunctionC(bool isPaused, IntPtr myData);
        public delegate void FreezeFunction(bool isPaused, object myData);

        static void OnFreezeCallback(bool isFreeze, IntPtr myData)
        {
            Tuple<FreezeFunction, object> tupleData = (Tuple<FreezeFunction, object>)GCHandle.FromIntPtr(myData).Target;
            FreezeFunction cSharpFunction = tupleData.Item1;
            object data = tupleData.Item2;
            cSharpFunction(isFreeze, data);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_freeze();
        public static Result Freeze() { return igs_freeze(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_is_frozen();
        public static bool IsFrozen() { return igs_is_frozen(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_unfreeze();
        public static int Unfreeze() { return igs_unfreeze(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_observe_freeze(FreezeFunctionC cb, IntPtr myData);
        public static int ObserveFreeze(FreezeFunction callback, object myData)
        {
            Tuple<FreezeFunction, object> tupleData = new Tuple<FreezeFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnFreezeCallback == null)
                _OnFreezeCallback = OnFreezeCallback;
            
            return igs_observe_freeze(_OnFreezeCallback, data);
        }

        private static AgentEventsFunctionC _OnAgentEvents;
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void AgentEventsFunctionC(AgentEvent agentEvent, IntPtr uuid, IntPtr name, IntPtr eventData, IntPtr myData);
        public delegate void AgentEventsFunction(AgentEvent agentEvent,
                                   string uuid,
                                   string name,
                                   object eventData,
                                   object myData);

        static void OnAgentEventsCallBack(AgentEvent agentEvent, IntPtr uuid, IntPtr name, IntPtr eventData, IntPtr myData)
        {
            GCHandle gCHandleData = GCHandle.FromIntPtr(myData);
            object eventDataAsObject = null;
            if (eventData != IntPtr.Zero && agentEvent != AgentEvent.PeerEntered)
                eventDataAsObject = PtrToStringFromUTF8(eventData);

            Tuple<AgentEventsFunction, object> tuple = (Tuple<AgentEventsFunction, object>)gCHandleData.Target;
            object data = tuple.Item2;

            AgentEventsFunction cSharpFunction = tuple.Item1;
            cSharpFunction(agentEvent, PtrToStringFromUTF8(uuid), PtrToStringFromUTF8(name), eventDataAsObject, data);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_observe_agent_events(AgentEventsFunctionC cb, IntPtr myData);
        public static int ObserveAgentEvents(AgentEventsFunction callback, object myData)
        {
            Tuple<AgentEventsFunction, object> tupleData = new Tuple<AgentEventsFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnAgentEvents == null)
                _OnAgentEvents = OnAgentEventsCallBack;
            
            return igs_observe_agent_events(_OnAgentEvents, data);
        }
        #endregion

        #region Editing & inspecting definitions, adding and removing inputs/outputs/parameters
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_definition_load_str(IntPtr json_str);
        public static Result DefinitionLoadStr(string json)
        {
            IntPtr jsonAsPtr = StringToUTF8Ptr(json);
            Result res = igs_definition_load_str(jsonAsPtr);
            Marshal.FreeHGlobal(jsonAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_definition_load_file(IntPtr file_path);
        public static Result DefinitionLoadFile(string file_path)
        {
            IntPtr pathAsPtr = StringToUTF8Ptr(file_path);
            Result res = igs_definition_load_file(pathAsPtr);
            Marshal.FreeHGlobal(pathAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_clear_definition();
        public static int ClearDefinition() { return igs_clear_definition(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_definition_json();
        public static string DefinitionJson()
        {
            IntPtr ptr = igs_definition_json();
            return (ptr == IntPtr.Zero) ? string.Empty : Marshal.PtrToStringAnsi(ptr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_definition_description();
        public static string DefinitionDescription()
        {
            IntPtr ptr = igs_definition_description();
            return PtrToStringFromUTF8(ptr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_definition_version();
        public static string DefinitionVersion()
        {
            IntPtr ptr = igs_definition_version();
            return (ptr == IntPtr.Zero) ? string.Empty : Marshal.PtrToStringAnsi(ptr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_definition_set_description(IntPtr description);
        public static void DefinitionSetDescription(string description)
        {
            IntPtr descriptionAsPtr = StringToUTF8Ptr(description);
            igs_definition_set_description(descriptionAsPtr);
            Marshal.FreeHGlobal(descriptionAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_definition_set_version(IntPtr Version);
        public static void DefinitionSetVersion(string Version)
        {
            IntPtr versionAsPtr = StringToUTF8Ptr(Version);
            igs_definition_set_version(versionAsPtr);
            Marshal.FreeHGlobal(versionAsPtr);
        }

        // Edit the definition using the API
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_create(IntPtr name, IopValueType value_type, IntPtr value, uint size);
        public static Result InputCreate(string name, IopValueType type, object value = null)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            if (value != null)
            {
                uint size;
                IntPtr valuePtr;
                if (value.GetType() == typeof(string))
                    valuePtr = StringToUTF8Ptr(Convert.ToString(value), out size);
                else if (value.GetType() == typeof(bool))
                    valuePtr = BoolToPtr(Convert.ToBoolean(value), out size);
                else if (value.GetType() == typeof(byte[]))
                    valuePtr = DataToPtr((byte[])value, out size);
                else if (value.GetType() == typeof(double))
                    valuePtr = DoubleToPtr(Convert.ToDouble(value), out size);
                else if (value.GetType() == typeof(float))
                    valuePtr = DoubleToPtr(Convert.ToDouble(value), out size);
                else if (value.GetType() == typeof(int))
                    valuePtr = IntToPtr(Convert.ToInt32(value), out size);
                else 
                    return Result.Failure;

                Result res = igs_input_create(nameAsPtr, type, valuePtr, size);
                Marshal.FreeHGlobal(nameAsPtr);
                Marshal.FreeHGlobal(valuePtr);
                return res;
            }
            else
            {
                Result res = igs_input_create(nameAsPtr, type, IntPtr.Zero, 0);
                Marshal.FreeHGlobal(nameAsPtr);
                return res;
            }
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_create(IntPtr name, IopValueType type, IntPtr value, uint size);
        public static Result OutputCreate(string name, IopValueType type, object value = null)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            if (value != null)
            {
                uint size;
                IntPtr valuePtr;
                if (value.GetType() == typeof(string))
                    valuePtr = StringToUTF8Ptr(Convert.ToString(value), out size);
                else if (value.GetType() == typeof(bool))
                    valuePtr = BoolToPtr(Convert.ToBoolean(value), out size);
                else if (value.GetType() == typeof(byte[]))
                    valuePtr = DataToPtr((byte[])value, out size);
                else if (value.GetType() == typeof(double))
                    valuePtr = DoubleToPtr(Convert.ToDouble(value), out size);
                else if (value.GetType() == typeof(float))
                    valuePtr = DoubleToPtr(Convert.ToDouble(value), out size);
                else if (value.GetType() == typeof(int))
                    valuePtr = IntToPtr(Convert.ToInt32(value), out size);
                else
                    return Result.Failure;

                Result res = igs_output_create(nameAsPtr, type, valuePtr, size);
                Marshal.FreeHGlobal(nameAsPtr);
                Marshal.FreeHGlobal(valuePtr);
                return res;
            }
            else
            {
                Result res = igs_output_create(nameAsPtr, type, IntPtr.Zero, 0);
                Marshal.FreeHGlobal(nameAsPtr);
                return res;
            }
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_create(IntPtr name, IopValueType type, IntPtr value, uint size);
        public static Result ParameterCreate(string name, IopValueType type, object value = null)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            if (value != null)
            {
                uint size;
                IntPtr valuePtr;
                if (value.GetType() == typeof(string))
                    valuePtr = StringToUTF8Ptr(Convert.ToString(value), out size);
                else if (value.GetType() == typeof(bool))
                    valuePtr = BoolToPtr(Convert.ToBoolean(value), out size);
                else if (value.GetType() == typeof(byte[]))
                    valuePtr = DataToPtr((byte[])value, out size);
                else if (value.GetType() == typeof(double))
                    valuePtr = DoubleToPtr(Convert.ToDouble(value), out size);
                else if (value.GetType() == typeof(float))
                    valuePtr = DoubleToPtr(Convert.ToDouble(value), out size);
                else if (value.GetType() == typeof(int))
                    valuePtr = IntToPtr(Convert.ToInt32(value), out size);
                else
                    return Result.Failure;
                Result res = igs_parameter_create(nameAsPtr, type, valuePtr, size);
                Marshal.FreeHGlobal(nameAsPtr);
                Marshal.FreeHGlobal(valuePtr);
                return res;
            }
            else
            {
                Result res = igs_parameter_create(nameAsPtr, type, IntPtr.Zero, 0);
                Marshal.FreeHGlobal(nameAsPtr);
                return res;
            }
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_remove(IntPtr name);
        public static Result InputRemove(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            Result res = igs_input_remove(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_remove(IntPtr name);
        public static Result OutputRemove(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            Result res = igs_output_remove(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_remove(IntPtr name);
        public static Result ParameterRemove(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            Result res = igs_parameter_remove(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        //check IOP type, lists and existence
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IopValueType igs_input_type(IntPtr name);
        public static IopValueType InputType(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IopValueType type = igs_input_type(StringToUTF8Ptr(name));
            Marshal.FreeHGlobal(nameAsPtr);
            return type;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IopValueType igs_output_type(IntPtr name);
        public static IopValueType OutputType(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IopValueType type = igs_output_type(StringToUTF8Ptr(name));
            Marshal.FreeHGlobal(nameAsPtr);
            return type;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IopValueType igs_parameter_type(IntPtr name);
        public static IopValueType ParameterType(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IopValueType type = igs_parameter_type(StringToUTF8Ptr(name));
            Marshal.FreeHGlobal(nameAsPtr);
            return type;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_input_count();
        public static int InputCount() { return igs_input_count(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_output_count();
        public static int OutputCount() { return igs_output_count(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_parameter_count();
        public static int ParameterCount() { return igs_parameter_count(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_input_list(ref int nbOfElements);
        public static string[] InputList()
        {
            int nbOfElements = 0;
            string[] list = null;
            IntPtr intptr = igs_input_list(ref nbOfElements);
            if (intptr != IntPtr.Zero)
            {
                IntPtr[] intPtrArray = new IntPtr[nbOfElements];
                list = new string[nbOfElements];
                Marshal.Copy(intptr, intPtrArray, 0, nbOfElements);
                for (int i = 0; i < nbOfElements; i++)
                    list[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);
                Igs.igs_free_iop_list(intptr, nbOfElements);
            }
            return list;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_output_list(ref int nbOfElements);
        public static string[] OutputList()
        {
            int nbOfElements = 0;
            string[] list = null;
            IntPtr intptr = igs_output_list(ref nbOfElements);
            if (intptr != IntPtr.Zero)
            {
                IntPtr[] intPtrArray = new IntPtr[nbOfElements];
                list = new string[nbOfElements];
                Marshal.Copy(intptr, intPtrArray, 0, nbOfElements);
                for (int i = 0; i < nbOfElements; i++)
                    list[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);
                Igs.igs_free_iop_list(intptr, nbOfElements);
            }
            return list;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_parameter_list(ref int nbOfElements);
        public static string[] ParameterList()
        {
            int nbOfElements = 0;
            string[] list = null;
            IntPtr intptr = igs_parameter_list(ref nbOfElements);
            if (intptr != IntPtr.Zero)
            {
                IntPtr[] intPtrArray = new IntPtr[nbOfElements];
                list = new string[nbOfElements];
                Marshal.Copy(intptr, intPtrArray, 0, nbOfElements);
                for (int i = 0; i < nbOfElements; i++)
                    list[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);
                Igs.igs_free_iop_list(intptr, nbOfElements);
            }
            return list;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void igs_free_iop_list(IntPtr list, int nbOfElements);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_input_exists(IntPtr name);
        public static bool InputExists(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            bool value = igs_input_exists(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_output_exists(IntPtr name);
        public static bool OutputExists(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            bool value = igs_output_exists(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_parameter_exists(IntPtr name);
        public static bool ParameterExists(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            bool value = igs_parameter_exists(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }
        #endregion

        #region Reading and writing inputs/outputs/parameters, a.k.a IOPs

        // IOP Model : Inputs, Outputs and Parameters read/write/check/observe/AgentMute


        //read per type
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_input_bool(IntPtr name);
        public static bool InputBool(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            bool value = igs_input_bool(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_input_int(IntPtr name);
        public static int InputInt(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            int value = igs_input_int(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igs_input_double(IntPtr name);
        public static double InputDouble(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            double value = igs_input_double(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_input_string(IntPtr name);
        public static string InputString(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr valueAsPtr = igs_input_string(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            string value = PtrToStringFromUTF8(valueAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_data(IntPtr name, ref IntPtr data, ref uint size);
        public static byte[] InputData(string name)
        {
            uint size = 0;
            byte[] data = null;
            IntPtr ptr = IntPtr.Zero;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            Result result = igs_input_data(nameAsPtr, ref ptr, ref size);
            Marshal.FreeHGlobal(nameAsPtr);
            if (result == Result.Success)
            {
                data = new byte[size];
                if (ptr != IntPtr.Zero)
                    Marshal.Copy(ptr, data, 0, (int)size);
            }
            return data;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_output_bool(IntPtr name);
        public static bool OutputBool(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            bool value = igs_output_bool(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_output_int(IntPtr name);
        public static int OutputInt(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            int value = igs_output_int(StringToUTF8Ptr(name));
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igs_output_double(IntPtr name);
        public static double OutputDouble(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            double value = igs_output_double(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_output_string(IntPtr name);
        public static string OutputString(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr valueAsPtr = igs_output_string(nameAsPtr);
            string value = PtrToStringFromUTF8(valueAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_data(IntPtr name, ref IntPtr data, ref uint size);
        public static byte[] OutputData(string name)
        {
            uint size = 0;
            byte[] data = null;
            IntPtr ptr = IntPtr.Zero;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            Result result = igs_output_data(nameAsPtr, ref ptr, ref size);
            Marshal.FreeHGlobal(nameAsPtr);
            if (result == Result.Success)
            {
                data = new byte[size];
                if (ptr != IntPtr.Zero)
                    Marshal.Copy(ptr, data, 0, (int)size);
            }
            return data;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_parameter_bool(IntPtr name);        
        public static bool ParameterBool(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            bool value = igs_parameter_bool(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_parameter_int(IntPtr name);
        public static int ParameterInt(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            int value = igs_parameter_int(StringToUTF8Ptr(name));
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igs_parameter_double(IntPtr name);
        public static double ParameterDouble(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            double value = igs_parameter_double(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_parameter_string(IntPtr name);
        public static string ParameterString(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr valueAsPtr = igs_parameter_string(nameAsPtr);
            string value = PtrToStringFromUTF8(valueAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_data(IntPtr name, ref IntPtr data, ref uint size);
        public static byte[] ParameterData(string name)
        {
            uint size = 0;
            byte[] data = null;
            IntPtr ptr = IntPtr.Zero;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            Result result = igs_parameter_data(nameAsPtr, ref ptr, ref size);
            Marshal.FreeHGlobal(nameAsPtr);
            if (result == Result.Success)
            {
                data = new byte[size];
                if (ptr != IntPtr.Zero)
                    Marshal.Copy(ptr, data, 0, (int)size);
            }
            return data;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_set_bool(IntPtr name, bool value);
        public static Result InputSetBool(string name, bool value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_input_set_bool(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_set_int(IntPtr name, int value);
        public static Result InputSetInt(string name, int value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_input_set_int(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_set_double(IntPtr name, double value);
        public static Result InputSetDouble(string name, double value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_input_set_double(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_set_string(IntPtr name, IntPtr value);
        public static Result InputSetString(string name, string value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr valueAsPtr = StringToUTF8Ptr(value);
            result = igs_input_set_string(nameAsPtr, valueAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_set_impulsion(IntPtr name);
        public static Result InputSetImpulsion(string name)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_input_set_impulsion(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_set_data(IntPtr name, IntPtr value, uint size);
        public static Result InputSetData(string name, byte[] value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            uint size = Convert.ToUInt32(((byte[])value).Length);
            IntPtr valueAsPtr = Marshal.AllocHGlobal((int)size);
            Marshal.Copy(value, 0, valueAsPtr, (int)size);
            result = igs_input_set_data(nameAsPtr, valueAsPtr, size);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_set_bool(IntPtr name, bool value);
        public static Result OutputSetBool(string name, bool value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_output_set_bool(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_set_int(IntPtr name, int value);
        public static Result OutputSetInt(string name, int value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_output_set_int(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_set_double(IntPtr name, double value);
        public static Result OutputSetDouble(string name, double value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_output_set_double(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_set_string(IntPtr name, IntPtr value);
        public static Result OutputSetString(string name, string value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr valueAsPtr = StringToUTF8Ptr(value);
            result = igs_output_set_string(nameAsPtr, valueAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_set_impulsion(IntPtr name);
        public static Result OutputSetImpulsion(string name)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_output_set_impulsion(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_set_data(IntPtr name, IntPtr value, uint size);
        public static Result OutputSetData(string name, byte[] value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            uint size = Convert.ToUInt32(((byte[])value).Length);
            IntPtr valueAsPtr = Marshal.AllocHGlobal((int)size);
            Marshal.Copy(value, 0, valueAsPtr, (int)size);
            result = igs_output_set_data(nameAsPtr, valueAsPtr, size);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_set_bool(IntPtr name, bool value);
        public static Result ParameterSetBool(string name, bool value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_parameter_set_bool(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_set_int(IntPtr name, int value);
        public static Result ParameterSetInt(string name, int value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_parameter_set_int(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_set_double(IntPtr name, double value);
        public static Result ParameterSetDouble(string name, double value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            result = igs_parameter_set_double(nameAsPtr, value);
            Marshal.FreeHGlobal(nameAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_set_string(IntPtr name, IntPtr value);
        public static Result ParameterSetString(string name, string value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr valueAsPtr = StringToUTF8Ptr(value);
            result = igs_parameter_set_string(nameAsPtr, valueAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_set_data(IntPtr name, IntPtr value, uint size);
        public static Result ParameterSetData(string name, byte[] value)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            uint size = Convert.ToUInt32(((byte[])value).Length);
            IntPtr valueAsPtr = Marshal.AllocHGlobal((int)size);
            Marshal.Copy(value, 0, valueAsPtr, (int)size);
            result = igs_parameter_set_data(nameAsPtr, valueAsPtr, size);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(valueAsPtr);
            return result;
        }

        //Constraints on IOPs
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_constraints_enforce(bool enforce);//default is false, i.e. disabled
        public static void ConstraintsEnforce(bool enforce){ igs_constraints_enforce(enforce); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_input_add_constraint(IntPtr name, IntPtr constraint);
        public static Result InputAddConstraint(string name, string constraint)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr constraintAsPtr = StringToUTF8Ptr(constraint);
            result = igs_input_add_constraint(nameAsPtr, constraintAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(constraintAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_output_add_constraint(IntPtr name, IntPtr constraint);
        public static Result OutputAddConstraint(string name, string constraint)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr constraintAsPtr = StringToUTF8Ptr(constraint);
            result = igs_output_add_constraint(nameAsPtr, constraintAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(constraintAsPtr);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_parameter_add_constraint(IntPtr name, IntPtr constraint);
        public static Result ParameterAddConstraint(string name, string constraint)
        {
            Result result = Result.Failure;
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr constraintAsPtr = StringToUTF8Ptr(constraint);
            result = igs_parameter_add_constraint(nameAsPtr, constraintAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(constraintAsPtr);
            return result;
        }

        //IOP descriptions
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_input_set_description(IntPtr name, IntPtr description);
        public static void InputSetDescription(string name, string description)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr descriptionAsPtr = StringToUTF8Ptr(description);
            igs_input_set_description(nameAsPtr, descriptionAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(descriptionAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_output_set_description(IntPtr name, IntPtr description);
        public static void OutputSetDescription(string name, string description)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr descriptionAsPtr = StringToUTF8Ptr(description);
            igs_output_set_description(nameAsPtr, descriptionAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(descriptionAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_parameter_set_description(IntPtr name, IntPtr description);
        public static void ParameterSetDescription(string name, string description)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            IntPtr descriptionAsPtr = StringToUTF8Ptr(description);
            igs_parameter_set_description(nameAsPtr, descriptionAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(descriptionAsPtr);
        }

        /*These two functions enable sending and receiving DATA on
         inputs/outputs by using zmsg_t structures. zmsg_t structures
         offer advanced functionalities for data serialization.
         More can be found here: http://czmq.zeromq.org/manual:zmsg */
        // No c# wrapping of zmsg_t structure exist.
        //FIXME: INGESCAPE_EXPORT igs_result_t igs_output_set_zmsg(const char *name, zmsg_t *msg);
        //FIXME: INGESCAPE_EXPORT igs_result_t igs_input_zmsg(const char *name, zmsg_t **msg); //msg is owned by caller

        //clear IOP data in memory without having to write the IOP
        //(relevant for IOPs with Data type only)
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_clear_input(IntPtr name);
        public static void ClearInput(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            igs_clear_input(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_clear_output(IntPtr name);
        public static void ClearOutput(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            igs_clear_output(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_clear_parameter(IntPtr name);
        public static void ClearParameter(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            igs_clear_parameter(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        // Observe writing to an IOP
        private static IopFunctionC _OnIOPCallback;
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void IopFunctionC(IopType iopType,
                                            IntPtr name,
                                            IopValueType valueType,
                                            IntPtr value,
                                            uint valueSize,
                                            IntPtr myData);
        public delegate void IopFunction(IopType iopType, string name, IopValueType valueType, object value, object myData);

        static void OnIOPCallback(IopType iopType,
                                IntPtr name, IopValueType valueType,
                                IntPtr value,
                                uint valueSize,
                                IntPtr myData)
        {
            GCHandle gCHandleData = GCHandle.FromIntPtr(myData);
            Tuple<IopFunction, object> tuple = (Tuple<IopFunction, object>)gCHandleData.Target;
            object data = tuple.Item2;
            IopFunction cSharpFunction = tuple.Item1;
            object newValue = null;
            switch (valueType)
            {
                case IopValueType.Bool:
                    newValue = PtrToBool(value);
                    break;
                case IopValueType.Data:
                    newValue = PtrToData(value, (int)valueSize);
                    break;
                case IopValueType.Double:
                    newValue = PtrToDouble(value);
                    break;
                case IopValueType.Impulsion:
                    break;
                case IopValueType.Integer:
                    newValue = PtrToInt(value);
                    break;
                case IopValueType.String:
                    newValue = PtrToStringFromUTF8(value);
                    break;
            }
            string decodedName = PtrToStringFromUTF8(name);
            cSharpFunction(iopType, decodedName, valueType, newValue, data);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observe_input(IntPtr name,
            [MarshalAs(UnmanagedType.FunctionPtr)] IopFunctionC cb,
            IntPtr myData);
        public static void ObserveInput(string inputName, IopFunction callback, object myData)
        {
            Tuple<IopFunction, object> tupleData = new Tuple<IopFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);

            if (_OnIOPCallback == null)
                _OnIOPCallback = OnIOPCallback;
            
            IntPtr nameAsPtr = StringToUTF8Ptr(inputName);
            igs_observe_input(nameAsPtr, _OnIOPCallback, data);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observe_output(IntPtr name, IopFunctionC cb, IntPtr myData);
        public static void ObserveOutput(string outputName, IopFunction callback, object myData)
        {
            Tuple<IopFunction, object> tupleData = new Tuple<IopFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);

            if (_OnIOPCallback == null)
                _OnIOPCallback = OnIOPCallback;

            IntPtr nameAsPtr = StringToUTF8Ptr(outputName);
            igs_observe_output(nameAsPtr, _OnIOPCallback, data);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observe_parameter(IntPtr name, IopFunctionC cb, IntPtr myData);
        public static void ObserveParameter(string ParameterName, IopFunction callback, object myData)
        {
            Tuple<IopFunction, object> tupleData = new Tuple<IopFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);

            if (_OnIOPCallback == null)
                _OnIOPCallback = OnIOPCallback;

            IntPtr nameAsPtr = StringToUTF8Ptr(ParameterName);
            igs_observe_parameter(nameAsPtr, _OnIOPCallback, data);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_output_mute(IntPtr name);
        public static void OutputMute(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            igs_output_mute(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_output_unmute(IntPtr name);
        public static void OutputUnmute(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            igs_output_unmute(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_output_is_muted(IntPtr name);
        public static bool OutputIsMuted(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            bool value = igs_output_is_muted(StringToUTF8Ptr(name));
            Marshal.FreeHGlobal(nameAsPtr);
            return value;
        }
        #endregion

        #region Mapping edition & inspection
        // Load / set / get mapping
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_mapping_load_str(IntPtr json_str);
        public static Result MappingLoadStr(string json_str)
        {
            IntPtr jsonAsPtr = StringToUTF8Ptr(json_str);
            Result res = igs_mapping_load_str(jsonAsPtr);
            Marshal.FreeHGlobal(jsonAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_mapping_load_file(IntPtr file_path);
        public static Result MappingLoadFile(string file_path)
        {
            IntPtr pathAsPtr = StringToUTF8Ptr(file_path);
            Result res = igs_mapping_load_file(pathAsPtr);
            Marshal.FreeHGlobal(pathAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_mapping_json();
        public static string MappingJson()
        {
            IntPtr ptr = igs_mapping_json();
            return (ptr == IntPtr.Zero) ? string.Empty : Marshal.PtrToStringAnsi(ptr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igs_mapping_count();
        public static uint MappingCount() { return igs_mapping_count(); }

        // Clear Mappings
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_clear_mappings();
        public static void ClearMappings() { igs_clear_mappings(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_clear_mappings_with_agent(IntPtr agentName);
        public static void ClearMappingsWithAgent(string agentName)
        {
            IntPtr ptrAgentName = StringToUTF8Ptr(agentName);
            igs_clear_mappings_with_agent(ptrAgentName);
            Marshal.FreeHGlobal(ptrAgentName);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong igs_mapping_add(IntPtr fromOurInput, IntPtr toAgent, IntPtr withOutput);
        public static ulong MappingAdd(string fromOurInput, string toAgent, string withOutput)
        {
            IntPtr fromOurInputAsPtr = StringToUTF8Ptr(fromOurInput);
            IntPtr toAgentAsPtr = StringToUTF8Ptr(toAgent);
            IntPtr withOutputAsPtr = StringToUTF8Ptr(withOutput);
            ulong id = igs_mapping_add(fromOurInputAsPtr, toAgentAsPtr, withOutputAsPtr);
            Marshal.FreeHGlobal(fromOurInputAsPtr);
            Marshal.FreeHGlobal(toAgentAsPtr);
            Marshal.FreeHGlobal(withOutputAsPtr);
            return id;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_mapping_remove_with_id(ulong theId);
        public static Result MappingRemoveWithId(ulong theId) { return igs_mapping_remove_with_id(theId); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_mapping_remove_with_name(IntPtr fromOurInput, IntPtr toAgent, IntPtr withOutput);
        public static Result MappingRemoveWithName(string fromOurInput, string toAgent, string withOutput)
        {
            IntPtr fromOurInputAsPtr = StringToUTF8Ptr(fromOurInput);
            IntPtr toAgentAsPtr = StringToUTF8Ptr(toAgent);
            IntPtr withOutputAsPtr = StringToUTF8Ptr(withOutput);
            Result res = igs_mapping_remove_with_name(fromOurInputAsPtr, toAgentAsPtr, withOutputAsPtr);
            Marshal.FreeHGlobal(fromOurInputAsPtr);
            Marshal.FreeHGlobal(toAgentAsPtr);
            Marshal.FreeHGlobal(withOutputAsPtr);
            return res;
        }

        // Edit our splits 
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igs_split_count();
        public static uint SplitCount() { return igs_split_count(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong igs_split_add(IntPtr fromOurInput, IntPtr toAgent, IntPtr withOutput);
        public static ulong SplitAdd(string fromOurInput, string toAgent, string withOutput)
        {
            IntPtr ptrFromOurInput = StringToUTF8Ptr(fromOurInput);
            IntPtr ptrToAgent = StringToUTF8Ptr(toAgent);
            IntPtr ptrWithOutput = StringToUTF8Ptr(withOutput);
            ulong result = igs_split_add(ptrFromOurInput, ptrToAgent, ptrWithOutput);
            Marshal.FreeHGlobal(ptrFromOurInput);
            Marshal.FreeHGlobal(ptrToAgent);
            Marshal.FreeHGlobal(ptrWithOutput);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_split_remove_with_id(ulong id);
        public static Result SplitRemoveWithId(ulong id) { return igs_split_remove_with_id(id); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_split_remove_with_name(IntPtr fromOurInput, IntPtr toAgent, IntPtr withOutput);
        public static Result SplitRemoveWithName(string fromOurInput, string toAgent, string withOutput)
        {
            IntPtr ptrFromOurInput = StringToUTF8Ptr(fromOurInput);
            IntPtr ptrToAgent = StringToUTF8Ptr(toAgent);
            IntPtr ptrWithOutput = StringToUTF8Ptr(withOutput);
            Result result = igs_split_remove_with_name(ptrFromOurInput, ptrToAgent, ptrWithOutput);
            Marshal.FreeHGlobal(ptrFromOurInput);
            Marshal.FreeHGlobal(ptrToAgent);
            Marshal.FreeHGlobal(ptrWithOutput);
            return result;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_mapping_set_outputs_request(bool notify);
        /// <summary>
        /// When mapping other agents, it is possible to request the
        /// mapped agents to send us their current output values
        /// through a private communication for our proper initialization.
        /// By default, this behavior is disabled. 
        /// </summary>
        public static void MappingSetOutputsRequest(bool notify) { igs_mapping_set_outputs_request(notify); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_mapping_outputs_request();
        public static bool MappingOutputsRequest() { return Convert.ToBoolean(igs_mapping_outputs_request()); }
        #endregion

        #region Services edition & inspection
        // Callback model to handle services received by our agent
        [StructLayout(LayoutKind.Explicit)]
        internal struct UnionServiceArgument
        {
            [FieldOffset(0)]
            public bool b;
            [FieldOffset(0)]
            public int i;
            [FieldOffset(0)]
            public double d;
            [FieldOffset(0)]
            public IntPtr c;
            [FieldOffset(0)]
            public IntPtr data;
        }

        // Service arguments are provided as a chained list
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        internal struct StructServiceArgument
        {
            public IntPtr name;
            public IopValueType type;
            public UnionServiceArgument union;
            public uint size;
            public IntPtr next;
        }

        // Arguments management
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void igs_service_args_add_int(ref IntPtr list, int value);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void igs_service_args_add_bool(ref IntPtr list, bool value);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void igs_service_args_add_double(ref IntPtr list, double value);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void igs_service_args_add_string(ref IntPtr list, IntPtr value);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void igs_service_args_add_data(ref IntPtr list, byte[] value, uint size);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void igs_service_args_destroy(ref IntPtr list);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_service_args_clone(IntPtr list);
        private static List<ServiceArgument> ServiceArgsClone(List<ServiceArgument> list)
        {
            List<ServiceArgument> newServiceArguments = new List<ServiceArgument>(list);
            return newServiceArguments;
        }

        // Call a service to another agent
        // requires to pass agent name or UUID, service name and a list of arguments
        // passed arguments list will be deallocated and destroyed
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_service_call(IntPtr agentNameOrUUID,
                                               IntPtr serviceName,
                                               ref IntPtr list, IntPtr token);

        // For a null list
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_service_call(IntPtr agentNameOrUUID,
                                               IntPtr serviceName,
                                               IntPtr list, IntPtr token);

        public static Result ServiceCall(string agentNameOrUUID, string serviceName, object[] arguments, string token = "")
        {
            IntPtr agentNameOrUUIDAsPtr = StringToUTF8Ptr(agentNameOrUUID);
            IntPtr serviceNameAsPtr = StringToUTF8Ptr(serviceName);
            IntPtr tokenAsPtr = StringToUTF8Ptr(token);
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
                            igs_service_args_add_bool(ref ptr, Convert.ToBoolean(argument));
                        else if (argument.GetType() == typeof(byte[]))
                        {
                            byte[] data = (byte[])argument;
                            igs_service_args_add_data(ref ptr, data, (uint)data.Length);
                        }
                        else if (argument.GetType() == typeof(double) || argument.GetType() == typeof(float))
                            igs_service_args_add_double(ref ptr, Convert.ToDouble(argument));
                        else if (argument.GetType() == typeof(int))
                            igs_service_args_add_int(ref ptr, Convert.ToInt32(argument));
                        else if (argument.GetType() == typeof(string))
                        {
                            IntPtr argAsPtr = StringToUTF8Ptr(Convert.ToString(argument));
                            igs_service_args_add_string(ref ptr, argAsPtr);
                            Marshal.FreeHGlobal(argAsPtr);
                        }
                    }
                    else
                    {
                        Error(string.Format("argument at {0} is null. Cannot call service {1}", i.ToString(), serviceName));
                        igs_service_args_destroy(ref ptr);
                        return Result.Failure;
                    }
                }
                success = igs_service_call(agentNameOrUUIDAsPtr, serviceNameAsPtr, ref ptr, tokenAsPtr);
                igs_service_args_destroy(ref ptr);
            }
            else
            {
                success = igs_service_call(agentNameOrUUIDAsPtr, serviceNameAsPtr, ptr, tokenAsPtr);
                igs_service_args_destroy(ref ptr);
            }
            Marshal.FreeHGlobal(agentNameOrUUIDAsPtr);
            Marshal.FreeHGlobal(serviceNameAsPtr);
            Marshal.FreeHGlobal(tokenAsPtr);
            return success;
        }

        private static ServiceFunctionC _OnServiceCallback;
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void ServiceFunctionC(IntPtr senderAgentName,
                                            IntPtr senderAgentUUID,
                                            IntPtr serviceName,
                                            IntPtr firstArgument,
                                            uint nbArgs,
                                            IntPtr token,
                                            IntPtr myData);
        public delegate void ServiceFunction(string senderAgentName, string senderAgentUUID, string serviceName, List<ServiceArgument> arguments, string token, object myData);

        static void OnServiceCallback(IntPtr senderAgentName,
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
            string serviceNameAsString = PtrToStringFromUTF8(serviceName);
            List<ServiceArgument> serviceArguments = ServiceArgumentsList(serviceNameAsString);
            cSharpFunction(PtrToStringFromUTF8(senderAgentName), PtrToStringFromUTF8(senderAgentUUID), serviceNameAsString, serviceArguments, PtrToStringFromUTF8(token), data);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_service_init(IntPtr name,
                                               [MarshalAs(UnmanagedType.FunctionPtr)] ServiceFunctionC cb,
                                               IntPtr myData);
        public static Result ServiceInit(string name, ServiceFunction callback, object myData)
        {
            Tuple<ServiceFunction, object> tupleData = new Tuple<ServiceFunction, object>(callback, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);

            if (_OnServiceCallback == null)
                _OnServiceCallback = OnServiceCallback;
            
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            Result res = igs_service_init(nameAsPtr, _OnServiceCallback, data);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_service_remove(IntPtr name);
        public static Result ServiceRemove(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            Result res = igs_service_remove(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_service_arg_add(IntPtr serviceName, IntPtr argName, IopValueType type);
        public static Result ServiceArgAdd(string serviceName, string argName, IopValueType type)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(serviceName);
            IntPtr argNameAsPtr = StringToUTF8Ptr(argName);
            Result res = igs_service_arg_add(nameAsPtr, argNameAsPtr, type);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(argNameAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_service_arg_remove(IntPtr serviceName, IntPtr argName);
        public static Result ServiceArgRemove(string serviceName, string argName)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(serviceName);
            IntPtr argNameAsPtr = StringToUTF8Ptr(argName);
            Result res = igs_service_arg_remove(nameAsPtr, argNameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(argNameAsPtr);
            return res;
        }

        // Introspection for services, arguments and replies
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igs_service_count();
        public static uint ServiceCount() { return igs_service_count(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_service_exists(IntPtr name);
        public static bool ServiceExists(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            bool res = igs_service_exists(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_service_list(ref uint nbOfElements);
        public static string[] ServiceList()
        {
            uint nbOfElements = 0;

            IntPtr intPtr = igs_service_list(ref nbOfElements);
            if (nbOfElements != 0)
            {
                IntPtr[] intPtrArray = new IntPtr[nbOfElements];
                Marshal.Copy(intPtr, intPtrArray, 0, (int)nbOfElements);
                string[] list = new string[nbOfElements];
                for (int i = 0; i < nbOfElements; i++)
                    list[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);

                igs_free_services_list(intPtr, nbOfElements);
                return list;
            }
            else return null;


        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void igs_free_services_list(IntPtr list, uint numberOfServices);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_service_args_first(IntPtr name);
        public static List<ServiceArgument> ServiceArgumentsList(string name)
        {
            IntPtr ptrArgument = igs_service_args_first(StringToUTF8Ptr(name));
            List<ServiceArgument> serviceArgumentsList = new List<ServiceArgument>();
            if (ptrArgument != null)
            {
                while (ptrArgument != IntPtr.Zero)
                {
                    // Marshals data from an unmanaged block of memory to a newly allocated managed object of the type specified by a generic type parameter.
                    StructServiceArgument structArgument = Marshal.PtrToStructure<StructServiceArgument>(ptrArgument);

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
                            value = PtrToStringFromUTF8(structArgument.union.c);
                            break;

                        case IopValueType.Data:
                            byte[] byteArray = new byte[structArgument.size];

                            // Copies data from an unmanaged memory pointer to a managed 8-bit unsigned integer array.
                            // Copy the content of the IntPtr to the byte array
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

                    ServiceArgument serviceArgument = new ServiceArgument(PtrToStringFromUTF8(structArgument.name), structArgument.type, value);
                    serviceArgumentsList.Add(serviceArgument);
                    ptrArgument = structArgument.next;
                }
                return serviceArgumentsList;
            }
            else
                return null;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint igs_service_args_count(IntPtr name);
        public static uint ServiceArgsCount(string name)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(name);
            uint res = igs_service_args_count(nameAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_service_arg_exists(IntPtr serviceName, IntPtr argName);
        public static bool ServiceArgExists(string serviceName, string argName)
        {
            IntPtr nameAsPtr = StringToUTF8Ptr(serviceName);
            IntPtr argAsPtr = StringToUTF8Ptr(argName);
            bool res = igs_service_arg_exists(nameAsPtr, argAsPtr);
            Marshal.FreeHGlobal(nameAsPtr);
            Marshal.FreeHGlobal(argAsPtr);
            return res;
        }
        #endregion

        #region  Timers 
        private static TimerFunctionC _OnTimer;
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void TimerFunctionC(int timerId, IntPtr myData);
        public delegate void TimerFunction(int timerId, object myData);

        static void OnTimer(int timerId, IntPtr myData)
        {
            GCHandle gCHandleData = GCHandle.FromIntPtr(myData);
            Tuple<TimerFunction, object> tuple = (Tuple<TimerFunction, object>)gCHandleData.Target;
            object data = tuple.Item2;
            TimerFunction cSharpFunction = tuple.Item1;
            cSharpFunction(timerId, data);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_timer_start(UIntPtr delay, UIntPtr times, TimerFunctionC cb, IntPtr myData);
        /// <summary>
        //Timers can be created to call code a certain number of times,
        //each time after a certain delay. 0 times means repeating forever.
        //Timers must be created after starting an agent.
        /// </summary>
        public static int TimerStart(uint delay, uint times, TimerFunction cbsharp, object myData)
        {
            if (cbsharp != null)
            {
                Tuple<TimerFunction, object> tupleData = new Tuple<TimerFunction, object>(cbsharp, myData);
                GCHandle gCHandle = GCHandle.Alloc(tupleData);
                IntPtr data = GCHandle.ToIntPtr(gCHandle);
                if (_OnTimer == null)
                    _OnTimer = OnTimer;
                
                return igs_timer_start((UIntPtr)delay, (UIntPtr)times, _OnTimer, data);
            }
            else
                return -1;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_timer_stop(int timerId);
        public static void TimerStop(int timerId) { igs_timer_stop(timerId); }
        #endregion

        #region BROKERS VS. SELF-DISCOVERY
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_broker_add(IntPtr brokerEndpoint);
        /// <summary>
        /// igs_start_with_device and igs_start_with_ip enable the agents to self-discover
        /// using UDP broadcast messages on the passed port.UDP broadcast messages can
        /// be blocked on some networks and can make things complex on networks with
        /// sub-networks.
        /// That is why ingescape also supports the use of brokers to relay discovery
        /// using TCP connections.Any agent can be a broker and agents using brokers
        /// simply have to use a list of broker endpoints.One broker is enough but
        /// several brokers can be set for robustness.
        /// 
        /// For clarity, it is better if brokers are well identified on your platform,
        /// started before any agent, and serve only as brokers.But any other architecture
        /// is permitted and brokers can be restarted at any time.
        /// 
        /// Endpoints have the form tcp://ip_address:port
        /// • igs_brokers_add is used to add brokers to connect to.Add
        /// as many brokers as you want.At least one declared broker is necessary to
        /// use igs_start_with_brokers. Use igs_clear_brokers to remove all the current
        /// brokers.
        ///  • The endpoint in igs_broker_set_endpoint is the broker address we should be reached
        /// at as a broker if we want to be one.Using igs_broker_set_endpoint makes us a broker
        /// when starting.
        /// • The endpoint in igs_broker_set_advertized_endpoint replaces the one declared in
        /// igs_start_with_brokers for the registration to the brokers.This function enables
        /// passing through NAT and using a public address.Attention: this public address
        ///  shall make sense to all the agents that will connect to us, independently from
        /// their local network.
        /// • Our agent endpoint in igs_start_with_brokers gives the address and port our
        /// agent can be reached at.This endpoint must be valid in the actual network
        /// configuration.*/
        /// </summary>
        public static Result BrokerAdd(string brokerEndpoint)
        {
            IntPtr brokerEndpointAsPtr = StringToUTF8Ptr(brokerEndpoint);
            Result res = igs_broker_add(brokerEndpointAsPtr);
            Marshal.FreeHGlobal(brokerEndpointAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_clear_brokers();
        public static void ClearBrokers() { igs_clear_brokers(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_broker_enable_with_endpoint(IntPtr ourBrokerEndpoint);
        public static void BrokerEnableWithEndpoint(string ourBrokerEndpoint)
        {
            IntPtr ourBrokerEndpointAsPtr = StringToUTF8Ptr(ourBrokerEndpoint);
            igs_broker_enable_with_endpoint(ourBrokerEndpointAsPtr);
            Marshal.FreeHGlobal(ourBrokerEndpointAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_broker_set_advertized_endpoint(IntPtr advertisedEndpoint);
        public static void BrokerSetAdvertizedEndpoint(string advertisedEndpoint)
        {
            IntPtr advertisedEndpointAsPtr = StringToUTF8Ptr(advertisedEndpoint);
            igs_broker_set_advertized_endpoint(advertisedEndpointAsPtr);
            Marshal.FreeHGlobal(advertisedEndpointAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_start_with_brokers(IntPtr agentEndpoint);
        public static Result StartWithBrokers(string agentEndpoint)
        {
            IntPtr agentEndpointAsPtr = StringToUTF8Ptr(agentEndpoint);
            Result res = igs_start_with_brokers(agentEndpointAsPtr);
            Marshal.FreeHGlobal(agentEndpointAsPtr);
            return res;
        }
        #endregion

        #region  Security : identity, end-to-end encryption        
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_enable_security(IntPtr privateKeyFile, IntPtr publicKeysDirectory);
        /// <summary>
        /// Security is about authentification of other agents and encrypted communications.
        ///  Both are offered by Ingescape using a public/private keys mechanism relying on ZeroMQ.
        ///  Security is activated optionally.
        ///  • If public/private keys are generated on the fly, one obtains the same protection as TLS
        ///  for HTTPS communications.Thirdparties cannot steal identities and communications are
        ///  encrypted end-to-end.But any Ingescape agent with security enabled can join a platform.
        ///  • If public/private keys are stored locally by each agent, no thirdparty can join a platform
        ///  without having a public key that is well-known by the other agents.This is safer but requires
        /// securing and synchronizing local files with each agent accessing its private key and public
        /// keys of other agents.
        /// 
        /// Security is enabled by calling igs_enable_security.
        ///  • If privateKey is null, our private key is generated on the fly and any agent with
        ///  security enabled will be able to connect, publicKeysDirectory will be ignored.
        ///  • If privateKey is NOT null, private key at privateKey path will be used and only
        ///  agents whose public keys are in publicKeysDirectory will be able to connect.
        ///  NB: if privateKey is NOT null and publicKeysDirectory is null or does not exist,
        ///  security will not be enabled and our agent will not start.
        /// </summary>
        public static Result EnableSecurity(string privateKeyFile, string publicKeysDirectory)
        {
            IntPtr privateKeyFileAsPtr = StringToUTF8Ptr(privateKeyFile);
            IntPtr publicKeysDirectoryAsPtr = StringToUTF8Ptr(publicKeysDirectory);
            Result res = igs_enable_security(privateKeyFileAsPtr, publicKeysDirectoryAsPtr);
            Marshal.FreeHGlobal(privateKeyFileAsPtr);
            Marshal.FreeHGlobal(publicKeysDirectoryAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_disable_security();
        public static void DisableSecurity() { igs_disable_security(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_broker_add_secure(IntPtr brokerEndpoint, IntPtr publicKeyPath);
        public static Result BrokerAddSecure(string brokerEndpoint, string publicKeyPath)
        {
            IntPtr brokerEndpointAsPtr = StringToUTF8Ptr(brokerEndpoint);
            IntPtr publicKeyPathAsPtr = StringToUTF8Ptr(publicKeyPath);
            Result res = igs_broker_add_secure(brokerEndpointAsPtr, publicKeyPathAsPtr);
            Marshal.FreeHGlobal(brokerEndpointAsPtr);
            Marshal.FreeHGlobal(publicKeyPathAsPtr);
            return res;
        }
        #endregion

        #region Elections and leadership between agents 
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_election_join(IntPtr electionName);
        /// <summary>
        /// Create named contests between agents and designate a leader, as soon as they
        /// are two or more.
        ///  • AgentWonElection means that the election is over and this agent has WON
        ///  • AgentLostElection means that the election is over and this agent has LOST
        ///  • When only one agent participates in an election, the election does not happen.
        ///  • When only one agent remains for an election, the election does not happen.
        /// At startup, this means that developers must either start their agents as
        /// 
        /// leaders or wait a reasonable amount of time for an election to happen.
        /// 
        /// During runtime, this means that developers shall rely on AgentExited
        /// events to check if they suddenly are alone in an election and thus shall
        /// become leaders.
        /// </summary>
        public static Result ElectionJoin(string electionName)
        {
            IntPtr electionNameAsPtr = StringToUTF8Ptr(electionName);
            Result res = igs_election_join(electionNameAsPtr);
            Marshal.FreeHGlobal(electionNameAsPtr);
            return res;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern Result igs_election_leave(IntPtr electionName);
        public static Result ElectionLeave(string electionName)
        {
            IntPtr electionNameAsPtr = StringToUTF8Ptr(electionName);
            Result res = igs_election_leave(StringToUTF8Ptr(electionName));
            Marshal.FreeHGlobal(electionNameAsPtr);
            return res;
        }
        #endregion

        #region Administration, logging, configuration and utilities

        #region LOG ALIASES
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log(LogLevel logLevel, IntPtr function, IntPtr message);
        /// <summary>
        /// Logs policy
        ///  - Fatal : Events that force application termination.
        ///  - Error : Events that are Fatal to the current operation but not the whole application.
        ///  - warning : Events that can potentially cause application anomalies but that can be recovered automatically(by circumventing or retrying).
        ///  - Info : Generally useful information to Log(service start/Stop, configuration assumptions, etc.).
        ///  - Debug : Information that is diagnostically helpful to people more than just developers but useless for system monitoring.
        ///  - Trace : Information about parts of functions, for detailed diagnostic only.
        /// </summary>
        public static void Log(LogLevel logLevel, string function, string message) { igs_log(logLevel, StringToUTF8Ptr(function), StringToUTF8Ptr(message)); }

        public static void Trace(string message, [CallerMemberName] string memberName = "") { Log(LogLevel.LogTrace, memberName, message); }
        public static void Debug(string message, [CallerMemberName] string memberName = "") { Log(LogLevel.LogDebug, memberName, message); }
        public static void Info(string message, [CallerMemberName] string memberName = "") { Log(LogLevel.LogInfo, memberName, message); }
        public static void Warn(string message, [CallerMemberName] string memberName = "") { Log(LogLevel.LogWarn, memberName, message); }
        public static void Error(string message, [CallerMemberName] string memberName = "") { Log(LogLevel.LogError, memberName, message); }
        public static void Fatal(string message, [CallerMemberName] string memberName = "") { Log(LogLevel.LogFatal, memberName, message); }
        #endregion

        #region PROTOCOL AND REGION
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_version();
        public static int Version() { return igs_version(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_protocol();
        public static int Protocol() { return igs_protocol(); }
        #endregion

        #region COMMAND LINE
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_set_command_line(IntPtr line);
        /// <summary>
        /// Command line for the agent can be passed here for inclusion in the
        /// agent's headers. If not used, header is initialized with exec path.
        /// </summary>
        public static void SetCommandLine(string line)
        {
            IntPtr lineAsPtr = StringToUTF8Ptr(line);
            igs_set_command_line(lineAsPtr);
            Marshal.FreeHGlobal(lineAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_set_command_line_from_args(int argc, IntPtr argv); //first element is replaced by absolute exec path on UNIX
        /// <summary>
        /// Command line for the agent can be passed here for inclusion in the
        /// agent's headers. If not used, header is initialized with exec path.
        /// </summary>
        public static void SetCommandLineFromArgs(string[] argv)
        {
            int argc = argv.Length;
            IntPtr argvAsPtr = Marshal.AllocCoTaskMem(argc * IntPtr.Size);
            IntPtr[] argvArray = new IntPtr[argv.Length];

            int index = 0;
            foreach (string arg in argv)
            {
                IntPtr argPtr = StringToUTF8Ptr(arg);
                argvArray[index] = argPtr;
                index++;
            }
            Marshal.Copy(argvArray, 0, argvAsPtr, argc);
            igs_set_command_line_from_args(argc, argvAsPtr);

            for (int i = 0; i < argc; i++)
            {
                IntPtr argPtr = argvArray[i];
                Marshal.FreeHGlobal(argPtr);
            }

            Marshal.FreeCoTaskMem(argvAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_command_line();
        public static string CommandLine()
        {
            IntPtr lineAsPtr = igs_command_line();
            string result = PtrToStringFromUTF8(lineAsPtr);
            return result;
        }
        #endregion

        #region LOGS MANAGEMENT
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_set_console(bool verbose);
        public static void LogSetConsole(bool verbose) { igs_log_set_console(verbose); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_log_console();
        public static bool LogConsole() { return igs_log_console(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_log_console_color();
        public static bool LogConsoleColor() { return igs_log_console_color(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_set_console_color(bool useColor);
        public static void LogSetConsoleColor(bool useColor) { igs_log_set_console_color(useColor); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_set_console_level(LogLevel level);
        public static void LogSetConsoleLevel(LogLevel level) { igs_log_set_console_level(level); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern LogLevel igs_log_console_level();
        public static LogLevel LogConsoleLevel() { return igs_log_console_level(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_set_stream(bool useLogStream);
        public static void LogSetStream(bool useLogStream) { igs_log_set_stream(useLogStream); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_log_stream();
        public static bool LogStream() { return igs_log_stream(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_set_file(bool useLogFile, IntPtr path);
        public static void LogSetFile(bool useLogFile, string path = null){ igs_log_set_file(useLogFile, StringToUTF8Ptr(path)); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_set_file_level(LogLevel level);
        public static void LogSetFileLevel(LogLevel level){ igs_log_set_file_level(level); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_set_file_max_line_length(uint size);
        public static void LogSetFileMaxLineLength(uint size){ igs_log_set_file_max_line_length(size); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_log_file();
        public static bool LogFile() { return igs_log_file(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_set_file_path(string path);
        public static void LogSetFilePath(string path) { igs_log_set_file_path(path); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_log_file_path();
        public static string LogFilePath()
        {
            IntPtr ptr = igs_log_file_path();
            return (ptr == IntPtr.Zero) ? string.Empty : Marshal.PtrToStringAnsi(ptr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_include_data(bool enable);
        public static void LogIncludeData(bool enable) { igs_log_include_data(enable); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log_include_services(bool enable);
        public static void LogIncludeServices(bool enable) { igs_log_include_services(enable); }
        #endregion

        #region DEFINITION & MAPPING FILE MANAGEMENT
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_definition_set_path(IntPtr path);
        public static void DefinitionSetPath(string path)
        {
            IntPtr ptrPath = StringToUTF8Ptr(path);
            igs_definition_set_path(ptrPath);
            Marshal.FreeHGlobal(ptrPath);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_definition_save();
        public static void DefinitionSave() { igs_definition_save(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_mapping_set_path(IntPtr path);
        public static void MappingSetPath(string path)
        {
            IntPtr ptrPath = StringToUTF8Ptr(path);
            igs_mapping_set_path(ptrPath);
            Marshal.FreeHGlobal(ptrPath);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_mapping_save();
        public static void MappingSave() { igs_mapping_save(); }
        #endregion

        #region ADVANCED TRANSPORTS
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_set_ipc(bool allow);
        /// <summary>
        /// Ingescape automatically detects agents on the same computer
        /// and then uses optimized inter-process communication protocols
        /// depending on the operating system.
        /// On Microsoft Windows systems, the loopback is used.
        /// Advanced transports are allowed by default and can be disabled
        /// </summary>
        public static void SetIpc(bool allow) { igs_set_ipc(allow); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_has_ipc();
        public static bool HasIpc() { return igs_has_ipc(); }

        /*FIXME:
        #if defined (__UNIX__)
        //set IPC folder path on UNIX systems (default is /tmp/ingescape/)
        INGESCAPE_EXPORT void igs_set_ipc_dir(const char *path);
        INGESCAPE_EXPORT const char * igs_ipc_dir(void);
        #endif*/
        #endregion

        #region NETWORK DEVICES
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_net_devices_list(ref int nb);
        public static string[] NetDevicesList()
        {
            int nb = 0;
            IntPtr ptrDevices = igs_net_devices_list(ref nb);
            IntPtr[] ptrArrayOfDevices = new IntPtr[nb];
            Marshal.Copy(ptrDevices, ptrArrayOfDevices, 0, nb);
            string[] devicesArray = new string[nb];
            for (int i = 0; i < nb; i++)
            {
                string isoString = PtrToStringFromISO(ptrArrayOfDevices[i]);
                devicesArray[i] = isoString;
            }

            igs_free_net_devices_list(ptrDevices, nb);
            return devicesArray;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_free_net_devices_list(IntPtr devices, int nb);

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_net_addresses_list(ref int nb);
        public static string[] NetAddressesList()
        {
            int nb = 0;
            IntPtr ptrAddresses = igs_net_addresses_list(ref nb);
            IntPtr[] ptrArrayOfAddresses = new IntPtr[nb];
            Marshal.Copy(ptrAddresses, ptrArrayOfAddresses, 0, nb);
            string[] addressesArray = new string[nb];
            for (int i = 0; i < nb; i++)
            {
                addressesArray[i] = Marshal.PtrToStringAnsi(ptrArrayOfAddresses[i]);
            }

            igs_free_net_addresses_list(ptrAddresses, nb);
            return addressesArray;
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_free_net_addresses_list(IntPtr addresses, int nb);
        #endregion

        #region NETWORK CONFIGURATION
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_net_set_publishing_port(uint port);
        public static void NetSetPublishingPort(uint port) { igs_net_set_publishing_port(port); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_net_set_log_stream_port(uint port);
        public static void NetSetLogStreamPort(uint port) { igs_net_set_log_stream_port(port); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_net_set_discovery_interval(uint port); //in milliseconds
        public static void NetSetDiscoveryInterval(uint port) { igs_net_set_discovery_interval(port); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_net_set_timeout(uint port);
        public static void NetSetTimeout(uint port) { igs_net_set_timeout(port); }               
        
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_net_raise_sockets_limit();
        /// <summary>
        //UNIX only, to be called before any ingescape or ZeroMQ activity
        /// </summary>
        public static void NetRaiseSocketsLimit() { igs_net_raise_sockets_limit(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_net_set_high_water_marks(int hwmValue);
        /// <summary>
        //Set high water marks (HWM) for the publish/subscribe sockets.
        //Setting HWM to 0 means that they are disabled.
        public static void NetSetHighWaterMarks(int hwmValue) { igs_net_set_high_water_marks(hwmValue); }
        #endregion

        #region PERFORMANCE CHECK
        //sends number of messages with defined size and displays performance
        //information when finished (information displayed as INFO-level Log)

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_net_performance_check(IntPtr peerId, uint msgSize, uint nbOfMsg);
        public static void NetPerformanceCheck(string peerId, uint msgSize, uint nbOfMsg)
        {
            IntPtr peerIdAsPtr = StringToUTF8Ptr(peerId);
            igs_net_performance_check(peerIdAsPtr, msgSize, nbOfMsg);
            Marshal.FreeHGlobal(peerIdAsPtr);
        }
        #endregion

        #region NETWORK MONITORING
        //Ingescape provides an integrated ObserveMonitor to detect events relative to the network.
        //NB: once igs_monitor_start has been called, igs_monitor_stop must be
        //called to actually Stop the ObserveMonitor. If not stopped, it may cause an Error when
        //an agent terminates.

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_monitor_start(uint period);//in milliseconds
        public static void MonitorStart(uint period) { igs_monitor_start(period); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_monitor_start_with_network(uint period,
                                                   IntPtr networkDevice,
                                                   uint port);
        public static void MonitorStartWithNetwork(uint period, string networkDevice, uint port)
        {
            IntPtr networkDeviceAsPtr = StringToUTF8Ptr(networkDevice);
            igs_monitor_start_with_network(period, networkDeviceAsPtr, port);
            Marshal.FreeHGlobal(networkDeviceAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_monitor_stop();
        public static void MonitorStop() { igs_monitor_stop(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool igs_monitor_is_running();
        public static bool MonitorIsRunning() { return igs_monitor_is_running(); }

        /* When the ObserveMonitor is started and igs_monitor_set_start_stop is set to true :
         - IP change will cause the agent to restart on the new IP (same device, same port)
         - Network device disappearance will cause the agent to Stop. Agent will restart when device is back.*/
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_monitor_set_start_stop(bool flag);
        public static void MonitorSetStartStop(bool flag) { igs_monitor_set_start_stop(flag); }

        private static MonitorFunctionC _OnMonitorCallback;
        private delegate void MonitorFunctionC(MonitorEvent monitorEvent,
                                    IntPtr device,
                                    IntPtr ipAddress,
                                    IntPtr myData);
        public delegate void MonitorFunction(MonitorEvent monitorEvent,
                                    string device,
                                    string ipAddress,
                                    object myData);

        static void OnMonitorCallback(MonitorEvent monitorEvent,
                                    IntPtr device,
                                    IntPtr ipAddress,
                                    IntPtr myData)
        {
            GCHandle gCHandleData = GCHandle.FromIntPtr(myData);
            Tuple<MonitorFunction, object> tuple = (Tuple<MonitorFunction, object>)gCHandleData.Target;
            object data = tuple.Item2;
            MonitorFunction cSharpFunction = tuple.Item1;
            cSharpFunction(monitorEvent, PtrToStringFromUTF8(device), PtrToStringFromUTF8(ipAddress), data);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observe_monitor(MonitorFunctionC cb, IntPtr myData);
        public static void ObserveMonitor(MonitorFunction cbsharp, object myData)
        {
            Tuple<MonitorFunction, object> tupleData = new Tuple<MonitorFunction, object>(cbsharp, myData);
            GCHandle gCHandle = GCHandle.Alloc(tupleData);
            IntPtr data = GCHandle.ToIntPtr(gCHandle);
            if (_OnMonitorCallback == null)
                _OnMonitorCallback = OnMonitorCallback;
            igs_observe_monitor(_OnMonitorCallback, data);
        }
        #endregion

        #region  CLEAN CONTEXT
        /// <summary>
        /// Use this function when you absolutely need to clean all the Ingescape content
        ///and you cannot Stop your application to do so.This function SHALL NOT be used
        ///in production environments.
        /// </summary>
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_clear_context();
        public static void ClearContext() { igs_clear_context(); }

        #endregion 

        #region LOGS REPLAY
        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_replay_init(IntPtr logFilePath, uint speed, IntPtr startTime,
                           bool waitForStart, uint replayMode, IntPtr agent);

        ///<summary>Ingescape logs contain all the necessary information for an agent to replay its changes for inputs, outputs, parameters and calls.
        ///ReplayTerminate cleans the thread and requires calling igs_replay_init again.
        ///Replay thread is cleaned automatically also when the Log file has been read completely.</summary>
        ///<param name="logFilePath">path to the Log file to be read.</param>
        ///<param name="speed">replay speed. Default is zero, meaning as fast as possible.</param>
        ///<param name="startTime">with format hh:mm::s, specifies the time when speed shall be used. Replay as fast as possible before that.</param>
        ///<param name="waitForStart">waits for a call to igs_replay_start before starting the replay. Default is false.</param>
        ///<param name="replayMode">a boolean composition of ReplayMode value to decide what shall be replayed.If mode is zero, all IOP and calls are replayed.</param>
        ///<param name="agent">an OPTIONAL agent name serving as filter when the logs contain activity for multiple agents.</param>   
        public static void ReplayInit(string logFilePath, uint speed, string startTime, bool waitForStart, ReplayMode replayMode, string agent)
        {
            IntPtr logFilePathAsPtr = StringToUTF8Ptr(logFilePath);
            IntPtr startTimeAsPtr = StringToUTF8Ptr(startTime);
            IntPtr agentAsPtr = StringToUTF8Ptr(agent);

            igs_replay_init(logFilePathAsPtr, speed, startTimeAsPtr, waitForStart, Convert.ToUInt32(replayMode), agentAsPtr);
            Marshal.FreeHGlobal(logFilePathAsPtr);
            Marshal.FreeHGlobal(startTimeAsPtr);
            Marshal.FreeHGlobal(agentAsPtr);
        }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_replay_start();
        public static void ReplayStart() { igs_replay_start(); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_replay_pause(bool pause);
        public static void ReplayPause(bool pause) { igs_replay_pause(pause); }

        [DllImport(ingescapeDLLPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_replay_terminate();
        public static void ReplayTerminate() { igs_replay_terminate(); }
        #endregion
        #endregion
    }
}
