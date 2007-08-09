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
    /// Passed to the client application when a user has finished editing a property value or when he is
    /// currently editing the value (in realtime mode).
    /// </summary>
    public class PropertyChangedEventArgs : PropertyEventArgs
    {
        private object _additionalValueKey;

        public object AdditionalValueKey
        {
            set { _additionalValueKey = value; }
            get { return _additionalValueKey; }
        }

        private int _selectedIndex;

        public int SelectedIndex
        {
            set { _selectedIndex = value; }
            get { return _selectedIndex; }
        }

        public PropertyChangedEventArgs(PropertyEnumerator propEnum, object additionalValueKey) : base(propEnum)
        {
            _additionalValueKey = additionalValueKey;
        }

        public PropertyChangedEventArgs(PropertyEnumerator propEnum) : base(propEnum)
        {
        }
    }
}
