using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Interface
{
  [Serializable]
  public class Model
  {
    String str = string.Empty;

    public static implicit operator Model(String str)
    {
      Model model = new Model();
      model.str = str;
      return model;
    }

    public static implicit operator String(Model model)
    {
      if (model != null)
        return model.str;
      else
        return String.Empty;
    }

    public override string ToString()
    {
      return str;
    }
  }
}
