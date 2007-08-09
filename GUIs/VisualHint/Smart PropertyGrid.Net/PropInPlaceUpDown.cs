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
using System.Drawing;
using System.ComponentModel;
using System.Security.Permissions;
using System.Globalization;
using Skybound.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when an updown control (with textbox or not) is needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceUpDown : Control, IInPlaceControl
    {
        // Fields
        private ButtonID _captured;
        private ButtonID _mouseOver;
        private ButtonID _pushed = ButtonID.None;
        private Timer _timer;
        private int _timerInterval;

        internal enum ButtonID
        {
            // Fields
            None = 0,
            Up = 1,
            Down = 2
        }

        /// <summary>
        /// Because Control.OnLostFocus and Control.OnGotFocus doesn't provide a reference to the
        /// window that lost or gained the focus, this variable will contain this missing information.
        /// It is initialized in WndProc when processing WM_FILLFOCUS and WM_SETFOCUS messages.
        /// It is also embedded in a FocusEventArgs instance for event listeners.
        /// </summary>
        private Control _wmFocusMissingArg;

        private PropInPlaceTextbox _edit = null;

        private string _initialValue;

        private PropertyValue.ValueValidationResult _currentValueValidationResult;

        private PropertyEnumerator _currentInvalidPropertyEnum;

        private PropertyEnumerator _ownerPropertyEnum;

        public PropertyEnumerator OwnerPropertyEnumerator
        {
            set
            {
                if (!Visible)
                    _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

                _ownerPropertyEnum = (PropertyEnumerator)value.Clone();

                Text = _ownerPropertyEnum.Property.Value.DisplayString;

                if (_edit != null)
                    _edit.OwnerPropertyEnumerator = value;

                _initialValue = Text;
            }

            get { return _ownerPropertyEnum; }
        }

        public bool ReadOnly
        {
            get { return _ownerPropertyEnum.Property.Value.IsReadOnly(_ownerPropertyEnum); }
        }

        public override string Text
        {
            set
            {
                if (_edit == null)
                {
                    base.Text = value;
                    Invalidate();
                }
                else
                    _edit.Text = value;
            }

            get
            {
                if (_edit == null)
                    return base.Text;
                else
                    return _edit.Text;
            }
        }

        private Decimal _increment = 1.0m;

        public Decimal Increment
        {
            get { return _increment; }
            set { _increment = value; }
        }

        private bool _realtimeChange = true;

        public bool RealtimeChange
        {
            set { _realtimeChange = value; }
            get { return _realtimeChange; }
        }

        public PropInPlaceUpDown(bool editable)
        {
            if (editable)
            {
                _edit = new PropInPlaceTextbox(false);
                _edit.Parent = this;
                _edit.TabStop = false;
            }

            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque | ControlStyles.ResizeRedraw, true);

            Cursor = Cursors.Arrow;
        }

        public override Font Font
        {
            get { return base.Font; }
            set
            {
                base.Font = value;
                if (_edit != null)
                    _edit.Font = value;
            }
        }

        protected override void OnMove(EventArgs e)
        {
            base.OnMove(e);
            OnSizeChanged(e);
        }

        public virtual void RepositionChildren()
        {
            if (_edit != null)
            {
                Rectangle clientRect = ClientRectangle;
                if (clientRect.Width > 0)
                {
                    // Get the margins of the editbox
                    int rawMargins = Win32Calls.LoWord(Win32Calls.SendMessage(_edit.Handle, Win32Calls.EM_GETMARGINS, 0, 0));
                    int margins = Win32Calls.LoWord(rawMargins) + Win32Calls.HiWord(rawMargins);

                    int leftPos = (_ownerPropertyEnum.Property.Value.EditboxLeftMargin == -1 ? 0 :
                        _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin - _ownerPropertyEnum.Property.Value.EditboxLeftMargin);
                    int width = (ReadOnly ? clientRect.Width - leftPos : clientRect.Width - SystemInformation.VerticalScrollBarWidth - leftPos);
                    Rectangle editRect = new Rectangle(leftPos, 0, width, clientRect.Height);
                    editRect.Y++;
                    editRect.Height--;

                    // The width of the editbox can't be smaller than the margins or the margins would be void and the text misplaced
                    if (editRect.Width <= margins)
                    {
                        if (_edit.Focused)
                            Parent.Focus();

                        _edit.Visible = false;
                    }
                    else
                    {
                        _edit.Visible = true;

                        Win32Calls.TEXTMETRIC tm = new Win32Calls.TEXTMETRIC();
                        Graphics graphics = CreateGraphics();
                        Win32Calls.GetTextMetrics(graphics, Font, ref tm);
                        graphics.Dispose();
                        int extraHeight = editRect.Height - tm.tmHeight;
                        editRect.Y += extraHeight / 2;
                        editRect.Height -= extraHeight / 2;

                        _edit.Invalidate();
                        _edit.Bounds = editRect;
                    }
                }
            }
            //            mEdit.Location = new Point(0, 0);
            //          mEdit.Size = new Size(ClientRectangle.Width - SystemInformation.VerticalScrollBarWidth, ClientRectangle.Height);
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            RepositionChildren();
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if ((_edit != null) && (Focused == false))
                Win32Calls.PostMessage(_edit.Handle, Win32Calls.WM_CHAR, e.KeyChar, 0);
            else if (_edit == null)
            {
                // When typing a letter, we try to find the next string after the selected one
                // that begins by this letter

                string[] displayedStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                char[] chArray = new char[1] { e.KeyChar };
                string letter = new string(chArray);

                for (int i = 0; i < displayedStrings.Length; i++)
                {
                    if (Text.Equals(displayedStrings[i]))
                    {
                        for (int j = 0; j < displayedStrings.Length; j++)
                        {
                            string currentStr = displayedStrings[((i + j) + 1) % displayedStrings.Length];
                            if (string.Compare(currentStr.Substring(0, 1), letter, true, CultureInfo.InvariantCulture) == 0)
                            {
                                Text = currentStr;
                                if (RealtimeChange)
                                    CommitChanges(false);
                                break;
                            }
                        }

                        break;
                    }
                }
            }
        }

        protected override void OnGotFocus(EventArgs e)
        {
            if (_edit != null)
            {
                if (_wmFocusMissingArg != _edit)
                    _edit.Focus();
                else
                    Parent.Focus();
            }
            else
                Invalidate();

            base.OnGotFocus(e);
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
            else if (m.Msg == 0x20a) // MouseWheel
            {
                Point pt = new Point(Win32Calls.LoWord((int)m.LParam), Win32Calls.HiWord((int)m.LParam));
                pt = PointToClient(pt);
                OnMouseWheel(new MouseEventArgs(MouseButtons.None, 0, pt.X, pt.Y, Win32Calls.HiWord((int)m.WParam)));
                m.Result = IntPtr.Zero;
                return;
            }
#if !_DOTNET2
            else if (m.Msg == 0x215)    // OnMouseCaptureChanged in .Net 2.0
            {
                if (Capture == false)
                    EndButtonPress();
                return;
            }
#endif

            base.WndProc(ref m);
        }

        public override Color BackColor
        {
            get { return base.BackColor; }
            set
            {
                base.BackColor = value;

                if (_edit != null)
                    _edit.BackColor = value;
            }
        }

        public override Color ForeColor
        {
            get { return base.ForeColor; }
            set
            {
                base.ForeColor = value;

                if (_edit != null)
                    _edit.ForeColor = value;
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            // Draw the background

            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(e.Graphics,
                ClientRectangle, _ownerPropertyEnum);

            // Draw the arrows

            if (!ReadOnly)
            {
                int halfHeight = (int)Math.Round(ClientSize.Height / 2.0);
                int buttonWidth = SystemInformation.VerticalScrollBarWidth;
                int leftSide = ClientSize.Width - buttonWidth;

                if (ThemeRenderer.Enabled)
                {
                    int state = (_pushed == ButtonID.Up ? ThemeSpin.Pressed :
                        (_mouseOver == ButtonID.Up ? ThemeSpin.Hot : ThemeSpin.Normal));
					ThemeSpin.Up.Draw(e.Graphics, state, new Rectangle(leftSide, 0, buttonWidth, halfHeight));

                    state = (_pushed == ButtonID.Down ? ThemeSpin.Pressed :
                        (_mouseOver == ButtonID.Down ? ThemeSpin.Hot : ThemeSpin.Normal));
					ThemeSpin.Down.Draw(e.Graphics, state, new Rectangle(leftSide, halfHeight, buttonWidth, halfHeight));
                }
                else
                {
                    ControlPaint.DrawScrollButton(e.Graphics, new Rectangle(leftSide, 0, buttonWidth, halfHeight), ScrollButton.Up, (_pushed == ButtonID.Up) ? ButtonState.Pushed : ButtonState.Normal);
                    ControlPaint.DrawScrollButton(e.Graphics, new Rectangle(leftSide, halfHeight, buttonWidth, halfHeight), ScrollButton.Down, (_pushed == ButtonID.Down) ? ButtonState.Pushed : ButtonState.Normal);
                }
            }

            // Draw the text

            if (_edit == null)
            {
	            // Draw the value
	            Rectangle valueRect = ClientRectangle;
                if (!ReadOnly)
                    valueRect.Width -= SystemInformation.VerticalScrollBarWidth + 1;

                if (Focused)
                {
                    Rectangle fillRect = valueRect;
                    fillRect.X++;
                    fillRect.Width--;
                    fillRect.Height--;
                    e.Graphics.FillRectangle(SystemBrushes.FromSystemColor(SystemColors.Highlight), fillRect);
                    ControlPaint.DrawFocusRectangle(e.Graphics, fillRect);
                }

                Color valueColor;
                if (_ownerPropertyEnum.Property.Enabled == false)
                    valueColor = SystemColors.GrayText;
                else
                {
                    if (Focused)
                        valueColor = SystemColors.HighlightText;
                    else
                        valueColor = _ownerPropertyEnum.Property.Value.ForeColor;
                }

                _ownerPropertyEnum.Property.Value.DrawValue(e.Graphics, valueRect, valueColor, _ownerPropertyEnum, Text);
            }

            base.OnPaint(e);
        }

        public bool OnForbidClickThrough(Point pt)
        {
            return (pt.X > ClientRectangle.Right - SystemInformation.VerticalScrollBarWidth);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (!ReadOnly)
            {
                if (e.X > ClientRectangle.Width - SystemInformation.VerticalScrollBarWidth)
                {
                    // Maybe the user typed something in the editbox that has not been validated yet
//                    if (_realtimeChange && (_edit != null))
  //                      _edit.CommitChanges();

                    if (e.Button == MouseButtons.Left)
                    {
                        BeginButtonPress(e);
                    }
                    base.OnMouseDown(e);

                    if (_edit != null)
                    {
                        _edit.Focus();
                        _edit.SelectAll();
                    }
                }
                else
                {
                    if (e.Clicks == 2)
                        ScrollValueOnDoubleClick(!(ModifierKeys == Keys.Shift),
                            ScrollValueOnDoubleClickSource.FromInPlaceCtrl, false);

                    base.OnMouseDown(e);
                }

                if (_edit == null)
                    Focus();
            }
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            _mouseOver = ButtonID.None;
            Invalidate();

            base.OnMouseLeave(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (Capture)
            {
                Rectangle rectangle1 = ClientRectangle;
                rectangle1.Height /= 2;
                if (_captured == ButtonID.Down)
                {
                    rectangle1.Y += rectangle1.Height;
                }
                if (rectangle1.Contains(e.X, e.Y))
                {
                    if (_pushed != _captured)
                    {
                        StartTimer();
                        _pushed = _captured;
                        Invalidate();
                    }
                }
                else if (_pushed != ButtonID.None)
                {
                    StopTimer();
                    _pushed = ButtonID.None;
                    Invalidate();
                }
            }

            int halfHeight = (int)Math.Round(ClientSize.Height / 2.0);
            int buttonWidth = SystemInformation.VerticalScrollBarWidth;
            int leftSide = ClientSize.Width - buttonWidth;

            Rectangle rectangle2 = new Rectangle(leftSide, 0, buttonWidth, halfHeight);
            Rectangle rectangle3 = new Rectangle(leftSide, halfHeight, buttonWidth, halfHeight);
            if (rectangle2.Contains(e.X, e.Y))
            {
                _mouseOver = ButtonID.Up;
                Invalidate();
            }
            else if (rectangle3.Contains(e.X, e.Y))
            {
                _mouseOver = ButtonID.Down;
                Invalidate();
            }

            base.OnMouseMove(e);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            if (e.X > ClientRectangle.Width - SystemInformation.VerticalScrollBarWidth)
            {
                if (e.Button == MouseButtons.Left)
                    EndButtonPress();
            }

            base.OnMouseUp(e);
        }

        private void BeginButtonPress(MouseEventArgs e)
        {
            if (e.Y < Size.Height / 2)
                _captured = _pushed = ButtonID.Up;
            else
                _captured = _pushed = ButtonID.Down;

            Invalidate();

            Capture = true;

            HandleUpDownButton(_pushed, false);

            StartTimer();
        }

        private void EndButtonPress()
        {
            _pushed = ButtonID.None;
            _captured = ButtonID.None;
            StopTimer();
            Capture = false;
            Invalidate();
        }

        protected void StartTimer()
        {
            if (_timer == null)
            {
                _timer = new Timer();
                _timer.Tick += new EventHandler(TimerHandler);
            }
            _timerInterval = 500;
            _timer.Interval = _timerInterval;
            _timer.Start();
        }

        protected void StopTimer()
        {
            if (_timer != null)
            {
                _timer.Stop();
                _timer.Dispose();
                _timer = null;
            }
        }

#if _DOTNET2
        protected override void OnMouseCaptureChanged(EventArgs e)
        {
            if (Capture == false)
                EndButtonPress();
        }
#endif
        private void TimerHandler(object source, EventArgs args)
        {
            if (Capture)
            {
                HandleUpDownButton(_pushed, false);

                _timerInterval *= 7;
                _timerInterval /= 10;
                if (_timerInterval < 1)
                    _timerInterval = 1;
                _timer.Interval = _timerInterval;
            }
        }

        private void HandleUpDownButton(ButtonID buttonId, bool rotate)
        {
            string strValue = Text;

            PropertyUpDownEventArgs args = new PropertyUpDownEventArgs(
                _ownerPropertyEnum, (buttonId == ButtonID.Up ? PropertyUpDownEventArgs.UpDownButtons.Up :
                PropertyUpDownEventArgs.UpDownButtons.Down));
            args.Value = Text;
            string oldStr = Text;

            _ownerPropertyEnum.Property.ParentGrid.OnPropertyUpDown(args);

            if (Text != args.Value)
                strValue = args.Value;
            else
            {
                string[] displayedStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();

                if (displayedStrings.Length > 0)
                {
                    strValue = Text;

                    if (_ownerPropertyEnum.Property.Value.HasMultipleValues)
                    {
                        int index = -1;
                        if (buttonId == ButtonID.Down)
                            index = displayedStrings.Length - 1;
                        else if (buttonId == ButtonID.Up)
                            index = 0;

                        if (index != -1)
                            strValue = displayedStrings[index];
                    }
                    else
                    {
                        for (int i = 0; i < displayedStrings.Length; i++)
                        {
                            if (displayedStrings[i] == strValue)
                            {
                                int index = -1;
                                if ((buttonId == ButtonID.Down) && (i > 0))
                                    index = i - 1;
                                else if ((buttonId == ButtonID.Down) && rotate)
                                    index = displayedStrings.Length - 1;
                                else if ((buttonId == ButtonID.Up) && (i < displayedStrings.Length - 1))
                                    index = i + 1;
                                else if ((buttonId == ButtonID.Up) && rotate)
                                    index = 0;

                                if (index != -1)
                                    strValue = displayedStrings[index];

                                break;
                            }
                        }
                    }
                }
                else if (_ownerPropertyEnum.Property.Value.UnderlyingType != typeof(string))
                {
                    TypeConverter tc = _ownerPropertyEnum.Property.Value.TypeConverter;
                    PropertyTypeDescriptorContext context = _ownerPropertyEnum.Property.Value.GetTypeDescriptorContext(_ownerPropertyEnum);

                    decimal value;
                    object originalValue;
                    try
                    {
                        if (_ownerPropertyEnum.Property.Value.HasMultipleValues)
                        {
                            DefaultValueAttribute attr = (DefaultValueAttribute)_ownerPropertyEnum.Property.Value.GetAttribute(typeof(DefaultValueAttribute));
                            if (attr != null)
                                originalValue = attr.Value;
                            else
                                originalValue = 0;
                            value = Convert.ToDecimal(originalValue);
                        }
                        else
                        {
                            originalValue = tc.ConvertFromString(context,
                                _ownerPropertyEnum.Property.Value.CultureInfo, Text);
                            value = Convert.ToDecimal(originalValue);
                        }
                    }
                    catch (Exception e)
                    {
                        _currentInvalidPropertyEnum = OwnerPropertyEnumerator;
                        _currentValueValidationResult = PropertyValue.ValueValidationResult.TypeConverterFailed;
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum, Text,
                                PropertyValue.ValueValidationResult.TypeConverterFailed, e));
                        return;
                    }

                    if (buttonId == ButtonID.Up)
                    {
                        try
                        {
                            value += Increment;
                        }
                        catch (OverflowException)
                        {
                        }
                    }
                    else if (buttonId == ButtonID.Down)
                    {
                        try
                        {
                            value -= Increment;
                        }
                        catch (OverflowException)
                        {
                        }
                    }

                    CultureInfo culture = _ownerPropertyEnum.Property.Value.CultureInfo;
                    strValue = tc.ConvertToString(context, culture,
                        Convert.ChangeType(value, originalValue.GetType(), culture));
                    if (strValue == null)
                        strValue = "";
                }
            }

            if (Text != strValue)
            {
                _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                PropertyValidatorBase validator = _ownerPropertyEnum.Property.Value.Validator;

                // Check current value
                try
                {
                    object valueToValidate = _ownerPropertyEnum.Property.Value.GetValueToValidate(Text);
                    if (validator != null)
                    {
                        if (!validator.Check(_ownerPropertyEnum.Property.Value.GetValueToValidate(Text), false))
                        {
                            if (_edit != null)
                                _edit.SelectAll();

                            _currentInvalidPropertyEnum = OwnerPropertyEnumerator;
                            _currentValueValidationResult = PropertyValue.ValueValidationResult.ValidatorFailed;
                            _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                                new ValueValidationEventArgs(OwnerPropertyEnumerator, OwnerPropertyEnumerator,
                                Text, _currentValueValidationResult));
                            return;
                        }
                    }
                }
                catch (Exception e)
                {
                    if (_edit != null)
                        _edit.SelectAll();

                    _currentInvalidPropertyEnum = OwnerPropertyEnumerator;
                    _currentValueValidationResult = PropertyValue.ValueValidationResult.TypeConverterFailed;
                    _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum, Text,
                            PropertyValue.ValueValidationResult.TypeConverterFailed, e));
                    return;
                }

                if (validator != null)
                {
                    if (!validator.Check(_ownerPropertyEnum.Property.Value.GetValueToValidate(strValue), false))
                        return;
                }

                if (_realtimeChange)
                    CommitChanges(strValue, false);
                else
                    Text = strValue;
            }

            if ((_edit != null) && _edit.Focused)
                _edit.SelectAll();
            else
                Invalidate();
        }

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override bool ProcessKeyPreview(ref Message m)
        {
            if (!ReadOnly)
            {
                if (m.Msg == Win32Calls.WM_KEYDOWN)
                {
                    if (((Keys)(int)m.WParam == Keys.Up) || ((Keys)(int)m.WParam == Keys.Down))
                    {
                        if ((Keys)(int)m.WParam == Keys.Up)
                            ProcessArrowKey(ButtonID.Up);
                        else if ((Keys)(int)m.WParam == Keys.Down)
                            ProcessArrowKey(ButtonID.Down);

                        return true;
                    }
                }
                else if (m.Msg == Win32Calls.WM_KEYUP)
                {
                    if (_pushed != ButtonID.None)
                        EndProcessArrowKey();
                }
            }

            return base.ProcessKeyPreview(ref m);
        }

        private void ProcessArrowKey(ButtonID buttonId)
        {
//            if (_realtimeChange && (_edit != null))
  //              _edit.CommitChanges();

            _pushed = buttonId;

            Invalidate();

            HandleUpDownButton(_pushed, false);

            StartTimer();
        }

        private void EndProcessArrowKey()
        {
            _pushed = ButtonID.None;
            StopTimer();
            Invalidate();
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
                    (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                        PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                {
                    if (_edit != null)
                    {
                        _edit.SelectAll();
                        return true;
                    }
                }
            }
            else if (key == Keys.Tab)
            {
                PropertyValue.ValueValidationResult changed = CommitChanges(true);

                if (PropertyValue.IsErrorCode(changed) &&
                    (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                        PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                {
                    if (_edit != null)
                        _edit.SelectAll();
                    return true;
                }
            }
            else if (key == Keys.Escape)
            {
                // Restore the old string in the textbox
                Text = _initialValue;
                CommitChanges(true);
            }

            if (ContainsFocus)
                return base.ProcessDialogKey(keyData);
            else
                return true;
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if (_edit == null)
            {
                if ((keyData == Keys.Up) || (keyData == Keys.Down))
                    return true;
            }

            return base.IsInputKey(keyData);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (_edit == null)
            {
                if (e.KeyData == Keys.Up)
                    ProcessArrowKey(ButtonID.Up);
                else if (e.KeyData == Keys.Down)
                    ProcessArrowKey(ButtonID.Down);
            }

            base.OnKeyDown(e);
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            if (_edit == null)
            {
                if ((e.KeyData == Keys.Up) || (e.KeyData == Keys.Down))
                    EndProcessArrowKey();
            }

            base.OnKeyUp(e);
        }

        public void OnF4()
        {
            Focus();
        }

        public bool OnParentKeyDown(KeyEventArgs e)
        {
            if ((_edit != null) && _edit.OnParentKeyDown(e))
                return true;

            return false;
        }

        protected override void OnMouseWheel(MouseEventArgs e)
        {
            int nb = e.Delta / 120;
            if (nb > 0)
            {
                for (int i = 0; i < nb; i++)
                    ProcessArrowKey(ButtonID.Up);
            }
            else
            {
                for (int i = 0; i < -nb; i++)
                    ProcessArrowKey(ButtonID.Down);
            }

            EndProcessArrowKey();
            Invalidate();
        }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            return true;
        }

        public virtual bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force)
        {
            if (ReadOnly)
                return false;

            bool oldRTChange = _realtimeChange;
            _realtimeChange = true;

            HandleUpDownButton((forward ? ButtonID.Up : ButtonID.Down), true);

            _realtimeChange = oldRTChange;

            // TODO : retourner false si valeur pas changee (cause validator par ex)

            return true;
        }

        public TextBox TextBox
        {
            get { return _edit; }
        }

        public PropertyValue.ValueValidationResult CommitChanges(string strToValidate, bool final)
        {
            PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;

            PropertyValue value = _ownerPropertyEnum.Property.Value;

            bool compareTextInCommitChanges = value.TypeConverter.CanConvertFrom(typeof(string));
            bool textHasChanged = false;
            if (compareTextInCommitChanges)
                textHasChanged = !value.DisplayString.Equals(strToValidate);

            // Store the previous value
            object oldValue = null;
            object newValue = null;
            bool multipleValues = value.HasMultipleValues;

            if (!compareTextInCommitChanges || textHasChanged)
            {
                if (!multipleValues)
                    oldValue = value.GetValue();

                try
                {
                    if (!multipleValues || (strToValidate.Length != 0))
                        newValue = value.GetActualValue(strToValidate);
                }
                catch (Exception e)
                {
                    _currentInvalidPropertyEnum = OwnerPropertyEnumerator;
                    _currentValueValidationResult = PropertyValue.ValueValidationResult.TypeConverterFailed;
                    _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum, Text,
                            PropertyValue.ValueValidationResult.TypeConverterFailed, e));

                    if (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore)
                    {
                        Text = value.DisplayString;
                        _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                            value.GetValue(), _currentValueValidationResult));
                    }

                    return _currentValueValidationResult;
                }
            }

            if ((multipleValues && textHasChanged) || 
                ((newValue != null) && ((oldValue == null) || (newValue.Equals(oldValue) == false))) ||
                ((newValue == null) && (oldValue != null)))
            {
                // Freeze painting because updating the property may also update other properties in the grid.
                _ownerPropertyEnum.Property.ParentGrid.BeginUpdate();

                _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                PropertyEnumerator invalidPropertyEnum;
                changed = value.SetValueFromInPlaceCtrl(newValue, out invalidPropertyEnum);
//                changed = value.SetValueFromInPlaceCtrl(strToValidate);

                if (PropertyValue.IsErrorCode(changed))
                    _currentInvalidPropertyEnum = invalidPropertyEnum;

                // Notify the grid if the value is invalid
                if (changed == PropertyValue.ValueValidationResult.ValidatorFailed)
                {
                    OwnerPropertyEnumerator.Property.ParentGrid.NotifyValueValidation(
                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                        newValue, PropertyValue.ValueValidationResult.ValidatorFailed));
                }
                else if (changed == PropertyValue.ValueValidationResult.ExceptionByClient)
                {
                    OwnerPropertyEnumerator.Property.ParentGrid.NotifyValueValidation(
                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                        newValue, PropertyValue.ValueValidationResult.ExceptionByClient, value.LastExceptionByClient));
                }

                // We have to update the edit control in case the value was refused or if its string representation
                // is different from what was typed (changes done by a CultureInfo for example)
                if ((_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                    PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore) ||
                    (changed == PropertyValue.ValueValidationResult.Validated))
                {
                    Text = value.DisplayString;
                    if (final)
                        _initialValue = Text;
                }

                // If value is invalid, previous value is restored if ValueNotValidBehaviorMode is set accordingly
                if (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore)
                {
                    if (PropertyValue.IsErrorCode(changed))
                    {
                        changed = PropertyValue.ValueValidationResult.PreviousValueRestored;
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                            value.GetValue(), changed));
                    }
                }

                if (changed == PropertyValue.ValueValidationResult.Validated)
                {
                    if (PropertyValue.IsErrorCode(_currentValueValidationResult))
                    {
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                            value.GetValue(), changed));
                    }

                    _ownerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(new PropertyChangedEventArgs(_ownerPropertyEnum));
                }

                _currentValueValidationResult = changed;

                // Defreeze painting and repaint
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
                        value.GetValue(), _currentValueValidationResult));
                }
            }

            return changed;
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            return CommitChanges(Text, final);
        }

        public Control GetDropDownContainer()
        {
            return null;
        }
    }
}
