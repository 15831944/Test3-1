// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System;
using System.Text;
using System.Reflection;
using System.ComponentModel;
using System.Collections;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <exclude />
    public class PropertyValueManaged : PropertyValue
    {
        // This PRopertyDescriptor is of no use to SPG but it is necessary to have it because of some
        // UITypeEditors that request it. If the PropertyDescriptorContext doesn't contain a reference to
        // this descriptor, then it raise an exception in the editor code 
        class ManagedPropertyDescriptor : PropertyDescriptor
        {
            private PropertyValueManaged _propertyValue;

            public ManagedPropertyDescriptor(PropertyValueManaged propertyValue, Attribute[] attributes)
                : base("internal", attributes)
            {
                _propertyValue = propertyValue;
            }

            public override Type ComponentType
            {
                get
                {
                    return _propertyValue.GetType();
                }
            }

            public override bool IsReadOnly
            {
                get
                {
                    return _propertyValue.ReadOnly;
                }
            }

            public override Type PropertyType
            {
                get
                {
                    return _propertyValue.UnderlyingType;
                }
            }

            public override bool CanResetValue(object component)
            {
                return false;
            }

            public override void ResetValue(object component) { }

            public override bool ShouldSerializeValue(object component)
            {
                return true;
            }

            // Not used
            public override void SetValue(object component, object value)
            {
                _propertyValue.SetValueInternal(value, ValueUpdateSource.FromOutside);
                OnValueChanged(component, EventArgs.Empty);
            }

            // Not used
            public override object GetValue(object component)
            {
                return _propertyValue.GetValue();
            }
        }

        private ManagedPropertyDescriptor _propertyDescriptor;

        public override Type UnderlyingType
        {
            get { return mUnderlyingType; }
        }

        public PropertyValueManaged(int id, Type valueType, object initialValue, Attribute[] attributes)
        {
            if ((initialValue != null) && (initialValue != DBNull.Value) && !valueType.IsInstanceOfType(initialValue))
                throw new Exception("The initial value supplied to create the property has the wrong type. It should be of type " + valueType);

            mUnderlyingValue = initialValue;

            mUnderlyingType = valueType;

            Attributes = attributes;

            _propertyDescriptor = new ManagedPropertyDescriptor(this, attributes);
        }

        public override TypeConverter TypeConverter
        {
            get
            {
                if (mConverter == null)
                {
                    TypeConverterAttribute attrib = (TypeConverterAttribute)GetAttribute(typeof(TypeConverterAttribute));
                    if ((attrib != null) && (attrib.ConverterTypeName.Length > 0))
                    {
                        Type[] typeArray = new Type[] { typeof(Type) };
                        Type converterType = Type.GetType(attrib.ConverterTypeName);
                        if (converterType.GetConstructor(typeArray) != null)
                            return (TypeConverter)Activator.CreateInstance(converterType, new object[] { UnderlyingType });
                        else
                            return (TypeConverter)Activator.CreateInstance(converterType, false);
                    }
                    else
                        return TypeDescriptor.GetConverter(UnderlyingType);
                }

                return mConverter;
            }
        }

        protected override bool SetValueInternal(object value, ValueUpdateSource updateSource)
        {
            bool result = false;

#if _DOTNET2
            NullableConverter nc = TypeConverter as NullableConverter;
            if ((value == null) || ((value != null) && UnderlyingType.IsInstanceOfType(value)) ||
                ((nc != null) && ((value == null) || nc.UnderlyingType.IsInstanceOfType(value))))
#else
            if ((value != null) && UnderlyingType.IsInstanceOfType(value))
#endif
            {
                UnderlyingValue = value;
                result = true;
            }

            if (updateSource != ValueUpdateSource.FromChild)
            {
                if ((mOwnerEnumerator != null) &&   // mOwnerEnumerator could be null for the boolean of Property._enabledVariable
                    !mOwnerEnumerator.Property.Value.NoLinkWithChildren)
                {
                    PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
                    if (TypeConverter.GetPropertiesSupported(context) == false)
                    {
                        PropertyEnumerator childEnum = mOwnerEnumerator.Children;
                        if (childEnum.Count > 0)
                        {
                            string masterStr = mOwnerEnumerator.Property.Value.GetStringValue();

                            char separator = mOwnerEnumerator.Property.Value.GroupedValueSeparator;
                            while (childEnum != childEnum.RightBound)
                            {
                                masterStr.TrimStart(null);

                                int count = masterStr.IndexOf(separator);

                                if (count != -1)
                                {
                                    childEnum.Property.Value.SetValue(masterStr.Substring(0, count));
                                    if (count + 2 < masterStr.Length)
                                        masterStr = masterStr.Substring(count + 2); // advance after the separator and the leading space
                                }
                                else
                                {
                                    childEnum.Property.Value.SetValue(masterStr);
                                    break;
                                }

                                childEnum.MoveNext();
                            }
                        }
                    }
                }
            }

            return result;
        }

        protected internal override bool ValidateSelfValueFromModifiedChild(PropertyEnumerator modifiedChildEnum, object value)
        {
            if (NoLinkWithChildren)
                return true;

            if (Validator != null)
            {
                string str = "";
                PropertyEnumerator selfEnum = modifiedChildEnum.Parent;
                PropertyEnumerator childEnum = selfEnum.Children;
                while (childEnum != childEnum.RightBound)
                {
                    if (childEnum.Equals(modifiedChildEnum))
                    {
                        if (value is string)
                            str += (value as string);
                        else
                        {
                            PropertyTypeDescriptorContext context = childEnum.Property.Value.GetTypeDescriptorContext(childEnum);
                            str += (string)TypeDescriptor.GetConverter(value).ConvertTo(context,
                                childEnum.Property.Value.CultureInfo, value, typeof(string));
                        }
                    }
                    else
                        str += childEnum.Property.Value.GetStringValue();
                    childEnum.MoveNext();

                    if (childEnum != childEnum.RightBound)
                    {
                        str += GroupedValueSeparator;
                        str += " ";
                    }
                }

                if (Validator.Check(str, false) == false)
                    return false;
            }

            return true;
        }

        public override bool ValidateValue(object value, out PropertyEnumerator invalidPropertyEnum)
        {
            invalidPropertyEnum = Grid.RightBound;

            // Validate the underlying value
            if (Validator != null)
            {
                if (Validator.Check(value, false) == false)
                {
                    invalidPropertyEnum = OwnerEnumerator;
                    return false;
                }
            }

            if (mOwnerEnumerator == null)
                return true;

            if ((NoLinkWithChildren == false) && !TypeConverter.GetPropertiesSupported())
            {
                // Validate children if any
                PropertyEnumerator childEnum = mOwnerEnumerator.Children;
                if (childEnum.Count > 0)
                {
                    string masterStr = (value as string);

                    char separator = mOwnerEnumerator.Property.Value.GroupedValueSeparator;
                    while (childEnum != childEnum.RightBound)
                    {
                        PropertyValidatorBase validator = childEnum.Property.Value.Validator;

                        masterStr.TrimStart(null);

                        int count = masterStr.IndexOf(separator);

                        if (count != -1)
                        {
                            if (validator != null)
                            {
                                object valueToCheck = childEnum.Property.Value.TypeConverter.ConvertFrom(
                                    GetTypeDescriptorContext(childEnum),
                                    childEnum.Property.Value.CultureInfo,
                                    masterStr.Substring(0, count));

                                if (validator.Check(valueToCheck, false) == false)
                                {
                                    invalidPropertyEnum = childEnum;
                                    return false;
                                }
                            }

                            if (count + 2 < masterStr.Length)
                                masterStr = masterStr.Substring(count + 2); // advance after the separator and the leading space

                            childEnum.MoveNext();
                        }
                        else
                        {
                            if (validator != null)
                            {
                                object valueToCheck = childEnum.Property.Value.TypeConverter.ConvertFrom(
                                    GetTypeDescriptorContext(childEnum),
                                    childEnum.Property.Value.CultureInfo,
                                    masterStr);

                                if (validator.Check(valueToCheck, false) == false)
                                {
                                    invalidPropertyEnum = childEnum;
                                    return false;
                                }
                            }

                            break;
                        }
                    }
                }
            }

            // Validate parent if any
            if (mOwnerEnumerator != null)
            {
                PropertyEnumerator parentEnum = mOwnerEnumerator.Parent;
                if ((parentEnum.Property != null) && (parentEnum.Property.Value != null))
                {
                    bool valid = parentEnum.Property.Value.ValidateSelfValueFromModifiedChild(mOwnerEnumerator, value);
                    if (!valid)
                        invalidPropertyEnum = parentEnum;
                    return valid;
                }
            }
/*
            PropertyEnumerator parentEnum = mOwnerEnumerator.Parent;
            if ((parentEnum.Property != null) && (parentEnum.Property.Value != null) &&
                (parentEnum.Property.Value.NoLinkWithChildren == false))
            {
                string str = "";
                PropertyEnumerator childEnum = parentEnum.Children;
                char separator = parentEnum.Property.Value.GroupedValueSeparator;
                while (childEnum != childEnum.RightBound)
                {
                    if (childEnum.Property.Value == this)
                        str += (value as string);
                    else
                        str += childEnum.Property.Value.GetStringValue();
                    childEnum.MoveNext();

                    if (childEnum != childEnum.RightBound)
                    {
                        str += separator;
                        str += " ";
                    }
                }

                PropertyValidatorBase validator = parentEnum.Property.Value.Validator;
                if ((validator != null) && (validator.Check(str, false) == false))
                    return false;
            }*/

            return true;
        }

        protected override void ReUpdateFromChildren()
        {
            OnChildValueChanged(null);
        }

        public override string GetStringValue()
        {
            string str = TypeConverter.ConvertToString(GetTypeDescriptorContext(OwnerEnumerator), CultureInfo, UnderlyingValue);
            return (str == null ? "" : str);
        }

        public override string GetDisplayString(string actualStr)
        {
            if (mActualStrToDisplayedStr != null)
            {
                string str = null;
                if (actualStr != null)
                    str = (string)mActualStrToDisplayedStr[actualStr];

                return (str != null ? str : actualStr);
            }
            else
                return actualStr;
        }
/*
        public override string GetDisplayString(string actualStr)
        {
            if ((mActualStrToDisplayedStr != null) && (mActualStrToDisplayedStr.Count > 0))
            {
                string str;
                if (UnderlyingType.IsEnum && (Attribute.GetCustomAttribute(UnderlyingType, typeof(FlagsAttribute)) != null))
                {
                    Array enumValues = Enum.GetValues(UnderlyingType);

                    Enum enumValue;
                    enumValue = (Enum)(UnderlyingValue);

                    StringBuilder builder = new StringBuilder();
                    for (int i = 0; i < enumValues.Length; i++)
                    {
                        if ((enumValue.GetHashCode() & enumValues.GetValue(i).GetHashCode()) != 0)
                        {
                            string dispStr = (string)mActualStrToDisplayedStr[enumValues.GetValue(i).ToString()];
                            if (dispStr != null)
                            {
                                builder.Append((string)mActualStrToDisplayedStr[enumValues.GetValue(i).ToString()]);
                                builder.Append(", ");
                            }
                        }
                    }

                    if (builder.Length > 0)
                    {
                        builder.Remove(builder.Length - 2, 2);
                        str = builder.ToString();
                    }
                    else
                        str = null;
                }
                else
                    str = (string)mActualStrToDisplayedStr[actualStr];

                return (str != null ? str : actualStr);
            }
            else
                return actualStr;
        }
        */
        public override Attribute GetAttribute(Type attributeType)
        {
            Attribute attr = base.GetAttribute(attributeType);
            if (attr != null)
                return attr;
/*
            attr = Attribute.GetCustomAttribute(_memberInfo, attributeType);
            if (attr != null)
                return attr;
            */
            return TypeDescriptor.GetAttributes(UnderlyingType)[attributeType];
        }

        public override ArrayList GetAttributes(Type attributeType)
        {
            ArrayList attrList = base.GetAttributes(attributeType);
            
            if (attrList.Count == 0)
            {
                AttributeUsageAttribute attrUsage = (AttributeUsageAttribute)TypeDescriptor.GetAttributes(attributeType)
                    [typeof(AttributeUsageAttribute)];
                if (attrUsage.AllowMultiple)
                {
                    foreach (Attribute attr in PropertyDescriptor.Attributes)
                    {
                        if (attr.GetType() == attributeType)
                            attrList.Add(attr);
                    }
                }

                if (attrList.Count == 0)
                {
                    // Try to find the attribute on the Type itself
                    Attribute attr = TypeDescriptor.GetAttributes(UnderlyingType)[attributeType];
                    if (attr != null)
                        attrList.Add(attr);
                }
            }

            return attrList;
        }

        protected internal override void OnChildValueChanged(PropertyValue childPropValue)
        {
            if ((childPropValue == null) || !childPropValue.ChildValueCreatedBySPG)
            {
                string str = "";

                PropertyEnumerator childEnum = mOwnerEnumerator.Children;
                char separator = mOwnerEnumerator.Property.Value.GroupedValueSeparator;
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

                SetValue(str, ValueUpdateSource.FromChild);
            }
        }

        public override bool IsReadOnly(PropertyEnumerator selfEnum)
        {
            if (!Grid.Grid.Enabled && (Grid.DisableMode == PropertyGrid.DisableModes.Simple))
                return true;

            if ((selfEnum.Parent.Property != null) && (selfEnum.Parent.Property.Value != null))
            {
                if (!selfEnum.Parent.Property.Value.NoLinkWithChildren &&
                    (selfEnum.Parent.Property.Value.IsReadOnly(selfEnum.Parent)))
                    return true;
            }

            // Check if a UITypeEditor has been found. Even if the property is marked Readonly (has only a get
            // accessor for example), the presence of an editor makes it editable (this is the case for collections
            // for example in the .net framework)
            ReadOnlyAttribute manualReadOnlyAttr = (ReadOnlyAttribute)base.GetAttribute(typeof(ReadOnlyAttribute));
            if ((mFeel is PropertyUITypeEditorFeel) && ((manualReadOnlyAttr == null) || !manualReadOnlyAttr.IsReadOnly))
                return false;

            ReadOnlyAttribute attr = (ReadOnlyAttribute)GetAttribute(typeof(ReadOnlyAttribute));
            return ((attr != null) && attr.IsReadOnly);
        }

        // see note about ManagedPropertyDescriptor
        public override PropertyDescriptor PropertyDescriptor
        {
            get { return _propertyDescriptor; }
        }

        // see note about ManagedPropertyDescriptor
        public override PropertyTypeDescriptorContext GetTypeDescriptorContext(PropertyEnumerator propEnum)
        {
            return new PropertyTypeDescriptorContext(PropertyDescriptor, this, propEnum, Grid);
        }
    }
}
