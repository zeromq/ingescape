using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;
using Ingescape;

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
                    bool stateReceived = Igs.igs_readInputAsBool("boolean");
                    Console.WriteLine("Callback sur 'boolean' input : " + stateReceived.ToString());
                    break;

                case iopType_t.IGS_DATA_T:
                    break;

                case iopType_t.IGS_DOUBLE_T:
                    double doubleReceived = Igs.igs_readInputAsDouble("double");
                    Console.WriteLine("Callback sur 'double' input : " + doubleReceived.ToString());
                    break;

                case iopType_t.IGS_IMPULSION_T:
                    Console.WriteLine("Callback sur 'impulsion' input");
                    break;

                case iopType_t.IGS_INTEGER_T:
                    int valueReceived = Igs.igs_readInputAsInt("integer");
                    Console.WriteLine("Callback sur 'integer' input : " + valueReceived.ToString());
                    break;

                case iopType_t.IGS_STRING_T:
                    IntPtr intPtr = Igs.igs_readInputAsString("string");
                    string msg = Marshal.PtrToStringAnsi(intPtr);
                    Console.WriteLine("Callback sur 'string' input : " + msg);
                    break;

                default:
                    break;
            }

            //Data
            string myDataStr =  Marshal.PtrToStringAnsi(myData);
        }

        public IgsAgent()
        {
            //Load a definition from file
            Igs.igs_loadDefinitionFromPath("../igs-csharp-sample-def.json");

            //Load mapping from file
            Igs.igs_loadMappingFromPath("../igs-csharp-sample-mapping.json");

            //Get mapping version 
            string mappingVersion = Igs.getMappingVersion();

            //Get agent name
            string agentName = Igs.getAgentName();

            //Get agent state
            string agentState = Igs.getAgentState();

            //Get network list devices
            string[] netDevicesList = Igs.getNetDevicesList();
 
            //Verbose
            Igs.igs_setVerbose(true);
            bool isVerbose = Igs.igs_isVerbose();
            Console.WriteLine("Is verbose : " + isVerbose);

            //Color Verbose
            Igs.igs_setUseColorVerbose(true);
            bool isColorVerbose = Igs.igs_getUseColorVerbose();
            Console.WriteLine("Is colored verbose : " + isColorVerbose);

            //Log Stream
            Igs.igs_setLogStream(true);
            bool isLogStream = Igs.igs_getLogStream();
            Console.WriteLine("Is log stream : " + isLogStream);

            //Log in file
            Igs.igs_setLogInFile(true);
            bool isLogFile = Igs.igs_getLogInFile();
            Console.WriteLine("Is log file : " + isLogFile);

            //Log path
            Igs.igs_setLogPath("testLogFile.txt");
            IntPtr logPathPtr = Igs.igs_getLogPath();
            string logPath = Marshal.PtrToStringAnsi(logPathPtr);
            Console.WriteLine("Log file path : " + logPath);

            //Log level
            Igs.igs_setLogLevel(igs_logLevel_t.IGS_LOG_TRACE);
            igs_logLevel_t logLevel = Igs.igs_getLogLevel();
            Console.WriteLine("Log level : " + logLevel);

            //Start the agent on the network
            Igs.igs_startWithDevice("Ethernet", 5670);
        }

        public void readGenericFunctions()
        {
            /*
             * Input
             */ 
            //Write a value
            string str = "helloword-input";
            Igs.igs_writeInputAsString("string", str);

            //Use the generic function to read the value of the input
            IntPtr[] intPtrArray = new IntPtr[1];
            int size = 0;
            Igs.igs_readInput("string", intPtrArray, ref size);
            string value = Marshal.PtrToStringAnsi(intPtrArray[0]);

            //Print the value
            Console.WriteLine("Value of the input string : " + value);

            /*
             * Output
             */
            //Write a value
            str = "helloword-out";
            Igs.igs_writeOutputAsString("string-out", str);

            //Use the generic function to read the value of the output
            //intPtrArray = new IntPtr[1];
            Igs.igs_readOutput("string-out", intPtrArray, ref size);
            value = Marshal.PtrToStringAnsi(intPtrArray[0]);

            //Print the value
            Console.WriteLine("Value of the output string : " + value);

            /*
             * Parameter
             */
            //Write a value
            str = "helloword-param";
            Igs.igs_writeParameterAsString("string-param", str);

            //Use the generic function to read the value of the parameter
            //intPtrArray = new IntPtr[1];
            Igs.igs_readParameter("string-param", intPtrArray, ref size);
            value = Marshal.PtrToStringAnsi(intPtrArray[0]);

            //Print the value
            Console.WriteLine("Value of the parameter string : " + value);
        }

        public void writeInLog(string msg)
        {
            //Test the log file function
            Igs.igs_log(igs_logLevel_t.IGS_LOG_INFO, System.Reflection.MethodBase.GetCurrentMethod().Name, msg);

            //Test the function like macro function wrapping
            Igs.igs_warn(msg);

            Console.WriteLine("write in log : " + msg);
        }

        public void observeInputs()
        {
            callbckPtr = genericCallback;

            //Listing of input
            int nbOfElement = -1;
            string[] inputsList = Igs.getInputsList(ref nbOfElement);

            string myData = "MyData";
            IntPtr myDataPtr = Marshal.StringToHGlobalAnsi(myData);

            //Fill the string tab
            for (int i = 0; i < nbOfElement; i++)
            {
                //Observe the current input    
                Igs.igs_observeInput(inputsList[i], callbckPtr, myDataPtr);
            }
        }

        public void createDefDynamically()
        {
            Igs.igs_createInput("string", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Igs.igs_createInput("impulsion", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);

            Igs.igs_createParameter("string", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Igs.igs_createParameter("impulsion", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);

            Igs.igs_createOutput("string-out", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Igs.igs_createOutput("impulsion-out", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);
            Igs.igs_createOutput("data-out", iopType_t.IGS_DATA_T, IntPtr.Zero, 0);
        }

        public void createMappingDynamically()
        {
            Igs.igs_addMappingEntry("string", "Csharp-Sample", "string-out");
            Igs.igs_addMappingEntry("impulsion", "Csharp-Sample", "impulsion-out");
        }

        public void writeOnInputs()
        {
            //Integer
            int value = 100;
            int result = Igs.igs_writeInputAsInt("integer", value);

            //Double
            double val = 100.111;
            result = Igs.igs_writeInputAsDouble("double", val);

            //String
            string msg = "Helloword";
            result = Igs.igs_writeOutputAsString("string", msg);

            //Impulsion
            result = Igs.igs_writeOutputAsImpulsion("impulsion");
        }

        public void writeAndReadData()
        {
            byte[] fooBytes;

            //Initialize object
            Foo foo = new Foo();

            //Serialize object to byte array
            BinaryFormatter bf = new BinaryFormatter();
            MemoryStream ms = new MemoryStream();
            bf.Serialize(ms, foo);
            fooBytes = ms.ToArray();

            //Write on output
            int size = fooBytes.Length;
            Igs.igs_writeOutputAsData("data-out", fooBytes, size);

            //Read data
            byte[] data = new byte[]{};
            Igs.readOutputAsData("data-out", ref data);

            //Deserialize          
            BinaryFormatter formatter = new BinaryFormatter();
            MemoryStream msRead = new MemoryStream(data);
            Foo readFoo = (Foo)formatter.Deserialize(msRead);
        }

        public void stop()
        {
            Igs.igs_stop();
        }
    }
}
