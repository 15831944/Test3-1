using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{
  [Serializable]
  public class ModelProperty
  {
    private String path = null;
    private object state = null;

    public ModelProperty()
    {
    }

    public ModelProperty(String path, object state)
    {
      this.path = path;
      this.state = state;
    }


    public String Path
    {
      get { return path; }
      set { path = value; }
    }

    public object State
    {
      get { return state; }
      set { state = value; }
    }


  }
}
