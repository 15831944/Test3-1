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
    /// Allows to hide child properties of a parent property.
    /// </summary>
    /// <remarks>
    /// A PropertyGrid can be used to see the value of a complex property by showing subproperties. This is the
    /// case for a font or a pen for example. In some cases, you will want to hide one or more of these subproperties
    /// and this attribute does exactly that. Since you have no access to the subproperties, the attribute is
    /// applied on the parent.
    /// </remarks>
    /// <example>
    /// <code>
    /// // Let's hide the Width property of MyPen property
    /// [PropertyHide("Width")]
    /// public Pen MyPen { ... }
    /// </code>
    /// </example>
    [AttributeUsage(AttributeTargets.All, AllowMultiple = true)]
    public class PropertyHideAttribute : Attribute
    {
        private string _childPropertyName = "";

        public string ChildPropertyName
        {
            get { return _childPropertyName; }
        }

        public PropertyHideAttribute()
        {
        }

        public PropertyHideAttribute(string childPropertyName)
        {
            if ((childPropertyName == null) || (childPropertyName.Length == 0))
                throw new ArgumentException("The name of the child property can't be null or empty");

            _childPropertyName = childPropertyName;
        }

        public override Object TypeId
        {
            get { return "PropertyHide" + _childPropertyName; }
        }

        private int _hashCode = 0;

        public override int GetHashCode()
        {
            if (_hashCode == 0)
                _hashCode = _childPropertyName.GetHashCode();

            return _hashCode;
        }

        public override bool Equals(object obj)
        {
            PropertyHideAttribute attribute = obj as PropertyHideAttribute;
            if (attribute == null)
                return false;

            if ((_childPropertyName != null) && (attribute._childPropertyName != null))
            {
                if (_childPropertyName.CompareTo(attribute._childPropertyName) != 0)
                    return false;
            }

            return true;
        }
    }
}
