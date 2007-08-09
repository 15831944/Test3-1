// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.ComponentModel;
using System.Security.Permissions;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// When assigned to a property, this attribute allows to sort its parent category by index.
    /// </summary>
    [AttributeUsage(AttributeTargets.All)]
    public class SortedCategoryAttribute : CategoryAttribute
    {
        private int _sortedIndex;

        public int SortedIndex
        {
            get { return _sortedIndex; }
        }

        public SortedCategoryAttribute(string category, int sortedIndex)
            : base(category)
        {
            _sortedIndex = sortedIndex;
        }
    }
}
