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
                    bool stateReceived = Igs.readInputAsBool("boolean");
                    Console.WriteLine("Callback sur 'boolean' input : " + stateReceived.ToString());
                    break;

                case iopType_t.IGS_DATA_T:
                    break;

                case iopType_t.IGS_DOUBLE_T:
                    double doubleReceived = Igs.readInputAsDouble("double");
                    Console.WriteLine("Callback sur 'double' input : " + doubleReceived.ToString());
                    break;

                case iopType_t.IGS_IMPULSION_T:
                    Console.WriteLine("Callback sur 'impulsion' input");
                    break;

                case iopType_t.IGS_INTEGER_T:
                    //int valueReceived = Igs.readInputAsInt("integer");
                    //Console.WriteLine("Callback sur 'integer' input : " + valueReceived.ToString());
                    break;

                case iopType_t.IGS_STRING_T:
                    string msg = Igs.readInputAsString("string");
                    Console.WriteLine("Callback sur 'string' input : " + msg);
                    break;

                default:
                    break;
            }

        }

        public IgsAgent()
        {
            //Load a definition from file
            Igs.loadDefinitionFromPath("../../../data/igs-csharp-sample-def.json");

            //Load mapping from file
            Igs.loadMappingFromPath("../../../data/igs-csharp-sample-mapping.json");

            //Get mapping version 
            string mappingVersion = Igs.getMappingVersion();

            //Get agent name
            string agentName = Igs.getAgentName();
            Igs.igs_setAgentName("test");

            //Get agent state
            string agentState = Igs.getAgentState();

            //Get network list devices
            string[] netDevicesList = Igs.getNetDevicesList();
 
            ////Verbose
            //Igs.setVerbose(true);
            //bool isVerbose = Igs.isVerbose();
            //Console.WriteLine("Is verbose : " + isVerbose);

            ////Color Verbose
            //Igs.setUseColorVerbose(true);
            //bool isColorVerbose = Igs.getUseColorVerbose();
            //Console.WriteLine("Is colored verbose : " + isColorVerbose);

            ////Log Stream
            //Igs.setLogStream(true);
            //bool isLogStream = Igs.getLogStream();
            //Console.WriteLine("Is log stream : " + isLogStream);

            ////Log in file
            //Igs.setLogInFile(true);
            //bool isLogFile = Igs.getLogInFile();
            //Console.WriteLine("Is log file : " + isLogFile);

            ////Log path
            //Igs.setLogPath("testLogFile.txt");
            //string logPath = Igs.getLogPath();
            //Console.WriteLine("Log file path : " + logPath);

            ////Log level
            //Igs.setLogLevel(igs_logLevel_t.IGS_LOG_TRACE);
            //igs_logLevel_t logLevel = Igs.getLogLevel();
            //Console.WriteLine("Log level : " + logLevel);

            //Start the agent on the network
            Igs.startWithDevice("Ethernet", 5670);

            //TODO : implement test of the command line functions
        }

        public void readGenericFunctions()
        {
            /*
             * Input
             */ 
            //Write a value
            string str = "helloword-input";
            Igs.writeInputAsString("string", str);

            //Use the generic function to read the value of the input
            IntPtr[] intPtrArray = new IntPtr[1];
            int size = 0;
            Igs.readInput("string", intPtrArray, ref size);
            string value = Marshal.PtrToStringAnsi(intPtrArray[0]);

            //Print the value
            Console.WriteLine("Value of the input string : " + value);

            /*
             * Output
             */
            //Write a value
            str = "helloword-out";
            Igs.writeOutputAsString("string-out", str);

            //Use the generic function to read the value of the output
            //intPtrArray = new IntPtr[1];
            Igs.readOutput("string-out", intPtrArray, ref size);
            value = Marshal.PtrToStringAnsi(intPtrArray[0]);

            //Print the value
            Console.WriteLine("Value of the output string : " + value);

            /*
             * Parameter
             */
            //Write a value
            str = "helloword-param";
            Igs.writeParameterAsString("string-param", str);

            //Use the generic function to read the value of the parameter
            //intPtrArray = new IntPtr[1];
            Igs.readParameter("string-param", intPtrArray, ref size);
            value = Marshal.PtrToStringAnsi(intPtrArray[0]);

            //Print the value
            Console.WriteLine("Value of the parameter string : " + value);
        }

        public void writeInLog(string msg)
        {
            //Test the log file function
            Igs.log(igs_logLevel_t.IGS_LOG_INFO, System.Reflection.MethodBase.GetCurrentMethod().Name, msg);

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
                Igs.observeInput(inputsList[i], callbckPtr, myDataPtr);
            }
        }

        public void createDefDynamically()
        {
            Igs.createInput("string", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Igs.createInput("impulsion", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);

            Igs.createParameter("string-param", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Igs.createParameter("impulsion-param", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);

            Igs.createOutput("string-out", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Igs.createOutput("impulsion-out", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);
            Igs.createOutput("data-out", iopType_t.IGS_DATA_T, IntPtr.Zero, 0);
        }

        public void createMappingDynamically()
        {
            Igs.addMappingEntry("string", "Csharp-Sample", "string-out");
            Igs.addMappingEntry("impulsion", "Csharp-Sample", "impulsion-out");
        }

        public void writeOnInputs()
        {
            //Integer
            int value = 100;
            int result = -1;
            result = Igs.writeInputAsInt("integer", value);

            //Double
            double val = 100.111;
            result = Igs.writeInputAsDouble("double", val);

            //String
            string msg = "Helloword";
            result = Igs.writeInputAsString("string", msg);

            //Impulsion
            result = Igs.writeInputAsImpulsion("impulsion");
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
            Igs.writeOutputAsData("data-out", fooBytes, size);

            //Read data
            byte[] data = new byte[]{};
            Igs.readOutputAsData("data-out", ref data);

            //Deserialize          
            BinaryFormatter formatter = new BinaryFormatter();
            MemoryStream msRead = new MemoryStream(data);
            Foo readFoo = (Foo)formatter.Deserialize(msRead);
        }

        public void memoryleakstest()
        {
            for (int i = 0; i < 1000; i++)
            {
                int result = -1;
                
                //Integer
                int value = 100;
                result = Igs.writeInputAsInt("integer", value);

                //Double
                double val = 100.111;
                result = Igs.writeInputAsDouble("double", val);

                //String
                string msg = "Helloword", outputName = "string-out";
                result = Igs.writeOutputAsString(outputName, msg);

                //Impulsion
                result = Igs.writeOutputAsImpulsion("impulsion-out");
            }
        }

        public void stop()
        {
            Igs.stop();
        }
    }
}
