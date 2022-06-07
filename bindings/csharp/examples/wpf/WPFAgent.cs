//
//  WPFAgent.cs
//  WPFAgent version 1.0
//  Created by IngeScape Editor on 2022/04/07
//
//  no description
//  Copyright © 2021 Ingenuity i/o. All rights reserved.
//
using Ingescape;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;

namespace WPFNamespace
{
    public class WPFAgent : INotifyPropertyChanged
    {
        const int _maxServiceLogLength = 4000;

        //inputs
        private int _impulsionICount;
        public int ImpulsionICount
        {

            get { return _impulsionICount; }

            set
            {
                if (_impulsionICount != value)
                {
                    _impulsionICount = value;
                    OnPropertyChanged();
                }
            }
        }

        public void SetImpulsionI()
        {
            Igs.Info("'impulsion' changed (impulsion)");
            ImpulsionICount++;
        }

        private bool _boolI;
        public bool BoolI
        {

            get { return _boolI; }

            set
            {
                if (_boolI != value)
                {
                    _boolI = value;
                    OnPropertyChanged();
                }
                Igs.Info(string.Format("'bool' bool value changed. New value : {0}", value.ToString()));
            }
        }

        private int _integerI;
        public int IntegerI
        {

            get { return _integerI; }

            set
            {
                if (_integerI != value)
                {
                    _integerI = value;
                    OnPropertyChanged();
                }
                Igs.Info(string.Format("'integer' int value changed. New value : {0}", value.ToString()));
            }
        }

        private double _doubleI;
        public double DoubleI
        {

            get { return _doubleI; }

            set
            {
                if (_doubleI != value)
                {
                    _doubleI = value;
                    OnPropertyChanged();
                }
                Igs.Info(string.Format("'double' double value changed. New value : {0}", value.ToString()));
            }
        }

        private string _stringI;
        public string StringI
        {

            get { return _stringI; }

            set
            {
                if (_stringI != value)
                {
                    _stringI = value;
                    OnPropertyChanged();
                }
                Igs.Info(string.Format("'string' string value changed. New value : {0}", value.ToString()));
            }
        }

        private string _dataIString;
        public string DataIString
        {

            get { return _dataIString; }

            set
            {
                if (_dataIString != value)
                {
                    _dataIString = value;
                    OnPropertyChanged();
                }
            }
        }

        private byte[] _dataI;
        public byte[] DataI
        {

            get { return _dataI; }

            set
            {
                if (_dataI != value)
                {
                    _dataI = value;
                    DataIString = BitConverter.ToString(value);
                    OnPropertyChanged();
                }
                Igs.Info(string.Format("'data' byte[] value changed. New value : {0}", DataIString));
            }
        }

        //outputs
        public void SetImpulsionO()
        {
            Igs.OutputSetImpulsion("impulsion");
        }

        private bool _boolO;
        public bool BoolO
        {
            get { return _boolO; }

            set
            {
                if (_boolO != value)
                    _boolO = value;
                Igs.OutputSetBool("bool", (bool)value);
            }
        }

        private int _integerO;
        public int IntegerO
        {
            get { return _integerO; }

            set
            {
                if (_integerO != value)
                    _integerO = value;
                Igs.OutputSetInt("integer", (int)value);
            }
        }

        private double _doubleO;
        public double DoubleO
        {
            get { return _doubleO; }

            set
            {
                if (_doubleO != value)
                    _doubleO = value;
                Igs.OutputSetDouble("double", (double)value);
            }
        }

        private string _stringO;
        public string StringO
        {
            get { return _stringO; }

            set
            {
                if (_stringO != value)
                    _stringO = value;
                Igs.OutputSetString("string", (string)value);
            }
        }

        private byte[] _dataO;
        public byte[] DataO
        {
            get { return _dataO; }

            set
            {
                if (_dataO != value)
                    _dataO = value;
                Igs.OutputSetData("data", (byte[])value);
            }
        }

        //parameters
        private bool _boolP;
        public bool BoolP
        {
            get { return _boolP; }

            set
            {
                if (_boolP != value)
                    _boolP = value;


                Igs.Info(string.Format("'bool' bool value changed. New value : {0}", value.ToString()));
            }
        }

        private int _integerP;
        public int IntegerP
        {
            get { return _integerP; }

            set
            {
                if (_integerP != value)
                    _integerP = value;


                Igs.Info(string.Format("'integer' int value changed. New value : {0}", value.ToString()));
            }
        }

        private double _doubleP;
        public double DoubleP
        {
            get { return _doubleP; }

            set
            {
                if (_doubleP != value)
                    _doubleP = value;


                Igs.Info(string.Format("'double' double value changed. New value : {0}", value.ToString()));
            }
        }

        private string _stringP;
        public string StringP
        {
            get { return _stringP; }

            set
            {
                if (_stringP != value)
                    _stringP = value;


                Igs.Info(string.Format("'string' string value changed. New value : {0}", value.ToString()));
            }
        }

        private byte[] _dataP;
        public byte[] DataP
        {
            get { return _dataP; }

            set
            {
                if (_dataP != value)
                    _dataP = value;


                Igs.Info(string.Format("'data' byte[] value changed. New value : {0}", value.ToString()));
            }
        }

        private string _servicesString;
        public string ServicesString
        {
            get { return _servicesString; }
            set 
            { 
                if (_servicesString != value)
                    _servicesString = value;
                OnPropertyChanged();
            }
        }

        public WPFAgent()
        {
        }

        //services
        public void Service(string fromAgent, string fromAgentUUID, List<ServiceArgument> arguments)
        {            
            string log = string.Format("{0} ; Received the service 'service' from {1} with following arguments : \n", DateTime.Now.ToString(), fromAgent);

            foreach (ServiceArgument argument in arguments)
            {
                string value = (argument.Type == IopValueType.Data) ? BitConverter.ToString((byte[])argument.Value) : argument.Value.ToString();
                log += string.Format("     - {0} : {1} \n", argument.Name, value);
            }
            Igs.Info(log);
            ServicesString += log;
            while (ServicesString.Length >= _maxServiceLogLength)
            {
                int backslashIndex = ServicesString.IndexOf("\n");
                ServicesString = ServicesString.Remove(0, backslashIndex + 1);
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }
    }
}
