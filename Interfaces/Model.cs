using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Interface
{
  [Serializable]
  public class Model
  {
    String str;

    public static implicit operator Model(String r)
    {
      Model model = new Model();
      model.str = r;
      return model;
    }

    public static implicit operator String(Model r)
    {
      return r.str;
    }
  }
}
