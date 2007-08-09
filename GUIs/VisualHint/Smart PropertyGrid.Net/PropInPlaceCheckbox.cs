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
using System.Collections;
using System.Reflection;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when one or more checkboxes are needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceCheckbox : Control, IInPlaceControl
    {
        private class NullableBoolean : ICloneable
        {
            private bool _value;

            private bool _isNull;

            public bool Value
            {
                set
                {
                    _isNull = false;
                    _value = value;
                }

                get
                {
                    if (_isNull)
                        throw new NullReferenceException("The nullable boolean is null");

                    return _value;
                }
            }

            public bool IsNull
            {
                get { return _isNull; }
                set { _isNull = value; }
            }

            public override string ToString()
            {
                if (_isNull)
                    throw new NullReferenceException("The nullable boolean is null");

                return _value.ToString();
            }

            public object Clone()
            {
                NullableBoolean nb = new NullableBoolean();
                nb._isNull = this.IsNull;
                if (!nb._isNull)
                    nb._value = this.Value;

                return nb;
            }
        }

        /// <summary>
        /// Because Control.OnLostFocus and Control.OnGotFocus doesn't provide a reference to the
        /// window that lost or gained the focus, this variable will contain this missing information.
        /// It is initialized in WndProc when processing WM_FILLFOCUS and WM_SETFOCUS messages.
        /// It is also embedded in a FocusEventArgs instance for event listeners.
        /// </summary>
        private Control _wmFocusMissingArg;

        private int _focusIndex = 0;

        /// <summary>
        /// Caches the enum data displayed by this control.
        /// </summary>
        private Enum _enumValue;

        private Enum _initialEnumValue;

        /// <summary>
        /// Caches the boolean data displayed by this control.
        /// </summary>
        private NullableBoolean _booleanValue = null;

        private NullableBoolean _initialBooleanValue;

        /// <summary>
        /// Caches the collection data displayed by this control.
        /// </summary>
        private ArrayList _collectionValue;

        private ArrayList _initialCollectionValue;

        private bool _multipleValues;

        private bool _initialMultipleValues;

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
                _ownerPropertyEnum = (PropertyEnumerator)value.Clone();

                if (IsBooleanValue)
                {
                    if (_booleanValue == null)
                        _booleanValue = new NullableBoolean();

                    // The initial boolean value is stored in an internal cache
                    if (_ownerPropertyEnum.Property.Value.HasMultipleValues)
                        _booleanValue.IsNull = true;
                    else
                        _booleanValue.Value = (bool)_ownerPropertyEnum.Property.Value.GetValue();

                    _initialBooleanValue = (NullableBoolean)_booleanValue.Clone();
                }
                else if (IsEnumValue)
                {
                    if (Attribute.GetCustomAttribute(_ownerPropertyEnum.Property.Value.UnderlyingType, typeof(FlagsAttribute)) == null)
                        throw new Exception("Checkboxes for Enums must be linked to a \"flagged\" enumeration.");

                    // The initial enum value is stored in an internal cache
                    string strValue = "0";
                    bool hasMultipleValues = _ownerPropertyEnum.Property.Value.HasMultipleValues;
                    _multipleValues = hasMultipleValues;
                    if (hasMultipleValues == false)
                        strValue = _ownerPropertyEnum.Property.Value.GetStringValue();
                    _enumValue = (Enum)_ownerPropertyEnum.Property.Value.TypeConverter.ConvertFromString(
                        _ownerPropertyEnum.Property.Value.GetTypeDescriptorContext(_ownerPropertyEnum),
                        _ownerPropertyEnum.Property.Value.CultureInfo, strValue);

                    _initialEnumValue = _enumValue;
                }
                else if (_ownerPropertyEnum.Property.Value.UnderlyingValue is ArrayList)
                {
                    if (_ownerPropertyEnum.Property.Value.HasMultipleValues)
                    {
                        _collectionValue = new ArrayList();
                        _multipleValues = true;
                    }
                    else
                        _collectionValue = (ArrayList)((ArrayList)_ownerPropertyEnum.Property.Value.UnderlyingValue).Clone();

                    _initialCollectionValue = (ArrayList)_collectionValue.Clone();
                }

                _initialMultipleValues = _multipleValues;
            }

            get { return _ownerPropertyEnum; }
        }

        private bool _realtimeChange = true;

        public bool RealtimeChange
        {
            set { _realtimeChange = value; }
            get { return _realtimeChange; }
        }

        public PropInPlaceCheckbox()
        {
            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque | ControlStyles.ResizeRedraw, true);

            Cursor = Cursors.Arrow;
        }

        private bool IsBooleanValue
        {
            get { return _ownerPropertyEnum.Property.Value.UnderlyingValue is Boolean; }
        }

        private bool IsEnumValue
        {
            get { return _ownerPropertyEnum.Property.Value.UnderlyingType.IsEnum; }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            // We must override OnPaint instead of letting the look class do the job because
            // of the focus rectangle to draw

            Rectangle clientRect = ClientRectangle;
            clientRect.Height++;

            // Fill the background
            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(e.Graphics,
                clientRect, _ownerPropertyEnum);

            int margin = _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
            clientRect.X += margin - 1;
            clientRect.Width -= margin - 1;

            Color valueColor;
            if (_ownerPropertyEnum.Property.Enabled == false)
                valueColor = SystemColors.GrayText;
            else
                valueColor = _ownerPropertyEnum.Property.Value.ForeColor;

            bool hasMultipleValues = _ownerPropertyEnum.Property.Value.HasMultipleValues;

            if (IsBooleanValue)
            {
                Rectangle checkboxRect = clientRect;

                checkboxRect.Height -= 4;
                checkboxRect.Y += 2;
                checkboxRect.Width = checkboxRect.Height;

                // Draw checkbox
                if (_booleanValue.IsNull)
                    _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawCheckBoxIndeterminate(e.Graphics,
                        _ownerPropertyEnum.Property.ParentGrid.Grid, checkboxRect, _ownerPropertyEnum.Property.Enabled);
                else
                {
                    _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawCheckBox(e.Graphics, this, checkboxRect,
                        _booleanValue.Value, _ownerPropertyEnum.Property.Enabled);
                    
                    Rectangle textRect = clientRect;
                    textRect.X = checkboxRect.Right + margin;
                    textRect.Width = clientRect.Right - textRect.Left;

                    // Draw corresponding text
                    string str = _ownerPropertyEnum.Property.Value.GetDisplayString(_booleanValue.ToString());
                    Win32Calls.DrawText(e.Graphics, str, ref textRect, Font, valueColor, Win32Calls.DT_VCENTER |
                        Win32Calls.DT_SINGLELINE | Win32Calls.DT_END_ELLIPSIS | Win32Calls.DT_NOPREFIX);

                    if (Focused && (str.Length > 0))
                    {
                        Rectangle focusRect = textRect;
                        focusRect.X -= 2;
                        focusRect.Width += 2;
                        focusRect.Y += 1;
                        focusRect.Height -= 2;
                        ControlPaint.DrawFocusRectangle(e.Graphics, focusRect);
                    }
                }
            }
            else if (IsEnumValue)
            {
                string[] displayedStrings =  _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                if (displayedStrings.Length > 0)
                {
                    Rectangle oneLineRect = clientRect;
                    oneLineRect.Height = clientRect.Height / displayedStrings.Length;
                    FieldInfo[] fis = _ownerPropertyEnum.Property.Value.UnderlyingType.GetFields(BindingFlags.Public | BindingFlags.Static);
#if _DOTNET2
                    // There is a bug in .Net 2.0 that prevents to use ArrayList.Sort (performance reasons)
                    Array.Sort<object>(fis, 0, fis.Length, new FieldInfoEnumComparer(_ownerPropertyEnum.Property.Value.UnderlyingType));
#else
                    Array.Sort(fis, 0, fis.Length, new FieldInfoEnumComparer(_ownerPropertyEnum.Property.Value.UnderlyingType));
#endif
                    int i = 0;
                    int displayedStringsIndex = 0;
                    foreach (FieldInfo fi in fis)
                    {
                        // If the field is not browsable, we continue to the next one
                        object[] attrs = fi.GetCustomAttributes(typeof(BrowsableAttribute), false);
                        if ((attrs.Length > 0) && (((BrowsableAttribute)attrs[0]).Browsable == false))
                        {
                            i++;
                            continue;
                        }

                        Rectangle checkboxRect = oneLineRect;

                        checkboxRect.Height -= 4;
                        checkboxRect.Y += 2;
                        checkboxRect.Width = checkboxRect.Height;

                        // Draw checkbox
                        if (_multipleValues)
                            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawCheckBoxIndeterminate(e.Graphics,
                                this, checkboxRect, _ownerPropertyEnum.Property.Enabled);
                        else
                        {
                            bool check = IsEnumCheckedAt(fi.GetValue(_ownerPropertyEnum.Property.Value.UnderlyingValue).GetHashCode());
                            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawCheckBox(e.Graphics, this,
                                checkboxRect, check, _ownerPropertyEnum.Property.Enabled);
                        }

                        Rectangle textRect = oneLineRect;
                        textRect.X = checkboxRect.Right + margin;
                        textRect.Width = oneLineRect.Right - textRect.Left;

                        // Draw corresponding text
                        Win32Calls.DrawText(e.Graphics, displayedStrings[displayedStringsIndex], ref textRect, Font,
                            valueColor, Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_END_ELLIPSIS | Win32Calls.DT_NOPREFIX);

                        if (Focused && (_focusIndex == displayedStringsIndex))
                        {
                            Rectangle focusRect = textRect;
                            focusRect.X -= 2;
                            focusRect.Width += 2;
                            focusRect.Y += 1;
                            focusRect.Height -= 2;
                            ControlPaint.DrawFocusRectangle(e.Graphics, focusRect);
                        }

                        oneLineRect.Offset(0, oneLineRect.Height);

                        i++;
                        displayedStringsIndex++;
                    }
                }
            }
            else if (_ownerPropertyEnum.Property.Value.UnderlyingValue is ArrayList)
            {
                string[] displayedStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                if (displayedStrings.Length > 0)
                {
                    Rectangle oneLineRect = clientRect;
                    oneLineRect.Height = clientRect.Height / displayedStrings.Length;

                    for (int i = 0; i < displayedStrings.Length; i++)
                    {
                        Rectangle checkboxRect = oneLineRect;

                        checkboxRect.Height -= 4;
                        checkboxRect.Y += 2;
                        checkboxRect.Width = checkboxRect.Height;

                        // Draw checkbox
                        if (_multipleValues)
                            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawCheckBoxIndeterminate(e.Graphics,
                                this, checkboxRect, _ownerPropertyEnum.Property.Enabled);
                        else
                        {
                            bool strFound = false;
                            foreach (object o in _collectionValue)
                            {
                                if (o.ToString() == displayedStrings[i])
                                    strFound = true;
                            }
                            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawCheckBox(e.Graphics, this,
                                checkboxRect, strFound, _ownerPropertyEnum.Property.Enabled);
                        }

                        Rectangle textRect = oneLineRect;
                        textRect.X = checkboxRect.Right + margin;
                        textRect.Width = oneLineRect.Right - textRect.Left;

                        // Draw corresponding text
                        Win32Calls.DrawText(e.Graphics, displayedStrings[i], ref textRect, Font, valueColor,
                            Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                            Win32Calls.DT_END_ELLIPSIS);

                        if (Focused && (_focusIndex == i))
                        {
                            Rectangle focusRect = textRect;
                            focusRect.X -= 2;
                            focusRect.Width += 2;
                            focusRect.Y += 1;
                            focusRect.Height -= 2;
                            ControlPaint.DrawFocusRectangle(e.Graphics, focusRect);
                        }

                        oneLineRect.Offset(0, oneLineRect.Height);
                    }
                }
            }
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (!Focused)
                    Focus();

                Rectangle clientRect = ClientRectangle;
                clientRect.Height++;

                string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                if (displayStrings.Length > 0)
                {
                    Rectangle oneLineRect = clientRect;
                    oneLineRect.Height = clientRect.Height / displayStrings.Length;
                    for (int i = 0; i < displayStrings.Length; i++)
                    {
                        if (oneLineRect.Contains(new Point(e.X, e.Y)))
                        {
                            _focusIndex = i;
                            Invalidate();

                            SwitchCheckboxAtIndex(i, false);

                            break;
                        }

                        oneLineRect.Offset(0, oneLineRect.Height);
                    }
                }
            }

            base.OnMouseDown(e);
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if ((keyData == Keys.Up) || (keyData == Keys.Down))
                return true;

            return base.IsInputKey(keyData);
        }

        protected void ProcessArrowKey(Keys key)
        {
            int count;
            if (IsEnumValue)
                count = _ownerPropertyEnum.Property.Value.GetStandardValues().Length;
            else
                count =  _ownerPropertyEnum.Property.Value.GetDisplayedValues().Length;

            if (count > 0)
            {
                if (key == Keys.Down)
                    _focusIndex = (_focusIndex + 1) % count;
                else if (key == Keys.Up)
                    _focusIndex = (--_focusIndex == -1 ? count - 1 : _focusIndex);

                Invalidate();
            }
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (((e.KeyCode == Keys.Down) || (e.KeyCode == Keys.Up)) && !IsBooleanValue)
                ProcessArrowKey(e.KeyCode);
            else if (e.KeyCode == Keys.Space)
                SwitchCheckboxAtIndex(_focusIndex, false);

            base.OnKeyDown(e);
        }

#if _DOTNET2
        [UIPermission(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
#endif
		protected override bool ProcessDialogKey(Keys keyData)
        {
            Keys key = (keyData & Keys.KeyCode);

            if (key == Keys.Return)
            {
                if (_realtimeChange == false)
                    CommitChanges(true);
                else
                {
                    // Reset the initial value to the new one
                    if (IsBooleanValue)
                        _initialBooleanValue = (NullableBoolean)_booleanValue.Clone();
                    else if (IsEnumValue)
                        _initialEnumValue = _enumValue;
                    else
                        _initialCollectionValue = (ArrayList)_collectionValue.Clone();
                }
            }
            else if (keyData == Keys.Escape)
            {
                // Restore the old value
                if (IsBooleanValue)
                    _booleanValue = (NullableBoolean)_initialBooleanValue.Clone();
                else if (IsEnumValue)
                    _enumValue = _initialEnumValue;
                else
                    _collectionValue = (ArrayList)_initialCollectionValue.Clone();

                _multipleValues = _initialMultipleValues;

                CommitChanges(true);
            }

            if (Focused)
                return base.ProcessDialogKey(keyData);
            else
                return true;
        }

        protected override void OnGotFocus(EventArgs e)
        {
            Invalidate();
            base.OnGotFocus(e);
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

        public void OnF4()
        {
            Focus();
        }

        public bool OnParentKeyDown(KeyEventArgs e)
        {
            return false;
        }

        bool IsEnumCheckedAt(int flagValue)
        {
            if (_multipleValues &&
                _enumValue.Equals((Enum)Enum.ToObject(_ownerPropertyEnum.Property.Value.UnderlyingType, 0)))
                return false;

            int currentValue = _enumValue.GetHashCode();
            return (((currentValue & flagValue) == flagValue) && (flagValue != 0)) || ((currentValue == 0) && (flagValue == 0));
        }

        private void SwitchCheckboxAtIndex(int index, bool forceRealtime)
        {
            if (IsEnumValue)
            {
                // The index must be translated in case there are some browsable(false) fields
                FieldInfo[] fis = _ownerPropertyEnum.Property.Value.UnderlyingType.GetFields(BindingFlags.Public | BindingFlags.Static);
#if _DOTNET2
                // There is a bug in .Net 2.0 that prevents to use ArrayList.Sort (performance reasons)
                Array.Sort<object>(fis, 0, fis.Length, new FieldInfoEnumComparer(_ownerPropertyEnum.Property.Value.UnderlyingType));
#else
                Array.Sort(fis, 0, fis.Length, new FieldInfoEnumComparer(_ownerPropertyEnum.Property.Value.UnderlyingType));
#endif
                int i = index;
                int j = 0;
                foreach (FieldInfo fi in fis)
                {
                    // If the field is not browsable, we continue to the next one
                    object[] attrs = fi.GetCustomAttributes(typeof(BrowsableAttribute), false);
                    if ((attrs.Length > 0) && (((BrowsableAttribute)attrs[0]).Browsable == false))
                    {
                        i++;
                        continue;
                    }

                    j++;
                    if (j > index)
                        break;
                }

                index = i;

                if (IsEnumCheckedAt(((FieldInfo)fis[index]).GetValue(_enumValue).GetHashCode()))
                {
                    // Remove a check mark
                    int clickedValue = ((FieldInfo)fis[index]).GetValue(_ownerPropertyEnum.Property.Value.UnderlyingValue).GetHashCode();
                    if (clickedValue == 0)
                    {
                        foreach (FieldInfo fi in fis)
                        {
                            int flagValue = fi.GetValue(_ownerPropertyEnum.Property.Value.UnderlyingValue).GetHashCode();
                            if (flagValue != 0)
                                _enumValue = (Enum)Enum.ToObject(_ownerPropertyEnum.Property.Value.UnderlyingType,
                                    Convert.ToInt32(_enumValue) | flagValue);
                        }
                    }
                    else
                        _enumValue = (Enum)Enum.ToObject(_ownerPropertyEnum.Property.Value.UnderlyingType,
                            Convert.ToInt32(_enumValue) & ~clickedValue);
                }
                else
                {
                    // Set a check mark
                    int clickedValue = ((FieldInfo)fis[index]).GetValue(_ownerPropertyEnum.Property.Value.UnderlyingValue).GetHashCode();
                    if (clickedValue == 0)
                        _enumValue = (Enum)Enum.ToObject(_ownerPropertyEnum.Property.Value.UnderlyingType, 0);
                    else
                        _enumValue = (Enum)Enum.ToObject(_ownerPropertyEnum.Property.Value.UnderlyingType,
                            Convert.ToInt32(_enumValue) | clickedValue);
                }

                _multipleValues = false;

                Invalidate();

                if (_realtimeChange)
                    CommitChanges(false);
            }
            else if (IsBooleanValue)
            {
                if (_booleanValue.IsNull)
                    _booleanValue.Value = true;
                else
                    _booleanValue.Value = !_booleanValue.Value;

                Invalidate();

                if (_realtimeChange || forceRealtime)
                    CommitChanges(false);
            }
            else // Collection
            {
                string[] displayedStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                if (index < displayedStrings.Length)
                {
                    int removeAtIndex = _collectionValue.IndexOf(displayedStrings[index]);

                    if (removeAtIndex != -1)
                        _collectionValue.RemoveAt(removeAtIndex);
                    else
                    {
                        int i;
                        int insertIndex = -1;
                        for (i = index - 1; i >= 0; i--)
                        {
                            insertIndex = _collectionValue.IndexOf(displayedStrings[i]);
                            if (insertIndex != -1)
                                break;
                        }

                        _collectionValue.Insert(insertIndex + 1, displayedStrings[index]);
                    }

                    _multipleValues = false;
                    Invalidate();

                    if (_realtimeChange)
                        CommitChanges(false);
                }
            }
        }

        protected override void OnMouseWheel(MouseEventArgs e)
        {
            if (!IsBooleanValue)
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
        }

        public virtual void RepositionChildren() { }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            return true;
        }

        public virtual bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force)
        {
            if (IsBooleanValue)
            {
                SwitchCheckboxAtIndex(0, true);
                return true;
            }

            return false;
        }

        public TextBox TextBox
        {
            get { return null; }
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            bool changed = false;
            
            // Check to see if the value actually changed
            object currentValue = _ownerPropertyEnum.Property.Value.GetValue();

            if (IsBooleanValue)
            {
                if (_booleanValue.IsNull == false)
                {
                    if (_ownerPropertyEnum.Property.Value.HasMultipleValues || ((bool)currentValue != _booleanValue.Value))
                        changed = true;
                }
            }
            else if (IsEnumValue)
            {
                if (_ownerPropertyEnum.Property.Value.HasMultipleValues)
                {
                    if (!_multipleValues)
                        changed = true;
                }
                else if (_enumValue.Equals(currentValue) == false)
                    changed = true;
            }
            else
            {
                if (_ownerPropertyEnum.Property.Value.HasMultipleValues)
                {
                    if (!_multipleValues)
                        changed = true;
                }
                else
                {
                    changed = false;
                    ArrayList currentValueArray = (currentValue as ArrayList);
                    if ((currentValueArray == null) && (_collectionValue == null))
                        changed = false;
                    else
                    {
                        if ((currentValueArray == null) || (_collectionValue == null))
                            changed = true;
                        else if (currentValueArray.Count != _collectionValue.Count)
                            changed = true;
                        else
                        {
                            for (int i = 0; i < _collectionValue.Count; i++)
                            {
                                if (currentValueArray[i].Equals(_collectionValue[i]) == false)
                                    changed = true;
                            }
                        }
                    }
                }
            }

            if (changed)
            {
                _ownerPropertyEnum.Property.Value.PreviousValue = currentValue;

                if (IsBooleanValue)
                {
                    _ownerPropertyEnum.Property.Value.SetValueFromInPlaceCtrl(_booleanValue.Value);
                    if (final)
                        _initialBooleanValue = (NullableBoolean)_booleanValue.Clone(); 
                }
                else if (IsEnumValue)
                {
                    _ownerPropertyEnum.Property.Value.SetValueFromInPlaceCtrl(_enumValue.ToString());
                    if (final)
                        _initialEnumValue = _enumValue;
                }
                else
                {
                    _ownerPropertyEnum.Property.Value.SetValueFromInPlaceCtrl(_collectionValue.Clone());
                    if (final)
                        _initialCollectionValue = (ArrayList)_collectionValue.Clone();
                }

                if (final)
                    _initialMultipleValues = false;

                _ownerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(new PropertyChangedEventArgs(_ownerPropertyEnum));

                Parent.Invalidate();
            }

            Invalidate();

            return (changed ? PropertyValue.ValueValidationResult.Validated : PropertyValue.ValueValidationResult.PreviousValueRestored);
        }

        public Control GetDropDownContainer()
        {
            return null;
        }
    }
}
