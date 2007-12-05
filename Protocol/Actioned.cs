using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.ObjectModel;

namespace SysCAD.Protocol
{
  [Serializable]
  public class Actioned
  {
    Collection<Guid> created = new Collection<Guid>();
    Collection<Guid> modified = new Collection<Guid>();
    Collection<Guid> deleted = new Collection<Guid>();

    public Collection<Guid> Created
    {
      get { return created; }
      set { created = value; }
    }

    public Collection<Guid> Modified
    {
      get { return modified; }
      set { modified = value; }
    }

    public Collection<Guid> Deleted
    {
      get { return deleted; }
      set { deleted = value; }
    }
  }
}
