//
//  CSharpSampleUnity.cs
//  CSharpSampleUnity version 1.0
//
//  no description
//  Copyright © 2021 Ingenuity i/o. All rights reserved.
//
using Ingescape;
using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class CSharpSampleUnity
{
    //inputs
    public void SetImpulsionI()
    {
        Debug.Log("'impulsion' changed (impulsion)");
        SetImpulsionO();
    }

    private bool _boolI;
    public bool BoolI
    {

        get { return _boolI; }

        set
        {
            if (_boolI != value)
                _boolI = value;

            Debug.Log(string.Format("'bool' bool value changed. New value : {0}", value.ToString()));
            BoolO = value;
        }
    }

    private int _integerI;
    public int IntegerI
    {

        get { return _integerI; }

        set
        {
            if (_integerI != value)
                _integerI = value;

            Debug.Log(string.Format("'integer' int value changed. New value : {0}", value.ToString()));
            IntegerO = value;
        }
    }

    private double _doubleI;
    public double DoubleI
    {

        get { return _doubleI; }

        set
        {
            if (_doubleI != value)
                _doubleI = value;

            Debug.Log(string.Format("'double' double value changed. New value : {0}", value.ToString()));
            DoubleO = value;
        }
    }

    private string _stringI;
    public string StringI
    {

        get { return _stringI; }

        set
        {
            if (_stringI != value)
                _stringI = value;

            Debug.Log(string.Format("'string' string value changed. New value : {0}", value.ToString()));
            StringO = value;
        }
    }

    private byte[] _dataI;
    public byte[] DataI
    {

        get { return _dataI; }

        set
        {
            if (_dataI != value)
                _dataI = value;

            Debug.Log(string.Format("'data' byte[] value changed. New value : {0}", BitConverter.ToString(value)));
            DataO = value;
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

            Debug.Log(string.Format("'bool' bool value changed. New value : {0}", value.ToString()));
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

            Debug.Log(string.Format("'integer' int value changed. New value : {0}", value.ToString()));
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

            Debug.Log(string.Format("'double' double value changed. New value : {0}", value.ToString()));
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

            Debug.Log(string.Format("'string' string value changed. New value : {0}", value.ToString()));
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

            Debug.Log(string.Format("'data' byte[] value changed. New value : {0}", BitConverter.ToString(value)));
        }
    }

    public CSharpSampleUnity()
    {
    }

    //services
    public void Service(string fromAgent, string fromAgentUUID, List<ServiceArgument> arguments)
    {
        Debug.Log(string.Format("Received service 'service' from {0} with following arguments :", fromAgent));
        foreach (ServiceArgument argument in arguments)
        {
            string value = (argument.Type == IopValueType.Data) ? BitConverter.ToString((byte[])argument.Value) : argument.Value.ToString();
            Debug.Log(string.Format("    - {0} : {1}", argument.Name, value));
        }
    }
}
