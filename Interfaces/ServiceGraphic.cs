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

namespace SysCAD.Interface
{
  [Serializable]
  public sealed class ServiceGraphic : BaseGraphic
  {
    private uint requestID = 0;
    private uint eventID = 0;

    public delegate bool CreateItemHandler(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate bool ModifyItemHandler(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate bool DeleteItemHandler(ServiceGraphic graphic, uint requestID, Guid guid);

    public delegate bool CreateLinkHandler(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool ModifyLinkHandler(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool DeleteLinkHandler(ServiceGraphic graphic, uint requestID, Guid guid);

    public delegate bool CreateThingHandler(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate bool ModifyThingHandler(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate bool DeleteThingHandler(ServiceGraphic graphic, uint requestID, Guid guid);

    public delegate PortStatus PortCheckHandler(ServiceGraphic graphic, Guid itemGuid, Anchor anchor);

    public delegate ArrayList PropertyListHandler(ServiceGraphic graphic, Guid guid, String tag, String path);

    private CreateItemHandler createItemHandler;
    private ModifyItemHandler modifyItemHandler;
    private DeleteItemHandler deleteItemHandler;

    private CreateLinkHandler createLinkHandler;
    private ModifyLinkHandler modifyLinkHandler;
    private DeleteLinkHandler deleteLinkHandler;

    private CreateThingHandler createThingHandler;
    private ModifyThingHandler modifyThingHandler;
    private DeleteThingHandler deleteThingHandler;

    private PortCheckHandler portCheckHandler;

    private PropertyListHandler propertyListHandler;

    public ServiceGraphic(
      CreateItemHandler createItemHandler, ModifyItemHandler modifyItemHandler, DeleteItemHandler deleteItemHandler,
      CreateLinkHandler createLinkHandler, ModifyLinkHandler modifyLinkHandler, DeleteLinkHandler deleteLinkHandler,
      CreateThingHandler createThingHandler, ModifyThingHandler modifyThingHandler, DeleteThingHandler deleteThingHandler,
      PortCheckHandler portCheckHandler, PropertyListHandler propertyListHandler)
    {
      this.createItemHandler = createItemHandler;
      this.modifyItemHandler = modifyItemHandler;
      this.deleteItemHandler = deleteItemHandler;

      this.createLinkHandler = createLinkHandler;
      this.modifyLinkHandler = modifyLinkHandler;
      this.deleteLinkHandler = deleteLinkHandler;

      this.createThingHandler = createThingHandler;
      this.modifyThingHandler = modifyThingHandler;
      this.deleteThingHandler = deleteThingHandler;

      this.portCheckHandler = portCheckHandler;

      this.propertyListHandler = propertyListHandler;
    }

    ~ServiceGraphic()
    {
    }

    public bool CreateItem(out uint requestID, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      this.requestID++;
      requestID = this.requestID;
      guid = Guid.NewGuid();
      return createItemHandler(this, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public bool ModifyItem(out uint requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicItems.ContainsKey(guid))
        return modifyItemHandler(this, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      else
        return false;
    }

    public bool DeleteItem(out uint requestID, Guid guid)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicItems.ContainsKey(guid))
        return deleteItemHandler(this, requestID, guid);
      else
        return false;
    }

    public bool CreateLink(out uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestID++;
      requestID = this.requestID;
      guid = new Guid();
      return createLinkHandler(this, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool ModifyLink(out uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicLinks.ContainsKey(guid))
        return modifyLinkHandler(this, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
      else
        return false;
    }

    public bool DeleteLink(out uint requestID, Guid guid)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicLinks.ContainsKey(guid))
        return deleteLinkHandler(this, requestID, guid);
      else
        return false;
    }


    public bool CreateThing(out uint requestID, out Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      this.requestID++;
      requestID = this.requestID;
      guid = Guid.NewGuid();
      return createThingHandler(this, requestID, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public bool ModifyThing(out uint requestID, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicThings.ContainsKey(guid))
        return modifyThingHandler(this, requestID, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
      else
        return false;
    }

    public bool DeleteThing(out uint requestID, Guid guid)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicThings.ContainsKey(guid))
        return deleteThingHandler(this, requestID, guid);
      else
        return false;
    }


    public PortStatus PortCheck(Guid itemGuid, Anchor anchor)
    {
      if (graphicItems.ContainsKey(itemGuid))
        return portCheckHandler(this, itemGuid, anchor);
      else
        return PortStatus.Unavailable;
    }


    public ArrayList PropertyList(Guid guid, String tag, String path)
    {
      //todo: check path is valid.
      return propertyListHandler(this, guid, tag, path);
    }


    public void DoItemCreated(uint requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (!graphicItems.ContainsKey(guid))
      {
        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = (ARectangleF)boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        eventID++;
        OnItemCreated(eventID, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoItemModified(uint requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.Tag = tag;
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = (ARectangleF)boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        eventID++;
        OnItemModified(eventID, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoItemDeleted(uint requestID, Guid guid)
    {
      if (graphicItems.ContainsKey(guid))
      {
        graphicItems.Remove(guid);

        eventID++;
        OnItemDeleted(eventID, requestID, guid);
      }
    }

    public void DoLinkCreated(uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (!graphicItems.ContainsKey(guid))
      {
        GraphicLink graphicLink = new GraphicLink(guid, tag, classID, origin, originPort, destination, destinationPort);

        graphicLink.controlPoints = new List<PointF>();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.controlPoints.Add(controlPoint);

        graphicLinks.Add(guid, graphicLink);

        eventID++;
        OnLinkCreated(eventID, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void DoLinkModified(uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      GraphicLink graphicLink;
      if (graphicLinks.TryGetValue(guid, out graphicLink))
      {
        graphicLink.Tag = tag;
        graphicLink.ClassID = classID;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        graphicLink.OriginPort = originPort;
        graphicLink.DestinationPort = destinationPort;

        graphicLink.controlPoints.Clear();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.controlPoints.Add(controlPoint);

        eventID++;
        OnLinkModified(eventID, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void DoLinkDeleted(uint requestID, Guid guid)
    {
      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        eventID++;
        OnLinkDeleted(eventID, requestID, guid);
      }
    }


    public void DoThingCreated(uint requestID, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (!graphicThings.ContainsKey(guid))
      {
        GraphicThing graphicThing = new GraphicThing(guid, tag);
        graphicThing.Path = path;
        graphicThing.BoundingRect = (ARectangleF)boundingRect;
        graphicThing.Angle = angle;
        graphicThing.FillColor = fillColor;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        graphicThings.Add(guid, graphicThing);

        eventID++;
        OnThingCreated(eventID, requestID, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoThingModified(uint requestID, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      GraphicThing graphicThing;
      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Tag = tag;
        graphicThing.Path = path;
        graphicThing.BoundingRect = (ARectangleF)boundingRect;
        graphicThing.Angle = angle;
        graphicThing.FillColor = fillColor;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        eventID++;
        OnThingModified(eventID, requestID, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoThingDeleted(uint requestID, Guid guid)
    {
      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        eventID++;
        OnThingDeleted(eventID, requestID, guid);
      }
    }

  }
}