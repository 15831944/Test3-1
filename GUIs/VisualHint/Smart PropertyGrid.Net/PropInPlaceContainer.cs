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
using System.Security.Permissions;
using System.Drawing.Drawing2D;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Parent form that is displayed as the listbox of an inplace control combobox.
    /// </summary>
    public class PropInPlaceContainer : Form, IMessageHookListener
    {
        internal event EventHandler MustBeClosed;

        private IDropDownContent _content = null;

        private enum MoveDirection
        {
            None = 0,
            Vertical = 1,
            NESW = 2,
            NWSE = 3
        }

        private MoveDirection _currentMoveDirection = MoveDirection.None;

        private Point _resizeStartPoint = Point.Empty;

        private bool _resizable = false;

        public bool Resizable
        {
            set { _resizable = value; }
            get { return _resizable; }
        }

        public static readonly int ResizeGripHeight = 0;

        public static readonly Size MinSize;

        private bool _resizeTop = false;

        public bool ResizeTop
        {
            set
            {
                DockPadding.Top = 0;
                DockPadding.Bottom = 0;

                _resizeTop = value;
                if (_resizable)
                {
                    _resizeGripImage = null;

                    if (_resizeTop)
                        DockPadding.Top = ResizeGripHeight + 1;
                    else
                        DockPadding.Bottom = ResizeGripHeight + 1;
                }
            }
            get { return _resizeTop; }
        }

        public void SetContent(PropertyEnumerator propEnum, string currentValue)
        {
            SetContent(propEnum, currentValue, null);
        }

        public void SetContent(PropertyEnumerator propEnum, string currentValue, object valueKey)
        {
            if (propEnum == null)
            {
                if (_content != null)
                {
                    (_content as Control).MouseUp -= new MouseEventHandler(OnContentClicked);
                    Controls.Remove(_content as Control);
                    (_content as Control).Dispose();
                    _content = null;
                }
            }
            else
            {
                _content = (IDropDownContent)propEnum.Property.ParentGrid.GetDropDownContent(propEnum, valueKey);

                Control contentControl = _content as Control;
                if (contentControl != null)
                {
                    contentControl.Parent = this;
                    contentControl.BackColor = propEnum.Property.Value.BackColor;
                    contentControl.ForeColor = propEnum.Property.Value.ForeColor;
                    contentControl.Font = propEnum.Property.ParentGrid.Font;
                    contentControl.MouseUp += new MouseEventHandler(OnContentClicked);
                    _content.InitializeContent(propEnum, currentValue, valueKey);

                    Size = new Size(contentControl.Width + 2, contentControl.Height + 2);

                    DropDownContentListBox dropdown = (ContentControl as DropDownContentListBox);
                    if (dropdown != null)
                    {
                        if (dropdown.Items.Count > (dropdown.Height / dropdown.ItemHeight))
                        {
                            Height += SystemInformation.HorizontalScrollBarHeight + 1;
                            Resizable = true;
                        }
                        else
                            Resizable = false;
                    }
                }
            }
        }

        public IDropDownContent DropDownContent
        {
            get { return _content; }
        }

        public Control ContentControl
        {
            get { return _content as Control; }
        }

        public int SelectedIndex
        {
            get
            {
                if (_content != null)
                    return _content.GetSelectedIndex();
                else
                    return -1;
            }
        }

        static PropInPlaceContainer()
        {
            ResizeGripHeight = SystemInformation.HorizontalScrollBarHeight;
            MinSize = new Size(SystemInformation.VerticalScrollBarWidth * 4, SystemInformation.HorizontalScrollBarHeight * 4);
        }

        public PropInPlaceContainer()
        {
            FormBorderStyle = FormBorderStyle.None;

            SetStyle(Utilities.DoubleBufferStyle, true);
            SetStyle(ControlStyles.ResizeRedraw, false);
            SetStyle(ControlStyles.UserMouse, true);
        }


        [CLSCompliant(false)]
        public bool OnListenMessageHook(Control control, uint message, IntPtr wParam, IntPtr lParam)
        {
			if (message == Win32Calls.WM_KILLFOCUS)
			{
				if (control != null)
				{
					if (Contains(control))
					{
						Control wndTakingFocus = FromHandle(wParam);
						if ((wndTakingFocus != this) && !Contains(wndTakingFocus))
							FireMustBeClosedEvent(true, wndTakingFocus);
					}
				}
			}

            return false;
        }

		private bool _insideFireMustBeClosedEvent = false;

        private void FireMustBeClosedEvent(bool reasonLostFocus, Control wndTakingFocus)
        {
            if (_insideFireMustBeClosedEvent)
                return;

            _insideFireMustBeClosedEvent = true;

            if (MustBeClosed != null)
                MustBeClosed(this, new DropDownContentClosedEventArgs(reasonLostFocus, wndTakingFocus));

            _insideFireMustBeClosedEvent = false;
        }

        private void FireMustBeClosedEvent(bool toBeValidated)
        {
            if (_insideFireMustBeClosedEvent)
                return;

            _insideFireMustBeClosedEvent = true;

            if (MustBeClosed != null)
                MustBeClosed(this, new DropDownContentClosedEventArgs(toBeValidated));

            _insideFireMustBeClosedEvent = false;
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

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            if (_resizable)
            {
                Rectangle rectangle = new Rectangle(0, ResizeTop ? -1 : (Height - ResizeGripHeight - 3), ResizeGripHeight, ResizeGripHeight);
                e.Graphics.DrawImage(GetResizeGripImage(e.Graphics), rectangle);
                int lineY = ResizeTop ? ContentControl.Top - 1 : ContentControl.Bottom;
                Pen pen = new Pen(SystemColors.ControlDark, 1f);
                e.Graphics.DrawLine(pen, 0, lineY, Width, lineY);
                pen.Dispose();
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

        void OnContentClicked(object sender, MouseEventArgs e)
        {
            if (_content.HasToBeClosedOnClick())
                FireMustBeClosedEvent(true);
        }

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override bool ProcessKeyPreview(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_KEYDOWN)
            {
                Keys key = (Keys)(int)m.WParam;
                if ((key == Keys.Return) || (key == Keys.F4))
                {
                    FireMustBeClosedEvent(true);
                    return true;
                }
                else if (key == Keys.Escape)
                {
                    FireMustBeClosedEvent(false);
                    return true;
                }
            }
            else if (m.Msg == Win32Calls.WM_SYSKEYDOWN)
            {
                Keys key = (Keys)(int)m.WParam;
                if ((key == Keys.Down) || (key == Keys.Up))
                {
                    FireMustBeClosedEvent(true);
                    return true;
                }
            }

            return false;
        }

        protected override void  OnVisibleChanged(EventArgs e)
        {
            _resizeGripImage = null;
            base.OnVisibleChanged(e);
        }

        protected override CreateParams CreateParams
        {
            [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
            get
            {
                CreateParams cp = base.CreateParams;

                cp.ExStyle = Win32Calls.WS_EX_TOOLWINDOW | Win32Calls.WS_EX_TOPMOST;
                cp.Style = Win32Calls.WS_POPUP | Win32Calls.WS_BORDER /*| Win32Calls.WS_VSCROLL | Win32Calls.WS_HSCROLL*/;

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

        public void DoModalLoop()
        {
            Focus();

            while (Visible)
            {
                Application.DoEvents();
    			Win32Calls.MsgWaitForMultipleObjects(0, IntPtr.Zero, true, 250, 0xff);
            }
        }
    }
}
