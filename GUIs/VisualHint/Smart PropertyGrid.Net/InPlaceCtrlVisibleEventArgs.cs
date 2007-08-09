// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System;
using System.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Passed to the client application when it is notified that an inplace control has been made
    /// visible for the currently selected property.
    /// </summary>
    public class InPlaceCtrlVisibleEventArgs : PropertyEventArgs
    {
        private Control _inPlaceCtrl;

        public Control InPlaceCtrl
        {
            get { return _inPlaceCtrl; }
        }

        public InPlaceCtrlVisibleEventArgs(PropertyEnumerator propEnum, Control c) : base(propEnum)
        {
            _inPlaceCtrl = c;
        }
    }
}
