// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Windows.Forms;
using System.Drawing;
using System.Reflection;
using System.ComponentModel;
using System.Security.Permissions;
using System.Globalization;
using System;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when a simple textbox is needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceTextbox : TextBox, IInPlaceControl
    {
        private bool _isPassword;

        private PropertyValue.ValueValidationResult _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

        private PropertyEnumerator _currentInvalidPropertyEnum;

        public PropertyEnumerator CurrentInvalidPropertyEnum
        {
            get { return _currentInvalidPropertyEnum; }
        }
        
        /// <summary>
        /// An enumerator on the underlying Property.
        /// </summary>
        private PropertyEnumerator _ownerPropertyEnum;

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

                if (PasswordChar != 0)
                {
                    if (Text != _ownerPropertyEnum.Property.Value.GetStringValue())
                        Text = _ownerPropertyEnum.Property.Value.GetStringValue();
                }
                else
                {
                    if (Text != _ownerPropertyEnum.Property.Value.DisplayString)
                        Text = _ownerPropertyEnum.Property.Value.DisplayString;
                }

                if ((_ownerPropertyEnum.Property.Value.IsReadOnly(_ownerPropertyEnum) &&
                    (_ownerPropertyEnum.Property.ParentGrid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.ReadOnlyFeel)) ||
                    _ownerPropertyEnum.Property.Value.ReadOnlyByTypeConverter)
                    ReadOnly = true;
                else
                    ReadOnly = false;
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

        /// <summary>
        /// CreateParams is overriden to remove the nonclient border.
        /// </summary>
        /// <value>A CreateParams object that contains the required creation parameters when the handle
        /// to the control is created.</value>
        protected override CreateParams CreateParams
        {
            [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
            get
            {
                CreateParams cp = base.CreateParams;

                // The border of the editbox must be removed
                cp.ExStyle &= ~Win32Calls.WS_EX_CLIENTEDGE;

                if (_isPassword)
                    cp.Style |= 0x0020;

                return cp;
            }
        }

        /// <summary>
        /// Initializes a new instance of the PropInPlaceTextbox class.
        /// </summary>
        public PropInPlaceTextbox(bool isPassword)
        {
            // Because we want to be free to resize the textbox at will...
            AutoSize = false;

            _isPassword = isPassword;
        }

        /// <summary>
        /// Gets or sets the font of the text displayed by the control.
        /// </summary>
        /// <value>The Font object to apply to the text displayed by the control.</value>
        public override Font Font
        {
            get { return base.Font; }
            set
            {
                base.Font = value;
                
                int margin = (_ownerPropertyEnum.Property.Value.EditboxLeftMargin != -1 ?
                    _ownerPropertyEnum.Property.Value.EditboxLeftMargin :
                    _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin);
                Win32Calls.SendMessage(Handle, Win32Calls.EM_SETMARGINS,
                    Win32Calls.EC_LEFTMARGIN | Win32Calls.EC_RIGHTMARGIN,
                    Win32Calls.MakeLong(margin, 0));
/*                
                // When the Font of the PropertyGrid is assigned to this control, its margins must be recalculated.
                // First they are set corresponding to the font info.
                Win32Calls.SendMessage(Handle, Win32Calls.EM_SETMARGINS,
                    Win32Calls.EC_LEFTMARGIN | Win32Calls.EC_RIGHTMARGIN,
                    Win32Calls.MakeLong(Win32Calls.EC_USEFONTINFO, Win32Calls.EC_USEFONTINFO));

                // Then 2 pixels are added to the left margin.
                int adjustedMargin = 2 + Win32Calls.LoWord(Win32Calls.SendMessage(Handle, Win32Calls.EM_GETMARGINS, 0, 0));
                Win32Calls.SendMessage(Handle, Win32Calls.EM_SETMARGINS,
                    Win32Calls.EC_LEFTMARGIN | Win32Calls.EC_RIGHTMARGIN,
                    Win32Calls.MakeLong(adjustedMargin, 0));
                */
                // This makes a pleasant reading of the content of the PropertyGrid.
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="e"></param>
        protected override void OnGotFocus(System.EventArgs e)
        {
            Cursor = Cursors.IBeam;

            if ((_wmFocusMissingArg == null) || (_ownerPropertyEnum.Property.ParentGrid.Contains(_wmFocusMissingArg) == false))
                _ownerPropertyEnum.Property.ParentGrid.Invalidate(true);

            base.OnGotFocus(e);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="e"></param>
        protected override void OnLostFocus(System.EventArgs e)
        {
            Select(0, 0);
            ScrollToCaret();

            base.OnLostFocus(e);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="keyData"></param>
        /// <returns></returns>
#if _DOTNET2
        [UIPermission(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
#endif
		protected override bool ProcessDialogKey(Keys keyData)
        {
            Keys key = (keyData & Keys.KeyCode);

            if (((key == Keys.Return) && !Multiline) || (Multiline && (keyData == Keys.Return)))
            {
                if (Parent is IInPlaceControl == false)
                {
                    PropertyValue.ValueValidationResult changed = CommitChanges(true);

                    if (PropertyValue.IsErrorCode(changed) &&
                        (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                    {
                        SelectAll();
                        return true;
                    }
                }
            }
            else if (key == Keys.Tab)
            {
                if (Parent is IInPlaceControl == false)
                {
                    PropertyValue.ValueValidationResult changed = CommitChanges(true);

                    if (PropertyValue.IsErrorCode(changed) &&
                        (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                    {
                        SelectAll();
                        return true;
                    }
                }
            }
            else if (key == Keys.Escape)
            {
                // Restore the old string in the textbox
                if (_isPassword)
                    Text = _ownerPropertyEnum.Property.Value.DisplayStringWithoutLook;
                else
                    Text = _ownerPropertyEnum.Property.Value.DisplayString;

                if (Parent is IInPlaceControl == false)
                    CommitChanges(true);
            }

            if (Focused)
                return base.ProcessDialogKey(keyData);
            else
                return true;
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            Keys key = keyData & Keys.KeyCode;

            if (key == Keys.A)
            {
                if ((((keyData & Keys.Control) != Keys.None) && ((keyData & Keys.Shift) == Keys.None)) && ((keyData & Keys.Alt) == Keys.None))
                {
                    SelectAll();
                    return true;
                }
            }
            else if ((key == Keys.C) || (key == Keys.V) || (key == Keys.X))
            {
                if ((((keyData & Keys.Control) != Keys.None) && ((keyData & Keys.Shift) == Keys.None)) && ((keyData & Keys.Alt) == Keys.None))
                    return false;
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="m"></param>
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
                if (Multiline)
                {
                    // If I don't intercept this message, the control flickers when it is multiline
                    // Some code must also be added in WM_PAINT
                    return;
                }
            }
            else if (m.Msg == Win32Calls.WM_PAINT)
            {
                if (Multiline)
                {
                    Graphics g = CreateGraphics();
                    Win32Calls.RECT rect = new Win32Calls.RECT();
                    Win32Calls.GetUpdateRect(Handle, out rect, false);
                    g.FillRectangle(new SolidBrush(BackColor), Rectangle.FromLTRB(rect.Left,rect.Top,rect.Right,rect.Bottom));
                    g.Dispose();
                }
            }
            else if (m.Msg == Win32Calls.WM_MOUSEACTIVATE)
            {
                if (Focused == false)
                {
                    if (Form.ActiveForm is PropInPlaceUITypeEditor.DropDownForm)
                        Focus();
                }
            }

            base.WndProc(ref m);
        }

        public void OnF4()
        {
            Focus();
            SelectAll();
        }

        public bool OnParentKeyDown(KeyEventArgs e)
        {
            if ((e.KeyCode == Keys.Delete) && !ReadOnly)
            {
                Clear();
                Focus();

                return true;
            }

            return false;
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if (ReadOnly && !OwnerPropertyEnumerator.Property.Value.ReadOnly)
            {
                // When typing a letter, we try to find the next string after the selected one
                // that begins by this letter

                string[] displayedStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                char[] chArray = new char[1] { e.KeyChar };
                string letter = new string(chArray);

                for (int i = 0; i < displayedStrings.Length; i++)
                {
                    if (_ownerPropertyEnum.Property.Value.DisplayString.Equals(displayedStrings[i]))
                    {
                        for (int j = 0; j < displayedStrings.Length; j++)
                        {
                            string currentStr = displayedStrings[((i + j) + 1) % displayedStrings.Length];
                            if ((currentStr.Length > 0) &&
                                (string.Compare(currentStr.Substring(0, 1), letter, true, CultureInfo.InvariantCulture) == 0))
                            {
                                Text = currentStr;
                                SelectAll();

                                if (Parent is IInPlaceControl)
                                {
                                    if ((Parent as IInPlaceControl).RealtimeChange)
                                        CommitChanges(false);
                                }
                                else
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

        public bool RealtimeChange
        {
            set { }
            get { return true; }
        }

        public virtual void RepositionChildren() { }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            return true;
        }

        public virtual bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force)
        {
            return false;
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (e.Clicks == 2)
            {
                if (Parent is IInPlaceControl)
                    (Parent as IInPlaceControl).ScrollValueOnDoubleClick(!(ModifierKeys == Keys.Shift),
                        ScrollValueOnDoubleClickSource.FromTextBox, false);
            }

            base.OnMouseDown(e);
        }

        public TextBox TextBox
        {
            get { return this; }
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;

            if (ReadOnly)
                return changed;

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
                        new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                        Text, PropertyValue.ValueValidationResult.TypeConverterFailed, e));

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
                string str = Text;

                // Freeze painting because updating the property may also update other properties in the grid.
                // If the value is invalid based on the validator, all updated properties will be reverted back to
                // the previous value and a flicker would happen
                _ownerPropertyEnum.Property.ParentGrid.BeginUpdate();

                _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                PropertyEnumerator invalidPropertyEnum;
                changed = value.SetValueFromInPlaceCtrl(str, out invalidPropertyEnum);

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
                    if (PasswordChar == 0)
                        Text = value.DisplayString;
                    else
                        Text = value.GetStringValue();
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
            return null;
        }
    }
}
