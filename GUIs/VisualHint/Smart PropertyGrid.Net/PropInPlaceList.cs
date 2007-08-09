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
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Globalization;
using System.Security.Permissions;
using Skybound.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when a dropdown button is needed to popup a simple listbox
    /// or a custom applet.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceList : Control, IInPlaceControl
    {
        private PropInPlaceTextbox _edit = null;

        private bool _dontTransferFocus = false;

        private int _oldImageListIndex;

        private PropertyValue.ValueValidationResult _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

        private string _initialValue;

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

                _oldImageListIndex = _ownerPropertyEnum.Property.Value.ImageIndex;

                if (_edit != null)
                {
                    _edit.OwnerPropertyEnumerator = value;
                    _edit.TextChanged += new EventHandler(OnTextboxTextChanged);
                }

                _initialValue = Text;

//                _editorContainer.Visible = false;
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

        private PropInPlaceContainer _editorContainer;// = new PropInPlaceContainer();

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

        public PropInPlaceList(bool editable)
        {
            if (editable)
            {
                _edit = new PropInPlaceTextbox(false);
                _edit.LostFocus += new EventHandler(OnChildLostFocus);
                _edit.Parent = this;
                _edit.TabStop = false;
            }

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
            if (ReadOnly)
                return Rectangle.Empty;

            int buttonWidth = GetButtonWidth(graphics);
            return new Rectangle(Width - buttonWidth, 0, buttonWidth, Height);
        }

        private bool ShowIcon
        {
            get
            {
                return ((_ownerPropertyEnum.Property.Value.ImageList != null) &&
                    (_ownerPropertyEnum.Property.Value.ImageIndex == -1));
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            // Draw the background

            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(e.Graphics,
                ClientRectangle, _ownerPropertyEnum);

            Rectangle buttonRect = GetButtonRect(e.Graphics);
            PropertyValue propValue = _ownerPropertyEnum.Property.Value;
            Rectangle valueRect = ClientRectangle;

            Color valueColor;
            if (_ownerPropertyEnum.Property.Enabled == false)
                valueColor = SystemColors.GrayText;
            else
            {
                if (Focused)
                    valueColor = SystemColors.HighlightText;
                else
                    valueColor = propValue.ForeColor;
            }

            // Draw the value
            if (_edit == null)
            {
                if (Focused)// && (propValue.Look == null))
                {
                    Rectangle fillRect = propValue.GetStringValueRect(e.Graphics, valueRect, Point.Empty);
                    int margin = _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
                    fillRect.X -= (ShowIcon ? margin / 2 : margin);
                    fillRect.Width = buttonRect.Left - 1 - fillRect.Left;
                    fillRect.Y++;
                    fillRect.Height -= 2;
                    e.Graphics.FillRectangle(SystemBrushes.FromSystemColor(SystemColors.Highlight), fillRect);
                    ControlPaint.DrawFocusRectangle(e.Graphics, fillRect);
                }
            }

            valueRect.Width -= buttonRect.Width + 1;
            propValue.DrawValue(e.Graphics, valueRect, valueColor, _ownerPropertyEnum, Text);

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

            base.OnPaint(e);
        }

        public bool OnForbidClickThrough(Point pt)
        {
            Graphics graphics = CreateGraphics();
            bool result = GetButtonRect(graphics).Contains(pt);
            graphics.Dispose();

            return result;
        }

        protected override void OnMove(EventArgs e)
        {
            base.OnMove(e);
            OnSizeChanged(e);
        }

        void OnTextboxTextChanged(object sender, EventArgs e)
        {
            Invalidate();
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

                    int buttonWidth = GetButtonWidth(null);

                    Rectangle valueRect = ClientRectangle;
                    valueRect.Width -= buttonWidth + 1;
                    valueRect.Height++; // for GetStringValueRect()
                    Graphics graphics = CreateGraphics();
                    valueRect = _ownerPropertyEnum.Property.Value.GetStringValueRect(graphics, valueRect, Point.Empty);
                    valueRect.Height--;

                    int margin = _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
                    int leftPos = valueRect.Left - margin + (_ownerPropertyEnum.Property.Value.EditboxLeftMargin == -1 ? 0 :
                        margin - _ownerPropertyEnum.Property.Value.EditboxLeftMargin);

                    Rectangle editRect = new Rectangle(leftPos, 0, clientRect.Width - buttonWidth - leftPos, clientRect.Height);
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
                        Win32Calls.GetTextMetrics(graphics, Font, ref tm);
                        int extraHeight = editRect.Height - tm.tmHeight;
                        editRect.Y += extraHeight / 2;
                        editRect.Height -= extraHeight / 2;

                        _edit.Invalidate();
                        _edit.Bounds = editRect;
                    }

                    graphics.Dispose();
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
                        if (((_editorContainer == null) && (_wmFocusMissingArg != null)) || (((_editorContainer != null) && (_wmFocusMissingArg != _editorContainer.ContentControl))))
                            _edit.Focus();
                    }
                    else
                        Parent.Focus();
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

        void OnChildLostFocus(object source, EventArgs e)
        {
            // Recalculate the image index based on changes in the textbox
            if (_ownerPropertyEnum.Property.Value.ImageList != null)
            {
                int index = 0;
                string[] displayedStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                _ownerPropertyEnum.Property.Value.ImageIndex = -1;
                foreach (string str in displayedStrings)
                {
                    if (str == _ownerPropertyEnum.Property.Value.DisplayString)
                    {
                        _ownerPropertyEnum.Property.Value.ImageIndex = index;
                        break;
                    }

                    index++;
                }
            }
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
                            int index = ((i + j) + 1) % displayedStrings.Length;
                            string currentStr = displayedStrings[index];
                            if (string.Compare(currentStr.Substring(0, 1), letter, true, CultureInfo.InvariantCulture) == 0)
                            {
                                Text = currentStr;
                                _ownerPropertyEnum.Property.Value.ImageIndex = index;
                                if (RealtimeChange)
                                    CommitChanges(false);
                                break;
                            }
                        }

                        break;
                    }
                }
            }

            base.OnKeyPress(e);
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
/*                    if (RealtimeChange)
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
                    */
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
                if (_edit == null)
                {
                    ProcessArrowKey(e.KeyCode);
                    Invalidate();
                }
            }

            base.OnKeyDown(e);
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

            base.OnKeyUp(e);
        }

        protected void ProcessArrowKey(Keys key)
        {
            if (ReadOnly)
                return;

            object[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();

            if (_ownerPropertyEnum.Property.Value.HasMultipleValues)
            {
                if (key == Keys.Up)
                {
                    Text = displayStrings[displayStrings.Length - 1].ToString();
                    _ownerPropertyEnum.Property.Value.ImageIndex = displayStrings.Length - 1;
                }
                else if (key == Keys.Down)
                {
                    Text = displayStrings[0].ToString();
                    _ownerPropertyEnum.Property.Value.ImageIndex = 0;
                }

                RepositionChildren();

                if (RealtimeChange)
                    CommitChanges(false);
                else
                    Invalidate();
            }
            else
            {
                for (int i = 0; i < displayStrings.Length; i++)
                {
                    if (Text.Equals(displayStrings[i]))
                    {
                        bool change = false;
                        if ((key == Keys.Up) && (i > 0))
                        {
                            Text = displayStrings[i - 1].ToString();
                            _ownerPropertyEnum.Property.Value.ImageIndex = i - 1;
                            change = true;
                        }
                        else if ((key == Keys.Down) && (i < displayStrings.Length - 1))
                        {
                            Text = displayStrings[i + 1].ToString();
                            _ownerPropertyEnum.Property.Value.ImageIndex = i + 1;
                            change = true;
                        }

                        if (change)
                        {
                            if (RealtimeChange)
                                CommitChanges(false);
                            else
                                Invalidate();
                        }

                        break;
                    }
                }
            }
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if (!ReadOnly)
            {
                if ((keyData == Keys.Up) || (keyData == Keys.Down))
                    return true;
            }

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
                _pushed = false;
                _mouseOver = false;

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
            else if (key == Keys.Escape)
            {
                _pushed = false;
                _mouseOver = false;

                // Restore the old string in the edit box
                Text = _initialValue;
                _ownerPropertyEnum.Property.Value.ImageIndex = _oldImageListIndex;

                CommitChanges(true);
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
                bool forward = ((keyData & Keys.Shift) == Keys.None);

                bool final = (_edit == null) || (!forward && _edit.Focused);
                PropertyValue.ValueValidationResult changed = CommitChanges(final);

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
                    if (forward && _edit.Focused)
                    {
                        if ((_ownerPropertyEnum.Property.ParentGrid.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys) ||
                            ((_ownerPropertyEnum.Property.ParentGrid.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyInSubControls) != 0))
                        {
//                            CommitChanges(false);
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
//                            CommitChanges(true);
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

            // TODO: Passer la valeur actuelle au lieu de Text qd realtimechange==false et TypeConverter->exception ?
            _editorContainer = new PropInPlaceContainer();
            _editorContainer.SetContent(_ownerPropertyEnum, Text);

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
            {
                _editorContainer.ResizeTop = true;
                origin.Y = bounds.Top - _editorContainer.Height + offset.Y;
            }
            else
                _editorContainer.ResizeTop = false;

            _editorContainer.Location = origin;

            // The previous call to ResizeTop resets the DockPadding to 0 if the container is not resizable
            _editorContainer.ContentControl.Dock = DockStyle.Fill;

            // Show the window but don't activate it
            Win32Calls.ShowWindow(new HandleRef(_editorContainer, _editorContainer.Handle), Win32Calls.SW_SHOWNA);
            _editorContainer.Visible = true;
            _ownerPropertyEnum.Property.ParentGrid.SetDropDownTopWindow(_editorContainer);

            _editorContainer.DoModalLoop();
        }

        void OnDropDownContentMustBeClosed(object sender, EventArgs e)
        {
            _ownerPropertyEnum.Property.ParentGrid.BeginUpdate();

            object obj = _editorContainer.DropDownContent.GetValue();
            int imageIndex = _editorContainer.SelectedIndex;
//            HideDropDownContent();

            DropDownContentClosedEventArgs ddccea = e as DropDownContentClosedEventArgs;

            if (ddccea.ToBeValidated)
            {
//                object obj = _editorContainer.DropDownContent.GetValue();

                if (obj != null) // obj == null if the list is closed without any selection
                {
                    if (obj is string)
                        Text = obj as string;
                    else
                    {
                        string str = _ownerPropertyEnum.Property.Value.TypeConverter.ConvertToString(
                            _ownerPropertyEnum.Property.Value.GetTypeDescriptorContext(_ownerPropertyEnum),
                            _ownerPropertyEnum.Property.Value.CultureInfo, obj);
                        Text = (str == null ? "" : str);
                    }
                    _ownerPropertyEnum.Property.Value.ImageIndex = imageIndex;
//                    _ownerPropertyEnum.Property.Value.ImageIndex = _editorContainer.SelectedIndex;
                    CommitChanges(true);
                }
            }
            else
            {
                _ownerPropertyEnum.Property.Value.ImageIndex = _oldImageListIndex;
                Text = _initialValue;

                CommitChanges(true);
            }
/*
            if (ddccea.ReasonLostFocus)
            {
                _wmFocusMissingArg = ddccea.WndTakingFocus;

                // We simulate a LostFocus on this PropInPlaceList
                base.OnLostFocus(e);
            }
*/
            HideDropDownContent();

            _ownerPropertyEnum.Property.ParentGrid.EndUpdate();
        }

        protected void HideDropDownContent()
        {
            // If no icon was initially shown (in the case of multiple values as a result of calling SelectedObjects)
            // then the textbox was at the far left. If a value has been entered, the textbox must be right shifted.
            if ((_oldImageListIndex == -1) && (_ownerPropertyEnum.Property.Value.ImageIndex >= 0))
                RepositionChildren();

            _editorContainer.MustBeClosed -= new EventHandler(OnDropDownContentMustBeClosed);

            // If the dropdown window is closed because the focus changed to another app,
            // the focus is not reset to the grid
//            if (_wmFocusMissingArg != null)
  //              Parent.Focus();

            _ownerPropertyEnum.Property.InPlaceCtrlInAction = null;

            _editorContainer.Visible = false;
            _editorContainer.SetContent(null, null);
            _ownerPropertyEnum.Property.ParentGrid.SetDropDownTopWindow(null);
            _ownerPropertyEnum.Property.ParentGrid.OnInPlaceCtrlFinishedEdition();

            _editorContainer.Dispose();
            _editorContainer = null;
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
            if ((_edit != null) && !_edit.ReadOnly && _edit.OnParentKeyDown(e))
                return true;

            return false;
        }

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override bool ProcessKeyPreview(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_KEYDOWN)
            {
                Keys key = (Keys)(int)m.WParam;
                if ((key == Keys.Down) || (key == Keys.Up))
                {
                    ProcessArrowKey(key);
                    _edit.SelectAll();
                    return true;
                }
            }

            return base.ProcessKeyPreview(ref m);
        }

        protected override void OnMouseWheel(MouseEventArgs e)
        {
            int nb = e.Delta / 120;
            if (nb > 0)
            {
                for (int i = 0; i < nb; i++)
                    ProcessArrowKey(Keys.Up);
            }
            else
            {
                for (int i = 0; i < -nb; i++)
                    ProcessArrowKey(Keys.Down);
            }

            Invalidate();
        }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            if (forward)
            {
                if ((_edit == null) || ReadOnly)
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
            if (ReadOnly)
                return false;

            string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();

            for (int i = 0; i < displayStrings.Length; i++)
            {
                if (Text.Equals(displayStrings[i]))
                {
                    int index;
                    if (forward)
                        index = (i < displayStrings.Length - 1 ? i + 1 : 0);
                    else
                        index = (i > 0 ? i - 1 : displayStrings.Length - 1);

                    _ownerPropertyEnum.Property.Value.ImageIndex = index;
                    Text = displayStrings[index];

                    if (force || RealtimeChange)
                        CommitChanges(source == ScrollValueOnDoubleClickSource.FromLabel);

                    if ((_edit != null) && (source != ScrollValueOnDoubleClickSource.FromLabel))
                        _edit.SelectAll();

                    return true;
                }
            }

            return false;
        }

        public TextBox TextBox
        {
            get { return _edit; }
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;

            PropertyValue value = _ownerPropertyEnum.Property.Value;

            bool compareTextInCommitChanges = value.TypeConverter.CanConvertFrom(typeof(string));
            bool textHasChanged = false;
            if (compareTextInCommitChanges)
                textHasChanged = !value.DisplayString.Equals(Text);

            object oldValue = null;
            object newValue = null;
            bool multipleValues = value.HasMultipleValues;

            if (!compareTextInCommitChanges || textHasChanged)
            {
                // Store the previous value
                if (!multipleValues)
                    oldValue = value.GetValue();

                // Assign the new value based on the edit content
                try
                {
                    if (!multipleValues || (Text.Length != 0))
                        newValue = value.GetActualValue(Text);
                }
                catch (Exception e)
                {
                    _currentInvalidPropertyEnum = _ownerPropertyEnum;
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

            if ((multipleValues && textHasChanged) || ((newValue != null) && ((oldValue == null) || (newValue.Equals(oldValue) == false))) ||
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

                // Notify the properties control if the value has really changed
                if (changed == PropertyValue.ValueValidationResult.Validated)
                {
                    if (PropertyValue.IsErrorCode(_currentValueValidationResult))
                    {
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                            value.GetValue(), changed));
                    }

                    PropertyChangedEventArgs e = new PropertyChangedEventArgs(_ownerPropertyEnum);
                    e.SelectedIndex = _ownerPropertyEnum.Property.Value.ImageIndex;
                    _ownerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(e);
                }

                if (final)
                    _oldImageListIndex = _ownerPropertyEnum.Property.Value.ImageIndex;
                _currentValueValidationResult = changed;

                // Defreeze painting and redraw
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

                if (final)
                {
                    _oldImageListIndex = _ownerPropertyEnum.Property.Value.ImageIndex;
                    _initialValue = Text;
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
