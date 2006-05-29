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

    public delegate bool CreateItemDelegate(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate bool ModifyItemDelegate(ServiceGraphic graphic, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate bool DeleteItemDelegate(ServiceGraphic graphic, uint requestID, Guid guid);

    public  CreateItemDelegate createItemDelegate;
    private ModifyItemDelegate modifyItemDelegate;
    private DeleteItemDelegate deleteItemDelegate;

    public ServiceGraphic(CreateItemDelegate createItemDelegate, ModifyItemDelegate modifyItemDelegate, DeleteItemDelegate deleteItemDelegate)
    {
      this.createItemDelegate = createItemDelegate;
      this.modifyItemDelegate = modifyItemDelegate;
      this.deleteItemDelegate = deleteItemDelegate;
    }

    ~ServiceGraphic()
    {
    }

    public bool CreateItem(out uint requestID, out Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      this.requestID++;
      requestID = this.requestID;
      guid = new Guid();
      return createItemDelegate(this, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public bool ModifyItem(out uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      this.requestID++;
      requestID = this.requestID;
      if (graphicItems.ContainsKey(guid))
        return modifyItemDelegate(this, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
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

    public void DoItemCreated(uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
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

      eventID++;
      OnItemCreated(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void DoItemModified(uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
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
      }

      eventID++;
      OnItemModified(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void DoItemDeleted(uint requestID, Guid guid)
    {
      graphicItems.Remove(guid);

      eventID++;
      OnItemDeleted(eventID, requestID, guid);
    }
  }
}