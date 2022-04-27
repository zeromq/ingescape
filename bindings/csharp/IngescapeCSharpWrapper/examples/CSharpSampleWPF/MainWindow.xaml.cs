using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Text.RegularExpressions;
using Ingescape;

namespace CSharpSampleWPFNamespace
{
    public enum ExitCode
    {
        Success = 0,
        ExternalStop = 1,
        NoDevice = 2
    }    

    public partial class MainWindow : Window
    {
        const string _dataPattern = @"^[a-fA-F0-9]+$";
        const string _doublePattern = @"^[0-9]+([,.][0-9]+)?$";
        const string _integerPattern = @"^[0-9]+$";

        static string _device = null;
        static uint _port = 5670;
        static bool _verbose = true;
        Dispatcher _dispatcherOfMainWindow = null;

        CSharpSampleWPF _agent;
        public CSharpSampleWPF Agent { get { return _agent; } }

        void ImpulsionInputCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.SetImpulsionI();
                }));
            }
        }
        void BoolInputCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.BoolI = (bool)value;
                }));
            }
        }
        void IntegerInputCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.IntegerI = (int)value;
                }));
            }
        }
        void DoubleInputCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.DoubleI = (double)value;
                }));
            }
        }
        void StringInputCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.StringI = (string)value;
                }));
            }
        }
        void DataInputCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.DataI = (byte[])value;
                }));
            }
        }

        void BoolParameterCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.BoolP = (bool)value;
                }));
            }
        }
        void IntegerParameterCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.IntegerP = (int)value;
                }));
            }
        }
        void DoubleParameterCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.DoubleP = (double)value;
                }));
            }
        }
        void StringParameterCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.StringP = (string)value;
                }));
            }
        }
        void DataParameterCallback(IopType iopType,
                                string name,
                                IopValueType valueType,
                                object value,
                                object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.DataP = (byte[])value;
                }));
            }
        }

        void ServiceCallback(string senderAgentName,
                                    string senderAgentUUID,
                                    string serviceName,
                                    List<ServiceArgument> serviceArguments,
                                    string token,
                                    object myData)
        {
            if (_dispatcherOfMainWindow != null)
            {
                _dispatcherOfMainWindow.InvokeAsync(new Action(() =>
                {
                    CSharpSampleWPF agent = myData as CSharpSampleWPF;
                    if (agent != null)
                        agent.Service(senderAgentName, senderAgentUUID, serviceArguments);
                }));
            }
        }

        void ExternalStopCallback(object data)
        {
            Igs.Info("ExternalForcedStop callback ...");
            Environment.Exit((int)ExitCode.ExternalStop);
        }

        public MainWindow()
        {
            InitializeComponent();
            _dispatcherOfMainWindow = this.Dispatcher;
            this.DataContext = this;

            Igs.AgentSetName("CSharpSampleWPF");
            Igs.DefinitionSetVersion("1.0");
            Igs.LogSetConsoleLevel(LogLevel.LogDebug);
            Igs.LogSetConsole(_verbose);
            Igs.LogSetFile(_verbose);
            Igs.LogSetFileLevel(LogLevel.LogDebug);
            Igs.LogSetStream(_verbose);
            Igs.SetCommandLineFromArgs(Environment.GetCommandLineArgs());

            string[] netDevicesList = Igs.NetDevicesList();
            string[] netAddressesList = Igs.NetAddressesList();
            int deviceCount = netDevicesList.Count();
            int addresseCount = netAddressesList.Count();

            if (deviceCount == addresseCount && string.IsNullOrEmpty(_device))
            {
                if (deviceCount == 1)
                {
                    _device = netDevicesList[0];
                    Igs.Info(string.Format("using {0} as default network _device (this is the only one available)", _device));
                }
                else if (deviceCount == 2 && (netAddressesList[0] == "127.0.0.1" || netAddressesList[1] == "127.0.0.1"))
                {
                    if (netAddressesList[0] == "127.0.0.1")
                        _device = netDevicesList[1];
                    else
                        _device = netDevicesList[0];
                    Igs.Info(string.Format("using {0} as default network _device (this is the only one available that is not the loopback)", _device));
                }
                else
                {
                    if (deviceCount == 0)
                        Igs.Error("No network _device found");
                    else
                    {
                        Igs.Error("More than 2 net devices. Please use one of these network devices :");
                        foreach (string netDevice in netDevicesList)
                            Igs.Info(string.Format("- {0}", netDevice));
                    }
                    Igs.Info("Press any key to exit");
                    Console.ReadKey();
                    Environment.Exit((int)ExitCode.NoDevice);
                }
            }

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

            CSharpSampleWPF agent = new CSharpSampleWPF();

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

            Igs.StartWithDevice(_device, _port);

            _agent = agent;
        }
    

        private void SendImpulsion(object sender, RoutedEventArgs e)
        {
            if (_agent != null)
                _agent.SetImpulsionO();
        }

        private void SendBoolean(object sender, RoutedEventArgs e)
        {
            if (_agent != null)
            {
                ComboBoxItem selectedItem = (ComboBoxItem)ComboBoxBooleanOutput.SelectedItem;
                if ((selectedItem != null))
                {
                    bool value = false;
                    if (bool.TryParse((string)selectedItem.Content, out value))
                        _agent.BoolO = value;
                    else
                        Igs.Warn(string.Format("'{0}' is not a valid bool number", (string)selectedItem.Content));
                }
            }
            else
                Igs.Error("Property agent is null");
        }

        private void IntegerOutputValueChanged(object sender, TextChangedEventArgs args)
        {
            string integerText = TbxIntegerOutput.Text;

            BtnSendInteger.IsEnabled = (integerText.Length > 0) && Regex.IsMatch(integerText, _integerPattern);
            LblIntegerOutputError.Visibility = (integerText.Length == 0) || BtnSendInteger.IsEnabled ? Visibility.Hidden : Visibility.Visible;
        }

        private void SendInteger(object sender, RoutedEventArgs e)
        {
            if ((_agent != null))
            {
                string integerText = TbxIntegerOutput.Text;
                if (!string.IsNullOrEmpty(integerText))
                {
                    if (Regex.IsMatch(integerText, _integerPattern))
                    {
                        int value = Convert.ToInt32(integerText.Replace(".", ","));
                        _agent.IntegerO = value;
                    }
                    else
                        Igs.Warn(string.Format("'{0}' is not a valid integer number", integerText));
                }
                else
                    Igs.Warn("The integer text is null or empty");
            }
            else
                Igs.Error("Property agent is null");
        }

        private void DoubleOutputValueChanged(object sender, TextChangedEventArgs args)
        {
            string doubleText = TbxDoubleOutput.Text;

            BtnSendDouble.IsEnabled = (doubleText.Length > 0) && Regex.IsMatch(doubleText, _doublePattern);
            LblDoubleOutputError.Visibility = (doubleText.Length == 0) || BtnSendDouble.IsEnabled ? Visibility.Hidden : Visibility.Visible;
        }

        private void SendDouble(object sender, RoutedEventArgs e)
        {
            if ((_agent != null))
            {
                string doubleText = TbxDoubleOutput.Text;
                if (!string.IsNullOrEmpty(doubleText))
                {
                    if (Regex.IsMatch(doubleText, _doublePattern))
                    {
                        double value = Convert.ToDouble(doubleText.Replace(".", ","));
                        _agent.DoubleO = value;
                    }
                    else
                        Igs.Warn(string.Format("'{0}' is not a valid double number", doubleText));
                }
                else
                    Igs.Warn("The double text is null or empty");
            }
            else
                Igs.Error("Property agent is null");
        }

        private void StringOutputValueChanged(object sender, TextChangedEventArgs args)
        {
            BtnSendString.IsEnabled = TbxStringOutput.Text.Length > 0;
        }

        private void SendString(object sender, RoutedEventArgs e)
        {
            if (_agent != null)
            {
                string stringValue = TbxStringOutput.Text;
                if (!string.IsNullOrEmpty(stringValue))
                    _agent.StringO = stringValue;
                else
                    Igs.Warn("The string value is null or empty");
            }
            else
                Igs.Error("Property agent is null");
        }
        
        private void DataOutputValueChanged(object sender, TextChangedEventArgs args)
        {
            string dataText = TbxDataOutput.Text;

            BtnSendData.IsEnabled = (dataText.Length > 0) && ((dataText.Length % 2) == 0) && Regex.IsMatch(dataText, _dataPattern);
            LblDataOutputError.Visibility = (dataText.Length == 0) || BtnSendData.IsEnabled ? Visibility.Hidden : Visibility.Visible;
        }

        private void SendData(object sender, RoutedEventArgs e)
        {
            if (_agent != null)
            {
                string dataText = TbxDataOutput.Text;
                if (!string.IsNullOrEmpty(dataText))
                {
                    if (((dataText.Length % 2) == 0) && Regex.IsMatch(dataText, _dataPattern))
                    {
                        byte[] value = new byte[dataText.Length / 2];
                        for (int i = 0; i < dataText.Length; i += 2)
                            value[i / 2] = Convert.ToByte(dataText.Substring(i, 2), 16);
                        _agent.DataO = value;
                    }
                    else
                        Igs.Warn("The data value must be even and in hexadecimal");
                }
                else
                    Igs.Warn("The data to send is null or empty");
            }
            else
                Igs.Error("Property agent is null");
        }
    }
}
