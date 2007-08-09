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
    /// Passed to the client application to dynamically gather displayed values for a property that use
    /// a combobox or an updown control.
    /// </summary>
    public class DisplayedValuesNeededEventArgs : PropertyEventArgs
    {
        private string[] _displayedValues = new string[0];

        public string[] DisplayedValues
        {
            get { return _displayedValues; }
            set { _displayedValues = value; }
        }

        public DisplayedValuesNeededEventArgs(PropertyEnumerator propEnum) : base(propEnum)
        {
        }
    }
}
