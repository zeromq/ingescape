
namespace Ingescape
{
    /// <summary>
    /// Argument of Call (for IngeScape C#)
    /// </summary>
    public class CallArgument
    {
        #region Properties

        /// <summary>
        /// Name of our argument
        /// </summary>
        public string Name { get; }

        /// <summary>
        /// Type of the value
        /// </summary>
        public iopType_t Type { get; }

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
        public CallArgument(iopType_t type, object value)
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
        public CallArgument(string name, iopType_t type, object value)
        {
            Name = name;
            Type = type;
            Value = value;
        }

        #endregion
    }
}
