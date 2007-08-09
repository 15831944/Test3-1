// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Globalization;
using System.Security.Permissions;
using System.Drawing.Design;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Summary description for InternalGrid.
	/// </summary>
    [
        ToolboxItem(false)
    ]
#if _XHEOSOURCECODE
	[System.ComponentModel.LicenseProvider(typeof(Xheo.Licensing.ExtendedLicenseProvider))]
#endif
    internal class InternalGrid : Control, IMouseHookClient, IMessageHookClient, IKeyboardHookClient
	{
#if _XHEOSOURCECODE
		System.ComponentModel.License _license = null;
#endif

        public class DisplayNameSortComparer : IComparer
        {
            public int Compare(object left, object right)
            {
                return string.Compare(((Property)left).DisplayName, ((Property)right).DisplayName, true, CultureInfo.CurrentCulture);
            }
        }

        private bool _validationWhileAppLoosingFocus = false;

        internal bool ValidationWhileAppLoosingFocus
        {
            get { return _validationWhileAppLoosingFocus; }
        }

        private int _beginVerticalFreezeCounter = 0;

        private MouseHook _mouseHook;
        private MessageHook _messageHook;
        private KeyboardHook _keyboardHook;

        private Keys _lastLeaveKey = Keys.None;

        private Control _focusedControlAfterKillFocus = null;

        private Control _dropDownTopWindow = null;

        private long _lastTimeClickedFromParent = 0;

        private long _lastTimeClicked = 0;

        private Point _lastPosClickedInParent;

        private Point _lastPosClicked;

        private bool _gridOrCurrentInPlaceCtrlClickedDuringDropDown = false;

        private bool _doubleClickDone = false;

        private bool _columnsLock = false;

        private bool _displayModeChangeInProgress = false;

//        private bool _updateParentChildrenAntiRecursion = false;

        private bool _dontShowInPlaceCtrlInGotFocus = false;

        private const int MinimumLabelColumnWidth = 20;

        private int _propertyVerticalMargin = 4;

        public int PropertyVerticalMargin
        {
            set
            {
                _propertyVerticalMargin = value;
                RecalcPropertyItemHeight();
            }

            get { return _propertyVerticalMargin; }
        }

        private Point _toolTipLocation = Point.Empty;

        private bool _lockRefresh = false;

        private bool _visibleItemCountValid = false;

        private bool _clickWithControl = false;

//        private bool _antiRecursion = false;

        private PropertyGrid _parentCtrl;

        private PropertyTree _categorizedPropertyCollection = new PropertyTree(0);

        private PropertyTree _flatPropertyCollection = new PropertyTree(1);

        private PropertyTree _activePropertyCollection;

        private int _firstDisplayedLine = 1;

        private int _basicPropertyHeight = -1;

        private bool _resizingColumns = false;

		private int _visibleItemCount = 0;

		private int _visibleLineCount = 0;

        private ArrayList _multiSelectedItems = new ArrayList();

        private Color _disabledForeColor = SystemColors.GrayText;

        private Color _readOnlyForeColor = SystemColors.GrayText;

        public int LabelColumnWidth
        {
			get
			{
				Rectangle clientRect = ClientRectangle;
                int labelColumnWidth = (int)Math.Round(_labelColumnWidthRatio * (clientRect.Width - LeftColumnWidth));

                if (labelColumnWidth < MinimumLabelColumnWidth)
                    labelColumnWidth = MinimumLabelColumnWidth;
                else if (LeftColumnWidth + labelColumnWidth + MinimumLabelColumnWidth > clientRect.Width)
                    labelColumnWidth = clientRect.Width - LeftColumnWidth - MinimumLabelColumnWidth;

				return labelColumnWidth;
			}
		}

        public int ValueColumnWidth
        {
            get
            {
                return ClientRectangle.Width - LeftColumnWidth - LabelColumnWidth;
            }
        }

        private double _labelColumnWidthRatio = 0.5;

        public double LabelColumnWidthRatio
        {
            set
            {
                if (value < 0.0)
                    value = 0.0;
                else if (value > 1.0)
                    value = 1.0;

                int oldWidth = LabelColumnWidth;

                _labelColumnWidthRatio = value;

                Rectangle clientRect = ClientRectangle;
                if (oldWidth > LabelColumnWidth)
                {
                    Invalidate(new Rectangle(LeftColumnWidth + 1, 0, clientRect.Width - LeftColumnWidth, clientRect.Height), false);
                    Update();
                }

                if (_currentInPlaceControl != null)
                {
                    Property property = SelectedPropertyEnumerator.Property;
                    Rectangle valueRect = GetItemRect(SelectedPropertyEnumerator);
                    valueRect = property.GetValueRect(valueRect);

                    PropertyFeel feel = property.Feel;
                    feel.MoveControl(valueRect, SelectedPropertyEnumerator);
                }

                if (oldWidth < LabelColumnWidth)
                {
                    Invalidate(new Rectangle(LeftColumnWidth + 1, 0, clientRect.Width - LeftColumnWidth, clientRect.Height), false);
                    Update();
                }

#if _SPGEVAL
                Parent.Refresh();
#endif
            }

            get { return _labelColumnWidthRatio; }
        }

        internal void SetLabelColumnWidthRatio(double value)
        {
            Rectangle clientRect = ClientRectangle;

            int labelColumnWidth = (int)Math.Round(value * (clientRect.Width - LeftColumnWidth));
            if (labelColumnWidth < MinimumLabelColumnWidth)
                LabelColumnWidthRatio = (double)labelColumnWidth / (clientRect.Width - LeftColumnWidth);
            else if (clientRect.Width - LeftColumnWidth - labelColumnWidth < MinimumLabelColumnWidth)
                LabelColumnWidthRatio = (double)(clientRect.Width - LeftColumnWidth - MinimumLabelColumnWidth) /
                    (clientRect.Width - LeftColumnWidth);
            else
                LabelColumnWidthRatio = value;
        }

        private PropertyGrid.DisableModes _disableMode = PropertyGrid.DisableModes.Simple;

        public PropertyGrid.DisableModes DisableMode
        {
            set
            {
                if (_disableMode != value)
                {
                    _disableMode = value;
                    Invalidate();

                    ProcessDisable();
                }
            }

            get { return _disableMode; }
        }

        private bool _disableModeGrayedOut = true;

        public bool DisableModeGrayedOut
        {
            set
            {
                if (_disableModeGrayedOut != value)
                {
                    _disableModeGrayedOut = value;
                    Invalidate();

                    ProcessDisable();
                }
            }

            get { return _disableModeGrayedOut; }
        }

        private bool _enabled = true;

        public new bool Enabled
        {
            set
            {
                _enabled = value;
                Invalidate();

                ProcessDisable();
            }

            get
            {
//                if (_disableMode == PropertyGrid.DisableModes.None)
  //                  return true;
                return _enabled;
            }
        }

        private bool CanSelectProperties
        {
            get
            {
                if (Enabled || (DisableMode == PropertyGrid.DisableModes.None))
                    return true;

                if (_disableMode == PropertyGrid.DisableModes.NoPropertySelection)
                    return false;

                return true;
            }
        }

        private ImageList _imageList = null;

        public ImageList ImageList
        {
            set { _imageList = value; }
            get { return _imageList; }
        }

        private ToolTipCtrl _tooltipCtrl;

        private PropertyGrid.ToolTipModes _toolTipMode = PropertyGrid.ToolTipModes.None;

        public PropertyGrid.ToolTipModes ToolTipMode
        {
            set
            {
                _toolTipMode = value;

                if (_toolTipMode == PropertyGrid.ToolTipModes.None)
	            {
                    if (_tooltipCtrl != null)
                    {
                        _tooltipCtrl.Dispose();
                        _tooltipCtrl = null;
                    }
	            }
            }

            get { return _toolTipMode; }
        }

        private PropertyGrid.ReadOnlyVisuals _readOnlyVisual = PropertyGrid.ReadOnlyVisuals.Disabled;

        public PropertyGrid.ReadOnlyVisuals ReadOnlyVisual
        {
            get { return _readOnlyVisual; }

            set
            {
                _readOnlyVisual = value;

                PropertyEnumerator propEnum = FirstProperty;
                while (propEnum != RightBound)
                {
                    PropertyValue propertyValue = propEnum.Property.Value;
                    if ((propertyValue != null) && propertyValue.ReadOnly)
                        EnableProperty(propEnum, _readOnlyVisual == PropertyGrid.ReadOnlyVisuals.ReadOnlyFeel);

                    propEnum.MoveNext();
                }

                Refresh();
            }
        }
        
        private CustomDrawManager _drawManager;

        public CustomDrawManager DrawManager
		{
			get { return _drawManager; }
            set
            {
                _drawManager = value;
                _drawManager.Grid = _parentCtrl;

                if (IsHandleCreated)
                {
                    Win32Calls.SetWindowPos(Handle, IntPtr.Zero, 0, 0, 0, 0, Win32Calls.SWP_FRAMECHANGED |
                        Win32Calls.SWP_NOZORDER | Win32Calls.SWP_NOACTIVATE | Win32Calls.SWP_NOMOVE |
                            Win32Calls.SWP_NOSIZE);
                }
            }
        }

        private PropertyEnumerator CloneHierarchyUnder(PropertyEnumerator oldParentEnum, PropertyEnumerator newParentEnum, Property selectedProperty, ref PropertyEnumerator newSelectedEnum)
        {
            PropertyEnumerator siblingEnum = oldParentEnum.Children.GetSiblingEnumerator();
            PropertyEnumerator lastEnum = oldParentEnum;
            while (siblingEnum != siblingEnum.RightBound)
            {
                PropertyEnumerator newEnum = AddPropertyUnder(newParentEnum, siblingEnum.Property);

                if (siblingEnum.Property == selectedProperty)
                    newSelectedEnum = newEnum;

                lastEnum = CloneHierarchyUnder(siblingEnum, newEnum, selectedProperty, ref newSelectedEnum).GetDeepEnumerator();

                siblingEnum.MoveNext();
            }

            return lastEnum;
        }

        private PropertyGrid.DisplayModes _displayMode = PropertyGrid.DisplayModes.Categorized;

        public PropertyGrid.DisplayModes DisplayMode
        {
            set
            {
	            if (_displayMode == value)
		            return;

                HideInPlaceControl();

                _displayMode = value;

	            if (_displayMode == PropertyGrid.DisplayModes.Categorized)
	            {
		            InvalidateVisibleItemCount();

                    _activePropertyCollection = _categorizedPropertyCollection;

                    // Init the selected property
                    PropertyEnumerator propEnum = _activePropertyCollection.GetDeepEnumerator().MoveFirst();
                    while (propEnum != propEnum.RightBound)
                    {
                        Property property = propEnum.Property;
                        if (property != null)
                        {
                            if (property.Value != null)
                            {
                                // TODO : could be optimized because OwnerEnumerator does more than just
                                // assigning the new propEnum
                                property.Value.OwnerEnumerator = propEnum;
                            }

                            if (property.Selected)
                            {
                                _selectedPropertyEnum = propEnum.GetVisibleDeepEnumerator();
                                break;
                            }
                        }

                        propEnum.MoveNext();
                    }

		            // Reset the first displayed item
                    _firstDisplayedPropertyEnumerator = _activePropertyCollection.GetVisibleDeepEnumerator();
                    _firstDisplayedPropertyEnumerator.MoveFirst();

		            // Cleanup the flat list
		            _flatPropertyCollection.Clear();

//		            HideInPlaceControl();

		            CheckScrollbar();

		            EnsureVisible(_selectedPropertyEnum);

                    Invalidate();

                    _parentCtrl.OnDisplayModeChanged(new EventArgs());
	            }
	            else if ((_displayMode == PropertyGrid.DisplayModes.Flat) || (_displayMode == PropertyGrid.DisplayModes.FlatSorted))
	            {
                    _displayModeChangeInProgress = true;

                    _parentCtrl.BeginUpdate();
                    
                    // Store the currently selected property
                    Property selectedProperty = null;
                    if (_selectedPropertyEnum.Property != null)
		            {
			            selectedProperty = _selectedPropertyEnum.Property;
                        // Unselect the selected item in the current collection
                        SelectProperty(_activePropertyCollection.GetVisibleDeepEnumerator().RightBound);
                    }

		            // Cleanup the flat list
		            _flatPropertyCollection.Clear();

		            _activePropertyCollection = _flatPropertyCollection;
                    _firstDisplayedPropertyEnumerator = _activePropertyCollection.GetVisibleDeepEnumerator().RightBound.GetVisibleDeepEnumerator();
		            _selectedPropertyEnum = _firstDisplayedPropertyEnumerator;
                    
                    // Collect all properties except those child of grouped properties
                    PropertyEnumerator propEnum = _categorizedPropertyCollection.GetDeepEnumerator().MoveFirst();
                    PropertyEnumerator selectedEnum = null;
                    while (propEnum != propEnum.RightBound)
                    {
                        Property property = propEnum.Property;

                        if ((property.Value != null) || (property is PropertyHyperLink))    // Something that is not a (sub)category
                        {
                            if (propEnum.Parent.Property.Value == null)   // and that is under a (sub)category
                            {
                                PropertyEnumerator newEnum = AddPropertyBefore(_flatPropertyCollection.GetDeepEnumerator().RightBound, property);
                                propEnum = CloneHierarchyUnder(propEnum, newEnum, selectedProperty, ref selectedEnum);

                                if (property == selectedProperty)
                                    selectedEnum = newEnum;
                            }
                        }
                        propEnum.MoveNext();
                    }
                    
		            // Sort the properties
                    if (_displayMode == PropertyGrid.DisplayModes.FlatSorted)
                        _flatPropertyCollection.Sort(new DisplayNameSortComparer());
                    
                    _firstDisplayedPropertyEnumerator = _flatPropertyCollection.GetVisibleDeepEnumerator();
                    _firstDisplayedLine = 1;
                    _firstDisplayedPropertyEnumerator.MoveFirst();

                    if (selectedEnum != null)
                        SelectProperty(selectedEnum);
		            else
                        SelectProperty(_firstDisplayedPropertyEnumerator);

                    _displayModeChangeInProgress = false;

                    ResetScrollPosToFirstDisplayedProperty();

                    EnsureVisible(_selectedPropertyEnum);

                    _parentCtrl.OnDisplayModeChanged(new EventArgs());

                    _parentCtrl.EndUpdate();
					Refresh();
					CheckScrollbar();
				}

                ProcessDisable();
            }

            get { return _displayMode; }
        }

        private PropertyGrid.MultiSelectModes _multiSelectMode = PropertyGrid.MultiSelectModes.None;

        public PropertyGrid.MultiSelectModes MultiSelectMode
        {
            set { _multiSelectMode = value; }
            get { return _multiSelectMode; }
        }

        private PropertyVisibleDeepEnumerator _selectedPropertyEnum;

        public PropertyVisibleDeepEnumerator SelectedPropertyEnumerator
		{
			get { return _selectedPropertyEnum; }
		}

        private Control _currentInPlaceControl;

        public Control InPlaceControl
        {
            get { return _currentInPlaceControl; }
        }

        public InternalGrid(PropertyGrid parentCtrl)
        {
#if _XHEOSOURCECODE
            _license = System.ComponentModel.LicenseManager.Validate(typeof(InternalGrid), this);
#endif
            _parentCtrl = parentCtrl;

            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque, true);

            UpdateStyles();

            LeftColumnWidth = 16;

            GridColor = SystemColors.ActiveBorder;

            _activePropertyCollection = _categorizedPropertyCollection;

            _firstDisplayedPropertyEnumerator = _activePropertyCollection.GetVisibleDeepEnumerator().RightBound.GetVisibleDeepEnumerator();
            _selectedPropertyEnum = _activePropertyCollection.GetVisibleDeepEnumerator().RightBound.GetVisibleDeepEnumerator();

            TabStop = true;

            BackColor = SystemColors.Window;

            _mouseHook = new MouseHook(this, this);
            _messageHook = new MessageHook(this, this);
            _keyboardHook = new KeyboardHook(this, this);
        }

        protected override CreateParams CreateParams
        {
            [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
            get
            {
                CreateParams cp = base.CreateParams;
                cp.Style |= Win32Calls.WS_VSCROLL;

                return cp;
            }
        }

        private PropertyVisibleDeepEnumerator _firstDisplayedPropertyEnumerator;

        public PropertyVisibleDeepEnumerator FirstDisplayedProperty
        {
            set
            {
                _firstDisplayedPropertyEnumerator = new PropertyVisibleDeepEnumerator(value.Node);
                ResetScrollPosToFirstDisplayedProperty();
            }

            get { return (PropertyVisibleDeepEnumerator)_firstDisplayedPropertyEnumerator.Clone(); }
        }

		protected override void OnPaint(PaintEventArgs e)
        {
            Rectangle clientRect = ClientRectangle;

            DrawManager.DrawGridBackground(e.Graphics, clientRect);

	        // Draw the vertical separation between the first column and the properties labels
	        Rectangle leftRect = clientRect;
	        leftRect.Width = LeftColumnWidth + 1;

            // The first property to be displayed is maybe a multiline property partially displayed.
            // In this case the starting position (Y) must be shifted upward.
            Rectangle propertyRect = ClientRectangle;
            propertyRect.Y -= (_firstDisplayedLine - 1) * _basicPropertyHeight;

            PropertyVisibleDeepEnumerator vdEnumerator = FirstDisplayedProperty;
            int position = 0;
            while (vdEnumerator != vdEnumerator.RightBound)
            {
                Property property = vdEnumerator.Property;
                int itemHeight = _basicPropertyHeight * property.HeightMultiplier;
                propertyRect.Height = itemHeight;

                if (propertyRect.Top > e.ClipRectangle.Bottom)
                    break;

                property.DisplayedAtPosition = position++;

                leftRect.Y = propertyRect.Y;
                leftRect.Height = propertyRect.Height;
                DrawManager.DrawLeftColumnBackground(e.Graphics, leftRect, vdEnumerator);

                if (Rectangle.Intersect(propertyRect, e.ClipRectangle).IsEmpty == false)
                    property.Draw(e.Graphics, propertyRect, vdEnumerator);

                propertyRect.Offset(0, itemHeight);

                if (propertyRect.Top > clientRect.Bottom)
                    break;

                vdEnumerator.MoveNext();
            }

#if _SPGEVAL
            Pen pen = new Pen(Color.LightGray, 2.0f);
	        Rectangle evalRect = clientRect;
            evalRect.Width = LeftColumnWidth + LabelColumnWidth;
            e.Graphics.DrawLine(pen, evalRect.Left, evalRect.Top, evalRect.Right, evalRect.Bottom);
            e.Graphics.DrawLine(pen, evalRect.Right, evalRect.Top, evalRect.Left, evalRect.Bottom);
            pen.Dispose();
#endif

            base.OnPaint(e);
        }

        private int _leftColumnWidth;

        public int LeftColumnWidth
        {
            set { _leftColumnWidth = value; Invalidate(); }
            get { return _leftColumnWidth; }
        }

        // TODO : will be used when implementing RTL
        private Rectangle _rootGlyphsRect = Rectangle.Empty;
        public Rectangle RootGlyphsRect
        {
            get { return _rootGlyphsRect; }
        }

        private Color _refColor;

        public Color GridColor
        {
            get { return _refColor; }
            set
            {
                _refColor = value;
/*	            if (force)
		            m_refColor = color;
	            else
	            {
		            double H,S,L;
		            RGBtoHSL(color,&H,&S,&L);

		            m_refColor = HSLtoRGB(H,S,192./255.);
	            }*/

                Refresh();
            }
        }

		private Color _selectedBkgColor = SystemColors.Highlight;

		public Color SelectedBackColor
		{
			set
            {
                _selectedBkgColor = value;
                Refresh();
            }
			get { return _selectedBkgColor; }
		}

        private Color _selectedNotFocusedBkgColor = Color.Empty;
        
        public Color SelectedNotFocusedBackColor
		{
            set
            {
                _selectedNotFocusedBkgColor = value;
                Refresh();
            }
            get
            {
                if (_selectedNotFocusedBkgColor == Color.Empty)
                    return GridColor;

                return _selectedNotFocusedBkgColor;
            }
		}

        private Color _highlightedTextColor = SystemColors.HighlightText;

        public Color HighlightedTextColor
        {
            set
            {
                _highlightedTextColor = value;
                Refresh();
            }
            get { return _highlightedTextColor; }
        }

        private void RecalcPropertyItemHeight()
        {
            Graphics graphics = CreateGraphics();

            Size size = Win32Calls.GetTextExtent(graphics, "A0j", Font);
            _basicPropertyHeight = size.Height + _propertyVerticalMargin;				// Make it a little bigger

            // Recalculate the height of all the inplace controls
            foreach (PropertyFeel feel in _parentCtrl._registeredFeels.Values)
            {
                if (feel != null)
                    feel.UpdateFont();
            }

            graphics.Dispose();
            Invalidate();
        }

        public int BasicPropertyHeight
        {
            get { return _basicPropertyHeight; }
        }

        public override Color BackColor
        {
            get { return base.BackColor; }
            set
            {
                base.BackColor = value;
                if ((_currentInPlaceControl != null) && _currentInPlaceControl.Visible)
                    _currentInPlaceControl.BackColor = value;
                Refresh();
            }
        }

        public override Color ForeColor
        {
            get { return base.ForeColor; }
            set
            {
                base.ForeColor = value;
                if ((_currentInPlaceControl != null) && _currentInPlaceControl.Visible)
                    _currentInPlaceControl.ForeColor = value;
                Refresh();
            }
        }

        public Color DisabledForeColor
        {
            get { return _disabledForeColor; }
            set
            {
                _disabledForeColor = value;
                Refresh();
            }
        }

        public Color ReadOnlyForeColor
        {
            get { return _readOnlyForeColor; }
            set
            {
                _readOnlyForeColor = value;
                Refresh();
            }
        }

        public bool ColumnsLock
        {
            set { _columnsLock = value; }
            get { return _columnsLock; }
        }

        public override Font Font
        {
            get { return base.Font; }
            set
            {
                base.Font = value;

                // A visible inplace control must be hidden because if the row height changes
                // the inplace control will be resized
                bool showInPlaceCtrlAfterSet = false;
                if ((_currentInPlaceControl != null) && _currentInPlaceControl.Visible)
                {
                    showInPlaceCtrlAfterSet = true;
                    HideInPlaceControl();
                }

                // Assign the font to the tooltip
                if (ToolTip != null)
                    ToolTip.Font = Font;

                // Calculate item height
                RecalcPropertyItemHeight();

		        // Recalculate the margin of editboxes
                using (TextBox tb = new TextBox())
                {
                    tb.Font = Font;
                    Win32Calls.SendMessage(tb.Handle, Win32Calls.EM_SETMARGINS, 0x0003, Win32Calls.MakeLong(0xffff, 0xffff));
                    _parentCtrl.SetGlobalTextMargin(2 + Win32Calls.LoWord(Win32Calls.SendMessage(tb.Handle, Win32Calls.EM_GETMARGINS, 0, 0)));

                    if (_parentCtrl.GlobalTextMargin < 5)
                        _parentCtrl.SetGlobalTextMargin(5);
                }

                // If an inplace control has been hidden, show it again
		        if (showInPlaceCtrlAfterSet)
			        ShowInPlaceControl(SelectedPropertyEnumerator);

		        CheckScrollbar();
            }
        }

        private bool IsScrollbarVisible()
        {
            return ((Win32Calls.GetWindowLong(Handle, Win32Calls.GWL_STYLE) & Win32Calls.WS_VSCROLL) != 0);
        }

        private bool _scrollingInProgress = false;

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_MOUSEACTIVATE)
            {
                if (Focused == false)
                {
                    Rectangle itemRect;
                    Point pt = PointToClient(MousePosition);
                    if (ClientRectangle.Contains(pt))
                    {
                        PropertyEnumerator propEnum = PropertyItemFromPoint(pt, out itemRect);
                        if (SelectedPropertyEnumerator != propEnum)
                        {
                            _dontShowInPlaceCtrlInGotFocus = true;

                            // When we are in the dropdown part of a UITypeEditor, we want to forward the click
                            // to the control (internalgrid or another inplace control). If Focus() is not called,
                            // it behaves like the MSPG
                            if (Form.ActiveForm is PropInPlaceUITypeEditor.DropDownForm)
                                Focus();
                        }
                    }
                }
            }
            else if (m.Msg == Win32Calls.WM_LBUTTONDOWN)
            {
                // If we don't do all this stuff here, a shift + double-click is not correctly detected
                // (this is used when double-clicking a property label to cycle values)
                _doubleClickDone = false;

                long now = DateTime.Now.Ticks;
                int x = Win32Calls.LoWord((int)m.LParam);
                int y = Win32Calls.HiWord((int)m.LParam);

                if ((_lastTimeClicked != 0) && ((int)((now - _lastTimeClicked) / 0x2710) < SystemInformation.DoubleClickTime))
                {
                    if ((Math.Abs(x - _lastPosClicked.X) < SystemInformation.DoubleClickSize.Width) &&
                        (Math.Abs(y - _lastPosClicked.Y) < SystemInformation.DoubleClickSize.Height))
                        _doubleClickDone = true;

                    _lastTimeClicked = 0;
                    _lastPosClicked = Point.Empty;
                }

                if (_doubleClickDone == false)
                {
                    _lastTimeClicked = now;
                    _lastPosClicked = new Point(x, y);
                }
            }
            else if (m.Msg == Win32Calls.WM_LBUTTONDBLCLK)
            {
                _lastTimeClicked = 0;
            }
            else if (m.Msg == Win32Calls.WM_NCCALCSIZE)
            {
                //                if (m.WParam != IntPtr.Zero)
                {
                    Win32Calls.NCCALCSIZE_PARAMS csp;

                    csp = (Win32Calls.NCCALCSIZE_PARAMS)Marshal.PtrToStructure(m.LParam, typeof(Win32Calls.NCCALCSIZE_PARAMS));
                    Rectangle rect = new Rectangle(csp.rgrc0.Left, csp.rgrc0.Top,
                        csp.rgrc0.Right - csp.rgrc0.Left, csp.rgrc0.Bottom - csp.rgrc0.Top);
                    DrawManager.NcCalcSize(ref rect);
                    csp.rgrc0.Left = rect.X;
                    csp.rgrc0.Right = rect.X + rect.Width;
                    csp.rgrc0.Top = rect.Y;
                    csp.rgrc0.Bottom = rect.Y + rect.Height;
                    Marshal.StructureToPtr(csp, m.LParam, false);
                }
            }
            else if (m.Msg == Win32Calls.WM_NOTIFY)
            {
                if (m.LParam != IntPtr.Zero)
                {
                    Win32Calls.NMHDR nmhdr = (Win32Calls.NMHDR)m.GetLParam(typeof(Win32Calls.NMHDR));
                    if (nmhdr.code == Win32Calls.TTN_SHOW)
                    {
                        if ((ToolTip != null) && (nmhdr.hwndFrom == ToolTip.Handle))
                        {
                            ToolTip.Location = _toolTipLocation;
                            m.Result = (IntPtr)1;
                            return;
                        }
                    }
                }
            }
            else if (m.Msg == Win32Calls.WM_NCPAINT)
            {
                IntPtr hDC = Win32Calls.GetWindowDC(m.HWnd);
                Graphics graphics = Graphics.FromHdc(hDC);

                Win32Calls.RECT rc = new Win32Calls.RECT();
                Win32Calls.GetWindowRect(m.HWnd, ref rc);
                Rectangle rect = new Rectangle(0, 0, rc.Right - rc.Left, rc.Bottom - rc.Top);
                DrawManager.DrawGridNonClientArea(graphics, rect);

                Win32Calls.ReleaseDC(m.HWnd, hDC);
            }
            else if (m.Msg == Win32Calls.WM_VSCROLL)
            {
                int nSBCode = Win32Calls.LoWord((int)m.WParam);

                if (nSBCode == Win32Calls.SB_ENDSCROLL)
                {
                    _scrollingInProgress = false;

                    if ((_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.TabKey) &&
                        ((_parentCtrl.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyWithAutoFocus) != 0))
                    {
                        ShowInPlaceControl(SelectedPropertyEnumerator);
                    }

                    return;
                }

                _scrollingInProgress = true;

                Rectangle clientRect = ClientRectangle;
                int itemsPerPage = clientRect.Height / _basicPropertyHeight;

                // Get some info about the old state of the scroll
                Win32Calls.SCROLLINFO si = new Win32Calls.SCROLLINFO();
                si.cbSize = Marshal.SizeOf(si);
                si.fMask = Win32Calls.SIF_PAGE | Win32Calls.SIF_RANGE;
                Win32Calls.GetScrollInfo(Handle, Win32Calls.SB_VERT, ref si);

                switch (nSBCode)
                {
                    case Win32Calls.SB_LINEDOWN:
                        {
                            if ((si.nPage > 0) && (Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) < si.nMax - (si.nPage - 1)))
                            {
                                if (_firstDisplayedLine == FirstDisplayedProperty.Property.HeightMultiplier)
                                {
                                    _firstDisplayedPropertyEnumerator.MoveNext();
                                    _firstDisplayedLine = 1;
                                }
                                else
                                    _firstDisplayedLine++;

                                Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) + 1, true);
                                Invalidate();
                            }
                            break;
                        }
                    case Win32Calls.SB_LINEUP:
                        {
                            if (Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) > 0)
                            {
                                if (_firstDisplayedLine > 1)
                                    _firstDisplayedLine--;
                                else
                                {
                                    _firstDisplayedPropertyEnumerator.MovePrev();
                                    _firstDisplayedLine = FirstDisplayedProperty.Property.HeightMultiplier;
                                }

                                Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) - 1, true);
                                Invalidate();
                            }
                            break;
                        }
                    case Win32Calls.SB_PAGEDOWN:
                        {
                            int counter = 0;
                            PropertyVisibleDeepEnumerator enumerator = FirstDisplayedProperty.GetVisibleDeepEnumerator();
                            while (enumerator != enumerator.RightBound)
                            {
                                Rectangle itemRect = GetAbsoluteItemRect(enumerator);
                                if (itemRect.Bottom > clientRect.Bottom)
                                {
                                    // We have found the last displayed property pointed by iter
                                    PropertyVisibleDeepEnumerator currentLastDisplayedPropertyEnum = enumerator.GetVisibleDeepEnumerator();
                                    int lastItemDisplayLine = (clientRect.Bottom - itemRect.Top) / _basicPropertyHeight + 1;

                                    // We loop to make it scroll up without creating blank space at the bottom
                                    do
                                    {
                                        // Scroll one line at the top
                                        if (_firstDisplayedLine == FirstDisplayedProperty.Property.HeightMultiplier)
                                        {
                                            _firstDisplayedPropertyEnumerator.MoveNext();
                                            _firstDisplayedLine = 1;
                                        }
                                        else
                                            _firstDisplayedLine++;

                                        // Determine the new bottom item
                                        if (lastItemDisplayLine == currentLastDisplayedPropertyEnum.Property.HeightMultiplier)
                                        {
                                            currentLastDisplayedPropertyEnum.MoveNext();
                                            lastItemDisplayLine = 1;
                                        }
                                        else
                                            lastItemDisplayLine++;

                                        counter++;
                                    }
                                    while ((counter < itemsPerPage - 1) && (currentLastDisplayedPropertyEnum != currentLastDisplayedPropertyEnum.RightBound));

                                    break;
                                }

                                enumerator.MoveNext();
                            }

                            Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) + counter, true);
                            Invalidate();

                            break;
                        }
                    case Win32Calls.SB_PAGEUP:
                        {
                            int newPos = Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) - itemsPerPage + 1;

                            // Provide a limit for the maximum position
                            if (newPos < 0)
                                newPos = 0;

                            // Scroll the first displayed element
                            for (int i = 0; i < Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) - newPos; i++)
                            {
                                if (_firstDisplayedLine > 1)
                                    _firstDisplayedLine--;
                                else
                                {
                                    _firstDisplayedPropertyEnumerator.MovePrev();
                                    _firstDisplayedLine = FirstDisplayedProperty.Property.HeightMultiplier;
                                }
                            }

                            Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, newPos, true);
                            Invalidate();
                            break;
                        }
                    case Win32Calls.SB_THUMBTRACK:
                    case Win32Calls.SB_THUMBPOSITION:
                        {
                            if (FirstDisplayedProperty != RightBound.GetVisibleDeepEnumerator())
                            {
                                int nPos = Win32Calls.HiWord((int)m.WParam);
                                int currentPos = Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT);

                                // Scroll the first displayed element
                                int i = 0;
                                for (; i < Math.Abs((nPos - currentPos)); i++)
                                {
                                    if (nPos > currentPos)
                                    {
                                        if (FirstDisplayedProperty.Property.HeightMultiplier == _firstDisplayedLine)
                                        {
                                            _firstDisplayedPropertyEnumerator.MoveNext();
                                            _firstDisplayedLine = 1;
                                        }
                                        else
                                            _firstDisplayedLine++;

                                        if (_firstDisplayedPropertyEnumerator == RightBound.GetVisibleDeepEnumerator())
                                            break;
                                    }
                                    else
                                    {
                                        if (_firstDisplayedLine > 1)
                                            _firstDisplayedLine--;
                                        else
                                        {
                                            _firstDisplayedPropertyEnumerator.MovePrev();
                                            _firstDisplayedLine = FirstDisplayedProperty.Property.HeightMultiplier;
                                        }
                                    }
                                }
                                
                                if (nPos < currentPos)
                                    Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, currentPos - i, true);
                                else
                                    Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, currentPos + i, true);
                                Invalidate();
                            }

                            break;
                        }
                }

                if (Focused == false)
                    Focus();

                CheckScrollbar();
            }

            base.WndProc(ref m);
        }

        internal PropertyFeel FindFeel(PropertyEnumerator parentEnum, PropertyEnumerator propEnum)
        {
            PropertyFeel feel = null;

            if (propEnum.Property.Name == "Nodes")
                feel = null;

            // Try to find if the user specified a PropertyFeelAttribute for the property
            ArrayList feelAttrs = propEnum.Property.Value.GetAttributes(typeof(PropertyFeelAttribute));
            foreach (PropertyFeelAttribute attr in feelAttrs)
            {
                if (attr.ChildPropertyName.Length == 0)
                {
                    if (attr.PropertyFeelName == null)
                        return null;

                    feel = _parentCtrl.GetRegisteredFeel(attr.PropertyFeelName);

                    if (feel == PropertyFeel.Empty)
                        return null;
                    else if (feel != null)
                        break;
                }
            }

            if (feel == null)
            {
                // Is there a PropertyFeelAttribute set on the parent and defined for this property ?
                if (parentEnum.Property.Value != null)
                {
                    feelAttrs = parentEnum.Property.Value.GetAttributes(typeof(PropertyFeelAttribute));
                    foreach (PropertyFeelAttribute attr in feelAttrs)
                    {
                        if (attr.ChildPropertyName == propEnum.Property.Value.DisplayName)
                        {
                            if (attr.PropertyFeelName == null)
                                return null;

                            feel = _parentCtrl.GetRegisteredFeel(attr.PropertyFeelName);
                            break;
                        }
                    }
                }
            }

#if _DOTNET2
            if (feel == null)
            {
                PasswordPropertyTextAttribute attr = (PasswordPropertyTextAttribute)propEnum.Property.Value.GetAttribute(typeof(PasswordPropertyTextAttribute));
                if ((attr != null) && attr.Password)
                    feel = _parentCtrl.GetRegisteredFeel(PropertyGrid.FeelEditPassword);
            }
#endif
            if (feel == null)
                // Is there a default PropertyFeel that has been registered for the Property type ?
                feel = _parentCtrl.GetRegisteredFeelAttachment(propEnum.Property.Value.UnderlyingType);

            if (feel == null)
            {
                // Try to find a UITypeEditor attached to the PropertyDescriptor
                UITypeEditor editor = (UITypeEditor)propEnum.Property.Value.PropertyDescriptor.GetEditor(typeof(UITypeEditor));
#if _DOTNET2
                if (editor == null)
                {
                    NullableConverter nc = propEnum.Property.Value.TypeConverter as NullableConverter;
                    if (nc != null)
                        editor = (UITypeEditor)TypeDescriptor.GetEditor(nc.UnderlyingType, typeof(UITypeEditor));
                }
#endif
                if (editor == null)
                {
                    // Try to find a UITypeEditor attached to the property with an attribute
                    EditorAttribute eAttr = (EditorAttribute)propEnum.Property.Value.GetAttribute(typeof(EditorAttribute));
                    if (eAttr == null)
                    {
                        foreach(object obj in propEnum.Property.Value.UnderlyingType.GetCustomAttributes(typeof(EditorAttribute), true))
                        {
                            if (obj is EditorAttribute)
                            {
                                eAttr = (EditorAttribute)obj;
                                break;
                            }
                        }
                    }

                    if (eAttr != null)
                    {
                        Type[] typeArray = new Type[] { typeof(Type) };
                        Type editorType = Utilities.GetTypeFromName(eAttr.EditorTypeName, propEnum.Property.Value.UnderlyingType);
                        if (editorType.IsSubclassOf(typeof(UITypeEditor)))
                        {
#if _DOTNET2
                            if (editorType.GetConstructor(typeArray) != null)
                                editor = (UITypeEditor)TypeDescriptor.CreateInstance(null, editorType, typeArray, new object[] { propEnum.Property.Value.UnderlyingType });
                            else
                                editor = (UITypeEditor)TypeDescriptor.CreateInstance(null, editorType, null, null);
#else
                            if (editorType.GetConstructor(typeArray) != null)
                                editor = (UITypeEditor)Activator.CreateInstance(editorType, new object[] { propEnum.Property.Value.UnderlyingType });
                            else
                                editor = (UITypeEditor)Activator.CreateInstance(editorType);
#endif
                        }
                    }
                }

                // Try to find a UITypeEditor attached to the Type
                if (editor == null)
                    editor = (UITypeEditor)TypeDescriptor.GetEditor(propEnum.Property.Value.UnderlyingType, typeof(UITypeEditor));

                if ((editor != null) && !editor.GetType().Equals(typeof(UITypeEditor)))
                {
                    PropertyTypeDescriptorContext context = propEnum.Property.Value.GetTypeDescriptorContext(propEnum);
                    if ((editor.GetEditStyle() == UITypeEditorEditStyle.None) &&
                        propEnum.Property.Value.TypeConverter.GetStandardValuesSupported(context))
                        feel = _parentCtrl.GetRegisteredFeel(PropertyGrid.FeelEditList);
                    else
                        feel = new PropertyUITypeEditorFeel(editor, _parentCtrl);
                }
            }

            // If the underlying type supports a list of allowed values, let's associate a list feel with it
            if (feel == null)
            {
                PropertyTypeDescriptorContext context = propEnum.Property.Value.GetTypeDescriptorContext(propEnum);
                if (propEnum.Property.Value.TypeConverter.GetStandardValuesSupported(context))
                    feel = _parentCtrl.GetRegisteredFeel(PropertyGrid.FeelEditList);
            }

            // If it was not found, assign the global default feel
            if (feel == null)
                feel = _parentCtrl.DefaultFeel;

            return feel;
        }

        PropertyLook FindLook(PropertyEnumerator parentEnum, PropertyEnumerator propEnum)
        {
            PropertyLook look = null;

            ArrayList plAttrs = propEnum.Property.Value.GetAttributes(typeof(PropertyLookAttribute));
            foreach (PropertyLookAttribute attr in plAttrs)
            {
                if (attr.ChildPropertyName.Length == 0)
                {
                    if (attr.LookType == null)
                        return null;

                    return (PropertyLook)Activator.CreateInstance(attr.LookType, attr.Parameters);
                }
            }

            if (look == null)
            {
                // Is there a look attribute set on the parent ?
                if (parentEnum.Property != null) // (In sorted mode you may have no parent...)
                {
                    if (parentEnum.Property.Value != null)
                    {
                        plAttrs = parentEnum.Property.Value.GetAttributes(typeof(PropertyLookAttribute));
                        foreach (PropertyLookAttribute attr in plAttrs)
                        {
                            if (attr.ChildPropertyName.CompareTo(propEnum.Property.Value.DisplayName) == 0)
                                return (PropertyLook)Activator.CreateInstance(attr.LookType);
                        }
                    }
                }
            }
#if _DOTNET2
            if (look == null)
            {
                PasswordPropertyTextAttribute attr = (PasswordPropertyTextAttribute)propEnum.Property.Value.GetAttribute(typeof(PasswordPropertyTextAttribute));
                if ((attr != null) && attr.Password)
                    look = new PropertyPasswordLook();
            }
#endif
            if (look == null)
                look = _parentCtrl.GetRegisteredLookAttachment(propEnum.Property.Value.UnderlyingType);

            return look;
        }

        public PropertyEnumerator AppendRootCategory(int id, string catName)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            RootCategory category = new RootCategory(id, catName);
            category.ParentGridInternal = _parentCtrl;

            PropertyEnumerator propEnum = _activePropertyCollection.Append(_activePropertyCollection.GetDeepEnumerator(), category);

            if (FirstDisplayedProperty == _activePropertyCollection.GetVisibleDeepEnumerator().RightBound)
                FirstDisplayedProperty = propEnum.GetVisibleDeepEnumerator();

            InvalidateVisibleItemCount();

	        if (Handle != IntPtr.Zero)    // necessary test ?
                Invalidate();

            if (FirstProperty.Count == 1)
                SelectProperty(FirstProperty);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        public PropertyEnumerator InsertRootCategory(PropertyEnumerator beforeCategory, int id, string catName)
        {
            if (beforeCategory == _activePropertyCollection.GetDeepEnumerator().RightBound)
                return AppendRootCategory(id, catName);

            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            System.Diagnostics.Debug.Assert(beforeCategory.HasParent == false, "Root categories can only be inserted at the root.");

            RootCategory category = new RootCategory(id, catName);
            category.ParentGridInternal = _parentCtrl;

            PropertyEnumerator propEnum = _activePropertyCollection.InsertBefore(beforeCategory, category);

            // When there is no category yet or (when we are in SelectedObject and the new category should be the
            // first one) then set the new category as the first displayed one
            PropertyEnumerator catEnum = ((PropertyEnumerator)propEnum.Clone()).MovePrev();
            if ((FirstDisplayedProperty == _activePropertyCollection.GetVisibleDeepEnumerator().RightBound) ||
                (_parentCtrl.IsInsideSelectedObjects && (catEnum == propEnum.LeftBound)))
                FirstDisplayedProperty = propEnum.GetVisibleDeepEnumerator();

            InvalidateVisibleItemCount();

            if (Handle != IntPtr.Zero)    // necessary test ?
                Invalidate();

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        internal bool AddChildProperties(PropertyEnumerator underParent)
        {
            PropertyValue parentValue = underParent.Property.Value;
            if (parentValue == null)
                return false;

            if (parentValue.TypeConverter.GetPropertiesSupported(parentValue.GetTypeDescriptorContext(underParent)) == false)
            {
                ShowChildPropertiesAttribute showChildrenAttr = (ShowChildPropertiesAttribute)underParent.Property.Value.GetAttribute(typeof(ShowChildPropertiesAttribute));
                if ((showChildrenAttr == null) || !showChildrenAttr.ShowChildProperties)
                    return false;
            }
            else
            {
                ShowChildPropertiesAttribute showChildrenAttr = (ShowChildPropertiesAttribute)underParent.Property.Value.GetAttribute(typeof(ShowChildPropertiesAttribute));
                if ((showChildrenAttr != null) && !showChildrenAttr.ShowChildProperties)
                    return false;
            }

            // Generate the target instances of the current property
            object[] parentTargets = underParent.Property.Value.TargetInstances;
            object[] targets;
            PropertyDescriptor[] propertyDescriptors;
            if (parentTargets == null)
            {
                targets = new object[1] { underParent.Property.Value.UnderlyingValue };
                propertyDescriptors = new PropertyDescriptor[1] { underParent.Property.Value.PropertyDescriptor };
            }
            else
            {
                targets = new object[parentTargets.Length];
                propertyDescriptors = new PropertyDescriptor[parentTargets.Length];

                for (int i = 0; i < parentTargets.Length; i++)
                {
                    object target = parentTargets[i];
                    try
                    {
                        targets[i] = underParent.Property.Value.PropertyDescriptor.GetValue(target);
                        propertyDescriptors[i] = underParent.Property.Value.PropertyDescriptor;
                    }
                    catch (Exception)
                    {
                    }
                }
            }

            // From these targets, get the common child values
            PropertyValue[] childValues = parentValue.GetChildValues(targets, propertyDescriptors);

            if ((childValues != null) && (childValues.Length > 0))
            {
                underParent.Property.Value.NoLinkWithChildren = false;

                ArrayList hideAttrs = underParent.Property.Value.GetAttributes(typeof(PropertyHideAttribute));
                ArrayList disableAttrs = underParent.Property.Value.GetAttributes(typeof(PropertyDisableAttribute));
                foreach (PropertyValue propValue in childValues)
                {
                    bool isHidden = false;
                    foreach (PropertyHideAttribute hideAttr in hideAttrs)
                    {
                        if (hideAttr.ChildPropertyName.CompareTo(propValue.DisplayName) == 0)
                        {
                            isHidden = true;
                            break;
                        }
                    }
                    if (isHidden)
                        continue;

                    Property property = new Property(propValue.Id, propValue.DisplayName);
                    property.ParentGridInternal = _parentCtrl;
                    property.ValueInternal = propValue;
                    property.Value.Grid = _parentCtrl;
                    if (property.Value.PropertyDescriptor != null)
                        property.Comment = property.Value.PropertyDescriptor.Description;

                    PropertyEnumerator propEnum = AddPropertyUnder(underParent, property);

                    foreach (PropertyDisableAttribute disableAttr in disableAttrs)
                    {
                        if (disableAttr.ChildPropertyName.CompareTo(propValue.DisplayName) == 0)
                        {
                            EnableProperty(propEnum, false);
                            break;
                        }
                    }

                    // If this category is disabled, force the state of the new property to disabled
                    if (IsAncestorDisabled(propEnum))
                        property.Enable(propEnum, false, true, true);

                    _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));
                }
            }

            return HasOneVisibleChild(underParent);
        }

        PropertyEnumerator AddPropertyBefore(PropertyEnumerator beforeEnum, Property property)
        {
            Invalidate();

            PropertyEnumerator propEnum = _activePropertyCollection.InsertBefore(beforeEnum, property);

            if (FirstDisplayedProperty == _activePropertyCollection.GetVisibleDeepEnumerator().RightBound)
            {
                _firstDisplayedLine = 1;
                FirstDisplayedProperty = propEnum.GetVisibleDeepEnumerator();
            }

            AddTargetInstances(propEnum, null, null);

            if (property.Value != null)
            {
                property.Value.OwnerEnumerator = propEnum;

                if (_activePropertyCollection == _categorizedPropertyCollection)
                {
                    if (AddChildProperties(propEnum))
                        ExpandProperty(propEnum, false);
                }
            }

            InvalidateVisibleItemCount();

            if (property.Value != null)
            {
                if (property.Look == null)
                {
                    property.Value.Look = FindLook(beforeEnum.Parent, propEnum);
                    if (propEnum.Property.Look != null)
                        propEnum.Property.Look.ControlHeightMultiplier();
                }

                if ((property.Feel == null) && (_displayMode == PropertyGrid.DisplayModes.Categorized))
                    property.Value.Feel = FindFeel(beforeEnum.Parent, propEnum);

                // Note: the feel must be set before calling IsReadOnly because some readonly properties may in fact
                // not be readonly because of a UITypeEditor (for collections)
                bool readOnly = property.Value.IsReadOnly(propEnum);
                if (readOnly)
                {
                    if (ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.Disabled)
                        EnableProperty(propEnum, false);
                }

                AssignValidator(propEnum);

                HandleAttributes(propEnum);
            }

            InvalidateVisibleItemCount();

            return propEnum;
        }

        private PropertyEnumerator AddPropertyUnder(PropertyEnumerator underParent, Property property)
        {
	        Invalidate();

            PropertyEnumerator propEnum = _activePropertyCollection.Append(underParent.Children, property);

            if (FirstDisplayedProperty == _activePropertyCollection.GetVisibleDeepEnumerator().RightBound)
            {
                _firstDisplayedLine = 1;
                FirstDisplayedProperty = propEnum.GetVisibleDeepEnumerator();
            }

            if (underParent.Property.Value is PropertyValueIndirect)
            {
                object[] targets = underParent.Property.Value.TargetInstances;
                object[] childTargets = new object[targets.Length];
                object[] propertyDescriptors = new object[targets.Length];

                if (targets.Length > 1)
                {
                    for(int i=0;i<targets.Length;i++)
                    {
                        propertyDescriptors[i] = property.Value.PropertyDescriptor;
                        childTargets[i] = underParent.Property.Value.PropertyDescriptor.GetValue(targets[i]);
                    }
                }

                AddTargetInstances(propEnum, childTargets, propertyDescriptors);
            }
            else
                AddTargetInstances(propEnum, null, null);

            if (property.Value != null)
            {
                property.Value.OwnerEnumerator = propEnum;

                if (_activePropertyCollection == _categorizedPropertyCollection)
                {
                    if (AddChildProperties(propEnum))
                        ExpandProperty(propEnum, false);
                }
            }

            InvalidateVisibleItemCount();

            if (property.Value != null)
            {
                if (propEnum.Property.Value.Look == null)
                {
                    propEnum.Property.Value.Look = FindLook(underParent, propEnum);
                    if (propEnum.Property.Value.Look != null)
                        propEnum.Property.Value.Look.ControlHeightMultiplier();
                }

                if (propEnum.Property.Feel == null)
                    property.Value.Feel = FindFeel(underParent, propEnum);

                // Note: the feel must be set before calling IsReadOnly because some readonly properties may in fact
                // not be readonly because of a UITypeEditor (for collections)
                bool readOnly = property.Value.IsReadOnly(propEnum);
                if (readOnly)
                {
                    if (ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.Disabled)
                        EnableProperty(propEnum, false);
                }

                AssignValidator(propEnum);

                HandleAttributes(propEnum);
            }

            return propEnum;
        }

        private void HandleAttributes(PropertyEnumerator propEnum)
        {
            ArrayList disableAttrs = propEnum.Property.Value.GetAttributes(typeof(PropertyDisableAttribute));
            foreach (PropertyDisableAttribute disableAttr in disableAttrs)
            {
                if (disableAttr.ChildPropertyName.Length == 0)
                {
                    EnableProperty(propEnum, false);
                    break;
                }
            }

            PropertyHeightMultiplierAttribute heightMultiplierAttr = propEnum.Property.Value.GetAttribute(
                typeof(PropertyHeightMultiplierAttribute)) as PropertyHeightMultiplierAttribute;
            if (heightMultiplierAttr != null)
                propEnum.Property.HeightMultiplier = heightMultiplierAttr.HeightMultiplier;

            PropertyManuallyDisabledAttribute manuallyDisableAttr = propEnum.Property.Value.GetAttribute(
                typeof(PropertyManuallyDisabledAttribute)) as PropertyManuallyDisabledAttribute;
            if (manuallyDisableAttr != null)
                propEnum.Property.SetManuallyDisabled(propEnum, true);

            ArrayList hideAttrs = propEnum.Property.Value.GetAttributes(typeof(PropertyHideAttribute));
            foreach (PropertyHideAttribute hideAttr in hideAttrs)
            {
                if (hideAttr.ChildPropertyName.Length == 0)
                {
                    ShowProperty(propEnum, false);
                    break;
                }
            }
        }

        private object[] _currentTargetInstancesToAdd;

        private ArrayList _currentPropertyDescriptorsToAdd;

        void AddTargetInstances(PropertyEnumerator propEnum, object[] targets, object[] propertyDescriptors)
        {
            if ((targets == null) && (_currentTargetInstancesToAdd != null))
            {
                targets = _currentTargetInstancesToAdd;
                propertyDescriptors = _currentPropertyDescriptorsToAdd.ToArray();
            }

            if ((targets != null) && (targets.Length > 1))
            {
                PropertyValueIndirect propertyValue = (propEnum.Property.Value as PropertyValueIndirect);
                for (int i = 1; i < targets.Length; i++)
                {
                    propertyValue.AddTargetInstance(
                        targets[i], (PropertyDescriptor)propertyDescriptors[i]);
                }

//                propertyValue.SetupChildrenTargetInstances();
            }
        }

        public PropertyEnumerator AppendProperty(PropertyEnumerator underCategory, int id, string propName, object[] targetInstances, ArrayList propertyDescriptors, string comment, Attribute[] attributes)
        {
            _currentTargetInstancesToAdd = targetInstances;
            _currentPropertyDescriptorsToAdd = propertyDescriptors;
            PropertyEnumerator propEnum = AppendProperty(underCategory, id, propName, targetInstances[0],
                (PropertyDescriptor)propertyDescriptors[0], comment, attributes);
            _currentPropertyDescriptorsToAdd = null;
            _currentTargetInstancesToAdd = null;

            return propEnum;
        }

        public PropertyEnumerator AppendProperty(PropertyEnumerator underCategory, int id, string propName, object container, PropertyDescriptor propertyDescriptor, string comment, Attribute[] attributes)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            Property property = new Property(id, propName);
            property.Comment = comment;
            property.ParentGridInternal = _parentCtrl;
            property.ValueInternal = new PropertyValueIndirect(_parentCtrl, null, container, propertyDescriptor, attributes);

            PropertyEnumerator propEnum = AddPropertyUnder(underCategory, property);

            // Initial check of the validity of the value assigned to the property
            PropertyValidatorBase validator = propEnum.Property.Value.Validator;
            if (validator != null)
                validator.Check(propEnum.Property.Value.GetValue(), true);

            // If this category is disabled, force the state of the new property to disabled
            if (IsAncestorDisabled(propEnum))
                property.Enable(propEnum, false, true, true);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        public PropertyEnumerator AppendProperty(PropertyEnumerator underCategory, int id, string propName, object container, string memberName, string comment, Attribute[] attributes)
        {
            PropertyDescriptor propertyDescriptor = TypeDescriptor.GetProperties(container)[memberName];
            if (propertyDescriptor == null)
                throw new MemberAccessException("The property " + memberName + " doesn't exist in type " + container.GetType());

            return AppendProperty(underCategory, id, propName, container, propertyDescriptor, comment, attributes);
        }

        public PropertyEnumerator InsertProperty(PropertyEnumerator beforeProperty, int id, string propName, object[] targetInstances, ArrayList propertyDescriptors, string comment, Attribute[] attributes)
        {
            _currentTargetInstancesToAdd = targetInstances;
            _currentPropertyDescriptorsToAdd = propertyDescriptors;
            PropertyEnumerator propEnum = InsertProperty(beforeProperty, id, propName, targetInstances[0],
                (PropertyDescriptor)propertyDescriptors[0], comment, attributes);
            _currentPropertyDescriptorsToAdd = null;
            _currentTargetInstancesToAdd = null;

            return propEnum;
        }

        public PropertyEnumerator InsertProperty(PropertyEnumerator beforeProperty, int id, string propName, object container, PropertyDescriptor propertyDescriptor, string comment, Attribute[] attributes)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            Property property = new Property(id, propName);
            property.Comment = comment;
            property.ParentGridInternal = _parentCtrl;
            property.ValueInternal = new PropertyValueIndirect(_parentCtrl, null, container, propertyDescriptor, attributes);

            PropertyEnumerator propEnum = AddPropertyBefore(beforeProperty, property);

            // Initial check of the validity of the value assigned to the property
            PropertyValidatorBase validator = propEnum.Property.Value.Validator;
            if (validator != null)
                validator.Check(propEnum.Property.Value.GetValue(), true);

            // Simulate a string value change in case we are in a grouped property
            //            if (underCategory.Property.Value != null)
            //                property.Value.SetValue(property.Value.GetStringValue());

            // If this category is disabled, force the state of the new property to disabled
            if (IsAncestorDisabled(propEnum))
                property.Enable(propEnum, false, true, true);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        public PropertyEnumerator InsertProperty(PropertyEnumerator beforeProperty, int id, string propName, object container, string memberName, string comment, Attribute[] attributes)
        {
            PropertyDescriptor propertyDescriptor = TypeDescriptor.GetProperties(container)[memberName];
            if (propertyDescriptor == null)
                throw new MemberAccessException("The property " + memberName + " doesn't exist in type " + container.GetType());

            return InsertProperty(beforeProperty, id, propName, container, propertyDescriptor, comment, attributes);
        }

        public void AddTargetInstance(PropertyEnumerator propEnum, object targetInstance, string memberName)
        {
            if (propEnum.Property.Value is PropertyValueIndirect == false)
                throw new ArgumentException("The property must already have a target instance.", "propEnum");

            ((PropertyValueIndirect)propEnum.Property.Value).AddTargetInstance(targetInstance, memberName);
        }

        public PropertyEnumerator AppendManagedProperty(PropertyEnumerator underCategory, int id, string propName, Type valueType, object initialValue, string comment, params Attribute[] attributes)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            Property property = new Property(id, propName);
            property.Comment = comment;
            property.ParentGridInternal = _parentCtrl;
            property.ValueInternal = new PropertyValueManaged(id, valueType, initialValue, attributes);
            property.Value.Grid = _parentCtrl;

            PropertyEnumerator propEnum = AddPropertyUnder(underCategory, property);

            // Initial check of the validity of the value assigned to the property
            PropertyValidatorBase validator = propEnum.Property.Value.Validator;
            if (validator != null)
                validator.Check(propEnum.Property.Value.GetValue(), true);

            // Simulate a string value change in case we are in a grouped property
//            if (underCategory.Property.Value != null)
//                property.Value.SetValue(initialValue);

            // If this category is disabled, force the state of the new property to disabled
            if (IsAncestorDisabled(propEnum))
                property.Enable(propEnum, false, true, true);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        public PropertyEnumerator InsertManagedProperty(PropertyEnumerator beforeProperty, int id, string propName, Type valueType, object initialValue, string comment, Attribute[] attributes)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            Property property = new Property(id, propName);
            property.Comment = comment;
            property.ParentGridInternal = _parentCtrl;
            property.ValueInternal = new PropertyValueManaged(id, valueType, initialValue, attributes);
            property.Value.Grid = _parentCtrl;

            PropertyEnumerator propEnum = AddPropertyBefore(beforeProperty, property);

            // Initial check of the validity of the value assigned to the property
            PropertyValidatorBase validator = propEnum.Property.Value.Validator;
            if (validator != null)
                validator.Check(propEnum.Property.Value.GetValue(), true);

            // Simulate a string value change in case we are in a grouped property
            //            if (underCategory.Property.Value != null)
            //                property.Value.SetValue(initialValue);

            // If this category is disabled, force the state of the new property to disabled
            if (IsAncestorDisabled(propEnum))
                property.Enable(propEnum, false, true, true);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        public PropertyEnumerator AppendHyperLinkProperty(PropertyEnumerator underCategory, int id, string propName, string comment)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            Property property = new PropertyHyperLink(id);
            property.DisplayName = propName;
            property.Comment = comment;
            property.ParentGridInternal = _parentCtrl;

            PropertyEnumerator propEnum = AddPropertyUnder(underCategory, property);

            if (IsAncestorDisabled(propEnum))
                property.Enable(propEnum, false, true, true);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        public PropertyEnumerator InsertHyperLinkProperty(PropertyEnumerator beforeProperty, int id, string propName, string comment)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            Property property = new PropertyHyperLink(id);
            property.DisplayName = propName;
            property.Comment = comment;
            property.ParentGridInternal = _parentCtrl;

            PropertyEnumerator propEnum = AddPropertyBefore(beforeProperty, property);

            if (IsAncestorDisabled(propEnum))
                property.Enable(propEnum, false, true, true);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        public PropertyEnumerator AppendSubCategory(PropertyEnumerator underCategory, int id, string catName)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            SubCategory subCategory = new SubCategory(id, catName);

            // The new property item must be attached to the same properties control
            subCategory.ParentGridInternal = _parentCtrl;

            PropertyEnumerator propEnum = AddPropertyUnder(underCategory, subCategory);

            // If this category is disabled, force the state of the new property to disabled
            if (IsAncestorDisabled(propEnum))
                subCategory.Enable(propEnum, false, true, true);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        public PropertyEnumerator InsertSubCategory(PropertyEnumerator beforeProperty, int id, string catName)
        {
            System.Diagnostics.Debug.Assert(DisplayMode == PropertyGrid.DisplayModes.Categorized, "Properties can only be inserted in categorized mode.");

            SubCategory subCategory = new SubCategory(id, catName);

            // The new property item must be attached to the same properties control
            subCategory.ParentGridInternal = _parentCtrl;

            PropertyEnumerator propEnum = AddPropertyBefore(beforeProperty, subCategory);

            // If this category is disabled, force the state of the new property to disabled
            if (IsAncestorDisabled(propEnum))
                subCategory.Enable(propEnum, false, true, true);

            CheckScrollbar();

            _parentCtrl.OnPropertyCreated(new PropertyCreatedEventArgs(propEnum));

            return propEnum;
        }

        internal bool IsAncestorDisabled(PropertyEnumerator propEnum)
        {
            PropertyEnumerator currentEnum = propEnum.Parent;
            while (currentEnum != currentEnum.RightBound)
            {
                if (currentEnum.Property.GetManuallyDisabledVariable() == false)
                    return true;

                currentEnum = currentEnum.Parent;
            }

	        return false;
        }

        public bool HasOneVisibleChild(PropertyEnumerator propEnum)
        {
            PropertyEnumerator childrenEnumerator = propEnum.Children;
            while (childrenEnumerator != childrenEnumerator.RightBound)
            {
                if (childrenEnumerator.Property.Visible)
                    return true;

                childrenEnumerator.MoveNext();
            }

            return false;
        }

        public bool DrawImage(int imageIndex, Graphics graphics, int x, int y)
        {
            try
            {
                Image img = _imageList.Images[imageIndex];
                Win32Calls.ImageListDraw(_imageList, imageIndex, graphics, x, y, img.Width, img.Height);
			}
            catch(Exception)
            {
                return false;
            }

            return true;
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            // Find the location of the mouse cursor
            PropertyEnumerator propEnum;
            Rectangle itemRect;
            PropertyGrid.HitTests hitTest = HitTest(new Point(e.X, e.Y), out propEnum, out itemRect);

            if (((e.Clicks == 2) || _doubleClickDone) && (hitTest != PropertyGrid.HitTests.TreeGlyph) &&
                (hitTest != PropertyGrid.HitTests.ManualDisable))
            {
                if (e.Button == MouseButtons.Left)
                {
                    Property property = propEnum.Property;

                    if (property != null)
                    {
                        if (hitTest == PropertyGrid.HitTests.ColumnSplit)
                        {
                            AdjustLabelColumn();
                        }
                        else if ((hitTest == PropertyGrid.HitTests.Label) && (InPlaceControl != null) &&
                            (HasOneVisibleChild(propEnum) == false))
                        {
                            bool valueChanged = ((IInPlaceControl)InPlaceControl).ScrollValueOnDoubleClick(
                                !(ModifierKeys == Keys.Shift), ScrollValueOnDoubleClickSource.FromLabel, true);
                            if (!valueChanged && !_currentInPlaceControl.ContainsFocus)
                                _parentCtrl.ActiveControl = _currentInPlaceControl;
                            if (!valueChanged || ((_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.TabKey) &&
                                ((_parentCtrl.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyWithAutoFocus) != 0)))
                                SelectAllText();
                        }
                        else
                        {
                            if (propEnum.Children.Count > 0)
                                ExpandProperty(propEnum, !property.Expanded);
                            else if (property.Enabled && (property is RootCategory == false) && (property is PropertyHyperLink == false))
                                Win32Calls.keybd_event(Win32Calls.VK_RETURN, 0, 0, UIntPtr.Zero);
                        }
                    }
                }
            }
            else
            {
                // Do nothing if clicking the label of an already selected property under tabkey+autofocus mode
                if ((_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.TabKey) &&
                    ((_parentCtrl.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyWithAutoFocus) != 0))
                {
                    if (!(propEnum.Property is PropertyHyperLink) && (propEnum == SelectedPropertyEnumerator) && ContainsFocus)
                    {
                        base.OnMouseDown(e);
                        return;
                    }
                }

                bool hasFocus = true;

                // Focus must be regained if lost
                if ((Focused == false) && CanSelectProperties)
                {
                    if (ContainsFocus == false)    // No child has focus ?
			            hasFocus = false;

                    // Here I use a trick to avoid calling ShowInPlaceControl when we set the Focus and then
                    // HideInPlaceControl and ShowInPlaceControl again when selecting the property later in this
                    // method. By setting _dontShowInPlaceCtrlInGotFocus to true it will prevent ShowInPlaceControl to be called.
                    // But it must be called when we click on the property that is already selected because
                    // SelectProperty won't.
                    if (SelectedPropertyEnumerator != propEnum)
                        _dontShowInPlaceCtrlInGotFocus = true;

                    Focus();

		            Invalidate();
	            }

                if (hitTest == PropertyGrid.HitTests.ColumnSplit)
                {
                    if (e.Button == MouseButtons.Left)
                    {
                        Cursor = Cursors.VSplit;
                        Capture = true;
                        _resizingColumns = true;
                    }
                }
                else
                {
                    Property property = propEnum.Property;

		            if (property != null)
		            {
                        if ((hitTest == PropertyGrid.HitTests.ManualDisable) &&
                            (Enabled || (DisableMode == PropertyGrid.DisableModes.None)) &&
                            (e.Button == MouseButtons.Left))
                        {
                            if (propEnum.HasParent == false)
                                EnableProperty(propEnum, !property.Enabled);
                            else
                            {
                                // The user can press on the checkbox only if its parent category is enabled
                                if (propEnum.Parent.Property.Enabled)
                                    EnableProperty(propEnum, !property.Enabled);
                            }
                        }

                        if (hitTest == PropertyGrid.HitTests.TreeGlyph)						// No selection when clicking +/-
			            {
                            if (e.Button == MouseButtons.Left)
                                ExpandProperty(propEnum, !property.Expanded);
			            }
                        else if (CanSelectProperties)
			            {
                            if ((ModifierKeys == Keys.Shift) && (_multiSelectMode != PropertyGrid.MultiSelectModes.None))
                            {
					            PropertyEnumerator ancestorEnum = null;
                                if (_multiSelectMode == PropertyGrid.MultiSelectModes.SameLevel)
                                {
                                    if (SelectedPropertyEnumerator.Children.Count == 0)
							            ancestorEnum = SelectedPropertyEnumerator.Parent;
						            else
							            ancestorEnum = SelectedPropertyEnumerator;
					            }

					            // Clear previous multi selection
                                foreach(PropertyVisibleDeepEnumerator currentEnum in _multiSelectedItems)
					            {
                                    currentEnum.Property.SelectedInternal = false;
					            }
					            _multiSelectedItems.Clear();

					            Rectangle r = GetAbsoluteItemRect(SelectedPropertyEnumerator);
					            PropertyVisibleDeepEnumerator selectEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();
					            do
					            {
						            if (r.Bottom < e.Y)
							            selectEnum.MoveNext();
						            else if (r.Top > e.Y)
							            selectEnum.MovePrev();

						            // In SAMELEVEL mode, we must test if we are not outside the correct level
                                    if (_multiSelectMode == PropertyGrid.MultiSelectModes.SameLevel)
                                    {
							            if (selectEnum.IsDescendantOf(ancestorEnum.GetDeepEnumerator()) == false)
								            break;
						            }

						            if (selectEnum.Children.Count == 0)
						            {
							            if (_parentCtrl.OnValidateMultiSelection(SelectedPropertyEnumerator, selectEnum))
							            {
                                            selectEnum.Property.SelectedInternal = true;
								            _multiSelectedItems.Add(selectEnum);
							            }
						            }
					            }
					            while(selectEnum != propEnum);
					            Invalidate();
				            }
                            else if ((ModifierKeys == Keys.Control) && (_multiSelectMode != PropertyGrid.MultiSelectModes.None))
                            {
					            bool selectImpossible = false;
                                if (_multiSelectMode == PropertyGrid.MultiSelectModes.SameLevel)
                                {
    					            PropertyEnumerator ancestorEnum;
                                    if (SelectedPropertyEnumerator.Children.Count == 0)
							            ancestorEnum = SelectedPropertyEnumerator.Parent;
						            else
							            ancestorEnum = SelectedPropertyEnumerator;

                                    if (propEnum.IsDescendantOf(ancestorEnum.GetDeepEnumerator()) == false)
							            selectImpossible = true;
					            }

					            _clickWithControl = true;

					            if (selectImpossible == false)
					            {
						            // If the main selected property is clicked to be unselected
						            if ((propEnum == SelectedPropertyEnumerator) && (_multiSelectedItems.Count > 0))
						            {
							            // Assign the main selected property to the first property of the multiselected set
                                        SelectProperty((PropertyEnumerator)_multiSelectedItems[0], new Point(e.X, e.Y));

							            // Remove the newly selected property from the multiselected set
                                        _multiSelectedItems.RemoveAt(0);
						            }
						            else if (propEnum.Children.Count == 0)
						            {
							            // If the clicked item must be unselected
							            if (propEnum.Property.Selected)
							            {
								            if (propEnum != SelectedPropertyEnumerator)
								            {
									            // Unselect the clicked property
                                                propEnum.Property.SelectedInternal = false;

									            // And remove it from the multiselected set
                                                _multiSelectedItems.Remove(propEnum);
								            }
							            }
							            // Else if the clicked item must be selected
							            else
							            {
								            if (_parentCtrl.OnValidateMultiSelection(SelectedPropertyEnumerator, propEnum.GetVisibleDeepEnumerator()))
								            {
                                                propEnum.Property.SelectedInternal = true;
									            _multiSelectedItems.Add(propEnum);
								            }
							            }
						            }

						            Invalidate();
					            }

					            _clickWithControl = false;
				            }
				            else
				            {
                                if ((_multiSelectedItems.Count > 0) && ((propEnum.Property.Selected && (hitTest == PropertyGrid.HitTests.Value)) || (hasFocus == false)))
                                {
                                    _clickWithControl = true;

                                    _multiSelectedItems.Add(SelectedPropertyEnumerator);
                                    PropertyVisibleDeepEnumerator oldSelectedEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();

                                    SelectProperty(propEnum, new Point(e.X, e.Y));

                                    oldSelectedEnum.Property.SelectedInternal = true;

                                    // Remove the new main selected property from the multiselected set
                                    _multiSelectedItems.Remove(propEnum);

                                    _clickWithControl = false;
                                }
                                else
                                {
                                    SelectProperty(propEnum, new Point(e.X, e.Y));
                                }
                            }

                            // We forward the click to the inplace control.
                            // We don't do it if Ctrl is pressed and a hyperlink has been defined for the property
				            if ((_currentInPlaceControl != null) &&
                                ((ModifierKeys != Keys.Control) || (property.HyperLinkFormat.Length == 0)) &&
                                (e.Button == MouseButtons.Left))
				            {
                                Rectangle rect = _currentInPlaceControl.Bounds;
                                Point pt = PointToScreen(new Point(e.X, e.Y));
                                pt = _currentInPlaceControl.PointToClient(pt);
                                if (rect.Contains(new Point(e.X, e.Y)) && !TryOnForbidClickThrough(pt))
                                {
                                    // Obsolete comment:
                                    // I release the capture because I discovered that the .NET framework takes the Capture while
                                    // handling the left mouse button. It releases it in the MouseUp handler.
                                    // Without releasing the Capture, my mouse_event call would not work as expected.
//                                    Capture = false;
//                                    _currentInPlaceControl.Focus();

                                    // Mouse position in the inplace control
                                    Point mousePos = PointToScreen(new Point(e.X, e.Y));
                                    mousePos = _currentInPlaceControl.PointToClient(mousePos);
                                    // Let's find if the inplace control has a child that would receive the mouse click
                                    IntPtr handle = IntPtr.Zero;
                                    if (_currentInPlaceControl.HasChildren)
                                    {
                                        Control child = _currentInPlaceControl.GetChildAtPoint(mousePos);
                                        if (child != null)
                                        {
                                            handle = child.Handle;
                                            mousePos = PointToScreen(new Point(e.X, e.Y));
                                            mousePos = child.PointToClient(mousePos);
                                        }
                                    }
                                    if (handle == IntPtr.Zero)
                                        handle = _currentInPlaceControl.Handle;
                                    // Send to the inplace control (or its child) a button down event
                                    _parentCtrl.ClickThroughInProgress = true;
                                    int mousePosMsg = Win32Calls.MakeLong(mousePos.X, mousePos.Y);
                                    Win32Calls.SendMessage(handle, Win32Calls.WM_LBUTTONDOWN, 0, mousePosMsg);
                                    // No up button is generated so that text selection can be immediate
//                                    Win32Calls.SendMessage(handle, Win32Calls.WM_LBUTTONUP, 0, mousePosMsg);
                                    _parentCtrl.ClickThroughInProgress = false;
//                                    _mouseHook.HookMessages = true;
                                    _lastTimeClickedFromParent = DateTime.Now.Ticks;
                                    _lastPosClickedInParent = PointToScreen(new Point(e.X, e.Y));
                                }
                            }

                            if ((hitTest != PropertyGrid.HitTests.ManualDisable) && (property.Enabled) &&
                                (e.Clicks == 1) && (e.Button == MouseButtons.Left))
                                property.OnPropertyClicked(propEnum, new Point(e.X, e.Y), itemRect);
                        }
		            }
                }
            }

            base.OnMouseDown(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (_resizingColumns)
            {
                SetLabelColumnWidthRatio((double)(e.X - LeftColumnWidth) / (ClientRectangle.Width - LeftColumnWidth));
            }
            else
            {
                Rectangle itemRect = Rectangle.Empty;
                PropertyEnumerator enumerator = null;
                if (ToolTipMode != PropertyGrid.ToolTipModes.None)
                    enumerator = PropertyItemFromPoint(new Point(e.X, e.Y), out itemRect);

                _toolTipLocation = Point.Empty;

                if ((ToolTipMode & PropertyGrid.ToolTipModes.ToolTipsOnValues) != 0)
                {
                    Property property = enumerator.Property;
                    if ((property != null) && !(property is RootCategory) && !property.Selected)
                    {
                        Rectangle valueRect = property.GetValueRect(itemRect);

                        if (valueRect.Contains(new Point(e.X, e.Y)) && (property.Value != null))
                        {
                            // Get the bounding rectangle for the string displayed in the control
                            Graphics graphics = CreateGraphics();
                            Rectangle strRect = property.Value.GetStringValueRect(graphics, valueRect, new Point(e.X, e.Y));

                            strRect.Height--;
                            if (strRect.Contains(new Point(e.X, e.Y)))
                            {
                                strRect.Height++;
                                Rectangle screenRect = Screen.GetWorkingArea(this);
                                Rectangle strScreenRect = strRect;
                                strScreenRect = RectangleToScreen(strScreenRect);

                                if ((strRect.Right > valueRect.Right) || (strScreenRect.Right > screenRect.Right) ||
                                    (strRect.Bottom > valueRect.Bottom))
                                {
                                    ToolTip.Font = property.Value.Font;

                                    Point pt = new Point(strRect.Left - 1, strRect.Top);
                                    pt = PointToScreen(pt);
                                    if ((PropertyVerticalMargin == 0) || (PropertyVerticalMargin == 2))
                                        pt.Y++;

                                    int line = (e.Y - valueRect.Top) / _basicPropertyHeight;

                                    // We also need the actual height of the text to set the tooltip margin
                                    Size size = property.Value.GetTooltipStringSize(graphics, valueRect, new Point(e.X, e.Y), line);
                                    int diff = (strRect.Height + 1 - size.Height - 4);
                                    int topMargin = diff % 2 + diff / 2;
                                    int bottomMargin = diff / 2;
                                    ToolTip.SetMargins(_parentCtrl.GlobalTextMargin - 2, topMargin, _parentCtrl.GlobalTextMargin - 2, bottomMargin);
                                    Win32Calls.SendMessage(ToolTip.Handle, Win32Calls.TTM_SETMAXTIPWIDTH, 0, size.Width);

                                    if (strScreenRect.Right > screenRect.Right)
                                        pt.X = screenRect.Right - size.Width - _parentCtrl.GlobalTextMargin;
                                    pt.X -= _parentCtrl.GlobalTextMargin;
                                    _toolTipLocation = new Point(pt.X, pt.Y - 1);

                                    ToolTip.SetText(property.Value.GetTooltipString(line), enumerator);

                                    //                                    int bk = System.Drawing.ColorTranslator.ToWin32(Color.White);
                                    //                                    Win32Calls.SendMessage(ToolTip.Handle, 0x0400 + 19, bk, ToolTip.GetTOOLINFO());
                                }
                            }

                            graphics.Dispose();
                        }
                    }
                }

                if ((ToolTipMode & PropertyGrid.ToolTipModes.ToolTipsOnLabels) != 0)
                {
                    Property property = enumerator.Property;
                    if ((property != null) && !(property is RootCategory) && (property.Value != null))
                    {
                        Rectangle labelRect = property.GetLabelTextRect(itemRect, enumerator);

                        // Get the bounding rectangle for the string displayed in the control
                        Graphics graphics = CreateGraphics();
                        Size size = Win32Calls.GetTextExtent(graphics, property.DisplayName, property.Font);
                        Rectangle strRect = labelRect;
                        strRect.Width = size.Width;
                        strRect.Height = _basicPropertyHeight;
                        graphics.Dispose();

                        if (strRect.Right > LeftColumnWidth + 1 + LabelColumnWidth)
                            strRect.Width = LeftColumnWidth + 1 + LabelColumnWidth - strRect.Left;

                        if (strRect.Contains(new Point(e.X, e.Y)))
                        {
                            Rectangle screenRect = Screen.GetWorkingArea(this);
                            Rectangle strScreenRect = labelRect;
                            strScreenRect.Width = size.Width;
                            strScreenRect = RectangleToScreen(strScreenRect);

                            int rightLimit = LeftColumnWidth + 1 + LabelColumnWidth;
                            if ((_parentCtrl.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnLabels) != 0)
                                rightLimit -= _parentCtrl.GlobalTextMargin;

                            if ((strRect.Left + size.Width > rightLimit) || (strScreenRect.Right > screenRect.Right))
                            {
                                ToolTip.Font = property.Font;

                                Point pt = new Point(strRect.Left - 1, strRect.Top);
                                pt = PointToScreen(pt);
                                if ((PropertyVerticalMargin == 0) || (PropertyVerticalMargin == 2))
                                    pt.Y++;

                                // We also need the actual height of the text to set the tooltip margin
                                int diff = (_basicPropertyHeight + 1 - size.Height - 4);
                                int topMargin = diff % 2 + diff / 2;
                                int bottomMargin = diff / 2;
                                ToolTip.SetMargins(_parentCtrl.GlobalTextMargin - 2, topMargin, _parentCtrl.GlobalTextMargin - 2, bottomMargin);
                                Win32Calls.SendMessage(ToolTip.Handle, Win32Calls.TTM_SETMAXTIPWIDTH, 0, size.Width);

                                if (strScreenRect.Right > screenRect.Right)
                                    pt.X = screenRect.Right - size.Width - _parentCtrl.GlobalTextMargin;
                                pt.X -= _parentCtrl.GlobalTextMargin;
                                _toolTipLocation = new Point(pt.X, pt.Y - 1);

                                ToolTip.SetText(property.DisplayName, enumerator);
                            }
                        }
                    }
                }

                if ((ToolTip != null) && (_toolTipLocation == Point.Empty))
                    ToolTip.SetText("", enumerator);
            }

            // OnSetCursor
            //------------

            PropertyEnumerator propEnum;
            Rectangle itemRect2;
            PropertyGrid.HitTests hitTest = HitTest(new Point(e.X, e.Y), out propEnum, out itemRect2);
            if (_resizingColumns == false)
            {
                if (hitTest == PropertyGrid.HitTests.ColumnSplit)
                {
                    Cursor = Cursors.VSplit;
                }
                else
                {
                    bool cursorSet = false;

                    if (Enabled || (DisableMode == PropertyGrid.DisableModes.None))
                    {
                        if (propEnum.Property != null)
                        {
                            if (propEnum.Property.OverrideCursor(new Point(e.X, e.Y), itemRect2, propEnum))
                                cursorSet = true;
                        }
                    }

                    if (cursorSet == false)
                        Cursor = Cursors.Arrow;
                }
            }

            base.OnMouseMove(e);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            if (_resizingColumns)
            {
                _resizingColumns = false;
                Capture = false;
            }

            base.OnMouseUp(e);
        }
        
        protected override void OnMouseWheel(MouseEventArgs e)
        {
            if (Focused)
            {
                // Check to see if there is a value to validate
                if ((InPlaceControl != null) && InPlaceControl.ContainsFocus)
                {
                    PropertyValue.ValueValidationResult changed = (_currentInPlaceControl as IInPlaceControl).CommitChanges(true);
                    if (((changed & PropertyValue.ValueValidationResult.ErrorCode) != 0) &&
                        (_parentCtrl.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                    {
                        SelectAllText();

                        base.OnMouseWheel(e);
                        return;
                    }
                }

                if (IsScrollbarVisible())
                {
                    int nb = e.Delta / 120;

                    if (e.Delta > 0)
                    {
                        for (int i = 0; i < nb; i++)
                            Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEUP, 0), 0);
                        Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_ENDSCROLL, 0), 0);
                    }
                    else if (e.Delta < 0)
                    {
                        for (int i = 0; i < -nb; i++)
                            Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEDOWN, 0), 0);
                        Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_ENDSCROLL, 0), 0);
                    }
                }

                // Simulate a mouse move so that the tooltip can be updated if any
                Point point = PointToClient(new Point(e.X, e.Y));
                Win32Calls.PostMessage(Handle, Win32Calls.WM_MOUSEMOVE, 0, Win32Calls.MakeLong(point.X, point.Y));
            }

            base.OnMouseWheel(e);
        }
        
        public PropertyGrid.HitTests HitTest(Point point, out PropertyEnumerator propEnum, out Rectangle itemRect)
        {
            propEnum = PropertyItemFromPoint(point, out itemRect);

            if (propEnum.Property != null)	// On an item row ?
            {
                if ((point.X >= LeftColumnWidth + 1 + LabelColumnWidth - 1) &&
                    (point.X <= LeftColumnWidth + 1 + LabelColumnWidth + 1))
                {
                    if ((_parentCtrl.DrawManager.ShowCategoryVerticalLine || (propEnum.Property is RootCategory == false)) &&
                        (propEnum.Property is PropertyHyperLink == false) && (_columnsLock == false))
                        return PropertyGrid.HitTests.ColumnSplit;
                }

                Rectangle signRect = Rectangle.Empty;
                signRect = propEnum.Property.GetSignRect(itemRect, propEnum);
                if (signRect != Rectangle.Empty)
                {
                    if (signRect.Contains(point))
                        return PropertyGrid.HitTests.TreeGlyph;
                }

                if ((DisplayMode == PropertyGrid.DisplayModes.Categorized) &&
                    (point.X <= LeftColumnWidth))
                    return PropertyGrid.HitTests.LeftColumn;

                if (propEnum.Property.CanBeDisabledManually)
                {
                    Rectangle checkRect = propEnum.Property.GetManuallyDisableRect(itemRect, propEnum);
                    if (checkRect.Contains(point))
                        return PropertyGrid.HitTests.ManualDisable;
                }

                if (point.X > LeftColumnWidth + 1 + LabelColumnWidth + 1)
                {
                    if (propEnum.Property is RootCategory == false)
                        return PropertyGrid.HitTests.Value;
                }

                Rectangle labelRect = propEnum.Property.GetLabelColumnRect(itemRect, propEnum);
                if (labelRect.Contains(point))
                    return PropertyGrid.HitTests.Label;
            }

            return PropertyGrid.HitTests.Client;
        }

        public PropertyEnumerator PropertyItemFromPoint(Point point, out Rectangle itemRect)
        {
            // Restrict the search to the list area
            Rectangle clientRect = ClientRectangle;
            itemRect = new Rectangle(-1, -1, -1, -1);

            // If above or below the list, return NULL
            if ((point.Y > clientRect.Bottom) || (point.Y < clientRect.Top))
                return _activePropertyCollection.GetDeepEnumerator().RightBound;

            // Scan the items from top to bottom
            PropertyEnumerator currentEnum = FirstDisplayedProperty;
            while (currentEnum != currentEnum.RightBound)
            {
                Rectangle rect = GetItemRect(currentEnum);

                if (rect.Contains(point))	// Success
                {
                    itemRect = rect;
                    break;
                }

                currentEnum.MoveNext();
            }

            return currentEnum;
        }

        public bool IsPropertyDisplayed(PropertyEnumerator propEnum)
        {
            return (GetItemRect(propEnum) != new Rectangle(-1, -1, -1, -1));
        }

        public Rectangle GetItemRect(PropertyEnumerator propEnum)
        {
            if (propEnum == propEnum.RightBound)
                return new Rectangle(-1, -1, -1, -1);

            Property property = propEnum.Property;

            // Calculate the rect of the first displayed item

            Rectangle clientRect = ClientRectangle;

            Rectangle itemRect = clientRect;
            itemRect.Y -= (_firstDisplayedLine - 1) * _basicPropertyHeight;
            itemRect.Height += (_firstDisplayedLine - 1) * _basicPropertyHeight;

            PropertyEnumerator currentEnum = FirstDisplayedProperty;
            while(currentEnum.Property != null)
            {
                int itemHeight = _basicPropertyHeight * currentEnum.Property.HeightMultiplier;
                itemRect.Height = itemHeight;

                if ((property == currentEnum.Property) && (itemRect.Top < clientRect.Bottom))
                    break;

                if (itemRect.Top >= clientRect.Bottom)	// out of the window
                {
                    itemRect = new Rectangle(-1, -1, -1, -1);
                    break;
                }

                itemRect.Offset(0, itemHeight);

                currentEnum.MoveNext();
            }

            if (currentEnum.Property == null)
                itemRect = new Rectangle(-1, -1, -1, -1);

            return itemRect;
        }

        Rectangle GetAbsoluteItemRect(PropertyEnumerator propEnum)
        {
            Property rectProperty = propEnum.Property;
            PropertyVisibleDeepEnumerator currentEnum = FirstDisplayedProperty;
            Property currentProperty = currentEnum.Property;

            if (currentProperty == null)
                return new Rectangle(-1, -1, -1, -1);

            // Calculate the rect of the first displayed item

            Rectangle clientRect = ClientRectangle;

            Rectangle itemRect = clientRect;
            itemRect.Y -= (_firstDisplayedLine - 1) *_basicPropertyHeight;
            itemRect.Height = currentProperty.HeightMultiplier * _basicPropertyHeight;

            // Find our item above the first displayed item
            while(currentProperty != null)
            {
                int itemHeight = _basicPropertyHeight * currentProperty.HeightMultiplier;
                itemRect.Y = itemRect.Bottom - itemHeight;
                itemRect.Height = itemHeight;

                if (currentProperty == rectProperty)
                    break;

                currentEnum.MovePrev();
                currentProperty = currentEnum.Property;

                itemRect.Height = 0;
	        }

            if (currentProperty != null)
                return itemRect;

	        // Find our item below the first displayed item

	        itemRect = clientRect;
	        itemRect.Y -= (_firstDisplayedLine - 1) * _basicPropertyHeight;

            currentEnum = FirstDisplayedProperty;
            currentProperty = currentEnum.Property;
            while (currentProperty != null)
            {
                int itemHeight = _basicPropertyHeight * currentProperty.HeightMultiplier;
                itemRect.Height = itemHeight;

                if (currentProperty == rectProperty)
                    break;

                itemRect.Offset(0,itemHeight);

                currentEnum.MoveNext();
                currentProperty = currentEnum.Property;
            }

            if (currentProperty != null)
                return itemRect;

	        return new Rectangle(-1,-1,-1,-1);
        }

        private void _EnableCategoryItem(PropertyEnumerator propEnum, bool enable, bool ancestorDisabled, bool direct)
        {
	        // Change the state of the item
            Property property = propEnum.Property;
            property.Enable(propEnum, enable, ancestorDisabled, direct);

            PropertyEnumerator childEnumerator = propEnum.Children.GetSiblingEnumerator();
            while (childEnumerator != childEnumerator.RightBound)
            {
                Property childProperty = childEnumerator.Property;

                if (childEnumerator.Children.Count > 0)
                {
                    _EnableCategoryItem(childEnumerator, enable, !property.GetManuallyDisabledVariable() || ancestorDisabled, false);
                }
		        else
		        {
                    childProperty.Enable(childEnumerator, enable, !property.GetManuallyDisabledVariable() || ancestorDisabled, false);
                }

                childEnumerator.MoveNext();
            }
        }

        public void EnableProperty(PropertyEnumerator propEnum, bool enable)
        {
            if (propEnum == propEnum.RightBound)
		        return;

            Property property = propEnum.Property;
            if (property.Enabled != enable)
            {
                if (HasOneVisibleChild(propEnum))
                {
                    _EnableCategoryItem(propEnum, enable, IsAncestorDisabled(propEnum), true);
                }
                else
                {
                    property.Enable(propEnum, enable, IsAncestorDisabled(propEnum), true);
                }

                if ((SelectedPropertyEnumerator.Property == property) && !enable && (_currentInPlaceControl != null))
                    HideInPlaceControl();
                else if (enable && (SelectedPropertyEnumerator.Property == property) && (_currentInPlaceControl == null))
                    ShowInPlaceControl(SelectedPropertyEnumerator);

                Invalidate();
            }
        }

        public void SelectProperty(PropertyEnumerator enumerator)
        {
            SelectProperty(enumerator, Point.Empty);
        }

        private void SelectProperty(PropertyEnumerator enumerator, Point mousePos)
        {
            if ((enumerator.Property != null) && (CheckPropertyVisibility(enumerator) == false))
                throw new ArgumentException("The property enumerator passed to SelectProperty must point to a visible property.", "enumerator");

	        // Clear multi selected items
	        bool multiSelectionCleared = false;
	        if (!_clickWithControl && (_multiSelectedItems.Count > 0))
	        {
                foreach(PropertyVisibleDeepEnumerator currentEnum in _multiSelectedItems)
		        {
                    if (currentEnum != _selectedPropertyEnum)
                        currentEnum.Property.SelectedInternal = false;
                }

		        _multiSelectedItems.Clear();
		        multiSelectionCleared = true;
	        }

            bool selectedPropertyChanged = ((_selectedPropertyEnum != enumerator) || multiSelectionCleared);
            PropertyEnumerator previousSelectedPropertyEnum = _selectedPropertyEnum;

            if ((enumerator != enumerator.RightBound) && (enumerator.RightBound != null))
            {
                if (selectedPropertyChanged)
                {
                    if (_selectedPropertyEnum != _selectedPropertyEnum.RightBound)
                        _selectedPropertyEnum.Property.SelectedInternal = false;

                    _selectedPropertyEnum = enumerator.GetVisibleDeepEnumerator();

                    if (IsHandleCreated)
                    {
                        Rectangle clientRect = ClientRectangle;
                        Rectangle rect = GetItemRect(enumerator);

                        // Show the inplace control, even if partially visible
                        if ((rect.Top < clientRect.Bottom) && (rect.Bottom > clientRect.Top))
                            ShowInPlaceControl(enumerator, mousePos);
                    }

                    if (selectedPropertyChanged)
                        _selectedPropertyEnum.Property.SelectedInternal = true;
                }
                else
                {
                    // If the selected property didn't change we must still ensure that the text will be selected
                    // if the navigation mode says so
                    if ((_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.TabKey) &&
                        ((_parentCtrl.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyWithAutoFocus) != 0))
                    {
                        if (_currentInPlaceControl != null)
                        {
                            if (!_currentInPlaceControl.ContainsFocus)
                                FocusTextBox();
                            SelectAllText();
                        }
                    }
                }
            }
		    else if (selectedPropertyChanged)	// No item selected
		    {
                if (IsHandleCreated)
                {
				    // We hide the in-place control
    			    HideInPlaceControl();
			    }

                if (_selectedPropertyEnum.Property != null)
                    _selectedPropertyEnum.Property.SelectedInternal = false;

                _selectedPropertyEnum = enumerator.GetVisibleDeepEnumerator();
            }

            if (selectedPropertyChanged)
            {
                if (IsHandleCreated)
                {
                    if (_parentCtrl.CommentsVisibility)
                        _parentCtrl.Refresh();
                    else
                        Refresh();
                }

                // Notify the derived class
                if (_selectedPropertyEnum != RightBound)
                    _parentCtrl.OnPropertySelected(new PropertySelectedEventArgs(_selectedPropertyEnum,
                        previousSelectedPropertyEnum));
            }
        }

        public void SelectAndFocusProperty(PropertyEnumerator enumerator, bool selectAllText)
        {
            SelectProperty(enumerator, Point.Empty);

            if ((enumerator.Property != null) && (InPlaceControl != null) &&
                !InPlaceControl.ContainsFocus)
            {
                _parentCtrl.ActiveControl = InPlaceControl;
                if (selectAllText)
                    SelectAllText();
            }
        }

        private bool _hideInPlaceControlInProgress = false;

        public void HideInPlaceControl()
        {
            if (_hideInPlaceControlInProgress == false)
            {
                _hideInPlaceControlInProgress = true;

                if (_currentInPlaceControl != null)
                {
                    if (_currentInPlaceControl.IsHandleCreated)
                    {
                        _currentInPlaceControl.Hide();

                        _currentInPlaceControl.GotFocus -= new EventHandler(OnChildGotFocus);

                        _parentCtrl.OnInPlaceCtrlHidden(new InPlaceCtrlVisibleEventArgs(_selectedPropertyEnum, _currentInPlaceControl));

                        if (_currentInPlaceControl.ContainsFocus)
                            Focus();

                        _currentInPlaceControl = null;
                        _mouseHook.HookMessages = false;
                        _keyboardHook.HookMessages = false;
//                        _messageHook.HookMessages = false;
                    }
                }

                _hideInPlaceControlInProgress = false;
            }
        }
        
        public void ShowInPlaceControl(PropertyEnumerator enumerator)
        {
            ShowInPlaceControl(enumerator, Point.Empty);
        }

        public void ShowInPlaceControl(PropertyEnumerator enumerator, Point mousePos)
        {
            if (enumerator.Property == null)
                return;

            if (!ContainsFocus)
                return;

            if (!Enabled && (DisableMode == PropertyGrid.DisableModes.NoValueSelection))
                return;

            if (enumerator.Property.Enabled && (Parent.ContainsFocus || Parent.Focused))
            {
                // We must hide the preceding inplace control if any
                HideInPlaceControl();

                Rectangle rect = GetItemRect(enumerator);
                Rectangle clientRect = ClientRectangle;
                if ((rect.Top < clientRect.Bottom) && (rect.Bottom > clientRect.Top))
                {
                    rect.X += LeftColumnWidth + 1 + LabelColumnWidth + 1;
                    rect.Width -= LeftColumnWidth + 1 + LabelColumnWidth + 1;

                    if ((enumerator.Property.Feel != PropertyFeel.Empty) && (enumerator.Property.Feel != null))
                    {
                        _keyboardHook.HookMessages = true;
                        _mouseHook.HookMessages = true;
                        _messageHook.HookMessages = true;
                        _currentInPlaceControl = enumerator.Property.Feel.ShowControl(rect, enumerator);

                        if (_currentInPlaceControl != null)
                        {
                            _currentInPlaceControl.GotFocus += new EventHandler(OnChildGotFocus);
                        }

                        if (!_scrollingInProgress && (InPlaceControl != null) &&
                            (_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.TabKey) &&
                            ((_parentCtrl.TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyWithAutoFocus) != 0))
                        {
                            rect = _currentInPlaceControl.Bounds;
                            Point pt = PointToScreen(mousePos);
                            pt = _currentInPlaceControl.PointToClient(pt);
                            if (rect.Contains(mousePos) && !TryOnForbidClickThrough(pt))
                                _parentCtrl.ClickThroughInProgress = true;
                            else
                                _parentCtrl.ActiveControl = InPlaceControl;

                            if (_parentCtrl.ClickThroughInProgress)
                                _parentCtrl.ClickThroughInProgress = false;
                            else
                                SelectAllText();
                        }
                    }
                }
            }
	        else
		        HideInPlaceControl();
        }

        public void FocusTextBox()
        {
            if (_currentInPlaceControl != null)
            {
                if ((_currentInPlaceControl as IInPlaceControl).TextBox != null)
                    (_currentInPlaceControl as IInPlaceControl).TextBox.Focus();
                else if (_currentInPlaceControl is TextBoxBase)
                    _currentInPlaceControl.Focus();
            }
        }

        public void SelectAllText()
        {
            if (_currentInPlaceControl != null)
            {
                if ((_currentInPlaceControl as IInPlaceControl).TextBox != null)
                    (_currentInPlaceControl as IInPlaceControl).TextBox.SelectAll();
                else if (_currentInPlaceControl is TextBoxBase)
                    (_currentInPlaceControl as TextBoxBase).SelectAll();
            }
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if (keyData == Keys.Down)
                return true;

            if (keyData == Keys.Up)
                return true;

            if (keyData == Keys.Right)
                return true;

            if (keyData == Keys.Left)
                return true;

            return base.IsInputKey(keyData);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            // If only Control is clicked we simulate a mouse move so that the cursor could be updated on
            // a hyperlink property.
            if (e.KeyCode == Keys.ControlKey)
            {
                if (_currentInPlaceControl != null)
                {
                    Win32Calls.RECT rect = new Win32Calls.RECT();
                    Win32Calls.GetWindowRect(_currentInPlaceControl.Handle, ref rect);
                    Rectangle bounds = Rectangle.FromLTRB(rect.Left, rect.Top, rect.Right, rect.Bottom);

                    if (bounds.Contains(MousePosition))
                    {
                        base.OnKeyDown(e);
                        return;
                    }
                }

                Rectangle itemRect;
                Point pt = PointToClient(MousePosition);
                PropertyEnumerator propEnum = PropertyItemFromPoint(pt, out itemRect);

                if (propEnum.Property != null)
                    propEnum.Property.OverrideCursor(pt, itemRect, propEnum);
                
                base.OnKeyDown(e);
                return;
            }

            Property selectedProperty = _selectedPropertyEnum.Property;

            if (selectedProperty != null)
            {
                // Hide the tooltip if it is visible
//                if (ToolTip != null)
  //                  ToolTip.SetText("", null);

                if (_currentInPlaceControl != null)
                {
                    IInPlaceControl c = (_currentInPlaceControl as IInPlaceControl);
                    if ((c != null) && c.OnParentKeyDown(e))
                        return;
                }

                switch (e.KeyCode)
                {
                    case Keys.F4:
                    {
                        if ((_currentInPlaceControl != null) && (e.Modifiers == 0))
                        {
                            IInPlaceControl c = (_currentInPlaceControl as IInPlaceControl);
                            if (c != null)
                            {
                                c.OnF4();
                            }
                        }
                        break;
                    }
                    case Keys.Space:
                    case Keys.Return:
                    {
                        if ((SelectedPropertyEnumerator.Property.CanBeDisabledManually) && (e.KeyCode == Keys.Space))
                        {
                            if (SelectedPropertyEnumerator.HasParent == false)
                            {
                                EnableProperty(SelectedPropertyEnumerator, !SelectedPropertyEnumerator.Property.Enabled);
#if _DOTNET2
                                e.SuppressKeyPress = true;
#else
                                HandleRef handle = new HandleRef(this, this.Handle);
                                Win32Calls.RemovePendingMessages(handle, 0x102, 0x102);
                                Win32Calls.RemovePendingMessages(handle, 0x106, 0x106);
                                Win32Calls.RemovePendingMessages(handle, 0x286, 0x286);
                                e.Handled = true;
#endif
                            }
                            else
                            {
                                // The user can press on the checkbox only if its parent category is enabled
                                if (SelectedPropertyEnumerator.Parent.Property.Enabled)
                                {
                                    EnableProperty(SelectedPropertyEnumerator, !SelectedPropertyEnumerator.Property.Enabled);
#if _DOTNET2
                                    e.SuppressKeyPress = true;
#else
                                    HandleRef handle = new HandleRef(this, this.Handle);
                                    Win32Calls.RemovePendingMessages(handle, 0x102, 0x102);
                                    Win32Calls.RemovePendingMessages(handle, 0x106, 0x106);
                                    Win32Calls.RemovePendingMessages(handle, 0x286, 0x286);
                                    e.Handled = true;
#endif
                                }
                            }
                        }
                        // TODO : the following code should not be in this class.
                        else if (SelectedPropertyEnumerator.Property is PropertyHyperLink)
                        {
                            if (SelectedPropertyEnumerator.Property.Enabled)
                            {
                                PropertyHyperLinkClickedEventArgs ev = new PropertyHyperLinkClickedEventArgs(SelectedPropertyEnumerator, SelectedPropertyEnumerator.Property.HyperLink);
                                _parentCtrl.OnHyperLinkPropertyClicked(ev);
                            }
                        }
                        else if (HasOneVisibleChild(SelectedPropertyEnumerator))
                        {
                            Property property = SelectedPropertyEnumerator.Property;
                            ExpandProperty(SelectedPropertyEnumerator, !property.Expanded);
                        }

                        break;
                    }
                    case Keys.Right:		// Opens the item if it is a closed category
                    case Keys.Add:
                    {
                        if ((e.KeyCode == Keys.Right) && (ModifierKeys == Keys.Control))
                        {
                            SetLabelColumnWidthRatio((double)(LabelColumnWidth + 3) / (ClientRectangle.Width - LeftColumnWidth));
                            break;
                        }

                        if (SelectedPropertyEnumerator.Children.Count > 0)
                        {
                            Property property = SelectedPropertyEnumerator.Property;
                            if (!property.Expanded && HasOneVisibleChild(SelectedPropertyEnumerator))
                            {
                                ExpandProperty(SelectedPropertyEnumerator, true);
                                break;
                            }
                        }

                        if (e.KeyCode == Keys.Add)
                            break;

                        goto case Keys.Down;
                    }
                    case Keys.Down:		// Moves the selected item down
                    {
                        if (e.Modifiers == Keys.Alt)
                        {
                            if (_currentInPlaceControl != null)
                            {
                                IInPlaceControl c = (_currentInPlaceControl as IInPlaceControl);
                                if ((c != null) && !SelectedPropertyEnumerator.Property.Value.IsReadOnly(SelectedPropertyEnumerator))
                                {
                                    c.OnF4();
                                    break;
                                }
                            }
                        }

                        PropertyVisibleDeepEnumerator nextEnum = new PropertyVisibleDeepEnumerator(_selectedPropertyEnum.Node);
                        nextEnum.MoveNext();

                        Property nextProperty = nextEnum.Property;
                        if (nextProperty != null)
                        {
                            Rectangle clientRect = ClientRectangle;
                            Rectangle itemRect = GetAbsoluteItemRect(nextEnum);

                            if (itemRect.Top < clientRect.Top)	// Test if the item is above the top of the grid
                            {
                                while (true)
                                {
                                    Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEUP, 0), 0);
                                    itemRect = GetAbsoluteItemRect(nextEnum);
                                    if (itemRect.Top == clientRect.Top)
                                        break;
                                }
                                Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_ENDSCROLL, 0), 0);
                            }
                            else if (itemRect.Bottom > clientRect.Bottom)		// Test if the item is below the bottom of the grid
                            {
                                // We scroll in a loop because the item may have a row height greater than the base row height
                                // if the current selected item is the last one displayed
                                // (TODO: we could be more performant by avoiding scrolls and directly calculating which property
                                // needs to be the first displayed)
                                _parentCtrl.BeginUpdate();  // To avoid the remancence of the inplace control
                                do
                                {
                                    Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEDOWN, 0), 0);
                                    itemRect = GetAbsoluteItemRect(nextEnum);
                                }
                                while ((itemRect.Bottom > clientRect.Bottom) && (itemRect.Top > clientRect.Top));
                                Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_ENDSCROLL, 0), 0);
                                _parentCtrl.EndUpdate();
                            }

                            SelectProperty(nextEnum);
                        }

                        break;
                    }
                    case Keys.Left:		// Closes the item if it is an expanded category
                    case Keys.Subtract:
                    {
                        if ((e.KeyCode == Keys.Left) && (ModifierKeys == Keys.Control))
                        {
                            SetLabelColumnWidthRatio((double)(LabelColumnWidth - 3) / (ClientRectangle.Width - LeftColumnWidth));
                            break;
                        }

                        if (SelectedPropertyEnumerator.Children.Count > 0)
                        {
                            Property property = SelectedPropertyEnumerator.Property;
                            if (property.Expanded && HasOneVisibleChild(SelectedPropertyEnumerator))
                            {
                                ExpandProperty(SelectedPropertyEnumerator, false);
                                break;
                            }
                        }

                        if (e.KeyCode == Keys.Subtract)
                            break;

                        goto case Keys.Up;
                    }
                    case Keys.Up:		// Moves the selected item up
                    {
                        PropertyVisibleDeepEnumerator prevEnum = new PropertyVisibleDeepEnumerator(_selectedPropertyEnum.Node);
                        prevEnum.MovePrev();

                        Property prevProperty = prevEnum.Property;
                        if (prevProperty != null)
                        {
                            Rectangle clientRect = ClientRectangle;
                            Rectangle itemRect = GetAbsoluteItemRect(prevEnum);

                            if (itemRect.Top < clientRect.Top)
                            {
                                _firstDisplayedLine = 1;
                                FirstDisplayedProperty = prevEnum;
                            }
                            else if (itemRect.Bottom > clientRect.Bottom)
                            {
                                while(true)
                                {
                                    Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEDOWN, 0), 0);
                                    itemRect = GetAbsoluteItemRect(prevEnum);
                                    if (itemRect.Bottom < clientRect.Bottom)
                                        break;
                                }
                                Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_ENDSCROLL, 0), 0);
                            }

                            SelectProperty(prevEnum);
                        }
                        break;
                    }
                    case Keys.Next:
                    {
                        Rectangle clientRect = ClientRectangle;
                        int itemsPerPage = clientRect.Height / _basicPropertyHeight - 1;
                        Rectangle itemRect = GetAbsoluteItemRect(SelectedPropertyEnumerator);

                        // Find the item one page down
                        int count = 0;
                        PropertyEnumerator propEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();
                        int lineCount = (propEnum == FirstDisplayedProperty ? _firstDisplayedLine : 1);
                        while ((count < itemsPerPage) && (propEnum != propEnum.RightBound))
                        {
                            if (lineCount == propEnum.Property.HeightMultiplier)
                            {
                                propEnum.MoveNext();
                                lineCount = 1;
                            }
                            else
                                lineCount++;

                            count++;
                        }

                        // Selected item is above the grid ?
                        if (itemRect.Bottom < clientRect.Top)
                        {
                            if (propEnum == propEnum.RightBound)
                            {
                                propEnum.MovePrev();
                                _firstDisplayedLine = propEnum.Property.HeightMultiplier;
                            }
                            else
                                _firstDisplayedLine = lineCount;

                            SelectProperty(propEnum);
                            EnsureVisible(propEnum);
                        }
                        // Selected item is in or below the grid ?
                        else
                        {
                            if (propEnum == propEnum.RightBound)	// Not a complete page down because at the far bottom ?
                            {
                                propEnum.MovePrev();
                                lineCount = propEnum.Property.HeightMultiplier;
                                PropertyVisibleDeepEnumerator propertyToSelectEnum = propEnum.GetVisibleDeepEnumerator();

                                count = itemsPerPage;
                                while ((propEnum != _activePropertyCollection.GetVisibleDeepEnumerator().MoveFirst()) && (count > 0))
                                {
                                    if (lineCount > 1)
                                        lineCount--;
                                    else
                                    {
                                        propEnum.MovePrev();
                                        lineCount = propEnum.Property.HeightMultiplier;
                                    }

                                    count--;
                                }

                                _firstDisplayedLine = lineCount;
                                FirstDisplayedProperty = propEnum as PropertyVisibleDeepEnumerator;
                                SelectProperty(propertyToSelectEnum);
                            }
                            else
                            {
                                // The grid height is to small to properly page down ?
                                if (itemsPerPage < propEnum.Property.HeightMultiplier)
                                {
                                    // Advance one item
                                    if (propEnum != _activePropertyCollection.GetVisibleDeepEnumerator().MoveLast())
                                    {
                                        if (propEnum == SelectedPropertyEnumerator)
                                            propEnum.MoveNext();

                                        _firstDisplayedLine = 1;
                                        FirstDisplayedProperty = propEnum as PropertyVisibleDeepEnumerator;
                                        SelectProperty(propEnum);
                                    }
                                }
                                else
                                {
                                    FirstDisplayedProperty = SelectedPropertyEnumerator;
                                    SelectProperty(propEnum);
                                }
                            }
                        }

                        Invalidate();

                        break;
                    }
                    case Keys.Prior:
                    {
                        Rectangle clientRect = ClientRectangle;
                        int itemsPerPage = clientRect.Height / _basicPropertyHeight - 1;

                        // Find the item one page up
                        int count = 0;
                        PropertyVisibleDeepEnumerator enumerator = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();
                        int lineCount = enumerator.Property.HeightMultiplier;
                        while ((count < itemsPerPage) && (enumerator != _activePropertyCollection.GetVisibleDeepEnumerator().MoveFirst()))
                        {
                            if (lineCount == 1)
                            {
                                enumerator.MovePrev();
                                lineCount = enumerator.Property.HeightMultiplier;
                            }
                            else
                                lineCount--;

                            count++;
                        }

                        if ((count == 0) && (enumerator != _activePropertyCollection.GetVisibleDeepEnumerator().MoveFirst()))
                        {
                            enumerator.MovePrev();
                            count = 1;
                        }

                        Rectangle itemRect = GetAbsoluteItemRect(enumerator);

                        // Is the new selected item in the grid ?
                        if ((itemRect.Top >= clientRect.Top) && (itemRect.Bottom < clientRect.Bottom))
                            SelectProperty(enumerator);

                        // Is the new selected item below the grid ?
                        else if (itemRect.Bottom > clientRect.Bottom)
                        {
                            PropertyEnumerator currentEnum = enumerator.GetVisibleDeepEnumerator();
                            lineCount = 1;
                            while((count > 0) && (currentEnum != _activePropertyCollection.GetVisibleDeepEnumerator().MoveFirst()))
                            {
                                if (lineCount == 1)
                                {
                                    currentEnum.MovePrev();
                                    lineCount = currentEnum.Property.HeightMultiplier;
                                }
                                else
                                    lineCount--;

                                count--;
                            }

                            FirstDisplayedProperty = currentEnum as PropertyVisibleDeepEnumerator;
                            SelectProperty(enumerator);
                        }

                        // Is the new selected item above the grid ?
                        else if (itemRect.Top < clientRect.Top)
                        {
                            _firstDisplayedLine = 1;
                            FirstDisplayedProperty = enumerator;
                            SelectProperty(FirstDisplayedProperty);
                        }

                        Refresh();

                        break;
                    }
                    case Keys.Home:
                    {
                        Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, 0, true);
                        _firstDisplayedPropertyEnumerator = _activePropertyCollection.GetVisibleDeepEnumerator();
                        _firstDisplayedPropertyEnumerator.MoveFirst();
                        _firstDisplayedLine = 1;
                        SelectProperty(FirstDisplayedProperty);

                        break;
                    }
                    case Keys.End:
                    {
                        Rectangle clientRect = ClientRectangle;

                        _firstDisplayedPropertyEnumerator = _activePropertyCollection.GetVisibleDeepEnumerator();
                        _firstDisplayedPropertyEnumerator.MoveLast();

                        _firstDisplayedLine = 1;
                        PropertyVisibleDeepEnumerator currentEnum = _firstDisplayedPropertyEnumerator.GetVisibleDeepEnumerator();
                        Rectangle itemRect = GetItemRect(currentEnum);
                        while ((_firstDisplayedPropertyEnumerator != _activePropertyCollection.GetVisibleDeepEnumerator().MoveFirst()) && (clientRect.Bottom - itemRect.Bottom > _basicPropertyHeight))
                        {
                            if (_firstDisplayedLine > 1)
                                _firstDisplayedLine--;
                            else
                            {
                                _firstDisplayedPropertyEnumerator.MovePrev();
                                _firstDisplayedLine = _firstDisplayedPropertyEnumerator.Property.HeightMultiplier;
                            }

                            itemRect = GetItemRect(currentEnum);
                        }

                        Win32Calls.SCROLLINFO si = new Win32Calls.SCROLLINFO();
                        si.cbSize = Marshal.SizeOf(si);
                        si.fMask = Win32Calls.SIF_PAGE | Win32Calls.SIF_RANGE;
                        Win32Calls.GetScrollInfo(Handle, Win32Calls.SB_VERT, ref si);
                        Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, si.nMax - (si.nPage - 1), true);

                        PropertyVisibleDeepEnumerator lastEnum = _activePropertyCollection.GetVisibleDeepEnumerator();
                        lastEnum.MoveLast();
                        SelectProperty(lastEnum);

                        break;
                    }
			        case Keys.A:
			        {
				        if (e.Control)
				        {
					        if (_multiSelectMode == PropertyGrid.MultiSelectModes.SameLevel)
					        {
						        // Find the parent of the concerned level
						        PropertyEnumerator ancestorEnum;
                                if (SelectedPropertyEnumerator.Children.Count == 0)
                                    ancestorEnum = SelectedPropertyEnumerator.Parent;
                                else
                                    ancestorEnum = SelectedPropertyEnumerator;

                                // And browse all properties at this level and under
                                PropertyEnumerator propEnum = ancestorEnum.Children.GetVisibleDeepEnumerator();
                                propEnum.RestrictedToThisLevelAndUnder = true;
                                while(propEnum != ancestorEnum.Children.RightBound)
						        {
                                    if (_parentCtrl.OnValidateMultiSelection(SelectedPropertyEnumerator, propEnum as PropertyVisibleDeepEnumerator))
							        {
                                        propEnum.Property.SelectedInternal = true;
								        _multiSelectedItems.Add(propEnum);
							        }

                                    propEnum.MoveNext();
                                }

						        Invalidate();
					        }
					        else if (_multiSelectMode == PropertyGrid.MultiSelectModes.Global)
					        {
                                PropertyEnumerator propEnum = _activePropertyCollection.GetVisibleDeepEnumerator();
                                propEnum.MoveFirst();
                                while (propEnum != propEnum.RightBound)
                                {
                                    if ((propEnum != SelectedPropertyEnumerator) && (propEnum.Children.Count == 0))
                                    {
                                        if (_parentCtrl.OnValidateMultiSelection(SelectedPropertyEnumerator, propEnum as PropertyVisibleDeepEnumerator))
                                        {
                                            propEnum.Property.SelectedInternal = true;
                                            _multiSelectedItems.Add(propEnum);
                                        }
                                    }

                                    propEnum.MoveNext();
                                }

						        Invalidate();
					        }
				        }

				        break;
			        }
                }
            }

            base.OnKeyDown(e);
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            // If the Control key is released the cursor is restored to default in case we were on a hyperlinked property
            // a hyperlink property.
            if (e.KeyCode == Keys.ControlKey)
            {
                if (_currentInPlaceControl != null)
                {
                    Win32Calls.RECT rect = new Win32Calls.RECT();
                    Win32Calls.GetWindowRect(_currentInPlaceControl.Handle, ref rect);
                    Rectangle bounds = Rectangle.FromLTRB(rect.Left, rect.Top, rect.Right, rect.Bottom);

                    if (bounds.Contains(MousePosition))
                    {
                        base.OnKeyUp(e);
                        return;
                    }
                }

                Rectangle itemRect;
                Point pt = PointToClient(MousePosition);
                PropertyEnumerator propEnum = PropertyItemFromPoint(pt, out itemRect);

                if (!(propEnum.Property is PropertyHyperLink))
                    Cursor = Cursors.Arrow;
            }

            base.OnKeyUp(e);
        }

        protected override bool ProcessDialogChar(char charCode)
        {
            if ((charCode == '+') || (charCode == '-'))
            {
                if (SelectedPropertyEnumerator.Children.Count > 0)
                {
                    if (HasOneVisibleChild(SelectedPropertyEnumerator))
                        return true;
                }
            }

            return base.ProcessDialogChar(charCode);
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if (_currentInPlaceControl != null)
            {
                _parentCtrl.ActiveControl = _currentInPlaceControl;
                SelectAllText();
                Win32Calls.PostMessage(_currentInPlaceControl.Handle, Win32Calls.WM_CHAR, e.KeyChar, 0);
            }
            
            base.OnKeyPress(e);
        }

        protected override void OnGotFocus(EventArgs e)
        {
            base.OnGotFocus(e);

            // The selected property must be repainted
            Invalidate();
//            Refresh();

            // Sometimes, when we hide the inplace control, the properties control
            // regains the focus and tries to show the inplace control again.
            if (_dontShowInPlaceCtrlInGotFocus)
            {
                _dontShowInPlaceCtrlInGotFocus = false;
                return;
            }

            Property property = SelectedPropertyEnumerator.Property;
            if ((property != null) && (property.InPlaceCtrlInAction == null))
            {
                ShowInPlaceControl(SelectedPropertyEnumerator);
//                SelectAllText();
            }
        }

        protected override void OnLostFocus(EventArgs e)
        {
            base.OnLostFocus(e);

            // The inplace control must be hidden when another application gains the focus (becomes the foreground window)
            if ((_currentInPlaceControl != null) && (_currentInPlaceControl.ContainsFocus == false))
                HideInPlaceControl();

            Invalidate();
        }

        public void ExpandProperty(PropertyEnumerator propEnum, bool expand)
        {
            if (propEnum == propEnum.RightBound)
                return;

            Property property = propEnum.Property;

            // Do nothing if the expand state won't change
            if (expand == property.Expanded)
                return;

            InvalidateVisibleItemCount();

            // Hide the inplace control if it is a child of the closed category
            if (property.Expanded && !expand)
            {
                if (SelectedPropertyEnumerator.IsDescendantOf(propEnum))
                    SelectProperty(propEnum);

                if (FirstDisplayedProperty.IsDescendantOf(propEnum))
                    FirstDisplayedProperty = propEnum.GetVisibleDeepEnumerator();
            }

            property.ExpandedInternal = expand;

            _parentCtrl.OnPropertyExpanded(new PropertyExpandedEventArgs(propEnum, expand));

            if (IsHandleCreated)
            {
                // We call OnSizeChanged here because this method checks to see if there
                // is some space left at the bottom of the grid and scrolls it accordingly.
                // It also calls CheckScrollbar()
                Rectangle rect = ClientRectangle;
                OnSizeChanged(rect.Width, rect.Height);

                Invalidate();
            }
        }

        public void ExpandAllProperties(bool expand)
        {
            PropertyEnumerator propEnum = FirstProperty;
            while (propEnum != RightBound)
            {
                ExpandProperty(propEnum, expand);
                propEnum.MoveNext();
            }
        }

        private void InvalidateVisibleItemCount()
        {
            _visibleItemCountValid = false;
        }

        private int GetVisibleItemCount()
        {
	        if (_visibleItemCountValid)
		        return _visibleItemCount;
	        else
	        {
		        _visibleItemCountValid = true;
		        _visibleItemCount = 0;
		        _visibleLineCount = 0;

                PropertyVisibleDeepEnumerator enumerator = _activePropertyCollection.GetVisibleDeepEnumerator();
                enumerator.MoveFirst();
                while(enumerator != enumerator.RightBound)
                {
                    _visibleLineCount += enumerator.Property.HeightMultiplier;
                    _visibleItemCount++;

                    enumerator.MoveNext();
                }

		        return _visibleItemCount;
	        }
        }

        private int GetVisibleLineCount()
        {
	        if (_visibleItemCountValid == false)
		        GetVisibleItemCount();

	        return _visibleLineCount;
        }

        private bool TryOnForbidClickThrough(Point pt)
        {
            MethodInfo methodInfo = _currentInPlaceControl.GetType().GetMethod("OnForbidClickThrough");
            if (methodInfo != null)
            {
                return (bool)methodInfo.Invoke(_currentInPlaceControl, new object[] { pt });
            }

            return false;
        }

        internal void CloseEmptySpace()
        {
            // Test for empty space at the bottom and scroll up accordingly
            PropertyVisibleDeepEnumerator lastVisiblePropertyEnum = _activePropertyCollection.GetVisibleDeepEnumerator();
            lastVisiblePropertyEnum.MoveLast();
            if (lastVisiblePropertyEnum.Property != null)
            {
                Rectangle clientRect = ClientRectangle;

                Rectangle itemRect = GetAbsoluteItemRect(lastVisiblePropertyEnum);
                if (!IsScrollbarVisible())
                {
                    while ((clientRect.Bottom - itemRect.Bottom >= _basicPropertyHeight) &&
                        ((FirstDisplayedProperty.MovePrev() != LeftBound) || (_firstDisplayedLine > 1)))
                    {
                        if (_firstDisplayedLine > 1)
                            _firstDisplayedLine--;
                        else
                        {
                            _firstDisplayedPropertyEnumerator.MovePrev();
                            _firstDisplayedLine = FirstDisplayedProperty.Property.HeightMultiplier;
                        }

                        CheckScrollbar();

                        itemRect = GetAbsoluteItemRect(lastVisiblePropertyEnum);
                    }
                }
                else
                {
                    bool scrollDone = false;
                    while ((clientRect.Bottom - itemRect.Bottom >= _basicPropertyHeight) && (Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) > 0))
                    {
                        Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEUP, 0), 0);
                        itemRect = GetAbsoluteItemRect(lastVisiblePropertyEnum);
                        scrollDone = true;
                    }
                    if (scrollDone)
                        Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_ENDSCROLL, 0), 0);
                }
            }
        }

        private int _scrollPosBeforeVerticalFreeze = -1;

        internal void BeginVerticalFreeze()
        {
            if (_beginVerticalFreezeCounter == 0)
                _scrollPosBeforeVerticalFreeze = Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT);

            _beginVerticalFreezeCounter++;
        }

        internal void EndVerticalFreeze()
        {
            // TODO : quoi si la selected property a ete supprimee ?

            _beginVerticalFreezeCounter--;

            if (_beginVerticalFreezeCounter == 0)
            {
                Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL,
                    Win32Calls.MakeLong(Win32Calls.SB_THUMBPOSITION, _scrollPosBeforeVerticalFreeze), 0);

                CloseEmptySpace();
                CheckScrollbar();
            }
        }

        internal bool IsVerticallyFrozen
        {
            get { return (_beginVerticalFreezeCounter > 0); }
        }

        private void OnSizeChanged(int width, int height)
        {
            if (IsVerticallyFrozen)
                return;

            // Test for empty space at the bottom and scroll up accordingly
            CloseEmptySpace();

            CheckScrollbar();
        }

        internal void HeightMultiplierChanged()
        {
            InvalidateVisibleItemCount();
            CheckScrollbar();
        }

		protected override void OnSizeChanged(EventArgs e)
		{
			base.OnSizeChanged(e);

			if (Size.Height == 0)
				return;

            _rootGlyphsRect = new Rectangle(0, 0, LeftColumnWidth, Size.Height);

			// Check scrollbars and avoid empty space at the bottom of the list
			OnSizeChanged(Size.Width, Size.Height);

			// Redraw the control
			//	        Invalidate(false);
		}

        internal void CheckScrollbar()
        {
            if (IsVerticallyFrozen)
                return;

            if (_displayModeChangeInProgress)
                return;

            if (IsHandleCreated == false)
                return;

            Rectangle clientRect = ClientRectangle;
            if (clientRect.Height == 0)
                return;

            Win32Calls.SCROLLINFO si = new Win32Calls.SCROLLINFO();
            si.cbSize = Marshal.SizeOf(si);

            // Determine if the number of properties to display can fit in the grid.
            int lineCount = GetVisibleLineCount();
            if (lineCount * _basicPropertyHeight > clientRect.Height)
            {
                // If it can't, the scrollbar must be visible and its parameters checked

                if (!IsScrollbarVisible())
                {
                    Win32Calls.ShowScrollBar(Handle, Win32Calls.SB_VERT, true);

                    // Redraw only the value column
                    Invalidate(new Rectangle(LeftColumnWidth + LabelColumnWidth, 0, clientRect.Width - (LeftColumnWidth + LabelColumnWidth), clientRect.Height), false);
                    Invalidate(false);

                    // Invalidate the inplace control
                    if (_currentInPlaceControl != null)
                        _currentInPlaceControl.Refresh();
                }

                // Set the scrollbar parameters
                si.nMin = 0;
                si.nMax = lineCount - 1;
                si.nPage = clientRect.Height / _basicPropertyHeight;
                si.fMask = Win32Calls.SIF_PAGE | Win32Calls.SIF_RANGE;
                Win32Calls.SetScrollInfo(Handle, Win32Calls.SB_VERT, ref si, true);

                // Enable both ends of the scrollbar
                Win32Calls.EnableScrollBar(Handle, Win32Calls.SB_VERT, Win32Calls.ESB_ENABLE_BOTH);

                // Disable the scrollbar arrow if it is at a limit
                Win32Calls.GetScrollInfo(Handle, Win32Calls.SB_VERT, ref si);
                if (Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) == (si.nMax - (si.nPage - 1)))
                    Win32Calls.EnableScrollBar(Handle, Win32Calls.SB_VERT, Win32Calls.ESB_DISABLE_DOWN);
                else if (Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT) == 0)
                    Win32Calls.EnableScrollBar(Handle, Win32Calls.SB_VERT, Win32Calls.ESB_DISABLE_UP);

                // Control the position of the scroll
                PropertyVisibleDeepEnumerator enumerator = _activePropertyCollection.GetVisibleDeepEnumerator();
                enumerator.MoveFirst();
                int pos = 0;
                while (enumerator != _firstDisplayedPropertyEnumerator)
                {
                    pos += enumerator.Property.HeightMultiplier;
                    enumerator.MoveNext();
                }
                if (_firstDisplayedPropertyEnumerator != _firstDisplayedPropertyEnumerator.RightBound)
                    pos += _firstDisplayedLine - 1;
                if (pos != Win32Calls.GetScrollPos(Handle, Win32Calls.SB_VERT))
                    Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, pos, true);
            }
            else
            {
                // If the properties can all fit in the grid, the scrollbar must be hidden

                if (IsScrollbarVisible())
                {
                    Win32Calls.ShowScrollBar(Handle, Win32Calls.SB_VERT, false);
                    Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, 0, true);

                    // Set the scrollbar parameters to 0 so that we can test these values to
                    // 0 elsewhere in the code (meaning no scroll is possible)
                    si.nMin = 0;
                    si.nMax = 0;
                    si.nPage = 0;
                    si.fMask = Win32Calls.SIF_PAGE | Win32Calls.SIF_RANGE;
                    Win32Calls.SetScrollInfo(Handle, Win32Calls.SB_VERT, ref si, true);

                    // Redraw only the value column
                    Invalidate(new Rectangle(LeftColumnWidth + LabelColumnWidth, 0, clientRect.Width - (LeftColumnWidth + LabelColumnWidth), clientRect.Height), false);
                    Invalidate(false);

                    // Invalidate the inplace control
                    if (_currentInPlaceControl != null)
                        _currentInPlaceControl.Refresh();
                }
            }

            // Resize current inplace control

            Property property = SelectedPropertyEnumerator.Property;
            if ((property != null) && property.Enabled)
            {
                Rectangle valueRect = GetItemRect(SelectedPropertyEnumerator);
                valueRect = property.GetValueRect(valueRect);

                PropertyFeel feel = property.Feel;
                if ((feel != null) && (feel != PropertyFeel.Empty))
                {
                    if ((valueRect.Bottom <= clientRect.Top) || (valueRect.Top >= clientRect.Bottom))
                        HideInPlaceControl();
                    else
                    {
                        if (_currentInPlaceControl == null)
                        {
                            // When we scroll and the selected property comes back into view, we need to restore the inplace control
                            ShowInPlaceControl(SelectedPropertyEnumerator);
                        }
                        else
                        {
                            feel.MoveControl(valueRect, SelectedPropertyEnumerator);
                            _currentInPlaceControl.Refresh();
                        }
                    }
                }
            }
        }

        private void ResetScrollPosToFirstDisplayedProperty()
        {
            if (_displayModeChangeInProgress)
                return;

            if (IsScrollbarVisible())
            {
                PropertyEnumerator propEnum = _activePropertyCollection.GetVisibleDeepEnumerator().MoveFirst();
                int delta = 0;
                int lineNumber = 1;
                while ((propEnum != FirstDisplayedProperty) || (lineNumber != _firstDisplayedLine))
                {
                    if (lineNumber == propEnum.Property.HeightMultiplier)
                    {
                        propEnum.MoveNext();
                        lineNumber = 1;
                    }
                    else
                        lineNumber++;

                    delta++;
                }

                Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, delta, true);
            }
        }

        public bool CheckPropertyVisibility(PropertyEnumerator propEnum)
        {
            PropertyEnumerator currentEnum = new PropertyDeepEnumerator(propEnum.Node);
            do
            {
                if (currentEnum.Property.Visible == false)
                    return false;

                if (currentEnum.HasParent)
                {
                    currentEnum = currentEnum.Parent;

                    if (currentEnum.Property.Expanded == false)
                        return false;
                }
            }
            while (currentEnum.HasParent);

            return true;
        }

        public bool EnsureVisible(PropertyEnumerator propEnum)
        {
	        if (propEnum == propEnum.RightBound)
		        return false;

	        if (_displayMode == PropertyGrid.DisplayModes.Categorized)
	        {
		        // Ensure that all ancestors are expanded and visible
                PropertyEnumerator parentEnum = propEnum;
                while(parentEnum.HasParent)
                {
                    parentEnum = parentEnum.Parent;
                    if (parentEnum.Property.Expanded == false)
                    {
                        ShowProperty(parentEnum, true);
                        ExpandProperty(parentEnum, true);
                    }
                }
	        }

	        Rectangle itemRect = GetAbsoluteItemRect(propEnum);
	        Rectangle clientRect = ClientRectangle;

	        // Make the item the first visible one at the top if it is above the control
	        if (itemRect.Top < clientRect.Top)
	        {
                _firstDisplayedLine = 1;
		        FirstDisplayedProperty = propEnum.GetVisibleDeepEnumerator();
	        }
	        // Make the item the last visible one at the bottom if it is below the control
	        else
	        {
		        while(itemRect.Bottom > clientRect.Bottom)
		        {
                    Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEDOWN, 0), 0);
			        itemRect = GetAbsoluteItemRect(propEnum);
		        }
                Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_ENDSCROLL, 0), 0);
            }

	        // We call OnSizeChanged here because this method checks to see if there
	        // is some space left at the bottom of the grid and scrolls it accordingly.
	        // It also calls CheckScrollbar()
	        OnSizeChanged(clientRect.Width, clientRect.Height);
            Refresh();

	        return true;
        }

        public void ShowProperty(PropertyEnumerator propEnum, bool show)
        {
            if (propEnum == propEnum.RightBound)
                return;

            if (propEnum.Property.Visible == show)
                return;

            propEnum.Property.VisibleInternal = show;

            InvalidateVisibleItemCount();

            if (!show)	// The item must be hidden
            {
                if (SelectedPropertyEnumerator == propEnum)
                {
                    PropertyEnumerator nextEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();
                    PropertyEnumerator prevEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator().MovePrev();
                    if (nextEnum != nextEnum.RightBound)
                        SelectProperty(nextEnum);
                    else if (prevEnum != prevEnum.LeftBound)
                        SelectProperty(prevEnum);
                    else
                        SelectProperty(nextEnum);
                }

                if (_firstDisplayedPropertyEnumerator == propEnum)
                {
                    PropertyEnumerator nextEnum = _firstDisplayedPropertyEnumerator.GetVisibleDeepEnumerator();
                    PropertyEnumerator prevEnum = _firstDisplayedPropertyEnumerator.GetVisibleDeepEnumerator().MovePrev();
                    if (nextEnum != nextEnum.RightBound)
                        _firstDisplayedPropertyEnumerator = nextEnum as PropertyVisibleDeepEnumerator;
                    else if (prevEnum != prevEnum.LeftBound)
                        _firstDisplayedPropertyEnumerator = prevEnum as PropertyVisibleDeepEnumerator;
                    else
                        _firstDisplayedPropertyEnumerator = nextEnum as PropertyVisibleDeepEnumerator;
                }
            }

            if (!_lockRefresh)
            {
                if (IsHandleCreated)
                {
                    // We call OnSizeChanged here because this method checks to see if there
                    // is some space left at the bottom of the grid and scrolls it accordingly.
                    // It also calls CheckScrollbar()
                    Rectangle rect = ClientRectangle;
                    OnSizeChanged(rect.Width, rect.Height);
                    Refresh();
                }
            }
        }

        ToolTipCtrl ToolTip
        {
            get
            {
                if (ToolTipMode == PropertyGrid.ToolTipModes.None)
                    return null;

                if (_tooltipCtrl == null)
                    _tooltipCtrl = new ToolTipCtrl(this);

                return _tooltipCtrl;
            }
        }

        public void Clear()
        {
	        InvalidateVisibleItemCount();

	        HideInPlaceControl();

            _categorizedPropertyCollection.Clear();
            _flatPropertyCollection.Clear();

            _firstDisplayedLine = 1;
            _firstDisplayedPropertyEnumerator = _activePropertyCollection.GetVisibleDeepEnumerator().RightBound.GetVisibleDeepEnumerator();
            _selectedPropertyEnum = _activePropertyCollection.GetVisibleDeepEnumerator().RightBound.GetVisibleDeepEnumerator();

            Invalidate();

	        if (IsScrollbarVisible())
	        {
                Win32Calls.SetScrollPos(Handle, Win32Calls.SB_VERT, 0, true);
                CheckScrollbar();
	        }
        }

        public void ClearUnderProperty(PropertyEnumerator propEnum)
        {
            PropertyEnumerator childEnum = propEnum.Children;
            while (childEnum != childEnum.RightBound)
            {
                PropertyEnumerator nextChildEnum = ((PropertyEnumerator)childEnum.Clone()).MoveNext();
                DeleteProperty(childEnum);
                childEnum = nextChildEnum;
            }
        }

        public PropertyEnumerator FindProperty(int propertyId)
        {
	        return _categorizedPropertyCollection.Find(propertyId);
        }

        public PropertyEnumerator FindPropertyAfter(PropertyEnumerator afterEnumerator, int propertyId)
        {
            return _categorizedPropertyCollection.FindAfter(afterEnumerator, propertyId);
        }

        private int MeasureWidestLabel()
        {
	        int maxWidth = 0;

            PropertyEnumerator propEnum = _activePropertyCollection.GetVisibleDeepEnumerator().MoveFirst();
            while(propEnum != propEnum.RightBound)
	        {
                Property property = propEnum.Property;

		        if (((property is RootCategory == false) || _parentCtrl.DrawManager.ShowCategoryVerticalLine) &&
                    (property is PropertyHyperLink == false))
		        {
                    Graphics graphics = CreateGraphics();
                    Size size = Win32Calls.GetTextExtent(graphics, property.DisplayName, Font);
                    graphics.Dispose();

                    Rectangle labelRect = property.GetLabelTextRect(GetItemRect(propEnum), propEnum);
                    int width = labelRect.Left + size.Width;

                    if (width > maxWidth)
                        maxWidth = width;
		        }

		        propEnum.MoveNext();
	        }

	        if (maxWidth == 0)
		        return 0;

            return maxWidth - LeftColumnWidth + _parentCtrl.GlobalTextMargin;
        }

        public void AdjustLabelColumn()
        {
	        int maxWidth = MeasureWidestLabel();
            if (maxWidth > 0)
                SetLabelColumnWidthRatio((double)maxWidth / (ClientRectangle.Width - LeftColumnWidth));
        }

#if _DOTNET2
        [UIPermission(SecurityAction.LinkDemand, Window = UIPermissionWindow.AllWindows)]
#endif
        protected override bool ProcessDialogKey(Keys keyData)
        {
            Keys key = (keyData & Keys.KeyCode);

            if (key == Keys.Return)
            {
                if (_currentInPlaceControl != null)
                {
                    if (Focused)
                    {
                        SelectAllText();
                        _parentCtrl.ActiveControl = _currentInPlaceControl;
                    }
                    else if (_currentInPlaceControl.ContainsFocus)
                    {
                        if (_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys)
                        {
                            Focus();
                            _currentInPlaceControl.Invalidate();
                        }
                        else
                        {
                            if (_parentCtrl.TabToNextProperty())
                                return true;
                        }
                    }

                    return true;
                }
            }
            else if (key == Keys.Escape)
            {
                if ((_currentInPlaceControl != null) && (_currentInPlaceControl.ContainsFocus))
                {
                    if (_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys)
                    {
                        Focus();
                        _currentInPlaceControl.Invalidate();
                    }
                    else
                    {
                        _parentCtrl.TabToNextProperty();
                    }
                }

                return true;
            }
            else if ((key == Keys.Tab) && Focused)
                SelectAllText();

            return base.ProcessDialogKey(keyData);
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            Keys key = keyData & Keys.KeyCode;

            if ((((keyData & Keys.Control) != Keys.None) && ((keyData & Keys.Shift) == Keys.None)) && ((keyData & Keys.Alt) == Keys.None))
            {
                if (key == Keys.A)
                {
                    if ((_currentInPlaceControl != null) && (((_currentInPlaceControl as IInPlaceControl).TextBox != null) ||
                        (_currentInPlaceControl is TextBoxBase)))
                    {
                        _currentInPlaceControl.Focus();
                        SelectAllText();
                    }
                    return true;
                }
                else if (key == Keys.C)
                {
                    DoCopy();
                    return true;
                }
                else if (key == Keys.V)
                {
                    if (_currentInPlaceControl != null)
                    {
                        _currentInPlaceControl.Focus();
                        SelectAllText();
                        DoPaste();
                    }

                    return true;
                }
                else if (key == Keys.X)
                {
                }
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        public void DoCut()
        {
            if (_currentInPlaceControl == null)
                return;

            TextBoxBase tb = (_currentInPlaceControl as IInPlaceControl).TextBox;
            if (tb == null)
                tb = (_currentInPlaceControl as TextBoxBase);
            if (tb != null)
            {
                _currentInPlaceControl.Focus();
                SelectAllText();
                tb.Cut();
            }
            else
            {
                string str = _currentInPlaceControl.Text;
                if ((str != null) && (str.Length > 0))
                    Clipboard.SetDataObject(str);
            }
        }

        public void DoCopy()
        {
            if (_currentInPlaceControl == null)
                return;

            TextBoxBase tb = (_currentInPlaceControl as IInPlaceControl).TextBox;
            if (tb == null)
                tb = (_currentInPlaceControl as TextBoxBase);
            if (tb != null)
            {
                _currentInPlaceControl.Focus();
                SelectAllText();
                tb.Copy();
            }
            else
            {
                string str = _currentInPlaceControl.Text;
                if ((str != null) && (str.Length > 0))
                    Clipboard.SetDataObject(str);
            }
        }

        public void DoPaste()
        {
            if (_currentInPlaceControl == null)
                return;

            TextBoxBase tb = (_currentInPlaceControl as IInPlaceControl).TextBox;
            if (tb == null)
                tb = (_currentInPlaceControl as TextBoxBase);
            if (tb != null)
            {
                if (tb.Focused)
                    tb.Paste();
                else
                {
                    _currentInPlaceControl.Focus();
                    SelectAllText();
                    tb.Paste();
                }
            }
            else
            {
                IDataObject dataObject = Clipboard.GetDataObject();
                if (dataObject != null)
                {
                    string str = (string)dataObject.GetData(typeof(string));
                    if (str != null)
                        _currentInPlaceControl.Text = str;
                }
            }
        }

        void OnChildGotFocus(object sender, EventArgs e)
        {
            _dontShowInPlaceCtrlInGotFocus = false;
        }

        void OnChildLostFocusInternal()
        {
            if (Focused)
            {
                _dontShowInPlaceCtrlInGotFocus = true;

                if (_focusedControlAfterKillFocus == null)
                {
                    (Parent as ContainerControl).ActiveControl = this;
                    HideInPlaceControl();
                }
            }
            else
            {
                if (_currentInPlaceControl != null)
                {
                    if (_currentInPlaceControl.ContainsFocus == false)
                    {
                        if (!PropertyGrid.IsInsideNotifyValueValidation)
                        {
                            // When another application is made active
                            Control activeControl = ((ContainerControl)TopLevelControl).ActiveControl;
                            if (((activeControl == _parentCtrl) ||
                                ((activeControl != null) && activeControl.Contains(_parentCtrl))) &&
                                (_focusedControlAfterKillFocus == null))
                            {
                                _validationWhileAppLoosingFocus = true;
                                PropertyValue.ValueValidationResult changed = (_currentInPlaceControl as IInPlaceControl).CommitChanges(true);

                                if (((changed & PropertyValue.ValueValidationResult.ErrorCode) != 0) &&
                                    (_parentCtrl.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                                {
                                    InPlaceControl.Refresh();
                                }
                                else
                                {
                                    _validationWhileAppLoosingFocus = false;
                                    _parentCtrl.ActiveControl = this;
                                    HideInPlaceControl();
                                }
                            }

                            // The inplace control must not be hidden when it is a dropdown content that is opened
                            // (making the inplace control loose the focus)
                            else if (_selectedPropertyEnum.Property.InPlaceCtrlInAction == null)
                                HideInPlaceControl();
                        }
                    }
                    else
                    {
                        if ((((ContainerControl)TopLevelControl).ActiveControl == _parentCtrl) &&
                            (_lastLeaveKey != Keys.None))
                        {
                            _parentCtrl.ActiveControl = this;
                        }
                    }
                }
            }

            _lastLeaveKey = Keys.None;

            Invalidate();
        }

        /// <summary>
        /// Removes a property or category from the PropertyGrid.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property or category being removed.</param>
        /// <remarks> If the property or category being removed has descendants, they are all removed.</remarks>
        public void DeleteProperty(PropertyEnumerator propEnum)
        {
            DeletePropertyInternal(propEnum);

            if (IsHandleCreated)
            {
                // We call OnSizeChanged here because this method checks to see if there
                // is some space left at the bottom of the grid and scrolls it accordingly.
                // It also calls CheckScrollbar()
                Rectangle rect = ClientRectangle;
                OnSizeChanged(rect.Width, rect.Height);

                Invalidate();
            }
        }

        internal void DeletePropertyInternal(PropertyEnumerator propEnum)
        {
            if (_multiSelectMode != PropertyGrid.MultiSelectModes.None)
                _multiSelectedItems.Clear();

            bool changeSelect = (_selectedPropertyEnum.IsDescendantOf(propEnum) || (propEnum == _selectedPropertyEnum));
            bool changeFirstDisplayed = (_firstDisplayedPropertyEnumerator.IsDescendantOf(propEnum) || (propEnum == _firstDisplayedPropertyEnumerator));

            if (changeSelect)
            {
                // Unselect it before deletion
                SelectProperty(_categorizedPropertyCollection.GetVisibleDeepEnumerator().RightBound.GetVisibleDeepEnumerator());
            }

            PropertyEnumerator newEnumerator = _categorizedPropertyCollection.Delete(propEnum);

            if (!IsVerticallyFrozen)
            {
                if (changeFirstDisplayed)
                    _firstDisplayedPropertyEnumerator = newEnumerator.GetVisibleDeepEnumerator();

                if (changeSelect)
                    SelectProperty(newEnumerator.GetVisibleDeepEnumerator());
            }
            else
            {
                if (changeFirstDisplayed)
                    _firstDisplayedPropertyEnumerator = RightBound.GetVisibleDeepEnumerator();

                if (changeSelect)
                    SelectProperty(RightBound);
            }

            InvalidateVisibleItemCount();
        }

        public PropertyEnumerator RightBound
        {
            get { return _activePropertyCollection.GetDeepEnumerator().RightBound; }
        }

        public PropertyEnumerator LeftBound
        {
            get { return _activePropertyCollection.GetDeepEnumerator().LeftBound; }
        }

        public PropertyEnumerator FirstProperty
        {
            get
            {
                PropertyEnumerator propEnum = _activePropertyCollection.GetDeepEnumerator();
                return propEnum.MoveFirst();
            }
        }

        public PropertyEnumerator LastProperty
        {
            get
            {
                PropertyEnumerator propEnum = _activePropertyCollection.GetDeepEnumerator();
                return propEnum.MoveLast();
            }
        }

        private void AssignValidator(PropertyEnumerator propEnum)
        {
            PropertyValue value = propEnum.Property.Value;

            // Try to find if the user specified a Validator for the property
            PropertyValidatorAttribute validatorAttr = (PropertyValidatorAttribute)value.GetAttribute(typeof(PropertyValidatorAttribute));
            if ((validatorAttr != null) && (validatorAttr.ChildPropertyName.Length == 0))
            {
                value.Validator = validatorAttr.Validator;
                value.Validator.Enumerator = propEnum;
            }
            else
            {
                // Try to find if the user specified a Validator for the property on its parent
                if ((propEnum.Parent.Property != null) && (propEnum.Parent.Property.Value != null))
                {
                    ArrayList attrs = propEnum.Parent.Property.Value.GetAttributes(typeof(PropertyValidatorAttribute));
                    foreach (PropertyValidatorAttribute attr in attrs)
                    {
                        if (attr.ChildPropertyName == propEnum.Property.DisplayName)
                        {
                            value.Validator = attr.Validator;
                            value.Validator.Enumerator = propEnum;
                        }
                    }
                }
            }
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            if (ToolTip != null)
                ToolTip.SetText("", null);
        }

        // When a property is not selected and a double-click is done in the value part, the inplace control is
        // first made visible and then a click is generated for him (in this.OnMouseDown). But the second click
        // is never treated as a double-click. That's why we hook the mouse messages so that we can generate this
        // lost message.
        public bool OnMouseHooked(IntPtr hWnd, uint message, Point pt, uint hitTest)
        {
            // We don't do anything if IsInsideNotifyValueValidation is true, meaning that we are surely
            // inside a modal form triggered by the client app because of an invalid value
            if (!PropertyGrid.IsInsideNotifyValueValidation && ((message == Win32Calls.WM_LBUTTONDOWN) ||
                (message == Win32Calls.WM_RBUTTONDOWN) || (message == Win32Calls.WM_MBUTTONDOWN) ||
                (message == Win32Calls.WM_LBUTTONDBLCLK) || (message == Win32Calls.WM_NCLBUTTONDOWN)))
            {
                Control control = FromHandle(hWnd);

                if (SelectedPropertyEnumerator.Property.InPlaceCtrlInAction == _currentInPlaceControl)
                {
                    if ((control == _currentInPlaceControl) && TryOnForbidClickThrough(_currentInPlaceControl.PointToClient(pt)))
                        /* ||
                        ((_dropDownTopWindow != null) && _dropDownTopWindow.Contains(control))*/
                    {
                        Focus();
                        if ((_currentInPlaceControl != null) && _currentInPlaceControl.Focused)
                            Focus();

                        if (_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.TabKey)
                        {
                            _parentCtrl.TabToNextProperty();
                        }
                        else
                        {
							if ((_currentInPlaceControl != null) && _currentInPlaceControl.Focused)
								Focus();
                        }

                        return true;
                    }
                    else if (control == this)
                    {
                        _parentCtrl.ActiveControl = this; // Focus() would give the focus to the inplace control tmporarily
                        Focus();
                    }
                }
                
                if (_validationWhileAppLoosingFocus)
                {
                    // Avoid getting a second validation in the processing of WM_ACTIVATE
                    _messageHook.HookMessages = false;

                    if ((_currentInPlaceControl as IInPlaceControl).TextBox == null)
                        _currentInPlaceControl.Focus();
                    else
                        (_currentInPlaceControl as IInPlaceControl).TextBox.Focus();

                    _validationWhileAppLoosingFocus = false;

                    PropertyValue.ValueValidationResult changed = (_currentInPlaceControl as IInPlaceControl).CommitChanges(true);
                    if (((changed & PropertyValue.ValueValidationResult.ErrorCode) != 0) &&
                        (_parentCtrl.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                    {
                        SelectAllText();
                    }

                    _messageHook.HookMessages = true;

                    return true;
                }
                else if ((_currentInPlaceControl != null) && ((control == _currentInPlaceControl) || (_currentInPlaceControl.Contains(control))))
                {
                    if ((int)((DateTime.Now.Ticks - _lastTimeClickedFromParent) / 0x2710) < SystemInformation.DoubleClickTime)
                    {
                        bool removeMsg = false;
                        if ((Math.Abs(pt.X - _lastPosClickedInParent.X) < SystemInformation.DoubleClickSize.Width) &&
                            (Math.Abs(pt.Y - _lastPosClickedInParent.Y) < SystemInformation.DoubleClickSize.Height))
                        {
                            pt = _currentInPlaceControl.PointToClient(pt);
                            Win32Calls.PostMessage(control.Handle, Win32Calls.WM_LBUTTONDBLCLK, 0,
                                Win32Calls.MakeLong(pt.X, pt.Y));

                            removeMsg = true;
                        }

                        _lastPosClickedInParent = Point.Empty;
                        _lastTimeClickedFromParent = 0;

                        // When we click in the inplace control but not in the textbox, the textbox text must be 
                        // commited to check for invalid values
                        TextBox tb = (_currentInPlaceControl as IInPlaceControl).TextBox;
                        if ((tb != null) && tb.Focused && (control != tb))
                        {
                            // The text is checked only if it has changed
                            if (!(_currentInPlaceControl as IInPlaceControl).OwnerPropertyEnumerator.Property.Value.DisplayString.Equals(
                                tb.Text))
                            {
                                PropertyValue.ValueValidationResult changed = (_currentInPlaceControl as IInPlaceControl).CommitChanges(false);
                                if (((changed & PropertyValue.ValueValidationResult.ErrorCode) != 0) &&
                                    (_parentCtrl.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                                {
                                    SelectAllText();
                                    return true;
                                }
                            }
                        }

                        if ((_selectedPropertyEnum.Property.InPlaceCtrlInAction != null) &&
                            (_currentInPlaceControl.Contains(control)))
                            _gridOrCurrentInPlaceCtrlClickedDuringDropDown = true;

                        return removeMsg;
                    }
                }
                else if ((_currentInPlaceControl != null) &&
                    !Win32Calls.IsChild(_currentInPlaceControl.Handle, hWnd) &&
                    ((IntPtr)Win32Calls.GetWindowLong(hWnd, -8) != _currentInPlaceControl.Handle) &&
                    (_currentInPlaceControl.ContainsFocus || (((IInPlaceControl)_currentInPlaceControl).OwnerPropertyEnumerator.Property.InPlaceCtrlInAction != null)) &&
                    ((control == null) || (control.ProductName != "Skybound VisualTips")))
                {
                    // In the next loop, find if the click was done inside a dropdown box
                    IntPtr handle = hWnd;
                    Control ancestorControl = null;
                    do
                    {
                        handle = (IntPtr)Win32Calls.GetWindowLong(handle, -8);
                        ancestorControl = Control.FromHandle(handle);
                        if (ancestorControl != null)
                        {
                            Control container = (_currentInPlaceControl as IInPlaceControl).GetDropDownContainer();
                            if (container == ancestorControl)
//                            if ((ancestorControl.GetType() == typeof(PropInPlaceUITypeEditor.DropDownForm)) ||
  //                              (ancestorControl.GetType() == typeof(PropInPlaceContainer)))
                                break;  // yes the click was done inside a dropdown box
                        }
                    }
                    while (handle != IntPtr.Zero);

                    if (ancestorControl == null)
                    {
                        PropertyValue.ValueValidationResult changed = (_currentInPlaceControl as IInPlaceControl).CommitChanges(true);
                        if (((changed & PropertyValue.ValueValidationResult.ErrorCode) != 0) &&
                            (_parentCtrl.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                        {
                            SelectAllText();

                            return true;
                        }
                    }

                    if ((_selectedPropertyEnum.Property.InPlaceCtrlInAction != null) &&
                        ((control == this) /*|| (control == _currentInPlaceControl)*/ ||
                        (_currentInPlaceControl.Contains(control))))
                    {
                        _gridOrCurrentInPlaceCtrlClickedDuringDropDown = true;
                    }
                }
            }

            return false;
        }

        public bool OnMessageHooked(Control control, uint message, IntPtr wParam, IntPtr lParam)
        {
            bool controlInsideDropDown = false;
            Control dropDownTopWindow = _dropDownTopWindow;
            if ((dropDownTopWindow != null) && (dropDownTopWindow.Contains(control)))
                controlInsideDropDown = true;

            if ((_dropDownTopWindow != null) && (_dropDownTopWindow is IMessageHookListener))
                (_dropDownTopWindow as IMessageHookListener).OnListenMessageHook(control, message, wParam, lParam);
            else if ((_currentInPlaceControl != null) && (_currentInPlaceControl is IMessageHookListener))
                (_currentInPlaceControl as IMessageHookListener).OnListenMessageHook(control, message, wParam, lParam);

            if (message == Win32Calls.WM_ACTIVATE)
            {
                /*
                // When a dropdown is closed, to avoid that the inplace control takes automatically the focus, the focus
                // is forced on the grid
                if (_currentInPlaceControl != null)
                {
                    Control container = (_currentInPlaceControl as IInPlaceControl).GetDropDownContainer();
                    if ((control == container) && (wParam == (IntPtr)0))
                        _parentCtrl.ActiveControl = this;
                }
                */

                if (_validationWhileAppLoosingFocus)
                {
                    _validationWhileAppLoosingFocus = false;

                    PropertyValue.ValueValidationResult changed = (_currentInPlaceControl as IInPlaceControl).CommitChanges(true);
                    if (((changed & PropertyValue.ValueValidationResult.ErrorCode) != 0) &&
                        (_parentCtrl.ValueNotValidBehaviorMode == PropertyGrid.ValueNotValidBehaviorModes.KeepFocus))
                    {
                        SelectAllText();
                    }
                }
            }
            else if (message == Win32Calls.WM_KILLFOCUS)
            {
                _focusedControlAfterKillFocus = FromHandle(wParam);

                if ((_currentInPlaceControl == control) ||
                    ((_currentInPlaceControl != null) && _currentInPlaceControl.Contains(control)) ||
                    (controlInsideDropDown && (wParam == IntPtr.Zero)) // Click outside app while dropdown active ?
                    )
                {
                    if (_currentInPlaceControl != null)
                    {
                        _currentInPlaceControl.Invalidate();

                        if ((_currentInPlaceControl != _focusedControlAfterKillFocus) &&
                            !_currentInPlaceControl.Contains(_focusedControlAfterKillFocus))
                        {
                            OnChildLostFocusInternal();
                        }
                    }
                }
            }

            return false;
        }

        public void OnInPlaceCtrlFinishedEdition()
        {
            if (Focused && !_gridOrCurrentInPlaceCtrlClickedDuringDropDown)
            {
                if (_parentCtrl.NavigationKeyMode == PropertyGrid.NavigationKeyModes.TabKey)
                    _parentCtrl.TabToNextProperty();
            }

            _gridOrCurrentInPlaceCtrlClickedDuringDropDown = false;
        }

        public void SetDropDownTopWindow(Control control)
        {
            _dropDownTopWindow = control;
        }

        public bool OnKeyboardHooked(int message, KeyEventArgs args)
        {
            if ((message == Win32Calls.WM_KEYDOWN) && (_currentInPlaceControl != null) &&
                _currentInPlaceControl.ContainsFocus)
            {
                if ((args.KeyCode == Keys.Return) || (args.KeyCode == Keys.Escape))
                    _lastLeaveKey = args.KeyCode;
                else
                    _lastLeaveKey = Keys.None;
            }

            return false;
        }

        private void ProcessDisable()
        {
            if (_enabled == false)
            {
                TabStop = true;

                if (_disableMode == PropertyGrid.DisableModes.Full)
                {
                    SelectProperty(RightBound);
                }
                else if (_disableMode == PropertyGrid.DisableModes.NoPropertySelection)
                {
                    SelectProperty(RightBound);
                    TabStop = false;
                }
                else if (_disableMode == PropertyGrid.DisableModes.NoValueSelection)
                {
                    HideInPlaceControl();
                }
                else if (_disableMode == PropertyGrid.DisableModes.Simple)
                {
                    if (ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.Disabled)
                        HideInPlaceControl();
                }
            }
        }

        public int GetPropertyVisibleIndex(PropertyEnumerator propEnum)
        {
            int index = -1;
            Rectangle itemRect = GetAbsoluteItemRect(propEnum);
            Rectangle clientRect = ClientRectangle;

            if ((itemRect.Top >= clientRect.Top) && (itemRect.Top < clientRect.Bottom))
            {
                PropertyEnumerator currentEnum = FirstDisplayedProperty;
                index = 0;
                while (currentEnum != propEnum)
                {
                    if (index == 0)
                        index += currentEnum.Property.HeightMultiplier - _firstDisplayedLine + 1;
                    else
                        index += currentEnum.Property.HeightMultiplier;

                    currentEnum.MoveNext();
                }
            }

            return index;
        }

        /// <summary>
        /// Returns a code that identifies a property among all other properties inside the grid.
        /// </summary>
        /// <remarks>This method is called when saving the states (expanded / selected) of the properties (see
        /// <see cref="SavePropertiesStates"/>).
        /// </remarks>
        /// <param name="propEnum">A reference to the property whose hashcode is requested.</param>
        /// <returns>A code that identifies a property among all other properties inside the grid.</returns>
        public virtual int GetPropertyHashCode(PropertyEnumerator propEnum)
        {
            int hashcode = propEnum.Depth.GetHashCode();

            if (propEnum.Property.Name.Equals(propEnum.Property.DisplayName))
                hashcode ^= propEnum.Property.Name.GetHashCode();
            else
                hashcode ^= propEnum.Property.Name.GetHashCode() ^
                            propEnum.Property.DisplayName.GetHashCode();

            if (propEnum.Property.Value != null)
                hashcode ^= propEnum.Property.Value.UnderlyingType.GetHashCode();

            if (propEnum.Parent.Property != null)
                hashcode ^= GetPropertyHashCode(propEnum.Parent);

            return hashcode;
        }

        public object SavePropertiesStates(PropertyEnumerator propEnum)
        {
            PropertiesStates _propertiesStates = new PropertiesStates();

            while (propEnum != propEnum.RightBound)
            {
                int hashcode = GetPropertyHashCode(propEnum);

                if (propEnum == FirstDisplayedProperty)
                {
                    _propertiesStates._firstDisplayedPropertyHashcode = hashcode;
                    _propertiesStates._firstDisplayedLine = _firstDisplayedLine;
                }

                int index = 0;
                if (propEnum.Property.Selected)
                    _propertiesStates._selectedPropertyVisibleIndex = GetPropertyVisibleIndex(propEnum);

                PropertiesStates.PropertyState state = new PropertiesStates.PropertyState(
                    propEnum.Property.Expanded && (propEnum.Children.Count > 0),
                    propEnum.Property.Selected, index);

                _propertiesStates._states[hashcode] = state;

                propEnum.MoveNext();
            }

            return _propertiesStates;
        }

        public virtual void RestorePropertiesStates(PropertyEnumerator propEnum, object propertiesStates)
        {
            PropertyEnumerator firstDisplayedPropertyEnum = null;
            int selectedPropertyVisibleIndex = (propertiesStates as PropertiesStates)._selectedPropertyVisibleIndex;
            
            while (propEnum != propEnum.RightBound)
            {
                int hashcode = GetPropertyHashCode(propEnum);

                if (hashcode == (propertiesStates as PropertiesStates)._firstDisplayedPropertyHashcode)
                    firstDisplayedPropertyEnum = (PropertyEnumerator)propEnum.Clone();

                object objState = (propertiesStates as PropertiesStates)._states[hashcode];
                if (objState != null)
                {
                    PropertiesStates.PropertyState state = (PropertiesStates.PropertyState)objState;
                    ExpandProperty(propEnum, state.expanded);
                    if (state.selected)
                    {
                        SelectProperty(propEnum);

                        if (selectedPropertyVisibleIndex != -1)
                        {
                            // The selected property was visible so we will ensure that it is displaye at
                            // the exact same row in the grid

                            int currentIndex = GetPropertyVisibleIndex(propEnum);
                            if (currentIndex == -1)
                            {
                                EnsureVisible(propEnum);
                                currentIndex = GetPropertyVisibleIndex(propEnum);
                            }

                            while (currentIndex > selectedPropertyVisibleIndex)
                            {
                                Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEDOWN, 0), 0);
                                currentIndex--;
                            }
                            while (currentIndex < selectedPropertyVisibleIndex)
                            {
                                Win32Calls.SendMessage(Handle, Win32Calls.WM_VSCROLL, Win32Calls.MakeLong(Win32Calls.SB_LINEUP, 0), 0);
                                currentIndex++;
                            }
                        }
                    }
                }

                propEnum.MoveNext();
            }

            if ((selectedPropertyVisibleIndex == -1) && (firstDisplayedPropertyEnum != null))
            {
                // The selected property was not visible so we will rely on the first property that
                // was displayed in the grid

                _firstDisplayedLine = (propertiesStates as PropertiesStates)._firstDisplayedLine;
                FirstDisplayedProperty = firstDisplayedPropertyEnum.GetVisibleDeepEnumerator();
            }
        }

#if _XHEOSOURCECODE
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (_license != null)
				{
					_license.Dispose();
					_license = null;
				}
			}
			base.Dispose(disposing);
		}
#endif
	}
}
