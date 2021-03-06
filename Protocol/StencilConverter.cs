
using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Globalization;

namespace SysCAD.Protocol
{

  public class ShapeConverter : StringConverter
  {

    private static List<Stencil> stencilList = new List<Stencil>();

    public static void AddStencil(String stencil)
    {
      stencilList.Add(stencil);
    }

    public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
    {

      if (sourceType == typeof(String))
        return true;

      return base.CanConvertFrom(context, sourceType);
    }

    static public void ClearStencilList()
    {
      stencilList.Clear();
    }

    public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
    {
      String str = value as String;

      if (str != null)
        return new Stencil(str);

      return base.ConvertFrom(context, culture, value);
    }

    public override System.ComponentModel.TypeConverter.StandardValuesCollection
           GetStandardValues(ITypeDescriptorContext context)
    {
      return new StandardValuesCollection(stencilList.ToArray());
    }

    public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
    {
      //true will limit to list. false will show the list, 
      //but allow free-form entry
      return true;
    }

    public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
    {
      //true means show a combobox
      return true;
    }
  }
}
