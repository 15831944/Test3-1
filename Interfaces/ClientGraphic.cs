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
using System.Security.Permissions;
using System.Drawing.Drawing2D;

namespace SysCAD.Interface
{
  [Serializable]
  public sealed class ClientGraphic : BaseGraphic
  {
    public ServiceGraphic serviceGraphic;

    public string connectionError = "";

    public ClientGraphic()
    {
    }

    ~ClientGraphic()
    {
      if (serviceGraphic != null)
      {
        serviceGraphic.ItemCreated -= new ServiceGraphic.ItemCreatedHandler(ServiceGraphicItemCreated);
        serviceGraphic.ItemModified -= new ServiceGraphic.ItemModifiedHandler(ServiceGraphicItemModified);
        serviceGraphic.ItemDeleted -= new ServiceGraphic.ItemDeletedHandler(ServiceGraphicItemDeleted);

        serviceGraphic.LinkCreated -= new ServiceGraphic.LinkCreatedHandler(ServiceGraphicLinkCreated);
        serviceGraphic.LinkModified -= new ServiceGraphic.LinkModifiedHandler(ServiceGraphicLinkModified);
        serviceGraphic.LinkDeleted -= new ServiceGraphic.LinkDeletedHandler(ServiceGraphicLinkDeleted);

        serviceGraphic.ThingCreated -= new ServiceGraphic.ThingCreatedHandler(ServiceGraphicThingCreated);
        serviceGraphic.ThingModified -= new ServiceGraphic.ThingModifiedHandler(ServiceGraphicThingModified);
        serviceGraphic.ThingDeleted -= new ServiceGraphic.ThingDeletedHandler(ServiceGraphicThingDeleted);
      }
    }



    public bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.ModifyItem(out requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.CreateItem(out requestId, out guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool DeleteItem(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteItem(out requestId, guid);
    }

    
    public bool ModifyLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return serviceGraphic.ModifyLink(out requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool CreateLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return serviceGraphic.CreateLink(out requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool DeleteLink(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteLink(out requestId, guid);
    }


    public bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.ModifyThing(out requestId, guid, tag, path, boundingRect, angle, fillColor, elements, decorations, textArea, fillMode, mirrorX, mirrorY);
    }

    public bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.CreateThing(out requestId, out guid, tag, path, boundingRect, angle, fillColor, elements, decorations, textArea, fillMode, mirrorX, mirrorY);
    }

    public bool DeleteThing(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteThing(out requestId, guid);
    }


    public PortStatus PortCheck(Guid itemGuid, Anchor anchor)
    {
      return serviceGraphic.PortCheck(itemGuid, anchor);
    }


    public ArrayList PropertyList(Guid guid, String tag, String path)
    {
      return serviceGraphic.PropertyList(guid, tag, path);
    }




    [EnvironmentPermissionAttribute(SecurityAction.LinkDemand, Unrestricted = true)]
    public bool Connect(Uri url)
    {
      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseGraphic), url.ToString()) as ServiceGraphic;

        Name = serviceGraphic.Name; // Force a test of the connection.

        serviceGraphic.ItemCreated += new ServiceGraphic.ItemCreatedHandler(ServiceGraphicItemCreated);
        serviceGraphic.ItemModified += new ServiceGraphic.ItemModifiedHandler(ServiceGraphicItemModified);
        serviceGraphic.ItemDeleted += new ServiceGraphic.ItemDeletedHandler(ServiceGraphicItemDeleted);

        serviceGraphic.LinkCreated += new ServiceGraphic.LinkCreatedHandler(ServiceGraphicLinkCreated);
        serviceGraphic.LinkModified += new ServiceGraphic.LinkModifiedHandler(ServiceGraphicLinkModified);
        serviceGraphic.LinkDeleted += new ServiceGraphic.LinkDeletedHandler(ServiceGraphicLinkDeleted);

        Sync();

        connectionError = "";
        return true;
      }
      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        connectionError = remotingException.Message;
        return false;
      }
    }

    public void Sync()
    {
      MemoryStream memoryStream;
      BinaryFormatter bf = new BinaryFormatter();

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicLinks);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicLinks = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicLink>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicItems);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicItems = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicItem>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicThings);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicThings = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicThing>;
    }



    public void ServiceGraphicItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (!graphicItems.ContainsKey(guid))
      {
        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        OnItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.Tag = tag;
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        OnItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (graphicItems.ContainsKey(guid))
      {
        graphicItems.Remove(guid);

        OnItemDeleted(eventId, requestId, guid);
      }
    }



    public void ServiceGraphicLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (!graphicLinks.ContainsKey(guid))
      {
        GraphicLink graphicLink = new GraphicLink(guid, tag, classId, origin, originPort, destination, destinationPort);
        graphicLink.ClassID = classId;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        graphicLink.OriginPort = originPort;
        graphicLink.DestinationPort = destinationPort;

        graphicLink.controlPoints = new List<PointF>();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.controlPoints.Add(controlPoint);

        graphicLinks.Add(guid, graphicLink);

        OnLinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void ServiceGraphicLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
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

        graphicLink.controlPoints.Clear();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.controlPoints.Add(controlPoint);

        OnLinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void ServiceGraphicLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        OnLinkDeleted(eventId, requestId, guid);
      }
    }



    public void ServiceGraphicThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (!graphicThings.ContainsKey(guid))
      {
        GraphicThing graphicThing = new GraphicThing(guid, tag);
        graphicThing.Path = path;
        graphicThing.BoundingRect = boundingRect;
        graphicThing.Angle = angle;
        graphicThing.FillColor = fillColor;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        graphicThings.Add(guid, graphicThing);

        OnThingCreated(eventId, requestId, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      GraphicThing graphicThing;
      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Tag = tag;
        graphicThing.Path = path;
        graphicThing.BoundingRect = boundingRect;
        graphicThing.Angle = angle;
        graphicThing.FillColor = fillColor;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        OnThingModified(eventId, requestId, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        OnThingDeleted(eventId, requestId, guid);
      }
    }
  }
}