
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

    public Dictionary<Guid, GraphicItem> graphicItems;
    public Dictionary<Guid, GraphicLink> graphicLinks;
    public Dictionary<Guid, GraphicThing> graphicThings;

    public Dictionary<Guid, GraphicGroup> graphicGroups;

    public Dictionary<DateTime, LogItem> logItems;

    public ItemCreatedHandler ItemCreated;
    public ItemDeletedHandler ItemDeleted;
    public ItemModifiedHandler ItemModified;

    public LinkCreatedHandler LinkCreated;
    public LinkDeletedHandler LinkDeleted;
    public LinkModifiedHandler LinkModified;

    public StateChangedHandler StateChanged;

    public StepHandler Step;

    public SyncHandler Sync;

    public ThingCreatedHandler ThingCreated;
    public ThingDeletedHandler ThingDeleted;
    public ThingModifiedHandler ThingModified;

    private String name;

    public delegate void ItemCreatedHandler(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ItemDeletedHandler(Int64 eventId, Int64 requestID, Guid guid);
    public delegate void ItemModifiedHandler(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);

    public delegate void LinkCreatedHandler(Int64 eventId, Int64 requestID, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate void LinkDeletedHandler(Int64 eventId, Int64 requestID, Guid guid);
    public delegate void LinkModifiedHandler(Int64 eventId, Int64 requestID, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);

    public delegate void StateChangedHandler(Int64 eventId, Int64 requestID, RunStates runState);

    public delegate void StepHandler(Int64 eventId, Int64 step, DateTime time);

    public delegate void SyncHandler(Int64 eventId);

    public delegate void ThingCreatedHandler(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);
    public delegate void ThingDeletedHandler(Int64 eventId, Int64 requestID, Guid guid);
    public delegate void ThingModifiedHandler(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);

    public enum RunStates
    {
      Edit, Run, Idle
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

    public void OnItemCreated(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {

      if (ItemCreated != null)
      {

        try
        {
          ItemCreated(eventId, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
        }

        catch (SocketException) { }
      }
    }

    public void OnItemDeleted(Int64 eventId, Int64 requestID, Guid guid)
    {

      if (ItemDeleted != null)
      {

        try
        {
          ItemDeleted(eventId, requestID, guid);
        }

        catch (SocketException) { }
      }
    }

    public void OnItemModified(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {

      if (ItemModified != null)
      {

        try
        {
          ItemModified(eventId, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
        }

        catch (SocketException) { }
      }
    }

    public void OnLinkCreated(Int64 eventId, Int64 requestID, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {

      if (LinkCreated != null)
      {

        try
        {
          LinkCreated(eventId, requestID, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
        }

        catch (SocketException) { }
      }
    }

    public void OnLinkDeleted(Int64 eventId, Int64 requestID, Guid guid)
    {

      if (LinkDeleted != null)
      {

        try
        {
          LinkDeleted(eventId, requestID, guid);
        }

        catch (SocketException) { }
      }
    }

    public void OnLinkModified(Int64 eventId, Int64 requestID, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {

      if (LinkModified != null)
      {

        try
        {
          LinkModified(eventId, requestID, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
        }

        catch (SocketException) { }
      }
    }

    public void OnStateChanged(Int64 eventId, Int64 requestID, RunStates runState)
    {

      if (StateChanged != null)
      {

        try
        {
          StateChanged(eventId, requestID, runState);
        }

        catch (SocketException) { }
      }
    }

    public void OnStep(Int64 eventId, Int64 step, DateTime time)
    {

      if (Step != null)
      {

        try
        {
          Step(eventId, step, time);
        }

        catch (SocketException) { }
      }
    }

    public void OnSync(Int64 eventId)
    {

      if (Sync != null)
      {

        try
        {
          Sync(eventId);
        }

        catch (SocketException) { }
      }
    }

    public void OnThingCreated(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {

      if (ThingCreated != null)
      {

        try
        {
          ThingCreated(eventId, requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
        }

        catch (SocketException) { }
      }
    }

    public void OnThingDeleted(Int64 eventId, Int64 requestID, Guid guid)
    {

      if (ThingDeleted != null)
      {

        try
        {
          ThingDeleted(eventId, requestID, guid);
        }

        catch (SocketException) { }
      }
    }

    public void OnThingModified(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {

      if (ThingModified != null)
      {

        try
        {
          ThingModified(eventId, requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
        }

        catch (SocketException) { }
      }
    }

    public String Name
    {
      get { return name; }

      protected set { name = value; }
    }
  }
}
