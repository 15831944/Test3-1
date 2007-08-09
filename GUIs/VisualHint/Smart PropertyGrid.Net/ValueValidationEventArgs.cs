// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Passed to the client application when a value set to a property is invalid as determined by an
    /// attached validator or by a TypeConverter, or when it comes back to a valid value.
    /// </summary>
    public class ValueValidationEventArgs : PropertyEventArgs
    {
        private PropertyEnumerator _invalidPropEnum;

        public PropertyEnumerator InvalidPropEnum
        {
            get { return _invalidPropEnum; }
        }

        private PropertyValue.ValueValidationResult _valueValidationResult;

        /// <summary>
        /// Indicates if a value is valid or not and, if it's not, indicates if it's due to the check of a validator
        /// or to a TypeConverter that was not able to convert a data to the property value type.
        /// </summary>
        public PropertyValue.ValueValidationResult ValueValidationResult
        {
            get { return _valueValidationResult; }
        }

        private object _valueToValidate;

        /// <summary>
        /// Reference to the value that was checked.
        /// </summary>
        public object ValueToValidate
        {
            get { return _valueToValidate; }
            set { _valueToValidate = value; }
        }

        private Exception _exception = null;

        /// <summary>
        /// Gives a reference to the exception that was generated when a TypeConverter was not able to convert
        /// a data to the property value type.
        /// </summary>
        public Exception Exception
        {
            get { return _exception; }
        }

        /// <summary>
        /// Propose an error message explaining the reason of an invalid value.
        /// </summary>
        /// <remarks>If the value is invalid because of a validator class, the message is the one proposed by
        /// the validator. If it is invalid because of a TypeConverter, the message is the one resulting of the
        /// generated exception.</remarks>
        public string Message
        {
            get
            {
                if ((_valueValidationResult == PropertyValue.ValueValidationResult.Validated) ||
                    (_valueValidationResult == PropertyValue.ValueValidationResult.PreviousValueRestored))
                    return "";

                if (_exception != null)
                    return _exception.Message;
                else
                    return PropertyValidatorBase.Message;
            }
        }

        public ValueValidationEventArgs(PropertyEnumerator propEnum, PropertyEnumerator invalidPropEnum, object value, PropertyValue.ValueValidationResult result)
            : base(propEnum)
        {
            _valueValidationResult = result;
            _valueToValidate = value;
            _invalidPropEnum = invalidPropEnum;
        }

        public ValueValidationEventArgs(PropertyEnumerator propEnum, PropertyEnumerator invalidPropEnum, object value, PropertyValue.ValueValidationResult result, Exception e)
            : this(propEnum, invalidPropEnum, value, result)
        {
            _exception = e;
        }
    }
}
