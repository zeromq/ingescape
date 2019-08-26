using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace Ingescape
{
    #region Enums

    public enum iop_t { IGS_INPUT_T = 1, IGS_OUTPUT_T, IGS_PARAMETER_T };
    public enum iopType_t { IGS_INTEGER_T = 1, IGS_DOUBLE_T, IGS_STRING_T, IGS_BOOL_T, IGS_IMPULSION_T, IGS_DATA_T };
    public enum igs_logLevel_t { IGS_LOG_TRACE = 0, IGS_LOG_DEBUG, IGS_LOG_INFO, IGS_LOG_WARN, IGS_LOG_ERROR, IGS_LOG_FATAL };
    public enum igs_license_limit_t { IGS_LICENSE_TIMEOUT = 0, IGS_LICENSE_TOO_MANY_AGENTS, IGS_LICENSE_TOO_MANY_IOPS };

    #endregion

    #region Callbacks

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void igs_observeCallback(iop_t iopType,
                                        [MarshalAs(UnmanagedType.LPStr)] string name,
                                        iopType_t valueType,
                                        IntPtr value,
                                        int valueSize,
                                        IntPtr myData);


    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void igs_forcedStopCallback(IntPtr myData);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void igs_muteCallback(bool isMuted, IntPtr myData);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void igs_freezeCallback(bool isPaused, IntPtr myData);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void igs_licenseCallback(igs_license_limit_t limit, IntPtr myData);

    #endregion

    public class Igs
    {
        #region Path to library C IngeScape

        //Librairie x86 debug
        //private const string ingescapeDLLPath = "C:\\ingescape\\libs\\debug\\ingescape.dll";

        // Library 32 bits (x86)
        //private const string ingescapeDLLPath = "C:\\Program Files (x86)\\ingescape\\lib\\ingescape.dll";

        // Library 64 bits
        private const string ingescapeDLLPath = "C:\\Program Files\\ingescape\\lib\\ingescape.dll";

        // Library MAC OS
        //private const string ingescapeDLLPath = "/usr/local/lib/libingescape.dylib";

        // Library iOS
        //private const string ingescapeDLLPath = "__Internal";

        #endregion

        //////////////////////////////////////////////////
        // Initialization and control

        //start & stop the agent
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_startWithDevice([MarshalAs(UnmanagedType.LPStr)]  string networkDevice, int port);
        public static int startWithDevice(string networkDevice, int port) { return igs_startWithDevice(networkDevice, port); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_startWithIP([MarshalAs(UnmanagedType.LPStr)]  string ipAddress, int port);
        public static int startWithIP(string ipAddress, int port) { return igs_startWithIP(ipAddress, port); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_stop();
        public static int stop() { return igs_stop(); }

        //There are four non-exclusive ways to stop the execution of an ingescape agent:
        //1- calling igs_stop from the hosting app's threads reacting on user actions or external events
        //2- handling SIGINT in the hosting app to call igs_stop and stop the rest of the app properly
        //3- monitoring the status of igs_Interrupted in the hosting app
        //4- using an igs_forcedStopCallback (see below) and calling code ON THE MAIN APP THREAD from it
        //In any case, igs_stop MUST NEVER BE CALLED directly from any ingeScape callback, as it would cause a thread deadlock.
        public static bool interrupted;
        //register a callback when the agent is forced to stop by the ingescape platform
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observeForcedStop([MarshalAs(UnmanagedType.FunctionPtr)] igs_forcedStopCallback cb,
                    IntPtr myData);
        public static void observeForcedStop(igs_forcedStopCallback cb, IntPtr myData) { igs_observeForcedStop(cb, myData); }

        //terminate the agent with trigger of SIGINT and call to the registered igs_forcedStopCallbacks
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_die();
        public static void die() { igs_die(); }

        //agent name set and get
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_setAgentName([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int setAgentName(string name) { return igs_setAgentName(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getAgentName();
        public static string getAgentName()
        {
            string agentName = "";
            IntPtr ptr = igs_getAgentName();
            agentName = Marshal.PtrToStringAnsi(ptr);
            return agentName;
        }

        //control agent state
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setAgentState([MarshalAs(UnmanagedType.LPStr)]  string state);
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getAgentState();
        public static string getAgentState()
        {
            string str = "";
            IntPtr ptr = igs_getAgentState();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }

        //mute the agent ouputs
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_mute();
        public static int mute() { return igs_mute(); }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_unmute();
        public static int unmute() { return igs_unmute(); }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_isMuted();
        public static bool isMuted() { return igs_isMuted(); }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observeMute([MarshalAs(UnmanagedType.FunctionPtr)] igs_muteCallback cb,
                    IntPtr myData);
        public static void observeMute(igs_muteCallback cb, IntPtr myData) { igs_observeMute(cb, myData); }

        //freeze and unfreeze the agent
        //When freezed, agent will not send anything on its outputs and
        //its inputs are not reactive to external data.
        //NB: the internal semantics of freeze and unfreeze for a given agent
        //are up to the developer and can be controlled using callbacks and igs_observeFreeze
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_freeze();
        public static int freeze() { return igs_freeze(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_isFrozen();
        public static bool isFrozen() { return igs_isFrozen(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_unfreeze();
        public static int unfreeze() { return igs_unfreeze(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_observeFreeze(igs_freezeCallback cb, IntPtr myData);
        public static int observeFreeze(igs_freezeCallback cb, IntPtr myData) { return igs_observeFreeze(cb, myData); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setCanBeFrozen(bool canBeFrozen);
        public static void setCanBeFrozen(bool canBeFrozen) { igs_setCanBeFrozen(canBeFrozen); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_canBeFrozen();
        public static bool canBeFrozen() { return igs_canBeFrozen(); }

        //////////////////////////////////////////////////
        //IOP Model : Inputs, Outputs and Parameters read/write/check/observe/mute

        //read IOP using void*
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readInput(string name, IntPtr[] value, ref int size);
        public static int readInput(string name, IntPtr[] value, ref int size) { return igs_readInput(name, value, ref size); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readOutput(string name, IntPtr[] value, ref int size);
        public static int readOutput(string name, IntPtr[] value, ref int size) { return igs_readOutput(name, value, ref size); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readParameter(string name, IntPtr[] value, ref int size);
        public static int readParameter(string name, IntPtr[] value, ref int size){ return igs_readParameter(name, value, ref size); }
    
        //read per type
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_readInputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static bool readInputAsBool(string name) { return igs_readInputAsBool(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readInputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int readInputAsInt(string name) { return igs_readInputAsInt(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igs_readInputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static double readInputAsDouble(string name) { return igs_readInputAsDouble(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_readInputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static string readInputAsString(string name) {
            string str = "";
            IntPtr ptr = igs_readInputAsString(name);
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, IntPtr[] data, ref int size);
        public static int readInputAsData(string name, ref byte[] data)
        {
            //Initialization
            int sizeRead = 0;
            IntPtr[] intPtrArray = new IntPtr[1];

            //Read output data
            int ret = igs_readInputAsData(name, intPtrArray, ref sizeRead);

            //Copy the content of the IntPtr to byte array
            data = new byte[sizeRead];
            Marshal.Copy(intPtrArray[0], data, 0, sizeRead);

            return ret;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_readOutputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static bool readOutputAsBool(string name) { return igs_readOutputAsBool(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readOutputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int readOutputAsInt(string name) { return igs_readOutputAsInt(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igs_readOutputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static double readOutputAsDouble(string name) { return igs_readOutputAsDouble(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_readOutputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static string readOutputAsString(string name)
        {
            string str = "";
            IntPtr ptr = igs_readOutputAsString(name);
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, IntPtr[] data, ref int size);
        public static int readOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, ref byte[] data)
        {
            //Initialization
            int sizeRead = 0;
            IntPtr[] intPtrArray = new IntPtr[1];

            //Read output data
            int ret = igs_readOutputAsData(name, intPtrArray, ref sizeRead);

            //IntPtr to byte array
            data = new byte[sizeRead];
            Marshal.Copy(intPtrArray[0], data, 0, sizeRead);

            return ret;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_readParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static bool readParameterAsBool(string name) { return igs_readParameterAsBool(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int readParameterAsInt(string name) { return igs_readParameterAsInt(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern double igs_readParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static double readParameterAsDouble(string name) { return igs_readParameterAsDouble(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_readParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static string readParameterAsString(string name)
        {
            string str = "";
            IntPtr ptr = igs_readParameterAsString(name);
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_readParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, IntPtr[] data, ref int size);
        public static int readParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, ref byte[] data)
        {
            //Initialization
            int sizeRead = 0;
            IntPtr[] intPtrArray = new IntPtr[1];

            //Read output data
            int ret = igs_readParameterAsData(name, intPtrArray, ref sizeRead);

            //IntPtr to byte array
            data = new byte[sizeRead];
            Marshal.Copy(intPtrArray[0], data, 0, sizeRead);

            return ret;
        }

        //write per type
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeInputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        public static int writeInputAsBool(string name, bool value) { return igs_writeInputAsBool(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeInputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        public static int writeInputAsInt(string name, int value) { return igs_writeInputAsInt(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeInputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        public static int writeInputAsDouble(string name, double value) { return igs_writeInputAsDouble(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeInputAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        public static int writeInputAsString(string name, string value) { return igs_writeInputAsString(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeInputAsImpulsion([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int writeInputAsImpulsion(string name) { return igs_writeInputAsImpulsion(name);  }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, byte[] value, int size);
        public static int writeInputAsData(string name, byte[] value, int size) { return igs_writeInputAsData(name, value, size);  }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeOutputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        public static int writeOutputAsBool(string name, bool value) { return igs_writeOutputAsBool(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeOutputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        public static int writeOutputAsInt(string name, int value) { return igs_writeOutputAsInt(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeOutputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        public static int writeOutputAsDouble(string name, double value) { return igs_writeOutputAsDouble(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeOutputAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        public static int writeOutputAsString(string name, string value) { return igs_writeOutputAsString(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeOutputAsImpulsion([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int writeOutputAsImpulsion(string name) { return igs_writeOutputAsImpulsion(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, byte[] value, int size);
        public static int writeOutputAsData(string name, byte[] value, int size) { return igs_writeOutputAsData(name, value, size); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        public static int writeParameterAsBool(string name, bool value) { return igs_writeParameterAsBool(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        public static int writeParameterAsInt(string name, int value) { return igs_writeParameterAsInt(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        public static int writeParameterAsDouble(string name, double value) { return igs_writeParameterAsDouble(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        public static int writeParameterAsString(string name, string value) { return igs_writeParameterAsString(name, value); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_writeParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, byte[] value, int size);
        public static int writeParameterAsData(string name, byte[] value, int size) { return igs_writeParameterAsData(name, value, size); }

        //clear IOP data in memory without having to write the IOP
        //(relevant for IOPs with IGS_DATA_T type only)
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_clearDataForInput(string name);
        public static void clearDataForInput(string name) { igs_clearDataForInput(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_clearDataForOutput(string name);
        public static void clearDataForOutput(string name) { igs_clearDataForOutput(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_clearDataForParameter(string name);
        public static void clearDataForParameter(string name) { igs_clearDataForParameter(name); }

        //observe writing to an IOP
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observeInput([MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.FunctionPtr)] igs_observeCallback cb,
            IntPtr myData);
        public static void observeInput(string name, igs_observeCallback cb, IntPtr myData) { igs_observeInput(name, cb, myData); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_observeOutput([MarshalAs(UnmanagedType.LPStr)]  string name, 
            igs_observeCallback cb,
            IntPtr myData);
        public static int observeOutput(string name, igs_observeCallback cb, IntPtr myData) { return igs_observeOutput(name, cb, myData); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_observeParameter([MarshalAs(UnmanagedType.LPStr)]  string name, 
            igs_observeCallback cb,
            IntPtr myData);
        public static int observeParameter(string name, igs_observeCallback cb, IntPtr myData) { return igs_observeParameter(name, cb, myData); }

        //mute or unmute an IOP
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_muteOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int muteOutput(string name) { return igs_muteOutput(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_unmuteOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int unmuteOutput(string name) { return igs_unmuteOutput(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_isOutputMuted([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static bool isOutputMuted(string name) { return igs_isOutputMuted(name); }

        //check IOP type, lists and existence
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern iopType_t igs_getTypeForInput([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static iopType_t getTypeForInput(string name) { return igs_getTypeForInput(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern iopType_t igs_getTypeForOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static iopType_t getTypeForOutput(string name) { return igs_getTypeForOutput(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern iopType_t igs_getTypeForParameter([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static iopType_t getTypeForParameter(string name) { return igs_getTypeForParameter(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_getInputsNumber();
        public static int getInputsNumber() { return igs_getInputsNumber(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_getOutputsNumber();
        public static int getOutputsNumber() { return igs_getOutputsNumber(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_getParametersNumber();
        public static int getParametersNumber() { return igs_getParametersNumber(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getInputsList(ref int nbOfElements); //returned char** must be freed using igs_freeIOPList
        public static string[] getInputsList(ref int nbOfElements)
        {
            IntPtr intptr = igs_getInputsList(ref nbOfElements);

            //intPtr tab
            IntPtr[] intPtrArray = new IntPtr[nbOfElements];

            //List of string inputs
            string[] list = new string[nbOfElements];

            //Copy the pointer to the tab of pointer
            Marshal.Copy(intptr, intPtrArray, 0, nbOfElements);

            //Fill the string tab
            for (int i = 0; i < nbOfElements; i++)
            {
                list[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);

                //TOFIX : release memory raise an exception
                //Marshal.FreeCoTaskMem(intPtrArray[i]);
            }
            //release the memory
            Igs.igs_freeIOPList(ref intptr, nbOfElements);

            return list;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getOutputsList(ref int nbOfElements); //returned char** must be freed using igs_freeIOPList
        public static string[] getOutputsList(ref int nbOfElements)
        {
            IntPtr intptr = igs_getOutputsList(ref nbOfElements);

            //intPtr tab
            IntPtr[] intPtrArray = new IntPtr[nbOfElements];

            //List of string inputs
            string[] list = new string[nbOfElements];

            //Copy the pointer to the tab of pointer
            Marshal.Copy(intptr, intPtrArray, 0, nbOfElements);

            //Fill the string tab
            for (int i = 0; i < nbOfElements; i++)
            {
                list[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);

                //TOFIX : release memory raise an exception
                //Marshal.FreeCoTaskMem(intPtrArray[i]);
            }

            return list;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getParametersList(ref int nbOfElements); //returned char** must be freed using igs_freeIOPList
        public static string[] getParametersList(ref int nbOfElements)
        {
            IntPtr intptr = igs_getParametersList(ref nbOfElements);

            //intPtr tab
            IntPtr[] intPtrArray = new IntPtr[nbOfElements];

            //List of string inputs
            string[] list = new string[nbOfElements];

            //Copy the pointer to the tab of pointer
            Marshal.Copy(intptr, intPtrArray, 0, nbOfElements);

            //Fill the string tab
            for (int i = 0; i < nbOfElements; i++)
            {
                list[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);

                //TOFIX : release memory raise an exception
                //Marshal.FreeCoTaskMem(intPtrArray[i]);
            }

            return list;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_freeIOPList(ref IntPtr list, int nbOfElements);
        public static void freeIOPList(ref IntPtr list, int nbOfElements) { igs_freeIOPList(ref list, nbOfElements); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_checkInputExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static bool checkInputExistence(string name) { return igs_checkInputExistence(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_checkOutputExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static bool checkOutputExistence(string name) { return igs_checkOutputExistence(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_checkParameterExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static bool checkParameterExistence(string name) { return igs_checkParameterExistence(name); }

        //////////////////////////////////////////////////
        //Definitions

        //load / set / get definition
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_loadDefinition([MarshalAs(UnmanagedType.LPStr)]  string json_str);
        public static int loadDefinition(string json_str) { return igs_loadDefinition(json_str); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_loadDefinitionFromPath([MarshalAs(UnmanagedType.LPStr)]  string file_path);
        public static int loadDefinitionFromPath(string file_path) { return igs_loadDefinitionFromPath(file_path); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_clearDefinition(); //clears definition data for the agent
        public static int clearDefinition() { return igs_clearDefinition(); } //clears definition data for the agent

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getDefinition(); //returns json string
        public static string getDefinition()
        {
            string str = "";
            IntPtr ptr = igs_getDefinition();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getDefinitionName();
        public static string getDefinitionName()
        {
            string str = "";
            IntPtr ptr = igs_getDefinitionName();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getDefinitionDescription();
        public static string getDefinitionDescription()
        {
            string str = "";
            IntPtr ptr = igs_getDefinitionDescription();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getDefinitionVersion();
        public static string getDefinitionVersion()
        {
            string str = "";
            IntPtr ptr = igs_getDefinitionVersion();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_setDefinitionName([MarshalAs(UnmanagedType.LPStr)] string name);
        public static int setDefinitionName(string name) { return igs_setDefinitionName(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_setDefinitionDescription([MarshalAs(UnmanagedType.LPStr)] string description);
        public static int setDefinitionDescription(string description) { return igs_setDefinitionDescription(description); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_setDefinitionVersion([MarshalAs(UnmanagedType.LPStr)] string version);
        public static int setDefinitionVersion(string version) { return igs_setDefinitionVersion(version); }


        //edit the definition using the API
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_createInput([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t value_type, IntPtr value, int size); //value must be copied in function
        public static int createInput(string name, iopType_t value_type, IntPtr value, int size) { return igs_createInput(name, value_type, value, size) ;} //value must be copied in function

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_createOutput([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t type, IntPtr value, int size); //value must be copied in function
        public static int createOutput(string name, iopType_t type, IntPtr value, int size) { return igs_createOutput(name, type, value, size); }  //value must be copied in function

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_createParameter([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t type, IntPtr value, int size); //value must be copied in function
        public static int createParameter(string name, iopType_t type, IntPtr value, int size) { return igs_createParameter(name, type, value, size); }  //value must be copied in function

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_removeInput([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int removeInput(string name) { return igs_removeInput(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_removeOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int removeOutput(string name) { return igs_removeOutput(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_removeParameter([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static int removeParameter(string name) { return igs_removeParameter(name); }

        //////////////////////////////////////////////////
        //mapping
        //load / set / get mapping
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_loadMapping([MarshalAs(UnmanagedType.LPStr)]  string json_str);
        public static int loadMapping(string json_str) { return igs_loadMapping(json_str); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_loadMappingFromPath([MarshalAs(UnmanagedType.LPStr)]  string file_path);
        public static int loadMappingFromPath(string file_path) { return igs_loadMappingFromPath(file_path); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_clearMapping(); //clears mapping data for the agent
        public static int clearMapping() { return igs_clearMapping(); }  //clears mapping data for the agent

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getMapping(); //returns json string
        public static string getMapping()
        {
            string str = "";
            IntPtr ptr = igs_getMapping();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getMappingName();
        public static string getMappingName()
        {
            string str = "";
            IntPtr ptr = igs_getMappingName();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getMappingDescription();
        public static string getMappingDescription()
        {
            string str = "";
            IntPtr ptr = igs_getMappingDescription();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getMappingVersion(); 
        public static string getMappingVersion()
        {
            string mappingVersion = "";

            IntPtr ptr = igs_getMappingVersion();

            mappingVersion = Marshal.PtrToStringAnsi(ptr);

            return mappingVersion;
        }       

        //edit mapping using the API
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_setMappingName([MarshalAs(UnmanagedType.LPStr)] string name);
        public static int setMappingName(string name) { return igs_setMappingName(name); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_setMappingDescription([MarshalAs(UnmanagedType.LPStr)] string description);
        public static int setMappingDescription(string description) { return igs_setMappingDescription(description); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_setMappingVersion([MarshalAs(UnmanagedType.LPStr)] string version);
        public static int setMappingVersion(string version) { return igs_setMappingVersion(version); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_getMappingEntriesNumber(); //number of entries in the mapping output type
        public static int getMappingEntriesNumber() { return igs_getMappingEntriesNumber(); } //number of entries in the mapping output type

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_addMappingEntry([MarshalAs(UnmanagedType.LPStr)]  string fromOurInput, [MarshalAs(UnmanagedType.LPStr)]  string toAgent, [MarshalAs(UnmanagedType.LPStr)]  string withOutput); //returns mapping id or zero or below if creation failed
        public static int addMappingEntry(string fromOurInput, string toAgent, string withOutput) { return igs_addMappingEntry(fromOurInput, toAgent, withOutput); } //returns mapping id or zero or below if creation failed

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_removeMappingEntryWithId(int theId);
        public static int removeMappingEntryWithId(int theId) { return igs_removeMappingEntryWithId(theId); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_removeMappingEntryWithName([MarshalAs(UnmanagedType.LPStr)]  string fromOurInput, [MarshalAs(UnmanagedType.LPStr)]  string toAgent, [MarshalAs(UnmanagedType.LPStr)]  string withOutput);
        public static int removeMappingEntryWithName(string fromOurInput, string toAgent, string withOutput) { return igs_removeMappingEntryWithName(fromOurInput, toAgent, withOutput); }


        //////////////////////////////////////////////////
        //administration, configuration & utilities

        //IngeScape library version
        //returns MAJOR*10000 + MINOR*100 + MICRO
        //displays MAJOR.MINOR.MICRO in console
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int igs_version();
        public static int version() { return igs_version(); }

        //Utility functions to find network adapters with broadcast capabilities
        //to be used in igs_startWithDevice
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_getNetdevicesList(IntPtr[,] devices, ref int nb);
        public static string [] getNetDevicesList()
        {
            //Get netdevices list
            IntPtr[,] ptrTab = new IntPtr[1, 1];
            int nb = 0;
            Igs.igs_getNetdevicesList(ptrTab, ref nb);

            //IntPtr to byte array
            IntPtr[] devicesPtrList = new IntPtr[nb];
            Marshal.Copy(ptrTab[0, 0], devicesPtrList, 0, nb);

            //List of string inputs
            string[] devicesList = new string[nb];

            //Fill the string tab
            for (int i = 0; i < nb; i++)
            {
                devicesList[i] = Marshal.PtrToStringAnsi(devicesPtrList[i]);
            }
            //HOTFIX : release the memory
            //Igs.igs_freeNetdevicesList(devicesPtrList, nb);

            return devicesList;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_freeNetdevicesList(IntPtr[] devices, int nb);
        public static void freeNetdevicesList(IntPtr[] devices, int nb) { igs_freeNetdevicesList(devices, nb); }

        //Command line for the agent can be passed here for inclusion in the
        //agent's headers. If not used, header is initialized with exec path.
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setCommandLine([MarshalAs(UnmanagedType.LPStr)]  string line);
        public static void setCommandLine(string line) { igs_setCommandLine(line); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setCommandLineFromArgs(int argc, string argv); //first element is replaced by absolute exec path on UNIX
        public static void setCommandLineFromArgs(int argc, string argv) { igs_setCommandLineFromArgs(argc, argv); } //first element is replaced by absolute exec path on UNIX

        //When mapping other agents, it is possible to request the
        //mapped agents to send us their current output values
        //through a private communication for our proper initialization.
        //By default, this behavior is disabled.
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setRequestOutputsFromMappedAgents(bool notify);
        public static void setRequestOutputsFromMappedAgents(bool notify) { igs_setRequestOutputsFromMappedAgents(notify); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_getRequestOutputsFromMappedAgents();
        public static bool getRequestOutputsFromMappedAgents() { return igs_getRequestOutputsFromMappedAgents(); }


        #region Logs

        /* Logs policy
         - fatal : Events that force application termination.
         - error : Events that are fatal to the current operation but not the whole application.
         - warning : Events that can potentially cause application anomalies but that can be recovered automatically (by circumventing or retrying).
         - info : Generally useful information to log (service start/stop, configuration assumptions, etc.).
         - debug : Information that is diagnostically helpful to people more than just developers but useless for system monitoring.
         - trace : Information about parts of functions, for detailed diagnostic only.
         */

        //set/get library parameters
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setVerbose(bool verbose);
        public static void setVerbose(bool verbose) { igs_setVerbose(verbose); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_isVerbose();
        public static bool isVerbose() { return igs_isVerbose(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setUseColorVerbose(bool useColor); //use colors in console
        public static void setUseColorVerbose(bool useColor) { igs_setUseColorVerbose(useColor); } //use colors in console

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_getUseColorVerbose();
        public static bool getUseColorVerbose() { return igs_getUseColorVerbose(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setLogStream(bool useLogStream); //enable logs in socket stream
        public static void setLogStream(bool useLogStream) { igs_setLogStream(useLogStream); } //enable logs in socket stream

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_getLogStream();
        public static bool getLogStream() { return igs_getLogStream(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setLogInFile(bool useLogFile); //enable logs in file
        public static void setLogInFile(bool useLogFile) { igs_setLogInFile(useLogFile); } //enable logs in file

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool igs_getLogInFile();
        public static bool getLogInFile() { return igs_getLogInFile(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setLogPath(string path); //default directory is ~/ on UNIX systems and current PATH on Windows
        public static void setLogPath(string path) { igs_setLogPath(path); } //default directory is ~/ on UNIX systems and current PATH on Windows

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getLogPath(); // must be freed by caller
        public static string getLogPath()
        {
            string str = "";
            IntPtr ptr = igs_getLogPath();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setLogLevel(igs_logLevel_t level); //set log level in console, default is IGS_LOG_INFO
        public static void setLogLevel(igs_logLevel_t level) { igs_setLogLevel(level); } //set log level in console, default is IGS_LOG_INFO

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern igs_logLevel_t igs_getLogLevel();
        public static igs_logLevel_t getLogLevel() { return igs_getLogLevel(); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_log(igs_logLevel_t logLevel, string function, string message, params object[] args);
        public static void log(igs_logLevel_t logLevel, string function, string message, params object[] args) { igs_log(logLevel, function, message, args); }

        public static void igs_trace(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_TRACE, memberName, message); }
        public static void igs_debug(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_DEBUG, memberName, message); }
        public static void igs_info(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_INFO, memberName, message); }
        public static void igs_warn(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_WARN, memberName, message); }
        public static void igs_error(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_ERROR, memberName, message); }
        public static void igs_fatal(string message, [CallerMemberName] string memberName = "") {igs_log(igs_logLevel_t.IGS_LOG_FATAL, memberName, message); }

        #endregion

        #region License

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_setLicensePath(string path);
        public static void setLicensePath(string path) { igs_setLicensePath(path); }

        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getLicensePath(); // must be freed by caller
        public static string getLicensePath()
        {
            string str = "";
            IntPtr ptr = igs_getLicensePath();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }

        // Any agent developer can use this function to check the license against her/his agent's unique id.
        // IDs are provided by the ingescape team.
        // Returns true if check is OK
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_checkLicenseForAgent(string agentId);
        public static void checkLicenseForAgent(string agentId) { igs_checkLicenseForAgent(agentId); }

        // use this callback mechanism to be notified when the timer
        // or number of agents or number of IOPs has been exceeded in demo mode
        [DllImport(ingescapeDLLPath, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void igs_observeLicense([MarshalAs(UnmanagedType.FunctionPtr)] igs_licenseCallback cb, IntPtr myData);
        public static void observeLicense(igs_licenseCallback cb, IntPtr myData) { igs_observeLicense(cb, myData); }

        #endregion
    }
}
