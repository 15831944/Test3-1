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
    /// Passed to the client application when it is notified that a property has been created during a call to
    /// <see cref="PropertyGrid.SelectedObject"/> and that it can be filtered out with all its descendants.
    /// </summary>
    /// <remarks>Read the description of the <see cref="PropertyGrid.OnPropertyPostFilterOut"/> method for more explanations.</remarks>
    public class PropertyPostFilterOutEventArgs : PropertyEventArgs
    {
        private bool _filterOut = false;

        /// <summary>
        /// Set this property to true to filter out the property and all its descendants.
        /// </summary>
        public bool FilterOut
        {
            set { _filterOut = value; }
        }

        internal bool GetFilterOutInternal()
        {
            return _filterOut;
        }

        internal PropertyPostFilterOutEventArgs(PropertyEnumerator propEnum) : base(propEnum)
        {
        }
    }
}
