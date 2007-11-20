
using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{

  [Serializable]
  public class NodeClass
  {
    String str = String.Empty;

    public NodeClass()
    {
    }

    public NodeClass(String str)
    {
      this.str = str;
    }

    public override String ToString()
    {
      return str;
    }

    public static implicit operator String(NodeClass modelClass)
    {

      if (modelClass != null)
        return modelClass.str;

      else
        return String.Empty;
    }

    public static implicit operator NodeClass(String str)
    {
      NodeClass model = new NodeClass();
      model.str = str;
      return model;
    }
  }
}
