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

    public Item()
    {
      Guid = Guid.NewGuid();
      Tag = Guid.ToString();
    }

    public Item(Guid guid, String tag)
    {
      Guid = guid;
      Tag = tag;
    }

    public Item(String tag)
    {
      Guid = Guid.NewGuid();
      Tag = tag;
    }

    public Guid Guid
    {
      get { return guid; }
      set
      {
        if (value == null) throw new NullReferenceException("Guid cannot be null.");
        guid = value;
      }
    }

    public String Tag
    {
      get { return tag; }
      set
      {
        if (value == null) throw new NullReferenceException("Tag cannot be null.");
        tag = value;
      }
    }
  }
}
