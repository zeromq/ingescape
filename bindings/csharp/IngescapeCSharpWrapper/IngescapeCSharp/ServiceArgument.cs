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

        /// <summary>
        /// Name of our argument
        /// </summary>
        public string Name { get; }

        /// <summary>
        /// Type of the value
        /// </summary>
        public IopValueType Type { get; }

        /// <summary>
        /// Can be a bool, an int, a double, a string or a byte[]
        /// </summary>
        public object Value { get; }

        #endregion

        #region Constructor

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="type"></param>
        /// <param name="value"></param>
        public ServiceArgument(IopValueType type, object value)
        {
            Name = "";
            Type = type;
            Value = value;
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name"></param>
        /// <param name="type"></param>
        /// <param name="value"></param>
        public ServiceArgument(string name, IopValueType type, object value)
        {
            Name = name;
            Type = type;
            Value = value;
        }

        #endregion
    }
}
