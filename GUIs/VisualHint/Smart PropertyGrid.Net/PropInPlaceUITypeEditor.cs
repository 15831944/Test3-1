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
using System.Windows.Forms.Design;
using System.Diagnostics;
using System.ComponentModel;
using System.Security.Permissions;
using System.Drawing.Design;
using System.Drawing.Drawing2D;
using Skybound.Windows.Forms;
using System.Collections;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when the usual Microsoft UITypeEditor selector is needed.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceUITypeEditor : Control, IInPlaceControl
    {
        internal class DropDownForm : Form, IMessageHookListener
        {
            private bool _canceled = false;
//            private bool _closeDropDownCalled;

            private PropInPlaceUITypeEditor _inPlaceCtrl;

            private enum MoveDirection
            {
                None = 0,
                Vertical = 1,
                NESW = 2,
                NWSE = 3
            }

            private MoveDirection _currentMoveDirection = MoveDirection.None;

            private bool _resizable;

            public static readonly int ResizeGripHeight = 0;

            public static readonly Size MinSize;

            public bool ResizeInProgress
            {
                get { return (_currentMoveDirection != MoveDirection.None); }
            }

            private Point _resizeStartPoint = Point.Empty;

            private bool _resizeTop;

            public bool ResizeTop
            {
                get { return _resizeTop; }

                set
                {
                    DockPadding.Bottom = 0;
                    DockPadding.Top = 0;

                    _resizeTop = value;
                    if (_resizable)
                    {
                        _resizeGripImage = null;

                        if (value)
                            DockPadding.Top = ResizeGripHeight + 1;
                        else
                            DockPadding.Bottom = ResizeGripHeight + 1;
                    }
                }
            }

            static DropDownForm()
            {
                ResizeGripHeight = SystemInformation.HorizontalScrollBarHeight;
                MinSize = new Size(SystemInformation.VerticalScrollBarWidth * 4, SystemInformation.HorizontalScrollBarHeight * 4);
            }

            public DropDownForm(PropInPlaceUITypeEditor inPlaceCtrl, bool resizable)
            {
                FormBorderStyle = FormBorderStyle.None;
                ShowInTaskbar = false;
                KeyPreview = true;
                StartPosition = FormStartPosition.Manual;
                _inPlaceCtrl = inPlaceCtrl;
                _inPlaceCtrl.SetDropDownContainer(this);
                _resizable = resizable;

                Win32Calls.SetWindowLong(Handle, -8, (int)inPlaceCtrl.Parent.Handle);
                IntPtr handle = (IntPtr)Win32Calls.GetWindowLong(Handle, -8);
                Control ancestorControl = Control.FromHandle(handle);
            }

            protected override CreateParams CreateParams
            {
                [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
                get
                {
                    CreateParams cp = base.CreateParams;

                    cp.ExStyle = Win32Calls.WS_EX_TOOLWINDOW | Win32Calls.WS_EX_TOPMOST;
                    cp.Style = Win32Calls.WS_POPUP | Win32Calls.WS_BORDER;

#if _DOTNET2
                if (OSFeature.IsPresent(SystemParameter.DropShadow))
                    cp.ClassStyle |= Win32Calls.CS_DROPSHADOW;
#else
					if ((Environment.OSVersion.Platform == PlatformID.Win32NT) &&
						(Environment.OSVersion.Version.CompareTo(new Version(5, 1, 0, 0)) >= 0))
						cp.ClassStyle |= Win32Calls.CS_DROPSHADOW;
#endif

                    return cp;
                }
            }

            private Control _innerControl;

            public void SetControl(Control ctl)
            {
                Controls.Clear();

                _innerControl = ctl;
                Controls.Add(_innerControl);
            }

            public Control GetControl()
            {
                return _innerControl;
            }

            public bool Canceled
            {
                get { return _canceled; }
            }

            public void CloseDropDown()
            {
                Hide();
            }

            protected override void OnVisibleChanged(EventArgs e)
            {
                if (Visible)
                    _canceled = false;

                _resizeGripImage = null;

                base.OnVisibleChanged(e);
            }

            public bool OnListenMessageHook(Control control, uint message, IntPtr wParam, IntPtr lParam)
            {
                if (message == Win32Calls.WM_KILLFOCUS)
                {
                    if (control != null)
                    {
                        if (Contains(control))
                        {
                            Control wndTakingFocus = FromHandle(wParam);
                            if ((wndTakingFocus != null) && (wndTakingFocus != this) && !Contains(wndTakingFocus))
                                _inPlaceCtrl.Parent.Focus();
                        }
                    }
                }

                return false;
            }

            protected override void OnKeyDown(KeyEventArgs e)
            {
                base.OnKeyDown(e);

                if (e.KeyCode == Keys.Escape)
                    _canceled = true;

                if ((e.KeyCode == Keys.Escape) || (e.KeyCode == Keys.F4) ||
                    (e.Alt && ((e.KeyCode == Keys.Down) || (e.KeyCode == Keys.Up))))
                {
                    Hide();
                }
            }

            private MoveDirection GetMoveDirection(Point pt)
            {
                if (ResizeTop)
                {
                    Rectangle rect = new Rectangle(0, 0, ResizeGripHeight, ResizeGripHeight);
                    if (rect.Contains(pt))
                        return MoveDirection.NWSE;
                }
                else
                {
                    Rectangle rect = new Rectangle(0, Height - ResizeGripHeight, ResizeGripHeight, ResizeGripHeight);
                    if (rect.Contains(pt))
                        return MoveDirection.NESW;
                }

                int halfHeightNS = ResizeGripHeight / 2;
                Rectangle vertRect = new Rectangle(0, ResizeTop ? 0 : Height - halfHeightNS, Width, halfHeightNS);
                if (vertRect.Contains(pt))
                    return MoveDirection.Vertical;

                return MoveDirection.None;
            }

            protected override void OnMouseDown(MouseEventArgs e)
            {
                if (e.Button == MouseButtons.Left)
                {
                    _currentMoveDirection = GetMoveDirection(new Point(e.X, e.Y));
                    if (_currentMoveDirection != MoveDirection.None)
                    {
                        _resizeStartPoint = PointToScreen(new Point(e.X, e.Y));
                        Capture = true;
                    }
                }

                base.OnMouseDown(e);
            }

            protected override void OnMouseMove(MouseEventArgs e)
            {
                if (_currentMoveDirection != MoveDirection.None)
                {
                    Point pt = PointToScreen(new Point(e.X, e.Y));

                    int deltaY = pt.Y - _resizeStartPoint.Y;
                    int deltaX = (_currentMoveDirection != MoveDirection.Vertical ? pt.X - _resizeStartPoint.X : 0);

                    if (Width - deltaX < MinSize.Width)
                    {
                        deltaX = Width - MinSize.Width;
                        _resizeStartPoint.X = Right - MinSize.Width + (_resizeStartPoint.X - Left);
                    }
                    else
                        _resizeStartPoint.X = pt.X;

                    if (ResizeTop)
                    {
                        if (Height - deltaY < MinSize.Height)
                        {
                            deltaY = Height - MinSize.Height;
                            _resizeStartPoint.Y = Bottom - MinSize.Height + (_resizeStartPoint.Y - Top);
                        }
                        else
                            _resizeStartPoint.Y = pt.Y;
                    }
                    else
                    {
                        if (Height + deltaY < MinSize.Height)
                        {
                            deltaY = MinSize.Height - Height;
                            _resizeStartPoint.Y = Top + MinSize.Height + (_resizeStartPoint.Y - Bottom);
                        }
                        else
                            _resizeStartPoint.Y = pt.Y;
                    }

                    Rectangle oldBounds = Bounds;
                    if (ResizeTop)
                        Bounds = new Rectangle(Left + deltaX, Top + deltaY, Width - deltaX, Height - deltaY);
                    else
                        Bounds = new Rectangle(Left + deltaX, Top, Width - deltaX, Height + deltaY);

                    if (oldBounds != Bounds)
                        Refresh();
                }
                else
                {
                    MoveDirection md = GetMoveDirection(new Point(e.X, e.Y));
                    switch (md)
                    {
                        case MoveDirection.Vertical:
                            Cursor = Cursors.SizeNS;
                            break;
                        case MoveDirection.NESW:
                            Cursor = Cursors.SizeNESW;
                            break;
                        case MoveDirection.NWSE:
                            Cursor = Cursors.SizeNWSE;
                            break;
                        default:
                            Cursor = null;
                            break;
                    }
                }

                base.OnMouseMove(e);
            }

            protected override void OnMouseLeave(EventArgs e)
            {
                Cursor = null;
                base.OnMouseLeave(e);
            }

            protected override void OnMouseUp(MouseEventArgs e)
            {
                base.OnMouseUp(e);

                if (e.Button == MouseButtons.Left)
                {
                    _currentMoveDirection = MoveDirection.None;
                    Capture = false;
                }
            }

            protected override void OnDeactivate(EventArgs e)
            {
                // We set the Owner to null BEFORE calling the base class.
                // If we wouldn't do it, the Picker form would lose focus after 
                // the dropdown is closed.
                Owner = null;
                base.OnDeactivate(e);

                Hide();

                // Click outside the app or ALT-TAB ?
                if (ActiveForm == null)
                {
                    _inPlaceCtrl.OwnerPropertyEnumerator.Property.ParentGrid.Grid.HideInPlaceControl();
                    _inPlaceCtrl.FindForm().ActiveControl = _inPlaceCtrl.Parent;
                }
            }

            protected override void OnPaint(PaintEventArgs e)
            {
                base.OnPaint(e);

                if (_resizable)
                {
                    Rectangle rectangle1 = new Rectangle(0, ResizeTop ? -1 : (Height - ResizeGripHeight - 3), ResizeGripHeight, ResizeGripHeight);
                    e.Graphics.DrawImage(GetResizeGripImage(e.Graphics), rectangle1);
                    int lineY = ResizeTop ? GetControl().Top - 1 : GetControl().Bottom;
                    Pen pen = new Pen(SystemColors.ControlDark, 1f);
                    e.Graphics.DrawLine(pen, 0, lineY, Width, lineY);
                    pen.Dispose();
                }
            }

            private Bitmap _resizeGripImage = null;

            private Bitmap GetResizeGripImage(Graphics g)
            {
                if (_resizeGripImage == null)
                {
                    _resizeGripImage = new Bitmap(ResizeGripHeight, ResizeGripHeight, g);
                    using (Graphics graphics = Graphics.FromImage(_resizeGripImage))
                    {
                        Matrix matrix = new Matrix();
                        matrix.Translate(ResizeGripHeight + 1, ResizeTop ? ResizeGripHeight + 1 : 0f);
                        matrix.Scale(-1f, ResizeTop ? -1f : 1f);
                        graphics.Transform = matrix;
                        ControlPaint.DrawSizeGrip(graphics, BackColor, 0, 0, ResizeGripHeight, ResizeGripHeight);
                        graphics.ResetTransform();
                    }
                    _resizeGripImage.MakeTransparent(BackColor);
                }
                return _resizeGripImage;
            }
        }

        // This class actually hosts the UITypeEditor by implementing the IWindowsFormsEditorService.
        private class EditorService : IWindowsFormsEditorService, IServiceProvider
        {
            // The associated  picker control.
            private PropInPlaceUITypeEditor _inPlaceCtrl;

            // Reference to the drop down, which hosts the UITypeEditor control.
            private DropDownForm _dropDownHolder;

            internal DropDownForm DropDownHolder
            {
                get { return _dropDownHolder; }
            }

            private DialogResult _dialogResult = DialogResult.Cancel;

            public DialogResult DialogResult
            {
                get { return _dialogResult; }
                set { _dialogResult = value; }
            }

            // Cached _DropDownHolder.Canceled flag in order to allow it to be inspected
            // by the owning Picker control.
            private bool _canceled;

            public EditorService(PropInPlaceUITypeEditor inPlaceCtrl)
            {
                _inPlaceCtrl = inPlaceCtrl;
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
                    _dropDownHolder = new DropDownForm(_inPlaceCtrl, _inPlaceCtrl.Resizable);

                _canceled = false;

                _dropDownHolder.SetControl(control);

                // Initialize the hosting form for the control.
#if _DOTNET2
                Rectangle rect = new Rectangle(0, 0, control.PreferredSize.Width, control.Bounds.Height);
#else
                Rectangle rect = control.Bounds;
#endif
                rect.Inflate(1,1);
                if (_inPlaceCtrl.Resizable)
                    rect.Height += DropDownForm.ResizeGripHeight + 1;
                _dropDownHolder.Bounds = rect;

                control.Dock = DockStyle.Fill;
                control.Resize += new EventHandler(OnUIControlResize);

                _dropDownHolder.Width = Math.Max(_inPlaceCtrl.Width + 1, _dropDownHolder.Width);

                // Lookup a parent form for the Picker control and make the dropdown form to be owned
                // by it. This prevents to show the dropdown form icon when the user presses the At+Tab system 
                // key while the dropdown form is displayed.
                Control pickerForm = GetParentForm(_inPlaceCtrl);
                if ((pickerForm != null) && (pickerForm is Form))
                    _dropDownHolder.Owner = (Form)pickerForm;

                // Ensure the whole drop-down UI is displayed on the screen and show it.
                PositionDropDownHolder();

                Win32Calls.ShowWindow(new System.Runtime.InteropServices.HandleRef(this._dropDownHolder, this._dropDownHolder.Handle), 8);
                _dropDownHolder.Visible = true;
                control.Focus();
                _dropDownHolder.Refresh();
                _dropDownHolder.Invalidate(true);

                ((PropertyGrid)_inPlaceCtrl.Parent.Parent).SetDropDownTopWindow(_dropDownHolder);

                // Wait for the user to select a new value (in which case the UITypeEditor calls our CloseDropDown
                // method) or cancel the selection (no CloseDropDown called, the Cancel flag is set to True).
                DoModalLoop();

                ((PropertyGrid)_inPlaceCtrl.Parent.Parent).SetDropDownTopWindow(null);

                // Remember the cancel flag
                _canceled = _dropDownHolder.Canceled;
            }

            void OnUIControlResize(object sender, EventArgs e)
            {
                if (_dropDownHolder.ResizeInProgress == false)
                {
                    Rectangle rect = _dropDownHolder.GetControl().Bounds;
                    rect.Inflate(1, 1);
                    if (_inPlaceCtrl.Resizable)
                    {
                        int delta = DropDownForm.ResizeGripHeight + 1;
                        rect.Height += delta;
                    }
                    _dropDownHolder.Size = rect.Size;
                    PositionDropDownHolder();
                }
            }

            public DialogResult ShowDialog(Form dialog)
            {
                _inPlaceCtrl.Parent.Focus();

                // Lookup a parent form for the Picker control and make the dropdown form to be owned
                // by it. This prevents to show the dropdown form icon when the user presses the At+Tab system 
                // key while the dropdown form is displayed.
                Control pickerForm = GetParentForm(_inPlaceCtrl);
                _dialogResult = dialog.ShowDialog(pickerForm);
                return _dialogResult;
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
                Point loc = _inPlaceCtrl.Parent.PointToScreen(_inPlaceCtrl.Location);
                loc.X -= _dropDownHolder.Width - _inPlaceCtrl.Width;

                Rectangle workingArea = Screen.FromControl(_inPlaceCtrl).WorkingArea;
                //                Rectangle workingArea = Screen.PrimaryScreen.WorkingArea;

                // Position the dropdown X coordinate in order to be displayed in its entirety.
                if (loc.X < workingArea.X)
                    loc.X = workingArea.X;
                else if (loc.X + _dropDownHolder.Width > workingArea.Right)
                    loc.X = workingArea.Right - _dropDownHolder.Width;

                // Do the same for the Y coordinate.
                if (loc.Y + _inPlaceCtrl.Height + _dropDownHolder.Height > workingArea.Bottom)
                {
                    loc.Offset(0, -_dropDownHolder.Height);  // dropdown will be above the picker control
                    _dropDownHolder.ResizeTop = true;
                }
                else
                {
                    loc.Offset(0, _inPlaceCtrl.Height); // dropdown will be below the picker
                    _dropDownHolder.ResizeTop = false;
                }

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

        // This is the  type editor - it displays the drop-down UI calling
        // our IWindowsFormsEditorService implementation.
        private UITypeEditor _editor = null;

        private EditorService _editorService;

        private PropInPlaceTextbox _edit = null;

        private bool _dontTransferFocus = false;

        private PropertyValue.ValueValidationResult _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

        private object _initialValue;

        private PropertyEnumerator _currentInvalidPropertyEnum;

        private PropertyEnumerator _ownerPropertyEnum;

        public PropertyEnumerator OwnerPropertyEnumerator
        {
            set
            {
                if (!Visible)
                    _currentValueValidationResult = PropertyValue.ValueValidationResult.PreviousValueRestored;

                _ownerPropertyEnum = (PropertyEnumerator)value.Clone();

                if (_ownerPropertyEnum.Property.Value.HasMultipleValues)
                    Text = "";
                else
                    Text = _ownerPropertyEnum.Property.Value.DisplayString;

                if (_edit != null)
                {
                    _edit.OwnerPropertyEnumerator = value;
                    _edit.TextChanged += new EventHandler(OnTextboxTextChanged);
                }

                _initialValue = _ownerPropertyEnum.Property.Value.GetValue();
            }

            get { return _ownerPropertyEnum; }
        }

        public bool ReadOnly
        {
            get
            {
                // A modal editor always gives the chance to edit the content of collections
/*                if ((_editor.GetEditStyle(_ownerPropertyEnum.Property.Value.GetTypeDescriptorContext(
                    _ownerPropertyEnum)) == UITypeEditorEditStyle.Modal) &&
                    _ownerPropertyEnum.Property.ParentGrid.Enabled)
                    return false;
                */
                return _ownerPropertyEnum.Property.Value.IsReadOnly(_ownerPropertyEnum);
            }
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

        public override string Text
        {
            set
            {
                if (_edit == null)
                    base.Text = value;
                else
                    _edit.Text = value;
                Refresh();
            }

            get
            {
                if (_edit == null)
                    return base.Text;
                else
                    return _edit.Text;
            }
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

        public PropInPlaceUITypeEditor(UITypeEditor _editor, bool editable)
        {
            this._editor = _editor;

            if (editable)
            {
                _edit = new PropInPlaceTextbox(false);
                _edit.Parent = this;
                _edit.Text = Text;
                _edit.TabStop = false;
            }

            _editorService = new EditorService(this);

            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque | ControlStyles.ResizeRedraw, true);

            Cursor = Cursors.Arrow;
        }

        public bool Resizable
        {
            get
			{
#if _DOTNET2
				return _editor.IsDropDownResizable;
#else
				return false;
#endif
			}
        }

        protected int GetButtonWidth(Graphics graphics)
        {
            if (ReadOnly)
                return 0;

            UITypeEditorEditStyle uiStyle = _editor.GetEditStyle(_ownerPropertyEnum.Property.Value.GetTypeDescriptorContext(_ownerPropertyEnum));
            if (uiStyle == UITypeEditorEditStyle.DropDown)
                return SystemInformation.VerticalScrollBarWidth;
            else
            {
                Graphics graphics2 = (graphics != null ? graphics : CreateGraphics());

                int width = Size.Round(graphics2.MeasureString(_buttonText, Font)).Width + 6;

                if (graphics == null)
                    graphics2.Dispose();

                return width;
            }
        }

        protected Rectangle GetButtonRect(Graphics graphics)
        {
            if (ReadOnly)
                return Rectangle.Empty;

            int buttonWidth = GetButtonWidth(graphics);
            return new Rectangle(Width - buttonWidth, 0, buttonWidth, Height);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            // Draw the background

            _ownerPropertyEnum.Property.ParentGrid.DrawManager.DrawPropertyValueBackground(e.Graphics,
                ClientRectangle, _ownerPropertyEnum);

            // Draw the value
            Rectangle buttonRect = GetButtonRect(e.Graphics);

            Rectangle valueRect = ClientRectangle;
            valueRect.Width -= buttonRect.Width + 1;

            if (_edit == null)
            {
                if (Focused && (_ownerPropertyEnum.Property.InPlaceCtrlInAction == null))
                {
                    Rectangle fillRect = _ownerPropertyEnum.Property.Value.GetStringValueRect(e.Graphics, valueRect, Point.Empty);
                    fillRect.Y++;
                    fillRect.Height -= 2;
                    fillRect.X -= _ownerPropertyEnum.Property.ParentGrid.GlobalTextMargin / 2;
                    fillRect.Width = buttonRect.Left - 1 - fillRect.Left;
                    e.Graphics.FillRectangle(SystemBrushes.FromSystemColor(SystemColors.Highlight), fillRect);
                }
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

            PropertyTypeDescriptorContext context = _ownerPropertyEnum.Property.Value.GetTypeDescriptorContext(_ownerPropertyEnum);
//            if (_ownerPropertyEnum.Property.Value.TypeConverter.CanConvertFrom(context, typeof(string)))
                _ownerPropertyEnum.Property.Value.DrawValue(e.Graphics, valueRect, valueColor, _ownerPropertyEnum, Text);
  //          else
    //            _ownerPropertyEnum.Property.Value.DrawValue(e.Graphics, valueRect, valueColor, _ownerPropertyEnum, null);

            // Draw the combobox arrow

            if (!ReadOnly)
            {
                UITypeEditorEditStyle uiStyle = _editor.GetEditStyle(context);

				if (ThemeRenderer.Enabled)
				{
                    if (uiStyle == UITypeEditorEditStyle.DropDown)
                    {
						int state = (_pushed ?
							(_mouseOver ? ThemeComboBox.DropDownButtonPressed : ThemeComboBox.DropDownButtonHot) :
							(_mouseOver ? ThemeComboBox.DropDownButtonHot : ThemeComboBox.DropDownButtonNormal));
						ThemeComboBox.DropDownButton.Draw(e.Graphics, state, buttonRect);
					}
                    else if (uiStyle == UITypeEditorEditStyle.Modal)
                    {
						int state = (_pushed ?
							(_mouseOver ? ThemeButton.PushButtonPressed : ThemeButton.PushButtonHot) :
							(_mouseOver ? ThemeButton.PushButtonHot : ThemeButton.PushButtonNormal));
						ThemeButton.PushButton.Draw(e.Graphics, state, buttonRect);

                        Win32Calls.DrawText(e.Graphics, _buttonText, ref buttonRect, Font,
							OwnerPropertyEnumerator.Property.Value.ForeColor, Win32Calls.DT_SINGLELINE |
							Win32Calls.DT_VCENTER | Win32Calls.DT_CENTER);
                    }
                }
                else
                {
                    if (uiStyle == UITypeEditorEditStyle.DropDown)
                        ControlPaint.DrawComboButton(e.Graphics, buttonRect, (_pushed && _mouseOver) ?
                            ButtonState.Pushed : ButtonState.Normal);
                    else if (uiStyle == UITypeEditorEditStyle.Modal)
                    {
                        ControlPaint.DrawButton(e.Graphics, buttonRect, (_pushed && _mouseOver) ? ButtonState.Pushed : ButtonState.Normal);
                        StringFormat stringFormat = (StringFormat)StringFormat.GenericDefault.Clone();
                        stringFormat.Alignment = StringAlignment.Center;
                        stringFormat.LineAlignment = StringAlignment.Center;
                        Brush brush = new SolidBrush(Color.Black);
                        e.Graphics.DrawString(_buttonText, Font, brush, buttonRect, stringFormat);
                        brush.Dispose();
                    }
                }

                if (Focused && (_ownerPropertyEnum.Property.InPlaceCtrlInAction == null))
                {
                    Rectangle focusRect = buttonRect;
                    focusRect.Inflate(-3, -3);
                    ControlPaint.DrawFocusRectangle(e.Graphics, focusRect);
                }
            }

            base.OnPaint(e);
        }

        void OnTextboxTextChanged(object sender, EventArgs e)
        {
            Invalidate();
        }

        public bool OnForbidClickThrough(Point pt)
        {
            Graphics graphics = CreateGraphics();
            bool result = GetButtonRect(graphics).Contains(pt);
            graphics.Dispose();

            return result;
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

                    UITypeEditorEditStyle uiStyle = _editor.GetEditStyle(_ownerPropertyEnum.Property.Value.GetTypeDescriptorContext(_ownerPropertyEnum));
                    int buttonWidth = 0;
                    if (uiStyle != UITypeEditorEditStyle.None)
                        buttonWidth = GetButtonWidth(null);

//                    Rectangle editRect = new Rectangle(0, 0, clientRect.Width - buttonWidth, clientRect.Height);
  //                  editRect.Inflate(0, -1);
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
                        if ((_wmFocusMissingArg != null) && (_editorService.DropDownHolder != _wmFocusMissingArg.TopLevelControl))
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
                        int j;
                        for (j = 0; j < displayedStrings.Length; j++)
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

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (e.Clicks == 2)
                {
					Graphics graphics = CreateGraphics();
                    if (GetButtonRect(graphics).Contains(new Point(e.X, e.Y)) == false)
						ScrollValueOnDoubleClick(!(ModifierKeys == Keys.Shift),
                            ScrollValueOnDoubleClickSource.FromInPlaceCtrl,  false);
                }
                else
                {
/*                    if (_realtimeChange)
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
            if (e.KeyData == Keys.Space)
            {
                _mouseOver = true;
                _pushed = true;
                Invalidate(false);
            }
            else if ((e.KeyData == Keys.Up) || (e.KeyData == Keys.Down))
            {
                ProcessArrowKey(e.KeyData);
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

        protected void ProcessArrowKey(Keys key)
        {
            if (ReadOnly)
                return;

            string[] displayStrings = _ownerPropertyEnum.Property.Value.GetDisplayedValues();

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
                    if (displayStrings[i].Equals(Text))
                    {
                        bool change = false;

                        if ((key == Keys.Up) && (i > 0))
                        {
                            Text = displayStrings[i - 1];
                            change = true;
                        }
                        else if ((key == Keys.Down) && (i < displayStrings.Length - 1))
                        {
                            Text = displayStrings[i + 1];
                            change = true;
                        }

                        if (change && _realtimeChange)
                            CommitChanges(false);

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

            if (keyData == Keys.Return)
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

                CommitChanges(_initialValue, true, true);
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
                    _edit.Refresh();
                    return true;
                }
            }

            return base.ProcessKeyPreview(ref m);
        }

        private DropDownForm _editorContainer;

        private void SetDropDownContainer(DropDownForm container)
        {
            _editorContainer = container;
        }

        protected void RunButton()
        {
            if (ReadOnly)
                return;

            try
            {
                if (_editor.GetEditStyle() == UITypeEditorEditStyle.DropDown)
                    _ownerPropertyEnum.Property.InPlaceCtrlInAction = this;

                PropertyValue propertyValue = _ownerPropertyEnum.Property.Value;

                object value;
                PropertyTypeDescriptorContext context = propertyValue.GetTypeDescriptorContext(_ownerPropertyEnum);
                if (propertyValue.HasMultipleValues)
                    value = "";
                else if (!RealtimeChange && propertyValue.TypeConverter.CanConvertFrom(context, typeof(string)))
                {
                    try
                    {
                        value = propertyValue.TypeConverter.ConvertFromString(context, propertyValue.CultureInfo, Text);
                    }
                    catch (Exception)
                    {
                        value = propertyValue.GetValue();
                    }
                }
                else
                    value = propertyValue.GetValue();

                // Display the UI.
                IWindowsFormsEditorService service1 = (IWindowsFormsEditorService)_editorService.GetService(typeof(IWindowsFormsEditorService));
                object newValue = _editor.EditValue(context, _editorService, value);

                if (((newValue == value) && (_editorService.DialogResult != DialogResult.OK)) || _editorService.Canceled)
                    newValue = _initialValue;

                CommitChanges(newValue, true, true);

                _editorService.DialogResult = DialogResult.Cancel;

                Parent.Invalidate();

                if ((_editor.GetEditStyle() == UITypeEditorEditStyle.Modal) && !Parent.Focused)
                {
                    // Necessary for modal editors that don't call IWindowsFormsEditorService.ShowDialog
                    Parent.Focus();
                }
            }
            catch (Exception)
            {
            }

            if (_editor.GetEditStyle() == UITypeEditorEditStyle.DropDown)
                _ownerPropertyEnum.Property.InPlaceCtrlInAction = null;

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
            if ((_edit != null) && !_edit.ReadOnly && _edit.OnParentKeyDown(e))
                return true;

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
            return CommitChanges(Text, false, final);
        }

        protected PropertyValue.ValueValidationResult CommitChanges(object newValue, bool fromObject, bool final)
        {
            PropertyValue.ValueValidationResult changed = PropertyValue.ValueValidationResult.PreviousValueRestored;

            object oldValue = null;

            PropertyValue value = _ownerPropertyEnum.Property.Value;
            bool compareTextInCommitChanges = value.TypeConverter.CanConvertFrom(typeof(string)) && (newValue is string);
            bool textHasChanged = false;
            bool multipleValues = value.HasMultipleValues;

            if (compareTextInCommitChanges && (newValue is string))
            {
                oldValue = value.DisplayString;
                textHasChanged = !value.DisplayString.Equals(newValue as string);
            }

            if (!compareTextInCommitChanges || textHasChanged)
            {
                // Store the previous value
                if (!multipleValues)
                    oldValue = value.GetValue();

                if (!fromObject)
                {
                    try
                    {
                        if (!multipleValues || !(newValue is string) || ((newValue as string).Length != 0))
                            newValue = value.GetActualValue(newValue as string);
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
            }

            if ((multipleValues && textHasChanged) ||
                ((newValue != null) && ((_editorService.DialogResult == DialogResult.OK) || (oldValue == null) ||
                (newValue.Equals(oldValue) == false))) ||
                ((newValue == null) && (oldValue != null)))
            {
                // Freeze painting because updating the property may also update other properties in the grid.
                _ownerPropertyEnum.Property.ParentGrid.BeginUpdate();

                _ownerPropertyEnum.Property.Value.PreviousValue = _ownerPropertyEnum.Property.Value.GetValue();

                PropertyEnumerator invalidPropertyEnum;
                changed = value.SetValueFromInPlaceCtrl(newValue, out invalidPropertyEnum);

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
                        _initialValue = newValue;
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

                if (final)
                    _initialValue = newValue;
            }

            return changed;
        }

        public Control GetDropDownContainer()
        {
            return _editorContainer;
        }
    }
}
