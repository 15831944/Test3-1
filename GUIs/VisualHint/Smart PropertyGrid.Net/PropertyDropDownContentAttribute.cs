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
    /// Attaches to a specific property the type of control displayed in the listbox part of a
    /// combobox inplace control.
    /// </summary>
    /// <remarks>
    /// A combobox is displayed for a property when a <see cref="PropertyGrid.FeelList"/> or
    /// <see cref="PropertyGrid.FeelEditList"/> is used. But this feel is not enough to tell what to display in
    /// the popup listbox. Therefore you must also tell the framework the type of the control that must be
    /// displayed in the listbox. This type must implement <see cref="IDropDownContent"/>.
    /// <para>Some built-in listbox contents have already been defined. Enumerations, booleans and strings use
    /// a control class named <see cref="DropDownContentListBox"/>.
    /// </para>
    /// <para>To create your own relation between a property type and a listbox content, you can use
    /// <see cref="PropertyGrid.RegisterDropDownContent"/>. To attach a content to only one property you can
    /// use this attribute.</para>
    /// </remarks>
    /// <example>
    /// <code>
    /// // To setup a property:
    /// [PropertyDropDownContent(typeof(AlphaColorPicker))]
    /// public Color MyColor
    /// {
    ///     set { _myColor = value; }
    ///     get { return _myColor; }
    /// }
    /// 
    /// // To setup a child property:
    /// [PropertyDropDownContent("Color", typeof(AlphaColorPicker))]
    /// public Pen MyPen { get { return _pen; } set { _pen = value; } }
    /// </code>
    /// </example>
    [AttributeUsage(AttributeTargets.All, AllowMultiple = true)]
    public class PropertyDropDownContentAttribute : Attribute
    {
        private Type _controlType;

        /// <summary>
        /// Get the type of the control that is displayed in the popup listbox of a combobox for the property
        /// that owns this attribute or a child property whose name is given by <see cref="ChildPropertyName"/>.
        /// </summary>
        public Type ControlType
        {
            get { return _controlType; }
        }

        private string _childPropertyName = "";

        public string ChildPropertyName
        {
            get { return _childPropertyName; }
        }

        /// <summary>
        /// Returns the type of the control displayed in the popup listbox of a combobox for a child property
        /// of the parent property that owns this attribute.
        /// </summary>
        /// <param name="childPropertyName">The name of the child property.</param>
        /// <returns>The type of the control displayed in the popup listbox of a combobox for a child property
        /// of the parent property that owns this attribute.</returns>
        public Type GetDropDownContent(string childPropertyName)
        {
            if (childPropertyName.CompareTo(_childPropertyName) == 0)
                return _controlType;

            return null;
        }

        public override Object TypeId
        {
            get { return "PropertyDropDownContent" + _childPropertyName; }
        }

        /// <summary>
        /// Constructor used to associate a control type to the property that owns this attribute.
        /// </summary>
        /// <param name="controlType">Type of the control to associate.</param>
        public PropertyDropDownContentAttribute(Type controlType)
        {
            _controlType = controlType;
        }

        /// <summary>
        /// Constructor used to associate a control type to a child property of the property that owns this attribute.
        /// </summary>
        /// <param name="childPropertyName">The name of the child property.</param>
        /// <param name="controlType">Type of the control to associate.</param>
        public PropertyDropDownContentAttribute(string childPropertyName, Type controlType)
        {
            if (childPropertyName == null)
                throw new ArgumentException("The name of the child property can't be null");

            _childPropertyName = childPropertyName;
            _controlType = controlType;
        }

        private int _hashCode = 0;

        public override int GetHashCode()
        {
            if (_hashCode == 0)
            {
                _hashCode = _childPropertyName.GetHashCode();
                _hashCode ^= _controlType.GetHashCode();
            }

            return _hashCode;
        }

        public override bool Equals(object obj)
        {
            PropertyDropDownContentAttribute attribute = obj as PropertyDropDownContentAttribute;
            if (attribute == null)
                return false;

            if (_controlType != attribute._controlType)
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
