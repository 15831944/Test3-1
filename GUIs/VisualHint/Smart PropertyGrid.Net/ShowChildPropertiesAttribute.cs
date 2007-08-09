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
    /// When this attribute is assigned to a property, it is possible to control whether all child properties
    /// are created or not.
    /// </summary>
    /// <remarks>
    /// If the underlying value's TypeConverter returns false for GetPropertiesSupported, this attribute is
    /// necessary if your intent is to show all child properties. This is the case for a Pen.
    /// If GetPropertiesSupported returns true, this attribute is necessary if your intent is to hide all child properties. This
    /// is the case for a Rectangle or a Font.
    /// </remarks>
    public class ShowChildPropertiesAttribute : Attribute
    {
        private bool _show;

        public bool ShowChildProperties
        {
            get { return _show; }
        }

        public ShowChildPropertiesAttribute(bool show)
        {
            _show = show;
        }
    }
}
