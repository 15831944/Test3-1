
using System;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using System.Runtime.Remoting.Lifetime;
using System.Drawing;
using System.Threading;

using System.Runtime.Remoting.Channels.Ipc;
using System.Runtime.Remoting.Channels;
using System.Collections;
using System.Runtime.Remoting.Channels.Tcp;
using System.Net.Sockets;

//using System.Security.Permissions;

namespace SysCAD.Protocol
{
  [Serializable]
  abstract public class BaseProtocol : MarshalByRefObject
  {
    public Dictionary<Guid, GraphicGroup> graphicGroups;

    public Dictionary<Guid, GraphicItem> graphicItems;
    public Dictionary<Guid, GraphicLink> graphicLinks;
    public Dictionary<Guid, GraphicThing> graphicThings;

    public Dictionary<DateTime, LogItem> logItems;

    private String name;

    //[SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }

    public String Name
    {
      get { return name; }
      protected set { name = value; }
    }
  }
}
