using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{
  [Serializable]
  public abstract class Item
  {
    private Guid guid;
    private String tag;

    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }
  }
}
