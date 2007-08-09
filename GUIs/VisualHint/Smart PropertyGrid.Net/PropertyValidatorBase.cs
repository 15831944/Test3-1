// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Base class whose actual derived classes can validate the new value of a property.
    /// </summary>
    public abstract class PropertyValidatorBase
    {
        private PropertyEnumerator _enumerator = null;

        public PropertyEnumerator Enumerator
        {
            set { _enumerator = value; }
            get { return _enumerator; }
        }

        private static string _message = "";

        public static string Message
        {
            set { _message = value; }
            get { return _message; }
        }

        public PropertyValidatorBase()
        {
        }
        
        public PropertyValidatorBase(PropertyEnumerator propEnum)
        {
            _enumerator = propEnum;
        }

        public virtual bool Check(object value, bool modify)
        {
            return true;
        }
    }
}
