using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Interface
{
  public class FloatAccessItem : BaseAccessItem
  {
    private float isoValue;

    public FloatAccessItem(String name, String description, float isoValue)
    {
      Name = name;
      Description = description;
      this.isoValue = isoValue;
    }

    public float ISOValue
    {
      get { return isoValue; }
      set { isoValue = value; }
    }
  }
}
