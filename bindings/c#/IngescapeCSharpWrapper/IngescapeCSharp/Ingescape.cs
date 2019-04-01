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

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    delegate void igs_observeCallback(iop_t iopType,
                                        [MarshalAs(UnmanagedType.LPStr)] string name,
                                        iopType_t valueType,
                                        IntPtr value,
                                        long valueSize,
                                        IntPtr myData);


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    delegate void igs_forcedStopCallback(IntPtr myData);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    delegate void igs_muteCallback(bool isMuted, IntPtr myData);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    delegate void igs_freezeCallback(bool isPaused, IntPtr myData);


    class Ingescape
    {
        //////////////////////////////////////////////////
        // Initialization and control

        //start & stop the agent
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_startWithDevice([MarshalAs(UnmanagedType.LPStr)]  string networkDevice, int port);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_startWithIP([MarshalAs(UnmanagedType.LPStr)]  string ipAddress, int port);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_stop();

        //There are four non-exclusive ways to stop the execution of an ingescape agent:
        //1- calling igs_stop from the hosting app's threads reacting on user actions or external events
        //2- handling SIGINT in the hosting app to call igs_stop and stop the rest of the app properly
        //3- monitoring the status of igs_Interrupted in the hosting app
        //4- using an igs_forcedStopCallback (see below) and calling code ON THE MAIN APP THREAD from it
        //In any case, igs_stop MUST NEVER BE CALLED directly from any ingeScape callback, as it would cause a thread deadlock.

        public static bool igs_Interrupted;
        //register a callback when the agent is forced to stop by the ingescape platform
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_observeForcedStop([MarshalAs(UnmanagedType.FunctionPtr)] igs_forcedStopCallback cb,
                    IntPtr myData);

        //terminate the agent with trigger of SIGINT and call to the registered igs_forcedStopCallbacks
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_die();

        //agent name set and get
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setAgentName([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getAgentName();

        //control agent state
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setAgentState([MarshalAs(UnmanagedType.LPStr)]  string state);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getAgentState();

        //mute the agent ouputs
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_mute();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_unmute();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isMuted();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_observeMute([MarshalAs(UnmanagedType.FunctionPtr)] igs_muteCallback cb,
                    IntPtr myData);

        //freeze and unfreeze the agent
        //When freezed, agent will not send anything on its outputs and
        //its inputs are not reactive to external data.
        //NB: the internal semantics of freeze and unfreeze for a given agent
        //are up to the developer and can be controlled using callbacks and igs_observeFreeze
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_freeze();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isFrozen();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_unfreeze();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_observeFreeze(igs_freezeCallback cb, IntPtr myData);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setCanBeFrozen(bool canBeFrozen);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
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
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readInputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readInputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readInputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readInputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        /*TODO : implement 
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);
        */

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readOutputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readOutputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readOutputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readOutputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        /* TODO : implement it
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);
        */

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        /*TODO : implement 
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);
        */

        //write per type
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInputAsImpulsion([MarshalAs(UnmanagedType.LPStr)]  string name);
        //TODO : implement it
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_writeInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void* value, long size);

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsImpulsion([MarshalAs(UnmanagedType.LPStr)]  string name);
        //TODO : implement it
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_writeOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void* value, long size);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutputAsData([MarshalAs(UnmanagedType.LPStr)] string name, byte[] data, long size);

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name, bool value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name, int value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name, double value);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value);
        //TODO : implement it
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_writeParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void* value, long size);

        //clear IOP data in memory without having to write the IOP
        //(relevant for IOPs with IGS_DATA_T type only)
        /* TODO : implement it
        PUBLIC void igs_clearDataForInput(const char* name);
        PUBLIC void igs_clearDataForOutput(const char* name);
        PUBLIC void igs_clearDataForParameter(const char* name);
        */

        //observe writing to an IOP
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_observeInput([MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.FunctionPtr)] igs_observeCallback cb,
            IntPtr myData);

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_observeOutput([MarshalAs(UnmanagedType.LPStr)]  string name, 
            igs_observeCallback cb,
            IntPtr myData);

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_observeParameter([MarshalAs(UnmanagedType.LPStr)]  string name, 
            igs_observeCallback cb,
            IntPtr myData);

        //mute or unmute an IOP
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_muteOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_unmuteOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isOutputMuted([MarshalAs(UnmanagedType.LPStr)]  string name);

        //check IOP type, lists and existence
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern iopType_t igs_getTypeForInput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern iopType_t igs_getTypeForOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern iopType_t igs_getTypeForParameter([MarshalAs(UnmanagedType.LPStr)]  string name);

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_getInputsNumber();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_getOutputsNumber();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_getParametersNumber();

        /* TODO : implement
        PUBLIC char** igs_getInputsList(long* nbOfElements); //returned char** must be freed using igs_freeIOPList
        PUBLIC char** igs_getOutputsList(long* nbOfElements); //returned char** must be freed using igs_freeIOPList
        PUBLIC char** igs_getParametersList(long* nbOfElements); //returned char** must be freed using igs_freeIOPList
        PUBLIC void igs_freeIOPList(char*** list, long nbOfElements);
        */

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkInputExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkOutputExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkParameterExistence([MarshalAs(UnmanagedType.LPStr)]  string name);

        //////////////////////////////////////////////////
        //Definitions
        
            //load / set / get definition
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_loadDefinition([MarshalAs(UnmanagedType.LPStr)]  string json_str);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_loadDefinitionFromPath([MarshalAs(UnmanagedType.LPStr)]  string file_path);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_clearDefinition(); //clears definition data for the agent
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getDefinition(); //returns json string
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getDefinitionName();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getDefinitionDescription();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getDefinitionVersion();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setDefinitionName([MarshalAs(UnmanagedType.LPStr)] string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setDefinitionDescription([MarshalAs(UnmanagedType.LPStr)] string description);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setDefinitionVersion([MarshalAs(UnmanagedType.LPStr)] string version);


        //edit the definition using the API
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_createInput([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t value_type, IntPtr value, long size); //value must be copied in function
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_createOutput([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t type, IntPtr value, long size); //value must be copied in function
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_createParameter([MarshalAs(UnmanagedType.LPStr)]  string name, iopType_t type, IntPtr value, long size); //value must be copied in function

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeInput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeParameter([MarshalAs(UnmanagedType.LPStr)]  string name);

        //////////////////////////////////////////////////
        //mapping
        //load / set / get mapping
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_loadMapping([MarshalAs(UnmanagedType.LPStr)]  string json_str);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_loadMappingFromPath([MarshalAs(UnmanagedType.LPStr)]  string file_path);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_clearMapping(); //clears mapping data for the agent
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getMapping(); //returns json string
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getMappingName();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getMappingDescription();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getMappingVersion();

        //edit mapping using the API
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setMappingName([MarshalAs(UnmanagedType.LPStr)] string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setMappingDescription([MarshalAs(UnmanagedType.LPStr)] string description);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setMappingVersion([MarshalAs(UnmanagedType.LPStr)] string version);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_getMappingEntriesNumber(); //number of entries in the mapping output type
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern long igs_addMappingEntry([MarshalAs(UnmanagedType.LPStr)]  string fromOurInput, [MarshalAs(UnmanagedType.LPStr)]  string toAgent, [MarshalAs(UnmanagedType.LPStr)]  string withOutput); //returns mapping id or zero or below if creation failed
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeMappingEntryWithId(long theId);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_removeMappingEntryWithName([MarshalAs(UnmanagedType.LPStr)]  string fromOurInput, [MarshalAs(UnmanagedType.LPStr)]  string toAgent, [MarshalAs(UnmanagedType.LPStr)]  string withOutput);

        //////////////////////////////////////////////////
        //administration, configuration & utilities

        //IngeScape library version
        //returns MAJOR*10000 + MINOR*100 + MICRO
        //displays MAJOR.MINOR.MICRO in console
        /* TODO : Implement
        PUBLIC int igs_version(void);
        */

        //Utility functions to find network adapters with broadcast capabilities
        //to be used in igs_startWithDevice
        /*TODO : implement
        PUBLIC void igs_getNetdevicesList(char*** devices, int* nb);
        PUBLIC void igs_freeNetdevicesList(char** devices, int nb);
        */

        //Command line for the agent can be passed here for inclusion in the
        //agent's headers. If not used, header is initialized with exec path.
        /*TODO : implement
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setCommandLine([MarshalAs(UnmanagedType.LPStr)]  string line);
        PUBLIC void igs_setCommandLineFromArgs(int argc, const char* argv[]); //first element is replaced by absolute exec path on UNIX
        */

        //When mapping other agents, it is possible to request the
        //mapped agents to send us their current output values
        //through a private communication for our proper initialization.
        //By default, this behavior is disabled.
        /*TODO : implement
        PUBLIC void igs_setRequestOutputsFromMappedAgents(bool notify);
        PUBLIC bool igs_getRequestOutputsFromMappedAgents(void);
        */

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

        //set/get library parameters
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setVerbose(bool verbose);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isVerbose();
        */
    }
}
