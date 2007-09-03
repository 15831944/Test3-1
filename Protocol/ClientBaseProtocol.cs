
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
  abstract public class ClientBaseProtocol : BaseProtocol
  {
    public GroupCreatedHandler GroupCreated;
    public GroupDeletedHandler GroupDeleted;
    public GroupModifiedHandler GroupModified;

    public ItemCreatedHandler ItemCreated;
    public ItemDeletedHandler ItemDeleted;
    public ItemModifiedHandler ItemModified;

    public LinkCreatedHandler LinkCreated;
    public LinkDeletedHandler LinkDeleted;
    public LinkModifiedHandler LinkModified;

    public PermissionsChangedHandler PermissionsChanged;

    public StepHandler Step;

    public SyncHandler Sync;

    public ThingCreatedHandler ThingCreated;
    public ThingDeletedHandler ThingDeleted;
    public ThingModifiedHandler ThingModified;

    public delegate void GroupCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect);
    public delegate void GroupDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);
    public delegate void GroupModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect);

    public delegate void ItemCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, float textAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ItemDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);
    public delegate void ItemModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, float textAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);

    public delegate void LinkCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate void LinkDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);
    public delegate void LinkModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);

    public delegate void PermissionsChangedHandler(Int64 eventId, Int64 requestId, Permissions permissions);

    public delegate void StepHandler(Int64 eventId, Int64 step, DateTime time);

    public delegate void SyncHandler(Int64 eventId);

    public delegate void ThingCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);
    public delegate void ThingDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);
    public delegate void ThingModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);

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

    // This shouldn't be in the abstract base class...
    //public BaseProtocol()
    //{
    //  graphicLinks = new Dictionary<Guid, GraphicLink>();
    //  graphicItems = new Dictionary<Guid, GraphicItem>();
    //  graphicThings = new Dictionary<Guid, GraphicThing>();
    //}

    //[SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }

    public void OnGroupCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      if (GroupCreated != null)
      {
        try {GroupCreated(eventId, requestId, guid, tag, path, boundingRect);}
        catch (SocketException) { }
      }
    }

    public void OnGroupDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (GroupDeleted != null)
      {
        try {GroupDeleted(eventId, requestId, guid);}
        catch (SocketException) { }
      }
    }

    public void OnGroupModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      if (GroupModified != null)
      {
        try {GroupModified(eventId, requestId, guid, tag, path, boundingRect);}
        catch (SocketException) { }
      }
    }

    public void OnItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, float textAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemCreated != null)
      {
        try {ItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, mirrorX, mirrorY);}
        catch (SocketException) { }
      }
    }

    public void OnItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (ItemDeleted != null)
      {
        try {ItemDeleted(eventId, requestId, guid);}
        catch (SocketException) { }
      }
    }

    public void OnItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, float textAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemModified != null)
      {
        try {ItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, mirrorX, mirrorY);}
        catch (SocketException) { }
      }
    }

    public void OnLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkCreated != null)
      {
        try {LinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);}
        catch (SocketException) { }
      }
    }

    public void OnLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (LinkDeleted != null)
      {
        try {LinkDeleted(eventId, requestId, guid);}
        catch (SocketException) { }
      }
    }

    public void OnLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkModified != null)
      {
        try {LinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);}
        catch (SocketException) { }
      }
    }

    public void OnPermissionsChanged(Int64 eventId, Int64 requestId, Permissions permissions)
    {
      if (PermissionsChanged != null)
      {
        try { PermissionsChanged(eventId, requestId, permissions); }
        catch (SocketException) { }
      }
    }

    public void OnStep(Int64 eventId, Int64 step, DateTime time)
    {
      if (Step != null)
      {
        try {Step(eventId, step, time);}
        catch (SocketException) { }
      }
    }

    public void OnSync(Int64 eventId)
    {
      if (Sync != null)
      {
        try {Sync(eventId);}
        catch (SocketException) { }
      }
    }

    public void OnThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (ThingCreated != null)
      {
        try {ThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);}
        catch (SocketException) { }
      }
    }

    public void OnThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (ThingDeleted != null)
      {
        try {ThingDeleted(eventId, requestId, guid);}
        catch (SocketException) { }
      }
    }

    public void OnThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (ThingModified != null)
      {
        try {ThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);}
        catch (SocketException) { }
      }
    }
  }
}
