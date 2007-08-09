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
    internal class DropDownContentClosedEventArgs : EventArgs
    {
        private bool _toBeValidated = false;

        public bool ToBeValidated
        {
            get { return _toBeValidated; }
        }

        private bool _reasonLostFocus = false;

        public bool ReasonLostFocus
        {
            get { return _reasonLostFocus; }
        }

        public DropDownContentClosedEventArgs(bool toBeValidated)
        {
            _toBeValidated = toBeValidated;
        }

        private Control _wndTakingFocus = null;

        public Control WndTakingFocus
        {
            get { return _wndTakingFocus; }
        }

        public DropDownContentClosedEventArgs(bool toBeValidated, Control wndTakingFocus)
        {
            _toBeValidated = toBeValidated;
            _wndTakingFocus = wndTakingFocus;
            _reasonLostFocus = true;
        }
    }
}
