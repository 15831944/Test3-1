// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System;
using System.ComponentModel;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Allows to build multiple-value properties in reflection mode.
    /// </summary>
    /// <remarks>
    /// When assigned to a .net property which is discovered by reflection (using SelectedObjec), the PropertyGrid
    /// won't create a new property. Instead it will add its value to another property, like a unit to its
    /// numerical value.
    /// </remarks>
    public class ValueAddedToPropertyAttribute : Attribute
    {
        private string _propertyName;

        public string PropertyName
        {
            get { return _propertyName; }
        }

        private string _valueKey;

        public string ValueKey
        {
            get { return _valueKey; }
        }

        /// <summary>
        /// Initializes a new instance of the ValueAddedToPropertyAttribute class.
        /// </summary>
        /// <param name="valueKey">A unique key to the additional value. This is the same parameter than the first
        /// one passed to <see href="/VisualHint.SmartPropertyGrid.Property.AddValue_overloads.html"/>.</param>
        /// <param name="propertyName">The name of the .net property that will host the additional value.</param>
        public ValueAddedToPropertyAttribute(string valueKey, string propertyName)
        {
            _propertyName = propertyName;
            _valueKey = valueKey;
        }

        private int _hashCode = 0;

        public override int GetHashCode()
        {
            if (_hashCode == 0)
            {
                _hashCode = _propertyName.GetHashCode();
                _hashCode ^= _valueKey.GetHashCode();
            }

            return _hashCode;
        }

        public override bool Equals(object obj)
        {
            ValueAddedToPropertyAttribute attribute = obj as ValueAddedToPropertyAttribute;
            if (attribute == null)
                return false;

            return (_propertyName.Equals(attribute._propertyName) &&
                    _valueKey.Equals(attribute._valueKey));
        }
    }
}
