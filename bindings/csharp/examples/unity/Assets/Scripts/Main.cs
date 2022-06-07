//
//  Main.cs
//  UnityAgent version 1.0
//
//  no description
//  Copyright © 2021 Ingenuity i/o. All rights reserved.
//

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using Ingescape;
using UnityEngine;
using UnityEngine.UI;

public class Main : MonoBehaviour
{
    public string _device = null;
    public uint _port = 5670;
    public bool _verbose = true;

    void ImpulsionInputCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.SetImpulsionI();
        });
    }

    void BoolInputCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.BoolI = (bool)value;
        });
    }

    void IntegerInputCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.IntegerI = (int)value;
        });
    }

    void DoubleInputCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.DoubleI = (double)value;
        });
    }

    void StringInputCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.StringI = (string)value;
        });
    }

    void DataInputCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.DataI = (byte[])value;
        });
    }

    void BoolParameterCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.BoolP = (bool)value;
        });
    }

    void IntegerParameterCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.IntegerP = (int)value;
        });
    }

    void DoubleParameterCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.DoubleP = (double)value;
        });
    }

    void StringParameterCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.StringP = (string)value;
        });
    }

    void DataParameterCallback(IopType iopType,
                            string name,
                            IopValueType valueType,
                            object value,
                            object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.DataP = (byte[])value;
        });
    }

    void ServiceCallback(string senderAgentName,
                                string senderAgentUUID,
                                string serviceName,
                                List<ServiceArgument> serviceArguments,
                                string token,
                                object myData)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            UnityAgent agent = myData as UnityAgent;
            if (agent != null)
                agent.Service(senderAgentName, senderAgentUUID, serviceArguments);
        });
    }

    void ExternalStopCallback(object data)
    {
        Debug.Log("ExternalForcedStop callback ...");
        UnityMainThreadDispatcher.Instance().Enqueue(Close());
    }

    void Start()
    {
        Application.logMessageReceived += LogMessageReceived;
        Igs.AgentSetName("echo");
        Igs.DefinitionSetVersion("1.0");
        Igs.LogSetConsoleLevel(LogLevel.LogDebug);
        Igs.LogSetConsole(_verbose);
        Igs.LogSetFile(_verbose);
        Igs.LogSetFileLevel(LogLevel.LogDebug);
        Igs.LogSetStream(_verbose);

        string[] netDevicesList = Igs.NetDevicesList();
        string[] netAddressesList = Igs.NetAddressesList();
        int deviceCount = netDevicesList.Count();
        int addresseCount = netAddressesList.Count();

    #if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
        if (deviceCount == addresseCount && string.IsNullOrEmpty(_device))
        {
            if (deviceCount == 1)
            {
                _device = netDevicesList[0];
                Debug.Log(string.Format("using {0} as default network device (this is the only one available)", _device));
            }
            else if (deviceCount == 2 && (netAddressesList[0] == "127.0.0.1" || netAddressesList[1] == "127.0.0.1"))
            {
                if (netAddressesList[0] == "127.0.0.1")
                    _device = netDevicesList[1];
                else
                    _device = netDevicesList[0];
                Debug.Log(string.Format("using {0} as default network device (this is the only one available that is not the loopback)", _device));
            }
            else
            {
                if (deviceCount == 0)
                    Debug.LogError("No network device found");
                else
                {
                    Debug.LogError("More than 2 net devices. Please use one of these network devices :");
                    foreach (string netDevice in netDevicesList)                    
                        Debug.LogError(string.Format("- {0}", netDevice));
                }
                Application.Quit();
            }
        }
    #else
        if (string.IsNullOrEmpty(_device))
        {
            Debug.LogError("Please use one of these network devices :");
            foreach (string netDevice in netDevicesList)
                Debug.LogError(string.Format("- {0}", netDevice));
            Application.Quit();
        }
    #endif
        Igs.InputCreate("impulsion", IopValueType.Impulsion);
        Igs.InputCreate("bool", IopValueType.Bool);
        Igs.InputCreate("integer", IopValueType.Integer);
        Igs.InputCreate("double", IopValueType.Double);
        Igs.InputCreate("string", IopValueType.String);
        Igs.InputCreate("data", IopValueType.Data);

        Igs.ParameterCreate("bool", IopValueType.Bool);
        Igs.ParameterCreate("integer", IopValueType.Integer);
        Igs.ParameterCreate("double", IopValueType.Double);
        Igs.ParameterCreate("string", IopValueType.String);
        Igs.ParameterCreate("data", IopValueType.Data);

        Igs.OutputCreate("impulsion", IopValueType.Impulsion);
        Igs.OutputCreate("bool", IopValueType.Bool);
        Igs.OutputCreate("integer", IopValueType.Integer);
        Igs.OutputCreate("double", IopValueType.Double);
        Igs.OutputCreate("string", IopValueType.String);
        Igs.OutputCreate("data", IopValueType.Data);

        UnityAgent agent = new UnityAgent();

        Igs.ObserveInput("impulsion", ImpulsionInputCallback, agent);
        Igs.ObserveInput("bool", BoolInputCallback, agent);
        Igs.ObserveInput("integer", IntegerInputCallback, agent);
        Igs.ObserveInput("double", DoubleInputCallback, agent);
        Igs.ObserveInput("string", StringInputCallback, agent);
        Igs.ObserveInput("data", DataInputCallback, agent);

        Igs.ObserveParameter("bool", BoolParameterCallback, agent);
        Igs.ObserveParameter("integer", IntegerParameterCallback, agent);
        Igs.ObserveParameter("double", DoubleParameterCallback, agent);
        Igs.ObserveParameter("string", StringParameterCallback, agent);
        Igs.ObserveParameter("data", DataParameterCallback, agent);

        Igs.ServiceInit("service", ServiceCallback, agent);
        Igs.ServiceArgAdd("service", "bool_arg", IopValueType.Bool);
        Igs.ServiceArgAdd("service", "integer_arg", IopValueType.Integer);
        Igs.ServiceArgAdd("service", "double_arg", IopValueType.Double);
        Igs.ServiceArgAdd("service", "string_arg", IopValueType.String);
        Igs.ServiceArgAdd("service", "data_arg", IopValueType.Data);

        Igs.ObserveForcedStop(ExternalStopCallback, null);

        if (Igs.StartWithDevice(_device, _port) == Result.Success)
            Debug.Log(string.Format("Starting with device {0} on port {1}", _device, _port));
    }

    private void LogMessageReceived(string condition, string stackTrace, LogType type)
    {
        string[] stackTraceLines = stackTrace.Split('\n');
        string caller = (stackTraceLines.Length >= 2) ? stackTraceLines[1] : "";
        switch (type)
        {
            case (LogType.Assert):
            case (LogType.Error):
            case (LogType.Exception):
                Igs.Error(condition, caller);
                break;
            case (LogType.Warning):
                Igs.Warn(condition, caller);
                break;
            default:
                Igs.Debug(condition, caller);
                break;
        }
    }


    private IEnumerator Close()
    {
        Debug.Log("Application quit");
        Application.Quit();
        yield return null;
    }

    void OnDestroy()
    {
        Application.Quit();
    }

    private void OnApplicationQuit()
    {
        Application.logMessageReceived -= LogMessageReceived;
#if UNITY_EDITOR
        Igs.ClearContext();
#endif
        Igs.Stop();
        Debug.Log("IngeScape stoped");
    }
}
