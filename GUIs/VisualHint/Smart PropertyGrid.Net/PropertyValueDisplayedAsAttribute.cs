// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System;
using System.Collections;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// When assigned to a property that has discrete values, it allows to modify the string displayed for its current value.
    /// </summary>
    [AttributeUsage(AttributeTargets.All, AllowMultiple = true)]
    public class PropertyValueDisplayedAsAttribute : Attribute
    {
        private string[] _displayedStrings;

        public virtual string[] DisplayedStrings
        {
            get
            {
                if (_childPropertyName.Length != 0)
                    return null;

                return _displayedStrings;
            }
        }

        private string _childPropertyName = "";

        public override Object TypeId
        {
            get { return "PropertyValueDisplayedAs" + _childPropertyName; }
        }

        public PropertyValueDisplayedAsAttribute()
        {
        }

        public PropertyValueDisplayedAsAttribute(string[] displayedStrings)
        {
            _displayedStrings = displayedStrings;
        }

        public PropertyValueDisplayedAsAttribute(ICollection displayedStrings)
        {
            _displayedStrings = new string[displayedStrings.Count];
            displayedStrings.CopyTo(_displayedStrings, 0);
        }

        public PropertyValueDisplayedAsAttribute(string displayedString)
        {
            _displayedStrings = new string[1];
            _displayedStrings[0] = displayedString;
        }

        public PropertyValueDisplayedAsAttribute(string childPropertyName, string[] displayedStrings)
        {
            if (childPropertyName == null)
                throw new ArgumentException("The name of the child property can't be null");

            _displayedStrings = displayedStrings;
            _childPropertyName = childPropertyName;
        }

        public PropertyValueDisplayedAsAttribute(string childPropertyName, ICollection displayedStrings)
        {
            if (childPropertyName == null)
                throw new ArgumentException("The name of the child property can't be null");

            _displayedStrings = new string[displayedStrings.Count];
            displayedStrings.CopyTo(_displayedStrings, 0);

            _childPropertyName = childPropertyName;
        }

        public virtual string[] GetDisplayedStrings(string propertyName)
        {
            if (propertyName.CompareTo(_childPropertyName) == 0)
                return _displayedStrings;

            return null;
        }

        private int _hashCode = 0;

        public override int GetHashCode()
        {
            if (_hashCode == 0)
            {
                if (_displayedStrings != null)
                    _hashCode = Utilities.GetHashCode(_displayedStrings);

                _hashCode ^= _childPropertyName.GetHashCode();
            }

            return _hashCode;
        }

        public override bool Equals(object obj)
        {
            PropertyValueDisplayedAsAttribute attribute = obj as PropertyValueDisplayedAsAttribute;
            if (attribute == null)
                return false;

            if (_childPropertyName.CompareTo(attribute._childPropertyName) != 0)
                return false;

            if (_displayedStrings != null)
            {
                if (_displayedStrings.Length != attribute._displayedStrings.Length)
                    return false;

                for (int i = 0; i < _displayedStrings.Length; i++)
                {
                    if (_displayedStrings[i] != attribute._displayedStrings[i])
                        return false;
                }
            }

            return true;
        }
    }
}
