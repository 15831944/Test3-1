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
    /// <exclude />
    public class FocusEventArgs : EventArgs
    {
        private Control _focusControl;

        public Control FocusControl
        {
            get { return _focusControl; }
        }

        private Keys _leaveKey;

        public Keys LeaveKey
        {
            get { return _leaveKey; }
        }

        public FocusEventArgs(Control focusControl, Keys leaveKey)
        {
            _focusControl = focusControl;
            _leaveKey = leaveKey;
        }
    }
}
