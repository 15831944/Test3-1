using System;

namespace SysCAD.Protocol
{
  public enum PortStatusEnum { Available = 0, Unavailable = -1 }

  [Serializable]
  public class PortInfo
   {
    private PortStatusEnum status;

    public PortInfo(PortStatusEnum status)
    {
      this.status = status;
    }

    public PortStatusEnum Status
    {
      get { return status; }
    }
  }

  [Serializable]
  public class ItemInfo
  {
  }

  [Serializable]
  public class LinkInfo
  {
  }
}