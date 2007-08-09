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
    public class PropertyValueSimulated : PropertyValueIndirect
    {
        public PropertyValueSimulated(PropertyGrid grid, PropertyValue parentValue, object target, PropertyDescriptor propertyDescriptor)
            : base(grid, parentValue, target, propertyDescriptor, null)
        {
        }

        public PropertyValueSimulated(PropertyGrid grid, PropertyValue parentValue, PropertyDescriptor propertyDescriptor)
            :
            base(grid, parentValue, propertyDescriptor)
        {
        }

        public override PropertyTypeDescriptorContext GetTypeDescriptorContext(PropertyEnumerator propEnum)
        {
            return new PropertyTypeDescriptorContext(ParentValue.PropertyDescriptor, ParentValue.TargetInstance, propEnum, Grid);
        }

        protected override bool SetValueInternal(object value, ValueUpdateSource updateSource)
        {
            try
            {
                PropertyValue parentValue = mOwnerEnumerator.Parent.Property.Value;
                TypeConverter converter = parentValue.TypeConverter;

                PropertyTypeDescriptorContext context = GetTypeDescriptorContext(OwnerEnumerator);
                PropertyDescriptorCollection collection = converter.GetProperties(context, parentValue.GetValue());

                if (collection != null)
                {
                    IDictionary dictionary = new Hashtable(collection.Count);
                    foreach (PropertyDescriptor propertyDescriptor in collection)
                    {
                        if (OwnerEnumerator.Property.Name.Equals(propertyDescriptor.Name))
                        {
                            if (value is string)
                            {
                                dictionary[propertyDescriptor.Name] = TypeConverter.ConvertFromString(
                                    context, CultureInfo, GetActualString(value as string));
                            }
                            else
                                dictionary[propertyDescriptor.Name] = value;
                        }
                        else
                            dictionary[propertyDescriptor.Name] = propertyDescriptor.GetValue(TargetInstance);
                    }

                    // Create the parent value from all the sibling values
                    PropertyDescriptor.SetValue(TargetInstance, value);
                    parentValue.SetValue(converter.CreateInstance(context, dictionary), ValueUpdateSource.FromChild);
                }
            }
            catch (Exception)
            {
                return false;
            }

            return true;
        }

        public override bool IsReadOnly(PropertyEnumerator selfEnum)
        {
            if (!Grid.Grid.Enabled && (Grid.DisableMode == PropertyGrid.DisableModes.Simple))
                return true;

            if (ParentValue.IsReadOnly(selfEnum.Parent))
                return true;

            return false;
        }
    }
}
