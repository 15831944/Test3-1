// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.ComponentModel;
using System.Collections;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <exclude />
    public class PropertyComparerDefaultSort : IComparer
    {
        public int Compare(object x, object y)
        {
            PropertyDescriptor p1 = x as PropertyDescriptor;
            PropertyDescriptor p2 = y as PropertyDescriptor;

            if (p1.Category.CompareTo(p2.Category) < 0)
                return -1;
            else if (p1.Category.CompareTo(p2.Category) > 0)
                return 1;
            else
            {
                int comp = p1.DisplayName.CompareTo(p2.DisplayName);
                if (comp < 0)
                    return -1;
                else if (comp > 0)
                    return 1;
                else
                    return 0;
            }
        }
    }
}
