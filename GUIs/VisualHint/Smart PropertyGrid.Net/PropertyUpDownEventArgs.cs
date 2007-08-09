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
    /// Passed to the client application when the UpDown inplace control needs to know the previous or next
    /// value to display.
    /// </summary>
    public class PropertyUpDownEventArgs : PropertyEventArgs
    {
        /// <summary>
        /// Defines the button that has been clicked in an updown control.
        /// </summary>
        public enum UpDownButtons
        {
            Up,
            Down
        }

        private UpDownButtons _buttonPressed;

        public UpDownButtons ButtonPressed
        {
            get { return _buttonPressed; }
        }

        public PropertyUpDownEventArgs(PropertyEnumerator propEnum, UpDownButtons buttonPressed) : base(propEnum)
        {
            _buttonPressed = buttonPressed;
        }

        private string _value;

        public string Value
        {
            set { _value = value; }
            get { return _value; }
        }
    }
}
