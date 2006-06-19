using System;
using System.ComponentModel;
using System.Collections;
using System.Globalization;
using System.Text;

namespace SysCAD.Interface
{
  /// <summary>
  /// Summary description for BasePropertyConverter.
  /// </summary>
  public class BaseAccessItemConverter : TypeConverter
  {
    public BaseAccessItemConverter()
    {
    }

    public override bool CanConvertFrom(ITypeDescriptorContext context,
                 Type sourceType)
    {
      if (sourceType == typeof(string))
        return true;

      return base.CanConvertFrom(context, sourceType);
    }

    public override bool CanConvertTo(ITypeDescriptorContext context,
               Type destinationType)
    {
      if ((destinationType == typeof(string))
         || (destinationType == typeof(FloatAccessItem))
         || (destinationType == typeof(BranchAccessItem)))
        return true;

      return base.CanConvertTo(context, destinationType);
    }

    //public override object ConvertFrom(ITypeDescriptorContext context,
    //            CultureInfo culture,
    //            object value)
    //{
    //  string s = value as string;
    //  if (s == null)
    //    return base.ConvertFrom(context, culture, value);

    //  // FIXME: use culture
    //  string[] subs = s.Split(' ');
    //  if (subs.Length != 2)
    //    throw new ArgumentException("Failed to parse Text(" + s + ") expected text in the format \"value Unit.\"");

    //  float isoValue = Single.Parse(subs[0]);

    //  return new FloatProperty("name", "description", isoValue);
    //}

    public override object ConvertTo(ITypeDescriptorContext context,
              CultureInfo culture,
              object value,
              Type destinationType)
    {
      // LAMESPEC: "The default implementation calls the object's
      // ToString method if the object is valid and if the destination
      // type is string." MS does not behave as per the specs.
      // Oh well, we have to be compatible with MS.
      if ((destinationType == typeof(string)) && (value is FloatAccessItem))
      {
        FloatAccessItem floatProperty = (FloatAccessItem)value;
        StringBuilder sb = new StringBuilder();
        sb.Append(floatProperty.ISOValue); sb.Append(" ISOUnit");
        return sb.ToString();
      }

      if ((destinationType == typeof(string)) && (value is BranchAccessItem))
      {
        BranchAccessItem floatProperty = (BranchAccessItem)value;
        StringBuilder sb = new StringBuilder();
        //sb.Append(floatProperty.ISOValue); sb.Append(" ISOUnit");
        return sb.ToString();
      }

      return base.ConvertTo(context, culture, value, destinationType);
    }

    //public override object CreateInstance(ITypeDescriptorContext context,
    //               IDictionary propertyValues)
    //{
    //  float isoValue = (float)propertyValues["ISOValue"];

    //  return new FloatProperty(isoValue);
    //}

    //public override bool GetCreateInstanceSupported(ITypeDescriptorContext context)
    //{
    //  return true;
    //}

    //public override PropertyDescriptorCollection GetProperties(
    //          ITypeDescriptorContext context,
    //          object value, Attribute[] attributes)
    //{
    //  if (value is BaseProperty)
    //    return TypeDescriptor.GetProperties(value, attributes);

    //  return base.GetProperties(context, value, attributes);
    //}

    //public override bool GetPropertiesSupported(ITypeDescriptorContext context)
    //{
    //  return true;
    //}
  }
}