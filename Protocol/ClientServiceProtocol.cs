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
using System.Drawing.Drawing2D;

namespace SysCAD.Protocol
{
  [Serializable]
  public sealed class ClientServiceProtocol : BaseProtocol
  {
    private Int64 requestId;
    private Int64 eventId;

    public delegate bool ChangeStateHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, RunStates runState);

    public delegate void GetPropertyValuesHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, ref ArrayList propertyList);
    public delegate void GetSubTagsHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, String propertyPath, out ArrayList propertyList);

    public delegate bool CreateItemHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool ModifyItemHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool ModifyItemPathHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid, String path);
    public delegate bool DeleteItemHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid);

    public delegate bool CreateLinkHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool ModifyLinkHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool DeleteLinkHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid);

    public delegate bool CreateThingHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);
    public delegate bool ModifyThingHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);
    public delegate bool ModifyThingPathHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid, String path);
    public delegate bool DeleteThingHandler(ClientServiceProtocol clientServiceProtocol, Int64 requestId, Guid guid);

    public delegate PortStatus PortCheckHandler(ClientServiceProtocol clientServiceProtocol, Guid itemGuid, Anchor anchor);

    public delegate ArrayList PropertyListHandler(ClientServiceProtocol clientServiceProtocol, Guid guid, String tag, String path);



    private ChangeStateHandler changeStateHandler;

    private GetPropertyValuesHandler getPropertyValuesHandler;
    private GetSubTagsHandler getSubTagsHandler;

    private CreateItemHandler createItemHandler;
    private ModifyItemHandler modifyItemHandler;
    private ModifyItemPathHandler modifyItemPathHandler;
    private DeleteItemHandler deleteItemHandler;

    private CreateLinkHandler createLinkHandler;
    private ModifyLinkHandler modifyLinkHandler;
    private DeleteLinkHandler deleteLinkHandler;

    private CreateThingHandler createThingHandler;
    private ModifyThingHandler modifyThingHandler;
    private ModifyThingPathHandler modifyThingPathHandler;
    private DeleteThingHandler deleteThingHandler;

    private PortCheckHandler portCheckHandler;

    private PropertyListHandler propertyListHandler;

    public ClientServiceProtocol(String name, 
      Dictionary<Guid, GraphicLink> graphicLinks, Dictionary<Guid, GraphicItem> graphicItems, Dictionary<Guid, GraphicThing> graphicThings,
      ChangeStateHandler changeStateHandler, GetPropertyValuesHandler getPropertyValuesHandler, GetSubTagsHandler getSubTagsHandler,
      CreateItemHandler createItemHandler, ModifyItemHandler modifyItemHandler, ModifyItemPathHandler modifyItemPathHandler, DeleteItemHandler deleteItemHandler,
      CreateLinkHandler createLinkHandler, ModifyLinkHandler modifyLinkHandler, DeleteLinkHandler deleteLinkHandler,
      CreateThingHandler createThingHandler, ModifyThingHandler modifyThingHandler, ModifyThingPathHandler modifyThingPathHandler, DeleteThingHandler deleteThingHandler,
      PortCheckHandler portCheckHandler, PropertyListHandler propertyListHandler)
    {
      this.Name = name;

      this.graphicLinks = graphicLinks;
      this.graphicItems = graphicItems;
      this.graphicThings = graphicThings;

      this.changeStateHandler = changeStateHandler;

      this.getPropertyValuesHandler = getPropertyValuesHandler;
      this.getSubTagsHandler = getSubTagsHandler;

      this.createItemHandler = createItemHandler;
      this.modifyItemHandler = modifyItemHandler;
      this.modifyItemPathHandler = modifyItemPathHandler;
      this.deleteItemHandler = deleteItemHandler;

      this.createLinkHandler = createLinkHandler;
      this.modifyLinkHandler = modifyLinkHandler;
      this.deleteLinkHandler = deleteLinkHandler;

      this.createThingHandler = createThingHandler;
      this.modifyThingHandler = modifyThingHandler;
      this.modifyThingPathHandler = modifyThingPathHandler;
      this.deleteThingHandler = deleteThingHandler;

      this.portCheckHandler = portCheckHandler;

      this.propertyListHandler = propertyListHandler;
    }

    public bool ChangeState(out Int64 requestId, RunStates runState)
    {
      this.requestId++;
      requestId = this.requestId;
      throw new NotImplementedException("The method or operation is not implemented.");
      return changeStateHandler(this, requestId, runState);
    }

    public void GetPropertyValues(out Int64 requestId, ref ArrayList tagPathList)
    {
      this.requestId++;
      requestId = this.requestId;
      throw new NotImplementedException("The method or operation is not implemented.");
      getPropertyValuesHandler(this, requestId, ref tagPathList);
    }

    public void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      this.requestId++;
      requestId = this.requestId;
      getSubTagsHandler(this, requestId, propertyPath, out propertyList);
    }

    public bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;
      guid = Guid.NewGuid();
      return createItemHandler(this, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicItems.ContainsKey(guid))
        return modifyItemHandler(this, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
      else
        return false;
    }

    public bool ModifyItemPath(out Int64 requestId, Guid guid, String path)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicItems.ContainsKey(guid))
        return modifyItemPathHandler(this, requestId, guid, path);
      else
        return false;
    }

    public bool DeleteItem(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicItems.ContainsKey(guid))
        return deleteItemHandler(this, requestId, guid);
      else
        return false;
    }

    public bool CreateLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestId++;
      requestId = this.requestId;
      guid = new Guid();
      return createLinkHandler(this, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool ModifyLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicLinks.ContainsKey(guid))
        return modifyLinkHandler(this, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      else
        return false;
    }

    public bool DeleteLink(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicLinks.ContainsKey(guid))
        return deleteLinkHandler(this, requestId, guid);
      else
        return false;
    }


    public bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;
      guid = Guid.NewGuid();
      return createThingHandler(this, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicThings.ContainsKey(guid))
        return modifyThingHandler(this, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      else
        return false;
    }

    public bool ModifyThingPath(out Int64 requestId, Guid guid, String path)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicThings.ContainsKey(guid))
        return modifyThingPathHandler(this, requestId, guid, path);
      else
        return false;
    }

    public bool DeleteThing(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicThings.ContainsKey(guid))
        return deleteThingHandler(this, requestId, guid);
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


    public void DoStateChanged(Int64 requestId, RunStates runState)
    {
      eventId++;
      OnStateChanged(eventId, requestId, runState);
    }


    public void DoStep(Int64 step, DateTime time)
    {
      eventId++;
      OnStep(eventId, step, time);
    }


    public void DoSync()
    {
      eventId++;
      OnSync(eventId);
    }


    public void DoItemCreated(Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
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
        graphicItem.FillMode = fillMode;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        eventId++;
        OnItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoItemModified(Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
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
        graphicItem.FillMode = fillMode;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        eventId++;
        OnItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoItemBoundingRectModified(Int64 requestId, Guid guid, RectangleF boundingRect)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.BoundingRect = boundingRect;

        eventId++;
        OnItemModified(eventId, requestId, guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }
    }

    public void DoItemPathModified(Int64 requestId, Guid guid, String path)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.Path = path;

        eventId++;
        OnItemModified(eventId, requestId, guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }
    }

    public void DoItemDeleted(Int64 requestId, Guid guid)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        foreach (GraphicLink graphicLink in graphicLinks.Values)
        {
          if (graphicLink.Origin == guid)
          {
            if (graphicLink.Destination == Guid.Empty) // it isn't connected to anything on the other end.
            {
              DoLinkDeleted(requestId, graphicLink.Guid);
            }
            else
            {
              DoLinkModified(requestId, graphicLink.Guid, graphicLink.Tag, graphicLink.ClassID, new Guid(), graphicLink.Destination, null, graphicLink.DestinationPort, graphicLink.ControlPoints);
            }
          }

          if (graphicLink.Destination == guid)
          {
            if (graphicLink.Origin == Guid.Empty) // it isn't connected to anything on the other end.
            {
              DoLinkDeleted(requestId, graphicLink.Guid);
            }
            else
            {
              DoLinkModified(requestId, graphicLink.Guid, graphicLink.Tag, graphicLink.ClassID, graphicLink.Origin, new Guid(), graphicLink.OriginPort, null, graphicLink.ControlPoints);
            }
          }
        }

        graphicItems.Remove(guid);

        eventId++;
        OnItemDeleted(eventId, requestId, guid);
      }
    }

    public void DoLinkCreated(Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (!graphicLinks.ContainsKey(guid))
      {
        GraphicLink graphicLink = new GraphicLink(guid, tag, classId, origin, originPort, destination, destinationPort);

        graphicLink.ControlPoints = new List<PointF>();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.ControlPoints.Add(controlPoint);

        graphicLinks.Add(guid, graphicLink);

        eventId++;
        OnLinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void DoLinkModified(Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      GraphicLink graphicLink;
      if (graphicLinks.TryGetValue(guid, out graphicLink))
      {
        graphicLink.Tag = tag;
        graphicLink.ClassID = classId;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        graphicLink.OriginPort = originPort;
        graphicLink.DestinationPort = destinationPort;

        graphicLink.ControlPoints.Clear();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.ControlPoints.Add(controlPoint);

        eventId++;
        OnLinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void DoLinkControlPointsModified(Int64 requestId, Guid guid, List<PointF> controlPoints)
    {
      GraphicLink graphicLink;
      if (graphicLinks.TryGetValue(guid, out graphicLink))
      {
        graphicLink.ControlPoints.Clear();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.ControlPoints.Add(controlPoint);

        eventId++;
        OnLinkModified(eventId, requestId, guid, graphicLink.Tag, graphicLink.ClassID, graphicLink.Origin, graphicLink.Destination, graphicLink.OriginPort, graphicLink.DestinationPort, graphicLink.ControlPoints);
      }
    }

    public void DoLinkDeleted(Int64 requestId, Guid guid)
    {
      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        eventId++;
        OnLinkDeleted(eventId, requestId, guid);
      }
    }


    public void DoThingCreated(Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (!graphicThings.ContainsKey(guid))
      {
        GraphicThing graphicThing = new GraphicThing(guid, tag);
        graphicThing.Path = path;
        graphicThing.BoundingRect = (ARectangleF)boundingRect;
        graphicThing.Xaml = xaml;
        graphicThing.Angle = angle;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        graphicThings.Add(guid, graphicThing);

        eventId++;
        OnThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void DoThingModified(Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      GraphicThing graphicThing;
      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Tag = tag;
        graphicThing.Path = path;
        graphicThing.BoundingRect = (ARectangleF)boundingRect;
        graphicThing.Xaml = xaml;
        graphicThing.Angle = angle;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        eventId++;
        OnThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void DoThingPathModified(Int64 requestId, Guid guid, String path)
    {
      GraphicThing graphicThing;
      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Path = path;

        eventId++;
        OnThingModified(eventId, requestId, guid, graphicThing.Tag, graphicThing.Path, graphicThing.BoundingRect, graphicThing.Xaml, graphicThing.Angle, graphicThing.MirrorX, graphicThing.MirrorY);
      }
    }

    public void DoThingDeleted(Int64 requestId, Guid guid)
    {
      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        eventId++;
        OnThingDeleted(eventId, requestId, guid);
      }
    }

  }
}
