
using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{

  [Serializable]
  [TypeConverter(typeof(ShapeConverter))]
  public class Shape
  {
    String str;

    public Shape(String str)
    {
      this.str = str;
    }

    public bool Contains(String substring)
    {
      return str.Contains(substring);
    }

    public override String ToString()
    {
      return str;
    }

    public static implicit operator String(Shape shape)
    {
    if (shape == null)
      return "";
    else
      return shape.str;
    }

    public static implicit operator Shape(String str)
    {
      Shape shape = new Shape(str);
      return shape;
    }
  }
}
