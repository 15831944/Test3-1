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
  public class BaseGraphic : MarshalByRefObject
  {
    public String name;

    public Dictionary<Guid, GraphicLink> graphicLinks;
    public Dictionary<Guid, GraphicItem> graphicItems;

    public BaseGraphic()
    {
      graphicLinks = new Dictionary<Guid, GraphicLink>();
      graphicItems = new Dictionary<Guid, GraphicItem>();
    }

    ~BaseGraphic()
    {
    }

    public String Name
    {
      get { return name; }
    }



    public delegate void ItemCreatedHandler(uint eventID, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ItemModifiedHandler(uint eventID, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ItemDeletedHandler(uint eventID, uint requestID, Guid guid);


    public delegate void LinkCreatedHandler(uint eventID, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate void LinkModifiedHandler(uint eventID, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate void LinkDeletedHandler(uint eventID, uint requestID, Guid guid);



    public ItemCreatedHandler ItemCreated;
    public ItemModifiedHandler ItemModified;
    public ItemDeletedHandler ItemDeleted;


    public LinkCreatedHandler LinkCreated;
    public LinkModifiedHandler LinkModified;
    public LinkDeletedHandler LinkDeleted;



    public void OnItemCreated(uint eventID, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemCreated != null)
        ItemCreated(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void OnItemModified(uint eventID, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemModified != null)
        ItemModified(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void OnItemDeleted(uint eventID, uint requestID, Guid guid)
    {
      if (ItemDeleted != null)
        ItemDeleted(eventID, requestID, guid);
    }



    public void OnLinkCreated(uint eventID, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkCreated != null)
        LinkCreated(eventID, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
    }

    public void OnLinkModified(uint eventID, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkModified != null)
        LinkModified(eventID, requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
    }

    public void OnLinkDeleted(uint eventID, uint requestID, Guid guid)
    {
      if (LinkDeleted != null)
        LinkDeleted(eventID, requestID, guid);
    }



    public override Object InitializeLifetimeService()
    {
      return null;
    }
  }
}