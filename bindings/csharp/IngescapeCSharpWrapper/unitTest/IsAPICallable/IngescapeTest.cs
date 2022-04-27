using Ingescape;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace IsAPICallable
{
    [TestClass]
    public class IngescapeTest
    {
        private string _agentName;
        private string _device;
        private uint _port;

        [TestInitialize]
        public void testInit()
        {
            _agentName = "unitTestAgent";
            _device = "Connexion au réseau local";
            _port = 5555;
        }

        [TestMethod]
        public void TestInitializationAndControl()
        {
            Igs.DefinitionSetVersion("0.1");
            Igs.DefinitionSetDescription("");
            string uuid = Igs.AgentUUID();
            Console.WriteLine(uuid);
            Igs.LogSetConsoleLevel(LogLevel.LogDebug);
            Igs.LogSetFile(true);
            Igs.LogSetFileLevel(LogLevel.LogDebug);
            Igs.LogSetFileMaxLineLength(100000);
            Igs.LogSetStream(true);
            Igs.AgentSetName(_agentName);
            Igs.StartWithDevice(_device, _port);

            Igs.AgentName();
            Igs.AgentSetState("testState");
            Igs.SetCommandLine(" ");
            Igs.MappingSetOutputsRequest(false);
            Igs.ClearMappingsWithAgent("agentName");
            Igs.LogSetConsoleColor(true);
            Igs.AgentMute();
            Igs.AgentIsMuted();
            Igs.AgentUnmute();
            Igs.Freeze();
            Igs.IsFrozen();
            Igs.Unfreeze();

            Igs.SetCommandLine("testCommandLine");
            string line = Igs.CommandLine();
        }

        [TestMethod]
        public void TestIOPs()
        {
            //TODO more IOPs methods

            Igs.InputCreate("myInput", IopValueType.String);
            Igs.OutputCreate("myOutput", IopValueType.String);
            Igs.ParameterCreate("myParameter", IopValueType.String);

            Igs.InputAddConstraint("myInput", "constraint");
            Igs.OutputAddConstraint("myOutput", "constraint");
            Igs.ParameterAddConstraint("myParameter", "constraint");

            Igs.ConstraintsEnforce(true);

            Igs.InputSetDescription("myInput", "description");
            Igs.OutputSetDescription("myOutput", "description");
            Igs.ParameterSetDescription("myParameter", "description");
        }

            [TestMethod]
        public void TestBroker()
        {
            Igs.BrokerAdd("tcp://10.0.0.64:55000");
            Igs.ClearBrokers();
            Igs.BrokerEnableWithEndpoint("tcp://10.0.0.64:55000");
            Igs.BrokerSetAdvertizedEndpoint("");
        }

        [TestMethod]
        public void TestSplit()
        {
            Igs.InputCreate("myInput", IopValueType.String);
            ulong splitId = Igs.SplitAdd("myInput", "toAgent", "toAgentOutput");
            uint splitcount = Igs.SplitCount();
            Igs.SplitRemoveWithId(splitId);
            Igs.SplitRemoveWithName("myInput", "toAgent", "toAgentOutput");
        }

        [TestMethod]
        public void TestStartWithBroker()
        {
            Igs.StartWithBrokers("tcp://10.0.0.64:50500");
        }

        [TestMethod]
        public void TestSecurity()
        {
            string privateKey = " ";
            string publicKeyDic = " ";
            Result securityEnabled = Igs.EnableSecurity(privateKey, publicKeyDic);
            Igs.DisableSecurity();
            Result brokerSecureAdded = Igs.BrokerAddSecure(" ", " ");
        }

        [TestMethod]
        public void TestElection()
        {
            string electionName = "UnitElection";
            Result electionCompeted = Igs.ElectionJoin(electionName);
            Result electionLeaved = Igs.ElectionLeave(electionName);
        }

        [TestMethod]
        public void TestNetworkConfiguration()
        {
            Igs.NetSetPublishingPort(1111);
            Igs.NetSetLogStreamPort(1111);
            Igs.NetSetDiscoveryInterval(100);
            Igs.NetSetTimeout(10000);
            Igs.NetRaiseSocketsLimit();
            Igs.NetSetHighWaterMarks(100);
        }

        [TestMethod]
        public void TestPerfomanceCheck()
        {
            Igs.NetPerformanceCheck("", 2048, 100);
        }

        [TestMethod]
        public void TestFamily()
        {
            Igs.AgentSetFamily("family");
            string family = Igs.AgentFamily();
        }

        void TimerFunction(int timerId, object myData)
        {

        }

        [TestMethod]
        public void TestTimers()
        {
            int timerId = Igs.TimerStart(10, 2, TimerFunction, null);
            Igs.TimerStop(timerId);
        }

        [TestMethod]
        public void TestLogs()
        {
            Igs.Log(LogLevel.LogDebug, "TestLogs", "test");
            Igs.Debug("Debug");
            Igs.Info("Info");
            Igs.Trace("Trace");
            Igs.Warn("Warn");
            Igs.Fatal("Fatal");
            Igs.Error("Error");
        }

        void MonitorFunction(MonitorEvent monitorEvent, string device, string ipAddress, object myData)
        {

        }

        [TestMethod]
        public void TestNetworkMonitoring()
        {
            Igs.MonitorStart(1000);
            Igs.MonitorStop();
            Igs.MonitorStartWithNetwork(1000, _device, _port);
            bool isMonitorEnabled = Igs.MonitorIsRunning();
            Igs.ObserveMonitor(MonitorFunction, null);
        }

        [TestMethod]
        public void TestLogReplay()
        {
            Igs.ReplayInit("", 0, "", true, ReplayMode.ReplayInput, _agentName);
            Igs.ReplayStart();
            Igs.ReplayPause(true);
            Igs.ReplayTerminate();
        }

        void busMessageIncomming(string eventInfo,
                                   string peerID,
                                   string name,
                                   string address,
                                   string channel,
                                   object myData)
        {

        }


        [TestMethod]
        public void TestCalls()
        {
            // TO DO :
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
