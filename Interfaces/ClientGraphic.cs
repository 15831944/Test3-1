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
        serviceGraphic.ItemCreated -= new ServiceGraphic.ItemCreatedHandler(serviceGraphic_ItemCreated);
        serviceGraphic.ItemModified -= new ServiceGraphic.ItemModifiedHandler(serviceGraphic_ItemModified);
        serviceGraphic.ItemDeleted -= new ServiceGraphic.ItemDeletedHandler(serviceGraphic_ItemDeleted);

        serviceGraphic.LinkCreated -= new ServiceGraphic.LinkCreatedHandler(serviceGraphic_LinkCreated);
        serviceGraphic.LinkModified -= new ServiceGraphic.LinkModifiedHandler(serviceGraphic_LinkModified);
        serviceGraphic.LinkDeleted -= new ServiceGraphic.LinkDeletedHandler(serviceGraphic_LinkDeleted);
      }
    }



    public bool ModifyItem(out uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.ModifyItem(out requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public bool CreateItem(out uint requestID, out Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.CreateItem(out requestID, out guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public bool DeleteItem(out uint requestID, Guid guid)
    {
      return serviceGraphic.DeleteItem(out requestID, guid);
    }



    public bool ModifyLink(out uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return serviceGraphic.ModifyLink(out requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool CreateLink(out uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return serviceGraphic.CreateLink(out requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool DeleteLink(out uint requestID, Guid guid)
    {
      return serviceGraphic.DeleteLink(out requestID, guid);
    }



    public bool Connect(string URL)
    {
      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseGraphic), URL) as ServiceGraphic;

        name = serviceGraphic.Name; // Force a test of the connection.

        serviceGraphic.ItemCreated += new ServiceGraphic.ItemCreatedHandler(serviceGraphic_ItemCreated);
        serviceGraphic.ItemModified += new ServiceGraphic.ItemModifiedHandler(serviceGraphic_ItemModified);
        serviceGraphic.ItemDeleted += new ServiceGraphic.ItemDeletedHandler(serviceGraphic_ItemDeleted);

        serviceGraphic.LinkCreated += new ServiceGraphic.LinkCreatedHandler(serviceGraphic_LinkCreated);
        serviceGraphic.LinkModified += new ServiceGraphic.LinkModifiedHandler(serviceGraphic_LinkModified);
        serviceGraphic.LinkDeleted += new ServiceGraphic.LinkDeletedHandler(serviceGraphic_LinkDeleted);

        connectionError = "";
        return true;
      }
      catch (Exception exception)
      {
        connectionError = exception.Message;
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
    }



    public void serviceGraphic_ItemCreated(uint eventID, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (!graphicItems.ContainsKey(guid))
      {
        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = shape;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        OnItemModified(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void serviceGraphic_ItemModified(uint eventID, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.Tag = tag;
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = shape;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        OnItemModified(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void serviceGraphic_ItemDeleted(uint eventID, uint requestID, Guid guid)
    {
      if (graphicItems.ContainsKey(guid))
      {
        graphicItems.Remove(guid);

        OnItemDeleted(eventID, requestID, guid);
      }
    }



    public void serviceGraphic_LinkCreated(uint eventID, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (!graphicLinks.ContainsKey(guid))
      {
        GraphicLink graphicLink = new GraphicLink(guid, tag);
        graphicLink.ClassID = classID;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        graphicLink.OriginPort = originPort;
        graphicLink.DestinationPort = destinationPort;

        graphicLink.controlPoints = new List<PointF>();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.controlPoints.Add(controlPoint);

        graphicLinks.Add(guid, graphicLink);

        OnLinkCreated(eventID, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void serviceGraphic_LinkModified(uint eventID, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
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

        OnLinkModified(eventID, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void serviceGraphic_LinkDeleted(uint eventID, uint requestID, Guid guid)
    {
      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        OnLinkDeleted(eventID, requestID, guid);
      }
    }
  }
}