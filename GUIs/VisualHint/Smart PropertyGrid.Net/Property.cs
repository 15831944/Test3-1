// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections;
using System.Diagnostics;

namespace VisualHint.SmartPropertyGrid
{
	/// <summary>
	/// Represents one line of data in the PropertyGrid that the user can usually edit.
    /// </summary>
    /// <remarks>
    /// A property is essentially a pair containing a label and a value. The client application doesn't
    /// create a Property instance explicitely. Instead it supplies the data to the PropertyGrid which
    /// returns an enumerator to a Property or simply makes the PropertyGrid point to the instance of a
    /// class (with <see cref="PropertyGrid.SelectedObject"/>) that will itself add the properties.
    /// </remarks>
#if _DOTNET2
    [DebuggerDisplay("Id={_id}, Name={_displayName}, Value={_value.DisplayString}")]
#endif
    public class Property
	{
        private int _displayedAtPosition = 0;

        /// <summary>
        /// Sets the position (expressed in multiple of <see cref="PropertyGrid.BasicPropertyHeight"/> of
        /// the property relatively to the top of the grid.
        /// </summary>
        /// <remarks>When you get this value, it does not mean the property is still visible. It means that
        /// it was at this position when it was last displayed. To check that it is actually displayed, call
        /// <see cref="PropertyGrid.IsPropertyDisplayed"/>.</remarks>
        public int DisplayedAtPosition
        {
            set { _displayedAtPosition = value; }
            get { return _displayedAtPosition; }
        }

        /// <exclude />
        protected const int TreeGlyphWidth = 8;

        private string _format = "";

        /// <summary>
        /// Applies a format string to the property value.
        /// </summary>
        /// <example>
        /// <code>
        /// // Will turn the property which contains an email address into a usable URL.
        /// propEnum.Property.HyperLinkFormat = "mailto:{0}";
        /// </code>
        /// </example>
        public string HyperLinkFormat
        {
            set
            {
                _format = value;
                if (_value != null)
                {
                    if (_format.Length != 0)
                        _value.ForeColor = Color.Blue;
                    else
                        _value.ForeColor = Color.Empty;
                }
            }

            get { return _format; }
        }

        /// <summary>
        /// Gets the hyperlink string attached to this property. <see cref="HyperLinkFormat"/> must be set
        /// previously so that the property value can be parameterized by the format.
        /// </summary>
        public string HyperLink
        {
            get
            {
                if (_format.Length == 0)
                    return "";
                else
                    return string.Format(HyperLinkFormat, Value.GetStringValue());
            }
        }

        private object _tag;

		/// <summary>
        /// Sets or gets an external user data that is attached to this property.
        /// </summary>
        /// <remarks>The client application is free to attach any value or object to a property.</remarks>
		public object Tag
		{
			set { _tag = value; }
			get { return _tag; }
		}

        private int _sortedIndex = int.MaxValue;

        /// <summary>
        /// Gets an integer that allows the PropertyGrid to sort sibling properties.
        /// </summary>
        /// <remarks>This value is used when the client application fills the PropertyGrid by using the
        /// <see cref="PropertyGrid.SelectedObject"/> C# property and sets the attributes
        /// <see cref="SortedPropertyAttribute"/> or <see cref="SortedCategoryAttribute"/> to the C# properties.
        /// <para>The index doesn't need to start at 0. Indexes are simply compared between each other.</para>
        /// </remarks>
        public int SortedIndex
        {
            get { return _sortedIndex; }
        }

        internal int SortedIndexInternal
        {
            set { _sortedIndex = value; }
        }
        
        private bool _enabled;

		/// <summary>
		/// Gets a boolean that tells if the property is enabled (true) or disabled (false).
		/// </summary>
        /// <remarks>
        /// A Property can be enabled/disabled by using <see cref="PropertyGrid.EnableProperty"/>. If a property
        /// being disabled has children, then they will be disabled. When reenabling a parent Property, all
        /// children will be enabled, whatever their previous state was, except if some of the children have a
        /// manually disabled box set to unchecked, in which case the child Property will stay disabled.
        /// <para>In a disabled state, a property does not show an inplace control when selected.</para>
        /// <para>The color of the text can be changed with <see cref="PropertyGrid.DisabledForeColor"/> or
        /// <see cref="DisabledForeColor"/>.</para>
        /// </remarks>
        public bool Enabled
		{
			get { return _enabled; }
		}

		private bool _visible;

		/// <summary>
		/// Gets a boolean that tells if the property is visible (true) or invisible (false).
		/// </summary>
        /// <remarks>
        /// A Property has a "visible" state. When it is not visible, the property is simply hidden and
        /// it can't never be seen. When the property is visible, it can be seen, but it could be temporarily invisible
        /// just because it is in a collapsed parent category/property. To check that, call
        /// <see cref="PropertyGrid.IsPropertyDisplayed"/>.
        /// <para>This state can be set by using <see cref="PropertyGrid.ShowProperty"/>.</para>
        /// </remarks>
        public bool Visible
		{
			get { return _visible; }
		}

        internal bool VisibleInternal
        {
            set { _visible = value; }
        }
        
        private string _displayName;

		/// <summary>
		/// Sets or gets the label of a property.
		/// </summary>
        /// <remarks>The label is the text that is displayed in the first column of the grid. It also appears in the
        /// comments area.</remarks>
		public string DisplayName
		{
			set
            {
                _displayName = value;
                if (_grid != null)
                    _grid.Grid.Invalidate();
            }

			get { return _displayName; }
		}

        /// <summary>
        /// Gets the name of the property.
        /// </summary>
        /// <remarks>
        /// For a property that is linked to the .net property of a target instance, the name of the .net property
        /// is returned. For a property created with 
        /// <see href="/O_T_VisualHint_SmartPropertyGrid_PropertyGrid_AppendManagedProperty.html">AppendManagedProperty</see>, the <see cref="DisplayName"/> is returned.
        /// </remarks>
        public string Name
        {
            get
            {
                if (_value == null)
                    return "";

                return _value.Name;
            }
        }

		private string _comment = "";

		/// <summary>
		/// Sets or gets the comment that is attached to this property.
		/// </summary>
        /// <remarks>The comment is only used and displayed in the comments area of the grid.</remarks>
		public string Comment
		{
			set
            {
                _comment = value;
                if (_grid != null)
                    _grid.Invalidate();
            }

			get { return _comment; }
		}

		private int _id;

		/// <summary>
		/// Sets or gets the identifier of a property.
		/// </summary>
        /// <remarks>
        /// This integer should usually be a unique identifier of a property but in some cases some properties
        /// could share the same number, therefore the framework doesn't check for unicity.
        /// <para>When you manually fill the PropertyGrid, you can pass the identifier in the parameters of
        /// methods like <see href="/VisualHint.SmartPropertyGrid.PropertyGrid.AppendProperty_overloads.html">
        /// AppendProperty</see>. When you use <see cref="PropertyGrid.SelectedObject"/> instead, you can use
        /// the <see cref="PropertyIdAttribute"> PropertyId</see> attribute to assign relevant identifiers to
        /// the properties of your instance. At last, you can dymacally assign the identifiers after the creation,
        /// like in the <see cref="PropertyGrid.OnPropertyCreated"/> notification message.</para>
        /// <para>Typically an identifier is used in notification messages to know what property is concerned
        /// by the message.</para>
        /// </remarks>
        /// <example>
        /// <code>
        /// protected override void OnPropertyChanged(VisualHint.SmartPropertyGrid.PropertyChangedEventArgs e)
        /// {
        ///    if (e.PropertyEnum.Property.Id == MyProperty_Identifier)
        ///    {
        ///       // Do stuff related to the change of this particular property.
        ///    }
        /// 
        ///    base.OnPropertyChanged(e);
        /// }
        /// </code>
        /// </example>
		public int Id
		{
			set { _id = value; }
			get { return _id; }
		}
        
        /// <summary>
        /// Sets of gets the Feel instance that is used to display an inplace control when the property is selected.
        /// </summary>
        /// <remarks>
        /// This property is a shortcut to <see cref=""PropertyValue.Feel/>. More explanations are given there.
        /// </remarks>
        public PropertyFeel Feel
        {
            get
            {
                if (Value != null)
                    return Value.Feel;

                return null;
            }

            set
            {
                if (Value != null)
                    Value.Feel = value;
            }
        }

        /// <summary>
        /// Sets of gets the Look instance that is used to paint the property when no inplace control is shown.
        /// </summary>
        /// <remarks>
        /// This property is a shortcut to <see cref=""PropertyValue.Look/>. More explanations are given there.
        /// </remarks>
        public PropertyLook Look
        {
            get
            {
                if (Value != null)
                    return Value.Look;

                return null;
            }

            set
            {
                if (Value != null)
                    Value.Look = value;
            }
        }
        
        private PropertyGrid _grid;

		/// <summary>
        /// Gets a reference on the <see cref="PropertyGrid"/> that contains this property.
		/// </summary>
		public PropertyGrid ParentGrid
		{
            get { return _grid; }
        }

        internal PropertyGrid ParentGridInternal
        {
            set { _grid = value; }
        }
        
        private bool _canBeDisabledManually = false;
        
		/// <summary>
		/// Gets a boolean that tells if a checkbox is visible at the left of the property label to let the user
        /// enable/disable the property.
		/// </summary>
        /// <remarks>
        /// To set a checkbox on a property, use
        /// <see href="/O_T_VisualHint_SmartPropertyGrid_PropertyGrid_SetManuallyDisabled.html">SetManuallyDisabled
        /// </see>. To get its value, use <see cref="GetManuallyDisabledVariable"/>.
        /// </remarks>
		public bool CanBeDisabledManually
		{
			get { return _canBeDisabledManually; }
		}
        
        internal void SetManuallyDisabled(PropertyEnumerator propEnum, bool disabled)
        {
            _canBeDisabledManually = disabled;
            if (_canBeDisabledManually)
            {
                _enabledVariable = new PropertyValueManaged(0, typeof(bool), _enabled, null);
                _enabledVariable.Grid = ParentGrid;
//                if (_enabled)
  //                  _enabled = !ParentGrid.Grid.IsAncestorDisabled(propEnum);
            }
            else
                _enabledVariable = null;
        }

        internal void SetManuallyDisabled(PropertyEnumerator propEnum, object containerInstance, string memberName)
		{
			if (containerInstance != null)
			{
				_canBeDisabledManually = true;
                _enabledVariable = new PropertyValueIndirect(_grid, null, containerInstance, TypeDescriptor.GetProperties(containerInstance)[memberName], null);
                _enabledVariable.Grid = ParentGrid;
                ParentGrid.EnableProperty(propEnum, (GetManuallyDisabledVariable() && !ParentGrid.Grid.IsAncestorDisabled(propEnum)));
//                _enabled = (GetManuallyDisabledVariable() && !ParentGrid.Grid.IsAncestorDisabled(propEnum));

//                if (_grid != null)
  //                  _grid.Grid.Invalidate();
            }
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public bool GetManuallyDisabledVariable()
		{
			if (_canBeDisabledManually)
				return bool.Parse(_enabledVariable.GetStringValue());
			else
				return Enabled;
		}

		private PropertyValue _enabledVariable;

		// True if this item is selected, false otherwise.
		private bool _selected;

        /// <summary>
        /// Gets a boolean that tells if the category/property is selected (true) or not (false). 
        /// </summary>
        /// <remarks>A property can be selected by using <see cref="PropertyGrid.SelectProperty"/>.</remarks>
		public bool Selected
		{
			get { return _selected; }
		}

        internal bool SelectedInternal
        {
            set { _selected = value; }
        }
        
        // A pointer to an inplace control if the in-place control is running on this item, null otherwise.
		private Control _inPlaceCtrlInAction = null;

        /// <summary>
        /// Returns true when the listbox part of an inplace control is visible, false otherwise.
        /// </summary>
        /// <remarks>
        /// This property is used anytime it is difficult to know if an inplace control is active but it has
        /// not the focus because a popup control is opened instead.
        /// </remarks>
        public Control InPlaceCtrlInAction
        {
            get { return _inPlaceCtrlInAction; }
            set { _inPlaceCtrlInAction = value; }
        }

		/// <summary>
		/// Points to a Property value. If this Property is a (sub)category, it is null.
		/// </summary>
		private PropertyValue _value;

        public PropertyValue Value
        {
            get { return _value; }
        }

		internal PropertyValue ValueInternal
		{
			set { _value = value; }
		}

        private Hashtable _additionalValues = new Hashtable();  // <object, PropertyValue>

		private Color _foreColor;

        /// <summary>
        /// Set or gets the color used to paint the text of the property label and value.
        /// </summary>
        /// <remarks>
        /// This value overrides the color set by <see cref="PropertyGrid.GridForeColor"/> on the PropertyGrid.
        /// <para>It is also used to paint the value text except if it has been overriden by using
        /// <see cref="PropertyValue.ForeColor"/> on the value itsef.</para>
        /// </remarks>
        public Color ForeColor
        {
            get
            {
                if (_foreColor.IsEmpty)
                    return _grid.GridForeColor;
                else
                    return _foreColor;
            }

            set
            {
                _foreColor = value;
                if (_grid != null)
                    _grid.Grid.Invalidate();
            }
        }

        private Color _disabledForeColor;

        /// <summary>
        /// Sets or gets the text color of a property (label and value) when the property is disabled.
        /// </summary>
        /// <remarks>
        /// This value overrides the color set by <see cref="PropertyGrid.DisabledForeColor"/>.
        /// <para>The text color of the value can be overriden by using <see cref="PropertyValue.DisabledForeColor"/>
        /// on the value itself.</para>
        /// </remarks>
        public Color DisabledForeColor
        {
            get
            {
                if (_disabledForeColor.IsEmpty)
                    return _grid.DisabledForeColor;
                else
                    return _disabledForeColor;
            }

            set
            {
                _disabledForeColor = value;
                if (_grid != null)
                    _grid.Grid.Invalidate();
            }
        }

        private Color _readOnlyForeColor;

        public Color ReadOnlyForeColor
        {
            get
            {
                if (_readOnlyForeColor.IsEmpty)
                    return _grid.ReadOnlyForeColor;
                else
                    return _readOnlyForeColor;
            }

            set
            {
                _readOnlyForeColor = value;
                if (_grid != null)
                    _grid.Grid.Invalidate();
            }
        }

        private Color _backColor;

        /// <summary>
        /// Set or gets the color used to paint the background of the property label and value.
        /// </summary>
        /// <remarks>
        /// This value overrides the color set by <see cref="PropertyGrid.GridBackColor"/> on the PropertyGrid.
        /// <para>It is also used to paint the value background except if it has been overriden by using
        /// <see cref="PropertyValue.BackColor"/> on the value itsef.</para>
        /// </remarks>
        public Color BackColor
        {
            get
            {
                if (_backColor.IsEmpty)
                    return _grid.GridBackColor;
                else
                    return _backColor;
            }

            set
            {
                _backColor = value;
                if (_grid != null)
                    _grid.Grid.Invalidate();
            }
        }

        private Font _font;

        /// <summary>
        /// Sets or gets the font used to paint the text of the property label and value.
        /// </summary>
        /// <remarks>
        /// This font overrides the font set by <see cref="PropertyGrid.Font"/> on the PropertyGrid.
        /// <para>It is also used to paint the value text except if it is later overriden by using
        /// <see cref="PropertyValue.Font"/> on the value itself.</para>
        /// </remarks>
        public Font Font
        {
            get
            {
                return (_font == null ? _grid.Font : _font);
            }

            set
            {
                _font = value;
                if (Value != null)
                    Value.FontSetOnProperty();
                if (_grid != null)
                    _grid.Grid.Invalidate();
            }
        }

        private int _heightMultiplier = 1;

        /// <summary>
        /// Sets or gets the height of a property, given in multiples of the basic height.
        /// </summary>
        /// <remarks>
        /// A property has normally a fixed height wich depends on the selected font. It is possible to
        /// have some properties with several lines of text or graphics and therefore their height is a
        /// multiple of the standard height. This value represents the number of lines used by the Property.
        /// </remarks>
        /// <example>
        /// <code>
        /// propEnum = AppendProperty(rootEnum, _id, "Multiline Edit box", this, "MyValue", "");
        /// propEnum.Property.Feel = GetRegisteredFeel(PropertyGrid.FeelMultilineEdit);
        /// propEnum.Property.HeightMultiplier = 3;     // 3 lines for the multiedit inplace control
        /// </code>
        /// </example>
        public int HeightMultiplier
        {
            set
            {
                if (value != _heightMultiplier)
                {
                    _heightMultiplier = value;
                    _grid.Grid.HeightMultiplierChanged();

                    _grid.Grid.Refresh();
                }
            }

            get { return _heightMultiplier; }
        }

		private bool _expanded;

        /// <summary>
        /// Sets or gets a boolean that tells if the category/property is expanded (true) or collapsed (false).
        /// </summary>
		public bool Expanded
		{
			get { return _expanded; }
		}

        internal bool ExpandedInternal
        {
            set { _expanded = value; }
        }

        private int _imageIndex = -1;

        /// <summary>
        /// Sets or gets the index of the icon displayed at the left of the property label.
        /// </summary>
        /// <remarks>
        /// The index points inside an image list that you can set by using <see cref="PropertyGrid.ImageList"/>. To
        /// remove an icon, use the value -1.
        /// </remarks>
        public int ImageIndex
        {
            get { return _imageIndex; }
            set { _imageIndex = value; }
        }

        internal Property(int propertyId, string name)
		{
            Id = propertyId;
			_selected = false;
			_enabled = true;
            _visible = true;
			_expanded = true;
            DisplayName = name;

            _foreColor = Color.Empty;
            _backColor = Color.Empty;
            _disabledForeColor = Color.Empty;
        }

		// For testing
		public override string ToString()
		{
			return "Property id = " + Id.ToString();
		}

		public virtual void Draw(Graphics graphics, Rectangle itemRect, PropertyVisibleDeepEnumerator enumSelf)
		{
	        Color textColor;
	        Color valueColor;

            // Choose the colors of the background and text regarding the item selection
	        if (Selected)
	        {
                if (!ParentGrid.Enabled && (ParentGrid.DisableMode != PropertyGrid.DisableModes.None) &&
                    ParentGrid.DisableModeGrayedOut)
                {
                    valueColor = _grid.DisabledForeColor;
                }
                else
                {
                    if (Enabled == false)
                        valueColor = Value.DisabledForeColor;
                    else
                    {
                        if (Value.ReadOnly)
                            valueColor = Value.ReadOnlyForeColor;
                        else
                            valueColor = Value.ForeColor;
                    }
                }

                if ((ParentGrid.GridContainsFocus) || (InPlaceCtrlInAction != null))
                    textColor = ParentGrid.HighlightedTextColor;
                else
                    textColor = ForeColor;
            }
	        else										// if not selected
	        {
                if ((!ParentGrid.Enabled && (ParentGrid.DisableMode != PropertyGrid.DisableModes.None)) &&
                    ParentGrid.DisableModeGrayedOut)
                {
                    textColor = _grid.DisabledForeColor;
                    valueColor = _grid.DisabledForeColor;
                }
                else
                {
                    if (Enabled == false)
                    {
/*                        if (Value.ReadOnly && (ParentGrid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.ReadOnlyFeel))
                        {
                            valueColor = Value.ReadOnlyForeColor;
                            textColor = ReadOnlyForeColor;
                        }
                        else*/
                        {
                            valueColor = Value.DisabledForeColor;
                            textColor = DisabledForeColor;
                        }
                    }
                    else
                    {
                        if (Value.ReadOnly)
                        {
                            valueColor = Value.ReadOnlyForeColor;
                            textColor = ReadOnlyForeColor;
                        }
                        else
                        {
                            textColor = ForeColor;
                            valueColor = Value.ForeColor;
                        }
                    }
                }
            }

	        // Draw the label background
	        //--------------------------

	        Rectangle rect = itemRect;
	        rect.X += ParentGrid.LeftColumnWidth + 1;
	        rect.Width = ParentGrid.LabelColumnWidth;
	        ParentGrid.DrawManager.DrawPropertyLabelBackground(graphics, rect, enumSelf);

            // Draw the +/- sign if needed
	        //----------------------------

	        if (ParentGrid.HasOneVisibleChild(enumSelf))
                ParentGrid.DrawManager.DrawPlusMinusSign(graphics, ParentGrid.Grid, itemRect, enumSelf);

            // Draw checkbox used to be able to manually disable/enable this property
	        //-----------------------------------------------------------------------

	        if (CanBeDisabledManually)
	        {
                Rectangle frameRect = GetManuallyDisableRect(itemRect, enumSelf);
                ParentGrid.DrawManager.DrawCheckBox(graphics, ParentGrid.Grid, frameRect, GetManuallyDisabledVariable(),
                    !ParentGrid.Grid.IsAncestorDisabled(enumSelf));
            }

            // Draw the value background
            //--------------------------

            Rectangle valueRect = GetValueRect(itemRect);
            ParentGrid.DrawManager.DrawPropertyValueBackground(graphics, valueRect, enumSelf);

            // Draw the vertical separation between the columns and the bottom horizontal line
            //--------------------------------------------------------------------------------

            Rectangle labelColumnRect = GetLabelColumnRect(itemRect, enumSelf);
            ParentGrid.DrawManager.DrawSeparationLines(graphics, itemRect, labelColumnRect, enumSelf);

            // Draw label text
	        //----------------

            // Draw icon if any
            Rectangle imgRect = GetImageRect(itemRect, enumSelf);
            if (imgRect != Rectangle.Empty)
            {
                Win32Calls.SetClippingRect(graphics, labelColumnRect);
                ParentGrid.DrawImage(ImageIndex, graphics, imgRect.Left, imgRect.Top);
                Win32Calls.ResetClippingRect(graphics);
            }

            Rectangle labelRect = GetLabelTextRect(itemRect, enumSelf);
            ParentGrid.DrawManager.DrawPropertyLabelText(graphics, labelRect, textColor, enumSelf);

	        // Draw value
	        //-----------

            // The height must be the same than the one assigned to inplace controls
            valueRect.Height--;

            Value.DrawValue(graphics, valueRect, valueColor, enumSelf, null);
		}

		public Rectangle GetValueRect(Rectangle itemRect)
		{
            Rectangle valueRect = itemRect;

            // the rectangle begins after the label rect and goes to the right end.
            valueRect.X += ParentGrid.LeftColumnWidth + 1 + ParentGrid.LabelColumnWidth + 1;
            valueRect.Width -= ParentGrid.LeftColumnWidth + 1 + ParentGrid.LabelColumnWidth + 1;

            return valueRect;
        }

		internal void Enable(PropertyEnumerator enumSelf, bool enable, bool ancestorDisabled, bool direct)
		{
            // We can't change the state if we want to enable in a disabled ancestor
            if (!enable || !ancestorDisabled)
            {
                if (!CanBeDisabledManually || direct)
                {
                    _enabled = enable;
                }
                else
                {
                    if (enable)
                        _enabled = GetManuallyDisabledVariable();
                    else
                        _enabled = false;
                }
            }

            // If there is a checkbox for manual operation, the attached variable
            // must be updated regarding the new state.
            if (CanBeDisabledManually && direct)
                _enabledVariable.SetValue(enable);

            if (_enabled)
            {
                if ((Feel != null) && (Feel.InPlaceCtrl != null))
                    (Feel.InPlaceCtrl as IInPlaceControl).RepositionChildren();
            }

            _grid.OnPropertyEnabled(new PropertyEnabledEventArgs(enumSelf, direct));
        }

        internal bool OnSetCursor(Point point, Rectangle itemRect, PropertyEnumerator enumSelf)
		{
			return false;
		}

		internal virtual void OnPropertyClicked(PropertyEnumerator enumSelf, Point point, Rectangle itemRect)
		{
            if ((Control.ModifierKeys == Keys.Control) && (HyperLinkFormat.Length > 0))
            {
                Rectangle valueRect = GetValueRect(itemRect);
                Graphics graphics = ParentGrid.CreateGraphics();
                Rectangle strRect = Value.GetStringValueRect(graphics, valueRect, point);
                graphics.Dispose();

                if (strRect.Contains(point))
                    ParentGrid.OnHyperLinkPropertyClicked(new PropertyHyperLinkClickedEventArgs(enumSelf, HyperLink));
            }
		}

		public Type GetValueType()
		{
			return _value.GetType();
		}

		public void UpdateValueFromChildren(PropertyEnumerator propEnum)
		{
            string str = "";

            PropertyEnumerator childEnum = propEnum.Children;
            char separator = propEnum.Property.Value.GroupedValueSeparator;
            while (childEnum != childEnum.RightBound)
            {
                str += childEnum.Property.Value.GetStringValue();
                childEnum.MoveNext();

                if (childEnum != childEnum.RightBound)
                {
                    str += separator;
                    str += " ";
                }
            }

            Value.SetValue(str);
        }

        public virtual Rectangle GetSignRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            if (ParentGrid.HasOneVisibleChild(enumSelf) == false)
                return Rectangle.Empty;

            Rectangle signRect = itemRect;

            signRect.Y += signRect.Height / HeightMultiplier / 2 - 4;
            signRect.Height = TreeGlyphWidth;

            signRect.X = ParentGrid.LeftColumnWidth + 1 + (enumSelf.Depth - 1) * (_grid.GlobalTextMargin + TreeGlyphWidth) + _grid.GlobalTextMargin;
            signRect.Width = TreeGlyphWidth;

            return signRect;
        }

        public virtual Rectangle GetManuallyDisableRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            if (CanBeDisabledManually == false)
                return Rectangle.Empty;

            Rectangle frameRect = itemRect;

            Rectangle signRect = GetSignRect(itemRect, enumSelf);
            if (signRect != Rectangle.Empty)
                frameRect.X = _grid.GlobalTextMargin + signRect.Right;
            else
            {
                frameRect.X = _grid.GlobalTextMargin + ParentGrid.LeftColumnWidth + 1 +
                    (enumSelf.Depth - 1) * (_grid.GlobalTextMargin + TreeGlyphWidth);
            }
            // Size it based on the row height
            frameRect.Y += 2;
            frameRect.Height = ParentGrid.BasicPropertyHeight - 4;
            frameRect.Width = frameRect.Height;

            return frameRect;
        }

        public virtual Rectangle GetImageRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            if (ImageIndex == -1)
                return Rectangle.Empty;

            Rectangle imgRect = itemRect;

            Rectangle checkboxRect = GetManuallyDisableRect(itemRect, enumSelf);
            if (checkboxRect != Rectangle.Empty)
                imgRect.X = checkboxRect.Right + _grid.GlobalTextMargin;
            else
            {
                Rectangle signRect = GetSignRect(itemRect, enumSelf);
                if (signRect != Rectangle.Empty)
                    imgRect.X = signRect.Right + _grid.GlobalTextMargin;
                else
                {
                    if (enumSelf.Depth == 0)
                        imgRect.X = ParentGrid.LeftColumnWidth + 1 + _grid.GlobalTextMargin;
                    else
                        imgRect.X = ParentGrid.LeftColumnWidth + 1 + _grid.GlobalTextMargin +
                        (enumSelf.Depth - 1) * (_grid.GlobalTextMargin + TreeGlyphWidth);
                }
            }

            try
            {
                Image img = ParentGrid.ImageList.Images[ImageIndex];
                imgRect.Width = img.Width;
                imgRect.Height = img.Height;
            }
            catch (Exception)
            {
                return Rectangle.Empty;
            }

            imgRect.Y = itemRect.Top + ParentGrid.BasicPropertyHeight / 2 - 8;

            return imgRect;
        }

        public virtual Rectangle GetLabelTextRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            Rectangle labelRect = itemRect;

            Rectangle imgRect = GetImageRect(itemRect, enumSelf);
            if (imgRect != Rectangle.Empty)
            {
                labelRect.X = imgRect.Right;
            }
            else
            {
                Rectangle checkboxRect = GetManuallyDisableRect(itemRect, enumSelf);
                if (checkboxRect != Rectangle.Empty)
                    labelRect.X = checkboxRect.Right;
                else
                {
                    Rectangle signRect = GetSignRect(itemRect, enumSelf);
                    if (signRect != Rectangle.Empty)
                        labelRect.X = signRect.Right;
                    else
                    {
                        labelRect.X = ParentGrid.LeftColumnWidth + 1 +
                            (enumSelf.Depth - 1) * (_grid.GlobalTextMargin + TreeGlyphWidth);
                    }
                }
            }

            labelRect.X += _grid.GlobalTextMargin;

            Graphics graphics = ParentGrid.CreateGraphics();
            labelRect.Width = Win32Calls.GetTextExtent(graphics, DisplayName, Font).Width;
            graphics.Dispose();

            int maxRight = ParentGrid.LeftColumnWidth + 1 + ParentGrid.LabelColumnWidth;
            if ((ParentGrid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnLabels) != 0)
                maxRight -= _grid.GlobalTextMargin;

            if (labelRect.Right > maxRight)
                labelRect.Width = maxRight - labelRect.Left;

            return labelRect;
        }

        public virtual Rectangle GetLabelColumnRect(Rectangle itemRect, PropertyEnumerator enumerator)
        {
            Rectangle labelRect = itemRect;
            labelRect.X = ParentGrid.LeftColumnWidth + 1;
            labelRect.Width = ParentGrid.LabelColumnWidth;

            return labelRect;
        }

        public virtual bool OverrideCursor(Point mouseLocation, Rectangle itemRect, PropertyEnumerator propEnum)
        {
            if ((HyperLinkFormat.Length != 0) && (Control.ModifierKeys == Keys.Control) && (Value != null))
            {
                Rectangle valueRect = GetValueRect(itemRect);
                Graphics graphics = ParentGrid.CreateGraphics();
                Rectangle strRect = Value.GetStringValueRect(graphics, valueRect, mouseLocation);
                graphics.Dispose();

                if (strRect.Contains(mouseLocation) && Enabled)
                {
                    ParentGrid.Grid.Cursor = Cursors.Hand;
                    return true;
                }
            }

            return false;
        }

        /// <summary>
        /// Attaches an additional property value to this property.
        /// </summary>
        /// <param name="key">The unique key that identifies the additional value.</param>
        /// <param name="container">The instance of an object containing the C# property displayed in this new property value.</param>
        /// <param name="memberName">The name of the C# property whose value is displayed in this new property value.</param>
        /// <param name="attributes">Since the new property is created dynamically, it is possible to attach
        /// attributes not know at design-time to the property.</param>
        /// <remarks>
        /// You use AddValue to create a property that holds multiple values. This is the technique used to create
        /// a property that contains a numerical value and its unit.
        /// <para>The value can be retrieved by using <see cref="Property.GetValue"/>.</para>
        /// </remarks>
        /// <example>
        /// <code>
        /// propEnum = AppendProperty(catEnum, _id, "Frequency", this, "Frequency", "");
        /// propEnum.Property.AddValue(PropertyUnitLook.UnitValue, this, "Unit", null);
        /// propEnum.Property.Value.Look = new PropertyUnitLook();
        /// propEnum.Property.Feel = GetRegisteredFeel(PropertyGrid.FeelEditUnit);
        /// </code>
        /// </example>
        public void AddValue(object key, object container, string memberName, params Attribute[] attributes)
        {
            PropertyValue propertyValue;
            if ((propertyValue = (PropertyValue)_additionalValues[key]) == null)
            {
                propertyValue = new PropertyValueIndirect(_grid, null, container, TypeDescriptor.GetProperties(container)[memberName], attributes);

                _additionalValues.Add(key, propertyValue);

                propertyValue.OwnerEnumerator = Value.OwnerEnumerator;
                propertyValue.AdditionalValueKey = key;
            }
            else
            {
                (propertyValue as PropertyValueIndirect).AddTargetInstance(container, TypeDescriptor.GetProperties(container)[memberName]);
            }
        }

        public void AddValue(object key, object container, PropertyDescriptor propertyDescriptor, params Attribute[] attributes)
        {
            PropertyValue propertyValue;
            if ((propertyValue = (PropertyValue)_additionalValues[key]) == null)
            {
                propertyValue = new PropertyValueIndirect(_grid, null, container, propertyDescriptor, attributes);

                _additionalValues.Add(key, propertyValue);

                propertyValue.OwnerEnumerator = Value.OwnerEnumerator;
                propertyValue.AdditionalValueKey = key;
            }
            else
            {
                (propertyValue as PropertyValueIndirect).AddTargetInstance(container, propertyDescriptor);
            }
        }

        /// <summary>
        /// Attaches an additional property value to this property.
        /// </summary>
        /// <param name="key">The unique key that identifies the additional value.</param>
        /// <param name="valueType">The type of the additional value stored by the property.</param>
        /// <param name="defaultValue">The initial value of the additional property value.</param>
        /// <param name="attributes">Since the new property is created dynamically, it is possible to attach
        /// attributes not know at design-time to the property.</param>
        /// <remarks>
        /// You use AddValue to create a property that holds multiple values. This is the technique used to create
        /// a property that contains a numerical value and its unit.
        /// <para>The value can be retrieved by using <see cref="Property.GetValue"/>.</para>
        /// </remarks>
        /// <example>
        /// <code>
        /// propEnum = AppendProperty(catEnum, _id, "Frequency", this, "Frequency", "");
        /// propEnum.Property.AddValue(PropertyUnitLook.UnitValue, this, "Unit", null);
        /// propEnum.Property.Value.Look = new PropertyUnitLook();
        /// propEnum.Property.Feel = GetRegisteredFeel(PropertyGrid.FeelEditUnit);
        /// </code>
        /// </example>
        public void AddManagedValue(object key, Type valueType, object defaultValue, params Attribute[] attributes)
        {
            PropertyValue value = new PropertyValueManaged(0, valueType, defaultValue, attributes);
            value.Grid = _grid;

            _additionalValues.Add(key, value);

            value.OwnerEnumerator = Value.OwnerEnumerator;
            value.AdditionalValueKey = key;
        }
        
        /// <summary>
        /// Returns the additional property value that has been attached to this property.
        /// </summary>
        /// <param name="key">The unique key that identifies the additional value.</param>
        /// <returns>The additional property value that has been attached to this property.</returns>
        public PropertyValue GetValue(object key)
        {
            PropertyValue propertyValue;
            if ((propertyValue = (PropertyValue)_additionalValues[key]) != null)
                return propertyValue;
            else
                return null;
        }
    }
}
