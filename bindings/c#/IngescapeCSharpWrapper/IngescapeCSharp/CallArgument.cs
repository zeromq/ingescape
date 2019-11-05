using Ingescape;
using System;
using System.Collections.Generic;
using System.Text;

namespace Ingescape
{
    public class CallArgument
    {
        #region Properties

        public string Name { get; }

        public iopType_t Type { get; }

        public object Value { get; }

        #endregion

        #region Constructor

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name"></param>
        /// <param name="type"></param>
        /// <param name="value"></param>
        public CallArgument(string name, iopType_t type, object value)
        {
            Name = name;
            Type = type;
            Value = value;
        }

        #endregion
    }
}
