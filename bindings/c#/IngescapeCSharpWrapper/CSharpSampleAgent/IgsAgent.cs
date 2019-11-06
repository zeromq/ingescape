using Ingescape;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;

namespace CSharpSampleAgent
{
    class IgsAgent
    {
        #region Attributes

        private int _count = 0;

        #endregion

        #region Callbacks

        public igs_observeCallback _callbackPtr;

        public igs_callFunction _functionCallPtr;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="iopType"></param>
        /// <param name="name"></param>
        /// <param name="valueType"></param>
        /// <param name="value"></param>
        /// <param name="valueSize"></param>
        /// <param name="myData"></param>
        void genericCallback(iop_t iopType,
                             string name,
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


        /// <summary>
        /// 
        /// </summary>
        /// <param name="senderAgentName"></param>
        /// <param name="senderAgentUUID"></param>
        /// <param name="callName"></param>
        /// <param name="firstArgument"></param>
        /// <param name="nbArgs"></param>
        /// <param name="myData"></param>
        void cSharpCallFunction(string senderAgentName,
                                string senderAgentUUID,
                                string callName,
                                IntPtr firstArgument,
                                uint nbArgs,
                                IntPtr myData)
        {
            Console.WriteLine("'{2}' called from '{0}' ({1}) with {3} args:", senderAgentName, senderAgentUUID, callName, nbArgs);

            if (myData != IntPtr.Zero)
            {
                string utf8 = Igs.getStringFromPointer(myData);
                if (!string.IsNullOrEmpty(utf8))
                {
                    Console.WriteLine("myData = {0}", utf8);
                }
            }

            if (nbArgs == 5)
            {
                List<CallArgument> callArgumentsList = Igs.getCallArgumentsList(firstArgument);
                int i = 0;

                foreach (CallArgument callArgument in callArgumentsList)
                {
                    if (callArgument != null)
                    {
                        if (callArgument.Type != iopType_t.IGS_DATA_T)
                        {
                            Console.WriteLine("{0}: {1} = {2} ({3})", i, callArgument.Name, callArgument.Value, callArgument.Type);
                        }
                        else
                        {
                            byte[] byteArray = (byte[])callArgument.Value;

                            // WARNING Special case: We know that the data is a string, so we can convert from byte[] to string
                            string stringData = Encoding.UTF8.GetString(byteArray);
                            stringData = stringData.TrimEnd('\0');

                            Console.WriteLine("{0}: {1} = {2} ({3})", i, callArgument.Name, stringData, callArgument.Type);
                        }
                    }
                    i++;
                }
            }
        }


        // License Callback
        public igs_licenseCallback ptrOnLicenseCallbck;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="limit"></param>
        /// <param name="myData"></param>
        void onLicenseCallback(igs_license_limit_t limit, IntPtr myData)
        {
            Console.WriteLine("onLicenseCallback " + limit);
        }

        #endregion

        #region Constructor

        /// <summary>
        /// Constructor
        /// </summary>
        public IgsAgent()
        {
            //Load a definition from file
            int success = Igs.loadDefinitionFromPath("../../../data/igs-csharp-sample-def.json");
            if (success == 1) {
                Console.WriteLine("The loading of the definition succeeded");
            }
            else {
                Console.WriteLine("ERROR: The loading of the definition failed !");
            }

            //Igs.setDefinitionName("A (éç) A");
            string defName = Igs.getDefinitionName();
            string defDescription = Igs.getDefinitionDescription();
            Console.WriteLine("Definition: Name = '{0}' -- Description = '{1}'", defName, defDescription);

            //Load mapping from file
            success = Igs.loadMappingFromPath("../../../data/igs-csharp-sample-mapping.json");
            if (success == 1) {
                Console.WriteLine("The loading of the mapping succeeded");
            }
            else {
                Console.WriteLine("ERROR: The loading of the mapping failed !");
            }

            //"name": "C#-Sample (é ç partï)"
            //Igs.setMappingName("A (éç) A");
            string mapName = Igs.getMappingName();
            string mapDescription = Igs.getMappingDescription();
            Console.WriteLine("Mapping: Name = '{0}' -- Description = '{1}'", mapName, mapDescription);

            //Get mapping version 
            string mappingVersion = Igs.getMappingVersion();

            //Get agent name
            string agentName = Igs.getAgentName();
            //Igs.setAgentName("test-é-ç");


            //
            // CALLS
            //
            #region Calls

            _functionCallPtr = cSharpCallFunction;

            //string callName = "çShàrpCàll";
            string callName = "cSharpCall";
            //string callArgName = "àrgÏnt2";
            string callArgName = "argInt2";

            string strMyData = "My Data (é ç parti)";
            IntPtr ptrMyData = Igs.getPointerFromString(strMyData);

            Igs.initCall(callName, _functionCallPtr, ptrMyData);
            Igs.addArgumentToCall(callName, "argBool1", iopType_t.IGS_BOOL_T);
            Igs.addArgumentToCall(callName, callArgName, iopType_t.IGS_INTEGER_T);
            //Igs.addArgumentToCall(callName, "argDoùble3", iopType_t.IGS_DOUBLE_T);
            Igs.addArgumentToCall(callName, "argDouble3", iopType_t.IGS_DOUBLE_T);
            Igs.addArgumentToCall(callName, "argString4", iopType_t.IGS_STRING_T);
            Igs.addArgumentToCall(callName, "argData5", iopType_t.IGS_DATA_T);

            uint numberOfCalls = Igs.getNumberOfCalls();
            Console.WriteLine("Number of Calls = {0}", numberOfCalls);

            //string callName1 = "séndMail";
            string callName1 = "sendMail";
            string callName2 = "Call_BIDON";
            //string callArgName1 = "sùbjéct";
            string callArgName1 = "subject";
            string callArgName2 = "Arg_BIDON";
            
            bool existCall1 = Igs.checkCallExistence(callName1);
            bool existCall2 = Igs.checkCallExistence(callName2);
            bool existCall = Igs.checkCallExistence(callName);

            Console.WriteLine("\nExist Calls: {0}={1} -- {2}={3} -- {4}={5}", callName1, existCall1, callName2, existCall2, callName, existCall);

            bool existCallArg1 = Igs.checkCallArgumentExistence(callName1, callArgName1);
            bool existCallArg2 = Igs.checkCallArgumentExistence(callName1, callArgName2);
            bool existCallArg3 = Igs.checkCallArgumentExistence(callName2, callArgName2);
            bool existCallArg = Igs.checkCallArgumentExistence(callName, callArgName);

            Console.WriteLine("\nExist Call Arg:");
            Console.WriteLine("{0}.{1}={2}\n{3}.{4}={5}\n{6}.{7}={8}",
                callName1, callArgName1, existCallArg1,
                callName1, callArgName2, existCallArg2,
                callName2, callArgName2, existCallArg3);
            Console.WriteLine("{0}.{1}={2}",
                callName, callArgName, existCallArg);

            string inputName1 = "boolean";
            string inputName2 = "zzzzz";

            bool existIn1 = Igs.checkInputExistence(inputName1);
            bool existIn2 = Igs.checkInputExistence(inputName2);

            Console.WriteLine("\nExist Inputs: {0}={1} -- {2}={3}", inputName1, existIn1, inputName2, existIn2);

            string[] callsList = Igs.getCallsList();
            for (int i = 0; i < callsList.Length; i++)
            {
                string tmpCallName = callsList[i];
                uint argsNb = Igs.getNumberOfArgumentsForCall(tmpCallName);

                Console.WriteLine("call {0} -- args nb = {1}", tmpCallName, argsNb);
            }


            IntPtr firstArg = Igs.getFirstArgumentForCall("sendMail");
            if (firstArg != IntPtr.Zero)
            {
                Console.WriteLine("First arg is defined:");

                List<CallArgument> callArgumentsList = Igs.getCallArgumentsList(firstArg);
                int i = 0;

                foreach (CallArgument callArgument in callArgumentsList)
                {
                    if (callArgument != null)
                    {
                        if (callArgument.Type != iopType_t.IGS_DATA_T)
                        {
                            Console.WriteLine("{0}: {1} = {2} ({3})", i, callArgument.Name, callArgument.Value, callArgument.Type);
                        }
                    }
                    i++;
                }
            }
            else
            {
                Console.WriteLine("First arg is NOT defined !");
            }

            #endregion


            // Get agent state
            //string checkAgentName = Igs.getAgentName();
            //Console.WriteLine("Agent name = '{0}'", checkAgentName);

            // Get agent state
            string agentState = Igs.getAgentState();

            // Get network devices and addresses
            string[] netDevicesList = Igs.getNetDevicesList();
            string[] netAddressesList = Igs.getNetAddressesList();
            if (netDevicesList.Length == netAddressesList.Length)
            {
                for (int i = 0; i < netDevicesList.Length; i++)
                {
                    string netDevice = netDevicesList[i];
                    string netAddress = netAddressesList[i];

                    Console.WriteLine("{0}: '{1}' with ip {2}", i, netDevice, netAddress);
                }
            }
 
            // Verbose
            Igs.setVerbose(true);
            //bool isVerbose = Igs.isVerbose();
            //Console.WriteLine("Is verbose: " + isVerbose);

            string documentsPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            string licensePath = string.Format(@"{0}\IngeScape\licenses\", documentsPath);
            //string licensePath = string.Format(@"{0}\IngeScape\licenses_aççents_égüe\", documentsPath);
            Console.WriteLine("documentsPath: {0} -- licensePath: {1}", documentsPath, licensePath);
            Igs.setLicensePath(licensePath);

            string checkLicensePath = Igs.getLicensePath();
            Console.WriteLine("get License Path: '{0}'", checkLicensePath);

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

            // Log level
            Igs.setLogLevel(igs_logLevel_t.IGS_LOG_TRACE);
            igs_logLevel_t logLevel = Igs.getLogLevel();
            Console.WriteLine("Log level: " + logLevel);

            ptrOnLicenseCallbck = onLicenseCallback;

            // Observe license error
            Igs.observeLicense(ptrOnLicenseCallbck, IntPtr.Zero);

            // 
            //Igs.checkLicenseForAgent("");


            // getting app parameters
            string igsNetDevice = ConfigurationManager.AppSettings["igsNetDevice"];
            //string igsIP = ConfigurationManager.AppSettings["igsIP"];
            int igsPort = int.Parse(ConfigurationManager.AppSettings["igsNetPort"]);
            Console.WriteLine("Parameters: Net device='{0}' -- Net port={1}", igsNetDevice, igsPort);

            //Start the agent on the network
            Igs.startWithDevice(igsNetDevice, igsPort);

            //TODO : implement test of the command line functions
        }

        #endregion

        #region Methods

        /// <summary>
        /// 
        /// </summary>
        public void readGenericFunctions()
        {
            /*
             * Input
             */
            //Write a value
            //string str = "helloword-input";
            string str = "helloword-é-ç-input";
            Igs.writeInputAsString("string", str);

            //Use the generic function to read the value of the input
            IntPtr[] intPtrArray = new IntPtr[1];
            uint size = 0;
            Igs.readInput("string", intPtrArray, ref size);
            string value = Marshal.PtrToStringAnsi(intPtrArray[0]);

            //Print the value
            Console.WriteLine("Value of the input string : " + value);

            /*
             * Output
             */
            //Write a value
            //str = "helloword-out";
            str = "helloword-é-ç-output";
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
            str = "helloword-é-ç-param";
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
            _callbackPtr = genericCallback;

            //Listing of input
            int nbOfElement = -1;
            string[] inputsList = Igs.getInputsList(ref nbOfElement);

            string myData = "MyData";
            IntPtr myDataPtr = Marshal.StringToHGlobalAnsi(myData);

            //Fill the string tab
            for (int i = 0; i < nbOfElement; i++)
            {
                //Observe the current input    
                Igs.observeInput(inputsList[i], _callbackPtr, myDataPtr);
            }
        }

        public void createDefDynamically()
        {
            Igs.createInput("string", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Igs.createInput("impulsion", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);
            Igs.createInput("data", iopType_t.IGS_DATA_T, IntPtr.Zero, 0);

            Igs.createParameter("string-param", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            //Igs.createParameter("impulsion-param", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);

            Igs.createOutput("string-out", iopType_t.IGS_STRING_T, IntPtr.Zero, 0);
            Igs.createOutput("impulsion-out", iopType_t.IGS_IMPULSION_T, IntPtr.Zero, 0);
            Igs.createOutput("data-out", iopType_t.IGS_DATA_T, IntPtr.Zero, 0);
        }

        public void createMappingDynamically()
        {
            Igs.addMappingEntry("string", "Csharp-Sample", "string-out");
            Igs.addMappingEntry("impulsion", "Csharp-Sample", "impulsion-out");
        }


        /// <summary>
        /// 
        /// </summary>
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


        /// <summary>
        /// 
        /// </summary>
        public void writeAndReadData()
        {
            byte[] fooBytes;

            //Initialize object
            Foo foo = new Foo();
            Console.WriteLine("foo X={0}, Y={1} will be written on output", foo.FooX, foo.FooY);

            //Serialize object to byte array
            BinaryFormatter bf = new BinaryFormatter();
            MemoryStream ms = new MemoryStream();
            bf.Serialize(ms, foo);
            fooBytes = ms.ToArray();

            //Write on output
            uint size = (uint)fooBytes.Length;
            //uint size = Convert.ToUInt32(fooBytes.Length);
            Igs.writeOutputAsData("data-out", fooBytes, size);

            //Read data
            byte[] data = new byte[]{};
            Igs.readOutputAsData("data-out", ref data);

            //Deserialize          
            BinaryFormatter formatter = new BinaryFormatter();
            MemoryStream msRead = new MemoryStream(data);
            Foo readFoo = (Foo)formatter.Deserialize(msRead);
            if (readFoo != null)
            {
                Console.WriteLine("Foo X={0} Y={1} has been read", readFoo.FooX, readFoo.FooY);
            }
        }


        /// <summary>
        /// Stop our agent
        /// </summary>
        public void stop()
        {
            Igs.stop();
        }


        /// <summary>
        /// 
        /// </summary>
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
                string msg = string.Format("Helloword {0}", i);
                result = Igs.writeOutputAsString("string-out", msg);

                //Impulsion
                result = Igs.writeOutputAsImpulsion("impulsion-out");
            }
        }


        /// <summary>
        /// 
        /// </summary>
        public void testSendCall()
        {
            IntPtr argsList = IntPtr.Zero;

            _count++;
            bool even = ((_count % 2) == 0);

            Igs.addIntToArgumentsList(ref argsList, _count);
            Igs.addBoolToArgumentsList(ref argsList, even);

            Console.WriteLine("call macosAgent-->OTHER_CALL");
            Igs.sendCall("macosAgent", "OTHER_CALL", ref argsList);

            Igs.destroyArgumentsList(ref argsList);


            /*Igs.addStringToArgumentsList(ref argsList, "peyruqueou@ingenuity.io");
            Igs.addStringToArgumentsList(ref argsList, "v.peyruqueou@gmail.com");
            Igs.addStringToArgumentsList(ref argsList, "");
            Igs.addStringToArgumentsList(ref argsList, "Test envoi via IngeScape call");
            Igs.addStringToArgumentsList(ref argsList, "Salut Vincent, Cordialement, Vincent P.");

            Console.WriteLine("call igsMail-->sendMail");
            Igs.sendCall("igsMail", "sendMail", ref argsList);*/
        }

        #endregion
    }
}
