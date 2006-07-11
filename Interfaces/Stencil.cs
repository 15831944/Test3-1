using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Interface
{
  [Serializable]
  [ComVisible(true)]
  [TypeConverter(typeof(ShapeConverter))]
  public class Shape
  {
    String str;

    public Shape(String r)
    {
      str = r ;
    }

    public static implicit operator Shape(String r)
    {
      Shape shape = new Shape(r);
      return shape;
    }

    public static implicit operator String(Shape r)
    {
      return r.str;
    }

    public bool Contains(String substr)
    {
      return str.Contains(substr);
    }

    public override string ToString()
    {
      return str;
    }
  }
}
