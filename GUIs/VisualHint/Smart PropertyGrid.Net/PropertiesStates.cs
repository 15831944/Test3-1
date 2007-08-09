// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.Collections;

namespace VisualHint.SmartPropertyGrid
{
    public class PropertiesStates
    {
        internal struct PropertyState
        {
            public bool expanded;
            public bool selected;

            public PropertyState(bool expanded, bool selected, int selectedVisibleIndex)
            {
                this.expanded = expanded;
                this.selected = selected;
            }
        }

        internal Hashtable _states = new Hashtable();
        internal int _selectedPropertyVisibleIndex;
        internal int _firstDisplayedPropertyHashcode;
        internal int _firstDisplayedLine;
    }
}
