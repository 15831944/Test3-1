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
    /// Inplace control shown in the value column when radio buttons are needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceRadioButton : Control, IInPlaceControl
    {
        /// <summary>
        /// Because Control.OnLostFocus and Control.OnGotFocus doesn't provide a reference to the
        /// window that lost or gained the focus, this variable will contain this missing information.
        /// It is initialized in WndProc when processing WM_FILLFOCUS and WM_SETFOCUS messages.
        /// It is also embedded in a FocusEventArgs instance for event listeners.
        /// </summary>
        private Control _wmFocusMissingArg;

        private int _focusIndex = -1;

        private bool _realtimeChange = true;

        public bool RealtimeChange
        {
            set { _realtimeChange = value; }
            get { return _realtimeChange; }
        }

        public PropInPlaceRadioButton()
        {
            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque | ControlStyles.ResizeRedraw, true);

            Cursor = Cursors.Arrow;
        }

        private string _initialValue;

        private PropertyEnumerator _ownerPropertyEnum;

        public PropertyEnumerator OwnerPropertyEnumerator
        {
            set
            {
                _ownerPropertyEnum = (PropertyEnumerator)value.Clone();

                Text = _ownerPropertyEnum.Property.Value.DisplayString;

                _initialValue = _ownerPropertyEnum.Property.Value.DisplayString;

                string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                for (int i = 0; i < displayStrings.Length; i++)
                {
                    if (displayStrings[i].Equals(_initialValue))
                    {
                        _focusIndex = i;
                        break;
                    }
                }
            }

            get { return _ownerPropertyEnum; }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
	        Rectangle clientRect = ClientRectangle;
            clientRect.Height++;

	        // Fill the background
            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(e.Graphics,
                clientRect, _ownerPropertyEnum);

            clientRect.X += _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin - 1;
            clientRect.Width -= _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin - 1;

            string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
            if (displayStrings.Length > 0)
            {
                Rectangle oneLineRect = clientRect;
                oneLineRect.Height = clientRect.Height / displayStrings.Length;
                for (int i = 0; i < displayStrings.Length; i++)
                {
                    Rectangle radioRect = oneLineRect;

                    radioRect.Height -= 4;
                    radioRect.Y += 2;
                    radioRect.Width = radioRect.Height;

                    // Draw radio button
                    _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawRadioButton(e.Graphics,
                        _ownerPropertyEnum.Property.ParentGrid.Grid, radioRect, (Text == displayStrings[i]),
                        _ownerPropertyEnum.Property.Enabled);

                    Rectangle textRect = oneLineRect;
                    textRect.X = radioRect.Right + _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin;
                    textRect.Width = oneLineRect.Right - textRect.Left;

                    // Draw corresponding text
                    Color valueColor;
                    if (_ownerPropertyEnum.Property.Enabled == false)
                        valueColor = SystemColors.GrayText;
                    else
                        valueColor = _ownerPropertyEnum.Property.Value.ForeColor;

                    Win32Calls.DrawText(e.Graphics, displayStrings[i], ref textRect, Font, valueColor,
                        Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_END_ELLIPSIS | Win32Calls.DT_NOPREFIX);

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
                        if (oneLineRect.Contains(new Point(e.X, e.Y)) && (_focusIndex != i))
                        {
                            _focusIndex = i;

                            Text = displayStrings[i];

                            if (_realtimeChange)
                            {
                                _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                                _ownerPropertyEnum.Property.Value.SetValueFromInPlaceCtrl(Text);
                                PropertyChangedEventArgs pce = new PropertyChangedEventArgs(_ownerPropertyEnum);
                                pce.SelectedIndex = _focusIndex;
                                _ownerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(pce);
                            }

                            Invalidate();
                            Parent.Invalidate();

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
            string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
            if (key == Keys.Down)
            {
                if (_focusIndex == -1)
                    _focusIndex = 0;
                else
                    _focusIndex = (_focusIndex + 1) % displayStrings.Length;
            }
            else if (key == Keys.Up)
            {
                if (_focusIndex == -1)
                    _focusIndex = displayStrings.Length - 1;
                else
                    _focusIndex = (--_focusIndex == -1 ? displayStrings.Length - 1 : _focusIndex);
            }

            Text = displayStrings[_focusIndex];

            if (_realtimeChange)
            {
                _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                _ownerPropertyEnum.Property.Value.SetValueFromInPlaceCtrl(Text);
                PropertyChangedEventArgs e = new PropertyChangedEventArgs(_ownerPropertyEnum);
                e.SelectedIndex = _focusIndex;
                _ownerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(e);
            }

            Invalidate();
            Parent.Invalidate();
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if ((e.KeyCode == Keys.Down) || (e.KeyCode == Keys.Up))
                ProcessArrowKey(e.KeyCode);

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
                    _initialValue = Text;
            }
            else if (keyData == Keys.Escape)
            {
                Text = _initialValue;
                CommitChanges(true);

                _focusIndex = -1;
                string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
                for (int i = 0; i < displayStrings.Length; i++)
                {
                    if (displayStrings[i].Equals(Text))
                    {
                        _focusIndex = i;
                        break;
                    }
                }
            }

            if (Focused)
                return base.ProcessDialogKey(keyData);
            else
                return true;
        }

        protected override void OnGotFocus(EventArgs e)
        {
            Text = _ownerPropertyEnum.Property.Value.DisplayString;

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

        public virtual void RepositionChildren() { }

        public virtual bool GoToNextPropertyOnTab(bool forward)
        {
            return true;
        }

        public virtual bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force)
        {
            if (forward)
                ProcessArrowKey(Keys.Down);
            else
                ProcessArrowKey(Keys.Up);

            return true;
        }

        public TextBox TextBox
        {
            get { return null; }
        }

        public virtual PropertyValue.ValueValidationResult CommitChanges(bool final)
        {
            PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;

            if (final)
                _initialValue = Text;

            PropertyValue propertyValue = _ownerPropertyEnum.Property.Value;
            if (Text != propertyValue.DisplayString)
            {
                propertyValue.PreviousValue = propertyValue.GetValue();

                changed = propertyValue.SetValueFromInPlaceCtrl(Text);
                PropertyChangedEventArgs pce = new PropertyChangedEventArgs(_ownerPropertyEnum);
                pce.SelectedIndex = _focusIndex;
                _ownerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(pce);
            }

            Invalidate();
            Parent.Invalidate();

            return changed;
        }

        public Control GetDropDownContainer()
        {
            return null;
        }
    }
}
