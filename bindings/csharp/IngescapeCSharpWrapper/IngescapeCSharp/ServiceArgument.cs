/*  =========================================================================
ServiceArgument.cs

Copyright (c) the Contributors as noted in the AUTHORS file.
This file is part of Ingescape, see https://github.com/zeromq/ingescape.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
=========================================================================
*/

namespace Ingescape
{
    /// <summary>
    /// Argument of service (for IngeScape C#)
    /// </summary>
    public class ServiceArgument
    {
        #region Properties
        public string Name { get; }
        public IopValueType Type { get; }
        public object Value { get; }
        #endregion

        #region Constructor
        public ServiceArgument(IopValueType type, object value)
        {
            Name = "";
            Type = type;
            Value = value;
        }
        public ServiceArgument(string name, IopValueType type, object value)
        {
            Name = name;
            Type = type;
            Value = value;
        }
        #endregion
    }
}
