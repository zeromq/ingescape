using Ingescape;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;

namespace IsAPICallable
{
    /// <summary>
    /// Description résumée pour UnitTest1
    /// </summary>
    [TestClass]
    public class IngescapeAgentest
    {
        private string _agentName;
        private string _device;
        private uint _port;
        private Agent _subAgent;

        [TestInitialize]
        public void testInit()
        {
            _agentName = "unitTestAgent";
            _device = "Ethernet 2";
            _port = 5670;
            Igs.AgentSetName(_agentName);
            Igs.StartWithDevice(_device, _port);
            _subAgent = new Agent("subUnitTest", true);
        }

        [TestMethod]
        public void TestAgentDestruction()
        {
            _subAgent.Destroy();
        }

        [TestMethod]
        public void TestActivateDeactivate()
        {
            _subAgent.Activate();
            bool isActivated = _subAgent.IsActivated;
            _subAgent.Deactivate();
        }

        [TestMethod]
        public void TestNameUUIDAndState()
        {
            string agentName = _subAgent.Name;
            _subAgent.Name = "newSubUnitTest";
            string nameAfterChanged = _subAgent.Name;

            string uuid = _subAgent.UUID;

            string state = _subAgent.State;
            _subAgent.State = "state";
        }

        [TestMethod]
        public void TestMute()
        {
            _subAgent.Mute();
            bool isMuted = _subAgent.IsMuted;
            _subAgent.Unmute();
        }

        [TestMethod]
        public void TestIOP()
        {
            string inputStringName = "string";
            Result createResult = _subAgent.InputCreate(inputStringName, IopValueType.String);
            string inputStringResult = _subAgent.InputString(inputStringName);
            _subAgent.InputSetString(inputStringName, "value");
            string inputBoolName = "bool";
            createResult = _subAgent.InputCreate(inputBoolName, IopValueType.Bool);
            bool inputBoolResult = _subAgent.InputBool(inputBoolName);
            _subAgent.InputSetBool(inputBoolName, true);
            string inputIntName = "int";
            createResult = _subAgent.InputCreate(inputIntName, IopValueType.Integer);
            int inputIntResult = _subAgent.InputInt(inputIntName);
            _subAgent.InputSetInt(inputIntName, 1);
            string inputDoubleName = "double";
            createResult = _subAgent.InputCreate(inputDoubleName, IopValueType.Double);
            double inputDoubleResult = _subAgent.InputDouble(inputDoubleName);
            _subAgent.InputSetDouble(inputDoubleName, 1.0);
            string inputDataName = "data";
            createResult = _subAgent.InputCreate(inputDataName, IopValueType.Data);
            byte[] intputByteValue = _subAgent.InputData(inputDataName);
            _subAgent.InputSetData(inputDataName, new byte[1]);


            string outputStringName = "output";
            string outputStringValue = "output Value";
            _subAgent.OutputCreate(outputStringName, IopValueType.String);
            string outputValueResult = _subAgent.OutputString(outputStringName);
            _subAgent.OutputSetString(outputStringName, outputStringValue);
            string outputBoolName = "bool";
            _subAgent.OutputCreate(outputBoolName, IopValueType.Bool);
            bool outputBoolResult = _subAgent.OutputBool(outputBoolName);
            _subAgent.OutputSetBool(outputBoolName, true);
            string outputIntName = "int";
            _subAgent.OutputCreate(outputIntName, IopValueType.Integer);
            int outputIntResult = _subAgent.OutputInt(outputIntName);
            _subAgent.OutputSetInt(outputIntName, 1);
            string outputDoubleName = "double";
            _subAgent.OutputCreate(outputDoubleName, IopValueType.Double);
            double outputDoubleResult = _subAgent.OutputDouble(outputDoubleName);
            _subAgent.OutputSetDouble(outputDoubleName, 1.0);
            string outputDataName = "data";
            _subAgent.OutputCreate(outputDataName, IopValueType.Data);
            byte[] outputByteValue = _subAgent.OutputData(outputDataName);
            _subAgent.OutputSetData(outputDataName, new byte[1]);

            string parameterStringName = "parameter";
            string parameterStringValue = "parameter Value";
            _subAgent.ParameterCreate(parameterStringName, IopValueType.String);
            string parameterValueResult = _subAgent.ParameterString(parameterStringName);
            _subAgent.ParameterSetString(parameterStringName, parameterStringValue);
            string parameterBoolName = "bool";
            _subAgent.ParameterCreate(parameterBoolName, IopValueType.Bool);
            bool parameterBoolResult = _subAgent.ParameterBool(parameterBoolName);
            _subAgent.ParameterSetBool(parameterBoolName, true);
            string parameterIntName = "int";
            _subAgent.ParameterCreate(parameterIntName, IopValueType.Integer);
            int parameterIntResult = _subAgent.ParameterInt(parameterIntName);
            _subAgent.ParameterSetInt(parameterIntName, 1);
            string parameterDoubleName = "double";
            _subAgent.ParameterCreate(parameterDoubleName, IopValueType.Double);
            double parametertDoubleResult = _subAgent.ParameterDouble(parameterDoubleName);
            _subAgent.ParameterSetDouble(parameterDoubleName, 1.0);
            string parameterDataName = "data";
            _subAgent.ParameterCreate(parameterDataName, IopValueType.Data);
            byte[] parameterByteValue = _subAgent.ParameterData(parameterDataName);
            _subAgent.ParameterSetData(parameterDataName, new byte[1]);

            string[] inputs = _subAgent.InputList();
            string[] outputs = _subAgent.OutputList();
            string[] parameters = _subAgent.ParameterList();

            bool isInputcreated = _subAgent.InputExists(inputBoolName);
            bool isOutcreated = _subAgent.OutputExists(inputBoolName);
            bool isParametercreated = _subAgent.ParameterExists(inputBoolName);

            _subAgent.InputAddConstraint(inputStringName, "constraint");
            _subAgent.OutputAddConstraint(outputStringName, "constraint");
            _subAgent.ParameterAddConstraint(parameterStringName, "constraint");

            _subAgent.ConstraintsEnforce(true);

            _subAgent.InputSetDescription(inputStringName, "description");
            _subAgent.OutputSetDescription(outputStringName, "description");
            _subAgent.ParameterSetDescription(parameterStringName, "description");

            Result removeResult = _subAgent.InputRemove(inputStringName);
            removeResult = _subAgent.InputRemove(inputBoolName);
            removeResult = _subAgent.InputRemove(inputIntName);
            removeResult = _subAgent.InputRemove(inputDoubleName);
            removeResult = _subAgent.InputRemove(inputDataName);

            removeResult = _subAgent.OutputRemove(outputStringName);
            removeResult = _subAgent.OutputRemove(outputBoolName);
            removeResult = _subAgent.OutputRemove(outputIntName);
            removeResult = _subAgent.OutputRemove(outputDoubleName);
            removeResult = _subAgent.OutputRemove(outputDataName);

            removeResult = _subAgent.ParameterRemove(parameterStringName);
            removeResult = _subAgent.ParameterRemove(parameterBoolName);
            removeResult = _subAgent.ParameterRemove(parameterIntName);
            removeResult = _subAgent.ParameterRemove(parameterDoubleName);
            removeResult = _subAgent.ParameterRemove(parameterDataName);
        }

        [TestMethod]
        public void TestDefinition()
        {
            Result result = _subAgent.DefinitionLoadStr("");
            result = _subAgent.DefinitionLoadFile("");
            _subAgent.DefinitionDescription = "";
            _subAgent.DefinitionVersion = "";

            //string def = _subAgent.DefinitionJson();
            //string defName = _subAgent.getDefinitionName();
            //string defDescr = _subAgent.DefinitionDescription();
            //string defVersion = _subAgent.DefinitionVersion();

            _subAgent.ClearDefinition();
        }

        [TestMethod]
        public void TestMapping()
        {
            Result result = _subAgent.MappingLoadStr("");
            result = _subAgent.MappingLoadFile("");

            _subAgent.ClearMappings();
            _subAgent.ClearMappingsWithAgent("agent");

            //_subAgent.setMappingName("");
            //_subAgent.setMappingDescription("");
            //_subAgent.setMappingVersion("");

            string mapping = _subAgent.MappingJson();
            //string mappingName = _subAgent.getMappingName();
            //string mappingDescription = _subAgent.getMappingDescription();
            //string mappingVersion = _subAgent.getMappingVersion();

            uint entriesNumber = _subAgent.MappingCount();
            uint mappingEntry = _subAgent.MappingAdd("fromOurInput", "toAgent", "withOutput");

            Result isMappingEntryRemovedWithId = _subAgent.MappingRemoveWithId(21514875);
            Result isMappingEntryRemovedWithName = _subAgent.MappingRemoveWithName("fromOurInput", "toAgent", "withOutput");

            _subAgent.MappingSetOutputsRequest(false);
            bool isOutputsRequested = _subAgent.MappingOutputsRequest();

        }

        [TestMethod]
        public void TestAdmin()
        {
            _subAgent.MappingSetOutputsRequest(false);
            bool isOutputsRequested = _subAgent.MappingOutputsRequest();
            _subAgent.DefinitionSetPath("path");
            _subAgent.MappingSetPath("path");
            _subAgent.DefinitionSave();
            _subAgent.MappingSave();
        }

        public static void OnServiceCB(Agent agent, string senderAgentName, string senderAgentUUID, string serviceName, List<ServiceArgument> arguments, string token, object myData)
        {
            Console.WriteLine("on service callback");
        }

        [TestMethod]
        public void TestCall()
        {
            Result res = _subAgent.ServiceInit("serviceName", OnServiceCB, null);
            res = _subAgent.ServiceArgAdd("serviceName", "arg", IopValueType.String);
            res = _subAgent.ServiceArgRemove("serviceName", "arg");

            uint numberOfServices = _subAgent.ServiceCount();
            bool isServiceExisted = _subAgent.ServiceExists("serviceName");
            string[] servicesList = _subAgent.ServiceList();
            res = _subAgent.ServiceArgAdd("serviceName", "arg", IopValueType.String);
            uint numberOfArgsFor = _subAgent.ServiceArgsCount("serviceName");
            bool isServiceArgumentExisted = _subAgent.ServiceArgExists("serviceName", "arg");
        }

        [TestMethod]
        public void TestMasterSlave()
        {
            Result res = _subAgent.ElectionJoin("");
            res = _subAgent.ElectionLeave("");
        }

        [TestMethod]
        public void TestLog()
        {
            _subAgent.Trace("test");
            _subAgent.Debug("test");
            _subAgent.Info("test");
            _subAgent.Warn("test");
            _subAgent.Error("test");
            _subAgent.Fatal("test");
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
