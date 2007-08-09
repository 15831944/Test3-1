// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2005 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.ComponentModel;

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

        /// <summary>
        /// Stores information about a user C# Property or field representing the underlying data of this property.
        /// </summary>
        private MemberInfo _memberInfo;

        internal MemberInfo MemberInfo
        {
            get { return _memberInfo; }
        }

        /// <summary>
        /// Stores a reference to a class instance where the underlying data is found and accessible
        /// by a field or C# property.
        /// </summary>
        private object _targetInstance;

        public override object TargetInstance
        {
            get { return _targetInstance; }
        }

        public override Type UnderlyingType
        {
            get { return mUnderlyingType; }
        }

        public PropertyValueManaged(int id, Type valueType, object defaultValue, Attribute[] attributes)
        {
            if ((defaultValue != null) && (defaultValue.GetType() != valueType))
                throw new Exception("The initial value supplied to create the property has the wrong type. It should be of type " + valueType);

            mUnderlyingValue = defaultValue;

            _targetInstance = this;
            _memberInfo = _targetInstance.GetType().GetProperty("UnderlyingValue", BindingFlags.Instance | BindingFlags.NonPublic);
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
                        return (TypeConverter)Activator.CreateInstance(Type.GetType(attrib.ConverterTypeName), false);
                    else
                        return TypeDescriptor.GetConverter(UnderlyingType);
                }

                return mConverter;
            }
        }

        protected override bool SetValueInternal(object value, ValueUpdateSource updateSource)
        {
            bool result = true;

            // If the user tries to set the value as a string
            if ((value != null) && (value.GetType() == typeof(string)))
            {
                if (UnderlyingType == typeof(string))
                {
                    try
                    {
                        // We set the value directly to the underlying data (to its C# Field or Property)
                        if (_memberInfo.MemberType == MemberTypes.Property)
                            ((PropertyInfo)_memberInfo).GetSetMethod(true).Invoke(_targetInstance, new object[] { value });
                        else
                            ((FieldInfo)_memberInfo).SetValue(_targetInstance, value);
                    }
                    catch (ArgumentException)
                    {
                        result = false;
                    }
                }
                else
                {
                    try
                    {
                        string newString = GetActualString(value as string);
                        if (newString != null)
                        {

                            if (_memberInfo.MemberType == MemberTypes.Property)
                                ((PropertyInfo)_memberInfo).GetSetMethod(true).Invoke(_targetInstance, new object[] {
                                    TypeConverter.ConvertFromString(GetTypeDescriptorContext(OwnerEnumerator),
                                    CultureInfo, newString) });
                            else
                                ((FieldInfo)_memberInfo).SetValue(_targetInstance, TypeConverter.ConvertFromString(
                                    GetTypeDescriptorContext(OwnerEnumerator), CultureInfo, newString));
                        }
                        else
                        {
                            if (_memberInfo.MemberType == MemberTypes.Property)
                                ((PropertyInfo)_memberInfo).GetSetMethod(true).Invoke(_targetInstance, new object[] {
                                    TypeConverter.ConvertFromString(GetTypeDescriptorContext(OwnerEnumerator),
                                    CultureInfo, value as string) });
                            else
                                ((FieldInfo)_memberInfo).SetValue(_targetInstance, TypeConverter.ConvertFromString(
                                    GetTypeDescriptorContext(OwnerEnumerator), CultureInfo, value as string));
                        }
                    }
                    catch (Exception)
                    {
                        result = false;
                    }
                }
            }

            // Else if the user doesn't try to set the value as a string
            else
            {
                try
                {
                    // We set the value directly to the underlying data (to its C# Field or Property)
                    if (_memberInfo.MemberType == MemberTypes.Property)
                        ((PropertyInfo)_memberInfo).GetSetMethod(true).Invoke(_targetInstance, new object[] { value });
                    else
                        ((FieldInfo)_memberInfo).SetValue(_targetInstance, value);
                }
                catch (ArgumentException)
                {
                    result = false;
                }
            }

            if (updateSource != ValueUpdateSource.FromChild)
            {
                if (mOwnerEnumerator != null)   // mOwnerEnumerator could be null for the boolean of Property._enabledVariable
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

            return result;
        }

        protected override bool ValidateValue(object value)
        {
            // Validate the underlying value
            if (Validator != null)
            {
                if (Validator.Check(value, false) == false)
                    return false;
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
                            if ((validator != null) && (validator.Check(masterStr.Substring(0, count), false) == false))
                                return false;

                            if (count + 2 < masterStr.Length)
                                masterStr = masterStr.Substring(count + 2); // advance after the separator and the leading space

                            childEnum.MoveNext();
                        }
                        else
                        {
                            if ((validator != null) && (validator.Check(masterStr, false) == false))
                                return false;

                            break;
                        }
                    }
                }
            }

            // Validate parent if any
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
            }

            return true;
        }

        protected override void ReUpdateFromChildren()
        {
            OnChildValueChanged(null);
        }

        public override string GetStringValue()
        {
            TypeConverter converter = TypeConverter;

            if (_memberInfo.MemberType == MemberTypes.Property)
                return converter.ConvertToString(GetTypeDescriptorContext(OwnerEnumerator), CultureInfo,
                    ((PropertyInfo)_memberInfo).GetGetMethod(true).Invoke(_targetInstance, new object[] { }));
            else
                return converter.ConvertToString(GetTypeDescriptorContext(OwnerEnumerator), CultureInfo,
                    ((FieldInfo)_memberInfo).GetValue(_targetInstance));
        }

        protected internal override string GetDisplayString(string actualStr)
        {
            if ((mActualStrToDisplayedStr != null) && (mActualStrToDisplayedStr.Count > 0))
            {
                string str;
                if (UnderlyingType.IsEnum && (Attribute.GetCustomAttribute(UnderlyingType, typeof(FlagsAttribute)) != null))
                {
                    Array enumValues = Enum.GetValues(UnderlyingType);

                    Enum enumValue;
                    if (_memberInfo.MemberType == MemberTypes.Property)
                        enumValue = (Enum)((PropertyInfo)_memberInfo).GetGetMethod(true).Invoke(_targetInstance, new object[] {});
                    else
                        enumValue = (Enum)((FieldInfo)_memberInfo).GetValue(_targetInstance);

                    StringBuilder builder = new StringBuilder();
                    for (int i = 0; i < enumValues.Length; i++)
                    {
                        if ((enumValue.GetHashCode() & enumValues.GetValue(i).GetHashCode()) != 0)
                        {
                            builder.Append((string)mActualStrToDisplayedStr[enumValues.GetValue(i).ToString()]);
                            builder.Append(", ");
                        }
                    }

                    if (builder.Length > 0)
                    {
                        builder.Remove(builder.Length - 2, 2);
                        str = builder.ToString();
                    }
                    else
                        str = "";
                }
                else
                    str = (string)mActualStrToDisplayedStr[actualStr];

                return (str != null ? str : actualStr);
            }
            else
                return actualStr;
        }

        public override Attribute GetAttribute(Type attributeType)
        {
            Attribute attr = base.GetAttribute(attributeType);
            if (attr != null)
                return attr;

            attr = Attribute.GetCustomAttribute(_memberInfo, attributeType);
            if (attr != null)
                return attr;

            return TypeDescriptor.GetAttributes(UnderlyingType)[attributeType];
        }

        public override List<Attribute> GetAttributes(Type attributeType)
        {
            List<Attribute> attrList = base.GetAttributes(attributeType);

            if (attrList.Count == 0)
            {
                AttributeUsageAttribute attrUsage = (AttributeUsageAttribute)TypeDescriptor.GetAttributes(attributeType)
                    [typeof(AttributeUsageAttribute)];
                if (attrUsage.AllowMultiple)
                {
                    foreach (Attribute attr in Attribute.GetCustomAttributes(_memberInfo, attributeType))
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

        protected internal override void OnChildValueChanged(PropertyValue parentPropValue)
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

        public override bool IsReadOnly(PropertyEnumerator selfEnum)
        {
            if (!Grid.Grid.Enabled && ((Grid.DisableMode & PropertyGrid.DisableModes.Simple) != 0))
                return true;

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
            return new PropertyTypeDescriptorContext(PropertyDescriptor, _targetInstance, propEnum);
        }
    }
}
