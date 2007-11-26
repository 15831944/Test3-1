
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
  abstract public class EngineBaseProtocol : BaseProtocol
  {
    public PortInfoRequestedHandler PortInfoRequested;
    public delegate void PortInfoRequestedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag);

    public enum RunState
    {
      Edit, Run, Idle
    }

    public void OnPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag)
    {
      if (PortInfoRequested != null)
      {
        foreach (PortInfoRequestedHandler portInfoRequestedHandler in PortInfoRequested.GetInvocationList())
        {
          try { portInfoRequestedHandler(eventId, requestId, guid, tag); }
          catch (SocketException) { PortInfoRequested -= portInfoRequestedHandler; }
        }
      }
    }
  }
}
