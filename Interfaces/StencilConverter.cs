using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Globalization;

namespace SysCAD.Interface
{
  public class ShapeConverter : StringConverter
  {
    private static List<Shape> stencilList = new List<Shape>();

    static public void ClearStencilList()
    {
      stencilList.Clear();
    }

    public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
    {
      //true means show a combobox
      return true;
    }

    public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
    {
      //true will limit to list. false will show the list, 
      //but allow free-form entry
      return true;
    }

    public override System.ComponentModel.TypeConverter.StandardValuesCollection
           GetStandardValues(ITypeDescriptorContext context)
    {
      return new StandardValuesCollection(stencilList.ToArray());
    }

    public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
    {
      if (sourceType == typeof(String))
        return true;

      return base.CanConvertFrom(context, sourceType);
    }

    public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
    {
      String str = value as String;
      if (str != null)
        return new Shape(str);

      return base.ConvertFrom(context, culture, value);
    }

    public static void AddStencil(string stencil)
    {
      stencilList.Add(stencil);
    }
  }
}
