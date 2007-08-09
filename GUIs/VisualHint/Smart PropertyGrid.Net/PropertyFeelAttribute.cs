// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// By assigning a feel attribute to a property, it defines the way this property will be edited.
    /// </summary>
    [AttributeUsage(AttributeTargets.All, AllowMultiple = true)]
    public class PropertyFeelAttribute : Attribute
    {
        private string _propertyFeelName;

        public string PropertyFeelName
        {
            get { return _propertyFeelName; }
        }

        private string _childPropertyName = "";

        public string ChildPropertyName
        {
            get { return _childPropertyName; }
        }

        public PropertyFeelAttribute(string childPropertyName, string propertyFeelName)
        {
            if (childPropertyName == null)
                throw new ArgumentException("The name of the child property can't be null");

            _childPropertyName = childPropertyName;
            _propertyFeelName = propertyFeelName;
        }

        public PropertyFeelAttribute(string propertyFeelName)
        {
            _propertyFeelName = propertyFeelName;
        }

        public override Object TypeId
        {
            get { return "PropertyFeel" + _childPropertyName; }
        }

        private int _hashCode = 0;

        public override int GetHashCode()
        {
            if (_hashCode == 0)
            {
                _hashCode ^= _childPropertyName.GetHashCode();
                _hashCode ^= _propertyFeelName.GetHashCode();
            }

            return _hashCode;
        }

        public override bool Equals(object obj)
        {
            PropertyFeelAttribute attribute = obj as PropertyFeelAttribute;
            if (attribute == null)
                return false;

            if (_propertyFeelName != attribute._propertyFeelName)
                return false;

            if (_childPropertyName != attribute._childPropertyName)
                return false;

            return true;
        }
    }
}
