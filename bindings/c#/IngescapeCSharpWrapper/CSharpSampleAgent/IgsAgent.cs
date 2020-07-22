using Ingescape;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;

namespace CSharpSampleAgent
{
    class IgsAgent
    {
        #region Attributes

        private int _igsPort = 5670;
        private string _igsDevice = "Wi-Fi";
        private string _mappingPath = "";
        private string _licensePath = "";
        private bool _verbose = false;
        private string _documentsPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        private bool initialized = false;

        #endregion

        #region Callbacks


        static void OnInputCallback(iop_t iopType,
                                ref string name,
                                iopType_t valueType,
                                object value,
                                object myData)
        {
            if (valueType == iopType_t.IGS_IMPULSION_T)
            {
                Console.WriteLine("On Input Callback... Impulsion");
            }
            else
            {
                if (valueType == iopType_t.IGS_DATA_T)
                {
                    string converted = Encoding.UTF8.GetString((byte[])value, 0, ((byte[])value).Length);
                    Console.WriteLine("On Input Callback... value : " + converted);
                }
                else
                {
                    Console.WriteLine("On Input Callback... value : " + value);
                }
            }
        }

        static void OnOutputCallback(iop_t iopType,
                                ref string name,
                                iopType_t valueType,
                                object value,
                                object myData)
        {
            if (valueType == iopType_t.IGS_IMPULSION_T)
            {
                Console.WriteLine("On Output Callback... impulsion");
            }
            else
            {
                if (valueType == iopType_t.IGS_DATA_T)
                {
                    string converted = Encoding.UTF8.GetString((byte[])value, 0, ((byte[])value).Length);
                    Console.WriteLine("On Output Callback... value : " + converted);
                }
                else
                {
                    Console.WriteLine("On Output Callback... value : " + value);
                }
            }
        }

        static void ForcedStopCB(object data)
        {
            IgsAgent script = (IgsAgent)data;
            Console.WriteLine("ForcedStop ...");
            
        }

        

        static void OnParameterCallback(iop_t iopType,
                               ref string name,
                               iopType_t valueType,
                               object value,
                               object myData)
        {
            if (valueType == iopType_t.IGS_IMPULSION_T)
            {
                Console.WriteLine("On Parameter Callback... impulsion");
            }
            else
            {
                if (valueType == iopType_t.IGS_DATA_T)
                {
                    string converted = Encoding.UTF8.GetString((byte[])value, 0, ((byte[])value).Length);
                    Console.WriteLine("On Parameter Callback... value : " + converted);
                }
                else
                {
                    Console.WriteLine("On Parameter Callback... value : " + value);
                }
            }
        }


        static void CallExampleCB(ref string senderAgentName,
                                    ref string senderAgentUUID,
                                    ref string callName,
                                    List<CallArgument> callArguments,
                                    object myData)
        {
            Console.WriteLine("CallBack of CallExampleCB ...");
        }

        static void OnObserveBus(ref string eventInfo,
                                   ref string peerID,
                                   ref string name,
                                   ref string address,
                                   ref string channel,
                                   object myData)
        {
            if(eventInfo == "JOIN")
            {
                Console.WriteLine("peerId : " + peerID + " name : " + name + " JOIN with address : " + address);
            }
            else if(eventInfo == "EXIT")
            {
                Console.WriteLine("peerId : " + peerID + " name : " + name + " EXIT with address : " + address);
            }
        }

        #endregion


        public int GetPort()
        {
            return _igsPort;
        }

        public string GetDevice()
        {
            return _igsDevice;
        }

        public int init()
        {
            if (!initialized)
            {
                initialized = true;
                _licensePath = _documentsPath + "/IngeScape/licenses";
                int index;
                List<string> argsList = Environment.GetCommandLineArgs().ToList();
                foreach (string arg in argsList)
                {
                    string argWithoutDashs = arg.Replace("-", "");
                    switch (argWithoutDashs)
                    {
                        case "verbose":
                            _verbose = true;
                            break;
                        case "license":
                            index = argsList.IndexOf(arg);
                            _licensePath = argsList[index + 1];
                            break;
                        case "mapping":
                            break;
                        case "port":
                            index = argsList.IndexOf(arg);
                            _igsPort = Convert.ToInt32(argsList[index + 1]);
                            break;
                        case "device":
                            index = argsList.IndexOf(arg);
                            _igsDevice = argsList[index + 1];
                            break;
                        case "help":
                            print_usage();
                            Environment.Exit(0);
                            break;

                    }
                }

                Igs.setAgentName("CSharpSample");
                Igs.setDefinitionName("CSharpSample def");
                Igs.setDefinitionVersion("1.0");
                Igs.setDefinitionDescription("CSharpSample");
                Igs.setLicensePath(_licensePath);
                Igs.setLogLevel(igs_logLevel_t.IGS_LOG_DEBUG);
                Igs.setVerbose(_verbose);
                Igs.setLogInFile(true);
                Igs.setLogStream(true);

                System.Text.Encoding encoding = System.Text.Encoding.GetEncoding(System.Text.Encoding.Default.BodyName);
                string[] netDevicesList = Igs.getNetDevicesList();
                foreach (string netDevice in netDevicesList)
                {
                    Console.WriteLine("netDevice " + netDevice);
                }
                              
                
                // Pass our class instance (this) in paramater of the IngeScape callback(s) 
                Igs.observeForcedStop(ForcedStopCB, this);

                Igs.createInput("boolInput", iopType_t.IGS_BOOL_T);
                Igs.createInput("stringInput", iopType_t.IGS_STRING_T);
                Igs.createInput("intInput", iopType_t.IGS_INTEGER_T);
                Igs.createInput("doubleInput", iopType_t.IGS_DOUBLE_T);
                Igs.createInput("impulsionInput", iopType_t.IGS_IMPULSION_T);
                Igs.createInput("dataInput", iopType_t.IGS_DATA_T);

                Igs.observeInput("boolInput", OnInputCallback, this);
                Igs.observeInput("stringInput", OnInputCallback, this);
                Igs.observeInput("intInput", OnInputCallback, this);
                Igs.observeInput("doubleInput", OnInputCallback, this);
                Igs.observeInput("impulsionInput", OnInputCallback, this);
                Igs.observeInput("dataInput", OnInputCallback, this);

                Igs.createOutput("boolOutput", iopType_t.IGS_BOOL_T);
                Igs.createOutput("stringOutput", iopType_t.IGS_STRING_T);
                Igs.createOutput("intOutput", iopType_t.IGS_INTEGER_T);
                Igs.createOutput("doubleOutput", iopType_t.IGS_DOUBLE_T);
                Igs.createOutput("impulsionOutput", iopType_t.IGS_IMPULSION_T);
                Igs.createOutput("dataOutput", iopType_t.IGS_DATA_T);

                Igs.observeOutput("boolOutput", OnOutputCallback, this);
                Igs.observeOutput("stringOutput", OnOutputCallback, this);
                Igs.observeOutput("intOutput", OnOutputCallback, this);
                Igs.observeOutput("doubleOutput", OnOutputCallback, this);
                Igs.observeOutput("impulsionOutput", OnOutputCallback, this);
                Igs.observeOutput("dataOutput", OnOutputCallback, this);

                Igs.createParameter("boolParameter", iopType_t.IGS_BOOL_T);
                Igs.createParameter("stringParameter", iopType_t.IGS_STRING_T);
                Igs.createParameter("intParameter", iopType_t.IGS_INTEGER_T);
                Igs.createParameter("doubleParameter", iopType_t.IGS_DOUBLE_T);
                Igs.createParameter("impulsionParameter", iopType_t.IGS_IMPULSION_T);
                Igs.createParameter("dataParameter", iopType_t.IGS_DATA_T);

                Igs.observeParameter("boolParameter", OnParameterCallback, this);
                Igs.observeParameter("stringParameter", OnParameterCallback, this);
                Igs.observeParameter("intParameter", OnParameterCallback, this);
                Igs.observeParameter("doubleParameter", OnParameterCallback, this);
                Igs.observeParameter("impulsionParameter", OnParameterCallback, this);
                Igs.observeParameter("dataParameter", OnParameterCallback, this);

                Igs.observeBus(OnObserveBus, this);


                #region initCall 

                int res = Igs.initCall("CallExample", CallExampleCB, this);

                #endregion

                return Igs.startWithDevice(_igsDevice, _igsPort);
            }
            return 0;
           
        }

        void print_usage()
        {
            Console.Write("Usage example: unitySample --verbose --port 5670 --device en0\n");
            Console.Write("\nthese parameters have default value (indicated here above):\n");
            Console.Write(String.Format("--mapping : path to the mapping file (default: {0})\n", "none"));
            Console.Write("--verbose : enable verbose mode in the application (default is disabled)\n");
            Console.Write(String.Format("--port port_number : port used for autodiscovery between agents (default: %d)\n", _igsPort));
            Console.Write("--device device_name : name of the network device to be used (useful if several devices available)\n");
            Console.Write(String.Format("--license path : path to the ingescape licenses files (default: %s)\n", _licensePath));
        }
    }
}
