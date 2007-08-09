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
    /// By assigning this attribute to a property, it triggers a validation operation each time the value of
    /// the property is changed.
    /// </summary>
    [AttributeUsage(AttributeTargets.All, AllowMultiple = true)]
    public class PropertyValidatorAttribute : Attribute
    {
        private PropertyValidatorBase _validator;

        public PropertyValidatorBase Validator
        {
            get { return _validator; }
        }

        private string _childPropertyName = "";

        public string ChildPropertyName
        {
            get { return _childPropertyName; }
        }

        public PropertyValidatorAttribute()
        {
        }

		public PropertyValidatorAttribute(Type validatorType)
		{
			_validator = (Activator.CreateInstance(validatorType) as PropertyValidatorBase);
		}

        public PropertyValidatorAttribute(Type validatorType, params object[] parameters)
        {
            _validator = (Activator.CreateInstance(validatorType, parameters) as PropertyValidatorBase);
        }

        public PropertyValidatorAttribute(string childPropertyName, Type validatorType, params object[] parameters)
        {
            if (childPropertyName == null)
                throw new ArgumentException("The name of the child property can't be null");

            _childPropertyName = childPropertyName;
            _validator = (Activator.CreateInstance(validatorType, parameters) as PropertyValidatorBase);
        }

        public override Object TypeId
        {
            get { return "PropertyValidator" + _childPropertyName; }
        }

        private int _hashCode = 0;

        public override int GetHashCode()
        {
            if (_hashCode == 0)
            {
                _hashCode = _childPropertyName.GetHashCode();
                _hashCode ^= _validator.GetHashCode();
            }

            return _hashCode;
        }

        public override bool Equals(object obj)
        {
            PropertyValidatorAttribute attribute = obj as PropertyValidatorAttribute;
            if (attribute == null)
                return false;

            if (_validator.Equals(attribute._validator) == false)
                return false;

            if (_childPropertyName != attribute._childPropertyName)
                return false;

            return true;
        }
    }
}
