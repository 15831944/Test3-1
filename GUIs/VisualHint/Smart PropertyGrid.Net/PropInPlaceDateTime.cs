// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System;
using System.Windows.Forms;
using System.Drawing;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Security.Permissions;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when edition of a date or time is needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceDateTime : DateTimePicker, IInPlaceControl, IMouseHookClient, IKeyboardHookClient
    {
        private PropertyValue.ValueValidationResult _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

        private MouseHook _mouseHook;
        private KeyboardHook _keyboardHook;

        private IntPtr _closeByClickOnWindow;

        //private bool _calendarClosedByKey = false;

        private bool _justVisible = false;

        private DateTime _oldValue;

        private PropertyEnumerator _currentInvalidPropertyEnum;

        private PropertyEnumerator _ownerPropertyEnum;

        private IntPtr _hMonthCalendar;

        /// <summary>
        /// Because Control.OnLostFocus and Control.OnGotFocus doesn't provide a reference to the
        /// window that lost or gained the focus, this variable will contain this missing information.
        /// It is initialized in WndProc when processing WM_FILLFOCUS and WM_SETFOCUS messages.
        /// It is also embedded in a FocusEventArgs instance for event listeners.
        /// </summary>
        private IntPtr _wmFocusMissingArg;

        public PropInPlaceDateTime()
        {
            _mouseHook = new MouseHook(this, this);
            _keyboardHook = new KeyboardHook(this, this);

            ShowUpDown = true;
        }

        /// <summary>
        /// Links a Property to this inplace control, so that its value will be displayed inside.
        /// </summary>
        /// <value>An enumerator to the Property to link to this control.</value>
        public PropertyEnumerator OwnerPropertyEnumerator
        {
            set
            {
                if (!Visible)
                    _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

                _ownerPropertyEnum = (PropertyEnumerator)value.Clone();

                Value = (DateTime)_ownerPropertyEnum.Property.Value.GetValue();

                Enabled = !ReadOnly;

                // Store the current value
                _oldValue = (DateTime)_ownerPropertyEnum.Property.Value.GetValue();
            }

            get { return _ownerPropertyEnum; }
        }

        public bool ReadOnly
        {
            get { return _ownerPropertyEnum.Property.Value.IsReadOnly(_ownerPropertyEnum); }
        }

        protected override void OnHandleCreated(EventArgs e)
        {
            Win32Calls.ModifyStyleEx(Handle, Win32Calls.WS_EX_CLIENTEDGE, 0, Win32Calls.SWP_FRAMECHANGED);

            Cursor = Cursors.Arrow;

            base.OnHandleCreated(e);
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_NCCALCSIZE)
            {
                Win32Calls.NCCALCSIZE_PARAMS csp;

                csp = (Win32Calls.NCCALCSIZE_PARAMS)Marshal.PtrToStructure(m.LParam, typeof(Win32Calls.NCCALCSIZE_PARAMS));

            	csp.rgrc0.Left += _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin - 2;
                Marshal.StructureToPtr(csp, m.LParam, false);
            }
            else if (m.Msg == Win32Calls.WM_NCPAINT)
            {
                IntPtr hDC = Win32Calls.GetWindowDC(m.HWnd);
                Graphics graphics = Graphics.FromHdc(hDC);

                Rectangle rect = ClientRectangle;
                rect.Width = _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin - 2;
                _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(graphics,
                    rect, _ownerPropertyEnum);

                Win32Calls.ReleaseDC(m.HWnd, hDC);

                return;
            }
            else if (m.Msg == Win32Calls.WM_ERASEBKGND)
            {
                if (_justVisible == false)
                {
                    _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(
                        Graphics.FromHdc(m.WParam), ClientRectangle, _ownerPropertyEnum);
                }

                m.Result = (IntPtr)1;
                return;
            }
            else if (m.Msg == Win32Calls.WM_PAINT)
            {
                if (_justVisible)
                {
                    Graphics graphics = CreateGraphics();
                    _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(
                        graphics, ClientRectangle, _ownerPropertyEnum);
                    graphics.Dispose();

                    _justVisible = false;
                }
            }
            if (m.Msg == Win32Calls.WM_SETFOCUS)
            {
                // The window that lose the focus is stored for future reference
                // in OnGotFocus. if the handle is in another application, the stored
                // value will be null.
                _wmFocusMissingArg = m.WParam;
            }
            else if (m.Msg == Win32Calls.WM_KILLFOCUS)
            {
                // The window that gained the focus is stored for future reference
                // in OnLostFocus. if the handle is in another application, the stored
                // value will be null.
                _wmFocusMissingArg = m.WParam;
            }

            base.WndProc(ref m);
        }

        protected override void OnVisibleChanged(EventArgs e)
        {
            if (Visible)
            {
                PropertyDateTimeLook look = (_ownerPropertyEnum.Property.Value.Look as PropertyDateTimeLook);
                if (look != null)
                {
                    CustomFormat = look.CustomFormat;
                    Format = look.Format;
                }
            }
            else
                _justVisible = true;

            base.OnVisibleChanged(e);
        }
        
        protected override void OnTextChanged(EventArgs e)
        {
            if (_realtimeChange || (_closeByClickOnWindow == _hMonthCalendar))
                CommitChanges(false);
        }

#if _DOTNET2
        [UIPermission(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
#endif
		protected override bool ProcessDialogKey(Keys keyData)
        {
            Keys key = (keyData & Keys.KeyCode);

            if (key == Keys.Return)
            {
                PropertyValue.ValueValidationResult changed = CommitChanges(true);

                if (PropertyValue.IsErrorCode(changed) &&
                    (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                {
//                    SelectAll();
                    return true;
                }
            }
            else if (key == Keys.Tab)
            {
                PropertyValue.ValueValidationResult changed = CommitChanges(true);

                if (PropertyValue.IsErrorCode(changed) &&
                    (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                {
//                    SelectAll();
                    return true;
                }
            }
            else if (key == Keys.Escape)
            {
                // Restore the old value in the control
                Value = _oldValue;
                CommitChanges(true);
            }

            if (Focused)
                return base.ProcessDialogKey(keyData);
            else
                return true;
        }

        public void OnF4()
        {
            Focus();

            bool altIsPressed = ((Win32Calls.GetKeyState(Win32Calls.VK_MENU) & 0x8000) == 1);
            if (!altIsPressed)
                Win32Calls.keybd_event(Win32Calls.VK_MENU, 0, 0, UIntPtr.Zero);

            Win32Calls.keybd_event(Win32Calls.VK_DOWN, 0, 0, UIntPtr.Zero);
            Win32Calls.keybd_event(Win32Calls.VK_DOWN, 0, 0x0002, UIntPtr.Zero);

            if (!altIsPressed)
                Win32Calls.keybd_event(Win32Calls.VK_MENU, 0, 0x0002, UIntPtr.Zero);
        }

        public bool OnParentKeyDown(KeyEventArgs e)
        {
            return false;
        }

        private bool _realtimeChange = false;

        public bool RealtimeChange
        {
            set { _realtimeChange = value;  }
            get { return _realtimeChange; }
        }

        public virtual void RepositionChildren()
        {
        }

        public bool OnForbidClickThrough(Point pt)
        {
            Rectangle rect = ClientRectangle;
            rect.X = rect.Right - SystemInformation.VerticalScrollBarWidth;
            rect.Width = SystemInformation.VerticalScrollBarWidth;
            return rect.Contains(pt);
        }

        protected override void OnDropDown(EventArgs eventargs)
        {
            _mouseHook.HookMessages = true;
            _keyboardHook.HookMessages = true;

            _ownerPropertyEnum.Property.InPlaceCtrlInAction = this;

            _hMonthCalendar = (IntPtr)Win32Calls.SendMessage(Handle, 0x1008 /*DTM_GETMONTHCAL*/, 0, 0);
            Win32Calls.SetWindowLong(_hMonthCalendar, -8, (int)Handle);
            base.OnDropDown(eventargs);
        }

        protected override void OnCloseUp(EventArgs eventargs)
        {
            _mouseHook.HookMessages = false;
            _keyboardHook.HookMessages = false;

            _hMonthCalendar = IntPtr.Zero;

            Control control = FromHandle(_closeByClickOnWindow);
            if (control != null)
            {
                Point pt = MousePosition;
                pt = control.PointToClient(pt);
                Win32Calls.PostMessage(_closeByClickOnWindow, Win32Calls.WM_LBUTTONDOWN, 0,
                    Win32Calls.MakeLong(pt.X, pt.Y));
            }
            else if (_monthCalCloseKey != Keys.None)
            {
                if (_monthCalCloseKey == Keys.Escape)
                    Value = _oldValue;

                CommitChanges(true);
                Parent.Focus();
            }

            _monthCalCloseKey = Keys.None;

            _ownerPropertyEnum.Property.InPlaceCtrlInAction = null;
            _ownerPropertyEnum.Property.ParentGrid.OnInPlaceCtrlFinishedEdition();

            //_calendarClosedByKey = false;

            RecreateHandle();  // To fix the native DateTimePicker focus bug

            base.OnCloseUp(eventargs);
        }

        protected override void OnLostFocus(EventArgs e)
        {
/*            if (_hMonthCalendar != _wmFocusMissingArg)
            {
                CommitChanges();

                base.OnLostFocus(e);
            }*/
            base.OnLostFocus(e);
        }

        [CLSCompliant(false)]
        public bool OnMouseHooked(IntPtr hWnd, uint message, Point pt, uint hitTest)
        {
            if (message == Win32Calls.WM_LBUTTONDOWN)
                _closeByClickOnWindow = hWnd;

            return false;
        }

        private Keys _monthCalCloseKey = Keys.None;

        public bool OnKeyboardHooked(int message, KeyEventArgs args)
        {
            // The month calendar doesn't close itself naturally on F4 or Alt+Down so we have to help...

            if (message == Win32Calls.WM_KEYDOWN)
            {
                if ((args.KeyCode == Keys.F4) || ((args.KeyCode == Keys.Down) && (args.Modifiers == Keys.Alt)))
                {
                    _monthCalCloseKey = Keys.Return;
                    Win32Calls.keybd_event(0xA4, 0, Win32Calls.VK_MENU, UIntPtr.Zero);
                    Win32Calls.keybd_event(Win32Calls.VK_RETURN, 0, 0, UIntPtr.Zero);
                    Win32Calls.keybd_event(Win32Calls.VK_RETURN, 0, 0x0002, UIntPtr.Zero);
                }
                else if (args.KeyCode == Keys.Escape)
                    _monthCalCloseKey = Keys.Escape;
                else if (args.KeyCode == Keys.Return)
                    _monthCalCloseKey = Keys.Return;
            }

            return false;
        }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            return true;
        }

        public virtual bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force)
        {
            return false;
        }

        public TextBox TextBox
        {
            get { return null; }
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;

            if (ReadOnly)
                return changed;

            PropertyValue value = _ownerPropertyEnum.Property.Value;

            // Store the previous value
            DateTime oldValue = (DateTime)value.GetValue();

            // Assign the new value based on the edit content
            if (Value != oldValue)
            {
                // Freeze painting because updating the property may also update other properties in the grid.
                _ownerPropertyEnum.Property.ParentGrid.BeginUpdate();

                _ownerPropertyEnum.Property.Value.PreviousValue = oldValue;

                PropertyEnumerator invalidPropertyEnum;
                changed = value.SetValueFromInPlaceCtrl(Value, out invalidPropertyEnum);

                if (PropertyValue.IsErrorCode(changed))
                    _currentInvalidPropertyEnum = invalidPropertyEnum;

                // Notify the grid if the value is invalid
                if (changed == PropertyValue.ValueValidationResult.ValidatorFailed)
                {
                    OwnerPropertyEnumerator.Property.ParentGrid.NotifyValueValidation(
                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                        Value, PropertyValue.ValueValidationResult.ValidatorFailed));
                }
                else if (changed == PropertyValue.ValueValidationResult.ExceptionByClient)
                {
                    OwnerPropertyEnumerator.Property.ParentGrid.NotifyValueValidation(
                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                        Value, PropertyValue.ValueValidationResult.ExceptionByClient, value.LastExceptionByClient));
                }

                // We have to update the edit control in case the value was refused or if its string representation
                // is different from what was typed (changes done by a CultureInfo for example)
                if ((_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                    PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore) ||
                    (changed == PropertyValue.ValueValidationResult.Validated))
                {
                    Value = (DateTime)value.GetValue();
                    if (final)
                        _oldValue = Value;
                }

                // If value is invalid, previous value is restored if ValueNotValidBehaviorMode is set accordingly
                if (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore)
                {
                    if (changed != PropertyValue.ValueValidationResult.Validated)
                    {
                        changed = PropertyValue.ValueValidationResult.PreviousValueRestored;
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                            Value, changed));
                    }
                }

                // Notify the properties control if the value has really changed
                if (changed == PropertyValue.ValueValidationResult.Validated)
                {
                    if (PropertyValue.IsErrorCode(_currentValueValidationResult))
                    {
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                            Value, changed));
                    }

                    _ownerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(new PropertyChangedEventArgs(_ownerPropertyEnum));
                }

                _currentValueValidationResult = changed;

                // Defreeze painting
                _ownerPropertyEnum.Property.ParentGrid.EndUpdate();
            }
            else
            {
                // Even if the value did not change, we ensure the text displayed is correct. A CultureInfo could
                // be involved for example that would change a modified text into the old value.
                Text = value.DisplayString;

                if (PropertyValue.IsErrorCode(_currentValueValidationResult))
                {
                    _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;
                    _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                        Value, _currentValueValidationResult));
                }
            }

            return changed;
        }

        public Control GetDropDownContainer()
        {
            return null;
        }
    }
}
