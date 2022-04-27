/*  =========================================================================
Helpers.cs

Copyright (c) the Contributors as noted in the AUTHORS file.
This file is part of Ingescape, see https://github.com/zeromq/ingescape.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
=========================================================================
*/

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Ingescape
{
    public partial class Igs
    {
        #region PtrToCSharpTypes
        internal static double PtrToDouble(IntPtr ptr)
        {
            double[] dValue = new double[1];
            Marshal.Copy(ptr, dValue, 0, 1);
            return dValue[0];
        }

        internal static int PtrToInt(IntPtr ptr)
        {
            int[] iValue = new int[1];
            Marshal.Copy(ptr, iValue, 0, 1);
            return iValue[0];
        }

        internal static bool PtrToBool(IntPtr ptr)
        {
            int[] ibValue = new int[1];
            Marshal.Copy(ptr, ibValue, 0, 1);
            bool bValue = Convert.ToBoolean(ibValue[0]);
            return bValue;
        }

        internal static byte[] PtrToData(IntPtr ptr, int size)
        {
            byte[] dataValue = new byte[size];
            Marshal.Copy(ptr, dataValue, 0, size);
            return dataValue;
        }

        internal static string PtrToStringFromISO(IntPtr native)
        {
            if (native == IntPtr.Zero)
            {
                return string.Empty;
            }

            int len = 0;

            while (Marshal.ReadByte(native, len) != 0)
            {
                ++len;
            }

            if (len == 0)
            {
                return string.Empty;
            }

            Encoding iso = Encoding.GetEncoding("ISO-8859-1");

            byte[] buffer = new byte[len];
            Marshal.Copy(native, buffer, 0, buffer.Length);

            string isoString = iso.GetString(buffer);
            return Encoding.Default.GetString(Encoding.Default.GetBytes(isoString));
        }

        internal static string PtrToStringFromUTF8(IntPtr native)
        {
            if (native == IntPtr.Zero)
            {
                return string.Empty;
            }

            int len = 0;

            while (Marshal.ReadByte(native, len) != 0)
            {
                ++len;
            }

            if (len == 0)
            {
                return string.Empty;
            }

            byte[] buffer = new byte[len];
            Marshal.Copy(native, buffer, 0, buffer.Length);
            string utf8String = Encoding.UTF8.GetString(buffer);
            return Encoding.Default.GetString(Encoding.Default.GetBytes(utf8String));
        }
        #endregion

        #region CSharpTypesToPtr
        internal static IntPtr StringToUTF8Ptr(string native)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(native + "\0");
            //Console.WriteLine(BitConverter.ToString(bytes));
            IntPtr ptr = Marshal.AllocHGlobal(bytes.Length);

            Marshal.Copy(bytes, 0, ptr, bytes.Length);
            return ptr;
        }

        internal static IntPtr StringToUTF8Ptr(string native, out uint size)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(native + "\0");
            IntPtr ptr = Marshal.AllocHGlobal(bytes.Length);

            Marshal.Copy(bytes, 0, ptr, bytes.Length);
            size = Convert.ToUInt32(bytes.Length);
            return ptr;
        }

        internal static IntPtr DoubleToPtr(double value, out uint size)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            IntPtr ptr = Marshal.AllocHGlobal(bytes.Length);

            Marshal.Copy(bytes, 0, ptr, bytes.Length);
            size = Convert.ToUInt32(bytes.Length);
            return ptr;
        }

        internal static IntPtr IntToPtr(int value, out uint size)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            IntPtr ptr = Marshal.AllocHGlobal(bytes.Length);

            Marshal.Copy(bytes, 0, ptr, bytes.Length);
            size = Convert.ToUInt32(bytes.Length);
            return ptr;
        }

        internal static IntPtr BoolToPtr(bool value, out uint size)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            IntPtr ptr = Marshal.AllocHGlobal(bytes.Length);

            Marshal.Copy(bytes, 0, ptr, bytes.Length);
            size = Convert.ToUInt32(bytes.Length);
            return ptr;
        }

        internal static IntPtr DataToPtr(byte[] value, out uint size)
        {
            IntPtr ptr = Marshal.AllocHGlobal(value.Length);
            Marshal.Copy(value, 0, ptr, value.Length);
            size = Convert.ToUInt32(value.Length);
            return ptr;
        }

        internal static IntPtr StringToLatin1Ptr(string native)
        {
            byte[] bytes = Encoding.GetEncoding("ISO-8859-1").GetBytes(native + "\0");
            IntPtr ptr = Marshal.AllocHGlobal(bytes.Length);
            Marshal.Copy(bytes, 0, ptr, bytes.Length);
            return ptr;
        }

        internal static IntPtr ObjectToPtr(object value, out uint size)
        {
            size = 0;
            IntPtr valuePtr = IntPtr.Zero;

            if (value.GetType() == typeof(string))
            {
                valuePtr = Igs.StringToUTF8Ptr(Convert.ToString(value), out size);
            }
            else if (value.GetType() == typeof(bool))
            {
                valuePtr = Igs.BoolToPtr(Convert.ToBoolean(value), out size);
            }
            else if (value.GetType() == typeof(byte[]))
            {
                valuePtr = Igs.DataToPtr((byte[])value, out size);
            }
            else if (value.GetType() == typeof(double))
            {
                valuePtr = Igs.DoubleToPtr(Convert.ToDouble(value), out size);
            }
            else if (value.GetType() == typeof(float))
            {
                valuePtr = Igs.DoubleToPtr(Convert.ToDouble(value), out size);
            }
            else if (value.GetType() == typeof(int))
            {
                valuePtr = Igs.IntToPtr(Convert.ToInt32(value), out size);
            }
            return valuePtr;
        }
        #endregion
    }
}
