using Ingescape;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Tester
{
    [TestClass]
    public class Main
    {
        private string _agentName = "tester";
        private uint _port = 5555;
        private bool _verbose = true;

        private bool _myBool = true;
        private int _myInt = 1;
        private double _myDouble = 1.0;
        private string _myString = "my string";
        private byte[] _myData = new byte[] { 1 };

        public void testerServiceCallback(string senderAgentName, string senderAgentUUID, string serviceName, List<ServiceArgument> arguments, string token, object _myData)
        {

        }

        public void testerIOPCallback(IopType iopType, string name, IopValueType valueType, object value, object _myData)
        {

        }

        public void agentServiceCallback(Agent agent, string senderAgentName, string senderAgentUUID, string serviceName, List<ServiceArgument> arguments, string token, object _myData)
        {
            Igs.Info(string.Format("Agent {0} Service callback", agent.Name));
            if (agent.Name == "secondAgent")
            {
                Assert.AreEqual(5, arguments.Count);
                Assert.AreEqual(true, arguments[0].Value);
                Assert.AreEqual(13, arguments[1].Value);
                Assert.AreEqual(13.3, arguments[2].Value);
                Assert.AreEqual("my string arg", arguments[3].Value);
                Assert.AreEqual(Convert.ToBase64String(new byte[] { 0, 0, 1, 1 }), Convert.ToBase64String(arguments[4].Value as byte[]));
            }
            else if (agent.Name == "firstAgent")
            {
                Assert.AreEqual(5, arguments.Count);
                Assert.AreEqual(true, arguments[0].Value);
                Assert.AreEqual(13, arguments[1].Value);
                Assert.AreEqual(13.3, arguments[2].Value);
                Assert.AreEqual("my string arg", arguments[3].Value);
                byte[] bytes = BitConverter.GetBytes('f');
                Assert.AreEqual(Convert.ToBase64String(bytes), Convert.ToBase64String(arguments[4].Value as byte[]));
            }
        }

        public void agentIOPCallback(Agent agent, IopType iopType, string name, IopValueType valueType, object value, object _myData)
        {

        }

        //callbacks and variables for agent events
        bool TesterFirstAgentEntered = false;
        bool TesterFirstAgentKnowsUs = false;
        bool TesterFirstAgentExited = false;
        bool TesterSecondAgentEntered = false;
        bool TesterSecondAgentKnowsUs = false;
        bool TesterSecondAgentExited = false;
        public void agentEvent(AgentEvent agentEvent, string uuid, string name, object eventData, object myCbData)
        {
            if (name == "firstAgent")
            {
                if (agentEvent == AgentEvent.AgentEntered)
                    TesterFirstAgentEntered = true;
                if (agentEvent == AgentEvent.AgentKnowsUs)
                    TesterFirstAgentKnowsUs = true;
                if (agentEvent == AgentEvent.AgentExited)
                    TesterFirstAgentExited = true;
            }
            else if (name == "secondAgent")
            {
                if (agentEvent == AgentEvent.AgentEntered)
                    TesterSecondAgentEntered = true;
                if (agentEvent == AgentEvent.AgentKnowsUs)
                    TesterSecondAgentKnowsUs = true;
                if (agentEvent == AgentEvent.AgentExited)
                    TesterSecondAgentExited = true;
            }
        }

        Agent FirstAgent = null;
        Agent SecondAgent = null;
        bool FirstSecondAgentEntered = false;
        bool FirstSecondAgentKnowsUs = false;
        bool FirstSecondAgentExited = false;
        bool FirstTesterAgentEntered = false;
        bool FirstTestergentKnowsUs = false;
        bool FirstTesterAgentExited = false;
        bool SecondFirstAgentEntered = false;
        bool SecondFirstAgentKnowsUs = false;
        bool SecondFirstAgentExited = false;
        bool SecondTesterAgentEntered = false;
        bool SecondTestergentKnowsUs = false;
        bool SecondTesterAgentExited = false;
        void agentEvent2(Agent agent, AgentEvent agentEvent, string uuid, string name, object eventData, object myCbData)
        {
            Assert.IsTrue(agent == FirstAgent || agent == SecondAgent);
            if (agent == FirstAgent)
            {
                if (name == "tester")
                {
                    if (agentEvent == AgentEvent.AgentEntered)
                        FirstTesterAgentEntered = true;
                    if (agentEvent == AgentEvent.AgentKnowsUs)
                        FirstTestergentKnowsUs = true;
                    if (agentEvent == AgentEvent.AgentExited)
                        FirstTesterAgentExited = true;
                }
                else if (name == "secondAgent")
                {
                    if (agentEvent == AgentEvent.AgentEntered)
                        FirstSecondAgentEntered = true;
                    if (agentEvent == AgentEvent.AgentKnowsUs)
                        FirstSecondAgentKnowsUs = true;
                    if (agentEvent == AgentEvent.AgentExited)
                        FirstSecondAgentExited = true;
                }
            }
            else if (agent == SecondAgent)
            {
                if (name == "tester")
                {
                    if (agentEvent == AgentEvent.AgentEntered)
                        SecondTesterAgentEntered = true;
                    if (agentEvent == AgentEvent.AgentKnowsUs)
                        SecondTestergentKnowsUs = true;
                    if (agentEvent == AgentEvent.AgentExited)
                        SecondTesterAgentExited = true;
                }
                else if (name == "firstAgent")
                {
                    if (agentEvent == AgentEvent.AgentEntered)
                        SecondFirstAgentEntered = true;
                    if (agentEvent == AgentEvent.AgentKnowsUs)
                        SecondFirstAgentKnowsUs = true;
                    if (agentEvent == AgentEvent.AgentExited)
                        SecondFirstAgentExited = true;
                }
            }
        }

        void TimerFunction(int timerId, object _myData)
        {

        }

        [TestMethod]
        public void Tester()
        {
            Igs.LogIncludeData(true);
            Igs.LogIncludeServices(true);

            //agent name and uuid
            Igs.AgentSetName("simple Demo Agent &");
            Assert.AreEqual("simple_Demo_Agent_&", Igs.AgentName());
            Igs.AgentSetName(_agentName);
            string uuid = Igs.AgentUUID();
            Assert.IsNotNull(Igs.AgentUUID());

            //agent family
            string family = Igs.AgentFamily();
            Assert.IsTrue(string.IsNullOrEmpty(family));
            Igs.AgentSetFamily("family_test_&");
            family = Igs.AgentFamily();
            Assert.AreEqual("family_test_&", family);

            //logs
            string logPath = Igs.LogFilePath();
            Assert.IsTrue(string.IsNullOrEmpty(logPath));
            Igs.LogSetConsole(true);
            Assert.IsTrue(Igs.LogConsole());
            Igs.LogSetStream(true);
            Assert.IsTrue(Igs.LogStream());
            Igs.LogSetFilePath("/tmp/Log.txt");
            logPath = Igs.LogFilePath();
            Assert.IsTrue(!string.IsNullOrEmpty(logPath));
            Assert.AreEqual("/tmp/Log.txt", logPath);
            Igs.LogSetFile(true);
            logPath = Igs.LogFilePath();
            Assert.IsTrue(string.IsNullOrEmpty(logPath));
            Assert.IsTrue(Igs.LogFile());
            Igs.LogSetConsoleLevel(LogLevel.LogTrace);
            Assert.AreEqual(LogLevel.LogTrace, Igs.LogConsoleLevel());
            Igs.LogSetFileLevel(LogLevel.LogTrace);
            Igs.LogSetFileMaxLineLength(1024);
            Igs.Trace(string.Format("trace example {0}", 1));
            Igs.Debug(string.Format("debug  example {0}", 2));
            Igs.Info(string.Format("info example {0}", 3));
            Igs.Warn(string.Format("warn example {0}", 4));
            Igs.Error(string.Format("error example {0}", 5));
            Igs.Fatal(string.Format("fatal example {0}", 6));
            Igs.Info(string.Format("multi-line Log \n second line"));
            logPath = Igs.LogFilePath();
            Assert.IsTrue(logPath.Length > 0);

            //try to write uninitialized definition and mapping (generates errors)
            Igs.DefinitionSave();
            Igs.MappingSave();

            //utilities
            Assert.IsTrue(Igs.Version() > 0);
            Assert.IsTrue(Igs.Protocol() >= 2);
            string[] devicesList = Igs.NetDevicesList();
            int nbDevices = devicesList.Count();
            for (int i = 0; i < nbDevices; i++)
            {
                Igs.Info(string.Format("device {0} - {1}", i, devicesList[i]));
            }
            devicesList = Igs.NetAddressesList();
            nbDevices = devicesList.Count();
            for (int i = 0; i < nbDevices; i++)
            {
                Igs.Info(string.Format("ip {0} - {1}", i, devicesList[i]));
            }
            Assert.IsTrue(string.IsNullOrEmpty(Igs.CommandLine()));
            Igs.SetCommandLine("my command line");
            string commandLine = Igs.CommandLine();
            Assert.AreEqual("my command line", commandLine);
            Assert.IsTrue(!Igs.MappingOutputsRequest());
            Igs.MappingSetOutputsRequest(true);
            Assert.IsTrue(Igs.MappingOutputsRequest());

            //general control functions
            Assert.IsTrue(!Igs.IsStarted());
            Igs.AgentSetState("");
            Igs.AgentSetState("my state");
            string state = Igs.AgentState();
            Assert.AreEqual("my state", state);
            Assert.IsTrue(!Igs.AgentIsMuted());
            Igs.AgentMute();
            Assert.IsTrue(Igs.AgentIsMuted());
            Igs.AgentUnmute();
            Assert.IsTrue(!Igs.AgentIsMuted());
            Assert.IsTrue(!Igs.IsFrozen());
            Igs.Freeze();
            Assert.IsTrue(Igs.IsFrozen());
            Igs.Unfreeze();
            Assert.IsTrue(!Igs.IsFrozen());

            //iops with null definition
            Assert.IsTrue(Igs.InputCount() == 0);
            Assert.IsTrue(Igs.OutputCount() == 0);
            Assert.IsTrue(Igs.ParameterCount() == 0);
            Assert.IsTrue(!Igs.InputExists("toto"));
            Assert.IsTrue(!Igs.OutputExists("toto"));
            Assert.IsTrue(!Igs.ParameterExists("toto"));
            string[] listOfStrings = null;
            listOfStrings = Igs.InputList();
            Assert.IsNull(listOfStrings);
            listOfStrings = Igs.OutputList();
            Assert.IsNull(listOfStrings);
            listOfStrings = Igs.ParameterList();
            Assert.IsNull(listOfStrings);
            Assert.IsTrue(!Igs.OutputIsMuted(null));
            Assert.IsTrue(!Igs.OutputIsMuted("toto"));
            Igs.OutputMute("toto");
            Igs.OutputUnmute("toto");
            Assert.IsTrue(!Igs.InputBool("toto"));
            Assert.IsTrue(Igs.InputInt("toto") == 0);
            Assert.IsTrue(Igs.InputDouble("toto") < 0.000001);
            Assert.IsTrue(string.IsNullOrEmpty(Igs.InputString("toto")));
            byte[] data = null;
            Assert.IsTrue(Igs.InputData("toto") == null);
            Igs.ClearInput("toto");
            Assert.IsTrue(!Igs.OutputBool("toto"));
            Assert.IsTrue(Igs.OutputInt("toto") == 0);
            Assert.IsTrue(Igs.OutputDouble("toto") < 0.000001);
            Assert.IsTrue(string.IsNullOrEmpty(Igs.OutputString("toto")));
            Assert.IsTrue(Igs.OutputData("toto") == null);
            Assert.IsTrue(!Igs.ParameterBool("toto"));
            Assert.IsTrue(Igs.ParameterInt("toto") == 0);
            Assert.IsTrue(Igs.ParameterDouble("toto") < 0.000001);
            Assert.IsTrue(string.IsNullOrEmpty(Igs.ParameterString("toto")));
            Assert.IsTrue(Igs.ParameterData("toto") == null);

            //definition - part 1
            Result strLoaded = Igs.DefinitionLoadStr("invalid json");
            Assert.IsTrue(strLoaded == Result.Failure);
            Assert.IsTrue(Igs.DefinitionLoadFile("/does not exist") == Result.Failure);
            Assert.IsTrue(!string.IsNullOrEmpty(Igs.DefinitionJson())); //intentional memory leak here
            Assert.AreEqual("tester", Igs.AgentName()); //intentional memory leak here
            Assert.IsTrue(string.IsNullOrEmpty(Igs.DefinitionDescription()));
            Assert.IsTrue(string.IsNullOrEmpty(Igs.DefinitionVersion()));
            Igs.DefinitionSetDescription("");
            Igs.DefinitionSetVersion("");
            //TODO: test loading valid string and file definitions
            Igs.DefinitionSetDescription("my description");
            string defDesc = Igs.DefinitionDescription();
            Assert.AreEqual(defDesc, "my description");
            Igs.DefinitionSetVersion("Version");
            string defVer = Igs.DefinitionVersion();
            Assert.AreEqual(defVer, "Version");
            Assert.IsTrue(Igs.InputCreate("toto", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("toto", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.ParameterCreate("toto", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("toto", IopValueType.Bool) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("toto", IopValueType.Bool) == Result.Failure);
            Assert.IsTrue(Igs.ParameterCreate("toto", IopValueType.Bool) == Result.Failure);
            Igs.OutputMute("toto");
            Assert.IsTrue(Igs.OutputIsMuted("toto"));
            Igs.OutputUnmute("toto");
            Assert.IsTrue(!Igs.OutputIsMuted("toto"));
            Assert.IsTrue(Igs.InputRemove("toto") == Result.Success);
            Assert.IsTrue(Igs.OutputRemove("toto") == Result.Success);
            Assert.IsTrue(Igs.ParameterRemove("toto") == Result.Success);
            Assert.IsTrue(Igs.InputRemove("toto") == Result.Failure);
            Assert.IsTrue(Igs.OutputRemove("toto") == Result.Failure);
            Assert.IsTrue(Igs.ParameterRemove("toto") == Result.Failure);

            //inputs
            Assert.IsTrue(Igs.InputCreate("my impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my data", IopValueType.Data, _myData) == Result.Failure);
            listOfStrings = null;
            listOfStrings = Igs.InputList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(Igs.InputCount() == 6);
            Assert.IsTrue(Igs.InputType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.InputExists("my_impulsion"));
            Assert.IsTrue(Igs.InputType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.InputExists("my_bool"));
            Assert.IsTrue(Igs.InputType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.InputExists("my_int"));
            Assert.IsTrue(Igs.InputType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.InputExists("my_double"));
            Assert.IsTrue(Igs.InputType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.InputExists("my_string"));
            Assert.IsTrue(Igs.InputType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.InputExists("my_data"));
            Assert.IsTrue(Igs.InputBool("my_bool"));
            Assert.IsTrue(Igs.InputInt("my_int") == 1);
            Assert.IsTrue(Igs.InputDouble("my_double") - 1.0 < 0.000001);
            string inputString = Igs.InputString("my_string");
            Assert.AreEqual(inputString, "my string");
            Assert.IsTrue(Igs.InputData("my_data").Length == 1);
            data = Igs.InputData("my_impulsion");
            Assert.IsTrue(data.Length == 0);
            Assert.IsTrue(Igs.InputSetImpulsion("my_impulsion") == Result.Success);
            Assert.IsTrue(Igs.InputSetBool("", false) == Result.Failure);
            Assert.IsTrue(Igs.InputSetBool("my_bool", false) == Result.Success);
            Assert.IsTrue(!Igs.InputBool("my_bool"));
            Assert.IsTrue(Igs.InputSetInt("", 2) == Result.Failure);
            Assert.IsTrue(Igs.InputSetInt("my_int", 2) == Result.Success);
            Assert.IsTrue(Igs.InputInt("my_int") == 2);
            Assert.IsTrue(Igs.InputSetDouble("", 2) == Result.Failure);
            Assert.IsTrue(Igs.InputSetDouble("my_double", 2) == Result.Success);
            Assert.IsTrue(Igs.InputDouble("my_double") - 2 < 0.000001);
            Assert.IsTrue(Igs.InputSetString("", "new string") == Result.Failure);
            Assert.IsTrue(Igs.InputSetString("my_string", "new string") == Result.Success);
            inputString = Igs.InputString("my_string");
            Assert.AreEqual(inputString, "new string");

            //outputs
            Assert.IsTrue(Igs.OutputCreate("my impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my data", IopValueType.Data, _myData) == Result.Failure);
            listOfStrings = null;
            listOfStrings = Igs.OutputList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(Igs.OutputCount() == 6);
            Assert.IsTrue(Igs.OutputType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.OutputExists("my_impulsion"));
            Assert.IsTrue(Igs.OutputType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.OutputExists("my_bool"));
            Assert.IsTrue(Igs.OutputType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.OutputExists("my_int"));
            Assert.IsTrue(Igs.OutputType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.OutputExists("my_double"));
            Assert.IsTrue(Igs.OutputType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.OutputExists("my_string"));
            Assert.IsTrue(Igs.OutputType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.OutputExists("my_data"));
            Assert.IsTrue(Igs.OutputBool("my_bool"));
            Assert.IsTrue(Igs.OutputInt("my_int") == 1);
            Assert.IsTrue(Igs.OutputDouble("my_double") - 1.0 < 0.000001);
            string outputString = Igs.OutputString("my_string");
            Assert.AreEqual(outputString, "my string");
            outputString = null;
            Assert.IsTrue(Igs.OutputData("my_data").Length == 1);
            Assert.IsTrue(Igs.OutputSetImpulsion("my_impulsion") == Result.Success);
            Assert.IsTrue(Igs.OutputSetBool("", false) == Result.Failure);
            Assert.IsTrue(Igs.OutputSetBool("my_bool", false) == Result.Success);
            Assert.IsTrue(!Igs.OutputBool("my_bool"));
            Assert.IsTrue(Igs.OutputSetInt("", 2) == Result.Failure);
            Assert.IsTrue(Igs.OutputSetInt("my_int", 2) == Result.Success);
            Assert.IsTrue(Igs.OutputInt("my_int") == 2);
            Assert.IsTrue(Igs.OutputSetDouble("", 2) == Result.Failure);
            Assert.IsTrue(Igs.OutputSetDouble("my_double", 2) == Result.Success);
            Assert.IsTrue(Igs.OutputDouble("my_double") - 2 < 0.000001);
            Assert.IsTrue(Igs.OutputSetString("", "new string") == Result.Failure);
            Assert.IsTrue(Igs.OutputSetString("my_string", "new string") == Result.Success);
            outputString = Igs.OutputString("my_string");
            Assert.AreEqual(outputString, "new string");
            Assert.IsTrue(Igs.OutputSetData("", _myData) == Result.Failure);
            Assert.IsTrue(Igs.OutputSetData("my_data", _myData) == Result.Success);
            data = Igs.OutputData("my_data");
            Assert.IsTrue(Igs.OutputData("my_data").Length == 1);
            Igs.ClearOutput("my_data");
            Assert.IsTrue(Igs.OutputData("my_data").Length == 0);

            //parameters
            Assert.IsTrue(Igs.ParameterCreate("my impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(Igs.ParameterCreate("my impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(Igs.ParameterCreate("my bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(Igs.ParameterCreate("my bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(Igs.ParameterCreate("my int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(Igs.ParameterCreate("my int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(Igs.ParameterCreate("my double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(Igs.ParameterCreate("my double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(Igs.ParameterCreate("my string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(Igs.ParameterCreate("my string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(Igs.ParameterCreate("my data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(Igs.ParameterCreate("my data", IopValueType.Data, _myData) == Result.Failure);
            listOfStrings = null;
            listOfStrings = Igs.ParameterList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(Igs.ParameterCount() == 6);
            Assert.IsTrue(Igs.ParameterType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.ParameterExists("my_impulsion"));
            Assert.IsTrue(Igs.ParameterType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.ParameterExists("my_bool"));
            Assert.IsTrue(Igs.ParameterType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.ParameterExists("my_int"));
            Assert.IsTrue(Igs.ParameterType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.ParameterExists("my_double"));
            Assert.IsTrue(Igs.ParameterType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.ParameterExists("my_string"));
            Assert.IsTrue(Igs.ParameterType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.ParameterExists("my_data"));
            Assert.IsTrue(Igs.ParameterBool("my_bool"));
            Assert.IsTrue(Igs.ParameterInt("my_int") == 1);
            Assert.IsTrue(Igs.ParameterDouble("my_double") - 1.0 < 0.000001);
            string parameterString = Igs.ParameterString("my_string");
            Assert.AreEqual(parameterString, "my string");
            parameterString = null;
            Assert.IsTrue(Igs.ParameterData("my_data").Length == 1);
            Assert.IsTrue(Igs.ParameterSetBool("", false) == Result.Failure);
            Assert.IsTrue(Igs.ParameterSetBool("my_bool", false) == Result.Success);
            Assert.IsTrue(!Igs.ParameterBool("my_bool"));
            Assert.IsTrue(Igs.ParameterSetInt("", 2) == Result.Failure);
            Assert.IsTrue(Igs.ParameterSetInt("my_int", 2) == Result.Success);
            Assert.IsTrue(Igs.ParameterInt("my_int") == 2);
            Assert.IsTrue(Igs.ParameterSetDouble("", 2) == Result.Failure);
            Assert.IsTrue(Igs.ParameterSetDouble("my_double", 2) == Result.Success);
            Assert.IsTrue(Igs.ParameterDouble("my_double") - 2 < 0.000001);
            Assert.IsTrue(Igs.ParameterSetString("", "new string") == Result.Failure);
            Assert.IsTrue(Igs.ParameterSetString("my_string", "new string") == Result.Success);
            parameterString = Igs.ParameterString("my_string");
            Assert.AreEqual(parameterString, "new string");
            Assert.IsTrue(Igs.ParameterSetData("", _myData) == Result.Failure);
            Assert.IsTrue(Igs.ParameterSetData("my_data", _myData) == Result.Success);
            Assert.IsTrue(Igs.ParameterData("my_data").Length == 1);
            Igs.ClearParameter("my_data");
            Assert.IsTrue(Igs.ParameterData("my_data").Length == 0);


            //definition - part 2
            //TODO: compare exported def, saved file and reference file
            string exportedDef = Igs.DefinitionJson();
            Assert.IsTrue(!string.IsNullOrEmpty(exportedDef));
            Igs.DefinitionSetPath("/tmp/simple Demo Agent.json");
            Igs.DefinitionSave();
            Igs.ClearDefinition();
            Igs.ClearDefinition();
            string name = Igs.AgentName();
            Assert.AreEqual(name, "tester");
            name = null;
            defDesc = Igs.DefinitionDescription();
            Assert.IsTrue(string.IsNullOrEmpty(defDesc));
            defVer = Igs.DefinitionVersion();
            Assert.IsTrue(string.IsNullOrEmpty(defVer));
            listOfStrings = Igs.InputList();
            Assert.IsNull(listOfStrings);
            listOfStrings = Igs.OutputList();
            Assert.IsNull(listOfStrings);
            listOfStrings = Igs.ParameterList();
            Assert.IsNull(listOfStrings);
            listOfStrings = Igs.ServiceList();
            Assert.IsNull(listOfStrings);
            //////////////////////////////////
            Igs.DefinitionLoadStr(exportedDef);
            listOfStrings = null;
            listOfStrings = Igs.InputList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(Igs.InputCount() == 6);
            Assert.IsTrue(Igs.InputType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.InputExists("my_impulsion"));
            Assert.IsTrue(Igs.InputType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.InputExists("my_bool"));
            Assert.IsTrue(Igs.InputType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.InputExists("my_int"));
            Assert.IsTrue(Igs.InputType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.InputExists("my_double"));
            Assert.IsTrue(Igs.InputType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.InputExists("my_string"));
            Assert.IsTrue(Igs.InputType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.InputExists("my_data"));
            listOfStrings = Igs.OutputList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(Igs.OutputCount() == 6);
            Assert.IsTrue(Igs.OutputType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.OutputExists("my_impulsion"));
            Assert.IsTrue(Igs.OutputType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.OutputExists("my_bool"));
            Assert.IsTrue(Igs.OutputType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.OutputExists("my_int"));
            Assert.IsTrue(Igs.OutputType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.OutputExists("my_double"));
            Assert.IsTrue(Igs.OutputType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.OutputExists("my_string"));
            Assert.IsTrue(Igs.OutputType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.OutputExists("my_data"));
            Assert.IsTrue(!Igs.OutputBool("my_bool"));
            Assert.IsTrue(Igs.OutputInt("my_int") == 2);
            Assert.IsTrue(Igs.OutputDouble("my_double") - 2.0 < 0.000001);
            outputString = Igs.OutputString("my_string");
            Assert.AreEqual(outputString, "new string");
            Assert.IsTrue(Igs.OutputData("my_data").Length == 0);
            listOfStrings = Igs.ParameterList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(Igs.ParameterCount() == 6);
            Assert.IsTrue(Igs.ParameterType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.ParameterExists("my_impulsion"));
            Assert.IsTrue(Igs.ParameterType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.ParameterExists("my_bool"));
            Assert.IsTrue(Igs.ParameterType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.ParameterExists("my_int"));
            Assert.IsTrue(Igs.ParameterType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.ParameterExists("my_double"));
            Assert.IsTrue(Igs.ParameterType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.ParameterExists("my_string"));
            Assert.IsTrue(Igs.ParameterType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.ParameterExists("my_data"));
            Assert.IsTrue(!Igs.ParameterBool("my_bool"));
            Assert.IsTrue(Igs.ParameterInt("my_int") == 2);
            Assert.IsTrue(Igs.ParameterDouble("my_double") - 2.0 < 0.000001);
            parameterString = Igs.ParameterString("my_string");
            Assert.AreEqual(parameterString, "new string");
            parameterString = null;
            Assert.IsTrue(Igs.ParameterData("my_data").Length == 0);
            Igs.ClearDefinition();
            exportedDef = null;
            //////////////////////////////////
            Igs.DefinitionLoadFile("/tmp/simple Demo Agent.json");
            listOfStrings = null;
            listOfStrings = Igs.InputList();
            Assert.IsNotNull(listOfStrings);
            listOfStrings = null;
            Assert.IsTrue(Igs.InputCount() == 6);
            Assert.IsTrue(Igs.InputType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.InputExists("my_impulsion"));
            Assert.IsTrue(Igs.InputType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.InputExists("my_bool"));
            Assert.IsTrue(Igs.InputType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.InputExists("my_int"));
            Assert.IsTrue(Igs.InputType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.InputExists("my_double"));
            Assert.IsTrue(Igs.InputType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.InputExists("my_string"));
            Assert.IsTrue(Igs.InputType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.InputExists("my_data"));
            data = null;
            listOfStrings = null;
            listOfStrings = Igs.OutputList();
            Assert.IsNotNull(listOfStrings);
            listOfStrings = null;
            Assert.IsTrue(Igs.OutputCount() == 6);
            Assert.IsTrue(Igs.OutputType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.OutputExists("my_impulsion"));
            Assert.IsTrue(Igs.OutputType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.OutputExists("my_bool"));
            Assert.IsTrue(Igs.OutputType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.OutputExists("my_int"));
            Assert.IsTrue(Igs.OutputType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.OutputExists("my_double"));
            Assert.IsTrue(Igs.OutputType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.OutputExists("my_string"));
            Assert.IsTrue(Igs.OutputType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.OutputExists("my_data"));
            Assert.IsTrue(!Igs.OutputBool("my_bool"));
            Assert.IsTrue(Igs.OutputInt("my_int") == 2);
            Assert.IsTrue(Igs.OutputDouble("my_double") - 2.0 < 0.000001);
            outputString = Igs.OutputString("my_string");
            Assert.AreEqual(outputString, "new string");
            outputString = null;
            data = null;
            Assert.IsTrue(Igs.OutputData("my_data").Length == 0);
            listOfStrings = null;
            listOfStrings = Igs.ParameterList();
            Assert.IsNotNull(listOfStrings);
            listOfStrings = null;
            Assert.IsTrue(Igs.ParameterCount() == 6);
            Assert.IsTrue(Igs.ParameterType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.ParameterExists("my_impulsion"));
            Assert.IsTrue(Igs.ParameterType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.ParameterExists("my_bool"));
            Assert.IsTrue(Igs.ParameterType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.ParameterExists("my_int"));
            Assert.IsTrue(Igs.ParameterType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.ParameterExists("my_double"));
            Assert.IsTrue(Igs.ParameterType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.ParameterExists("my_string"));
            Assert.IsTrue(Igs.ParameterType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.ParameterExists("my_data"));
            Assert.IsTrue(!Igs.ParameterBool("my_bool"));
            Assert.IsTrue(Igs.ParameterInt("my_int") == 2);
            Assert.IsTrue(Igs.ParameterDouble("my_double") - 2.0 < 0.000001);
            parameterString = Igs.ParameterString("my_string");
            Assert.AreEqual(parameterString, "new string");
            parameterString = null;
            data = null;
            Assert.IsTrue(Igs.ParameterData("my_data").Length == 0);
            Igs.ClearDefinition();

            //mapping & split
            Assert.IsTrue(Igs.MappingLoadStr("invalid json") == Result.Failure);
            Assert.IsTrue(Igs.MappingLoadFile("/does not exist") == Result.Failure);
            Assert.IsTrue(Igs.MappingCount() == 0);

            Assert.IsTrue(Igs.MappingAdd("toto", "other_agent", "tata") != 0);
            ulong mapId = Igs.MappingAdd("toto", "other_agent", "tata");
            Assert.IsTrue(Igs.MappingAdd("toto", "other_agent", "tata") == mapId);
            Assert.IsTrue(mapId > 0);
            Assert.IsTrue(Igs.MappingRemoveWithId(12345) == Result.Failure);
            Assert.IsTrue(Igs.MappingRemoveWithId(mapId) == Result.Success);
            Assert.IsTrue(Igs.MappingAdd("toto", "other_agent", "tata") > 0);
            Assert.IsTrue(Igs.MappingCount() == 1);
            Assert.IsTrue(Igs.MappingRemoveWithName("toto", "other_agent", "tata") == Result.Success);
            Assert.IsTrue(Igs.MappingRemoveWithName("toto", "other_agent", "tata") == Result.Failure);
            Assert.IsTrue(Igs.MappingCount() == 0);
            Assert.IsTrue(Igs.MappingAdd("toto", "other_agent", "tata") > 0);
            Assert.IsTrue(Igs.MappingCount() == 1);
            Igs.ClearMappingsWithAgent("other_agent");
            Assert.IsTrue(Igs.MappingCount() == 0);
            Assert.IsTrue(Igs.MappingRemoveWithName("toto", "other_agent", "tata") == Result.Failure);

            Assert.IsTrue(Igs.SplitCount() == 0);
            Assert.IsTrue(Igs.SplitAdd("toto", "other_agent", "tata") != 0);
            ulong splitId = Igs.SplitAdd("toto", "other_agent", "tata");
            Assert.IsTrue(splitId > 0);
            Assert.IsTrue(Igs.SplitCount() == 1);
            Assert.IsTrue(Igs.SplitRemoveWithId(12345) == Result.Failure);
            Assert.IsTrue(Igs.SplitRemoveWithId(splitId) == Result.Success);
            Assert.IsTrue(Igs.SplitCount() == 0);
            Assert.IsTrue(Igs.SplitAdd("toto", "other_agent", "tata") != 0);
            Assert.IsTrue(Igs.SplitCount() == 1);
            Assert.IsTrue(Igs.SplitRemoveWithName("toto", "other_agent", "tata") == Result.Success);
            Assert.IsTrue(Igs.SplitCount() == 0);
            Assert.IsTrue(Igs.SplitRemoveWithName("toto", "other_agent", "tata") == Result.Failure);

            Igs.ClearMappings();
            Igs.MappingAdd("toto", "other_agent", "tata");
            Igs.SplitAdd("toto", "other_agent", "tata");
            string exportedMapping = Igs.MappingJson();
            Igs.MappingSetPath("/tmp/simple Demo Agent mapping.json");
            Igs.MappingSave();
            Igs.ClearMappings();
            Igs.MappingLoadStr(exportedMapping);
            Assert.IsTrue(Igs.MappingRemoveWithName("toto", "other_agent", "tata") == Result.Success);
            Assert.IsTrue(Igs.SplitRemoveWithName("toto", "other_agent", "tata") == Result.Success);
            Igs.ClearMappings();
            Igs.MappingLoadFile("/tmp/simple Demo Agent mapping.json");
            Assert.IsTrue(Igs.MappingRemoveWithName("toto", "other_agent", "tata") == Result.Success);
            Assert.IsTrue(Igs.SplitRemoveWithName("toto", "other_agent", "tata") == Result.Success);
            Igs.ClearMappings();

            //services
            Assert.IsTrue(Igs.ServiceInit("myService", testerServiceCallback, null) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "_myBool", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "_myInt", IopValueType.Integer) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "_myDouble", IopValueType.Double) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "_myString", IopValueType.String) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "_myData", IopValueType.Data) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgsCount("myService") == 5);
            Assert.IsTrue(Igs.ServiceArgExists("myService", "_myBool"));
            Assert.IsTrue(Igs.ServiceArgExists("myService", "_myInt"));
            Assert.IsTrue(Igs.ServiceArgExists("myService", "_myDouble"));
            Assert.IsTrue(Igs.ServiceArgExists("myService", "_myString"));
            Assert.IsTrue(Igs.ServiceArgExists("myService", "_myData"));
            List<ServiceArgument> list = Igs.ServiceArgumentsList("myService");
            Assert.IsNotNull(list);
            Assert.AreEqual(list[0].Name, "_myBool");
            Assert.IsTrue(list[0].Type == IopValueType.Bool);
            Assert.IsFalse((bool)list[0].Value);
            Assert.AreEqual(list[1].Name, "_myInt");
            Assert.IsTrue(list[1].Type == IopValueType.Integer);
            Assert.IsTrue((int)list[1].Value == 0);
            Assert.AreEqual(list[2].Name, "_myDouble");
            Assert.IsTrue(list[2].Type == IopValueType.Double);
            Assert.IsTrue((double)list[2].Value < 0.000001);
            Assert.AreEqual(list[3].Name, "_myString");
            Assert.IsTrue(list[3].Type == IopValueType.String);
            Assert.IsTrue(list[3].Value == string.Empty);
            Assert.AreEqual(list[4].Name, "_myData");
            Assert.IsTrue(list[4].Type == IopValueType.Data);
            Assert.IsTrue(list[4].Value == null);
            Igs.DefinitionSave();

            //prepare agent for dynamic tests by adding proper complete definitions
            Igs.AgentSetName(_agentName);
            Igs.LogSetConsole(_verbose);

            Igs.AgentSetName(_agentName);
            Igs.DefinitionSetDescription("One example for each type of IOP and call");
            Igs.DefinitionSetVersion("1.0");
            Igs.InputCreate("my_impulsion", IopValueType.Impulsion);
            Igs.InputCreate("my_bool", IopValueType.Bool, _myBool);
            Igs.InputCreate("my_int", IopValueType.Integer, _myInt);
            Igs.InputCreate("my_double", IopValueType.Double, _myDouble);
            Igs.InputCreate("my_string", IopValueType.String, _myString);
            Igs.InputCreate("my_data", IopValueType.Data, _myData);
            Igs.InputCreate("my_impulsion_split", IopValueType.Impulsion);
            Igs.InputCreate("my_bool_split", IopValueType.Bool, _myBool);
            Igs.InputCreate("my_int_split", IopValueType.Integer, _myInt);
            Igs.InputCreate("my_double_split", IopValueType.Double, _myDouble);
            Igs.InputCreate("my_string_split", IopValueType.String, _myString);
            Igs.InputCreate("my_data_split", IopValueType.Data, _myData);
            Igs.OutputCreate("my_impulsion", IopValueType.Impulsion);
            Igs.OutputCreate("my_bool", IopValueType.Bool, _myBool);
            Igs.OutputCreate("my_int", IopValueType.Integer, _myInt);
            Igs.OutputCreate("my_double", IopValueType.Double, _myDouble);
            Igs.OutputCreate("my_string", IopValueType.String, _myString);
            Igs.OutputCreate("my_data", IopValueType.Data, _myData);
            Igs.ParameterCreate("my_impulsion", IopValueType.Impulsion);
            Igs.ParameterCreate("my_bool", IopValueType.Bool, _myBool);
            Igs.ParameterCreate("my_int", IopValueType.Integer, _myInt);
            Igs.ParameterCreate("my_double", IopValueType.Double, _myDouble);
            Igs.ParameterCreate("my_string", IopValueType.String, _myString);
            Igs.ParameterCreate("my_data", IopValueType.Data, _myData);
            Igs.ServiceInit("myService", testerServiceCallback, null);
            Igs.ServiceArgAdd("myService", "_myBool", IopValueType.Bool);
            Igs.ServiceArgAdd("myService", "_myInt", IopValueType.Integer);
            Igs.ServiceArgAdd("myService", "_myDouble", IopValueType.Double);
            Igs.ServiceArgAdd("myService", "_myString", IopValueType.String);
            Igs.ServiceArgAdd("myService", "_myData", IopValueType.Data);

            Igs.ObserveInput("my_impulsion", testerIOPCallback, null);
            Igs.ObserveInput("my_bool", testerIOPCallback, null);
            Igs.ObserveInput("my_int", testerIOPCallback, null);
            Igs.ObserveInput("my_double", testerIOPCallback, null);
            Igs.ObserveInput("my_string", testerIOPCallback, null);
            Igs.ObserveInput("my_data", testerIOPCallback, null);
            Igs.ObserveInput("my_impulsion_split", testerIOPCallback, null);
            Igs.ObserveInput("my_bool_split", testerIOPCallback, null);
            Igs.ObserveInput("my_int_split", testerIOPCallback, null);
            Igs.ObserveInput("my_double_split", testerIOPCallback, null);
            Igs.ObserveInput("my_string_split", testerIOPCallback, null);
            Igs.ObserveInput("my_data_split", testerIOPCallback, null);

            Igs.MappingAdd("my_impulsion", "partner", "sparing_impulsion");
            Igs.MappingAdd("my_bool", "partner", "sparing_bool");
            Igs.MappingAdd("my_int", "partner", "sparing_int");
            Igs.MappingAdd("my_double", "partner", "sparing_double");
            Igs.MappingAdd("my_string", "partner", "sparing_string");
            Igs.MappingAdd("my_data", "partner", "sparing_data");

            Igs.SplitAdd("my_impulsion_split", "partner", "sparing_impulsion");
            Igs.SplitAdd("my_bool_split", "partner", "sparing_bool");
            Igs.SplitAdd("my_int_split", "partner", "sparing_int");
            Igs.SplitAdd("my_double_split", "partner", "sparing_double");
            Igs.SplitAdd("my_string_split", "partner", "sparing_string");
            Igs.SplitAdd("my_data_split", "partner", "sparing_data");


            //add multiple agents to be enabled and disabled on demand
            //first additional agent is activated immediately
            FirstAgent = new Agent("firstAgent_bogus", true);
            Assert.AreEqual("firstAgent_bogus", FirstAgent.Name); //intentional memory leak here
            FirstAgent.Name = "firstAgent";
            Assert.AreEqual("firstAgent", FirstAgent.Name); //intentional memory leak here
            FirstAgent.DefinitionDescription = "First virtual agent";

            FirstAgent.DefinitionVersion = "1.0";

            FirstAgent.InputCreate("Firstimpulsion", IopValueType.Impulsion);
            FirstAgent.InputCreate("Firstbool", IopValueType.Bool, _myBool);
            FirstAgent.InputCreate("Firstint", IopValueType.Integer, _myInt);
            FirstAgent.InputCreate("Firstdouble", IopValueType.Double, _myDouble);
            FirstAgent.InputCreate("Firststring", IopValueType.String, _myString);
            FirstAgent.InputCreate("Firstdata", IopValueType.Data, _myData);
            FirstAgent.InputCreate("Firstimpulsion_split", IopValueType.Impulsion);
            FirstAgent.InputCreate("Firstbool_split", IopValueType.Bool, _myBool);
            FirstAgent.InputCreate("Firstint_split", IopValueType.Integer, _myInt);
            FirstAgent.InputCreate("Firstdouble_split", IopValueType.Double, _myDouble);
            FirstAgent.InputCreate("Firststring_split", IopValueType.String, _myString);
            FirstAgent.InputCreate("Firstdata_split", IopValueType.Data, _myData);
            FirstAgent.OutputCreate("Firstimpulsion", IopValueType.Impulsion);
            FirstAgent.OutputCreate("Firstbool", IopValueType.Bool, _myBool);
            FirstAgent.OutputCreate("Firstint", IopValueType.Integer, _myInt);
            FirstAgent.OutputCreate("Firstdouble", IopValueType.Double, _myDouble);
            FirstAgent.OutputCreate("Firststring", IopValueType.String, _myString);
            FirstAgent.OutputCreate("Firstdata", IopValueType.Data, _myData);
            FirstAgent.ParameterCreate("Firstimpulsion", IopValueType.Impulsion);
            FirstAgent.ParameterCreate("Firstbool", IopValueType.Bool, _myBool);
            FirstAgent.ParameterCreate("Firstint", IopValueType.Integer, _myInt);
            FirstAgent.ParameterCreate("Firstdouble", IopValueType.Double, _myDouble);
            FirstAgent.ParameterCreate("Firststring", IopValueType.String, _myString);
            FirstAgent.ParameterCreate("Firstdata", IopValueType.Data, _myData);

            FirstAgent.ServiceInit("firstService", agentServiceCallback, null);
            FirstAgent.ServiceArgAdd("firstService", "firstBool", IopValueType.Bool);
            FirstAgent.ServiceArgAdd("firstService", "firstInt", IopValueType.Integer);
            FirstAgent.ServiceArgAdd("firstService", "firstDouble", IopValueType.Double);
            FirstAgent.ServiceArgAdd("firstService", "firstString", IopValueType.String);
            FirstAgent.ServiceArgAdd("firstService", "firstData", IopValueType.Data);

            FirstAgent.ObserveInput("Firstimpulsion", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firstbool", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firstint", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firstdouble", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firststring", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firstdata", agentIOPCallback, null);

            FirstAgent.ObserveInput("Firstimpulsion_split", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firstbool_split", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firstint_split", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firstdouble_split", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firststring_split", agentIOPCallback, null);
            FirstAgent.ObserveInput("Firstdata_split", agentIOPCallback, null);

            FirstAgent.MappingAdd("Firstimpulsion", "partner", "sparing_impulsion");
            FirstAgent.MappingAdd("Firstbool", "partner", "sparing_bool");
            FirstAgent.MappingAdd("Firstint", "partner", "sparing_int");
            FirstAgent.MappingAdd("Firstdouble", "partner", "sparing_double");
            FirstAgent.MappingAdd("Firststring", "partner", "sparing_string");
            FirstAgent.MappingAdd("Firstdata", "partner", "sparing_data");

            FirstAgent.SplitAdd("Firstimpulsion_split", "partner", "sparing_impulsion");
            FirstAgent.SplitAdd("Firstbool_split", "partner", "sparing_bool");
            FirstAgent.SplitAdd("Firstint_split", "partner", "sparing_int");
            FirstAgent.SplitAdd("Firstdouble_split", "partner", "sparing_double");
            FirstAgent.SplitAdd("Firststring_split", "partner", "sparing_string");
            FirstAgent.SplitAdd("Firstdata_split", "partner", "sparing_data");

            //second additional agent is NOT activated immediately
            SecondAgent = new Agent("secondAgent", false);
            SecondAgent.DefinitionDescription = "Second virtual agent";
            SecondAgent.DefinitionVersion = "1.0";
            SecondAgent.InputCreate("second_impulsion", IopValueType.Impulsion);
            SecondAgent.InputCreate("second_bool", IopValueType.Bool, _myBool);
            SecondAgent.InputCreate("second_int", IopValueType.Integer, _myInt);
            SecondAgent.InputCreate("second_double", IopValueType.Double, _myDouble);
            SecondAgent.InputCreate("second_string", IopValueType.String, _myString);
            SecondAgent.InputCreate("second_data", IopValueType.Data, _myData);
            SecondAgent.InputCreate("second_impulsion_split", IopValueType.Impulsion);
            SecondAgent.InputCreate("second_bool_split", IopValueType.Bool, _myBool);
            SecondAgent.InputCreate("second_int_split", IopValueType.Integer, _myInt);
            SecondAgent.InputCreate("second_double_split", IopValueType.Double, _myDouble);
            SecondAgent.InputCreate("second_string_split", IopValueType.String, _myString);
            SecondAgent.InputCreate("second_data_split", IopValueType.Data, _myData);
            SecondAgent.OutputCreate("second_impulsion", IopValueType.Impulsion);
            SecondAgent.OutputCreate("second_bool", IopValueType.Bool, _myBool);
            SecondAgent.OutputCreate("second_int", IopValueType.Integer, _myInt);
            SecondAgent.OutputCreate("second_double", IopValueType.Double, _myDouble);
            SecondAgent.OutputCreate("second_string", IopValueType.String, _myString);
            SecondAgent.OutputCreate("second_data", IopValueType.Data, _myData);
            SecondAgent.ParameterCreate("second_impulsion", IopValueType.Impulsion);
            SecondAgent.ParameterCreate("second_bool", IopValueType.Bool, _myBool);
            SecondAgent.ParameterCreate("second_int", IopValueType.Integer, _myInt);
            SecondAgent.ParameterCreate("second_double", IopValueType.Double, _myDouble);
            SecondAgent.ParameterCreate("second_string", IopValueType.String, _myString);
            SecondAgent.ParameterCreate("second_data", IopValueType.Data, _myData);
            SecondAgent.ServiceInit("secondService", agentServiceCallback, null);
            SecondAgent.ServiceArgAdd("secondService", "secondBool", IopValueType.Bool);
            SecondAgent.ServiceArgAdd("secondService", "secondInt", IopValueType.Integer);
            SecondAgent.ServiceArgAdd("secondService", "secondDouble", IopValueType.Double);
            SecondAgent.ServiceArgAdd("secondService", "secondString", IopValueType.String);
            SecondAgent.ServiceArgAdd("secondService", "secondData", IopValueType.Data);

            SecondAgent.ObserveInput("second_impulsion", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_bool", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_int", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_double", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_string", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_data", agentIOPCallback, null);

            SecondAgent.ObserveInput("second_impulsion_split", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_bool_split", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_int_split", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_double_split", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_string_split", agentIOPCallback, null);
            SecondAgent.ObserveInput("second_data_split", agentIOPCallback, null);

            SecondAgent.MappingAdd("second_impulsion", "partner", "sparing_impulsion");
            SecondAgent.MappingAdd("second_bool", "partner", "sparing_bool");
            SecondAgent.MappingAdd("second_int", "partner", "sparing_int");
            SecondAgent.MappingAdd("second_double", "partner", "sparing_double");
            SecondAgent.MappingAdd("second_string", "partner", "sparing_string");
            SecondAgent.MappingAdd("second_data", "partner", "sparing_data");

            SecondAgent.MappingAdd("second_impulsion", "tester", "my_impulsion");
            SecondAgent.MappingAdd("second_bool", "tester", "my_bool");
            SecondAgent.MappingAdd("second_int", "tester", "my_int");
            SecondAgent.MappingAdd("second_double", "tester", "my_double");
            SecondAgent.MappingAdd("second_string", "tester", "my_string");
            SecondAgent.MappingAdd("second_data", "tester", "my_data");

            SecondAgent.MappingAdd("second_impulsion", "FirstAgent", "Firstimpulsion");
            SecondAgent.MappingAdd("second_bool", "FirstAgent", "Firstbool");
            SecondAgent.MappingAdd("second_int", "FirstAgent", "Firstint");
            SecondAgent.MappingAdd("second_double", "FirstAgent", "Firstdouble");
            SecondAgent.MappingAdd("second_string", "FirstAgent", "Firststring");
            SecondAgent.MappingAdd("second_data", "FirstAgent", "Firstdata");

            SecondAgent.SplitAdd("second_impulsion_split", "partner", "sparing_impulsion");
            SecondAgent.SplitAdd("second_bool_split", "partner", "sparing_bool");
            SecondAgent.SplitAdd("second_int_split", "partner", "sparing_int");
            SecondAgent.SplitAdd("second_double_split", "partner", "sparing_double");
            SecondAgent.SplitAdd("second_string_split", "partner", "sparing_string");
            SecondAgent.SplitAdd("second_data_split", "partner", "sparing_data");

            SecondAgent.SplitAdd("second_impulsion_split", "tester", "my_impulsion");
            SecondAgent.SplitAdd("second_bool_split", "tester", "my_bool");
            SecondAgent.SplitAdd("second_int_split", "tester", "my_int");
            SecondAgent.SplitAdd("second_double_split", "tester", "my_double");
            SecondAgent.SplitAdd("second_string_split", "tester", "my_string");
            SecondAgent.SplitAdd("second_data_split", "tester", "my_data");

            SecondAgent.SplitAdd("second_impulsion_split", "FirstAgent", "Firstimpulsion");
            SecondAgent.SplitAdd("second_bool_split", "FirstAgent", "Firstbool");
            SecondAgent.SplitAdd("second_int_split", "FirstAgent", "Firstint");
            SecondAgent.SplitAdd("second_double_split", "FirstAgent", "Firstdouble");
            SecondAgent.SplitAdd("second_string_split", "FirstAgent", "Firststring");
            SecondAgent.SplitAdd("second_data_split", "FirstAgent", "Firstdata");

            //test mapping in same process between second_agent and first_agent
            //TODO :
            SecondAgent.Activate();
            //test service in the same process
            FirstAgent.ServiceCall("secondAgent", "secondService", new object[] { true, 13, 13.3, "my string arg", new byte[] { 0, 0, 1, 1 } }, "token");
            SecondAgent.ServiceCall("firstAgent", "firstService", new object[] { true, 13, 13.3, "my string arg", BitConverter.GetBytes('f') }, "token");

            //test agent events in same process
            SecondAgent.Deactivate();
            FirstAgent.Deactivate();
            Igs.ObserveAgentEvents(agentEvent, this);
            FirstAgent.ObserveAgentEvents(agentEvent2, this);
            SecondAgent.ObserveAgentEvents(agentEvent2, this);
            FirstAgent.Activate();

            Assert.IsTrue(TesterFirstAgentEntered);
            Assert.IsTrue(TesterFirstAgentKnowsUs);
            Assert.IsTrue(FirstTesterAgentEntered);
            Assert.IsTrue(FirstTestergentKnowsUs);
            SecondAgent.Activate();
            Assert.IsTrue(SecondFirstAgentEntered);
            Assert.IsTrue(SecondFirstAgentKnowsUs);
            Assert.IsTrue(SecondTesterAgentEntered);
            Assert.IsTrue(SecondTestergentKnowsUs);
            Assert.IsTrue(TesterSecondAgentEntered);
            Assert.IsTrue(TesterSecondAgentKnowsUs);
            Assert.IsTrue(FirstSecondAgentEntered);
            Assert.IsTrue(FirstSecondAgentKnowsUs);
            FirstAgent.Deactivate();
            Assert.IsTrue(TesterFirstAgentExited);
            Assert.IsTrue(SecondFirstAgentExited);
            SecondAgent.Deactivate();
            Assert.IsTrue(TesterSecondAgentExited);

            FirstAgent.Activate();
            SecondAgent.Activate();

            //elections
            Assert.IsTrue(Igs.ElectionLeave("my election") == Result.Failure);
            Assert.IsTrue(Igs.ElectionJoin("my election") == Result.Success);
            Assert.IsTrue(Igs.ElectionJoin("my election") == Result.Failure);
            Assert.IsTrue(Igs.ElectionJoin("INGESCAPE_PRIVATE") == Result.Failure);
            Assert.IsTrue(Igs.ElectionLeave("my election") == Result.Success);
            Assert.IsTrue(Igs.ElectionLeave("my election") == Result.Failure);
            Assert.IsTrue(Igs.ElectionLeave("my other election") == Result.Failure);
            Assert.IsTrue(Igs.ElectionJoin("my other election") == Result.Success);
            Assert.IsTrue(Igs.ElectionJoin("my other election") == Result.Failure);
            Assert.IsTrue(Igs.ElectionLeave("my other election") == Result.Success);
            Assert.IsTrue(Igs.ElectionLeave("my other election") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionLeave("my election") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionJoin("my election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionJoin("my election") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionJoin("INGESCAPE_PRIVATE") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionLeave("my election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionLeave("my election") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionLeave("my other election") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionJoin("my other election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionJoin("my other election") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionLeave("my other election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionLeave("my other election") == Result.Failure);

            FirstAgent.ClearDefinition();

            //inputs
            Assert.IsTrue(FirstAgent.InputCreate("my impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my data", IopValueType.Data, _myData) == Result.Failure);
            listOfStrings = null;
            listOfStrings = FirstAgent.InputList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(FirstAgent.InputCount() == 6);
            Assert.IsTrue(FirstAgent.InputType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(FirstAgent.InputExists("my_impulsion"));
            Assert.IsTrue(FirstAgent.InputType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(FirstAgent.InputExists("my_bool"));
            Assert.IsTrue(FirstAgent.InputType("my_int") == IopValueType.Integer);
            Assert.IsTrue(FirstAgent.InputExists("my_int"));
            Assert.IsTrue(FirstAgent.InputType("my_double") == IopValueType.Double);
            Assert.IsTrue(FirstAgent.InputExists("my_double"));
            Assert.IsTrue(FirstAgent.InputType("my_string") == IopValueType.String);
            Assert.IsTrue(FirstAgent.InputExists("my_string"));
            Assert.IsTrue(FirstAgent.InputType("my_data") == IopValueType.Data);
            Assert.IsTrue(FirstAgent.InputExists("my_data"));
            Assert.IsTrue(FirstAgent.InputBool("my_bool"));
            Assert.IsTrue(FirstAgent.InputInt("my_int") == 1);
            Assert.IsTrue(FirstAgent.InputDouble("my_double") - 1.0 < 0.000001);
            inputString = FirstAgent.InputString("my_string");
            Assert.AreEqual(inputString, "my string");
            Assert.IsTrue(FirstAgent.InputData("my_data").Length == 1);
            data = FirstAgent.InputData("my_impulsion");
            Assert.IsTrue(data.Length == 0);
            Assert.IsTrue(FirstAgent.InputSetImpulsion("my_impulsion") == Result.Success);
            Assert.IsTrue(FirstAgent.InputSetBool("", false) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputSetBool("my_bool", false) == Result.Success);
            Assert.IsTrue(!FirstAgent.InputBool("my_bool"));
            Assert.IsTrue(FirstAgent.InputSetInt("", 2) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputSetInt("my_int", 2) == Result.Success);
            Assert.IsTrue(FirstAgent.InputInt("my_int") == 2);
            Assert.IsTrue(FirstAgent.InputSetDouble("", 2) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputSetDouble("my_double", 2) == Result.Success);
            Assert.IsTrue(FirstAgent.InputDouble("my_double") - 2 < 0.000001);
            Assert.IsTrue(FirstAgent.InputSetString("", "new string") == Result.Failure);
            Assert.IsTrue(FirstAgent.InputSetString("my_string", "new string") == Result.Success);
            inputString = FirstAgent.InputString("my_string");
            Assert.AreEqual(inputString, "new string");

            //outputs
            Assert.IsTrue(FirstAgent.OutputCreate("my impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my data", IopValueType.Data, _myData) == Result.Failure);
            listOfStrings = null;
            listOfStrings = FirstAgent.OutputList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(FirstAgent.OutputCount() == 6);
            Assert.IsTrue(FirstAgent.OutputType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(FirstAgent.OutputExists("my_impulsion"));
            Assert.IsTrue(FirstAgent.OutputType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(FirstAgent.OutputExists("my_bool"));
            Assert.IsTrue(FirstAgent.OutputType("my_int") == IopValueType.Integer);
            Assert.IsTrue(FirstAgent.OutputExists("my_int"));
            Assert.IsTrue(FirstAgent.OutputType("my_double") == IopValueType.Double);
            Assert.IsTrue(FirstAgent.OutputExists("my_double"));
            Assert.IsTrue(FirstAgent.OutputType("my_string") == IopValueType.String);
            Assert.IsTrue(FirstAgent.OutputExists("my_string"));
            Assert.IsTrue(FirstAgent.OutputType("my_data") == IopValueType.Data);
            Assert.IsTrue(FirstAgent.OutputExists("my_data"));
            Assert.IsTrue(FirstAgent.OutputBool("my_bool"));
            Assert.IsTrue(FirstAgent.OutputInt("my_int") == 1);
            Assert.IsTrue(FirstAgent.OutputDouble("my_double") - 1.0 < 0.000001);
            outputString = FirstAgent.OutputString("my_string");
            Assert.AreEqual(outputString, "my string");
            outputString = null;
            Assert.IsTrue(FirstAgent.OutputSetImpulsion("my_impulsion") == Result.Success);
            Assert.IsTrue(FirstAgent.OutputData("my_data").Length == 1);
            Assert.IsTrue(FirstAgent.OutputSetBool("", false) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputSetBool("my_bool", false) == Result.Success);
            Assert.IsTrue(!FirstAgent.OutputBool("my_bool"));
            Assert.IsTrue(FirstAgent.OutputSetInt("", 2) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputSetInt("my_int", 2) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputInt("my_int") == 2);
            Assert.IsTrue(FirstAgent.OutputSetDouble("", 2) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputSetDouble("my_double", 2) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputDouble("my_double") - 2 < 0.000001);
            Assert.IsTrue(FirstAgent.OutputSetString("", "new string") == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputSetString("my_string", "new string") == Result.Success);
            outputString = FirstAgent.OutputString("my_string");
            Assert.AreEqual(outputString, "new string");
            Assert.IsTrue(FirstAgent.OutputSetData("", _myData) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputSetData("my_data", _myData) == Result.Success);
            data = FirstAgent.OutputData("my_data");
            Assert.IsTrue(FirstAgent.OutputData("my_data").Length == 1);
            FirstAgent.ClearOutput("my_data");
            Assert.IsTrue(FirstAgent.OutputData("my_data").Length == 0);

            //parameters
            Assert.IsTrue(FirstAgent.ParameterCreate("my impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterCreate("my impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterCreate("my bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterCreate("my bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterCreate("my int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterCreate("my int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterCreate("my double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterCreate("my double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterCreate("my string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterCreate("my string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterCreate("my data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterCreate("my data", IopValueType.Data, _myData) == Result.Failure);
            listOfStrings = null;
            listOfStrings = FirstAgent.ParameterList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(FirstAgent.ParameterCount() == 6);
            Assert.IsTrue(FirstAgent.ParameterType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(FirstAgent.ParameterExists("my_impulsion"));
            Assert.IsTrue(FirstAgent.ParameterType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(FirstAgent.ParameterExists("my_bool"));
            Assert.IsTrue(FirstAgent.ParameterType("my_int") == IopValueType.Integer);
            Assert.IsTrue(FirstAgent.ParameterExists("my_int"));
            Assert.IsTrue(FirstAgent.ParameterType("my_double") == IopValueType.Double);
            Assert.IsTrue(FirstAgent.ParameterExists("my_double"));
            Assert.IsTrue(FirstAgent.ParameterType("my_string") == IopValueType.String);
            Assert.IsTrue(FirstAgent.ParameterExists("my_string"));
            Assert.IsTrue(FirstAgent.ParameterType("my_data") == IopValueType.Data);
            Assert.IsTrue(FirstAgent.ParameterExists("my_data"));
            Assert.IsTrue(FirstAgent.ParameterBool("my_bool"));
            Assert.IsTrue(FirstAgent.ParameterInt("my_int") == 1);
            Assert.IsTrue(FirstAgent.ParameterDouble("my_double") - 1.0 < 0.000001);
            parameterString = FirstAgent.ParameterString("my_string");
            Assert.AreEqual(parameterString, "my string");
            parameterString = null;
            Assert.IsTrue(FirstAgent.ParameterData("my_data").Length == 1);
            Assert.IsTrue(FirstAgent.ParameterSetBool("", false) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterSetBool("my_bool", false) == Result.Success);
            Assert.IsTrue(!FirstAgent.ParameterBool("my_bool"));
            Assert.IsTrue(FirstAgent.ParameterSetInt("", 2) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterSetInt("my_int", 2) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterInt("my_int") == 2);
            Assert.IsTrue(FirstAgent.ParameterSetDouble("", 2) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterSetDouble("my_double", 2) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterDouble("my_double") - 2 < 0.000001);
            Assert.IsTrue(FirstAgent.ParameterSetString("", "new string") == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterSetString("my_string", "new string") == Result.Success);
            parameterString = FirstAgent.ParameterString("my_string");
            Assert.AreEqual(parameterString, "new string");
            Assert.IsTrue(FirstAgent.ParameterSetData("", _myData) == Result.Failure);
            Assert.IsTrue(FirstAgent.ParameterSetData("my_data", _myData) == Result.Success);
            Assert.IsTrue(FirstAgent.ParameterData("my_data").Length == 1);
            FirstAgent.ClearParameter("my_data");
            Assert.IsTrue(FirstAgent.ParameterData("my_data").Length == 0);

            Igs.AgentSetFamily("family_test");

            Assert.IsTrue(Igs.StartWithDevice("", _port) == Result.Failure);

            FirstAgent.Destroy();
            SecondAgent.Destroy();
        }

        [TestCleanup]
        public void clean()
        {
            if (Igs.IsStarted())
            {
                Igs.Stop();
            }
            Igs.ClearContext();
        }
    }
}
