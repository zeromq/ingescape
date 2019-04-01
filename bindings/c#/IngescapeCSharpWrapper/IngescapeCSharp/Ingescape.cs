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


    class Ingescape
    {
        //initialization and configuration

        //This flag can be used to check if the IngeScape internal thread
        //has been interrupted or not. The flag is updated when using the
        //igs_start* (set to true) and igs_stop (set to false) functions,
        //and also when a DIE command is received by the agent (set to false).
        //There three non-exclusive ways to check & control the execution of the IngeScape
        //instance and its hosting application:
        //1- using igs_start* and igs_stop from the hosting app
        //2- catching SIGINT in the hosting app that is triggered by a DIE command in the agent
        //3- monitoring the status of igs_Interrupted in the hosting app
        //4- using igs_observeInterrupt below and providing a callback
        public static bool igs_Interrupted;

        //start & stop the agent
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_startWithDevice([MarshalAs(UnmanagedType.LPStr)]  string networkDevice, int port);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_startWithIP([MarshalAs(UnmanagedType.LPStr)]  string ipAddress, int port);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_stop();

        //terminate the agent and use the forcedStopCallbacks
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_die();

        //register a callback when the agent is asked to stop on the network
        //NB: callbacks should execute their code in the main application thread
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_observeForcedStop([MarshalAs(UnmanagedType.FunctionPtr)] igs_forcedStopCallback cb,
            IntPtr myData);


        //agent name set and get
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setAgentName([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getAgentName();

        //Command line for the agent can be passed here for inclusion in the
        //agent's headers. If not used, header is initialized with exec path.
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setCommandLine([MarshalAs(UnmanagedType.LPStr)]  string line);

        //By default, an agent notifies all the agent it maps. Each notification
        //makes the mapped agents publish their outputs (except for data & impulsions).
        //We allow to disable this notification to avoid side effects by agents frequently
        //changing their mapping.
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setNotifyMappedAgents(bool notify);

        //pause and resume the agent
        //TODO : implement it
        //      typedef void (*igs_freezeCallback)(bool isPaused, void* myData);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_freeze(void);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern bool igs_isFrozen(void);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_unfreeze(void);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_observeFreeze(igs_freezeCallback cb, void* myData);

        //control agent state
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_setAgentState([MarshalAs(UnmanagedType.LPStr)]  string state);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_getAgentState();

        //mute the agent
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_mute();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_unmute();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isMuted();

        //set/get library parameters
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setVerbose(bool verbose);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_getVerbose();
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_setCanBeFrozen(bool canBeFrozen);

        //utility function to find network adapters with broadcast capabilities
        //to be used in igs_startWithDevice
        //TODO : implement it
        //void igs_getNetdevicesList(char*** devices, int* nb);
        //void igs_freeNetdevicesList(char** devices, int nb);

        //////////////////////////////////////////////////
        //IOP Model : Inputs, Outputs and Parameters read/write/check/observe/mute

        //read/write IOP using void*
        //TODO : implement it
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_readInput([MarshalAs(UnmanagedType.LPStr)]  string name, void** value, long* size);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_readOutput([MarshalAs(UnmanagedType.LPStr)]  string name, void** value, long* size);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_readParameter([MarshalAs(UnmanagedType.LPStr)]  string name, void** value, long* size);

        //read per type
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readInputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readInputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readInputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readInputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        //TODO : implement it
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_readInputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readOutputAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readOutputAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readOutputAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readOutputAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        //TODO : implement it
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_readOutputAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_readParameterAsBool([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_readParameterAsInt([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern double igs_readParameterAsDouble([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr igs_readParameterAsString([MarshalAs(UnmanagedType.LPStr)]  string name);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_readParameterAsData([MarshalAs(UnmanagedType.LPStr)]  string name, void** data, long* size);

        //write using values in a string format
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeInput([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value, long size);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeOutput([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value, long size);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_writeParameter([MarshalAs(UnmanagedType.LPStr)]  string name, [MarshalAs(UnmanagedType.LPStr)] string value, long size);

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

        //TODO : implement it
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern char** igs_getInputsList(long* nbOfElements);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern char** igs_getOutputsList(long* nbOfElements);
        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern char** igs_getParametersList(long* nbOfElements);

        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkInputExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkOutputExistence([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_checkParameterExistence([MarshalAs(UnmanagedType.LPStr)]  string name);

        //observe IOP
        /**
         * \var typedef void (*igs_observeCallback)(iop_t iopType, const char *name, iopType_t valueType, void *value, void * myData)
         * \ingroup observefct
         * \brief typedef for the callback used in observe functions
         */
        //TODO : implement it
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igs_observeInput([MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.FunctionPtr)] igs_observeCallback cb,
            IntPtr myData);

        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_observeOutput([MarshalAs(UnmanagedType.LPStr)]  string name, igs_observeCallback cb, void* myData);

        //      [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int igs_observeParameter([MarshalAs(UnmanagedType.LPStr)]  string name, igs_observeCallback cb, void* myData);

        //mute or unmute an IOP
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_muteOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int igs_unmuteOutput([MarshalAs(UnmanagedType.LPStr)]  string name);
        [DllImport("C:\\Program Files (x86)\\ingescape\\lib\\libingescape.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igs_isOutputMuted([MarshalAs(UnmanagedType.LPStr)]  string name);

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
        //categories
        //TODO: later

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

    }
}
