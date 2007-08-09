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
    /// Passed to the client application when it is notified that a property has been enabled or disabled.
    /// </summary>
    public class PropertyEnabledEventArgs : PropertyEventArgs
    {
        private bool _directlyModified;

        public bool DirectlyModified
        {
            get { return _directlyModified; }
        }

        public PropertyEnabledEventArgs(PropertyEnumerator propEnum, bool direct) : base(propEnum)
        {
            _directlyModified = direct;
        }
    }
}
