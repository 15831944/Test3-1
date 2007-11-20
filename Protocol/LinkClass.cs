
using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{

  [Serializable]
  public class LinkClass
  {
    String str = String.Empty;

    public LinkClass()
    {
    }

    public LinkClass(String str)
    {
      this.str = str;
    }

    public override String ToString()
    {
      return str;
    }

    public static implicit operator String(LinkClass linkClass)
    {

      if (linkClass != null)
        return linkClass.str;

      else
        return String.Empty;
    }

    public static implicit operator LinkClass(String str)
    {
      LinkClass model = new LinkClass();
      model.str = str;
      return model;
    }
  }
}
