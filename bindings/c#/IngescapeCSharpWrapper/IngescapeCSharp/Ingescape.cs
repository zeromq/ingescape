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
    public enum iop_t { IGS_INPUT_T = 1, IGS_OUTPUT_T, IGS_PARAMETER_T };
    public enum iopType_t { IGS_INTEGER_T = 1, IGS_DOUBLE_T, IGS_STRING_T, IGS_BOOL_T, IGS_IMPULSION_T, IGS_DATA_T };
    public enum igs_logLevel_t{IGS_LOG_TRACE = 0,IGS_LOG_DEBUG,IGS_LOG_INFO,IGS_LOG_WARN,IGS_LOG_ERROR,IGS_LOG_FATAL};

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void igs_observeCallback(iop_t iopType,
                                        [MarshalAs(UnmanagedType.LPStr)] string name,
                                        iopType_t valueType,
                                        IntPtr value,
                                        int valueSize,
                                        IntPtr myData);


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void igs_forcedStopCallback(IntPtr myData);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void igs_muteCallback(bool isMuted, IntPtr myData);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void igs_freezeCallback(bool isPaused, IntPtr myData);


     public class Igs
    {
        //////////////////////////////////////////////////
        // Initialization and control

        //start & stop the agent
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_startWithDevice([MarshalAs(UnmanagedType.LPStr)]  string networkDevice, int port);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_startWithIP([MarshalAs(UnmanagedType.LPStr)]  string ipAddress, int port);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_stop();

        //There are four non-exclusive ways to stop the execution of an ingescape agent:
        //1- calling igs_stop from the hosting app's threads reacting on user actions or external events
        //2- handling SIGINT in the hosting app to call igs_stop and stop the rest of the app properly
        //3- monitoring the status of igs_Interrupted in the hosting app
        //4- using an igs_forcedStopCallback (see below) and calling code ON THE MAIN APP THREAD from it
        //In any case, igs_stop MUST NEVER BE CALLED directly from any ingeScape callback, as it would cause a thread deadlock.

        public static bool igs_Interrupted;
        //register a callback when the agent is forced to stop by the ingescape platform
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_observeForcedStop([MarshalAs(UnmanagedType.FunctionPtr)] igs_forcedStopCallback cb,
                    IntPtr myData);

        //terminate the agent with trigger of SIGINT and call to the registered igs_forcedStopCallbacks
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_die();

        //agent name set and get
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setAgentName([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getAgentName();
        public static string getAgentName()
        {
            string agentName = "";
            IntPtr ptr = igs_getAgentName();
            agentName = Marshal.PtrToStringAnsi(ptr);
            return agentName;
        }

        //control agent state
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setAgentState([MarshalAs(UnmanagedType.LPStr)]  string state);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getAgentState();
        public static string getAgentState()
        {
            string str = "";
            IntPtr ptr = igs_getAgentState();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }

        //mute the agent ouputs
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_mute();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_unmute();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isMuted();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_observeMute([MarshalAs(UnmanagedType.FunctionPtr)] igs_muteCallback cb,
                    IntPtr myData);

        //freeze and unfreeze the agent
        //When freezed, agent will not send anything on its outputs and
        //its inputs are not reactive to external data.
        //NB: the internal semantics of freeze and unfreeze for a given agent
        //are up to the developer and can be controlled using callbacks and igs_observeFreeze
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_freeze();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isFrozen();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_unfreeze();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_observeFreeze(igs_freezeCallback cb, IntPtr myData);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setCanBeFrozen(bool canBeFrozen);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_canBeFrozen();

        //////////////////////////////////////////////////
        //IOP Model : Inputs, Outputs and Parameters read/write/check/observe/mute

        //read IOP using void*
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readInput(string name, IntPtr[] value, ref int size);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readOutput(string name, IntPtr[] value, ref int size);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readParameter(string name, IntPtr[] value, ref int size);

        //read per type
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readInputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readInputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readInputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readInputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, IntPtr[] data, ref int size);

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readOutputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readOutputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readOutputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readOutputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static string readOutputAsString(string name)
        {
            string str = "";
            IntPtr ptr = igs_readOutputAsString(name);
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, IntPtr[] data, ref int size);
        

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        public static string readParameterAsString(string name)
        {
            string str = "";
            IntPtr ptr = igs_readParameterAsString(name);
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, IntPtr[] data, ref int size);


        //write per type
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsImpulsion([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, byte[] value, int size);

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsImpulsion([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, byte[] value, int size);

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, byte[] value, int size);

        //clear IOP data in memory without having to write the IOP
        //(relevant for IOPs with IGS_DATA_T type only)
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_clearDataForInput(string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_clearDataForOutput(string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_clearDataForParameter(string name);
       
        //observe writing to an IOP
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_observeInput([MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.FunctionPtr)] igs_observeCallback cb,
            IntPtr myData);

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_observeOutput([MarshalAs(UnmanagedType.LPStr)]  string name, 
            igs_observeCallback cb,
            IntPtr myData);

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_observeParameter([MarshalAs(UnmanagedType.LPStr)]  string name, 
            igs_observeCallback cb,
            IntPtr myData);

        //mute or unmute an IOP
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_muteOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_unmuteOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isOutputMuted([MarshalAs(UnmanagedType.LPStr)]  string name);

        //check IOP type, lists and existence
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern iopType_t igs_getTypeForInput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern iopType_t igs_getTypeForOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern iopType_t igs_getTypeForParameter([MarshalAs(UnmanagedType.LPStr)]  string name);

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_getInputsNumber();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_getOutputsNumber();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_getParametersNumber();

        
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
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
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
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
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
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
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_freeIOPList(ref IntPtr list, int nbOfElements);
        

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkInputExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkOutputExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkParameterExistence([MarshalAs(UnmanagedType.LPStr)]  string name);

        //////////////////////////////////////////////////
        //Definitions
        
            //load / set / get definition
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_loadDefinition([MarshalAs(UnmanagedType.LPStr)]  string json_str);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_loadDefinitionFromPath([MarshalAs(UnmanagedType.LPStr)]  string file_path);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_clearDefinition(); //clears definition data for the agent
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getDefinition(); //returns json string
        public static string getDefinition()
        {
            string str = "";
            IntPtr ptr = igs_getDefinition();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getDefinitionName();
        public static string getDefinitionName()
        {
            string str = "";
            IntPtr ptr = igs_getDefinitionName();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getDefinitionDescription();
        public static string getDefinitionDescription()
        {
            string str = "";
            IntPtr ptr = igs_getDefinitionDescription();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getDefinitionVersion();
        public static string getDefinitionVersion()
        {
            string str = "";
            IntPtr ptr = igs_getDefinitionVersion();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setDefinitionName([MarshalAs(UnmanagedType.LPStr)] string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setDefinitionDescription([MarshalAs(UnmanagedType.LPStr)] string description);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setDefinitionVersion([MarshalAs(UnmanagedType.LPStr)] string version);


        //edit the definition using the API
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_createInput([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t value_type, IntPtr value, int size); //value must be copied in function
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_createOutput([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t type, IntPtr value, int size); //value must be copied in function
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_createParameter([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t type, IntPtr value, int size); //value must be copied in function

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeInput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeParameter([MarshalAs(UnmanagedType.LPStr)]  string name);

        //////////////////////////////////////////////////
        //mapping
        //load / set / get mapping
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_loadMapping([MarshalAs(UnmanagedType.LPStr)]  string json_str);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_loadMappingFromPath([MarshalAs(UnmanagedType.LPStr)]  string file_path);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_clearMapping(); //clears mapping data for the agent
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getMapping(); //returns json string
        public static string getMapping()
        {
            string str = "";
            IntPtr ptr = igs_getMapping();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getMappingName();
        public static string getMappingName()
        {
            string str = "";
            IntPtr ptr = igs_getMappingName();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getMappingDescription();
        public static string getMappingDescription()
        {
            string str = "";
            IntPtr ptr = igs_getMappingDescription();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr igs_getMappingVersion(); 
        public static string getMappingVersion()
        {
            string mappingVersion = "";

            IntPtr ptr = igs_getMappingVersion();

            mappingVersion = Marshal.PtrToStringAnsi(ptr);

            return mappingVersion;
        }       

        //edit mapping using the API
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setMappingName([MarshalAs(UnmanagedType.LPStr)] string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setMappingDescription([MarshalAs(UnmanagedType.LPStr)] string description);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setMappingVersion([MarshalAs(UnmanagedType.LPStr)] string version);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_getMappingEntriesNumber(); //number of entries in the mapping output type
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_addMappingEntry([MarshalAs(UnmanagedType.LPStr)]  string fromOurInput, [MarshalAs(UnmanagedType.LPStr)]  string toAgent, [MarshalAs(UnmanagedType.LPStr)]  string withOutput); //returns mapping id or zero or below if creation failed
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeMappingEntryWithId(int theId);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeMappingEntryWithName([MarshalAs(UnmanagedType.LPStr)]  string fromOurInput, [MarshalAs(UnmanagedType.LPStr)]  string toAgent, [MarshalAs(UnmanagedType.LPStr)]  string withOutput);

        //////////////////////////////////////////////////
        //administration, configuration & utilities

        //IngeScape library version
        //returns MAJOR*10000 + MINOR*100 + MICRO
        //displays MAJOR.MINOR.MICRO in console
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_version();

        //Utility functions to find network adapters with broadcast capabilities
        //to be used in igs_startWithDevice
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_getNetdevicesList(out IntPtr devices, ref int nb);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_freeNetdevicesList(IntPtr devices, int nb);


        //Command line for the agent can be passed here for inclusion in the
        //agent's headers. If not used, header is initialized with exec path.
        /*TODO : implement
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setCommandLine([MarshalAs(UnmanagedType.LPStr)]  string line);
        PUBLIC void igs_setCommandLineFromArgs(int argc, const char* argv[]); //first element is replaced by absolute exec path on UNIX
        */

        //When mapping other agents, it is possible to request the
        //mapped agents to send us their current output values
        //through a private communication for our proper initialization.
        //By default, this behavior is disabled.
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setRequestOutputsFromMappedAgents(bool notify);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_getRequestOutputsFromMappedAgents();


        /* Logs policy
         - fatal : Events that force application termination.
         - error : Events that are fatal to the current operation but not the whole application.
         - warning : Events that can potentially cause application anomalies but that can be recovered automatically (by circumventing or retrying).
         - info : Generally useful information to log (service start/stop, configuration assumptions, etc.).
         - debug : Information that is diagnostically helpful to people more than just developers but useless for system monitoring.
         - trace : Information about parts of functions, for detailed diagnostic only.
         */

        //set/get library parameters
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setVerbose(bool verbose);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isVerbose();

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setUseColorVerbose(bool useColor); //use colors in console
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_getUseColorVerbose();

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setLogStream(bool useLogStream); //enable logs in socket stream
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_getLogStream();

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setLogInFile(bool useLogFile); //enable logs in file
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_getLogInFile();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setLogPath(string path); //default directory is ~/ on UNIX systems and current PATH on Windows
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getLogPath(); // must be freed by caller
        public static string getLogPath()
        {
            string str = "";
            IntPtr ptr = igs_getLogPath();
            str = Marshal.PtrToStringAnsi(ptr);
            return str;
        }
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setLogLevel(igs_logLevel_t level); //set log level in console, default is IGS_LOG_INFO
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern igs_logLevel_t igs_getLogLevel();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_log(igs_logLevel_t logLevel, string function, string message, params object[] args);

        public static void igs_trace(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_TRACE, memberName, message); }
        public static void igs_debug(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_DEBUG, memberName, message); }
        public static void igs_info(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_INFO, memberName, message); }
        public static void igs_warn(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_WARN, memberName, message); }
        public static void igs_error(string message, [CallerMemberName] string memberName = "") { igs_log(igs_logLevel_t.IGS_LOG_ERROR, memberName, message); }
        public static void igs_fatal(string message, [CallerMemberName] string memberName = "") {igs_log(igs_logLevel_t.IGS_LOG_FATAL, memberName, message);}
    }
}
