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
    /// Passed to the client application when it is notified that a property has been expanded or collapsed.
    /// </summary>
    public class PropertyExpandedEventArgs : PropertyEventArgs
    {
        private bool _expanded;

        public bool Expanded
        {
            get { return _expanded; }
        }

        public PropertyExpandedEventArgs(PropertyEnumerator propEnum, bool expanded) : base(propEnum)
        {
            _expanded = expanded;
        }
    }
}
