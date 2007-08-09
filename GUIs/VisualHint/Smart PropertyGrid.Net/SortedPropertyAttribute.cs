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
    /// When assigned to a property, this attribute allows to sort it among its sibling properties.
    /// </summary>
    public class SortedPropertyAttribute : Attribute
    {
        private int _sortIndex = 0;

        public int SortedIndex
        {
            get { return _sortIndex; }
        }

        public SortedPropertyAttribute(int sortIndex)
        {
            _sortIndex = sortIndex;
        }
    }
}
