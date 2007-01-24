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
    private String name;

    public Dictionary<Guid, GraphicLink> graphicLinks;
    public Dictionary<Guid, GraphicItem> graphicItems;
    public Dictionary<Guid, GraphicThing> graphicThings;

    public BaseProtocol()
    {
      graphicLinks = new Dictionary<Guid, GraphicLink>();
      graphicItems = new Dictionary<Guid, GraphicItem>();
      graphicThings = new Dictionary<Guid, GraphicThing>();
    }

    public String Name
    {
      get { return name; }
      protected set { name = value; }
    }

    public enum RunStates
    {
      Edit, Run, Idle
    }



    public delegate void StateChangedHandler(Int64 eventId, Int64 requestId, RunStates runState);

    public delegate void StepHandler(Int64 eventId, Int64 step, DateTime time);

    public delegate void ItemCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ItemModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ItemDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);


    public delegate void LinkCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate void LinkModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate void LinkDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);


    public delegate void ThingCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);
    public delegate void ThingModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);
    public delegate void ThingDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);



    public StateChangedHandler StateChanged;

    public StepHandler Step;


    public ItemCreatedHandler ItemCreated;
    public ItemModifiedHandler ItemModified;
    public ItemDeletedHandler ItemDeleted;


    public LinkCreatedHandler LinkCreated;
    public LinkModifiedHandler LinkModified;
    public LinkDeletedHandler LinkDeleted;


    public ThingCreatedHandler ThingCreated;
    public ThingModifiedHandler ThingModified;
    public ThingDeletedHandler ThingDeleted;



    public void OnStateChanged(Int64 eventId, Int64 requestId, RunStates runState)
    {
      if (StateChanged != null)
      {
        try
        {
          StateChanged(eventId, requestId, runState);
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


    public void OnItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemCreated != null)
      {
        try
        {
          ItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
        }
        catch (SocketException) { }
      }
    }

    public void OnItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemModified != null)
      {
        try
        {
          ItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
        }
        catch (SocketException) {}
      }
    }

    public void OnItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (ItemDeleted != null)
      {
        try
        {
          ItemDeleted(eventId, requestId, guid);
        }
        catch (SocketException) { }
      }
    }



    public void OnLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkCreated != null)
      {
        try
        {
          LinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
        }
        catch (SocketException) { }
      }
    }

    public void OnLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkModified != null)
      {
        try
        {
          LinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
        }
        catch (SocketException) { }
      }
    }

    public void OnLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (LinkDeleted != null)
      {
        try
        {
          LinkDeleted(eventId, requestId, guid);
        }
        catch (SocketException) { }
      }
    }


    public void OnThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (ThingCreated != null)
      {
        try
        {
          ThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
        }
        catch (SocketException) { }
      }
    }

    public void OnThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (ThingModified != null)
      {
        try
        {
          ThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
        }
        catch (SocketException) { }
      }
    }

    public void OnThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (ThingDeleted != null)
      {
        try
        {
          ThingDeleted(eventId, requestId, guid);
        }
        catch (SocketException) { }
      }
    }

    //[SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }
  }
}