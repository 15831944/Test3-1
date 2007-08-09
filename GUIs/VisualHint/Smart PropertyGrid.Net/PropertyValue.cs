// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System;
using System.Reflection;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Globalization;
using System.Drawing.Design;

using System.Runtime.InteropServices;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Represents the value part of a property.
    /// </summary>
    public class PropertyValue
    {
        /// <summary>
        /// Enumeration value passed to the client application in the ValueValidation event arguments when a value
        /// is invalid or when it is valid just after it has been invalid.
        /// </summary>
        [Flags]
        public enum ValueValidationResult
        {
            /// <summary>
            /// Indicates that a value is now valid and that it is new. It means that there was a prior notification
            ///  with an invalidid error code.
            /// </summary>
            Validated = 1,

            /// <summary>
            /// Indicates that a previous valid value has been restored. It means that there was a prior notification
            ///  with an invalidid error code.
            /// </summary>
            PreviousValueRestored = 2,

            /// <summary>
            /// Indicates that a value is invalid because of an exception raised by the attached TypeConverter.
            /// </summary>
            TypeConverterFailed = 4,

            /// <summary>
            /// Indicates that a value is invalid because of an attached Validator class.
            /// </summary>
            ValidatorFailed = 8,

            /// <summary>
            /// Indicates that when setting a new value, the set accessor method raised an exception.
            /// </summary>
            ExceptionByClient = 16,

            /// <summary>
            /// Convenience code allowing to "AND" it with another value of this enumeration to know if it is
            /// indicating a valid or invalid value.
            /// </summary>
            ErrorCode = 0x1C
        }

        internal event EventHandler PropertyChangedEvent;

        private static int UITypeEditorCustomDrawingWidth = 20;

        protected Type mUnderlyingType;

        /// <summary>
        /// Gets the type of the target value that is attached to a property.
        /// </summary>
        public virtual Type UnderlyingType
        {
            get
            {
                if (mUnderlyingType == null)
                {
                    object obj = UnderlyingValue;
                    mUnderlyingType = (obj != null ? obj.GetType() : null);
                }

                return mUnderlyingType;
            }
        }

        /// <summary>
        /// Gets a boolean that indicates if a property is linked to several .Net properties that have different values.
        /// </summary>
        /// <remarks>
        /// This call is useful when <see cref="PropertyGrid.SelectedObjects"/> or
        /// <see cref="PropertyGrid.AddTargetInstance"/> has been used, creating a property that targets several .Net
        /// properties in several instances. If initially one of them has a different value, the grid can not display a
        /// unique value. In this case, HasMultipleValues returns true.
        /// </remarks>
        public virtual bool HasMultipleValues
        {
            get { return false; }
        }

        protected object mUnderlyingValue;

		internal virtual object UnderlyingValue
		{
			set
            {
                mUnderlyingValue = value;
            }

            get { return mUnderlyingValue; }
		}

        private bool _noLinkWithChildren = true;

        /// <summary>
        /// Sets or gets a boolean indicating if a parent property's value has no logical link with the values of
        /// the child properties. The default is false.
        /// </summary>
        /// <remarks>
        /// To create properties that have no link between each other, first create the parent, set NoLinkWithChildren
        /// to true, then create the child properties underneath.
        /// </remarks>
        public bool NoLinkWithChildren
        {
            set { _noLinkWithChildren = value; }
            get { return _noLinkWithChildren; }
        }

        private bool _childValueCreatedBySPG = false;

        internal bool ChildValueCreatedBySPG
        {
            set { _childValueCreatedBySPG = value; }
            get { return _childValueCreatedBySPG; }
        }

        private CultureInfo _cultureInfo = null;

        /// <summary>
        /// Sets or gets a CultureInfo instance for a property value.
        /// </summary>
        /// <remarks>
        /// Each property can have a different culture. This CultureInfo instance is passed to the TypeConverter
        /// attached to the underlying .Net property.
        /// </remarks>
        public CultureInfo CultureInfo
        {
            get
            {
                if (_cultureInfo == null)
                    _cultureInfo = (CultureInfo)CultureInfo.CurrentCulture.Clone();

                return _cultureInfo;
            }

            set
            {
                _cultureInfo = value;
            }
        }

        private int _id;

        internal int Id
        {
            set { _id = value; }
            get { return _id; }
        }

        private object _additionalValueKey;

        public object AdditionalValueKey
        {
            set { _additionalValueKey = value; }
            get { return _additionalValueKey; }
        }

        private bool _previousValueValid = true;

        public bool PreviousValueValid
        {
            set { _previousValueValid = value; }
            get { return _previousValueValid; }
        }

        private object _previousValue;

        public object PreviousValue
        {
            set { _previousValue = value; _previousValueValid = !HasMultipleValues; }
            
            get
            {
                if (_previousValueValid == false)
                    throw new MultipleValuesException();

                return _previousValue;
            }
        }

        private int _editboxLeftMargin = -1;

        public int EditboxLeftMargin
        {
            get { return _editboxLeftMargin; }
        }

        private object _tag;

        /// <summary>
        /// Sets or gets an external user data that is attached to this value.
        /// </summary>
        /// <remarks>The client application is free to attach any value or object to a property Value.</remarks>
        public object Tag
        {
            set { _tag = value; }
            get { return _tag; }
        }

        private Font _font;

        public Font Font
        {
            get { return (_font == null ? mOwnerEnumerator.Property.Font : _font); }
            set
            {
                _font = value;
                FontSetOnProperty();

                if ((mOwnerEnumerator != null) && (mOwnerEnumerator.Property != null))
                    mOwnerEnumerator.Property.ParentGrid.Grid.Invalidate();
            }
        }

        internal void FontSetOnProperty()
        {
            TextBox tb = new TextBox();
            tb.Font = Font;
            Win32Calls.SendMessage(tb.Handle, Win32Calls.EM_SETMARGINS, 0x0003, Win32Calls.MakeLong(0xffff, 0xffff));
//            _editboxLeftMargin = 2 + Win32Calls.LoWord(Win32Calls.SendMessage(tb.Handle, Win32Calls.EM_GETMARGINS, 0, 0));

            _editboxLeftMargin = Grid.GlobalTextMargin - 3;

            if (mOwnerEnumerator.Property.Selected)
            {
                Control inPlaceControl = mOwnerEnumerator.Property.ParentGrid.InPlaceControl;
                if (inPlaceControl != null)
                {
                    inPlaceControl.Font = Font;

                    Rectangle itemRect = mOwnerEnumerator.Property.ParentGrid.Grid.GetItemRect(mOwnerEnumerator);
                    Rectangle valueRect = mOwnerEnumerator.Property.GetValueRect(itemRect);
                    Feel.MoveControl(valueRect, mOwnerEnumerator);
                }
            }
        }

        private Color _foreColor;

        public Color ForeColor
        {
            get
            {
                if (_foreColor.IsEmpty)
                    return mOwnerEnumerator.Property.ForeColor;
                else
                    return _foreColor;
            }

            set
            {
                _foreColor = value;
                Control inPlaceControl = mOwnerEnumerator.Property.ParentGrid.InPlaceControl;
                if (inPlaceControl != null)
                    inPlaceControl.ForeColor = _foreColor;
                else
                    mOwnerEnumerator.Property.ParentGrid.Refresh();
            }
        }

        private Color _disabledForeColor;

        public Color DisabledForeColor
        {
            get
            {
                if (_disabledForeColor.IsEmpty)
                    return mOwnerEnumerator.Property.DisabledForeColor;
                else
                    return _disabledForeColor;
            }

            set
            {
                _disabledForeColor = value;
                Control inPlaceControl = mOwnerEnumerator.Property.ParentGrid.InPlaceControl;
                if (inPlaceControl != null)
                    inPlaceControl.ForeColor = _disabledForeColor;
                else
                    mOwnerEnumerator.Property.ParentGrid.Refresh();
            }
        }

        private Color _readOnlyForeColor;

        public Color ReadOnlyForeColor
        {
            get
            {
                if (_readOnlyForeColor.IsEmpty)
                    return mOwnerEnumerator.Property.ReadOnlyForeColor;
                else
                    return _readOnlyForeColor;
            }

            set
            {
                _readOnlyForeColor = value;
                Control inPlaceControl = mOwnerEnumerator.Property.ParentGrid.InPlaceControl;
                if (inPlaceControl != null)
                    inPlaceControl.ForeColor = _readOnlyForeColor;
                else
                    mOwnerEnumerator.Property.ParentGrid.Refresh();
            }
        }

        private Color _backColor;

        public Color BackColor
        {
            get
            {
                if (_backColor.IsEmpty)
                    return mOwnerEnumerator.Property.BackColor;
                else
                    return _backColor;
            }

            set
            {
                _backColor = value;
                Control inPlaceControl = mOwnerEnumerator.Property.ParentGrid.InPlaceControl;
                if (inPlaceControl != null)
                    inPlaceControl.BackColor = _backColor;
                else
                    mOwnerEnumerator.Property.ParentGrid.Refresh();
            }
        }

        private Size _imageSize = Size.Empty;

        public Size ImageSize
        {
            get { return _imageSize; }
            set { _imageSize = value; }
        }

        private ImageList _imageList = null;

        public ImageList ImageList
        {
            set
            {
                _imageList = value;

                if (HasMultipleValues == false)
                {
                    string[] displayedValues = GetDisplayedValues();
                    if (displayedValues.Length == 0)
                    {
                        DisplayedValuesNeededEventArgs e = new DisplayedValuesNeededEventArgs(OwnerEnumerator);
                        OwnerEnumerator.Property.ParentGrid.OnDisplayedValuesNeeded(e);

                        if (e.DisplayedValues == null)
                            displayedValues = new string[0];
                        else
                            displayedValues = e.DisplayedValues;
                    }

                    int index = 0;
                    foreach (object str in displayedValues)
                    {
                        if (str.ToString() == DisplayString)
                        {
                            ImageIndex = index;
                            break;
                        }

                        index++;
                    }
                }
                else
                    _imageIndex = -1;
            }

            get { return _imageList; }
        }

        private int _imageIndex;

        public int ImageIndex
        {
            set { _imageIndex = value; }
            get { return _imageIndex; }
        }

		// Optional look attached to this value.
		private PropertyLook _look;

        /// <summary>
        /// Sets of gets the Look instance that is used to paint the property when no inplace control is shown.
        /// </summary>
        /// <remarks>
        /// Contrarily to feels, a look is created by instanciating its class and assigning it to the value object.
        /// </remarks>
        /// <example>
        /// <code>
        /// propEnum.Property.Value.Look = new PropertyMaskedEditLook("(000) 000-0000");
        /// </code>
        /// </example>
        public PropertyLook Look
        {
            set
            {
                _look = value;
                if (_look != null)
                {
                    _look.Value = this;
                    _look.AttachProperty(mOwnerEnumerator);
                    _look.ControlHeightMultiplier();

                    if (IsReadOnly(mOwnerEnumerator) && _look.NoFeelForReadOnly && (mPropCtrl.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.ReadOnlyFeel))
                        Feel = null;
                }
            }

            get { return _look; }
        }

        protected PropertyFeel mFeel;

        /// <summary>
        /// Sets of gets the Feel instance that is used to display an inplace control when the property is selected.
        /// </summary>
        /// <remarks>
        /// To let the user edit a property, you must assign a feel to it. Using this property is a way to proceed.
        /// Because a feel is a singleton, you don't create yourself a feel object. Instead you request from the
        /// PropertyGrid a previously registered feel by using <see cref="PropertyGrid.GetRegisteredFeel"/>.
        /// <para>The other ways to assign a feel to a property are explained in <see cref="PropertyFeelAttribute"/> and
        /// in <see cref="PropertyGrid.RegisterFeelAttachment"/>.</para>
        /// </remarks>
        /// <example>
        /// <code>
        /// propEnum.Property.Feel = GetRegisteredFeel(PropertyGrid.FeelEdit);
        /// </code>
        /// </example>
        public PropertyFeel Feel
        {
            set { mFeel = value; }

            get
            {
                if (ReadOnly && (Look != null) && (Look.NoFeelForReadOnly))
                    return null;

                if (!Grid.Grid.Enabled && (Grid.DisableMode == PropertyGrid.DisableModes.Simple) &&
                    (Grid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.Disabled))
                    return null;

                return mFeel;
            }
        }

        private char _groupedValueSeparator = ';';

        public char GroupedValueSeparator
        {
            get { return _groupedValueSeparator; }
            set { _groupedValueSeparator = value; }
        }

        // A pointer to the parent PropertyGrid.
		protected PropertyGrid mPropCtrl;

        public PropertyGrid Grid
        {
            set { mPropCtrl = value; }
            get { return mPropCtrl; }
        }

        private bool _drawingDoneByEditor = false;

        public bool DrawingDoneByEditor
        {
            get { return _drawingDoneByEditor; }
        }

        private UITypeEditor _uiTypeEditor;

        protected PropertyEnumerator mOwnerEnumerator;

        public PropertyEnumerator OwnerEnumerator
        {
            set
            {
                mOwnerEnumerator = (PropertyEnumerator)value.Clone();

                ExtractDisplayedValues();

                // This call is to simulate a value change report to the parent so that the parent value
                // can be updated while creating and initializing this value.
                if ((mOwnerEnumerator.Parent.Property != null) && (mOwnerEnumerator.Parent.Property.Value != null) &&
                    (mOwnerEnumerator.Parent.Property.Value.NoLinkWithChildren == false))
                    mOwnerEnumerator.Parent.Property.Value.OnChildValueChanged(this);

                _uiTypeEditor = (UITypeEditor)mOwnerEnumerator.Property.Value.PropertyDescriptor.GetEditor(typeof(UITypeEditor));
#if _DOTNET2
                if (_uiTypeEditor == null)
                {
                    NullableConverter nc = mOwnerEnumerator.Property.Value.TypeConverter as NullableConverter;
                    if (nc != null)
                        _uiTypeEditor = (UITypeEditor)TypeDescriptor.GetEditor(nc.UnderlyingType, typeof(UITypeEditor));
                }
#endif
                if (_uiTypeEditor == null)
                {
                    // Try to find a UITypeEditor attached to the property with an attribute
                    EditorAttribute eAttr = (EditorAttribute)mOwnerEnumerator.Property.Value.GetAttribute(typeof(EditorAttribute));
                    if (eAttr == null)
                    {
                        foreach (object obj in mOwnerEnumerator.Property.Value.UnderlyingType.GetCustomAttributes(typeof(EditorAttribute), true))
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
                        Type editorType = Utilities.GetTypeFromName(eAttr.EditorTypeName, mOwnerEnumerator.Property.Value.UnderlyingType);
                        if (editorType.IsSubclassOf(typeof(UITypeEditor)))
                        {
#if _DOTNET2
                            if (editorType.GetConstructor(typeArray) != null)
                                _uiTypeEditor = (UITypeEditor)TypeDescriptor.CreateInstance(null, editorType, typeArray, new object[] { mOwnerEnumerator.Property.Value.UnderlyingType });
                            else
                                _uiTypeEditor = (UITypeEditor)TypeDescriptor.CreateInstance(null, editorType, null, null);
#else
                            if (editorType.GetConstructor(typeArray) != null)
                                _uiTypeEditor = (UITypeEditor)Activator.CreateInstance(editorType, new object[] { mOwnerEnumerator.Property.Value.UnderlyingType });
                            else
                                _uiTypeEditor = (UITypeEditor)Activator.CreateInstance(editorType);
#endif
                        }
                    }
                }

                // Try to find a UITypeEditor attached to the Type
                if (_uiTypeEditor == null)
                    _uiTypeEditor = (UITypeEditor)TypeDescriptor.GetEditor(UnderlyingType, typeof(UITypeEditor));

                PropertyTypeDescriptorContext context = GetTypeDescriptorContext(mOwnerEnumerator);
                if ((_uiTypeEditor != null) && !_uiTypeEditor.GetType().Equals(typeof(UITypeEditor)) &&
                    (_uiTypeEditor.GetPaintValueSupported(context)))
                {
                    if ((_uiTypeEditor.GetEditStyle() == UITypeEditorEditStyle.None) &&
                        mOwnerEnumerator.Property.Value.TypeConverter.GetStandardValuesSupported(context))
                        _drawingDoneByEditor = true;
                }
            }

            get
            {
                return mOwnerEnumerator;
            }
        }

        public virtual string DisplayName
        {
            get { return ""; }
        }

        public virtual string Name
        {
            get { return mOwnerEnumerator.Property.DisplayName; }
        }

        private Hashtable _customAttributes = new Hashtable(); // <Type, ArrayList<Attribute>>

        private PropertyValue _parentValue;

        protected PropertyValue ParentValue
        {
            get { return _parentValue; }
        }

        internal Attribute[] Attributes
        {
            set
            {
                if ((value != null) && (value.Length > 0))
                {
                    foreach (Attribute attr in value)
                    {
                        AttributeUsageAttribute attrUsage = (AttributeUsageAttribute)TypeDescriptor.GetAttributes(attr)
                            [typeof(AttributeUsageAttribute)];
                        if (attrUsage.AllowMultiple == false)
                            SetSingleAttribute(attr);
                        else
                        {
                            ArrayList oneTypeAttributes;
                            if (_customAttributes.ContainsKey(attr.GetType()) == false)
                            {
                                oneTypeAttributes = new ArrayList();
                                _customAttributes[attr.GetType()] = oneTypeAttributes;
                            }
                            else
                                oneTypeAttributes = (ArrayList)_customAttributes[attr.GetType()];

                            oneTypeAttributes.Add(attr);
                        }
                    }
                }
            }
        }

        internal void SetSingleAttribute(Attribute attr)
        {
            ArrayList oneTypeAttributes;
            if ((oneTypeAttributes = (ArrayList)_customAttributes[attr.GetType()]) == null)
            {
                oneTypeAttributes = new ArrayList();
                _customAttributes[attr.GetType()] = oneTypeAttributes;
            }
            else
                oneTypeAttributes.Clear();

            oneTypeAttributes.Add(attr);
        }

        internal void DeleteSingleAttribute(Attribute attr)
        {
            _customAttributes.Remove(attr.GetType());
        }

        /// <summary>
        /// 
        /// </summary>
        public bool ReadOnly
        {
            get
            {
                return IsReadOnly(mOwnerEnumerator);
            }

            set
            {
                ReadOnlyAttribute attr = (ReadOnlyAttribute)_GetAttribute(typeof(ReadOnlyAttribute));
                bool change = false;
                if (attr != null)
                {
                    if (value && (attr.IsReadOnly == false))
                    {
                        SetSingleAttribute(ReadOnlyAttribute.Yes);
                        change = true;
                    }
                    else if ((value == false) && attr.IsReadOnly)
                    {
                        SetSingleAttribute(ReadOnlyAttribute.No);
                        change = true;
                    }
                }
                else
                {
                    if (value && !ReadOnly)
                    {
                        SetSingleAttribute(ReadOnlyAttribute.Yes);
                        change = true;
                    }
                    else if ((value == false) && ReadOnly)
                    {
                        SetSingleAttribute(ReadOnlyAttribute.No);
                        change = true;
                    }
                }

                if (change)
                {
                    PropertyGrid pgrid = OwnerEnumerator.Property.ParentGrid;

                    if (ReadOnly)
                    {
                        if (pgrid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.Disabled)
                            pgrid.EnableProperty(OwnerEnumerator, false);
                        else
                            pgrid.Grid.Invalidate();
                    }
                    else
                    {
                        pgrid.EnableProperty(OwnerEnumerator, true);
                        pgrid.Grid.Invalidate();
                    }
                }
            }
        }

        private PropertyValidatorBase _validator = null;

        public PropertyValue()
        {
            _foreColor = Color.Empty;
            _backColor = Color.Empty;
            _disabledForeColor = Color.Empty;
        }

        public PropertyValue(PropertyValue parentValue) : this()
        {
            _parentValue = parentValue;
        }

        public PropertyValidatorBase Validator
        {
            set { _validator = value; _validator.Enumerator = mOwnerEnumerator; }
            get { return _validator; }
        }

        protected internal virtual bool ValidateSelfValueFromModifiedChild(PropertyEnumerator modifiedChildEnum, object value)
        {
            return true;
        }

        public virtual bool ValidateValue(object value, out PropertyEnumerator invalidPropertyEnum)
        {
            invalidPropertyEnum = Grid.RightBound;
            return true;
        }

        public static bool IsErrorCode(ValueValidationResult result)
        {
            return ((result & ValueValidationResult.ErrorCode) != 0);
        }

        /// <summary>
        /// Set a new value to a property.
        /// </summary>
        /// <param name="value">The new value to be set.</param>
        /// <returns>An enumeration value specifying if the value has been set or not.</returns>
        public ValueValidationResult SetValue(object value)
        {
            PropertyEnumerator invalidPropEnum;
            return SetValue(value, out invalidPropEnum);
        }

        /// <summary>
        /// Set a new value to a property. This method must be called from the CommitChanges method of an inplace
        /// control.
        /// </summary>
        /// <param name="value">The new value to be set.</param>
        /// <returns>An enumeration value specifying if the value has been set or not.</returns>
        public ValueValidationResult SetValueFromInPlaceCtrl(object value)
        {
            PropertyEnumerator invalidPropEnum;
            return SetValueFromInPlaceCtrl(value, out invalidPropEnum);
        }

        private bool _setValueFromInPlaceCtrl = false;

        /// <summary>
        /// Set a new value to a property. This method must be called from the CommitChanges method of an inplace
        /// control.
        /// </summary>
        /// <param name="value">The new value to be set.</param>
        /// <param name="invalidPropEnum">Indicates the property that is really invalid, RightBound otherwise.</param>
        /// <returns>An enumeration value specifying if the value has been set or not.</returns>
        public ValueValidationResult SetValueFromInPlaceCtrl(object value, out PropertyEnumerator invalidPropEnum)
        {
            _setValueFromInPlaceCtrl = true;
            ValueValidationResult result = SetValue(value, out invalidPropEnum);
            _setValueFromInPlaceCtrl = false;

            return result;
        }

        /// <summary>
        /// Set a new value to a property.
        /// </summary>
        /// <param name="value">The new value to be set.</param>
        /// <param name="invalidPropEnum">Indicates the property that is really invalid, RightBound otherwise.</param>
        /// <returns>An enumeration value specifying if the value has been set or not.</returns>
        public ValueValidationResult SetValue(object value, out PropertyEnumerator invalidPropEnum)
        {
            invalidPropEnum = Grid.RightBound;

            object valueToSet = value;

            // Let's transform the string value into a typed object
            if (value is string)
            {
                string str = (valueToSet as string);
                if (_look != null)
                    str = _look.TransformDisplayString(str);

                try
                {
                    valueToSet = GetActualString(str);
                    PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
                    if (TypeConverter.CanConvertFrom(context, typeof(string)))
                        valueToSet = GetValueToValidate(valueToSet as string);
                }
                catch (Exception e)
                {
                    Grid.NotifyValueValidation(new ValueValidationEventArgs(OwnerEnumerator, OwnerEnumerator,
                        value, PropertyValue.ValueValidationResult.TypeConverterFailed, e));
                    return ValueValidationResult.TypeConverterFailed;
                }
            }
            
            // Validate the value and make the change if it is valid
            if (ValidateValue(valueToSet, out invalidPropEnum))
                return SetValue(valueToSet, ValueUpdateSource.FromOutside);
            else
                return ValueValidationResult.ValidatorFailed;
        }

        private Exception _lastExceptionByClient;
        public Exception LastExceptionByClient
        {
            get { return _lastExceptionByClient; }
        }

        protected internal ValueValidationResult SetValue(object value, ValueUpdateSource updateSource)
        {
            bool result;
            try
            {
                result = SetValueInternal(value, updateSource);
            }
            catch (Exception e)
            {
                _lastExceptionByClient = e;
                return ValueValidationResult.ExceptionByClient;
            }

            if (result && (mOwnerEnumerator != null)) // mOwnerEnumerator could be null for the boolean of Property._enabledVariable
            {
//                RecreateChildProperties();

                // Inform its parent that this value has changed
                if (updateSource != ValueUpdateSource.FromParent)
                {
                    if ((mOwnerEnumerator.Parent.Property != null) && (mOwnerEnumerator.Parent.Property.Value != null) &&
                        (mOwnerEnumerator.Parent.Property.Value.NoLinkWithChildren == false))
                        mOwnerEnumerator.Parent.Property.Value.OnChildValueChanged(this);
                }

                if ((mOwnerEnumerator.Children.Count > 0) && (updateSource != ValueUpdateSource.FromChild) &&
                    (NoLinkWithChildren == false))
                {
                    // Inform its children that this value has changed
                    PropertyEnumerator childEnum = mOwnerEnumerator.Children;
                    while (childEnum != childEnum.RightBound)
                    {
                        if (childEnum.Property.Value != null)
                            childEnum.Property.Value.OnParentValueChanged(this);
                        childEnum.MoveNext();
                    }

                    ReUpdateFromChildren();
                }

                Control inplaceCtrl = mOwnerEnumerator.Property.ParentGrid.InPlaceControl;
                IInPlaceControl iinplaceCtrl = inplaceCtrl as IInPlaceControl;

                if ((iinplaceCtrl != null) && !_setValueFromInPlaceCtrl && (iinplaceCtrl.OwnerPropertyEnumerator == mOwnerEnumerator))
                    // If there is an inplace control for this value, then update its content
                    iinplaceCtrl.OwnerPropertyEnumerator = mOwnerEnumerator;
                else
                    // Else just redraw the grid
                    mOwnerEnumerator.Property.ParentGrid.Grid.Invalidate();
            }

            return (result ? ValueValidationResult.Validated : ValueValidationResult.TypeConverterFailed);
        }

        /// <summary>
        /// Modifies the content of the underlying value.
        /// </summary>
        /// <param name="value">The object from which content must be assigned to the underlying value.</param>
        /// <param name="updateSource"></param>
        /// <returns>A boolean is the underlying value was successfully changed, false otherwise.</returns>
        protected virtual bool SetValueInternal(object value, ValueUpdateSource updateSource)
        {
            return false;
        }

        protected void RecreateChildProperties()
        {
            // In the case of an array for example, when the array changed, all the child properties
            // must be deleted and recreated.
            PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
            if (TypeConverter.GetPropertiesSupported(context)/* && !TypeConverter.GetCreateInstanceSupported(context)*/)
            {
                bool expanded = OwnerEnumerator.Property.Expanded && (OwnerEnumerator.Children.Count > 0);
                
                PropertyEnumerator propEnum = OwnerEnumerator.Children;
                while (propEnum != propEnum.RightBound)
                {
                    PropertyEnumerator nextEnum = ((PropertyEnumerator)propEnum.Clone()).MoveNext();
                    Grid.Grid.DeletePropertyInternal(propEnum);
                    propEnum = nextEnum;
                }

                Grid.Grid.AddChildProperties(OwnerEnumerator);
                Grid.ExpandProperty(OwnerEnumerator, expanded);

                Grid.Grid.CloseEmptySpace();
                Grid.Grid.CheckScrollbar();
            }
        }

        protected internal PropertyValue[] GetChildValues(object[] targetInstances, PropertyDescriptor[] propertyDescriptors)
        {
            PropertyValue[] valueArray;

            Hashtable propertyDescriptorDefs = new Hashtable(); // <PropertyDescriptor, List<PropertyDescriptor>>

            PropertyDescriptorCollection collection = null;
            bool firstTarget = true;
            bool toBeSorted = true;
            for(int i=0; i<targetInstances.Length; i++)
            {
                object targetInstance = targetInstances[i];

                PropertyDescriptorCollection pdCollection = OwnerEnumerator.Property.ParentGrid.
                    GetPropertyDescriptors(targetInstance, propertyDescriptors[i], out toBeSorted);
                if (collection == null)
                {
                    PropertyDescriptor[] array = new PropertyDescriptor[pdCollection.Count];
                    pdCollection.CopyTo(array, 0);
                    collection = new PropertyDescriptorCollection(array);
                }

                foreach (PropertyDescriptor propertyDescriptor in pdCollection)
                {
                    MergablePropertyAttribute mergeAttr = propertyDescriptor.Attributes[typeof(MergablePropertyAttribute)]
                        as MergablePropertyAttribute;
                    if ((mergeAttr != null) && (mergeAttr.AllowMerge == false))
                        continue;

                    if (firstTarget)
                    {
                        // For the first target instance, we store all the PropertyDescriptors
                        ArrayList pdList = new ArrayList();
                        pdList.Add(propertyDescriptor);

                        propertyDescriptorDefs.Add(propertyDescriptor, pdList);
                    }
                    else
                    {
                        // For all the remaining target instances, we compare the PropertyDescriptors
                        // to the first set of stored PropertyDescriptors. Their name and property type must
                        // correspond
                        ArrayList existPdList = (ArrayList)propertyDescriptorDefs[propertyDescriptor];
                        if (existPdList != null)
                        {
                            // If the propertyDesriptors match, we increment a count
                            if (OwnerEnumerator.Property.ParentGrid.MatchPropertyDescriptors(
                                (PropertyDescriptor)existPdList[0], propertyDescriptor))
                            {
                                existPdList.Add(propertyDescriptor);
                            }
                            // else the PropertyDescriptor is not eligible and is removed
                            else
                                propertyDescriptorDefs.Remove(propertyDescriptor);
                        }
                    }
                }

                firstTarget = false;
            }

            ArrayList validPropertyDescriptors = new ArrayList();

            // Ensure that we count only propertyDescriptors that are present in all target instances
            foreach (ArrayList pdList in propertyDescriptorDefs.Values)
            {
                if (pdList.Count != targetInstances.Length)
                    collection.Remove((PropertyDescriptor)pdList[0]);
            }

            // Sort the collection
            if (toBeSorted && (Grid.PropertyComparer != null))
                collection = collection.Sort(Grid.PropertyComparer);

            PropertyTypeDescriptorContext context = new PropertyTypeDescriptorContext(null, targetInstances[0], null,
                OwnerEnumerator.Property.ParentGrid);
            bool readOnlyProperties = TypeConverter.GetCreateInstanceSupported(context);
            valueArray = new PropertyValue[collection.Count];
            int index = 0;
            foreach (PropertyDescriptor descriptor in collection)
            {
                PropertyValue propValue;
                try
                {
                    object obj = UnderlyingValue;
                    if (obj is ICustomTypeDescriptor)
                        obj = ((ICustomTypeDescriptor)obj).GetPropertyOwner(descriptor);

                    // TODO : pourquoi cette ligne ?
                    descriptor.GetValue(obj);
                }
                catch (Exception)
                {
                    // Reason : activeXHide ?
                }

                if (readOnlyProperties)
                    propValue = new PropertyValueSimulated(Grid, this, descriptor);
                else
                    propValue = new PropertyValueIndirect(Grid, this, UnderlyingValue, descriptor, null);

                propValue.ChildValueCreatedBySPG = true;

                PropertyIdAttribute attr = descriptor.Attributes[typeof(PropertyIdAttribute)] as PropertyIdAttribute;
                propValue.Id = (attr != null ? attr.Id : 0);

                valueArray[index++] = propValue;
            }

            return valueArray;
        }
/*
        protected internal virtual PropertyValue[] GetChildValues(object[] targetInstances)
        {
            PropertyValue[] valueArray;

            PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
            if (TypeConverter.GetPropertiesSupported(context))
            {
                PropertyDescriptorCollection collection = null;

                try
                {
                    collection = TypeConverter.GetProperties(context, UnderlyingValue,
                    new Attribute[1] { BrowsableAttribute.Yes });
                }
                catch (Exception)
                {
                }

                if (collection == null)
                    return null;

                // Sort the collection
                if (Grid.PropertyComparer != null)
                    collection = collection.Sort(Grid.PropertyComparer);

                bool readOnlyProperties = TypeConverter.GetCreateInstanceSupported(context);
                valueArray = new PropertyValue[collection.Count];
                int index = 0;
                foreach (PropertyDescriptor descriptor in collection)
                {
                    PropertyValue propValue;
                    try
                    {
                        object obj = UnderlyingValue;
                        if (obj is ICustomTypeDescriptor)
                            obj = ((ICustomTypeDescriptor)obj).GetPropertyOwner(descriptor);

                        // TODO : pourquoi cette ligne ?
                        descriptor.GetValue(obj);
                    }
                    catch (Exception)
                    {
                        // Reason : activeXHide ?
                    }

                    if (readOnlyProperties)
                        propValue = new PropertyValueSimulated(Grid, this, descriptor);
                    else
                        propValue = new PropertyValueIndirect(Grid, this, UnderlyingValue, descriptor, null);

                    PropertyIdAttribute attr = descriptor.Attributes[typeof(PropertyIdAttribute)] as PropertyIdAttribute;
                    propValue.Id = (attr != null ? attr.Id : 0);

                    valueArray[index++] = propValue;
                }
            }
            else
            {
                PropertyDescriptorCollection collection = TypeDescriptor.GetProperties(UnderlyingValue, new Attribute[1] { BrowsableAttribute.Yes });

                if (collection == null)
                    return null;

                // Sort the collection
                if (Grid.PropertyComparer != null)
                    collection = collection.Sort(Grid.PropertyComparer);

                valueArray = new PropertyValue[collection.Count];
                int index = 0;
                foreach (PropertyDescriptor descriptor in collection)
                {
                    try
                    {
                        valueArray[index] = new PropertyValueIndirect(Grid, this, UnderlyingValue, descriptor, null);

                        PropertyIdAttribute attr = descriptor.Attributes[typeof(PropertyIdAttribute)] as PropertyIdAttribute;
                        valueArray[index].Id = (attr != null ? attr.Id : 0);

                        index++;
                    }
                    catch (Exception)
                    {
                    }
                }

                if (index != collection.Count)
                {
                    PropertyValue[] valueArray2 = new PropertyValue[index];
                    Array.Copy(valueArray, valueArray2, index);
                    valueArray = valueArray2;
                }
            }

            return valueArray;
        }
*/
        /// <exclude />
        public enum ValueUpdateSource
        {
            FromChild,
            FromParent,
            FromOutside
        }

        public string DisplayString
        {
            get
            {
                if (_look != null)
                    return _look.DisplayString;
                else
                    return GetDisplayString(GetStringValue());
            }
        }

        public string DisplayStringWithoutLook
        {
            get
            {
                return GetDisplayString(GetStringValue());
            }
        }

        public virtual string GetDisplayString(string actualStr)
        {
            return null;
        }

        public string GetActualString(string displayStr)
        {
            if (mDisplayedStrToActualStr != null)
            {
                // We try to find an actual string for the displayed string
                string actualStr = (string)mDisplayedStrToActualStr[displayStr];

                // But if we can't, we assume that maybe the parameter displayStr was actually an actual string
                return (actualStr == null ? displayStr : actualStr);
            }
            else
                return displayStr;
        }

        public virtual string GetStringValue()
        {
            return "";
        }

        public virtual Rectangle GetStringValueRect(Graphics graphics, Rectangle valueRect, Point point)
        {
	        if (_look != null)
		        return _look.GetDisplayStringRect(graphics, valueRect, point);
	        else
	        {
                Size size;
                if (HasMultipleValues)
                    size = Win32Calls.GetTextExtent(graphics, "", Font);
                else
                    size = Win32Calls.GetTextExtent(graphics, DisplayString, Font);

                Rectangle rect = valueRect;
                rect.X += Grid.GlobalTextMargin;
                rect.Width = Size.Round(size).Width;

                if ((ImageList != null) && (ImageIndex >= 0))
                {
                    if (_imageSize == Size.Empty)
                        rect.X += valueRect.Height + Grid.GlobalTextMargin;
                    else
                        rect.X += _imageSize.Width + Grid.GlobalTextMargin;
                }

                PropertyTypeDescriptorContext context = mOwnerEnumerator.Property.Value.GetTypeDescriptorContext(mOwnerEnumerator);
                if (DrawingDoneByEditor ||
                    ((Feel is PropertyUITypeEditorFeel) && (_uiTypeEditor.GetPaintValueSupported(context))))
                {
                    int delta = UITypeEditorCustomDrawingWidth + 1 + Grid.GlobalTextMargin / 2;
                    rect.X += delta;
                    rect.Width = Size.Round(size).Width;
                }

                return rect;
	        }
        }

        public void DrawValue(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if (Rectangle.Intersect(valueRect, Rectangle.Round(graphics.ClipBounds)).IsEmpty)
                return;

            if (_look != null)
                _look.OnDraw(graphics, valueRect, textColor, propEnum, drawAnotherString);
            else
                OnDraw(graphics, valueRect, textColor, propEnum, drawAnotherString);
        }

        private bool _initDrawingDoneByEditorForReadOnly = true;

        public virtual void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            string stringToDraw = "";
            if (drawAnotherString != null)
                stringToDraw = drawAnotherString;
            else if (HasMultipleValues == false)
                stringToDraw = DisplayString;

            PropertyTypeDescriptorContext context = propEnum.Property.Value.GetTypeDescriptorContext(propEnum);

            if (_initDrawingDoneByEditorForReadOnly)
            {
                if ((Feel == null) && (DrawingDoneByEditor == false))
                {
                    if (ReadOnly && (Grid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.Disabled) &&
                        (Grid.Grid.FindFeel(propEnum.Parent, propEnum) is PropertyUITypeEditorFeel))
                    {
                        if (_uiTypeEditor.GetPaintValueSupported(context))
                            _drawingDoneByEditor = true;
                    }
                }

                _initDrawingDoneByEditorForReadOnly = false;
            }

            if (((Feel is PropertyUITypeEditorFeel) && (_uiTypeEditor.GetPaintValueSupported(context))) ||
                  DrawingDoneByEditor)
            {
                Rectangle drawRect = valueRect;
                drawRect.X += Grid.GlobalTextMargin / 2;
                drawRect.Width = UITypeEditorCustomDrawingWidth;
                drawRect.Y++;
                drawRect.Height -= 2;
                if (drawAnotherString == null)
                {
                    if (HasMultipleValues)
                        _uiTypeEditor.PaintValue(new PaintValueEventArgs(context, null, graphics, drawRect));
                    else
                        _uiTypeEditor.PaintValue(new PaintValueEventArgs(context,
                            propEnum.Property.Value.GetValue(), graphics, drawRect));
                }
                else
                {
                    object valueToDraw = null;
                    try
                    {
                        valueToDraw = propEnum.Property.Value.GetActualValue(stringToDraw);
                    }
                    catch (Exception)
                    {
                        valueToDraw = propEnum.Property.Value.GetValue();
                    }

                    _uiTypeEditor.PaintValue(new PaintValueEventArgs(context, valueToDraw, graphics, drawRect));
                }

                graphics.DrawRectangle(SystemPens.WindowText, drawRect.Left, drawRect.Top, UITypeEditorCustomDrawingWidth - 1, drawRect.Height - 1);

                drawRect.Height++;
                drawRect.X = drawRect.Right + Grid.GlobalTextMargin + 1;
                drawRect.Width = valueRect.Right - drawRect.Left;

                Win32Calls.DrawText(graphics, stringToDraw, ref drawRect, Font, textColor, Win32Calls.DT_VCENTER |
                    Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                    ((mPropCtrl.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));

                return;
            }

            valueRect.Height++;

            Rectangle strRect = GetStringValueRect(graphics, valueRect, Point.Empty /* not used */);
            strRect.Width = valueRect.Right - strRect.Left;

            if (stringToDraw != null)
            {
                Font font;
                string hyperlink = mOwnerEnumerator.Property.HyperLink;
                if (hyperlink.Length != 0)
                    font = new Font(Font, FontStyle.Underline);
                else
                    font = Font;

                // Draw a bitmap if needed
                if ((ImageList != null) && (ImageIndex < ImageList.Images.Count) && (ImageIndex >= 0))
                {
                    if ((_imageSize != Size.Empty) && (_imageSize.Height <= valueRect.Height - 3))
                    {
                        int y = valueRect.Top + (valueRect.Height - 3 - _imageSize.Height) / 2 + 1;
                        Win32Calls.ImageListDraw(_imageList, ImageIndex, graphics,
                            valueRect.Left + Grid.GlobalTextMargin, y,
                            _imageSize.Width, _imageSize.Height);
                    }
                    else
                    {
                        Win32Calls.ImageListDraw(_imageList, ImageIndex, graphics,
                            valueRect.Left + Grid.GlobalTextMargin, valueRect.Top + 1,
							valueRect.Height - 3, valueRect.Height - 3);
                    }
                }

                Win32Calls.DrawText(graphics, stringToDraw, ref strRect, font, textColor, Win32Calls.DT_VCENTER |
                    Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                    ((mPropCtrl.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));

                if (hyperlink.Length != 0)
                    font.Dispose();
            }
        }

		protected TypeConverter mConverter;

        public virtual TypeConverter TypeConverter
        {
            get
            {
                if (mConverter == null)
                {
                    object obj = UnderlyingValue;
                    if (obj == null)
                        mConverter = TypeDescriptor.GetConverter(UnderlyingType);
                    else
                        mConverter = TypeDescriptor.GetConverter(obj);
                }

                return mConverter;
            }
        }

        protected string[] cachedDisplayedValues = null;

        public string[] GetDisplayedValues()
        {
            if ((cachedDisplayedValues == null) || (cachedDisplayedValues.Length == 0))
                ExtractDisplayedValues();

            return cachedDisplayedValues;
        }

        private string[] _standardValues = null;

        Attribute _GetAttribute(Type attributeType)
        {
            ArrayList attrList = (ArrayList)_customAttributes[attributeType];
            if (attrList != null)
                return (Attribute)attrList[0];

            return null;
        }

        public virtual Attribute GetAttribute(Type attributeType)
        {
            return _GetAttribute(attributeType);
        }

        public virtual ArrayList GetAttributes(Type attributeType)
        {
            ArrayList attrList = (ArrayList)_customAttributes[attributeType];
            if (attrList != null)
                return attrList;

            return new ArrayList();
        }

        protected void ClearStandardValues()
        {
            _standardValues = null;
        }

        public string[] GetStandardValues()
        {
            if (_standardValues == null)
            {
                TypeConverter converter = TypeConverter;
                PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);

                if (UnderlyingType.IsEnum)
                {
                    ICollection collection = converter.GetStandardValues(context);
                    if (collection != null)
                    {
                        int i = 0;
                        _standardValues = new string[collection.Count];
                        foreach (object o in collection)
                        {
                            string str = converter.ConvertToString(context, CultureInfo, o);
                            _standardValues[i++] = (str == null ? "" : str);
                        }
                    }
                    else
                        _standardValues = new string[0];
                }
                else
                {
                    ICollection collection = converter.GetStandardValues(context);
                    if (collection != null)
                    {
                        int i = 0;
                        _standardValues = new string[collection.Count];
                        foreach (object o in collection)
                        {
                            string str = converter.ConvertToString(context, CultureInfo, o);
                            _standardValues[i++] = (str == null ? "" : str);
                        }
                    }
                    else
                        _standardValues = new string[0];
                }
            }

            return _standardValues;
        }

        protected Hashtable mActualStrToDisplayedStr;
        protected Hashtable mDisplayedStrToActualStr;

        protected void ExtractDisplayedValues()
        {
            ExtractDisplayedValues(false);
        }

        protected void ExtractDisplayedValues(bool dontTriggerEvent)
        {
            if (mActualStrToDisplayedStr != null)
            {
                mActualStrToDisplayedStr.Clear();
                mDisplayedStrToActualStr.Clear();
                cachedDisplayedValues = null;
            }

            ArrayList cachedDisplayedValueList = new ArrayList();

            string[] displayedValues = new string[0];

            // 1. Try to find if there is a dynamic overriden by the client app at run-time
            if (dontTriggerEvent == false)
            {
                DisplayedValuesNeededEventArgs e = new DisplayedValuesNeededEventArgs(OwnerEnumerator);
                Grid.OnDisplayedValuesNeeded(e);
                if (e.DisplayedValues == null)
                    displayedValues = new string[0];
                else
                    displayedValues = e.DisplayedValues;
            }

            // 2. Try to find a PropertyDisplayedAs attribute on the property itself
            if (displayedValues.Length == 0)
            {
                ArrayList pvdaAttrs = GetAttributes(typeof(PropertyValueDisplayedAsAttribute));
                foreach (PropertyValueDisplayedAsAttribute attr in pvdaAttrs)
                {
                    if (attr.DisplayedStrings != null)
                    {
                        displayedValues = attr.DisplayedStrings;
                        break;
                    }
                }
            }

            // 3. Try to find a PropertyDisplayedAs attribute on the parent of the property
            if ((displayedValues.Length == 0) && (ParentValue != null))
            {
                ArrayList pvdaAttrs = ParentValue.GetAttributes(typeof(PropertyValueDisplayedAsAttribute));
                foreach (PropertyValueDisplayedAsAttribute attr in pvdaAttrs)
                {
                    string[] displayedStrings = attr.GetDisplayedStrings(DisplayName);
                    if (displayedStrings != null)
                    {
                        displayedValues = displayedStrings;
                        break;
                    }
                }
            }

            if (displayedValues.Length != 0)
            {
                if (mActualStrToDisplayedStr == null)
                {
                    mActualStrToDisplayedStr = new Hashtable();
                    mDisplayedStrToActualStr = new Hashtable();
                }

                if (UnderlyingType.IsEnum)
                {
                    int i = 0;

                    FieldInfo[] fis = UnderlyingType.GetFields(BindingFlags.Public | BindingFlags.Static);
#if _DOTNET2
                    // There is a bug in .Net 2.0 that prevents to use ArrayList.Sort (performance reasons)
                    Array.Sort<object>(fis, 0, fis.Length, new FieldInfoEnumComparer(UnderlyingType));
#else
                    Array.Sort(fis, 0, fis.Length, new FieldInfoEnumComparer(UnderlyingType));
#endif
                    foreach (FieldInfo fi in fis)
                    {
                        bool done = false;

                        object[] attrs = fi.GetCustomAttributes(typeof(BrowsableAttribute), false);
                        if ((attrs.Length > 0) && (((BrowsableAttribute)attrs[0]).Browsable == false))
                        {
                            done = true;
                        }

                        if (done == false)
                        {
                            mActualStrToDisplayedStr[fi.Name] = displayedValues[i];
                            mDisplayedStrToActualStr[displayedValues[i]] = fi.Name;
                            cachedDisplayedValueList.Add(displayedValues[i]);
                        }

                        i++;
                    }
                }
                else if (GetStandardValues().Length > 0)
                {
                    int i = 0;
                    foreach (string stdValue in GetStandardValues())
                    {
                        mActualStrToDisplayedStr[stdValue] = displayedValues[i];
                        mDisplayedStrToActualStr[displayedValues[i]] = stdValue;
                        cachedDisplayedValueList.Add(displayedValues[i]);

                        i++;
                    }
                }
                else
                {
                    foreach (string displayedValue in displayedValues)
                    {
                        mActualStrToDisplayedStr[displayedValue] = displayedValue;
                        mDisplayedStrToActualStr[displayedValue] = displayedValue;
                        cachedDisplayedValueList.Add(displayedValue);
                    }
                }
            }
            else
            {
                object[] collection = GetStandardValues();

                if (collection.Length > 0)
                {
                    if (mActualStrToDisplayedStr == null)
                    {
                        mActualStrToDisplayedStr = new Hashtable();
                        mDisplayedStrToActualStr = new Hashtable();
                    }

                    if (UnderlyingType.IsEnum)
                    {
                        FieldInfo[] fis = UnderlyingType.GetFields(BindingFlags.Public | BindingFlags.Static);
#if _DOTNET2
                        // There is a bug in .Net 2.0 that prevents to use ArrayList.Sort (performance reasons)
                        Array.Sort<object>(fis, 0, fis.Length, new FieldInfoEnumComparer(UnderlyingType));
#else
                        Array.Sort(fis, 0, fis.Length, new FieldInfoEnumComparer(UnderlyingType));
#endif
                        IEnumerator stdValuesEnumerator = collection.GetEnumerator();
                        stdValuesEnumerator.MoveNext();
                        foreach (FieldInfo fi in fis)
                        {
                            string stdValue = stdValuesEnumerator.Current as string;

                            // Some TypeConverters remove some values in the enum (like GraphicsUnit in a Font)
                            // so we must check
                            PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
                            if (fi.Name != TypeConverter.ConvertFromString(context, CultureInfo, stdValue).ToString())
                                continue;

                            // If the field is not browsable, we continue to the next one
                            object[] attrs = fi.GetCustomAttributes(typeof(BrowsableAttribute), false);
                            if ((attrs.Length > 0) && (((BrowsableAttribute)attrs[0]).Browsable == false))
                            {
                                stdValuesEnumerator.MoveNext();
                                continue;
                            }

                            attrs = fi.GetCustomAttributes(typeof(PropertyValueDisplayedAsAttribute), false);
                            if (attrs.Length > 0)
                            {
                                string displayStr = ((PropertyValueDisplayedAsAttribute)attrs[0]).DisplayedStrings[0];
                                mDisplayedStrToActualStr[displayStr] = stdValue;
                                mActualStrToDisplayedStr[stdValue] = displayStr;
                                cachedDisplayedValueList.Add(displayStr);
                            }
                            else
                            {
                                cachedDisplayedValueList.Add(stdValue);
                                mActualStrToDisplayedStr[stdValue] = stdValue;
                                mDisplayedStrToActualStr[stdValue] = stdValue;
                            }

                            if (stdValuesEnumerator.MoveNext() == false)
                                break;
                        }
                    }
                    else
                    {
                        foreach (string stdValue in collection)
                        {
                            cachedDisplayedValueList.Add(stdValue);
                            mActualStrToDisplayedStr[stdValue] = stdValue;
                            mDisplayedStrToActualStr[stdValue] = stdValue;
                        }
                    }
                }
            }

            cachedDisplayedValues = new string[cachedDisplayedValueList.Count];
            cachedDisplayedValueList.CopyTo(cachedDisplayedValues, 0);

            if (dontTriggerEvent == false)
            {
                if (_look != null)
                    _look.ControlHeightMultiplier();
            }
        }

        /// <summary>
        /// Clears any previously associated set of displayed strings for this value and reassociates a
        /// new set of strings.
        /// </summary>
        /// <param name="displayedValues">The array of new displayed strings for this value.</param>
        public void ResetDisplayedValues(string[] displayedValues)
        {
            PropertyValueDisplayedAsAttribute attr = new PropertyValueDisplayedAsAttribute(displayedValues);
            SetSingleAttribute(attr);
            ClearStandardValues();

            ExtractDisplayedValues(true);

            if (_look != null)
                _look.ControlHeightMultiplier();
        }

        /// <summary>
        /// Clears any previously associated set of displayed strings for this value.
        /// </summary>
        /// <remarks>
        /// If you don't further associate new displayed strings, the value will use the standard values.
        /// For example, an enumeration will use the strings provided in the enumeration fields.
        /// </remarks>
        public void ResetDisplayedValues(bool triggerEvent)
        {
            DeleteSingleAttribute(new PropertyValueDisplayedAsAttribute());

            ClearStandardValues();

            if (triggerEvent)
            {
                ExtractDisplayedValues();
            }
            else
            {
                if (mActualStrToDisplayedStr != null)
                {
                    mActualStrToDisplayedStr.Clear();
                    mDisplayedStrToActualStr.Clear();
                    cachedDisplayedValues = null;
                }
            }
        }

        public object GetValue()
        {
            return UnderlyingValue;
        }

        public object GetValueToValidate(string valueToValidateAsStr)
        {
            return GetActualValue(valueToValidateAsStr);
        }

        public object GetActualValue(string drawAnotherString)
        {
//            try
            {
                PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
                if (TypeConverter.CanConvertFrom(context, typeof(string)))
                {
                    return TypeConverter.ConvertFromString(context, CultureInfo,
                    (drawAnotherString == null ? DisplayString : GetActualString(drawAnotherString)));
                }
//                else if (drawAnotherString != null)
  //                  return drawAnotherString;
                else
                    return UnderlyingValue;
            }
/*            catch (Exception)
            {
                return UnderlyingValue;
            }*/
        }

        public virtual object TargetInstance
        {
            get
            {
                return null;
            }
        }

        public virtual object[] TargetInstances
        {
            get
            {
                return null;
            }
        }

        public virtual PropertyDescriptor PropertyDescriptor
        {
            get
            {
                return null;
            }
        }

        public bool ReadOnlyByTypeConverter
        {
            get
            {
                if (TypeConverter != null)
                {
                    PropertyTypeDescriptorContext context = GetTypeDescriptorContext(mOwnerEnumerator);
                    if (TypeConverter.CanConvertFrom(context, typeof(string)) == false)
                        return true;

                    if (TypeConverter.GetStandardValuesExclusive(context))
                        return true;
                }

                // TODO : on pourrait inventer un attribute [ReadOnlyTextbox] qui permettrait d'avoir juste
                // le textbox readonly mais pas le listbox par exemple. Ca eviterait d'avoir a ecrire un
                // TypeConverter juste pour qu'il ramene GetStandardValuesExclusive()==true.
                
                return false;
            }
        }
        
        public virtual bool IsReadOnly(PropertyEnumerator selfEnum)
        {
            return false;
        }

        public virtual string GetTooltipString(int line)
        {
	        if (Look != null)
                return Look.GetTooltipString(line);
	        else
                return GetDisplayString(GetStringValue());
        }

        public virtual Size GetTooltipStringSize(Graphics graphics, Rectangle valueRect, Point point, int line)
        {
            if (Look != null)
                return Look.GetTooltipStringSize(graphics, valueRect, point, line);
            else
                return Win32Calls.GetTextExtent(graphics, DisplayString, Font);
        }

        public virtual PropertyTypeDescriptorContext GetTypeDescriptorContext(PropertyEnumerator propEnum)
        {
            return null;
        }

        protected virtual void OnParentValueChanged(PropertyValue parentPropValue)
        {
        }

        protected internal virtual void OnChildValueChanged(PropertyValue childPropValue)
        {
        }

        protected virtual void ReUpdateFromChildren()
        {
        }

        public virtual void Recreate(bool sendPropertyCreatedEvent)
        {
        }

        protected internal virtual void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            if (PropertyChangedEvent != null)
                PropertyChangedEvent(this, e);
        }
    }
}
