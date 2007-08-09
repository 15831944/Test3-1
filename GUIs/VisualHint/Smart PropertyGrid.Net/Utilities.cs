// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.Windows.Forms;

namespace VisualHint.SmartPropertyGrid
{
    internal class Utilities
    {
/*        public static bool IsNullableType(Type theType)
        {
            return (theType.IsGenericType &&
                theType.GetGenericTypeDefinition().Equals(typeof(Nullable<>)));
        }
        */
        public static Type GetTypeFromName(string typeName, Type valueType)
        {
            if ((typeName == null) || (typeName.Length == 0))
                return null;

            Type type = Type.GetType(typeName);
            if (type != null)
                return type;

            int pos = typeName.IndexOf(',');
            if (pos != -1)
                typeName = typeName.Substring(0, pos);

            return valueType.Assembly.GetType(typeName);
        }

        public static int GetHashCode(string[] strArray)
        {
            int hashCode = 0;
            foreach (string str in strArray)
                hashCode ^= str.GetHashCode();
            return hashCode;
        }

        public static ControlStyles DoubleBufferStyle
        {
            get
            {
#if _DOTNET2
                return ControlStyles.OptimizedDoubleBuffer;
#else
                return ControlStyles.DoubleBuffer;
#endif
            }
        }

        public class KeyValuePair
        {
            private object _key;

            private object _value;

            public KeyValuePair(object key, object value)
            {
                _value = value;
                _key = key;
            }

            public object Key
            {
                get
                {
                    return _key;
                }
            }

            public object Value
            {
                get
                {
                    return _value;
                }
            }
        }

    }
}
