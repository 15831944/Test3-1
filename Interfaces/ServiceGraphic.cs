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

    public delegate bool CreateItemDelegate(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate bool ModifyItemDelegate(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate bool DeleteItemDelegate(ServiceGraphic graphic, uint requestID, Guid guid);

    public delegate bool CreateLinkDelegate(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool ModifyLinkDelegate(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool DeleteLinkDelegate(ServiceGraphic graphic, uint requestID, Guid guid);

    public delegate PortStatus PortCheckDelegate(ServiceGraphic graphic, Guid itemGuid, Anchor anchor);

    private CreateItemDelegate createItemDelegate;
    private ModifyItemDelegate modifyItemDelegate;
    private DeleteItemDelegate deleteItemDelegate;

    private CreateLinkDelegate createLinkDelegate;
    private ModifyLinkDelegate modifyLinkDelegate;
    private DeleteLinkDelegate deleteLinkDelegate;

    private PortCheckDelegate portCheckDelegate;

    public ServiceGraphic(
      CreateItemDelegate createItemDelegate, ModifyItemDelegate modifyItemDelegate, DeleteItemDelegate deleteItemDelegate,
      CreateLinkDelegate createLinkDelegate, ModifyLinkDelegate modifyLinkDelegate, DeleteLinkDelegate deleteLinkDelegate,
      PortCheckDelegate portCheckDelegate)
    {
      this.createItemDelegate = createItemDelegate;
      this.modifyItemDelegate = modifyItemDelegate;
      this.deleteItemDelegate = deleteItemDelegate;

      this.createLinkDelegate = createLinkDelegate;
      this.modifyLinkDelegate = modifyLinkDelegate;
      this.deleteLinkDelegate = deleteLinkDelegate;

      this.portCheckDelegate = portCheckDelegate;
    }

    ~ServiceGraphic()
    {
    }

    public bool CreateItem(out uint requestID, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      this.requestID++;
      requestID = this.requestID;
      guid = Guid.NewGuid();
      return createItemDelegate(this, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public bool ModifyItem(out uint requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicItems.ContainsKey(guid))
        return modifyItemDelegate(this, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      else
        return false;
    }

    public bool DeleteItem(out uint requestID, Guid guid)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicItems.ContainsKey(guid))
        return deleteItemDelegate(this, requestID, guid);
      else
        return false;
    }

    public bool CreateLink(out uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestID++;
      requestID = this.requestID;
      guid = new Guid();
      return createLinkDelegate(this, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool ModifyLink(out uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicLinks.ContainsKey(guid))
        return modifyLinkDelegate(this, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
      else
        return false;
    }

    public bool DeleteLink(out uint requestID, Guid guid)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicLinks.ContainsKey(guid))
        return deleteLinkDelegate(this, requestID, guid);
      else
        return false;
    }

    
    public PortStatus PortCheck(Guid itemGuid, Anchor anchor)
    {
      if (graphicItems.ContainsKey(itemGuid))
        return portCheckDelegate(this, itemGuid, anchor);
      else
        return PortStatus.Unavailable;
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
  }
}