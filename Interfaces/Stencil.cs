using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Interface
{
  [Serializable]
  public class Stencil
  {
    String str;

    public static implicit operator Stencil(String r)
    {
      Stencil stencil = new Stencil();
      stencil.str = r;
      return stencil;
    }

    public static implicit operator String(Stencil r)
    {
      return r.str;
    }

    public bool Contains(String substr)
    {
      return str.Contains(substr);
    }
  }
}
