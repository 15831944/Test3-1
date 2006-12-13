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
//using System.Security.Permissions;

namespace SysCAD.Interface
{
  [Serializable]
  public class BaseGraphic : MarshalByRefObject
  {
    private String name;

    public Dictionary<Guid, GraphicLink> graphicLinks;
    public Dictionary<Guid, GraphicItem> graphicItems;
    public Dictionary<Guid, GraphicThing> graphicThings;

    public BaseGraphic()
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


    public delegate void ThingCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ThingModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
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
        StateChanged(eventId, requestId, runState);
    }


    public void OnStep(Int64 eventId, Int64 step, DateTime time)
    {
      if (Step != null)
        Step(eventId, step, time);
    }


    public void OnItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemCreated != null)
        ItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void OnItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemModified != null)
        ItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void OnItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (ItemDeleted != null)
        ItemDeleted(eventId, requestId, guid);
    }



    public void OnLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkCreated != null)
        LinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public void OnLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkModified != null)
        LinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public void OnLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (LinkDeleted != null)
        LinkDeleted(eventId, requestId, guid);
    }


    public void OnThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ThingCreated != null)
        ThingCreated(eventId, requestId, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void OnThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ThingModified != null)
        ThingModified(eventId, requestId, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void OnThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (ThingDeleted != null)
        ThingDeleted(eventId, requestId, guid);
    }

    //[SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }
  }
}