
using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{

  [Serializable]
  [TypeConverter(typeof(ShapeConverter))]
  public class Stencil
  {
    String str;

    public Stencil(String str)
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

    public static implicit operator String(Stencil stencil)
    {
    if (stencil == null)
      return "";
    else
      return stencil.str;
    }

    public static implicit operator Stencil(String str)
    {
      Stencil stencil = new Stencil(str);
      return stencil;
    }
  }
}
