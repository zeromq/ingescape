using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace IngescapeCSharp
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


     public class Ingescape
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
        public static extern IntPtr igs_getAgentName();

        //control agent state
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setAgentState([MarshalAs(UnmanagedType.LPStr)]  string state);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getAgentState();

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
        /* TODO : implement
        PUBLIC int igs_readInput(const char* name, void** value, size_t *size);
        PUBLIC int igs_readOutput(const char* name, void** value, size_t *size);
        PUBLIC int igs_readParameter(const char* name, void** value, size_t *size);
        */

        //read per type
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readInputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readInputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readInputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readInputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        /*TODO : implement 
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);
        */

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readOutputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readOutputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readOutputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readOutputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        /* TODO : implement it
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);
        */

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        /*TODO : implement 
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);
        */

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
        //TODO : implement it
        //      [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_writeInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void* value, long size);

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
        //TODO : implement it
        //      [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_writeOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void* value, long size);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsData([MarshalAs(UnmanagedType.LPStr)] string name, byte[] data, int size);

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        //TODO : implement it
        //      [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_writeParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void* value, long size);

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
        public static extern IntPtr igs_getInputsList(ref int nbOfElements); //returned char** must be freed using igs_freeIOPList
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getOutputsList(ref int nbOfElements); //returned char** must be freed using igs_freeIOPList
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getParametersList(ref int nbOfElements); //returned char** must be freed using igs_freeIOPList
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
        public static extern IntPtr igs_getDefinition(); //returns json string
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getDefinitionName();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getDefinitionDescription();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getDefinitionVersion();
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
        public static extern IntPtr igs_getMapping(); //returns json string
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getMappingName();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getMappingDescription();
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getMappingVersion();

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
        /*TODO : implement
        typedef enum {
           IGS_LOG_TRACE = 0,
           IGS_LOG_DEBUG,
           IGS_LOG_INFO,
           IGS_LOG_WARN,
           IGS_LOG_ERROR,
           IGS_LOG_FATAL
        }
        igs_logLevel_t;
        */

        //set/get library parameters
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setVerbose(bool verbose);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isVerbose();

        //PUBLIC void igs_setUseColorVerbose(bool useColor); //use colors in console
        //PUBLIC bool igs_getUseColorVerbose(void);

        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setLogStream(bool useLogStream); //enable logs in socket stream
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_getLogStream();

        //PUBLIC void igs_setLogInFile(bool useLogFile); //enable logs in file
        //PUBLIC bool igs_getLogInFile(void);
        //PUBLIC void igs_setLogPath(const char* path); //default directory is ~/ on UNIX systems and current PATH on Windows
        //PUBLIC char* igs_getLogPath(void); // must be freed by caller

        //PUBLIC void igs_setLogLevel(igs_logLevel_t level); //set log level in console, default is IGS_LOG_INFO
        //PUBLIC igs_logLevel_t igs_getLogLevel(void);
        //PUBLIC void igs_log(igs_logLevel_t, const char* function, const char* format, ...)  CHECK_PRINTF(3);
        [DllImport("C:\\ingescape\\libs\\debug\\ingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_log(igs_logLevel_t logLevel, string function, string message, params object[] args);

        //#define igs_trace(...) igs_log(IGS_LOG_TRACE, __func__, __VA_ARGS__)
        //#define igs_debug(...) igs_log(IGS_LOG_DEBUG, __func__, __VA_ARGS__)
        //#define igs_info(...)  igs_log(IGS_LOG_INFO, __func__, __VA_ARGS__)
        //#define igs_warn(...)  igs_log(IGS_LOG_WARN, __func__, __VA_ARGS__)
        //#define igs_error(...) igs_log(IGS_LOG_ERROR, __func__, __VA_ARGS__)
        //#define igs_fatal(...) igs_log(IGS_LOG_FATAL, __func__, __VA_ARGS__)

    }
}
