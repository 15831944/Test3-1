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
//  public class GraphicData : MarshalByRefObject
//  {
//  }

//----------------------------

  public class Graphic : MarshalByRefObject
  {
    public Graphic remoteGraphic;

    public string connectionError = "";

    public String name;

    public Dictionary<string, Link> links;
    public Dictionary<string, Item> items;
    public Dictionary<string, Area> ___areas;

    public Graphic()
    {
      links = new Dictionary<string, Link>();
      items = new Dictionary<string, Item>();
      ___areas = new Dictionary<string, Area>();
    }

    public String Name
    {
      get { return name; }
    }

    public void ModifyItem(String tag, RectangleF boundingRect, Single angle)
    {
      remoteGraphic.OnItemModified(tag, boundingRect, angle);
    }

    public delegate void ItemModifiedHandler(string tag, RectangleF boundingRect, Single angle);

    public ItemModifiedHandler ItemModified;

    public void OnItemModified(string tag, RectangleF boundingRect, Single angle)
    {
      if (ItemModified != null)
      {
        ItemModified(tag, boundingRect, angle);
      }
    }

    public override Object InitializeLifetimeService()
    {
      return null;
    }

    public bool Connect(string URL)
    {
      try
      {
        remoteGraphic = Activator.GetObject(typeof(Graphic), URL) as Graphic;
        name = remoteGraphic.Name; // Force a test of the connection.
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
      bf.Serialize(memoryStream, remoteGraphic.links);
      memoryStream.Seek(0, SeekOrigin.Begin);
      links = bf.Deserialize(memoryStream) as Dictionary<string, Link>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteGraphic.items);
      memoryStream.Seek(0, SeekOrigin.Begin);
      items = bf.Deserialize(memoryStream) as Dictionary<string, Item>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteGraphic.___areas);
      memoryStream.Seek(0, SeekOrigin.Begin);
      ___areas = bf.Deserialize(memoryStream) as Dictionary<string, Area>;
    }

    public void remoteGraphic_ItemModified(string tag, RectangleF boundingRect, Single angle)
    {
      Item item;
      if (items.TryGetValue(tag, out item))
      {
        item.X = boundingRect.X;
        item.Y = boundingRect.Y;
        item.Width = boundingRect.Width;
        item.Height = boundingRect.Height;
        item.Angle = angle;

        OnItemModified(tag, boundingRect, angle);
      }
    }
  }
}
