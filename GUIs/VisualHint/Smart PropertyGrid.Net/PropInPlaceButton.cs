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
using Skybound.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when a button (with textbox or not) is needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceButton : Control, IInPlaceControl
    {
        protected PropInPlaceTextbox mEdit = null;

        private bool _dontTransferFocus = false;

        private PropertyValue.ValueValidationResult _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

        protected PropertyEnumerator mOwnerPropertyEnum;

        public PropertyEnumerator OwnerPropertyEnumerator
        {
            set
            {
                if (!Visible)
                    _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

                mOwnerPropertyEnum = (PropertyEnumerator)value.Clone();

                Text = mOwnerPropertyEnum.Property.Value.DisplayString;

                if (mEdit != null)
                {
                    mEdit.OwnerPropertyEnumerator = value;
//                    mEdit.Text = Text;
                }
            }

            get { return mOwnerPropertyEnum; }
        }

        public bool ReadOnly
        {
            get { return mOwnerPropertyEnum.Property.Value.IsReadOnly(mOwnerPropertyEnum); }
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

        public PropInPlaceButton(bool editable)
        {
            if (editable)
            {
                mEdit = new PropInPlaceTextbox(false);
                mEdit.Parent = this;
                mEdit.Text = Text;
                mEdit.TabStop = false;
            }

            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque | ControlStyles.ResizeRedraw, true);

            Cursor = Cursors.Arrow;
        }

        private string _buttonText = "...";

        public string ButtonText
        {
            set { _buttonText = value; RepositionChildren(); }
            get { return _buttonText; }
        }

        public override Font Font
        {
            get { return base.Font; }
            set
            {
                base.Font = value;
                if (mEdit != null)
                    mEdit.Font = value;
            }
        }

        protected int GetButtonWidth(Graphics graphics)
        {
            if (ReadOnly)
                return 0;

            Graphics graphics2 = (graphics != null ? graphics : CreateGraphics());
            int width = Size.Round(graphics2.MeasureString(_buttonText, Font)).Width + 6;

            if (graphics == null)
                graphics2.Dispose();

            return width;
        }

        protected Rectangle GetButtonRect(Graphics graphics)
        {
            int buttonWidth = GetButtonWidth(graphics);
            return new Rectangle(Width - buttonWidth, 0, buttonWidth, Height);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            // Draw the background

            mOwnerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(e.Graphics,
                ClientRectangle, mOwnerPropertyEnum);

            Rectangle buttonRect = GetButtonRect(e.Graphics);

            if (!ReadOnly)
            {
				if (ThemeRenderer.Enabled)
				{
                    int state = (_pushed ?
                        (_mouseOver ? ThemeButton.PushButtonPressed : ThemeButton.PushButtonHot) :
                        (_mouseOver ? ThemeButton.PushButtonHot : ThemeButton.PushButtonNormal));
					ThemeButton.PushButton.Draw(e.Graphics, state, buttonRect);

					Win32Calls.DrawText(e.Graphics, _buttonText, ref buttonRect, Font,
                        OwnerPropertyEnumerator.Property.Value.ForeColor, Win32Calls.DT_SINGLELINE |
                        Win32Calls.DT_VCENTER | Win32Calls.DT_CENTER);
                }
                else
                {
                    ControlPaint.DrawButton(e.Graphics, buttonRect, (_pushed && _mouseOver) ? ButtonState.Pushed : ButtonState.Normal);
                    StringFormat stringFormat = (StringFormat)StringFormat.GenericDefault.Clone();
                    stringFormat.Alignment = StringAlignment.Center;
                    stringFormat.LineAlignment = StringAlignment.Center;
                    Brush brush = new SolidBrush(Color.Black);
                    e.Graphics.DrawString(_buttonText, Font, brush, buttonRect, stringFormat);
                    brush.Dispose();
                }

                if (Focused)
                {
                    Rectangle focusRect = buttonRect;
                    focusRect.Inflate(-3, -3);
                    ControlPaint.DrawFocusRectangle(e.Graphics, focusRect);
                }
            }

            // Draw the value
            if (mEdit == null)
            {
                Rectangle valueRect = ClientRectangle;
                valueRect.Width -= buttonRect.Width + 1;

                Color valueColor;
                if (mOwnerPropertyEnum.Property.Enabled == false)
                    valueColor = SystemColors.GrayText;
                else
                    valueColor = mOwnerPropertyEnum.Property.Value.ForeColor;

                mOwnerPropertyEnum.Property.Value.DrawValue(e.Graphics, valueRect, valueColor, mOwnerPropertyEnum, null);
            }

            base.OnPaint(e);
        }

        public virtual void RepositionChildren()
        {
            if (mEdit != null)
            {
                Rectangle clientRect = ClientRectangle;
                if (clientRect.Width > 0)
                {
                    // Get the margins of the editbox
                    int rawMargins = Win32Calls.LoWord(Win32Calls.SendMessage(mEdit.Handle, Win32Calls.EM_GETMARGINS, 0, 0));
                    int margins = Win32Calls.LoWord(rawMargins) + Win32Calls.HiWord(rawMargins);

                    int margin = mOwnerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
                    int leftPos = (mOwnerPropertyEnum.Property.Value.EditboxLeftMargin == -1 ? 0 :
                        margin - mOwnerPropertyEnum.Property.Value.EditboxLeftMargin);

                    int buttonWidth = GetButtonWidth(null);
                    Rectangle editRect = new Rectangle(leftPos, 0, clientRect.Width - buttonWidth - leftPos, clientRect.Height);
                    editRect.Y++;
                    editRect.Height--;

                    // The width of the editbox can't be smaller than the margins or the margins would be void and the text misplaced
                    if (editRect.Width <= margins)
                    {
                        if (mEdit.Focused)
                            Parent.Focus();

                        mEdit.Visible = false;
                    }
                    else
                    {
                        mEdit.Visible = true;

                        Win32Calls.TEXTMETRIC tm = new Win32Calls.TEXTMETRIC();
                        Graphics graphics = CreateGraphics();
                        Win32Calls.GetTextMetrics(graphics, Font, ref tm);
                        graphics.Dispose();
                        int extraHeight = editRect.Height - tm.tmHeight;
                        editRect.Y += extraHeight / 2;
                        editRect.Height -= extraHeight / 2;

                        mEdit.Invalidate();
                        mEdit.Bounds = editRect;
                    }
                }
            }

            Invalidate(false);

//            mEdit.Location = new Point(0, 0);
//          mEdit.Size = new Size(ClientRectangle.Width - SystemInformation.VerticalScrollBarWidth, ClientRectangle.Height);
        }

        protected override void OnMove(EventArgs e)
        {
            base.OnMove(e);
            RepositionChildren();
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            RepositionChildren();
        }

        protected override void OnGotFocus(EventArgs e)
        {
            if ((mEdit != null) && mEdit.Visible)
            {
                if (_dontTransferFocus == false)
                {
                    if (_wmFocusMissingArg != mEdit)
                        mEdit.Focus();
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

            base.WndProc(ref m);
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if ((mEdit != null) && (Focused == false))
                Win32Calls.PostMessage(mEdit.Handle, Win32Calls.WM_CHAR, e.KeyChar, 0);

            base.OnKeyPress(e);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                Graphics graphics = CreateGraphics();
                if (GetButtonRect(graphics).Contains(new Point(e.X, e.Y)))
                {
                    _pushed = true;
                    Capture = true;
                    Invalidate(false);
                }
                graphics.Dispose();

                _dontTransferFocus = true;
                Focus();
                _dontTransferFocus = false;
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
                    {
                        // Maybe the user typed something in the editbox that has not been validated yet
                        if (mEdit != null)
                        {
                            if (!PropertyValue.IsErrorCode(mEdit.CommitChanges(false)))
                                RunButton();
                            else
                            {
                                mEdit.SelectAll();
                                mEdit.Focus();
                            }
                        }
                        else
                            RunButton();
                    }
                }
            }

            base.OnMouseUp(e);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if ((e.KeyData == Keys.Return) || (e.KeyData == Keys.Space))
            {
                _mouseOver = true;
                _pushed = true;
                Invalidate(false);
            }

            base.OnKeyDown(e);
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            if ((e.KeyData == Keys.Return) || (e.KeyData == Keys.Space))
            {
                if (_pushed)
                {
                    _mouseOver = false;
                    _pushed = false;
                    Invalidate(false);

                    RunButton();
                }
            }

            base.OnKeyUp(e);
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if (keyData == Keys.Return)
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
                if ((mEdit != null) && mEdit.Focused)
                {
                    PropertyValue.ValueValidationResult changed = CommitChanges(true);

                    if (PropertyValue.IsErrorCode(changed) &&
                        (mOwnerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                    {
                        mEdit.SelectAll();
                        return true;
                    }
                }
            }
            else if (key == Keys.F4)
            {
                if ((mEdit != null) && mEdit.Focused)
                {
                    PropertyValue.ValueValidationResult changed = CommitChanges(false);

                    if (!PropertyValue.IsErrorCode(changed) ||
                        (mOwnerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore))
                    {
                        OnF4();
                    }
                    else
                        mEdit.SelectAll();
                }
                else
                    OnF4();

                return true;
            }
            else if (key == Keys.Escape)
            {
                if ((mEdit != null) && mEdit.Focused)
                {
                    _pushed = false;

                    PropertyValue value = mOwnerPropertyEnum.Property.Value;
                    Text = value.DisplayString;

                    if (PropertyValue.IsErrorCode(_currentValueValidationResult))
                    {
                        _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;
                        mOwnerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, mEdit.CurrentInvalidPropertyEnum,
                            value.GetValue(), _currentValueValidationResult));
                    }
                }
            }
            else if (key == Keys.Tab)
            {
                if ((mEdit != null) && !ReadOnly)
                {
                    bool forward = ((keyData & Keys.Shift) == Keys.None);

                    PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;
                    if (mEdit.Focused)
                    {
                        bool final = !forward && (mOwnerPropertyEnum.Property.ParentGrid.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys);
                        changed = CommitChanges(final);

                        if (PropertyValue.IsErrorCode(changed) &&
                            (mOwnerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                        {
                            mEdit.SelectAll();
                            return true;
                        }

                        if (forward)
                        {
                            if ((mOwnerPropertyEnum.Property.ParentGrid.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys) ||
                                ((mOwnerPropertyEnum.Property.ParentGrid.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyInSubControls) != 0))
                            {
//                                CommitChanges(false);
                                _dontTransferFocus = true;
                                Focus();
                                Invalidate(false);
                                _dontTransferFocus = false;
                                return true;
                            }
                        }
                        else
                        {
                            if (mOwnerPropertyEnum.Property.ParentGrid.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys)
                            {
//                                CommitChanges(true);
                                Parent.Focus();
                                return true;
                            }
                        }
                    }
                    else if (!forward && Focused)
                    {
                        mEdit.Focus();
                        mEdit.SelectAll();
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

        public override Color BackColor
        {
            get { return base.BackColor; }
            set
            {
                base.BackColor = value;

                if (mEdit != null)
                    mEdit.BackColor = value;
            }
        }

        public override Color ForeColor
        {
            get { return base.ForeColor; }
            set
            {
                base.ForeColor = value;

                if (mEdit != null)
                    mEdit.ForeColor = value;
            }
        }

        protected virtual void RunButton()
        {
            Parent.Focus();

            PropertyButtonClickedEventArgs eventArgs = new PropertyButtonClickedEventArgs(mOwnerPropertyEnum);
            mOwnerPropertyEnum.Property.Value.PreviousValue = mOwnerPropertyEnum.Property.Value.GetValue();

            mOwnerPropertyEnum.Property.ParentGrid.OnPropertyButtonClicked(eventArgs);

            if (eventArgs.PropertyChanged)
            {
                if (mEdit != null)
                    mEdit.Text = mOwnerPropertyEnum.Property.Value.DisplayString;

                mOwnerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(new PropertyChangedEventArgs(mOwnerPropertyEnum));
            }

            mOwnerPropertyEnum.Property.ParentGrid.OnInPlaceCtrlFinishedEdition();
        }

        public bool OnForbidClickThrough(Point pt)
        {
            Graphics graphics = CreateGraphics();
            bool result = GetButtonRect(graphics).Contains(pt);
            graphics.Dispose();

            return result;
        }

        public void OnF4()
        {
            if (!ReadOnly)
                RunButton();
        }

        public bool OnParentKeyDown(KeyEventArgs e)
        {
            if ((mEdit != null) && mEdit.OnParentKeyDown(e))
                return true;

            return false;
        }

        public bool RealtimeChange
        {
            set { }
            get { return true; }
        }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            if (forward)
            {
                if ((mEdit == null) || ReadOnly)
                    return true;
                else
                    return Focused;
            }
            else
            {
                if (mEdit == null)
                    return true;
                else
                    return mEdit.Focused;
            }
        }

        public virtual bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force)
        {
            return false;
        }

        public TextBox TextBox
        {
            get { return mEdit; }
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            if (mEdit != null)
            {
                _currentValueValidationResult = mEdit.CommitChanges(final);
                return _currentValueValidationResult;
            }

            _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;
            return _currentValueValidationResult;
        }

        public Control GetDropDownContainer()
        {
            return null;
        }
    }
}
