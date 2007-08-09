// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System.Windows.Forms;
using System.Security.Permissions;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    internal class InternalTrackBar : TrackBar
    {
        /// <summary>
        /// Because Control.OnLostFocus and Control.OnGotFocus doesn't provide a reference to the
        /// window that lost or gained the focus, this variable will contain this missing information.
        /// It is initialized in WndProc when processing WM_FILLFOCUS and WM_SETFOCUS messages.
        /// It is also embedded in a FocusEventArgs instance for event listeners.
        /// </summary>
        private Control _wmFocusMissingArg;

        private PropertyEnumerator _ownerPropertyEnum;

        public PropertyEnumerator OwnerPropertyEnumerator
        {
            set
            {
                _ownerPropertyEnum = value;

                PropertyValidatorMinMax validator = (_ownerPropertyEnum.Property.Value.Validator as PropertyValidatorMinMax);

                if (validator != null)
                {
                    Minimum = (int)validator.Min;
                    Maximum = (int)validator.Max;
                }
                else
                {
                    Minimum = 0;
                    Maximum = 100;
                }
            }
        }

        public InternalTrackBar()
        {
            AutoSize = false;
            TickStyle = TickStyle.None;
        }

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_SETFOCUS)
            {
                // The window that lose the focus is stored for future reference
                // in OnGotFocus. if the handle is in another application, the stored
                // value will be null.
                _wmFocusMissingArg = FromHandle(m.WParam);
            }
            else if (m.Msg == Win32Calls.WM_KILLFOCUS)
            {
                // The window that gained the focus is stored for future reference
                // in OnLostFocus. if the handle is in another application, the stored
                // value will be null.
                _wmFocusMissingArg = FromHandle(m.WParam);
            }
            else if (m.Msg == Win32Calls.WM_ERASEBKGND)
            {
                // If I don't intercept this message, the control flickers
                return;
            }

            base.WndProc(ref m);
        }
    }
}
