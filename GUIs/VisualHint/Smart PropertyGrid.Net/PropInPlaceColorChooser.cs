// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2006 VisualHint
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
using System.Windows.Forms.Design;
using System.Diagnostics;
using System.ComponentModel;
using System.Security.Permissions;
#if _DOTNET2
using System.Windows.Forms.VisualStyles;
#endif

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when the usual Microsoft color selector is needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceColorChooser : ContainerControl, IInPlaceControl
    {
        private class DropDownForm : Form
        {
            private bool _canceled = false;
            private bool _closeDropDownCalled;

            public DropDownForm()
            {
                FormBorderStyle = FormBorderStyle.None;
                ShowInTaskbar = false;
                KeyPreview = true;
                StartPosition = FormStartPosition.Manual;
            }

            public void SetControl(Control ctl)
            {
                Controls.Add(ctl);
            }

            protected override CreateParams CreateParams
            {
                [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
                get
                {
                    CreateParams cp = base.CreateParams;

                    cp.ExStyle = Win32Calls.WS_EX_TOOLWINDOW | Win32Calls.WS_EX_TOPMOST;
                    cp.Style = Win32Calls.WS_POPUP | Win32Calls.WS_BORDER;

                    if (OSFeature.IsPresent(SystemParameter.DropShadow))
                        cp.ClassStyle |= Win32Calls.CS_DROPSHADOW;

                    return cp;
                }
            }

            public bool Canceled
            {
                get { return _canceled; }
            }

            public void CloseDropDown()
            {
                _closeDropDownCalled = true;
                Hide();
            }

            protected override void OnKeyDown(KeyEventArgs e)
            {
                base.OnKeyDown(e);

                if ((e.KeyCode == Keys.Escape) || (e.KeyCode == Keys.F4) ||
                         ((ModifierKeys == Keys.Alt) && ((e.KeyCode == Keys.Down) || (e.KeyCode == Keys.Up))))
                    Hide();
            }

            protected override void OnVisibleChanged(EventArgs e)
            {
                if (Visible)
                    _canceled = false;

                base.OnVisibleChanged(e);
            }

            protected override void OnDeactivate(EventArgs e)
            {
                // We set the Owner to Nothing BEFORE calling the base class.
                // If we wouldn't do it, the Picker form would lose focus after 
                // the dropdown is closed.
                Owner = null;
                
                base.OnDeactivate(e);

                // If the form was closed by any other means as the CloseDropDown call, it is because
                // the user clicked outside the form, or pressed the ESC key.
                if (!_closeDropDownCalled)
                    _canceled = true;

                Hide();
            }
        }

        // This class actually hosts the ColorEditor.ColorUI by implementing the IWindowsFormsEditorService.
        private class EditorService : IWindowsFormsEditorService, IServiceProvider
        {
            // The associated color picker control.
            private PropInPlaceColorChooser _picker;

            // Reference to the drop down, which hosts the ColorUI control.
            private DropDownForm _dropDownHolder;

            // Cached _DropDownHolder.Canceled flag in order to allow it to be inspected
            // by the owning ColorPicker control.
            private bool _canceled;

            public EditorService(PropInPlaceColorChooser picker)
            {
                _picker = picker;
            }

            public bool Canceled
            {
                get { return _canceled; }
            }

            public void CloseDropDown()
            {
                if (_dropDownHolder != null)
                    _dropDownHolder.CloseDropDown();
            }

            public void DropDownControl(Control control)
            {
                if (_dropDownHolder == null)
                    _dropDownHolder = new DropDownForm();

                _canceled = false;

                // Initialize the hosting form for the control.
                _dropDownHolder.Bounds = control.Bounds;
                _dropDownHolder.SetControl(control);

                // Lookup a parent form for the Picker control and make the dropdown form to be owned
                // by it. This prevents to show the dropdown form icon when the user presses the At+Tab system 
                // key while the dropdown form is displayed.
                Control pickerForm = GetParentForm(_picker);
                if ((pickerForm != null) && (pickerForm is Form))
                    _dropDownHolder.Owner = (Form)pickerForm;

                // Ensure the whole drop-down UI is displayed on the screen and show it.
                PositionDropDownHolder();

                // Show the window but don't activate it
                _dropDownHolder.Show(); // ShowDialog would disable clicking outside the dropdown area!

                // Wait for the user to select a new color (in which case the ColorUI calls our CloseDropDown
                // method) or cancel the selection (no CloseDropDown called, the Cancel flag is set to True).
                DoModalLoop();

                // Remember the cancel flag
                _canceled = _dropDownHolder.Canceled;
//                _dropDownHolder.Dispose();
  //              _dropDownHolder = null;
            }

            public DialogResult ShowDialog(Form dialog)
            {
                throw new NotSupportedException();
            }

            public object GetService(Type serviceType)
            {
                if (serviceType.Equals(typeof(IWindowsFormsEditorService)))
                    return this;

                return null;
            }

            private void DoModalLoop()
            {
                Debug.Assert(_dropDownHolder != null);

                while(_dropDownHolder.Visible)
                {
                    Application.DoEvents();
                    Win32Calls.MsgWaitForMultipleObjects(1, IntPtr.Zero, true, 5, 255);
                }
            }

            private void PositionDropDownHolder()
            {
                // Convert _Picker location to screen coordinates.
                Point loc = _picker.Parent.PointToScreen(_picker.Location);
                loc.X -= _dropDownHolder.Width - _picker.Width;

                Rectangle workingArea = Screen.FromControl(_picker).WorkingArea;
//                Rectangle workingArea = Screen.PrimaryScreen.WorkingArea;

                // Position the dropdown X coordinate in order to be displayed in its entirety.
                if (loc.X < workingArea.X)
                    loc.X = workingArea.X;
                else if (loc.X + _dropDownHolder.Width > workingArea.Right)
                    loc.X = workingArea.Right - _dropDownHolder.Width;

                // Do the same for the Y coordinate.
                if (loc.Y + _picker.Height + _dropDownHolder.Height > workingArea.Bottom)
                    loc.Offset(0, -_dropDownHolder.Height);  // dropdown will be above the picker control
                else
                    loc.Offset(0, _picker.Height); // dropdown will be below the picker

                _dropDownHolder.Location = loc;
            }

            private Control GetParentForm(Control ctl)
            {
                do
                {
                    if (ctl.Parent == null)
                        return ctl;
                    else
                        ctl = ctl.Parent;
                }
                while (true);
            }
        }

        // This is the Color type editor - it displays the drop-down UI calling
        // our IWindowsFormsEditorService implementation.
        private System.Drawing.Design.ColorEditor _colorEditor = new System.Drawing.Design.ColorEditor();

        private EditorService _editorService;

        private PropInPlaceTextbox _edit = null;

        private bool _dontTransferFocus = false;

        private PropertyEnumerator _ownerPropertyEnum;

        public PropertyEnumerator OwnerPropertyEnumerator
        {
            set
            {
                _ownerPropertyEnum = value;

                if (_edit != null)
                {
                    _edit.OwnerPropertyEnumerator = value;
                    _edit.Text = Text;
                    _edit.LostFocus += new EventHandler(OnChildLostFocus);
                }
            }

            get { return _ownerPropertyEnum; }
        }

        public bool RealtimeChange
        {
            set {  }
            get { return true; }
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

        public string ActualText
        {
            set
            {
                if (_edit == null)
                    Text = value;
                else
                    _edit.Text = value;
            }

            get
            {
                if (_edit == null)
                    return Text;
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

        public PropInPlaceColorChooser(bool editable)
        {
            if (editable)
            {
                _edit = new PropInPlaceTextbox(false);
                _edit.Parent = this;
                _edit.Text = Text;
                _edit.TabStop = false;
            }

            _editorService = new EditorService(this);

            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
            ControlStyles.OptimizedDoubleBuffer | ControlStyles.Opaque | ControlStyles.ResizeRedraw, true);

            Cursor = Cursors.Arrow;
        }

        protected int GetButtonWidth(Graphics graphics)
        {
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

            // Draw the combobox arrow

            Rectangle buttonRect = GetButtonRect(e.Graphics);

            if (Application.RenderWithVisualStyles)
            {
                VisualStyleRenderer renderer1 = new VisualStyleRenderer(_mouseOver ? VisualStyleElement.ComboBox.DropDownButton.Hot : VisualStyleElement.ComboBox.DropDownButton.Normal);
                if (_pushed)
                    renderer1.SetParameters(VisualStyleElement.ComboBox.DropDownButton.Pressed);

                renderer1.DrawBackground(e.Graphics, buttonRect);
            }
            else
            {
                ControlPaint.DrawComboButton(e.Graphics, buttonRect, _mouseOver ? ButtonState.Pushed : ButtonState.Normal);
            }

            if (Focused && (_ownerPropertyEnum.Property.InPlaceCtrlInAction == null))
            {
                Rectangle focusRect = buttonRect;
                focusRect.Inflate(-3, -3);
                ControlPaint.DrawFocusRectangle(e.Graphics, focusRect);
            }

            // Draw the value
            if (_edit == null)
            {
                Rectangle valueRect = ClientRectangle;
                valueRect.Width -= buttonRect.Width + 1;

                if (Focused && (_ownerPropertyEnum.Property.InPlaceCtrlInAction == null))
                {
                    Rectangle fillRect = _ownerPropertyEnum.Property.Value.GetStringValueRect(e.Graphics, valueRect, Point.Empty);
                    fillRect.Y++;
                    fillRect.Height-=2;
                    fillRect.X -= PropertyGrid.GlobalTextMargin / 2;
                    fillRect.Width = buttonRect.Left - 1 - fillRect.Left;
                    e.Graphics.FillRectangle(SystemBrushes.FromSystemColor(SystemColors.Highlight), fillRect);
                }

                Color valueColor;
                if (_ownerPropertyEnum.Property.Enabled == false)
                    valueColor = SystemColors.GrayText;
                else
                {
                    if (Focused && (_ownerPropertyEnum.Property.InPlaceCtrlInAction == null))
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
            Graphics graphics = CreateGraphics();
            bool result = GetButtonRect(graphics).Contains(pt);
            graphics.Dispose();

            return result;
        }

        public void RepositionChildren()
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
                    Rectangle editRect = new Rectangle(0, 0, clientRect.Width - buttonWidth, clientRect.Height);
                    editRect.Inflate(0, -1);

                    // The width of the editbox can't be smaller than the margins or the margins would be void and the text misplaced
                    if (editRect.Width <= margins)
                        _edit.Visible = false;
                    else
                    {
                        _edit.Visible = true;

                        Win32Calls.TEXTMETRIC tm = new Win32Calls.TEXTMETRIC();
                        Graphics graphics = CreateGraphics();
                        Win32Calls.GetTextMetrics(graphics, Font, ref tm);
                        graphics.Dispose();
                        editRect.Y += (clientRect.Height - tm.tmHeight) / 2;
                        editRect.Height -= (clientRect.Height - tm.tmHeight) / 2;

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

        protected virtual void OnWmSetFocus(FocusEventArgs e)
        {
            _wmFocusMissingArg = e.FocusControl;

            if (_edit != null)
            {
                if (_dontTransferFocus == false)
                {
                    if (e.FocusControl != _edit)
                        _edit.Focus();
                    else if (e.FocusControl == _edit)
                        Parent.Focus();
                }
            }
            else
                Invalidate();
        }

        protected void ValidateTextInternal()
        {
//            if (mEdit == null)
            {
                Invalidate();

                PropertyValue value = _ownerPropertyEnum.Property.Value;

                // Store the previous value
                string oldValue = value.GetStringValue();

                if (oldValue != ActualText)
                {
                    // Freeze painting because updating the property may also update other properties in the grid.
                    _ownerPropertyEnum.Property.ParentGrid.BeginUpdate();

                    _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                    value.UpdateStringValue(ActualText);

                    // Defreeze painting
                    _ownerPropertyEnum.Property.ParentGrid.EndUpdate();

                    // Notify the properties control if the value has really changed
                    if (value.GetStringValue() != oldValue)
                        _ownerPropertyEnum.Property.ParentGrid.OnPropertyChanged(new PropertyChangedEventArgs(_ownerPropertyEnum));

                    Parent.Invalidate();
                }
            }
        }

        protected virtual void OnWmKillFocus(FocusEventArgs e)
        {
            _wmFocusMissingArg = e.FocusControl;
        }

        protected override void OnLostFocus(EventArgs e)
        {
            if (_edit == null)
                ValidateTextInternal();

            base.OnLostFocus(new FocusEventArgs(_wmFocusMissingArg, false));
        }

        void OnChildLostFocus(object source, EventArgs e)
        {
            Invalidate(false);
            base.OnLostFocus(e);
        }

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_SETFOCUS)
                OnWmSetFocus(new FocusEventArgs(FromHandle(m.WParam), false));
            else if (m.Msg == Win32Calls.WM_KILLFOCUS)
                OnWmKillFocus(new FocusEventArgs(FromHandle(m.WParam), false));

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
                    if (_ownerPropertyEnum.Property.Value.DisplayString.Equals(displayedStrings[i]))
                    {
                        int j;
                        for (j = 0; j < displayedStrings.Length; j++)
                        {
                            string currentStr = displayedStrings[((i + j) + 1) % displayedStrings.Length];
                            if (string.Compare(currentStr.Substring(0, 1), letter, true, CultureInfo.InvariantCulture) == 0)
                            {
                                Text = currentStr;
                                ValidateTextInternal();
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
                Graphics graphics = CreateGraphics();
                if (GetButtonRect(graphics).Contains(e.Location))
                {
                    _pushed = true;
                    _mouseOver = true;
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

        protected override void OnMouseDoubleClick(MouseEventArgs mea)
        {
            string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
            if (ActualText.Equals(displayStrings[displayStrings.Length - 1]))
            {
                ActualText = displayStrings[0];
            }
            else
                ProcessArrowKey(Keys.Down);
            Invalidate();
        }

        protected override void OnMouseLeave(EventArgs e)
        {
//            _mouseOver = false;
//            Invalidate(false);

            base.OnMouseLeave(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (_pushed)
            {
                bool oldMouseOver = _mouseOver;
                Graphics graphics = CreateGraphics();
                _mouseOver = GetButtonRect(graphics).Contains(e.Location);
                graphics.Dispose();
                if (oldMouseOver != _mouseOver)
                    Invalidate(false);
            }

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

                Graphics graphics = CreateGraphics();
                if (GetButtonRect(graphics).Contains(e.Location))
                {
                    // Maybe the user typed something in the editbox that has not been validated yet
                    if (_edit != null)
                        _edit.ValidateTextInternal();

                    RunButton();
                }
                graphics.Dispose();
            }

            base.OnMouseUp(e);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.KeyData == Keys.Space)
            {
                _mouseOver = true;
                Invalidate(false);
            }
            else if ((e.KeyData == Keys.Up) || (e.KeyData == Keys.Down))
            {
                ProcessArrowKey(e.KeyData);
                Invalidate();
            }
        }

        protected void ProcessArrowKey(Keys key)
        {
            string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();
            for (int i = 0; i < displayStrings.Length; i++)
            {
                if (displayStrings[i].Equals(ActualText))
                {
                    if ((key == Keys.Up) && (i > 0))
                        ActualText = displayStrings[i - 1];
                    else if ((key == Keys.Down) && (i < displayStrings.Length - 1))
                        ActualText = displayStrings[i + 1];

                    break;
                }
            }
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            if (e.KeyData == Keys.Space)
            {
                _mouseOver = false;
                Invalidate(false);

                RunButton();
            }
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if ((keyData == Keys.Up) || (keyData == Keys.Down))
                return true;

            return base.IsInputKey(keyData);
        }

        protected override bool ProcessTabKey(bool forward)
        {
            if (_edit != null)
            {
                if (forward && _edit.Focused)
                {
                    _dontTransferFocus = true;
                    ActiveControl = null;
                    Focus();
                    _dontTransferFocus = false;
                    return true;
                }
            }

            return base.ProcessTabKey(forward);
        }

#if _DOTNET2
        [UIPermission(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
#endif
		protected override bool ProcessDialogKey(Keys keyData)
        {
            Keys key = (keyData & Keys.KeyCode);

            if (key == Keys.Return)
            {
                ValidateTextInternal();
                Parent.Focus();
                return true;
            }
            else if (key == Keys.Escape)
            {
                if (_edit == null)
                {
                    // Restore the old string in the edit box
                    Text = _ownerPropertyEnum.Property.Value.DisplayString;
                }

                Parent.Focus();
                Invalidate();
                return true;
            }
            else if (key == Keys.F4)
            {
                Parent.Focus();
                OnF4();
                return true;
            }

            return base.ProcessDialogKey(keyData);
        }

        protected void RunButton()
        {
            try
            {
                _ownerPropertyEnum.Property.InPlaceCtrlInAction = this;

                PropertyValue propertyValue = _ownerPropertyEnum.Property.Value;

                // Display the UI.
                object newValue = _colorEditor.EditValue(_editorService, propertyValue.TypeConverter.ConvertFromString(
                    propertyValue.GetTypeDescriptorContext(_ownerPropertyEnum), ActualText));

                // If the user didn't cancel the selection, remember the new color.
                if ((newValue != null) && (!_editorService.Canceled))
                {
                    ActualText = propertyValue.TypeConverter.ConvertToString(propertyValue.GetTypeDescriptorContext(_ownerPropertyEnum),
                        newValue);
                    ValidateTextInternal();
                }

                Parent.Focus();

//                _colorEditor = null;
            }
            catch(Exception)
            {
            }

            _ownerPropertyEnum.Property.InPlaceCtrlInAction = null;
        }

        public void OnF4()
        {
            Focus();

            RunButton();
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

            (e as HandledMouseEventArgs).Handled = true;
        }

        public bool GoToNextPropertyOnTab(bool forward)
        {
            return false;
        }
    }
}
