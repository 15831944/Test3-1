using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Interface
{
  [Serializable]
  [ComVisible(true)]
  [TypeConverter(typeof(BaseAccessItemConverter))]
  public class BaseAccessItem// : ICustomTypeDescriptor
  {
    private String name;
    private String description;

    public String Name
    {
      get { return name; }
      set { name = value; }
    }

    public String Description
    {
      get { return description; }
      set { description = value; }
    }
  }
}
