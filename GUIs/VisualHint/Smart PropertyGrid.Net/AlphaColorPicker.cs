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
using System.Drawing.Drawing2D;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Security.Permissions;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Control used in the dropdown area of a combobox inplace control to let the user edit a color
    /// with an alpha component.
    /// </summary>
    /// <remarks>It can only be used with color properties.
    /// </remarks>
    /// <example>
    /// To associate this control with a property of the grid, use the following code if you want to link to
    /// one of your C# properties:
    /// <code>
    /// [PropertyDropDownContent(typeof(AlphaColorPicker))]
    /// public Color MyColor
    /// {
    ///     set { _color = value; }
    ///     get { return _color; }
    /// }
    /// </code>
    /// If you want to create a property not linked to a C# property, do this:
    /// <code>
    /// PropertyEnumerator propEnum = AppendManagedProperty(parentEnum, id, "Color", typeof(Color),
    ///    Color.Red, "", new PropertyDropDownContentAttribute(typeof(AlphaColorPicker)));
    /// </code>
    /// In both cases you have to associate the property with the right feel and look:
    /// <code>
    /// propEnum.Property.Feel = GetRegisteredFeel(VisualHint.SmartPropertyGrid.PropertyGrid.FeelList);
    /// propEnum.Property.Value.Look = new PropertyAlphaColorLook();
    /// </code>
    /// </example>
    [ToolboxItem(false)]
    public class AlphaColorPicker : Control, IDropDownContent, IColorComponentDrawingImplementer
    {
        private CustomTrackBar _hueBar;

        private SaturationBrightnessMapPicker _sbMapPicker = new SaturationBrightnessMapPicker();

        private CustomTrackBar _alphaBar;

        private TextBox _rEdit = new TextBox();
        private TextBox _gEdit = new TextBox();
        private TextBox _bEdit = new TextBox();
        private TextBox _aEdit = new TextBox();

        private Label _rLabel = new Label();
        private Label _gLabel = new Label();
        private Label _bLabel = new Label();
        private Label _aLabel = new Label();

        private bool _preventReentrantCalls = false;

        private Color _color;

        public override Font Font
        {
            get { return base.Font; }

            set
            {
            }
        }


        public int GetSelectedIndex()
        {
            return -1;
        }

        public AlphaColorPicker()
        {
            TabStop = false;
            _hueBar = new CustomTrackBar(this);
            _alphaBar = new CustomTrackBar(this);
        }

        public void InitializeContent(PropertyEnumerator propEnum, string currentValue, object valueKey)
        {
            _color = (Color)propEnum.Property.Value.GetValue();

            Width = 200;
            Height = 212;
            BackColor = SystemColors.Control;

            double h, s, b;
            ColorUtils.RgbToHsb(_color, out h, out s, out b);

            _hueBar.Bounds = new Rectangle(35, 10, 155, 12);
            _hueBar.Margins = new Win32Calls.RECT(25, 1, 1, 1);
            _hueBar.Label = "Hue";
            _hueBar.Value = (int)Math.Round(h / 360.0 * 255.0);
			_hueBar.ValueChanged += new EventHandler(OnTrackBarValueChanged);
            _hueBar.TabIndex = 2;

            _sbMapPicker.Color = _color;
            _sbMapPicker.Bounds = new Rectangle(40, 30, 150, 150);
            _sbMapPicker.ValueChanged += new EventHandler(OnSbMapValueChanged);
            _sbMapPicker.TabIndex = 3;

            _alphaBar.Bounds = new Rectangle(10, 10, 20, 100);
            _alphaBar.Margins = new Win32Calls.RECT(1, 1, 1, 1);
            _alphaBar.Orientation = Orientation.Vertical;
            _alphaBar.ReversedOrigin = true;
            _alphaBar.Value = _color.A;
            _alphaBar.ValueChanged += new EventHandler(OnTrackBarValueChanged);
            _alphaBar.TabIndex = 1;

            _rLabel.TextAlign = ContentAlignment.MiddleLeft;
            _rLabel.Bounds = new Rectangle(10, 185, 15, 19);
            _rLabel.Text = "R";
            _rEdit.AutoSize = false;
            _rEdit.Bounds = new Rectangle(25, 185, 26, 19);
            _rEdit.Text = _color.R.ToString();
            _rEdit.MaxLength = 3;
            _rEdit.KeyPress += new KeyPressEventHandler(OnEditKeyPress);
            _rEdit.TextChanged += new EventHandler(OnEditTextChanged);

            _gLabel.TextAlign = ContentAlignment.MiddleLeft;
            _gLabel.Bounds = new Rectangle(56, 185, 15, 19);
            _gLabel.Text = "G";
            _gEdit.AutoSize = false;
            _gEdit.Bounds = new Rectangle(71, 185, 26, 19);
            _gEdit.Text = _color.G.ToString();
            _gEdit.MaxLength = 3;
            _gEdit.KeyPress += new KeyPressEventHandler(OnEditKeyPress);
            _gEdit.TextChanged += new EventHandler(OnEditTextChanged);

            _bLabel.TextAlign = ContentAlignment.MiddleLeft;
            _bLabel.Bounds = new Rectangle(102, 185, 15, 19);
            _bLabel.Text = "B";
            _bEdit.AutoSize = false;
            _bEdit.Bounds = new Rectangle(117, 185, 26, 19);
            _bEdit.Text = _color.B.ToString();
            _bEdit.MaxLength = 3;
            _bEdit.KeyPress += new KeyPressEventHandler(OnEditKeyPress);
            _bEdit.TextChanged += new EventHandler(OnEditTextChanged);

            _aLabel.TextAlign = ContentAlignment.MiddleLeft;
            _aLabel.Bounds = new Rectangle(148, 185, 15, 19);
            _aLabel.Text = "A";
            _aEdit.AutoSize = false;
            _aEdit.Bounds = new Rectangle(163, 185, 26, 19);
            _aEdit.Text = _color.A.ToString();
            _aEdit.MaxLength = 3;
            _aEdit.KeyPress += new KeyPressEventHandler(OnEditKeyPress);
            _aEdit.TextChanged += new EventHandler(OnEditTextChanged);

            Controls.Add(_hueBar);
            Controls.Add(_sbMapPicker);
            Controls.Add(_alphaBar);
            Controls.Add(_rEdit);
            Controls.Add(_gEdit);
            Controls.Add(_bEdit);
            Controls.Add(_aEdit);
            Controls.Add(_rLabel);
            Controls.Add(_gLabel);
            Controls.Add(_bLabel);
            Controls.Add(_aLabel);
        }

        /// <summary>
        /// Returns the color modified by the user.
        /// </summary>
        /// <returns>the color modified by the user.</returns>
        public object GetValue()
        {
            return _color;
        }

        /// <summary>
        /// Indicates to the PropertyGrid that the color picker must not be closed when the user clicks it.
        /// </summary>
        /// <remarks>Contrarily to a dropdown listbox, where a click terminates a selection, false is returned
        /// here because the color picker is composed of several color boxes that react on a mouse click.</remarks>
        /// <returns>false to indicate to the PropertyGrid that the color picker must not be closed when the user
        /// clicks it.</returns>
        public bool HasToBeClosedOnClick()
        {
            return false;
        }

        /// <summary>
        /// Draws the content of a gradient colored trackbar.
        /// </summary>
        /// <param name="sender">The trackbar that requests that its content be painted by this object.</param>
        /// <param name="graphics">A reference to a graphics object used for painting.</param>
        /// <param name="rect">The clipping rectangle used for drawing.</param>
        /// <remarks>Because the internal trackbar class is generic, this is up to the AlphaColorPicker to draw
        /// the correct content inside each trackbar used in the picker. In this case, it is called for the hue
        /// bar and for the alpha bar.
        /// </remarks>
        public void DrawContent(Control sender, Graphics graphics, Rectangle rect)
        {
            if (sender == _hueBar)
            {
                using (LinearGradientBrush brush = new LinearGradientBrush(rect, Color.Black, Color.Black, 0f, false))
                {
                    ColorBlend blend = new ColorBlend();
                    blend.Colors = new Color[7] { Color.FromArgb(255, 0, 0), Color.FromArgb(255, 255, 0), Color.FromArgb(0, 255, 0), Color.FromArgb(0, 255, 255), Color.FromArgb(0, 0, 255), Color.FromArgb(255, 0, 255), Color.FromArgb(255, 0, 0) };
                    blend.Positions = new float[7] { 0f, 0.1667f, 0.3372f, 0.502f, 0.6686f, 0.8313f, 1f };
                    brush.InterpolationColors = blend;
                    graphics.FillRectangle(brush, rect);
                }
            }
            else if (sender == _alphaBar)
            {
                Brush whiteBrush = new SolidBrush(Color.White);
                Brush grayBrush = new SolidBrush(Color.Gray);
                for (int y = rect.Top; y <= rect.Bottom; y += 4)
                {
                    int height = (y + 4 > rect.Bottom ? rect.Bottom - y : 4);
                    for (int x = rect.Left; x <= rect.Right; x += 5)
                    {
                        int width = (x + 5 > rect.Right ? rect.Right - x : 5);
                        if (((x - rect.Left) % 10 == ((y - rect.Top) % 8 == 0 ? 0 : 5)))
                            graphics.FillRectangle(whiteBrush, x, y, width, height);
                        else
                            graphics.FillRectangle(grayBrush, x, y, width, height);
                    }
                }

                whiteBrush.Dispose();
                grayBrush.Dispose();

                // Draw the color with alpha
                Brush brush = new LinearGradientBrush(rect, Color.FromArgb(255, _color), Color.FromArgb(0, _color), 90.0f);
                graphics.FillRectangle(brush, rect);
                brush.Dispose();
            }
        }

        void OnTrackBarValueChanged(object sender, EventArgs e)
        {
            if (_preventReentrantCalls == false)
            {
                if (sender == _hueBar)
                {
                    double h = _hueBar.Value / 255.0 * 359.0;
                    _sbMapPicker.Hue = h;

                    OnSbMapValueChanged(sender, e);
                }
                else if (sender == _alphaBar)
                {
                    _color = Color.FromArgb(_alphaBar.Value, _color);
                    _sbMapPicker.Color = _color;
                    _aEdit.Text = _color.A.ToString();
                }
            }
        }

        void OnSbMapValueChanged(object sender, EventArgs e)
        {
            _color = _sbMapPicker.Color;
            _alphaBar.Invalidate();

            _preventReentrantCalls = true;
            _rEdit.Text = _color.R.ToString();
            _gEdit.Text = _color.G.ToString();
            _bEdit.Text = _color.B.ToString();
            _aEdit.Text = _color.A.ToString();
            _preventReentrantCalls = false;
        }

        void OnEditKeyPress(object sender, KeyPressEventArgs e)
        {
            if ((e.KeyChar != 8) && !Char.IsDigit(e.KeyChar))
                e.Handled = true;
        }

        void OnEditTextChanged(object sender, EventArgs e)
        {
            if (_preventReentrantCalls == false)
            {
                TextBox tb = sender as TextBox;
                int value = (tb.Text.Length > 0 ? Int32.Parse(tb.Text) : 0);

                if (value > 255)
                    value = 255;

                if (sender == _rEdit)
                    _color = Color.FromArgb(_color.A, value, _color.G, _color.B);
                else if (sender == _gEdit)
                    _color = Color.FromArgb(_color.A, _color.R, value, _color.B);
                else if (sender == _bEdit)
                    _color = Color.FromArgb(_color.A, _color.R, _color.G, value);
                else if (sender == _aEdit)
                    _color = Color.FromArgb(value, _color.R, _color.G, _color.B);

                _alphaBar.Value = _color.A;
                _sbMapPicker.ResetColor();
                _sbMapPicker.Color = _color;

                double h, s, b;
                ColorUtils.RgbToHsb(_color, out h, out s, out b);
                _preventReentrantCalls = true;
                _hueBar.Value = (int)Math.Round(h / 360.0 * 255.0);
                _preventReentrantCalls = false;
            }
        }
    }

    interface IColorComponentDrawingImplementer
    {
        void DrawContent(Control sender, Graphics graphics, Rectangle rect);
    }

    interface IColorComponentTracker
    {
        void Draw(Graphics graphics, int value, Point pt1, Point pt2, Rectangle clientRectangle);
    }

    class TriangleTracker : IColorComponentTracker
    {
        public void Draw(Graphics graphics, int value, Point pt1, Point pt2, Rectangle clientRectangle)
        {
        }
    }

    class SimpleLineTracker : IColorComponentTracker
    {
        public void Draw(Graphics graphics, int value, Point pt1, Point pt2, Rectangle clientRectangle)
        {
            Pen pen = new Pen(Color.Black, 2);
            graphics.DrawLine(pen, pt1, pt2);
            pen.Dispose();
        }
    }

    internal class CustomTrackBar : Control
    {
        public event EventHandler ValueChanged;

        private bool _dragInProgress = false;

        public enum Orientations
        {
            Horizontal,
            Vertical
        }

        private Orientation _orientation = Orientation.Horizontal;

        public Orientation Orientation
        {
            get { return _orientation; }
            set { _orientation = value; }
        }

        private string _label;

        public String Label
        {
            get { return _label; }
            set { _label = value; }
        }

        private IColorComponentTracker _tracker = new SimpleLineTracker();

        public IColorComponentTracker Tracker
        {
            set
            {
                if ((value == null) && (value.GetType() != typeof(SimpleLineTracker)))
                {
                    _tracker = new SimpleLineTracker();
                }
                else if (_tracker.GetType() != value.GetType())
                {
                    _tracker = value;
                }
            }

            get { return _tracker; }
        }

        private IColorComponentDrawingImplementer _drawingImplementer;

        private int _minimum = 0;

        public int Minimum
        {
            set { _minimum = value; }
            get { return _minimum; }
        }

        private int _maximum = 255;

        public int Maximum
        {
            set { _maximum = value; }
            get { return _maximum; }
        }

        private bool _reversedOrigin = false;

        public bool ReversedOrigin
        {
            set { _reversedOrigin = value; }
        }

        private int _value;

        public int Value
        {
            set
            {
                int oldValue = _value;

                if ((value >= _minimum) && (value <= _maximum))
                    _value = value;

                if ((ValueChanged != null) && (oldValue != _value))
                    ValueChanged(this, EventArgs.Empty);

                Invalidate();
            }

            get { return _value; }
        }

        private int _smallChange = 1;

        public int SmallChange
        {
            set { _smallChange = value; }
            get { return _smallChange; }
        }

        private int _largeChange = 10;

        public int LargeChange
        {
            set { _largeChange = value; }
            get { return _largeChange; }
        }

        private Rectangle _contentRect = Rectangle.Empty;

        private Win32Calls.RECT _margins;

        public Win32Calls.RECT Margins
        {
            set { _margins = value; }
            get { return _margins; }
        }

        public CustomTrackBar(IColorComponentDrawingImplementer implementer)
        {
            _drawingImplementer = implementer;

            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque, true);
        }

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_ERASEBKGND)
            {
                // If I don't intercept this message, the control flickers
                return;
            }

            base.WndProc(ref m);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Rectangle clientRect = ClientRectangle;

            Brush brush = new SolidBrush(BackColor);
            e.Graphics.FillRectangle(brush, clientRect);
            brush.Dispose();

            if (_orientation == Orientation.Horizontal)
            {
                // Draw label
                Font font = new Font("Tahoma", 8);
                Size strSize = Size.Round(e.Graphics.MeasureString(_label, font));
                Rectangle strRect = clientRect;
                strRect.Width = strSize.Width;
                strRect.Y = (int)((ContentRect.Top + ContentRect.Bottom - strSize.Height) / 2.0);
                strRect.Height = strSize.Height;
                StringFormat stringFormat = (StringFormat)StringFormat.GenericDefault.Clone();
                stringFormat.LineAlignment = StringAlignment.Center;
                e.Graphics.DrawString(_label, font, SystemBrushes.WindowText, strRect, stringFormat);
                font.Dispose();
            }

            // Draw border
            Rectangle borderRect = ContentRect;
            borderRect.Inflate(1,1);
            e.Graphics.DrawLine(SystemPens.ControlDarkDark, new Point(borderRect.Left, borderRect.Bottom - 2), borderRect.Location);
            e.Graphics.DrawLine(SystemPens.ControlDarkDark, borderRect.Location, new Point(borderRect.Right - 2, borderRect.Top));
            e.Graphics.DrawLine(SystemPens.ControlLightLight, new Point(borderRect.Left, borderRect.Bottom - 1), new Point(borderRect.Right - 1, borderRect.Bottom - 1));
            e.Graphics.DrawLine(SystemPens.ControlLightLight, new Point(borderRect.Right - 1, borderRect.Bottom - 1), new Point(borderRect.Right - 1, borderRect.Top));

            if (Focused)
            {
                borderRect.Inflate(1, 1);
                ControlPaint.DrawFocusRectangle(e.Graphics, borderRect);
            }

            // Draw content
            _drawingImplementer.DrawContent(this, e.Graphics, ContentRect);

            // Draw tracker
            int leftOrTop, valuePos, rightOrBottom;
            GetPositions(out leftOrTop, out rightOrBottom, out valuePos);
            if (_orientation == Orientation.Horizontal)
                _tracker.Draw(e.Graphics, _value, new Point(valuePos, ContentRect.Top), new Point(valuePos, ContentRect.Bottom), clientRect);
            else if (_orientation == Orientation.Vertical)
                _tracker.Draw(e.Graphics, _value, new Point(ContentRect.Left, valuePos), new Point(ContentRect.Right, valuePos), clientRect);
        }

        protected Rectangle ContentRect
        {
            get
            {
                if (_contentRect.IsEmpty)
                {
                    Rectangle clientRect = ClientRectangle;

                    Rectangle borderRect = new Rectangle();
                    borderRect.X = _margins.Left;
                    borderRect.Y = _margins.Top;
                    borderRect.Width = clientRect.Right - _margins.Right - borderRect.Left;
                    borderRect.Height = clientRect.Bottom - _margins.Bottom - borderRect.Top;
                    _contentRect = borderRect;
                    _contentRect.Inflate(-1, -1);
                }

                return _contentRect;
            }
        }

        protected void GetPositions(out int leftOrTop, out int rightOrBottom, out int trackerPos)
        {
            leftOrTop = ContentRect.Left;
            rightOrBottom = ContentRect.Right;

            if (_reversedOrigin)
            {
                if (_orientation == Orientation.Horizontal)
                    trackerPos = ContentRect.Right - (int)Math.Round((double)ContentRect.Width * (double)(_value - _minimum) / _maximum);
                else
                    trackerPos = ContentRect.Bottom - (int)Math.Round((double)ContentRect.Height * (double)(_value - _minimum) / _maximum);
            }
            else
            {
                if (_orientation == Orientation.Horizontal)
                    trackerPos = ContentRect.Left + (int)Math.Round((double)ContentRect.Width * (double)(_value - _minimum) / _maximum);
                else
                    trackerPos = ContentRect.Top + (int)Math.Round((double)ContentRect.Height * (double)(_value - _minimum) / _maximum);
            }
        }

        protected int GetValueFromPosition(int trackerPos)
        {
            if (_reversedOrigin)
            {
                if (_orientation == Orientation.Horizontal)
                    return _minimum + (int)Math.Round((double)(ContentRect.Right - trackerPos) / (double)ContentRect.Width * _maximum);
                else if (_orientation == Orientation.Vertical)
                    return _minimum + (int)Math.Round((double)(ContentRect.Bottom - trackerPos) / (double)ContentRect.Height * _maximum);
            }
            else
            {
                if (_orientation == Orientation.Horizontal)
                    return _minimum + (int)Math.Round((double)(trackerPos - ContentRect.Left) / (double)ContentRect.Width * _maximum);
                else if (_orientation == Orientation.Vertical)
                    return _minimum + (int)Math.Round((double)(trackerPos - ContentRect.Top) / (double)ContentRect.Height * _maximum);
            }

            return _minimum;
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (_orientation == Orientation.Horizontal)
                {
                    if ((e.X >= ContentRect.Left) && (e.X <= ContentRect.Right))
                    {
                        Focus();
                        Value = GetValueFromPosition(e.X);
                        _dragInProgress = true;
                    }
                }
                else if (_orientation == Orientation.Vertical)
                {
                    if ((e.Y >= ContentRect.Top) && (e.Y <= ContentRect.Bottom))
                    {
                        Focus();
                        Value = GetValueFromPosition(e.Y);
                        _dragInProgress = true;
                    }
                }
            }

            base.OnMouseDown(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (_dragInProgress)
            {
                if (_orientation == Orientation.Horizontal)
                {
                    int position = e.X;
                    if (position < ContentRect.Left)
                        position = ContentRect.Left;
                    else if (position > ContentRect.Right)
                        position = ContentRect.Right;

                    Value = GetValueFromPosition(position);
                }
                else if (_orientation == Orientation.Vertical)
                {
                    int position = e.Y;
                    if (position < ContentRect.Top)
                        position = ContentRect.Top;
                    else if (position > ContentRect.Bottom)
                        position = ContentRect.Bottom;

                    Value = GetValueFromPosition(position);
                }
            }

            base.OnMouseMove(e);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            if (_dragInProgress)
                _dragInProgress = false;

            base.OnMouseUp(e);
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if (((_orientation == Orientation.Horizontal) && ((keyData == Keys.Left) || (keyData == Keys.Right))) ||
                ((_orientation == Orientation.Vertical) && ((keyData == Keys.Up) || (keyData == Keys.Down))))
                return true;

            return base.IsInputKey(keyData);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.KeyData == Keys.Left)
                Value -= SmallChange;
            else if (e.KeyData == Keys.Down)
                Value -= SmallChange;
            else if (e.KeyData == Keys.Right)
                Value += SmallChange;
            else if (e.KeyData == Keys.Up)
                Value += SmallChange;
            else if (e.KeyData == Keys.Home)
                Value = Minimum;
            else if (e.KeyData == Keys.End)
                Value = Maximum;
            else if (e.KeyData == Keys.PageUp)
                Value += LargeChange;
            else if (e.KeyData == Keys.PageDown)
                Value -= LargeChange;

            base.OnKeyDown(e);
        }

        protected override void OnGotFocus(EventArgs e)
        {
            Invalidate();
            base.OnGotFocus(e);
        }

        protected override void OnLostFocus(EventArgs e)
        {
            Invalidate();
            base.OnLostFocus(e);
        }
    }

    internal class SaturationBrightnessMapPicker : Control
    {
        public event EventHandler ValueChanged;

        private double _hue, _saturation, _brightness;

        public double Hue
        {
            set
            {
                _hue = value;
                _color = Color.FromArgb(_color.A, ColorUtils.HsbToRgb(_hue, _saturation, _brightness));
                _needToRecrateInternalImage = true;
                Invalidate();
            }
        }

        private Color _color = Color.Empty;

        public Color Color
        {
            set
            {
                if (_color == Color.Empty)
                {
                    _color = value;
                    ColorUtils.RgbToHsb(_color, out _hue, out _saturation, out _brightness);
                }
                else
                    _color = value;

                _needToRecrateInternalImage = true;

                Invalidate();
            }

            get { return _color; }
        }

        public void ResetColor()
        {
            _color = Color.Empty;
        }

        private bool _dragInProgress = false;

        public SaturationBrightnessMapPicker()
        {
            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque, true);
        }

        // To optimize the drawing of the saturation/brightness map
        private bool _needToRecrateInternalImage = true;
        private Image _saturationBrightnessImage;

        protected override void OnPaint(PaintEventArgs e)
        {
            Rectangle rect = ClientRectangle;

            if (_needToRecrateInternalImage)
            {
                _saturationBrightnessImage = new Bitmap(rect.Width, rect.Height, e.Graphics);
                _needToRecrateInternalImage = false;

                Graphics gr = Graphics.FromImage(_saturationBrightnessImage);

                Color color = ColorUtils.HsbToRgb(_hue, 100.0, 100.0);

                double r = ((double)(255 - color.R)) / rect.Width;
                double g = ((double)(255 - color.G)) / rect.Width;
                double b = ((double)(255 - color.B)) / rect.Width;
                double num4 = 255;
                double num5 = 255;
                double num6 = 255;
                for (int num7 = rect.Left; num7 <= rect.Right; num7++)
                {
                    using (LinearGradientBrush brush1 = new LinearGradientBrush(new Rectangle(num7, rect.Top, 1, rect.Height + 1), Color.FromArgb((int)Math.Round(num4), (int)Math.Round(num5), (int)Math.Round(num6)), Color.FromArgb(0, 0, 0), 90f, false))
                    {
                        gr.FillRectangle(brush1, new Rectangle(num7, rect.Top, 1, rect.Height + 1));
                    }
                    num4 -= r;
                    num5 -= g;
                    num6 -= b;
                }

                gr.Dispose();
            }

            e.Graphics.DrawImage(_saturationBrightnessImage, 0, 0);

            // Draw the color tracker
            int x = (int)Math.Round(_saturation / 100.0 * rect.Width);
            int y = rect.Height - (int)Math.Round(_brightness / 100.0 * rect.Height);
            Rectangle circleRect = new Rectangle(x - 5, y - 5, 10, 10);
            Pen pen = new Pen(Color.White);
            e.Graphics.DrawEllipse(pen, circleRect);
            pen.Dispose();
            circleRect.Inflate(1, 1);
            pen = new Pen(Color.Black);
            e.Graphics.DrawEllipse(pen, circleRect);
            pen.Dispose();
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_NCCALCSIZE)
            {
                Win32Calls.NCCALCSIZE_PARAMS csp;

                csp = (Win32Calls.NCCALCSIZE_PARAMS)Marshal.PtrToStructure(m.LParam, typeof(Win32Calls.NCCALCSIZE_PARAMS));
                Rectangle rect = new Rectangle(csp.rgrc0.Left, csp.rgrc0.Top,
                    csp.rgrc0.Right - csp.rgrc0.Left, csp.rgrc0.Bottom - csp.rgrc0.Top);
                rect.Inflate(-2, -2);
                csp.rgrc0.Left = rect.X;
                csp.rgrc0.Right = rect.X + rect.Width;
                csp.rgrc0.Top = rect.Y;
                csp.rgrc0.Bottom = rect.Y + rect.Height;
                Marshal.StructureToPtr(csp, m.LParam, false);
            }
            else if (m.Msg == Win32Calls.WM_NCPAINT)
            {
                IntPtr hDC = Win32Calls.GetWindowDC(m.HWnd);
                Graphics graphics = Graphics.FromHdc(hDC);

                Win32Calls.RECT rc = new Win32Calls.RECT();
                Win32Calls.GetWindowRect(m.HWnd, ref rc);
                Rectangle rect = new Rectangle(0, 0, rc.Right - rc.Left, rc.Bottom - rc.Top);

                if (Focused)
                    ControlPaint.DrawFocusRectangle(graphics, rect);
                else
                {
                    Rectangle bkRect = rect;
                    bkRect.Width--;
                    bkRect.Height--;
                    Pen pen = new Pen(Parent.BackColor);
                    graphics.DrawRectangle(pen, bkRect);
                    pen.Dispose();
                }

                rect.Inflate(-1, -1);
                rect.Width--;
                rect.Height--;
                graphics.DrawLine(SystemPens.ControlDarkDark, new Point(rect.Left, rect.Bottom), rect.Location);
                graphics.DrawLine(SystemPens.ControlDarkDark, rect.Location, new Point(rect.Right, rect.Top));
                graphics.DrawLine(SystemPens.ControlLightLight, new Point(rect.Left, rect.Bottom), new Point(rect.Right, rect.Bottom));
                graphics.DrawLine(SystemPens.ControlLightLight, new Point(rect.Right, rect.Bottom), new Point(rect.Right, rect.Top));

                Win32Calls.ReleaseDC(m.HWnd, hDC);
            }

            base.WndProc(ref m);
        }

        protected override void OnGotFocus(EventArgs e)
        {
            // Redraw the non client area for the focus rectangle
            Win32Calls.RedrawWindow(Handle, IntPtr.Zero, IntPtr.Zero, 0x0505/*RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME*/);

            base.OnGotFocus(e);
        }

        protected override void OnLostFocus(EventArgs e)
        {
            // Redraw the non client area to erase the focus rectangle
            Win32Calls.RedrawWindow(Handle, IntPtr.Zero, IntPtr.Zero, 0x0505/*RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME*/);

            base.OnLostFocus(e);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                Focus();

                UpdateColor(new Point(e.X, e.Y));

                _dragInProgress = true;
            }

            base.OnMouseDown(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (_dragInProgress)
                UpdateColor(new Point(e.X, e.Y));

            base.OnMouseMove(e);
        }

        protected void UpdateColor(Point mousePos)
        {
            Rectangle clientRect = ClientRectangle;
            if (mousePos.X < 0) mousePos.X = 0;
            if (mousePos.Y < 0) mousePos.Y = 0;
            if (mousePos.X > clientRect.Right - 1) mousePos.X = clientRect.Right - 1;
            if (mousePos.Y > clientRect.Bottom - 1) mousePos.Y = clientRect.Bottom - 1;

            Rectangle rect = ClientRectangle;
            _saturation = mousePos.X * 100.0 / rect.Width;
            _brightness = (rect.Height - mousePos.Y) * 100.0 / rect.Height;

            Color color = Color.FromArgb(_color.A, ColorUtils.HsbToRgb(_hue, _saturation, _brightness));

            Invalidate();

            if (color != _color)
            {
                _color = color;

                if (ValueChanged != null)
                    ValueChanged(this, EventArgs.Empty);
            }
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            if (_dragInProgress)
                _dragInProgress = false;

            base.OnMouseUp(e);
        }

        protected override bool ProcessDialogKey(Keys keyData)
        {
            Keys key = (keyData & Keys.KeyCode);

//            _saturation = mousePos.X * 100.0 / rect.Width;
  //          _brightness = (rect.Height - mousePos.Y) * 100.0 / rect.Height;

            if (key == Keys.Down)
            {
                Rectangle rect = ClientRectangle;
                int x = (int)Math.Round(_saturation / 100.0 * rect.Width);
                int y = rect.Height - (int)Math.Round(_brightness / 100.0 * rect.Height) + 1;
                UpdateColor(new Point(x, y));

                return true;
            }
            else if (key == Keys.Up)
            {
                Rectangle rect = ClientRectangle;
                int x = (int)Math.Round(_saturation / 100.0 * rect.Width);
                int y = rect.Height - (int)Math.Round(_brightness / 100.0 * rect.Height) - 1;
                UpdateColor(new Point(x, y));

                return true;
            }
            else if (key == Keys.Right)
            {
                Rectangle rect = ClientRectangle;
                int x = (int)Math.Round(_saturation / 100.0 * rect.Width) + 1;
                int y = rect.Height - (int)Math.Round(_brightness / 100.0 * rect.Height);
                UpdateColor(new Point(x, y));

                return true;
            }
            else if (key == Keys.Left)
            {
                Rectangle rect = ClientRectangle;
                int x = (int)Math.Round(_saturation / 100.0 * rect.Width) - 1;
                int y = rect.Height - (int)Math.Round(_brightness / 100.0 * rect.Height);
                UpdateColor(new Point(x, y));

                return true;
            }

            return base.ProcessDialogKey(keyData);
        }
    }
}


