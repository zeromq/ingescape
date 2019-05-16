using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using IngescapeCSharp;

namespace CSharpSampleAgent
{
    class IgsAgent
    {
        public igs_observeCallback callbckPtr;

        void genericCallback(iop_t iopType,
        [MarshalAs(UnmanagedType.LPStr)] string name,
        iopType_t valueType,
        IntPtr value,
        int valueSize,
        IntPtr myData)
        {
            Console.WriteLine("callback test");
            switch (valueType)
            {
                case iopType_t.IGS_BOOL_T:
                    bool stateReceived = Ingescape.igs_readInputAsBool("boolean");
                    Console.WriteLine("Callback sur 'boolean' input : " + stateReceived.ToString());
                    break;

                case iopType_t.IGS_DATA_T:
                    break;

                case iopType_t.IGS_DOUBLE_T:
                    double doubleReceived = Ingescape.igs_readInputAsDouble("double");
                    Console.WriteLine("Callback sur 'double' input : " + doubleReceived.ToString());
                    break;

                case iopType_t.IGS_IMPULSION_T:
                    Console.WriteLine("Callback sur 'impulsion' input");
                    break;

                case iopType_t.IGS_INTEGER_T:
                    int valueReceived = Ingescape.igs_readInputAsInt("integer");
                    Console.WriteLine("Callback sur 'integer' input : " + valueReceived.ToString());
                    break;

                case iopType_t.IGS_STRING_T:
                    IntPtr intPtr = Ingescape.igs_readInputAsString("string");
                    string msg = Marshal.PtrToStringAnsi(intPtr);
                    Console.WriteLine("Callback sur 'string' input : " + msg);
                    break;

                default:
                    break;
            }
        }

        public IgsAgent()
        {
            //Load a definition from file
            Ingescape.igs_loadDefinitionFromPath("../igs-csharp-sample-def.json");

            //Load mapping from file
            Ingescape.igs_loadMappingFromPath("../igs-csharp-sample-mapping.json");

            //Verbose
            Ingescape.igs_setVerbose(true);
            bool isVerbose = Ingescape.igs_isVerbose();
            Console.WriteLine("Is verbose : " + isVerbose);

            //Color Verbose
            Ingescape.igs_setUseColorVerbose(true);
            bool isColorVerbose = Ingescape.igs_getUseColorVerbose();
            Console.WriteLine("Is colored verbose : " + isColorVerbose);

            //Log Stream
            Ingescape.igs_setLogStream(true);
            bool isLogStream = Ingescape.igs_getLogStream();
            Console.WriteLine("Is log stream : " + isLogStream);

            //Log in file
            Ingescape.igs_setLogInFile(true);
            bool isLogFile = Ingescape.igs_getLogInFile();
            Console.WriteLine("Is log file : " + isLogFile);

            //Log path
            Ingescape.igs_setLogPath("testLogFile.txt");
            IntPtr logPathPtr = Ingescape.igs_getLogPath();
            string logPath = Marshal.PtrToStringAnsi(logPathPtr);
            Console.WriteLine("Log file path : " + logPath);

            //Log level
            Ingescape.igs_setLogLevel(igs_logLevel_t.IGS_LOG_TRACE);
            igs_logLevel_t logLevel = Ingescape.igs_getLogLevel();
            Console.WriteLine("Log level : " + logLevel);

            //Start the agent on the network
            Ingescape.igs_startWithDevice("Ethernet", 5670);
        }

        public void writeInLog(string msg)
        {
            //Test the log file function
            Ingescape.igs_log(igs_logLevel_t.IGS_LOG_WARN, System.Reflection.MethodBase.GetCurrentMethod().Name, msg);

            //Test the function like macro function wrapping
            Ingescape.igs_warn(msg);

            Console.WriteLine("write in log : " + msg);
        }

        public void observeInputs()
        {
            callbckPtr = genericCallback;

            //Listing of input
            int nbOfElement = -1;
            IntPtr intptr = Ingescape.igs_getInputsList(ref nbOfElement);

            //intPtr tab
            IntPtr[] intPtrArray = new IntPtr[nbOfElement];

            //List of string inputs
            string[] inputsList = new string[nbOfElement];

            //Copy the pointer to the tab of pointer
            Marshal.Copy(intptr, intPtrArray, 0, nbOfElement);

            //Fill the string tab
            for (int i = 0; i < nbOfElement; i++)
            {
                inputsList[i] = Marshal.PtrToStringAnsi(intPtrArray[i]);

                //TOFIX : release memory raise an exception
                //Marshal.FreeCoTaskMem(intPtrArray[i]);

                //Observe the current input
                Ingescape.igs_observeInput(inputsList[i], callbckPtr, IntPtr.Zero);
            }

            //release the memory
            Ingescape.igs_freeIOPList(ref intptr, nbOfElement);           
        }

        public void createDefDynamically()
        {
            Ingescape.igs_createInput("string", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Ingescape.igs_createInput("impulsion", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);

            Ingescape.igs_createParameter("string", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Ingescape.igs_createParameter("impulsion", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);

            Ingescape.igs_createOutput("string-out", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Ingescape.igs_createOutput("impulsion-out", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);
        }

        public void createMappingDynamically()
        {
            Ingescape.igs_addMappingEntry("string", "Csharp-Sample", "string-out");
            Ingescape.igs_addMappingEntry("impulsion", "Csharp-Sample", "impulsion-out");
        }

        public void writeOnInputs()
        {
            //Integer
            int value = 100;
            int result = Ingescape.igs_writeInputAsInt("integer", value);

            //Double
            double val = 100.111;
            result = Ingescape.igs_writeInputAsDouble("double", val);

            //String
            string msg = "Helloword";
            result = Ingescape.igs_writeOutputAsString("string", msg);

            //Impulsion
            result = Ingescape.igs_writeOutputAsImpulsion("impulsion");
        }

        public void stop()
        {
            Ingescape.igs_stop();
        }
    }
}
