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
    /// By assigning a look attribute to a property, it defines the way this property will be displayed.
    /// </summary>
    [AttributeUsage(AttributeTargets.All, AllowMultiple = true)]
    public class PropertyLookAttribute : Attribute
    {
        private Type _lookType;

        public Type LookType
        {
            get { return _lookType; }
        }

        private string _childPropertyName = "";

        public string ChildPropertyName
        {
            get { return _childPropertyName; }
        }

        private object[] _parameters;

        public object[] Parameters
        {
            get { return _parameters; }
        }

        public PropertyLook GetPropertyLook(string childPropertyName)
        {
            if (_childPropertyName.CompareTo(childPropertyName) == 0)
                return (PropertyLook)Activator.CreateInstance(_lookType);

            return null;
        }

        public PropertyLookAttribute(string childPropertyName, Type lookType)
        {
            if (childPropertyName == null)
                throw new ArgumentException("The name of the child property can't be null");

            _childPropertyName = childPropertyName;
            _lookType = lookType;
        }

        public PropertyLookAttribute(string childPropertyName, Type lookType, params object[] parameters)
        {
            if (childPropertyName == null)
                throw new ArgumentException("The name of the child property can't be null");

            _childPropertyName = childPropertyName;
            _lookType = lookType;
            _parameters = (object[])parameters.Clone();
        }

        public PropertyLookAttribute(Type lookType)
        {
            _lookType = lookType;
        }

        public PropertyLookAttribute(Type lookType, params object[] parameters)
        {
            _lookType = lookType;
            _parameters = (object[])parameters.Clone();
        }

        public override Object TypeId
        {
            get { return "PropertyLook" + _childPropertyName; }
        }

        private int _hashCode = 0;

        public override int GetHashCode()
        {
            if (_hashCode == 0)
            {
                _hashCode ^= _childPropertyName.GetHashCode();
                _hashCode ^= _lookType.GetHashCode();
            }

            return _hashCode;
        }

        public override bool Equals(object obj)
        {
            PropertyLookAttribute attribute = obj as PropertyLookAttribute;
            if (attribute == null)
                return false;

            if (_lookType.Equals(attribute._lookType) == false)
                return false;

            if (_childPropertyName != attribute._childPropertyName)
                return false;

            return true;
        }
    }
}
