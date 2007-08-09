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

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when a trackbar (with textbox or not) is needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceTrackbar : Control, IInPlaceControl
    {
        private PropertyGrid _parentGrid;

        private int _initialValue;

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

                if (_ownerPropertyEnum.Property.Value.Validator == null)
                    throw new Exception("A validator must be attached to a property that uses a trackbar.");
                
                _parentGrid = _ownerPropertyEnum.Property.ParentGrid;

                if (_edit != null)
                {
                    _edit.OwnerPropertyEnumerator = value;
//                    _edit.Text = _ownerPropertyEnum.Property.Value.DisplayString;
                }

                if (!ReadOnly)
                {
                    if (_trackbar == null)
                    {
                        _trackbar = new InternalTrackBar();
                        if (_edit != null)
                            _trackbar.TabStop = false;
                        _trackbar.Parent = this;
                        _trackbar.Scroll += new EventHandler(OnScroll);
                    }

                    _trackbar.OwnerPropertyEnumerator = _ownerPropertyEnum;

                    int trackValue = (int)_ownerPropertyEnum.Property.Value.GetValue();
                    try
                    {
                        _trackbar.Value = trackValue;
                    }
                    catch (ArgumentOutOfRangeException)
                    {
                        if (trackValue < _trackbar.Minimum)
                            _trackbar.Value = _trackbar.Minimum;
                        else
                            _trackbar.Value = _trackbar.Maximum;
                    }

                    Text = _trackbar.Value.ToString();
                    _initialValue = _trackbar.Value;

                    if (_realtimeChange)
                        CommitChanges(false);
                }
                else
                {
                    if (_trackbar != null)
                        _trackbar = null;

                    Text = _ownerPropertyEnum.Property.Value.GetValue().ToString();
                    if (_realtimeChange)
                        CommitChanges(false);
                }
            }

            get { return _ownerPropertyEnum; }
        }

        /// <summary>
        /// Because Control.OnLostFocus and Control.OnGotFocus doesn't provide a reference to the
        /// window that lost or gained the focus, this variable will contain this missing information.
        /// It is initialized in WndProc when processing WM_FILLFOCUS and WM_SETFOCUS messages.
        /// It is also embedded in a FocusEventArgs instance for event listeners.
        /// </summary>
        private Control _wmFocusMissingArg;

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

        private PropInPlaceTextbox _edit = null;

        private InternalTrackBar _trackbar = null;

        /// <summary>
        /// Gives access to the actual internal TrackBar used by this inplace control.
        /// </summary>
        public TrackBar TrackBar
        {
            get { return _trackbar; }
        }

        private bool _realtimeChange = false;

        public bool RealtimeChange
        {
            set { _realtimeChange = value; }
            get { return _realtimeChange; }
        }

        public bool ReadOnly
        {
            get { return _ownerPropertyEnum.Property.Value.IsReadOnly(_ownerPropertyEnum); }
        }

        public int LargeChange
        {
            get { return _trackbar.LargeChange; }
            set { _trackbar.LargeChange = value; }
        }

        public int SmallChange
        {
            get { return _trackbar.SmallChange; }
            set { _trackbar.SmallChange = value; }
        }

        public PropInPlaceTrackbar(bool editable)
        {
            TabStop = false;

            if (editable)
            {
                _edit = new PropInPlaceTextbox(false);
                _edit.Parent = this;
            }

            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque | ControlStyles.ResizeRedraw, true);

            Cursor = Cursors.Arrow;
        }

        protected override void OnPaint(PaintEventArgs pevent)
        {
            _parentGrid.DrawManager.DrawPropertyValueBackground(pevent.Graphics,
                ClientRectangle, _ownerPropertyEnum);

            if (_edit == null)
            {
                Rectangle textRect = new Rectangle(0, 0, GetEditWidth(), ClientRectangle.Height);
                _ownerPropertyEnum.Property.Value.DrawValue(pevent.Graphics, textRect,
                    _ownerPropertyEnum.Property.Value.ForeColor, _ownerPropertyEnum, Text);
            }
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

        private int GetEditWidth()
        {
            if (_trackbar != null)
            {
                Graphics graphics = CreateGraphics();
                int width1 = Win32Calls.GetTextExtent(graphics, _trackbar.Minimum.ToString(), Font).Width;
                int width2 = Win32Calls.GetTextExtent(graphics, _trackbar.Maximum.ToString(), Font).Width;
                graphics.Dispose();
                if (width1 < width2)
                    width1 = width2;

                return width1 + 2 * _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
            }
            else
                return ClientRectangle.Width;
        }

        public virtual void RepositionChildren()
        {
            Rectangle clientRect = ClientRectangle;
            if (clientRect.Width > 0)
            {
                if (_edit != null)
                {
                    // Get the margins of the editbox
                    int rawMargins = Win32Calls.LoWord(Win32Calls.SendMessage(_edit.Handle, Win32Calls.EM_GETMARGINS, 0, 0));
                    int margins = Win32Calls.LoWord(rawMargins) + Win32Calls.HiWord(rawMargins);

                    int margin = _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
                    int leftPos = (_ownerPropertyEnum.Property.Value.EditboxLeftMargin == -1 ? 0 :
                        margin - _ownerPropertyEnum.Property.Value.EditboxLeftMargin);

                    Rectangle editRect = new Rectangle(leftPos, 0, GetEditWidth() - leftPos, clientRect.Height);
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

                    if (_trackbar != null)
                    {
                        Rectangle trackbarRect = new Rectangle(editRect.Right, 0+2, clientRect.Right - editRect.Right, clientRect.Height-2);
                        if (trackbarRect.Width < 15)
                            _trackbar.Visible = false;
                        else
                        {
                            _trackbar.Visible = true;
                            _trackbar.Bounds = trackbarRect;
                        }
                    }
                }
                else
                {
                    if (_trackbar != null)
                    {
                        int editWidth = GetEditWidth();
                        Rectangle trackbarRect = new Rectangle(editWidth, 0, clientRect.Width - editWidth, clientRect.Height);
                        _trackbar.Bounds = trackbarRect;
                    }
                }
            }
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            RepositionChildren();
        }

        void OnScroll(object sender, EventArgs e)
        {
            Text = _trackbar.Value.ToString();
            if (_realtimeChange)
                CommitChanges(false);
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

        protected override void OnGotFocus(EventArgs e)
        {
            if (_edit != null)
            {
                if (_wmFocusMissingArg != _edit)
                    _edit.Focus();
                else
                    Parent.Focus();
            }
            else if ((_trackbar != null) && (_wmFocusMissingArg != _trackbar))
                _trackbar.Focus();
            else
                Parent.Focus();

            base.OnGotFocus(e);
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if ((_edit != null) && (Focused == false))
                Win32Calls.PostMessage(_edit.Handle, Win32Calls.WM_CHAR, e.KeyChar, 0);
            else
                base.OnKeyPress(e);
        }

#if _DOTNET2
        [UIPermission(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
#endif
		protected override bool ProcessDialogKey(Keys keyData)
        {
            Keys key = (keyData & Keys.KeyCode);

            if (key == Keys.Return)
            {
                if ((_trackbar != null) && _trackbar.Focused)
                {
                    CommitChanges(true);
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
            else if (key == Keys.Tab)
            {
                PropertyValue.ValueValidationResult changed = CommitChanges(_trackbar.Focused);

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
                        if ((_parentGrid.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys) ||
                            ((_parentGrid.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyInSubControls) != 0))
                        {
                            _trackbar.Focus();
                            return true;
                        }
                    }
                    else if (forward && _trackbar.Focused)
                    {
                        if (_realtimeChange == false)
                            CommitChanges(true);
                    }
                    else if (!forward && _trackbar.Focused)
                    {
                        _edit.Focus();
                        _edit.SelectAll();
                        Invalidate(false);
                        return true;
                    }
                }
            }
            else if (key == Keys.Escape)
            {
                // Restore the old value in the textbox
                _trackbar.Value = _initialValue;
                Text = _trackbar.Value.ToString();
                CommitChanges(true);
            }

            if (ContainsFocus)
                return base.ProcessDialogKey(keyData);
            else
                return true;
        }

        protected override bool ProcessKeyPreview(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_KEYDOWN)
            {
                Keys key = (Keys)(int)m.WParam;
                if ((key == Keys.Down) || (key == Keys.Up) || (key == Keys.PageDown) || (key == Keys.PageUp))
                {
                    if ((_edit != null) && _edit.Focused)
                    {
                        // Check to see if the current displayed value is valid regarding its TypeConverter
                        object valueToValidate = null;
                        try
                        {
                            valueToValidate = _ownerPropertyEnum.Property.Value.GetValueToValidate(Text);
                        }
                        catch (Exception e)
                        {
                            _edit.SelectAll();
                            _currentValueValidationResult = PropertyValue.ValueValidationResult.TypeConverterFailed;
                            _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                                new ValueValidationEventArgs(OwnerPropertyEnumerator, OwnerPropertyEnumerator,
                                Text, PropertyValue.ValueValidationResult.TypeConverterFailed, e));
                            return true;
                        }

                        int newValue = (int)valueToValidate;
                        if (key == Keys.Down)
                            newValue -= _trackbar.SmallChange;
                        else if (key == Keys.Up)
                            newValue += _trackbar.SmallChange;
                        else if (key == Keys.PageDown)
                            newValue -= _trackbar.LargeChange;
                        else if (key == Keys.PageUp)
                            newValue += _trackbar.LargeChange;

                        // Check to see if the current displayed value is valid regarding its validator
                        PropertyValidatorBase validator = _ownerPropertyEnum.Property.Value.Validator;
                        if (validator != null)
                        {
                            bool newValueValid = validator.Check(newValue, false);

                            if (!validator.Check(valueToValidate, false))
                            {
                                // If the current value is invalid, let's see if the new one will be valid
                                if (newValueValid == false)
                                {
                                    _currentInvalidPropertyEnum = OwnerPropertyEnumerator;
                                    _edit.SelectAll();
                                    _currentValueValidationResult = PropertyValue.ValueValidationResult.ValidatorFailed;
                                    _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                                        valueToValidate, _currentValueValidationResult));
                                    return true;
                                }
                            }
                        }

                        if (newValue != _trackbar.Value)
                        {
                            if (newValue > _trackbar.Maximum)
                                newValue = _trackbar.Maximum;
                            else if (newValue < _trackbar.Minimum)
                                newValue = _trackbar.Minimum;

                            _trackbar.Value = newValue;

                            Text = newValue.ToString();
                            _edit.SelectAll();
                            if (_realtimeChange)
                                CommitChanges(false);

                            return true;
                        }
                    }
                }
            }

            return base.ProcessKeyPreview(ref m);
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

        public bool OnForbidClickThrough(Point pt)
        {
            if (_trackbar == null)
                return false;

            return _trackbar.Bounds.Contains(pt);
        }

        public void DoubleClickFromParent(Point pt) { }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            if (forward)
                return _trackbar.Focused;
            else
            {
                if (_edit == null)
                    return _trackbar.Focused;
                else
                    return _edit.Focused;
            }
        }

        public virtual bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force)
        {
            return false;
        }

        public TextBox TextBox
        {
            get { return _edit; }
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;

            if ((_edit != null) && _edit.ReadOnly)
                return changed;

            PropertyValue value = _ownerPropertyEnum.Property.Value;

            // Store the previous value
            object oldValue = null;
            if (value.HasMultipleValues == false)
                oldValue = value.GetValue();

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
                _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                PropertyEnumerator invalidPropertyEnum;
                changed = value.SetValueFromInPlaceCtrl(Text, out invalidPropertyEnum);

                if (PropertyValue.IsErrorCode(changed))
                    _currentInvalidPropertyEnum = invalidPropertyEnum;

                _currentValueValidationResult = changed;

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
                        _initialValue = _trackbar.Value;
                }

                // If value is invalid, previous value is restored if ValueNotValidBehaviorMode is set accordingly
                if (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore)
                {
                    if (PropertyValue.IsErrorCode(changed))
                    {
                        _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                            value.GetValue(), _currentValueValidationResult));
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

                    _trackbar.Value = (int)newValue;

                    _parentGrid.NotifyPropertyChanged(new PropertyChangedEventArgs(_ownerPropertyEnum));
                }

                Parent.Refresh();
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
            return null;
        }
    }
}
