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
using System.Globalization;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Security.Permissions;
using Skybound.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when a textbox for a numerical value plus a combobox for its
    /// unit are needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceUnit : Control, IInPlaceControl
    {
        private PropInPlaceTextbox _edit = null;

        private bool _dontTransferFocus = false;

        private PropertyValue.ValueValidationResult _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

        private string _initialListValue;

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

                PropertyValue unitValue = _ownerPropertyEnum.Property.GetValue(PropertyUnitLook.UnitValue);
                _initialListValue = unitValue.DisplayString;
                _listText = _initialListValue;

                if (_edit != null)
                    _edit.OwnerPropertyEnumerator = value;

                _editorContainer.Visible = false;
            }

            get { return _ownerPropertyEnum; }
        }

        public bool ReadOnly
        {
            get { return _ownerPropertyEnum.Property.Value.IsReadOnly(_ownerPropertyEnum); }
        }

        private bool _realtimeChange = true;

        public bool RealtimeChange
        {
            set { _realtimeChange = value; }
            get { return _realtimeChange; }
        }

        private bool _pushed = false;

        private bool _mouseOver = false;

        /// <summary>
        /// Because Control.OnLostFocus and Control.OnGotFocus doesn't provide a reference to the
        /// window that lost or gained the focus, this variable will contain this missing information.
        /// It is initialized in WndProc when processing WM_FILLFOCUS and WM_SETFOCUS messages.
        /// It is also embedded in a FocusEventArgs instance for event listeners.
        /// </summary>
        private Control _wmFocusMissingArg;

        private PropInPlaceContainer _editorContainer = new PropInPlaceContainer();

        public override string Text
        {
            set { _edit.Text = value; }
            get { return _edit.Text; }
        }

        private string _listText;

        public string ListText
        {
            set { _listText = value; Invalidate(false); }
            get { return _listText; }
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

        public PropInPlaceUnit()
        {
            _edit = new PropInPlaceTextbox(false);
            _edit.Parent = this;
            _edit.TabStop = false;

            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque | ControlStyles.ResizeRedraw, true);

            Cursor = Cursors.Arrow;
        }

        protected int GetButtonWidth(Graphics graphics)
        {
            if (ReadOnly)
                return 0;

            return SystemInformation.VerticalScrollBarWidth;
        }

        protected Rectangle GetButtonRect(Graphics graphics)
        {
            int buttonWidth = GetButtonWidth(graphics);
            return new Rectangle(Width - buttonWidth, 0, buttonWidth, Height);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            // Draw the background

            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(e.Graphics,
                ClientRectangle, _ownerPropertyEnum);

            Rectangle buttonRect = GetButtonRect(e.Graphics);
            int listboxWidth = GetListboxWidth();

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

            // Draw the combobox arrow
            if (!ReadOnly)
            {
				if (ThemeRenderer.Enabled)
				{
					int state = (_pushed ?
						(_mouseOver ? ThemeComboBox.DropDownButtonPressed : ThemeComboBox.DropDownButtonHot) :
						(_mouseOver ? ThemeComboBox.DropDownButtonHot : ThemeComboBox.DropDownButtonNormal));

					ThemeComboBox.DropDownButton.Draw(e.Graphics, state, buttonRect);
				}
                else
                {
                    ControlPaint.DrawComboButton(e.Graphics, buttonRect, (_pushed && _mouseOver) ?
                        ButtonState.Pushed : ButtonState.Normal);
                }

                if (Focused)
                {
                    Rectangle focusRect = buttonRect;
                    focusRect.Inflate(-3, -3);
                    ControlPaint.DrawFocusRectangle(e.Graphics, focusRect);
                }
            }

            // Draw the listbox content

            Rectangle listRect = ClientRectangle;
            listRect.X = listRect.Right - listboxWidth;
            listRect.Width = listboxWidth - GetButtonWidth(null);
            Rectangle listRect2 = listRect;
            listRect2.Inflate(0, -1);
            listRect2.Width--;
            if (Focused)
            {
                e.Graphics.FillRectangle(SystemBrushes.FromSystemColor(SystemColors.Highlight), listRect2);
                ControlPaint.DrawFocusRectangle(e.Graphics, listRect2);
            }
            else
            {
                Brush listBrush = new SolidBrush(_ownerPropertyEnum.Property.ParentGrid.GridColor);
                e.Graphics.FillRectangle(listBrush, listRect2);
                listBrush.Dispose();
            }

            Rectangle strRect = listRect;
            int margin = _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
            strRect.X += margin;
            strRect.Width = strRect.Right - strRect.Left - margin;
            strRect.Y++;
            strRect.Height--;
            Win32Calls.DrawText(e.Graphics, ListText, ref strRect, Font, valueColor,
                Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX);

            // Draw the value
            if (_edit == null)
            {
                Rectangle valueRect = ClientRectangle;
                valueRect.Width -= listboxWidth;

                _ownerPropertyEnum.Property.Value.DrawValue(e.Graphics, valueRect, valueColor, _ownerPropertyEnum, Text);
            }

            base.OnPaint(e);
        }

        public bool OnForbidClickThrough(Point pt)
        {
            Rectangle listRect = ClientRectangle;
            int listboxWidth = GetListboxWidth();
            listRect.X = listRect.Right - listboxWidth;
            listRect.Width = listboxWidth;

            return listRect.Contains(pt);
        }

        public int GetListboxWidth()
        {
            PropertyValue unitValue = _ownerPropertyEnum.Property.GetValue(PropertyUnitLook.UnitValue);

            string[] units = unitValue.GetDisplayedValues();

            Graphics graphics = CreateGraphics();
            int maxWidth = 0;
            foreach (string unit in units)
            {
                Size size = Win32Calls.GetTextExtent(graphics, unit, Font);
                if (size.Width > maxWidth)
                    maxWidth = size.Width;
            }

            int buttonWidth = GetButtonWidth(graphics);
            int width = maxWidth + 2 * _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin + buttonWidth;
            graphics.Dispose();

            if (width > ClientRectangle.Width - buttonWidth - 20)
                width = ClientRectangle.Width - buttonWidth - 20;
            if (width < buttonWidth)
                width = buttonWidth;

            return width;
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

                    int margin = _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
                    int leftPos = (_ownerPropertyEnum.Property.Value.EditboxLeftMargin == -1 ? 0 :
                        margin - _ownerPropertyEnum.Property.Value.EditboxLeftMargin);

                    int listboxWidth = GetListboxWidth();
                    Rectangle editRect = new Rectangle(leftPos, 0, clientRect.Width - listboxWidth - leftPos, clientRect.Height);
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

            Invalidate(false);
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            RepositionChildren();
        }

        protected override void OnGotFocus(EventArgs e)
        {
            if (_edit != null)
            {
                if (_dontTransferFocus == false)
                {
                    if (_wmFocusMissingArg != _edit)
                    {
                        if (_wmFocusMissingArg != _editorContainer.ContentControl)
                            _edit.Focus();
                    }
                    else
                    {
                        if ((_ownerPropertyEnum.Property.ParentGrid.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys) ||
                            ((_ownerPropertyEnum.Property.ParentGrid.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyInSubControls) == 0))
                            Parent.Focus();
                    }
                }
            }
            else
                Invalidate();

            base.OnGotFocus(e);
        }

        protected override void OnLostFocus(EventArgs e)
        {
            _pushed = false;
            _mouseOver = false;

            base.OnLostFocus(e);
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

            base.WndProc(ref m);
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
                                CommitChanges(false);
                                break;
                            }
                        }

                        break;
                    }
                }
            }

        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (e.Clicks == 2)
                {
                    Graphics graphics = CreateGraphics();
                    if (GetButtonRect(graphics).Contains(new Point(e.X, e.Y)) == false)
                        ScrollValueOnDoubleClick(!(ModifierKeys == Keys.Shift),
                            ScrollValueOnDoubleClickSource.FromInPlaceCtrl, false);
                }
                else
                {
                    if (RealtimeChange)
                    {
                        PropertyValue.ValueValidationResult changed = CommitChanges(false);

                        if (PropertyValue.IsErrorCode(changed) &&
                            (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                                PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                        {
                            _edit.Focus();
                            _edit.SelectAll();
                            return;
                        }
                    }

                    Graphics graphics = CreateGraphics();
                    if (GetButtonRect(graphics).Contains(new Point(e.X, e.Y)))
                    {
                        _pushed = true;
                        _mouseOver = true;
                        Capture = true;
                    }
                    graphics.Dispose();

                    _dontTransferFocus = true;
                    Focus();
                    _dontTransferFocus = false;

                    Invalidate(false);
                    Update();
                }
            }

            base.OnMouseDown(e);
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            _mouseOver = false;
            Invalidate(false);

            base.OnMouseLeave(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            bool oldMouseOver = _mouseOver;
            Graphics graphics = CreateGraphics();
            _mouseOver = GetButtonRect(graphics).Contains(new Point(e.X, e.Y));
            graphics.Dispose();
            if (oldMouseOver != _mouseOver)
                Invalidate(false);

            base.OnMouseMove(e);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            if (_pushed && (e.Button == MouseButtons.Left))
            {
                Capture = false;
                _pushed = false;
                _mouseOver = false;

                Invalidate(false);

                using (Graphics graphics = CreateGraphics())
                {
                    if (GetButtonRect(graphics).Contains(new Point(e.X, e.Y)))
                        RunButton();
                }
            }

            base.OnMouseUp(e);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Space)
            {
                _mouseOver = true;
                _pushed = true;
                Invalidate(false);
            }
            else if ((e.KeyCode == Keys.Up) || (e.KeyCode == Keys.Down))
            {
                ProcessArrowKey(e.KeyCode, false);
                Invalidate();
            }
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            if (e.KeyData == Keys.Space)
            {
                _mouseOver = false;
                _pushed = false;
                Invalidate(false);

                RunButton();
            }
        }

        protected void ProcessArrowKey(Keys key, bool rotate)
        {
            PropertyValue unitValue = _ownerPropertyEnum.Property.GetValue(PropertyUnitLook.UnitValue);

            object[] displayStrings = unitValue.GetDisplayedValues();
            int index = -1;

            if (unitValue.HasMultipleValues)
            {
                if (key == Keys.Up)
                    index = displayStrings.Length - 1;
                else if (key == Keys.Down)
                    index = 0;
            }
            else
            {
                for (int i = 0; i < displayStrings.Length; i++)
                {
                    if (ListText.Equals(displayStrings[i]))
                    {
                        if ((key == Keys.Up) && (i > 0))
                            index = i - 1;
                        else if ((key == Keys.Up) && rotate)
                            index = displayStrings.Length - 1;
                        else if ((key == Keys.Down) && (i < displayStrings.Length - 1))
                            index = i + 1;
                        else if ((key == Keys.Down) && rotate)
                            index = 0;

                        break;
                    }
                }
            }

            if (index != -1)
            {
                ListText = displayStrings[index].ToString();

                if (RealtimeChange)
                    CommitListChanges();
            }
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if ((keyData == Keys.Up) || (keyData == Keys.Down))
                return true;

            return base.IsInputKey(keyData);
        }

#if _DOTNET2
        [UIPermission(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
#endif
		protected override bool ProcessDialogKey(Keys keyData)
        {
            Keys key = (keyData & Keys.KeyCode);

            if (key == Keys.Return)
            {
                if (Focused)
                {
                    _pushed = false;
                    _mouseOver = false;

                    if (!RealtimeChange)
                        CommitListChanges();

                    _initialListValue = ListText;
                }
                else
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
            }
            else if (key == Keys.Escape)
            {
                if (Focused)
                {
                    _pushed = false;
                    _mouseOver = false;

                    ListText = _initialListValue;

                    if (RealtimeChange)
                        CommitListChanges();
                }
                else
                {
                    // Restore the old string in the textbox
                    Text = _ownerPropertyEnum.Property.Value.DisplayString;
                    CommitChanges(true);
                }
            }
            else if ((key == Keys.F4) || ((key == Keys.Down) && ((keyData & Keys.Alt) != Keys.None)))
            {
                _pushed = false;
                _mouseOver = false;

                if ((_edit != null) && _edit.Focused)
                {
                    PropertyValue.ValueValidationResult changed = CommitChanges(false);

                    if (!PropertyValue.IsErrorCode(changed) ||
                        (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                            PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore))
                    {
                        OnF4();
                    }
                    else
                        _edit.SelectAll();
                }
                else
                    OnF4();

                return true;
            }
            else if (key == Keys.Tab)
            {
                PropertyValue.ValueValidationResult changed = CommitChanges(Focused);

                if (PropertyValue.IsErrorCode(changed) &&
                    (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                        PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                {
                    if (_edit != null)
                        _edit.SelectAll();
                    return true;
                }

                if (_edit != null)
                {
                    bool forward = ((keyData & Keys.Shift) == Keys.None);

                    if (forward && _edit.Focused)
                    {
                        if ((_ownerPropertyEnum.Property.ParentGrid.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys) ||
                            ((_ownerPropertyEnum.Property.ParentGrid.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyInSubControls) != 0))
                        {
                            _dontTransferFocus = true;
                            Focus();
                            Invalidate(false);
                            _dontTransferFocus = false;
                            return true;
                        }
                    }
                    else if (!forward && _edit.Focused)
                    {
                        if (_ownerPropertyEnum.Property.ParentGrid.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys)
                        {
                            Parent.Focus();
                            return true;
                        }
                    }
                    else if (!forward && Focused)
                    {
                        _edit.Focus();
                        _edit.SelectAll();
                        Invalidate(false);
                        return true;
                    }
                }
            }

            if (ContainsFocus)
                return base.ProcessDialogKey(keyData);
            else
                return true;
        }

        protected void RunButton()
        {
            if (ReadOnly)
                return;

            _ownerPropertyEnum.Property.InPlaceCtrlInAction = this;

            PropertyValue unitValue = _ownerPropertyEnum.Property.GetValue(PropertyUnitLook.UnitValue);
            _editorContainer.SetContent(_ownerPropertyEnum, unitValue.DisplayString, PropertyUnitLook.UnitValue);

            if (_editorContainer.ContentControl == null)
                return;

            _editorContainer.MustBeClosed += new EventHandler(OnDropDownContentMustBeClosed);

            Win32Calls.SetWindowLong(_editorContainer.Handle, -8, (int)Parent.Handle);

            Rectangle workingArea = Screen.FromControl(this).WorkingArea;
            Point offset = Parent.PointToScreen(new Point(0, 0));

            Rectangle bounds = Bounds;

            // No listbox smaller than the value column
            if (_editorContainer.Width < Size.Width)
                _editorContainer.Width = Size.Width;

            // No listbox left truncated
            Point origin = new Point(bounds.Right - _editorContainer.Size.Width + offset.X, bounds.Bottom + 1 + offset.Y);
            if (origin.X < workingArea.Left)
                origin.X = workingArea.Left;

            // No listbox bottom truncated
            if (origin.Y + _editorContainer.Height > workingArea.Bottom)
                origin.Y = bounds.Top - _editorContainer.Height + offset.Y;

            _editorContainer.Location = origin;
            _editorContainer.ContentControl.Dock = DockStyle.Fill;

            // Show the window but don't activate it
            Win32Calls.ShowWindow(new HandleRef(_editorContainer, _editorContainer.Handle), Win32Calls.SW_SHOWNA);
            _editorContainer.Visible = true;
            _ownerPropertyEnum.Property.ParentGrid.SetDropDownTopWindow(_editorContainer);

            _editorContainer.DoModalLoop();
        }

        void OnDropDownContentMustBeClosed(object sender, EventArgs e)
        {
            DropDownContentClosedEventArgs ddccea = e as DropDownContentClosedEventArgs;
            if (ddccea.ReasonLostFocus)
                _wmFocusMissingArg = ddccea.WndTakingFocus;

            object obj = _editorContainer.DropDownContent.GetValue();

            if (ddccea.ToBeValidated)
            {
                if (obj is string)
                {
                    ListText = obj.ToString();
                    CommitListChanges();
                    _initialListValue = ListText;
                }
            }

            HideDropDownContent();
        }

        protected void HideDropDownContent()
        {
            _editorContainer.MustBeClosed -= new EventHandler(OnDropDownContentMustBeClosed);

            _ownerPropertyEnum.Property.InPlaceCtrlInAction = null;
            _editorContainer.Visible = false;
            _editorContainer.SetContent(null, null);
            _ownerPropertyEnum.Property.ParentGrid.SetDropDownTopWindow(null);
            _ownerPropertyEnum.Property.ParentGrid.OnInPlaceCtrlFinishedEdition();
        }

        public void OnF4()
        {
            // If the textbox takes the focus it will show a blink, so we prevent that
            if (!ReadOnly)
                _dontTransferFocus = true;
            Focus();
            _dontTransferFocus = false;

            RunButton();
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
                    ProcessArrowKey(Keys.Up, false);
            }
            else
            {
                for (int i = 0; i < -nb; i++)
                    ProcessArrowKey(Keys.Down, false);
            }

            Invalidate();
        }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            if (forward)
            {
                if (_edit == null)
                    return true;
                else
                    return Focused;
            }
            else
            {
                if (_edit == null)
                    return true;
                else
                    return _edit.Focused;
            }
        }

        public virtual bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force)
        {
            if (ReadOnly || (source != ScrollValueOnDoubleClickSource.FromInPlaceCtrl))
                return false;

            if (forward)
                ProcessArrowKey(Keys.Down, true);
            else
                ProcessArrowKey(Keys.Up, true);

            return true;
        }

        public TextBox TextBox
        {
            get { return _edit; }
        }

        public void CommitListChanges()
        {
            PropertyValue unitValue = _ownerPropertyEnum.Property.GetValue(PropertyUnitLook.UnitValue);
            string oldValue = unitValue.DisplayString;

            if (ListText.Equals(oldValue) == false)
            {
                unitValue.PreviousValue = unitValue.GetValue();
                unitValue.SetValueFromInPlaceCtrl(ListText);

                if (unitValue.DisplayString != oldValue)
                    _ownerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(
                        new PropertyChangedEventArgs(_ownerPropertyEnum, unitValue.AdditionalValueKey));
            }
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;

            PropertyValue value = _ownerPropertyEnum.Property.Value;

            // Store the previous value
            object oldValue = null;
            if (value.HasMultipleValues == false)
                oldValue = value.GetValue();

            // Assign the new value based on the edit content
            object newValue = null;
            try
            {
                if ((value.HasMultipleValues == false) || (Text.Length != 0))
                    newValue = value.GetActualValue(Text);
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

            if (((newValue != null) && ((oldValue == null) || (newValue.Equals(oldValue) == false))) ||
                ((newValue == null) && (oldValue != null)))
            {
                // Freeze painting because updating the property may also update other properties in the grid.
                _ownerPropertyEnum.Property.ParentGrid.BeginUpdate();

                _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                PropertyEnumerator invalidPropertyEnum;
                changed = value.SetValueFromInPlaceCtrl(Text, out invalidPropertyEnum);

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

                // Notify the properties control if the value has really changed
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

        public Control GetDropDownContainer()
        {
            return _editorContainer;
        }
    }
}

