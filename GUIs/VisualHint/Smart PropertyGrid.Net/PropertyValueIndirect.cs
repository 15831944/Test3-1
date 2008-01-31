// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System;
using System.ComponentModel;
using System.Collections;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <exclude />
    public class PropertyValueIndirect : PropertyValue
    {
        private ArrayList _targets = new ArrayList(1); // KeyValuePair<PropertyDescriptor, object>
//        private List<KeyValuePair<PropertyDescriptor, object>> _targets = new List<KeyValuePair<PropertyDescriptor,object>>(1);

        public override object TargetInstance
        {
            get
            {
                if (((Utilities.KeyValuePair)_targets[0]).Value == null)
                    return ParentValue.UnderlyingValue;
                else
                    return GetTargetInstance(((Utilities.KeyValuePair)_targets[0]).Value,
                        (PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key);
            }
        }

        public override object[] TargetInstances
        {
            get
            {
                if (_targets.Count == 1)
                    return new object[1] { TargetInstance };
                else
                {
                    ArrayList targets = new ArrayList();
                    foreach (Utilities.KeyValuePair pair in _targets)
                    {
                        object target = pair.Value;
                        PropertyDescriptor pd = (PropertyDescriptor)pair.Key;
                        targets.Add(GetTargetInstance(target, pd));
                    }

                    return targets.ToArray();
                }
            }
        }

        private object GetTargetInstance(object supposedTarget, PropertyDescriptor pd)
        {
            object target = supposedTarget;
            if (target is ICustomTypeDescriptor)
                target = ((ICustomTypeDescriptor)target).GetPropertyOwner(pd);

            return target;
        }

        public override PropertyDescriptor PropertyDescriptor
        {
            get
            {
                return (PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key;
            }
        }

        public PropertyValueIndirect(PropertyGrid grid, PropertyValue parentValue, object target, PropertyDescriptor propertyDescriptor, Attribute[] attributes)
            : base(parentValue)
        {
            Grid = grid;

            _targets.Add(new Utilities.KeyValuePair(propertyDescriptor, target));

            Attributes = attributes;

//            ExtractDisplayedValues();
        }

        public PropertyValueIndirect(PropertyGrid grid, PropertyValue parentValue, PropertyDescriptor propertyDescriptor)
            : base(parentValue)
        {
            Grid = grid;

            _targets.Add(new Utilities.KeyValuePair(propertyDescriptor, null));

//            ExtractDisplayedValues();
        }

        public override bool HasMultipleValues
        {
            get
            {
                object value1 = null;
                foreach (Utilities.KeyValuePair pair in _targets)
                {
                    object target = GetTargetInstance(pair.Value, (PropertyDescriptor)pair.Key);

                    if (value1 == null)
                    {
                        try
                        {
                            value1 = ((PropertyDescriptor)pair.Key).GetValue(target);
                        }
                        catch (Exception)
                        {
                        }
                    }
                    else
                    {
                        if (value1.Equals(((PropertyDescriptor)pair.Key).GetValue(target)) == false)
                            return true;
                    }
                }

                return false;
            }
        }

        internal override object UnderlyingValue
        {
            get
            {
                return ((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).GetValue(TargetInstance);
            }
        }

        public override Type UnderlyingType
        {
            get
            {
                if (mUnderlyingType == null)
                {
                    mUnderlyingType = ((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).PropertyType;
                    if ((mUnderlyingType == typeof(object)) && (UnderlyingValue != null))
                        mUnderlyingType = UnderlyingValue.GetType();
                }

                return mUnderlyingType;
            }
        }

        public override TypeConverter TypeConverter
        {
            get
            {
                if (mConverter == null)
                {
                    try
                    {
                        TypeConverterAttribute attrib = (TypeConverterAttribute)GetAttribute(typeof(TypeConverterAttribute));
                        if ((attrib != null) && (attrib.ConverterTypeName.Length > 0))
                        {
                            Type[] typeArray = new Type[] { typeof(Type) };
                            Type converterType = Type.GetType(attrib.ConverterTypeName);
                            if (converterType.GetConstructor(typeArray) != null)
                                mConverter = (TypeConverter)Activator.CreateInstance(converterType, new object[] { UnderlyingType });
                            else
                                mConverter = (TypeConverter)Activator.CreateInstance(converterType, false);
                        }
                    }
                    catch (Exception)
                    {
                    }

                    if (mConverter == null)
                        mConverter = ((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).Converter;
                }

                return mConverter;
            }
        }

        public override string DisplayName
        {
            get { return ((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).DisplayName; }
        }

        public override string Name
        {
            get { return ((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).Name; }
        }

        private bool SetMultipleValues(object value)
        {
            foreach (Utilities.KeyValuePair pair in _targets)
            {
                ((PropertyDescriptor)pair.Key).SetValue(
                    GetTargetInstance(pair.Value, (PropertyDescriptor)pair.Key), value);
            }

            return true;
        }

        protected internal override bool ValidateSelfValueFromModifiedChild(PropertyEnumerator modifiedChildEnum, object value)
        {
            if (Validator != null)
            {
                if (modifiedChildEnum.Property.Value is PropertyValueSimulated)
                {
                    try
                    {
                        PropertyTypeDescriptorContext context = GetTypeDescriptorContext(modifiedChildEnum);
                        PropertyDescriptorCollection collection = TypeConverter.GetProperties(context, GetValue());
                        PropertyValue childValue = modifiedChildEnum.Property.Value;

                        if (collection != null)
                        {
                            IDictionary dictionary = new Hashtable(collection.Count);
                            foreach (PropertyDescriptor propertyDescriptor in collection)
                            {
                                if (modifiedChildEnum.Property.Name.Equals(propertyDescriptor.Name))
                                {
                                    if (value is string)
                                    {
                                        dictionary[propertyDescriptor.Name] = childValue.TypeConverter.ConvertFromString(
                                            context, childValue.CultureInfo, childValue.GetActualString(value as string));
                                    }
                                    else
                                        dictionary[propertyDescriptor.Name] = value;
                                }
                                else
                                    dictionary[propertyDescriptor.Name] = propertyDescriptor.GetValue(childValue.TargetInstance);
                            }

                            // Create the parent value from all the sibling values
                            object newParentValue = TypeConverter.CreateInstance(context, dictionary);

                            // Validate the parent value
                            PropertyEnumerator invalidPropEnum;
                            if (ValidateValue(newParentValue, out invalidPropEnum))
                                return true;
                        }
                    }
                    catch (Exception)
                    {
                        return false;
                    }

                    return true;
                }
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
                    invalidPropertyEnum = mOwnerEnumerator;
                    return false;
                }
            }

            // Validate children if any
            if (mOwnerEnumerator != null)
            {
                PropertyEnumerator childEnum = mOwnerEnumerator.Children;
                while(childEnum != childEnum.RightBound)
                {
                    if (childEnum.Property.Value != null)
                    {
                        PropertyValidatorBase validator = childEnum.Property.Value.Validator;
                        if (validator != null)
                        {
                            object childValue = childEnum.Property.Value.PropertyDescriptor.GetValue(value);
                            if (validator.Check(childValue, false) == false)
                            {
                                invalidPropertyEnum = childEnum;
                                return false;
                            }
                        }
                    }

                    childEnum.MoveNext();
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

            return true;
        }

        protected override bool SetValueInternal(object value, ValueUpdateSource updateSource)
        {
            bool result;

            // Update the new value of this property
            if (value is string)
            {
                if (UnderlyingType.IsEnum)
                {
                    object enumValue = TypeConverter.ConvertFromString(GetTypeDescriptorContext(OwnerEnumerator),
                        CultureInfo, GetActualString(value as string));

                    // If the enum is flagged, we don't test if the value to be assigned is correct
                    if (UnderlyingType.IsDefined(typeof(FlagsAttribute), false))
                        result = SetMultipleValues(enumValue);
                    else
                    {
                        if (Enum.IsDefined(UnderlyingType, enumValue))
                            result = SetMultipleValues(enumValue);
                        else
                            result = false;
                    }
                }
                else
                {
                    PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
                    string str = GetActualString(value as string);
                    if (TypeConverter.CanConvertFrom(context, typeof(string)))
                    {
                        result = SetMultipleValues(TypeConverter.ConvertFromString(
                            context, CultureInfo, str));
                    }
                    else
                    {
                        result = SetMultipleValues(str);
                    }
                }
            }
            else
                result = SetMultipleValues(value);
            
            return result;
        }

        private enum TriStateBool { tsFalse, tsTrue, tsUndefined };

        //private TriStateBool _readonlyByPropertyDescriptor = TriStateBool.tsUndefined;

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

            bool readonlyPd = ((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).IsReadOnly;
            ReadOnlyAttribute attr = (ReadOnlyAttribute)selfEnum.Property.Value.GetAttribute(typeof(ReadOnlyAttribute));
            bool readonlyByAttribute = (attr != null && attr.IsReadOnly);
            bool ro = (readonlyPd || readonlyByAttribute);

            /*
            // Some checks on the readonly state (it could have been changed by a PropertyDescriptor)
            if (_readonlyByPropertyDescriptor != TriStateBool.tsUndefined)
            {
                if ((readonlyPd && (_readonlyByPropertyDescriptor == TriStateBool.tsFalse)) ||
                    (!readonlyPd && (_readonlyByPropertyDescriptor == TriStateBool.tsTrue)))
                {
                    if (ro && (selfEnum.Property.ParentGrid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.Disabled) &&
                        selfEnum.Property.Enabled)
                        selfEnum.Property.ParentGrid.EnableProperty(selfEnum, false);
                    else if (!ro && !selfEnum.Property.Enabled)
                        selfEnum.Property.ParentGrid.EnableProperty(selfEnum, true);
                }
            }

            _readonlyByPropertyDescriptor = (readonlyPd ? TriStateBool.tsTrue : TriStateBool.tsFalse);
            */
            return ro;
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

        public override string GetStringValue()
        {
            if (HasMultipleValues)
                return "";

            try
            {
                string str = TypeConverter.ConvertToString(GetTypeDescriptorContext(OwnerEnumerator), CultureInfo, UnderlyingValue);
                return (str == null ? "" : str);
            }
            catch (Exception e)
            {
                if (e.InnerException != null)
                    return e.InnerException.Message;
                else
                    return e.Message;
            }
        }

        public override Attribute GetAttribute(Type attributeType)
        {
            // Let's see if we have an attribute that has been passed with AppendProperty
            Attribute attr = base.GetAttribute(attributeType);
            if (attr != null)
                return attr;

            // If not then metadata is searched
            return ((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).Attributes[attributeType];
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
                    foreach (Attribute attr in ((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).Attributes)
                    {
                        if (attr.GetType() == attributeType)
                            attrList.Add(attr);
                    }
                }
            }

            return attrList;
        }

        public override PropertyTypeDescriptorContext GetTypeDescriptorContext(PropertyEnumerator propEnum)
        {
            PropertyDescriptor pd = (PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key;
            return new PropertyTypeDescriptorContext(pd, TargetInstance, propEnum, Grid);
        }

        public override void Recreate(bool sendPropertyCreatedEvent)
        {
            Grid.InsideValueRecreate = true;
            Grid.BeginUpdate();
            object states = Grid.SavePropertiesStates();

            // Set the grid in categorized mode
            PropertyGrid.DisplayModes dispMode = Grid.DisplayMode;
            if (dispMode != PropertyGrid.DisplayModes.Categorized)
                Grid.Grid.DisplayMode = PropertyGrid.DisplayModes.Categorized;
            
            RecreateChildProperties();
            
            PropertyValue parentValue = null;
            if (OwnerEnumerator.Parent.Property != null)
                parentValue = OwnerEnumerator.Parent.Property.Value;

            PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);

            if ((parentValue == null) || !parentValue.TypeConverter.GetPropertiesSupported(context) ||
                parentValue.TypeConverter.GetCreateInstanceSupported(context))
            {
                PropertyDescriptor pd = null;

                object target = TargetInstance;
                if (target != null)
                    pd = TypeDescriptor.GetProperties(target,
                        new Attribute[1] { BrowsableAttribute.Yes })[((PropertyDescriptor)((Utilities.KeyValuePair)_targets[0]).Key).Name];

                if (pd != null)
                {
                    _targets[0] = new Utilities.KeyValuePair(pd, ((Utilities.KeyValuePair)_targets[0]).Value);
                    ResetDisplayedValues(true);

                    Grid.ShowProperty(mOwnerEnumerator, true);
                }
                else
                    // Hide properties that are suddenly not browsable
                    Grid.ShowProperty(mOwnerEnumerator, false);
            }

            // Handle ReadOnly
            bool readOnly = IsReadOnly(mOwnerEnumerator);
            if (readOnly)
            {
                if (Grid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.Disabled)
                    Grid.EnableProperty(mOwnerEnumerator, false);
                else
                    mOwnerEnumerator.Property.ForeColor = DisabledForeColor;
            }
            else
            {
                Grid.EnableProperty(mOwnerEnumerator, true);
                mOwnerEnumerator.Property.ForeColor = Grid.ForeColor;
            }

            if (Feel == null)
            {
                if (!readOnly || (((Look == null) || !Look.NoFeelForReadOnly) && (Grid.ReadOnlyVisual == PropertyGrid.ReadOnlyVisuals.ReadOnlyFeel)))
                    Feel = Grid.Grid.FindFeel(mOwnerEnumerator.Parent, mOwnerEnumerator);
            }

            if (sendPropertyCreatedEvent)
                Grid.OnPropertyCreated(new PropertyCreatedEventArgs(OwnerEnumerator));

            // Set the grid in non categorized mode if it was as such before calling SelectedObject(s)
            if (dispMode != PropertyGrid.DisplayModes.Categorized)
                Grid.Grid.DisplayMode = dispMode;

            Grid.RestorePropertiesStates(states);
            Grid.EndUpdate();
            Grid.InsideValueRecreate = false;
        }

        internal void AddTargetInstance(object targetInstance, PropertyDescriptor propertyDescriptor)
        {
            if (propertyDescriptor != null)
                _targets.Add(new Utilities.KeyValuePair(propertyDescriptor, targetInstance));
        }

        public void AddTargetInstance(object targetInstance, string memberName)
        {
            PropertyDescriptor propertyDescriptor = TypeDescriptor.GetProperties(targetInstance, new Attribute[1] { BrowsableAttribute.Yes })[memberName];
            AddTargetInstance(targetInstance, propertyDescriptor);
        }

        internal void SetTargetInstances(object[] targetInstances, ArrayList propertyDescriptors)
        {
            // TODO
        }
        /*
        private void SetupChildrenTargetInstances(PropertyEnumerator parentPropEnum)
        {
            PropertyEnumerator childEnum = parentPropEnum.Children;
            while (childEnum != childEnum.RightBound)
            {
                PropertyValueIndirect childPropertyValue = ((PropertyValueIndirect)childEnum.Property.Value);
                for (int i = 1; i < _targets.Count; i++)
                {
                    Utilities.KeyValuePair pair = (Utilities.KeyValuePair)_targets[i];
                    object parentTarget = GetTargetInstance(pair.Value, (PropertyDescriptor)pair.Key);
                    object childTarget = PropertyDescriptor.GetValue(parentTarget);
                    childPropertyValue.AddTargetInstance(childTarget, childEnum.Property.Value.PropertyDescriptor);
                }

                childPropertyValue.SetupChildrenTargetInstances(childEnum);

                childEnum.MoveNext();
            }
        }

        internal void SetupChildrenTargetInstances()
        {
            SetupChildrenTargetInstances(OwnerEnumerator);
        }*/
/*
        protected internal override PropertyValue[] GetChildValues(object[] targetInstances)
        {
            PropertyValue[] values = base.GetChildValues(null);

            PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
            bool readOnlyProperties = TypeConverter.GetCreateInstanceSupported(context);

            if ((values.Length > 0) && (targetInstances != null) && (targetInstances.Length > 1) && !readOnlyProperties)
            {
                ArrayList returnedPropertyValues = new ArrayList();
                returnedPropertyValues.AddRange(values);

                Hashtable propertyDescriptorDefs = new Hashtable(); // <PropertyDescriptor, List<PropertyDescriptor>>

                bool firstTarget = true;
                foreach (object targetInstance in targetInstances)
                {
                    foreach (PropertyValue propertyValue in values)
                    {
                        PropertyDescriptor propertyDescriptor;
                        if (firstTarget)
                            propertyDescriptor = propertyValue.PropertyDescriptor;
                        else
                            TODO

                        MergablePropertyAttribute mergeAttr = propertyDescriptor.Attributes[typeof(MergablePropertyAttribute)]
                            as MergablePropertyAttribute;
                        if ((mergeAttr != null) && (mergeAttr.AllowMerge == false))
                            continue;

                        if (firstTarget)
                        {
                            // For the first target instance, we store all the PropertyDescriptors
                            ArrayList pdList = new ArrayList();
                            pdList.Add(propertyDescriptor);

                            propertyDescriptorDefs.Add(propertyDescriptor, new Utilities.KeyValuePair(pdList, propertyValue));
                        }
                        else
                        {
                            // For all the remaining target instances, we compare the PropertyDescriptors
                            // to the first set of stored PropertyDescriptors. Their name and property type must
                            // correspond
                            Utilities.KeyValuePair pair = (Utilities.KeyValuePair)propertyDescriptorDefs[propertyDescriptor];
                            if (pair != null)
                            {
                                ArrayList existPdList = (ArrayList)pair.Key;

                                // If the propertyDesriptors match, we increment a count
                                if (Grid.MatchPropertyDescriptors((PropertyDescriptor)existPdList[0], propertyDescriptor))
                                    existPdList.Add(propertyDescriptor);
                                // else the PropertyDescriptor is not eligible and is removed
                                else
                                    propertyDescriptorDefs.Remove(propertyDescriptor);
                            }
                        }
                    }

                    firstTarget = false;
                }

                // Ensure that we count only propertyDescriptors that are present in all target instances
                foreach (Utilities.KeyValuePair pair in propertyDescriptorDefs.Values)
                {
                    ArrayList pdList = (ArrayList)pair.Key;
                    if (pdList.Count != targetInstances.Length)
                        returnedPropertyValues.Remove(pair.Value);
                }

                PropertyValue[] array = new PropertyValue[returnedPropertyValues.Count];
                Array.Copy(returnedPropertyValues.ToArray(), array, returnedPropertyValues.Count);
                return array;
            }
            else
                return values;
        }
 */
    }
}
