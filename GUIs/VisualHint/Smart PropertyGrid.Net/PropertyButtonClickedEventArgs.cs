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
    /// Passed to the client application when the button part of an inplace control has been clicked.
    /// </summary>
    public class PropertyButtonClickedEventArgs : PropertyEventArgs
    {
        private bool _propertyChanged = false;

        public bool PropertyChanged
        {
            set { _propertyChanged = value;  }
            get { return _propertyChanged; }
        }

        public PropertyButtonClickedEventArgs(PropertyEnumerator propEnum) : base(propEnum)
        {
        }
    }
}
