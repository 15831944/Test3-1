
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

namespace SysCAD.Protocol
{
  [Serializable]
  abstract public class ClientBaseProtocol : BaseProtocol
  {
    public ChangedHandler Changed;
    public PortInfoRequestedHandler PortInfoRequested;
    public PermissionsChangedHandler PermissionsChanged;
    public StepHandler Step;
    public SyncHandler Sync;

    public delegate void ChangedHandler(Int64 eventId, Int64 requestId, List<Guid> created, List<Guid> modified, List<Guid> deleted);
    public delegate void PortInfoRequestedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo);
    public delegate void PermissionsChangedHandler(Int64 eventId, Int64 requestId, Permissions permissions);
    public delegate void StepHandler(Int64 eventId, Int64 step, DateTime time);
    public delegate void SyncHandler(Int64 eventId);

    [Serializable]
    public class Permissions
    {
      public Permissions(bool create, bool modify, bool delete)
      {
        this.create = create;
        this.modify = modify;
        this.delete = delete;
      }

      private bool create = false;

      public bool Create
      {
        get { return create; }
        set { create = value; }
      }
      private bool modify = false;

      public bool Modify
      {
        get { return modify; }
        set { modify = value; }
      }
      private bool delete = false;

      public bool Delete
      {
        get { return delete; }
        set { delete = value; }
      }
    }

    public override Object InitializeLifetimeService()
    {
      return null;
    }

    public void OnChanged(Int64 eventId, Int64 requestId, List<Guid> created, List<Guid> modified, List<Guid> deleted)
    {
      if (Changed != null)
      {
        foreach (ChangedHandler changedHandler in Changed.GetInvocationList())
        {
          try { changedHandler(eventId, requestId, created, modified, deleted); }
          catch (SocketException) { Changed -= changedHandler; }
        }
      }
    }

    public void OnPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      if (PortInfoRequested != null)
      {
        foreach (PortInfoRequestedHandler portInfoRequestedHandler in PortInfoRequested.GetInvocationList())
        {
          try { portInfoRequestedHandler(eventId, requestId, guid, tag, portInfo); }
          catch (SocketException) { PortInfoRequested -= portInfoRequestedHandler; }
        }
      }
    }

    public void OnPermissionsChanged(Int64 eventId, Int64 requestId, Permissions permissions)
    {
      if (PermissionsChanged != null)
      {
        foreach (PermissionsChangedHandler permissionsChangedHandler in PermissionsChanged.GetInvocationList())
        {
          try { permissionsChangedHandler(eventId, requestId, permissions); }
          catch (SocketException) { PermissionsChanged -= permissionsChangedHandler; }
        }
      }
    }

    public void OnStep(Int64 eventId, Int64 step, DateTime time)
    {
      if (Step != null)
      {
        foreach (StepHandler stepHandler in Step.GetInvocationList())
        {
          try { stepHandler(eventId, step, time); }
          catch (SocketException) { Step -= stepHandler; }
        }
      }
    }

    public void OnSync(Int64 eventId)
    {
      if (Sync != null)
      {
        foreach (SyncHandler syncHandler in Sync.GetInvocationList())
        {
          try { Sync(eventId); }
          catch (SocketException) { Sync -= syncHandler;  }
        }
      }
    }
  }
}
