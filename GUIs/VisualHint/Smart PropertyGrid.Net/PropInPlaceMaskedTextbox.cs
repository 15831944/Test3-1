// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Windows.Forms;
using System.Reflection;
using System.ComponentModel;
using System.Security.Permissions;
using System;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when a textbox with a mask is needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceMaskedTextbox : MaskedTextBox, IInPlaceControl
    {
        private PropertyValue.ValueValidationResult _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

        private PropertyEnumerator _currentInvalidPropertyEnum;

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

                if (Text != _ownerPropertyEnum.Property.Value.DisplayString)
                    Text = _ownerPropertyEnum.Property.Value.DisplayString;

                if (_ownerPropertyEnum.Property.Value.IsReadOnly(_ownerPropertyEnum) &&
                    (_ownerPropertyEnum.Property.ParentGrid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.ReadOnlyFeel))
                    ReadOnly = true;
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

                return cp;
            }
        }

        /// <summary>
        /// Initializes a new instance of the PropInPlaceMaskedTextbox class.
        /// </summary>
        public PropInPlaceMaskedTextbox()
        {
            // Because we want to be free to resize the textbox at will...
            AutoSize = false;
        }

        /// <summary>
        /// Gets or sets the font of the text displayed by the control.
        /// </summary>
        /// <value>The Font object to apply to the text displayed by the control.</value>
        public override System.Drawing.Font Font
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
                    Win32Calls.MakeLong(margin, margin));
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

/*        protected override void OnGotFocus(System.EventArgs e)
        {
            if ((_wmFocusMissingArg == null) || (_ownerPropertyEnum.Property.ParentGrid.Contains(_wmFocusMissingArg) == false))
                _ownerPropertyEnum.Property.ParentGrid.Invalidate(true);

            base.OnGotFocus(e);
        }
        */
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
        [UIPermission(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
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
                    SelectAll();
                    return true;
                }
            }
            else if (key == Keys.Tab)
            {
                PropertyValue.ValueValidationResult changed = CommitChanges(true);

                if (PropertyValue.IsErrorCode(changed) &&
                    (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                        PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                {
                    SelectAll();
                    return true;
                }
            }
            else if (key == Keys.Escape)
            {
                // Restore the old string in the edit box
                PropertyValue value = _ownerPropertyEnum.Property.Value;
                if (value != null)
                {
                    Text = value.DisplayString;

                    if (PropertyValue.IsErrorCode(_currentValueValidationResult))
                    {
                        _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;
                        _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                            new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                            value.GetValue(), _currentValueValidationResult));
                    }
                }
            }

            if (Focused)
                return base.ProcessDialogKey(keyData);
            else
                return true;
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

                DefWndProc(ref m);  // Avoids calling MaskedTextBox.WmSetFocus which resets the caret
                return;
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
                // If I don't intercept this message, the control flickers when it is multiline
                return;
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
            if (e.KeyCode == Keys.Delete)
            {
                Clear();
                Focus();

                return true;
            }

            return false;
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
            if (value != null)
            {
                string oldMask = Mask;
                Mask = "";
                string dislayedText = Text;
                Mask = oldMask;

                // Store the previous value
                object oldValue = null;
                if (value.HasMultipleValues == false)
                    oldValue = value.GetValue();

                // Assign the new value based on the edit content
                object newValue = null;
                try
                {
                    if ((value.HasMultipleValues == false) || (dislayedText.Length != 0))
                        newValue = value.GetActualValue(dislayedText);
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

                if (((newValue != null) && ((oldValue == null) || (newValue.Equals(oldValue) == false))) ||
                    ((newValue == null) && (oldValue != null)))
                {
                    if (MaskCompleted)
                    {
                        // Freeze painting because updating the property may also update other properties in the grid.
                        // If the value is invalid based on the validator, all updated properties will be reverted back to
                        // the previous value and a flicker would happen
                        _ownerPropertyEnum.Property.ParentGrid.BeginUpdate();

                        _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                        PropertyEnumerator invalidPropertyEnum;
                        changed = value.SetValueFromInPlaceCtrl(dislayedText, out invalidPropertyEnum);
                        if (changed == PropertyValue.ValueValidationResult.ValidatorFailed)
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
                    }
                    else
                    {
                        if (_ownerPropertyEnum.Property.ParentGrid.ValueNotValidBehaviorMode ==
                            PropertyGrid.ValueNotValidBehaviorModes.IgnoreAndRestore)
                        {
                            OwnerPropertyEnumerator.Property.ParentGrid.NotifyValueValidation(
                                new ValueValidationEventArgs(OwnerPropertyEnumerator, OwnerPropertyEnumerator,
                                newValue, PropertyValue.ValueValidationResult.ValidatorFailed));

                            Text = value.DisplayString;

                            _ownerPropertyEnum.Property.ParentGrid.NotifyValueValidation(
                                new ValueValidationEventArgs(OwnerPropertyEnumerator, OwnerPropertyEnumerator,
                                value.GetValue(), changed));
                        }
                        else
                        {
                            changed = PropertyValue.ValueValidationResult.ValidatorFailed;
                            _currentInvalidPropertyEnum = OwnerPropertyEnumerator;
                            PropertyValidatorBase.Message = "Incomplete mask";
                            OwnerPropertyEnumerator.Property.ParentGrid.NotifyValueValidation(
                                new ValueValidationEventArgs(OwnerPropertyEnumerator, _currentInvalidPropertyEnum,
                                    newValue, changed));
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
