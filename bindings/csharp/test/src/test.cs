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
        private uint _port = 5670;
        private string _networkDevice = "Ethernet";
        private bool _verbose = false;

        private bool _myBool = true;
        private int _myInt = 1;
        private double _myDouble = 1.0;
        private string _myString = "my string";
        private byte[] _myData = new byte[] { 1 };
        private byte[] myOtherData = new byte[] { 1 };

        public void testerServiceCallback(string senderAgentName, string senderAgentUUID, string serviceName, List<ServiceArgument> arguments, string token, object _myData)
        {

        }

        public void testerAgentServiceCallback(Agent agent, string senderAgentName, string senderAgentUUID, string serviceName, List<ServiceArgument> arguments, string token, object _myData)
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
            //agent name and uuid
            Assert.AreEqual("no_name", Igs.AgentName());
            Igs.AgentSetName("simple Demo Agent");
            Assert.AreEqual("simple Demo Agent", Igs.AgentName());
            Igs.AgentSetName(_agentName);
            string uuid = Igs.AgentUUID();
            Assert.IsNotNull(Igs.AgentUUID());

            //package and class
            string classStr = "my_class";
            Igs.DefinitionSetClass(classStr);
            Assert.AreEqual(classStr, Igs.DefinitionClass());

            string package = "my::package";
            Igs.DefinitionSetPackage(package);
            Assert.AreEqual(package, Igs.DefinitionPackage());

            //constraints
            Igs.InputCreate("constraint_impulsion", IopValueType.Impulsion);
            Igs.InputCreate("constraint_int", IopValueType.Integer);
            Igs.InputCreate("constraint_bool", IopValueType.Bool);
            Igs.InputCreate("constraint_double", IopValueType.Double);
            Igs.InputCreate("constraint_string", IopValueType.String);
            Igs.InputCreate("constraint_data", IopValueType.Data);

            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_int", "min 10.12")); //will set 10 as min constraint
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_int", "max 10.12")); //will set 10 as max constraint
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_int", "[-.1, +10.13]"));
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_int", "[-.1  ,  +10.13]"));
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_int", "[-.1,+10.13]"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_int", "[1,-10.13]"));

            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_double", "min 10.12"));
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_double", "max 10.12"));
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_double", "[-.1, +10.13]"));
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_double", "[12.12,12.12]"));

            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_bool", "min 10.12"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_bool", "max 10.12"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_bool", "[1,10.13]"));

            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_impulsion", "min 10.12"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_impulsion", "max 10.12"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_impulsion", "[1,10.13]"));

            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_string", "min 10.12"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_string", "max 10.12"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_string", "[1,10.13]"));

            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_data", "min 10.12"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_data", "max 10.12"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_data", "[1,-10.13]"));

            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_string", "~ [^ +")); //bad format for regex
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_string", "~ (\\d+)"));
            Assert.AreEqual(Result.Success, Igs.InputAddConstraint("constraint_string", "~ (\\d+)"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_impulsion", "~ (\\d+)"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_int", "~ (\\d+)"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_double", "~ (\\d+)"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_bool", "~ (\\d+)"));
            Assert.AreEqual(Result.Failure, Igs.InputAddConstraint("constraint_data", "~ (\\d+)"));

            Igs.InputRemove("constraint_impulsion");
            Igs.InputRemove("constraint_int");
            Igs.InputRemove("constraint_bool");
            Igs.InputRemove("constraint_double");
            Igs.InputRemove("constraint_string");
            Igs.InputRemove("constraint_data");

            //agent family
            string family = Igs.AgentFamily();
            Assert.IsTrue(string.IsNullOrEmpty(family));
            Igs.AgentSetFamily("family_test");
            family = Igs.AgentFamily();
            Assert.AreEqual("family_test", family);

            //logs
            Assert.IsFalse(Igs.LogConsole());
            Assert.IsFalse(Igs.LogConsoleColor());
            Assert.IsFalse(Igs.LogStream());
            Assert.IsFalse(Igs.LogFile());

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

            //ios with null definition
            Assert.IsTrue(Igs.InputCount() == 0);
            Assert.IsTrue(Igs.OutputCount() == 0);
            Assert.IsTrue(Igs.AttributeCount() == 0);
            Assert.IsTrue(!Igs.InputExists("toto"));
            Assert.IsTrue(!Igs.OutputExists("toto"));
            Assert.IsTrue(!Igs.AttributeExists("toto"));
            string[] listOfStrings = null;
            listOfStrings = Igs.InputList();
            Assert.IsNull(listOfStrings);
            listOfStrings = Igs.OutputList();
            Assert.IsNull(listOfStrings);
            listOfStrings = Igs.AttributeList();
            Assert.IsNull(listOfStrings);
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
            Assert.IsTrue(!Igs.AttributeBool("toto"));
            Assert.IsTrue(Igs.AttributeInt("toto") == 0);
            Assert.IsTrue(Igs.AttributeDouble("toto") < 0.000001);
            Assert.IsTrue(string.IsNullOrEmpty(Igs.AttributeString("toto")));
            Assert.IsTrue(Igs.AttributeData("toto") == null);

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
            Assert.IsTrue(Igs.AttributeCreate("toto", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("toto", IopValueType.Bool) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("toto", IopValueType.Bool) == Result.Failure);
            Assert.IsTrue(Igs.AttributeCreate("toto", IopValueType.Bool) == Result.Failure);
            Igs.OutputMute("toto");
            Assert.IsTrue(Igs.OutputIsMuted("toto"));
            Igs.OutputUnmute("toto");
            Assert.IsTrue(!Igs.OutputIsMuted("toto"));
            Assert.IsTrue(Igs.InputRemove("toto") == Result.Success);
            Assert.IsTrue(Igs.OutputRemove("toto") == Result.Success);
            Assert.IsTrue(Igs.AttributeRemove("toto") == Result.Success);
            Assert.IsTrue(Igs.InputRemove("toto") == Result.Failure);
            Assert.IsTrue(Igs.OutputRemove("toto") == Result.Failure);
            Assert.IsTrue(Igs.AttributeRemove("toto") == Result.Failure);

            //inputs
            Assert.IsTrue(Igs.InputCreate("my_impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my_impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my_bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my_bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my_int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my_int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my_double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my_double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my_string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my_string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(Igs.InputCreate("my_data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(Igs.InputCreate("my_data", IopValueType.Data, _myData) == Result.Failure);
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
            Assert.IsTrue(Igs.InputData("my_data").SequenceEqual(_myData));
            data = Igs.InputData("my_impulsion");
            Assert.IsNull(data);
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
            Assert.IsTrue(Igs.InputSetData("", myOtherData) == Result.Failure);
            Assert.IsTrue(Igs.InputSetData("my_data", myOtherData) == Result.Success);
            Assert.IsTrue(Igs.InputData("my_data").SequenceEqual(myOtherData));
            Igs.ClearInput("my_data");
            Assert.IsNull(Igs.InputData("my_data"));

            //outputs
            Assert.IsTrue(Igs.OutputCreate("my_impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my_impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my_bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my_bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my_int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my_int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my_double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my_double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my_string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my_string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(Igs.OutputCreate("my_data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(Igs.OutputCreate("my_data", IopValueType.Data, _myData) == Result.Failure);
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
            Assert.IsTrue(Igs.OutputData("my_data").SequenceEqual(_myData));
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
            Assert.IsTrue(Igs.OutputSetData("", myOtherData) == Result.Failure);
            Assert.IsTrue(Igs.OutputSetData("my_data", myOtherData) == Result.Success);
            Assert.IsTrue(Igs.OutputData("my_data").SequenceEqual(myOtherData));
            Igs.ClearOutput("my_data");
            Assert.IsTrue(Igs.OutputData("my_data").Length == 0);

            //parameters
            Assert.IsTrue(Igs.AttributeCreate("my_impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(Igs.AttributeCreate("my_impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(Igs.AttributeCreate("my_bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(Igs.AttributeCreate("my_bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(Igs.AttributeCreate("my_int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(Igs.AttributeCreate("my_int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(Igs.AttributeCreate("my_double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(Igs.AttributeCreate("my_double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(Igs.AttributeCreate("my_string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(Igs.AttributeCreate("my_string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(Igs.AttributeCreate("my_data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(Igs.AttributeCreate("my_data", IopValueType.Data, _myData) == Result.Failure);
            listOfStrings = null;
            listOfStrings = Igs.AttributeList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(Igs.AttributeCount() == 6);
            Assert.IsTrue(Igs.AttributeType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.AttributeExists("my_impulsion"));
            Assert.IsTrue(Igs.AttributeType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.AttributeExists("my_bool"));
            Assert.IsTrue(Igs.AttributeType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.AttributeExists("my_int"));
            Assert.IsTrue(Igs.AttributeType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.AttributeExists("my_double"));
            Assert.IsTrue(Igs.AttributeType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.AttributeExists("my_string"));
            Assert.IsTrue(Igs.AttributeType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.AttributeExists("my_data"));
            Assert.IsTrue(Igs.AttributeBool("my_bool"));
            Assert.IsTrue(Igs.AttributeInt("my_int") == 1);
            Assert.IsTrue(Igs.AttributeDouble("my_double") - 1.0 < 0.000001);
            string parameterString = Igs.AttributeString("my_string");
            Assert.AreEqual(parameterString, "my string");
            parameterString = null;
            Assert.IsTrue(Igs.AttributeData("my_data").SequenceEqual(_myData));
            Assert.IsTrue(Igs.AttributeSetBool("", false) == Result.Failure);
            Assert.IsTrue(Igs.AttributeSetBool("my_bool", false) == Result.Success);
            Assert.IsTrue(!Igs.AttributeBool("my_bool"));
            Assert.IsTrue(Igs.AttributeSetInt("", 2) == Result.Failure);
            Assert.IsTrue(Igs.AttributeSetInt("my_int", 2) == Result.Success);
            Assert.IsTrue(Igs.AttributeInt("my_int") == 2);
            Assert.IsTrue(Igs.AttributeSetDouble("", 2) == Result.Failure);
            Assert.IsTrue(Igs.AttributeSetDouble("my_double", 2) == Result.Success);
            Assert.IsTrue(Igs.AttributeDouble("my_double") - 2 < 0.000001);
            Assert.IsTrue(Igs.AttributeSetString("", "new string") == Result.Failure);
            Assert.IsTrue(Igs.AttributeSetString("my_string", "new string") == Result.Success);
            parameterString = Igs.AttributeString("my_string");
            Assert.AreEqual(parameterString, "new string");
            Assert.IsTrue(Igs.AttributeSetData("", myOtherData) == Result.Failure);
            Assert.IsTrue(Igs.AttributeSetData("my_data", myOtherData) == Result.Success);
            Assert.IsTrue(Igs.AttributeData("my_data").SequenceEqual(myOtherData));
            Igs.ClearAttribute("my_data");
            Assert.IsTrue(Igs.AttributeData("my_data").Length == 0);


            //definition - part 2
            //TODO: compare exported def, saved file and reference file
            //io description
            Assert.IsTrue(Igs.InputSetDescription("my_impulsion", "my io description here") == Result.Success);
            Assert.IsTrue(Igs.OutputSetDescription("my_impulsion", "my io description here") == Result.Success);
            Assert.IsTrue(Igs.AttributeSetDescription("my_impulsion", "my io description here") == Result.Success);
            Assert.IsTrue(Igs.InputSetDetailedType("my_impulsion", "protobuf", "some prototbuf \"here\"") == Result.Success);
            Assert.IsTrue(Igs.OutputSetDetailedType("my_impulsion", "protobuf", "some prototbuf \"here\"") == Result.Success);
            Assert.IsTrue(Igs.AttributeSetDetailedType("my_impulsion", "protobuf", "some prototbuf \"here\"") == Result.Success);
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
            listOfStrings = Igs.AttributeList();
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
            Assert.IsTrue(Igs.OutputInt("my_int") == 0);
            Assert.IsTrue(Igs.OutputDouble("my_double") == 0.0);
            outputString = Igs.OutputString("my_string");
            Assert.AreEqual(outputString, null);
            Assert.IsTrue(Igs.OutputData("my_data").Length == 0);
            listOfStrings = Igs.AttributeList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(Igs.AttributeCount() == 6);
            Assert.IsTrue(Igs.AttributeType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.AttributeExists("my_impulsion"));
            Assert.IsTrue(Igs.AttributeType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.AttributeExists("my_bool"));
            Assert.IsTrue(Igs.AttributeType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.AttributeExists("my_int"));
            Assert.IsTrue(Igs.AttributeType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.AttributeExists("my_double"));
            Assert.IsTrue(Igs.AttributeType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.AttributeExists("my_string"));
            Assert.IsTrue(Igs.AttributeType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.AttributeExists("my_data"));
            Assert.IsTrue(!Igs.AttributeBool("my_bool"));
            Assert.IsTrue(Igs.AttributeInt("my_int") == 0);
            Assert.IsTrue(Igs.AttributeDouble("my_double") == 0);
            parameterString = Igs.AttributeString("my_string");
            Assert.AreEqual(parameterString, null);
            parameterString = null;
            Assert.IsTrue(Igs.AttributeData("my_data").Length == 0);
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
            Assert.IsTrue(Igs.OutputInt("my_int") == 0);
            Assert.IsTrue(Igs.OutputDouble("my_double") == 0);
            outputString = Igs.OutputString("my_string");
            Assert.AreEqual(outputString, null);
            outputString = null;
            data = null;
            Assert.IsTrue(Igs.OutputData("my_data").Length == 0);
            listOfStrings = null;
            listOfStrings = Igs.AttributeList();
            Assert.IsNotNull(listOfStrings);
            listOfStrings = null;
            Assert.IsTrue(Igs.AttributeCount() == 6);
            Assert.IsTrue(Igs.AttributeType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(Igs.AttributeExists("my_impulsion"));
            Assert.IsTrue(Igs.AttributeType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(Igs.AttributeExists("my_bool"));
            Assert.IsTrue(Igs.AttributeType("my_int") == IopValueType.Integer);
            Assert.IsTrue(Igs.AttributeExists("my_int"));
            Assert.IsTrue(Igs.AttributeType("my_double") == IopValueType.Double);
            Assert.IsTrue(Igs.AttributeExists("my_double"));
            Assert.IsTrue(Igs.AttributeType("my_string") == IopValueType.String);
            Assert.IsTrue(Igs.AttributeExists("my_string"));
            Assert.IsTrue(Igs.AttributeType("my_data") == IopValueType.Data);
            Assert.IsTrue(Igs.AttributeExists("my_data"));
            Assert.IsTrue(!Igs.AttributeBool("my_bool"));
            Assert.IsTrue(Igs.AttributeInt("my_int") == 0);
            Assert.IsTrue(Igs.AttributeDouble("my_double") == 0);
            parameterString = Igs.AttributeString("my_string");
            Assert.AreEqual(parameterString, null);
            parameterString = null;
            data = null;
            Assert.IsTrue(Igs.AttributeData("my_data").Length == 0);
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

            Assert.IsTrue(Igs.MappingAdd("toto", "other_agent", "tata") > 0);
            Assert.IsTrue(Igs.MappingCount() == 1);
            Igs.ClearMappingsForInput("toto");
            Assert.IsTrue(Igs.MappingCount() == 0);
            Assert.IsTrue(Igs.MappingAdd("toto", "other_agent", "tata") > 0);
            Assert.IsTrue(Igs.MappingCount() == 1);
            Igs.ClearMappingsForInput("tata");
            Assert.IsTrue(Igs.MappingCount() == 1);
            Igs.ClearMappingsForInput("toto");
            Assert.IsTrue(Igs.MappingCount() == 0);

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
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "myBool", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "myInt", IopValueType.Integer) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "myDouble", IopValueType.Double) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "myString", IopValueType.String) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgAdd("myService", "myData", IopValueType.Data) == Result.Success);
            Assert.IsTrue(Igs.ServiceArgsCount("myService") == 5);
            Assert.IsTrue(Igs.ServiceArgExists("myService", "myBool"));
            Assert.IsTrue(Igs.ServiceArgExists("myService", "myInt"));
            Assert.IsTrue(Igs.ServiceArgExists("myService", "myDouble"));
            Assert.IsTrue(Igs.ServiceArgExists("myService", "myString"));
            Assert.IsTrue(Igs.ServiceArgExists("myService", "myData"));
            List<ServiceArgument> list = Igs.ServiceArgumentsList("myService");
            Assert.IsNotNull(list);
            Assert.AreEqual(list[0].Name, "myBool");
            Assert.IsTrue(list[0].Type == IopValueType.Bool);
            Assert.IsFalse((bool)list[0].Value);
            Assert.AreEqual(list[1].Name, "myInt");
            Assert.IsTrue(list[1].Type == IopValueType.Integer);
            Assert.IsTrue((int)list[1].Value == 0);
            Assert.AreEqual(list[2].Name, "myDouble");
            Assert.IsTrue(list[2].Type == IopValueType.Double);
            Assert.IsTrue((double)list[2].Value < 0.000001);
            Assert.AreEqual(list[3].Name, "myString");
            Assert.IsTrue(list[3].Type == IopValueType.String);
            Assert.IsTrue(list[3].Value == null);
            Assert.AreEqual(list[4].Name, "myData");
            Assert.IsTrue(list[4].Type == IopValueType.Data);
            Assert.IsTrue(list[4].Value == null);
            Igs.DefinitionSave();

            Assert.IsTrue(Igs.ServiceDescription("myService") == null);
            Assert.IsTrue(Igs.ServiceSetDescription("unknow", "myService description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceSetDescription("myService", "myService description") == Result.Success);
            string servicedescription = Igs.ServiceDescription("myService");
            Assert.IsTrue(servicedescription == "myService description");

            Assert.IsTrue(Igs.ServiceInit("myService2", testerServiceCallback, null) == Result.Success);
            Assert.IsTrue(Igs.ServiceSetDescription("myService2", "myService2 description") == Result.Success);

            Assert.IsTrue(Igs.ServiceArgDescription("myService", "myBool") == null);
            Assert.IsTrue(Igs.ServiceArgSetDescription("unknow", "myBool", "myBool description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceArgSetDescription("myService", "unknow", "myBool description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceArgSetDescription("myService", "myBool", "myBool description") == Result.Success);
            string serviceArgBoolDescription = Igs.ServiceArgDescription("myService", "myBool");
            Assert.IsTrue(serviceArgBoolDescription == "myBool description");
            Assert.IsTrue(Igs.ServiceArgDescription("unknow", "myBool") == null);
            Assert.IsTrue(Igs.ServiceArgDescription("myService", "unknow") == null);

            Assert.IsTrue(Igs.ServiceArgDescription("myService", "myInt") == null);
            Assert.IsTrue(Igs.ServiceArgSetDescription("unknow", "myInt", "myInt description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceArgSetDescription("myService", "unknow", "myInt description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceArgSetDescription("myService", "myInt", "myInt description") == Result.Success);
            string serviceArgIntDescription = Igs.ServiceArgDescription("myService", "myInt");
            Assert.IsTrue(serviceArgIntDescription == "myInt description");
            Assert.IsTrue(Igs.ServiceArgDescription("unknow", "myInt") == null);
            Assert.IsTrue(Igs.ServiceArgDescription("myService", "unknow") == null);

            string currentDefinition = Igs.DefinitionJson();
            Igs.ClearDefinition();
            Igs.DefinitionLoadStr(currentDefinition);
            string newServiceDescription = Igs.ServiceDescription("myService");
            Assert.IsTrue(newServiceDescription == "myService description");
            string newService2Description = Igs.ServiceDescription("myService2");
            Assert.IsTrue(newService2Description == "myService2 description");
            string newServiceArgBoolDescription = Igs.ServiceArgDescription("myService", "myBool");
            Assert.IsTrue(newServiceArgBoolDescription == "myBool description");
            string newServiceArgIntDescription = Igs.ServiceArgDescription("myService", "myInt");
            Assert.IsTrue(newServiceArgIntDescription == "myInt description");

            //service with reply
            Assert.IsTrue(Igs.ServiceInit("myServiceWithReplies", testerServiceCallback, null) == Result.Success);
            Assert.IsTrue(!Igs.ServiceHasReplies("myServiceWithReplies"));
            Assert.IsTrue(!Igs.ServiceHasReply("myServiceWithReplies", "toto"));
            string[] names = Igs.ServiceReplyNames("myServiceWithReplies");
            Assert.IsTrue(names == null);
            Assert.IsTrue(Igs.ServiceReplyArgumentsList("myServiceWithReplies", "toto") == null);
            Assert.IsTrue(Igs.ServiceReplyArgsCount("myServiceWithReplies", "toto") == 0);
            Assert.IsTrue(Igs.ServiceReplyAdd("myServiceWithReplies", "myReply") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyAdd("myServiceWithReplies", "myReply2") == Result.Success);
            Assert.IsTrue(Igs.ServiceHasReplies("myServiceWithReplies"));
            Assert.IsTrue(Igs.ServiceHasReply("myServiceWithReplies", "myReply"));
            Assert.IsTrue(Igs.ServiceHasReply("myServiceWithReplies", "myReply2"));
            Assert.IsTrue(!Igs.ServiceHasReply("myServiceWithReplies", "myReply3"));
            Assert.IsTrue(Igs.ServiceHasReplies("myServiceWithReplies"));
            names = Igs.ServiceReplyNames("myServiceWithReplies");
            Assert.IsTrue(names != null);
            Assert.IsTrue(names.Length == 2);
            Assert.IsTrue(names.Contains("myReply"));
            Assert.IsTrue(names.Contains("myReply2"));

            Assert.IsTrue(Igs.ServiceReplyDescription("myServiceWithReplies", "myReply") == null);
            Assert.IsTrue(Igs.ServiceReplySetDescription("unknow", "myReply", "myServiceReply description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplySetDescription("myServiceWithReplies", "unknow", "myServiceReply description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplySetDescription("myServiceWithReplies", "myReply", "myServiceReply description") == Result.Success);
            string replyDescription = Igs.ServiceReplyDescription("myServiceWithReplies", "myReply");
            Assert.IsTrue(replyDescription == "myServiceReply description");

            Assert.IsTrue(Igs.ServiceReplyDescription("myServiceWithReplies", "myReply2") == null);
            Assert.IsTrue(Igs.ServiceReplySetDescription("myServiceWithReplies", "myReply2", "myServiceReply2 description") == Result.Success);

            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myBool", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myBool") == null);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("unknow", "myReply", "myBool", "myArgBool description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("myServiceWithReplies", "unknow", "myBool", "myArgBool description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "unknow", "myArgBool description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "myBool", "myArgBool description") == Result.Success);
            string replyArgBoolDescription = Igs.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myBool");
            Assert.IsTrue(replyArgBoolDescription == "myArgBool description");
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myInt", IopValueType.Integer) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myInt") == null);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("unknow", "myReply", "myInt", "myArgInt description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("myServiceWithReplies", "unknow", "myInt", "myArgInt description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "unknow", "myArgInt description") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "myInt", "myArgInt description") == Result.Success);
            string replyArgIntDescription = Igs.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myInt");
            Assert.IsTrue(replyArgIntDescription == "myArgInt description");

            currentDefinition = Igs.DefinitionJson();
            Igs.ClearDefinition();
            Igs.DefinitionLoadStr(currentDefinition);
            string newReplyDescription = Igs.ServiceReplyDescription("myServiceWithReplies", "myReply");
            Assert.IsTrue(newReplyDescription == "myServiceReply description");
            string newReply2Description = Igs.ServiceReplyDescription("myServiceWithReplies", "myReply2");
            Assert.IsTrue(newReply2Description == "myServiceReply2 description");
            string newReplyArgBoolDescription = Igs.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myBool");
            Assert.IsTrue(newReplyArgBoolDescription == "myArgBool description");
            string newReplyArgIntDescription = Igs.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myInt");
            Assert.IsTrue(newReplyArgIntDescription == "myArgInt description");

            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "myBool", "") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "myInt", "") == Result.Success);
            Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myBool");
            Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myInt");

            Assert.IsTrue(Igs.ServiceReplyArgumentsList("myServiceWithReplies", "myReply") == null);
            Assert.IsTrue(Igs.ServiceReplyArgsCount("myServiceWithReplies", "myReply") == 0);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myBool", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myInt", IopValueType.Integer) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myDouble", IopValueType.Double) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myString", IopValueType.String) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myData", IopValueType.Data) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myBool2", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myInt2", IopValueType.Integer) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myDouble2", IopValueType.Double) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myString2", IopValueType.String) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myData2", IopValueType.Data) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgumentsList("myServiceWithReplies", "myReply") != null);
            Assert.IsTrue(Igs.ServiceReplyArgsCount("myServiceWithReplies", "myReply") == 5);
            Assert.IsTrue(Igs.ServiceReplyArgumentsList("myServiceWithReplies", "myReply2") != null);
            Assert.IsTrue(Igs.ServiceReplyArgsCount("myServiceWithReplies", "myReply2") == 5);
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myBool"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myInt"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myDouble"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myString"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myData"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myBool2"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myInt2"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myDouble2"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myString2"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myData2"));
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myBool") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myInt") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myDouble") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myString") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myData") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myBool") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myInt") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myDouble") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myString") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myData") == Result.Failure);
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myBool"));
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myInt"));
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myDouble"));
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myString"));
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myData"));
            Assert.IsTrue(Igs.ServiceReplyArgsCount("myServiceWithReplies", "myReply") == 0);
            Assert.IsTrue(Igs.ServiceReplyArgumentsList("myServiceWithReplies", "myReply") == null);
            Assert.IsTrue(Igs.ServiceReplyRemove("myServiceWithReplies", "myReply") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyRemove("myServiceWithReplies", "myReply") == Result.Failure);
            Assert.IsTrue(!Igs.ServiceHasReply("myServiceWithReplies", "myReply"));
            names = Igs.ServiceReplyNames("myServiceWithReplies");
            Assert.IsTrue(names.Length == 1 && ("myReply2" == names[0]));
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myBool2") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myInt2") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myDouble2") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myString2") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myData2") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myBool2") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myInt2") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myDouble2") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myString2") == Result.Failure);
            Assert.IsTrue(Igs.ServiceReplyArgRemove("myServiceWithReplies", "myReply2", "myData2") == Result.Failure);
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myBool2"));
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myInt2"));
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myDouble2"));
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myString2"));
            Assert.IsTrue(!Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myData2"));
            Assert.IsTrue(Igs.ServiceReplyArgsCount("myServiceWithReplies", "myReply2") == 0);
            Assert.IsTrue(Igs.ServiceReplyArgumentsList("myServiceWithReplies", "myReply2") == null);
            Assert.IsTrue(Igs.ServiceHasReply("myServiceWithReplies", "myReply2"));
            Assert.IsTrue(Igs.ServiceReplyRemove("myServiceWithReplies", "myReply2") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyRemove("myServiceWithReplies", "myReply2") == Result.Failure);
            Assert.IsTrue(!Igs.ServiceHasReply("myServiceWithReplies", "myReply2"));
            names = Igs.ServiceReplyNames("myServiceWithReplies");
            Assert.IsTrue(names == null);
            Assert.IsTrue(!Igs.ServiceHasReplies("myServiceWithReplies"));
            Assert.IsTrue(Igs.ServiceReplyAdd("myServiceWithReplies", "myReply") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyAdd("myServiceWithReplies", "myReply2") == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myBool", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myInt", IopValueType.Integer) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myDouble", IopValueType.Double) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myString", IopValueType.String) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myData", IopValueType.Data) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myBool2", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myInt2", IopValueType.Integer) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myDouble2", IopValueType.Double) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myString2", IopValueType.String) == Result.Success);
            Assert.IsTrue(Igs.ServiceReplyArgAdd("myServiceWithReplies", "myReply2", "myData2", IopValueType.Data) == Result.Success);
            Igs.DefinitionSetClass("my_class");
            Igs.DefinitionSetPackage("my::package");
            Igs.DefinitionSave();
            Assert.IsTrue(Igs.ServiceRemove("myServiceWithReplies") == Result.Success);
            Igs.ClearDefinition();
            Igs.DefinitionLoadFile("/tmp/simple Demo Agent.json");
            Assert.AreEqual("my_class", Igs.DefinitionClass());
            Assert.AreEqual("my::package", Igs.DefinitionPackage());
            Assert.IsTrue(Igs.ServiceHasReply("myServiceWithReplies", "myReply"));
            Assert.IsTrue(Igs.ServiceHasReply("myServiceWithReplies", "myReply2"));
            Assert.IsTrue(Igs.ServiceReplyArgumentsList("myServiceWithReplies", "myReply") != null);
            Assert.IsTrue(Igs.ServiceReplyArgsCount("myServiceWithReplies", "myReply") == 5);
            Assert.IsTrue(Igs.ServiceReplyArgumentsList("myServiceWithReplies", "myReply2") != null);
            Assert.IsTrue(Igs.ServiceReplyArgsCount("myServiceWithReplies", "myReply2") == 5);
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myBool"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myInt"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myDouble"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myString"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply", "myData"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myBool2"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myInt2"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myDouble2"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myString2"));
            Assert.IsTrue(Igs.ServiceReplyArgExists("myServiceWithReplies", "myReply2", "myData2"));

            //prepare agent for dynamic tests by adding proper complete definitions
            Igs.AgentSetName(_agentName);
            Igs.LogSetConsole(_verbose);

            Igs.AgentSetName(_agentName);
            Igs.DefinitionSetDescription("One example for each type of IO and call");
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
            Igs.AttributeCreate("my_impulsion", IopValueType.Impulsion);
            Igs.AttributeCreate("my_bool", IopValueType.Bool, _myBool);
            Igs.AttributeCreate("my_int", IopValueType.Integer, _myInt);
            Igs.AttributeCreate("my_double", IopValueType.Double, _myDouble);
            Igs.AttributeCreate("my_string", IopValueType.String, _myString);
            Igs.AttributeCreate("my_data", IopValueType.Data, _myData);
            Igs.ServiceInit("myService", testerServiceCallback, null);
            Igs.ServiceArgAdd("myService", "myBool", IopValueType.Bool);
            Igs.ServiceArgAdd("myService", "myInt", IopValueType.Integer);
            Igs.ServiceArgAdd("myService", "myDouble", IopValueType.Double);
            Igs.ServiceArgAdd("myService", "myString", IopValueType.String);
            Igs.ServiceArgAdd("myService", "myData", IopValueType.Data);

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

            //io description
            Assert.IsTrue(Igs.InputDescription("my_impulsion") == null);
            Assert.IsTrue(Igs.InputSetDescription("my_impulsion", "my iop description here") == Result.Success);
            Assert.IsTrue(Igs.InputDescription("my_impulsion") == "my iop description here");
            Assert.IsTrue(Igs.OutputDescription("my_impulsion") == null);
            Assert.IsTrue(Igs.OutputSetDescription("my_impulsion", "my iop description here") == Result.Success);
            Assert.IsTrue(Igs.OutputDescription("my_impulsion") == "my iop description here");
            Assert.IsTrue(Igs.AttributeDescription("my_impulsion") == null);
            Assert.IsTrue(Igs.AttributeSetDescription("my_impulsion", "my iop description here") == Result.Success);
            Assert.IsTrue(Igs.AttributeDescription("my_impulsion") == "my iop description here");

            //IO writing and types conversions
            Igs.InputSetImpulsion("my_impulsion");
            Igs.InputSetImpulsion("my_bool");
            Assert.IsFalse(Igs.InputBool("my_bool"));
            Igs.InputSetImpulsion("my_int");
            Assert.IsTrue(Igs.InputInt("my_int") == 0);
            Igs.InputSetImpulsion("my_double");
            Assert.IsTrue(Igs.InputDouble("my_double") < 0.000001);
            Igs.InputSetImpulsion("my_string");
            string readResult = Igs.InputString("my_string");
            Assert.IsNull(readResult);
            Igs.InputSetImpulsion("my_data");
            Assert.IsNull(Igs.InputData("my_data"));

            Igs.InputSetBool("my_impulsion", true);
            Igs.InputSetBool("my_bool", true);
            Assert.IsTrue(Igs.InputBool("my_bool"));
            Igs.InputSetBool("my_int", true);
            Assert.IsTrue(Igs.InputInt("my_int") == 1);
            Igs.InputSetBool("my_double", true);
            Assert.IsTrue(Igs.InputDouble("my_double") - 1.0 < 0.000001);
            Igs.InputSetBool("my_string", true);
            readResult = Igs.InputString("my_string");
            Assert.IsTrue(readResult == "1");
            Igs.InputSetBool("my_data", true);
            Assert.IsTrue(Igs.InputData("my_data").Length == sizeof(bool));


            Igs.InputSetInt("my_impulsion", 3);
            Igs.InputSetInt("my_bool", 3);
            Assert.IsTrue(Igs.InputBool("my_bool"));
            Igs.InputSetInt("my_int", 3);
            Assert.IsTrue(Igs.InputInt("my_int") == 3);
            Igs.InputSetInt("my_double", 3);
            Assert.IsTrue(Igs.InputDouble("my_double") - 3.0 < 0.000001);
            Igs.InputSetInt("my_string", 3);
            readResult = Igs.InputString("my_string");
            Assert.IsTrue(readResult == "3");
            Igs.InputSetInt("my_data", 3);
            Assert.IsTrue(Igs.InputData("my_data").Length == sizeof(int));

            Igs.InputSetDouble("my_impulsion", 3.3);
            Igs.InputSetDouble("my_bool", 3.3);
            Assert.IsTrue(Igs.InputBool("my_bool"));
            Igs.InputSetDouble("my_int", 3.3);
            Assert.IsTrue(Igs.InputInt("my_int") == 3);
            Igs.InputSetDouble("my_double", 3.3);
            Assert.IsTrue(Igs.InputDouble("my_double") - 3.3 < 0.000001);
            Igs.InputSetDouble("my_string", 3.3);
            readResult = Igs.InputString("my_string");
            Assert.IsTrue(readResult == "3.300000");
            Igs.InputSetDouble("my_data", 3.3);
            Assert.IsTrue(Igs.InputData("my_data").Length == sizeof(double));

            Igs.InputSetString("my_impulsion", "true");
            Igs.InputSetString("my_bool", "true");
            Assert.IsTrue(Igs.InputBool("my_bool"));
            Igs.InputSetString("my_int", "3.3");
            Assert.IsTrue(Igs.InputInt("my_int") == 3);
            Igs.InputSetString("my_double", "3.3");
            Assert.IsTrue(Igs.InputDouble("my_double") - 3.3 < 0.000001);
            Igs.InputSetString("my_string", "3.3");
            readResult = Igs.InputString("my_string");
            Assert.IsTrue(readResult == "3.3");

            Assert.IsTrue(Igs.InputSetString("my_data", "this is a test string") == Result.Success); //converted as raw data
            data = Igs.InputData("my_data");
            Assert.IsTrue(data.Length == ("this is a test string".Length + 1));

            Assert.IsTrue(Igs.InputSetString("my_data", "0123456789abcdef") == Result.Success);
            Assert.IsTrue(Igs.InputData("my_data").Length == 8);

            Assert.IsTrue(Igs.InputSetString("my_data", "12") == Result.Success);
            Assert.IsTrue(Igs.InputData("my_data").Length == 1);

            Assert.IsTrue(Igs.InputSetString("my_data", null) == Result.Success);
            Assert.IsNull(Igs.InputData("my_data"));

            byte[] myRawData = System.Text.Encoding.UTF8.GetBytes("my raw data" + "\0");
            Assert.IsTrue(Igs.InputSetData("my_impulsion", myRawData) == Result.Success);
            Assert.IsTrue(Igs.InputSetData("my_bool", myRawData) == Result.Failure);
            Assert.IsTrue(Igs.InputSetData("my_int", myRawData) == Result.Failure);
            Assert.IsTrue(Igs.InputSetData("my_double", myRawData) == Result.Failure);
            Assert.IsTrue(Igs.InputSetData("my_string", myRawData) == Result.Success);
            Assert.AreEqual(Igs.InputString("my_string"), "my raw data");
            Assert.IsTrue(Igs.InputSetData("my_data", myRawData) == Result.Success);
            Assert.IsTrue(Igs.InputData("my_data").SequenceEqual(myRawData));
            bool myBool = true;
            Igs.InputSetBool("my_bool", false);
            Igs.InputSetData("my_bool", BitConverter.GetBytes(myBool));
            Assert.IsTrue(Igs.InputBool("my_bool"));
            int myInt = 123456;
            Igs.InputSetInt("my_int", 0);
            Igs.InputSetData("my_int", BitConverter.GetBytes(myInt));
            Assert.AreEqual(Igs.InputInt("my_int"), myInt);
            double myDouble = 123.456;
            Igs.InputSetDouble("my_double", 0.0);
            Igs.InputSetData("my_double", BitConverter.GetBytes(myDouble));
            Assert.AreEqual(Igs.InputDouble("my_double"), myDouble);

            Igs.InputSetBool("my_bool", true);
            Assert.IsTrue(Igs.InputBool("my_bool"));
            Assert.IsTrue(Igs.InputInt("my_bool") == 1);
            Assert.IsTrue(Igs.InputDouble("my_bool") - 1 < 0.000001);
            Assert.IsTrue(Igs.InputString("my_bool") == "true");
            Igs.InputSetBool("my_bool", false);
            Assert.IsTrue(!Igs.InputBool("my_bool"));
            Assert.IsTrue(Igs.InputInt("my_bool") == 0);
            Assert.IsTrue(Igs.InputDouble("my_bool") < 0.000001);
            Assert.IsTrue(Igs.InputString("my_bool") == "false");
            Assert.IsTrue(Igs.InputData("my_bool").Length == sizeof(bool));

            Igs.InputSetInt("my_int", 3);
            Assert.IsTrue(Igs.InputBool("my_int"));
            Assert.IsTrue(Igs.InputInt("my_int") == 3);
            Assert.IsTrue(Igs.InputDouble("my_int") - 3 < 0.000001);
            Assert.IsTrue(Igs.InputString("my_int") == "3");
            Igs.InputSetInt("my_int", 0);
            Assert.IsTrue(!Igs.InputBool("my_int"));
            Assert.IsTrue(Igs.InputInt("my_int") == 0);
            Assert.IsTrue(Igs.InputDouble("my_int") < 0.000001);
            Assert.IsTrue(Igs.InputString("my_int") == "0");
            Assert.IsTrue(Igs.InputData("my_int").Length == sizeof(int));

            Igs.InputSetDouble("my_double", 3.3);
            Assert.IsTrue(Igs.InputBool("my_double"));
            Assert.IsTrue(Igs.InputInt("my_double") == 3);
            Assert.IsTrue(Igs.InputDouble("my_double") - 3.3 < 0.000001);
            Assert.IsTrue(Igs.InputString("my_double") == "3.300000");
            Igs.InputSetDouble("my_double", 0.0);
            Assert.IsTrue(!Igs.InputBool("my_double"));
            Assert.IsTrue(Igs.InputInt("my_double") == 0);
            Assert.IsTrue(Igs.InputDouble("my_double") < 0.000001);
            Assert.IsTrue(Igs.InputString("my_double") == "0.000000");
            Assert.IsTrue(Igs.InputData("my_double").Length == sizeof(double));

            Igs.InputSetString("my_string", "true");
            Assert.IsTrue(Igs.InputBool("my_string"));
            Igs.InputSetString("my_string", "false");
            Assert.IsTrue(!Igs.InputBool("my_string"));
            Igs.InputSetString("my_string", "10.1");
            Assert.IsTrue(Igs.InputInt("my_string") == 10);
            Assert.IsTrue(Igs.InputDouble("my_string") - 10.1 < 0.000001);
            Assert.IsTrue(Igs.InputString("my_string") == "10.1");
            Assert.IsTrue(Igs.InputData("my_string").Length == 5);
            Igs.InputSetString("my_string", "");
            Assert.AreEqual(Igs.InputString("my_string"), "");
            Igs.InputSetString("my_string", null);
            Assert.IsNull(Igs.InputString("my_string"));

            byte[] myData = System.Text.Encoding.UTF8.GetBytes("my data" + "\0");
            Igs.InputSetData("my_data", myData);
            Assert.IsFalse(Igs.InputBool("my_data"));
            Assert.IsTrue(Igs.InputInt("my_data") == 0);
            Assert.IsTrue(Igs.InputDouble("my_data") < 0.000001);
            Assert.IsTrue(Igs.InputString("my_data") == null);
            Assert.IsTrue(Igs.InputData("my_data").SequenceEqual(myData));


            Igs.StartWithDevice(_networkDevice, _port);
            //Channel methods call test
            Assert.IsTrue(Igs.ChannelShout("channel", "msg") == Result.Success);
            Assert.IsTrue(Igs.ChannelShout("channel", new byte[] { 0, 1, 0, 1 }) == Result.Success);
            Assert.IsTrue(Igs.ChannelWhisper("agentName", "msg") == Result.Success);
            Assert.IsTrue(Igs.ChannelWhisper("agentName", new byte[] { 0, 1, 0, 1 }) == Result.Success);
            Assert.IsTrue(Igs.PeerAddHeader("headerKey", "headerValue") == Result.Success);
            Igs.Stop();
            Assert.IsTrue(Igs.PeerRemoveHeader("headerKey") == Result.Success);

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
            FirstAgent.AttributeCreate("Firstimpulsion", IopValueType.Impulsion);
            FirstAgent.AttributeCreate("Firstbool", IopValueType.Bool, _myBool);
            FirstAgent.AttributeCreate("Firstint", IopValueType.Integer, _myInt);
            FirstAgent.AttributeCreate("Firstdouble", IopValueType.Double, _myDouble);
            FirstAgent.AttributeCreate("Firststring", IopValueType.String, _myString);
            FirstAgent.AttributeCreate("Firstdata", IopValueType.Data, _myData);

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
            SecondAgent.AttributeCreate("second_impulsion", IopValueType.Impulsion);
            SecondAgent.AttributeCreate("second_bool", IopValueType.Bool, _myBool);
            SecondAgent.AttributeCreate("second_int", IopValueType.Integer, _myInt);
            SecondAgent.AttributeCreate("second_double", IopValueType.Double, _myDouble);
            SecondAgent.AttributeCreate("second_string", IopValueType.String, _myString);
            SecondAgent.AttributeCreate("second_data", IopValueType.Data, _myData);
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

            //test service in the same process
            SecondAgent.Activate();
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
            Assert.IsTrue(Igs.ElectionLeave("my election") == Result.Success);
            Assert.IsTrue(Igs.ElectionJoin("my election") == Result.Success);
            Assert.IsTrue(Igs.ElectionJoin("my election") == Result.Failure);
            Assert.IsTrue(Igs.ElectionJoin("INGESCAPE_PRIVATE") == Result.Failure);
            Assert.IsTrue(Igs.ElectionLeave("my election") == Result.Success);
            Assert.IsTrue(Igs.ElectionLeave("my election") == Result.Success);
            Assert.IsTrue(Igs.ElectionLeave("my other election") == Result.Success);
            Assert.IsTrue(Igs.ElectionJoin("my other election") == Result.Success);
            Assert.IsTrue(Igs.ElectionJoin("my other election") == Result.Failure);
            Assert.IsTrue(Igs.ElectionLeave("my other election") == Result.Success);
            Assert.IsTrue(Igs.ElectionLeave("my other election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionLeave("my election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionJoin("my election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionJoin("my election") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionJoin("INGESCAPE_PRIVATE") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionLeave("my election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionLeave("my election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionLeave("my other election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionJoin("my other election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionJoin("my other election") == Result.Failure);
            Assert.IsTrue(FirstAgent.ElectionLeave("my other election") == Result.Success);
            Assert.IsTrue(FirstAgent.ElectionLeave("my other election") == Result.Success);

            FirstAgent.ClearDefinition();

            //inputs
            Assert.IsTrue(FirstAgent.InputCreate("my_impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my_impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my_bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my_bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my_int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my_int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my_double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my_double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my_string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my_string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(FirstAgent.InputCreate("my_data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(FirstAgent.InputCreate("my_data", IopValueType.Data, _myData) == Result.Failure);
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

            Assert.IsNull(FirstAgent.InputDescription("my_impulsion"));
            Assert.IsTrue(FirstAgent.InputSetDescription("my_impulsion", "myFirstAgent input impulsion description") == Result.Success);
            Assert.AreEqual(FirstAgent.InputDescription("my_impulsion"), "myFirstAgent input impulsion description");

            //outputs
            Assert.IsTrue(FirstAgent.OutputCreate("my_impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my_impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my_bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my_bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my_int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my_int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my_double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my_double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my_string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my_string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(FirstAgent.OutputCreate("my_data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(FirstAgent.OutputCreate("my_data", IopValueType.Data, _myData) == Result.Failure);
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

            Assert.IsNull(FirstAgent.OutputDescription("my_impulsion"));
            Assert.IsTrue(FirstAgent.OutputSetDescription("my_impulsion", "myFirstAgent output impulsion description") == Result.Success);
            Assert.AreEqual(FirstAgent.OutputDescription("my_impulsion"), "myFirstAgent output impulsion description");

            //parameters
            Assert.IsTrue(FirstAgent.AttributeCreate("my_impulsion", IopValueType.Impulsion) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_impulsion", IopValueType.Impulsion) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_bool", IopValueType.Bool, _myBool) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_bool", IopValueType.Bool, _myBool) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_int", IopValueType.Integer, _myInt) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_int", IopValueType.Integer, _myInt) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_double", IopValueType.Double, _myDouble) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_double", IopValueType.Double, _myDouble) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_string", IopValueType.String, _myString) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_string", IopValueType.String, _myString) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_data", IopValueType.Data, _myData) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeCreate("my_data", IopValueType.Data, _myData) == Result.Failure);
            listOfStrings = null;
            listOfStrings = FirstAgent.AttributeList();
            Assert.IsTrue(listOfStrings != null && listOfStrings.Length == 6);
            listOfStrings = null;
            Assert.IsTrue(FirstAgent.AttributeCount() == 6);
            Assert.IsTrue(FirstAgent.AttributeType("my_impulsion") == IopValueType.Impulsion);
            Assert.IsTrue(FirstAgent.AttributeExists("my_impulsion"));
            Assert.IsTrue(FirstAgent.AttributeType("my_bool") == IopValueType.Bool);
            Assert.IsTrue(FirstAgent.AttributeExists("my_bool"));
            Assert.IsTrue(FirstAgent.AttributeType("my_int") == IopValueType.Integer);
            Assert.IsTrue(FirstAgent.AttributeExists("my_int"));
            Assert.IsTrue(FirstAgent.AttributeType("my_double") == IopValueType.Double);
            Assert.IsTrue(FirstAgent.AttributeExists("my_double"));
            Assert.IsTrue(FirstAgent.AttributeType("my_string") == IopValueType.String);
            Assert.IsTrue(FirstAgent.AttributeExists("my_string"));
            Assert.IsTrue(FirstAgent.AttributeType("my_data") == IopValueType.Data);
            Assert.IsTrue(FirstAgent.AttributeExists("my_data"));
            Assert.IsTrue(FirstAgent.AttributeBool("my_bool"));
            Assert.IsTrue(FirstAgent.AttributeInt("my_int") == 1);
            Assert.IsTrue(FirstAgent.AttributeDouble("my_double") - 1.0 < 0.000001);
            parameterString = FirstAgent.AttributeString("my_string");
            Assert.AreEqual(parameterString, "my string");
            parameterString = null;
            Assert.IsTrue(FirstAgent.AttributeData("my_data").Length == 1);
            Assert.IsTrue(FirstAgent.AttributeSetBool("", false) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeSetBool("my_bool", false) == Result.Success);
            Assert.IsTrue(!FirstAgent.AttributeBool("my_bool"));
            Assert.IsTrue(FirstAgent.AttributeSetInt("", 2) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeSetInt("my_int", 2) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeInt("my_int") == 2);
            Assert.IsTrue(FirstAgent.AttributeSetDouble("", 2) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeSetDouble("my_double", 2) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeDouble("my_double") - 2 < 0.000001);
            Assert.IsTrue(FirstAgent.AttributeSetString("", "new string") == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeSetString("my_string", "new string") == Result.Success);
            parameterString = FirstAgent.AttributeString("my_string");
            Assert.AreEqual(parameterString, "new string");
            Assert.IsTrue(FirstAgent.AttributeSetData("", _myData) == Result.Failure);
            Assert.IsTrue(FirstAgent.AttributeSetData("my_data", _myData) == Result.Success);
            Assert.IsTrue(FirstAgent.AttributeData("my_data").Length == 1);
            FirstAgent.ClearAttribute("my_data");
            Assert.IsTrue(FirstAgent.AttributeData("my_data").Length == 0);

            Assert.IsNull(FirstAgent.AttributeDescription("my_impulsion"));
            Assert.IsTrue(FirstAgent.AttributeSetDescription("my_impulsion", "myFirstAgent attribute impulsion description") == Result.Success);
            Assert.AreEqual(FirstAgent.AttributeDescription("my_impulsion"), "myFirstAgent attribute impulsion description");


            Assert.IsTrue(FirstAgent.ServiceInit("myService", testerAgentServiceCallback, null) == Result.Success);
            Assert.IsTrue(FirstAgent.ServiceArgAdd("myService", "myBool", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(FirstAgent.ServiceArgAdd("myService", "myInt", IopValueType.Integer) == Result.Success);

            Assert.IsTrue(FirstAgent.ServiceDescription("myService") == null);
            Assert.IsTrue(FirstAgent.ServiceSetDescription("unknow", "myFirstAgent service description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceSetDescription("myService", "myFirstAgent service description") == Result.Success);
            string agentServicedescription = FirstAgent.ServiceDescription("myService");
            Assert.AreEqual(agentServicedescription,"myFirstAgent service description");

            Assert.IsTrue(FirstAgent.ServiceInit("myService2", testerAgentServiceCallback, null) == Result.Success);
            Assert.IsTrue(FirstAgent.ServiceSetDescription("myService2", "myFirstAgent service2 description") == Result.Success);

            Assert.IsTrue(FirstAgent.ServiceArgDescription("myService", "myBool") == null);
            Assert.IsTrue(FirstAgent.ServiceArgSetDescription("unknow", "myBool", "myFirstAgent Bool description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceArgSetDescription("myService", "unknow", "myFirstAgent Bool description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceArgSetDescription("myService", "myBool", "myFirstAgent Bool description") == Result.Success);
            string agentServiceArgBoolDescription = FirstAgent.ServiceArgDescription("myService", "myBool");
            Assert.IsTrue(agentServiceArgBoolDescription == "myFirstAgent Bool description");
            Assert.IsTrue(FirstAgent.ServiceArgDescription("unknow", "myBool") == null);
            Assert.IsTrue(FirstAgent.ServiceArgDescription("myService", "unknow") == null);

            Assert.IsTrue(FirstAgent.ServiceArgDescription("myService", "myInt") == null);
            Assert.IsTrue(FirstAgent.ServiceArgSetDescription("unknow", "myInt", "myFirstAgent Int description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceArgSetDescription("myService", "unknow", "myFirstAgent Int description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceArgSetDescription("myService", "myInt", "myFirstAgent Int description") == Result.Success);
            string agentServiceArgIntDescription = FirstAgent.ServiceArgDescription("myService", "myInt");
            Assert.IsTrue(agentServiceArgIntDescription == "myFirstAgent Int description");
            Assert.IsTrue(FirstAgent.ServiceArgDescription("unknow", "myInt") == null);
            Assert.IsTrue(FirstAgent.ServiceArgDescription("myService", "unknow") == null);

            string currentAgentDefinition = FirstAgent.DefinitionJson();
            Igs.ClearDefinition();
            FirstAgent.DefinitionLoadStr(currentAgentDefinition);
            string newAgentServiceDescription = FirstAgent.ServiceDescription("myService");
            Assert.AreEqual(newAgentServiceDescription, "myFirstAgent service description");
            string newAgentService2Description = FirstAgent.ServiceDescription("myService2");
            Assert.AreEqual(newAgentService2Description, "myFirstAgent service2 description");
            string newAgentServiceArgBoolDescription = FirstAgent.ServiceArgDescription("myService", "myBool");
            Assert.AreEqual(newAgentServiceArgBoolDescription, "myFirstAgent Bool description");
            string newAgentServiceArgIntDescription = FirstAgent.ServiceArgDescription("myService", "myInt");
            Assert.AreEqual(newAgentServiceArgIntDescription, "myFirstAgent Int description");

            Assert.IsTrue(FirstAgent.ServiceInit("myServiceWithReplies", testerAgentServiceCallback, null) == Result.Success);
            Assert.IsTrue(!FirstAgent.ServiceHasReplies("myServiceWithReplies"));
            Assert.IsTrue(!FirstAgent.ServiceHasReply("myServiceWithReplies", "toto"));
            string[] agentNames = FirstAgent.ServiceReplyNames("myServiceWithReplies");
            Assert.IsTrue(agentNames == null);
            Assert.IsTrue(FirstAgent.ServiceReplyArgumentsList("myServiceWithReplies", "toto") == null);
            Assert.IsTrue(FirstAgent.ServiceReplyArgsCount("myServiceWithReplies", "toto") == 0);
            Assert.IsTrue(FirstAgent.ServiceReplyAdd("myServiceWithReplies", "myReply") == Result.Success);
            Assert.IsTrue(FirstAgent.ServiceReplyAdd("myServiceWithReplies", "myReply2") == Result.Success);
            Assert.IsTrue(FirstAgent.ServiceHasReplies("myServiceWithReplies"));
            Assert.IsTrue(FirstAgent.ServiceHasReply("myServiceWithReplies", "myReply"));
            Assert.IsTrue(FirstAgent.ServiceHasReply("myServiceWithReplies", "myReply2"));
            Assert.IsTrue(!FirstAgent.ServiceHasReply("myServiceWithReplies", "myReply3"));
            Assert.IsTrue(FirstAgent.ServiceHasReplies("myServiceWithReplies"));
            agentNames = FirstAgent.ServiceReplyNames("myServiceWithReplies");
            Assert.IsTrue(agentNames != null);
            Assert.IsTrue(agentNames.Length == 2);
            Assert.IsTrue(agentNames.Contains("myReply"));
            Assert.IsTrue(agentNames.Contains("myReply2"));

            Assert.IsTrue(FirstAgent.ServiceReplyDescription("myServiceWithReplies", "myReply") == null);
            Assert.IsTrue(FirstAgent.ServiceReplySetDescription("unknow", "myReply", "myFirstAgent ServiceReply description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceReplySetDescription("myServiceWithReplies", "unknow", "myFirstAgent ServiceReply description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceReplySetDescription("myServiceWithReplies", "myReply", "myFirstAgent ServiceReply description") == Result.Success);
            string agentReplyDescription = FirstAgent.ServiceReplyDescription("myServiceWithReplies", "myReply");
            Assert.AreEqual(agentReplyDescription, "myFirstAgent ServiceReply description");

            Assert.IsTrue(FirstAgent.ServiceReplyDescription("myServiceWithReplies", "myReply2") == null);
            Assert.IsTrue(FirstAgent.ServiceReplySetDescription("myServiceWithReplies", "myReply2", "myFirstAgent ServiceReply2 description") == Result.Success);

            Assert.IsTrue(FirstAgent.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myBool", IopValueType.Bool) == Result.Success);
            Assert.IsTrue(FirstAgent.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myBool") == null);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("unknow", "myReply", "myBool", "myFirstAgent ArgBool description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("myServiceWithReplies", "unknow", "myBool", "myFirstAgent ArgBool description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "unknow", "myFirstAgent ArgBool description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "myBool", "myFirstAgent ArgBool description") == Result.Success);
            string agentReplyArgBoolDescription = FirstAgent.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myBool");
            Assert.IsTrue(agentReplyArgBoolDescription == "myFirstAgent ArgBool description");
            Assert.IsTrue(FirstAgent.ServiceReplyArgAdd("myServiceWithReplies", "myReply", "myInt", IopValueType.Integer) == Result.Success);
            Assert.IsTrue(FirstAgent.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myInt") == null);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("unknow", "myReply", "myInt", "myFirstAgent ArgInt description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("myServiceWithReplies", "unknow", "myInt", "myFirstAgent ArgInt description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "unknow", "myFirstAgent ArgInt description") == Result.Failure);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "myInt", "myFirstAgent ArgInt description") == Result.Success);
            string agentReplyArgIntDescription = FirstAgent.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myInt");
            Assert.IsTrue(agentReplyArgIntDescription == "myFirstAgent ArgInt description");

            currentDefinition = FirstAgent.DefinitionJson();
            FirstAgent.ClearDefinition();
            FirstAgent.DefinitionLoadStr(currentDefinition);
            string newAgentReplyDescription = FirstAgent.ServiceReplyDescription("myServiceWithReplies", "myReply");
            Assert.IsTrue(newAgentReplyDescription == "myFirstAgent ServiceReply description");
            string newAgentReply2Description = FirstAgent.ServiceReplyDescription("myServiceWithReplies", "myReply2");
            Assert.IsTrue(newAgentReply2Description == "myFirstAgent ServiceReply2 description");
            string newAgentReplyArgBoolDescription = FirstAgent.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myBool");
            Assert.IsTrue(newAgentReplyArgBoolDescription == "myFirstAgent ArgBool description");
            string newAgentReplyArgIntDescription = FirstAgent.ServiceReplyArgDescription("myServiceWithReplies", "myReply", "myInt");
            Assert.IsTrue(newAgentReplyArgIntDescription == "myFirstAgent ArgInt description");

            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "myBool", "") == Result.Success);
            Assert.IsTrue(FirstAgent.ServiceReplyArgSetDescription("myServiceWithReplies", "myReply", "myInt", "") == Result.Success);
            FirstAgent.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myBool");
            FirstAgent.ServiceReplyArgRemove("myServiceWithReplies", "myReply", "myInt");

            Igs.AgentSetFamily("family_test");

            // Saturation control
            Igs.UnbindPipe();
            Igs.MonitorPipeStack(true);

            Assert.IsTrue(Igs.StartWithDevice("", _port) == Result.Failure);

            FirstAgent.Destroy();
            SecondAgent.Destroy();

            ///////////
            /// C# part

            Igs.RtSetTimestamps(true);
            Assert.IsTrue(Igs.RtTimestamps());
            Assert.IsTrue(Igs.RtGetCurrentTimestamp() == 0);

            Igs.RtSetTimestamps(false);
            Igs.RtSetTime(1000);
            Igs.RtSetTimestamps(true);
            Assert.IsTrue(Igs.RtTime() == 1000);

            Igs.LogIncludeData(true);
            Igs.LogIncludeServices(true);

            Igs.RtSetSynchronousMode(true);
            Assert.IsTrue(Igs.RtSynchronousMode());
            Igs.RtSetSynchronousMode(false);
            Assert.IsFalse(Igs.RtSynchronousMode());

            Igs.LogNoWarningIfUndefinedService(true);
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
