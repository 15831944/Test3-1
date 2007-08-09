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
    /// Passed to the client application when it is notified that a new property has been selected.
    /// </summary>
    public class PropertySelectedEventArgs : PropertyEventArgs
    {
        private PropertyEnumerator _previousSelectedPropertyEnum;

        public PropertyEnumerator PreviousSelectedPropertyEnum
        {
            get { return _previousSelectedPropertyEnum; }
        }

        public PropertySelectedEventArgs(PropertyEnumerator propEnum, PropertyEnumerator previousSelectedPropEnum)
            : base(propEnum)
        {
            _previousSelectedPropertyEnum = previousSelectedPropEnum;
        }
    }
}
