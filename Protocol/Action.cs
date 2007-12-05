using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.ObjectModel;

namespace SysCAD.Protocol
{
  [Serializable]
  public class Action
  {
    Collection<Item> create = new Collection<Item>();
    Collection<Item> modify = new Collection<Item>();
    Collection<Guid> delete = new Collection<Guid>();

    public Collection<Item> Create
    {
      get { return create; }
      set { create = value; }
    }

    public Collection<Guid> Delete
    {
      get { return delete; }
      set { delete = value; }
    }

    public Collection<Item> Modify
    {
      get { return modify; }
      set { modify = value; }
    }
  }
}
