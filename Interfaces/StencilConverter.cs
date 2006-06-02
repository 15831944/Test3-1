using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace SysCAD.Interface
{
  public class StencilConverter : StringConverter
  {
    public static List<string> stencilList = new List<string>();

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
  }
}
