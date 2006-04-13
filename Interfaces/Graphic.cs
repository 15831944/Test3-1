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
  public class Graphic : MarshalByRefObject
  {
    public Graphic remoteGraphic;

    public string connectionError = "";

    public String name;

    public Dictionary<string, GraphicLink> graphicLinks;
    public Dictionary<string, GraphicItem> graphicItems;
    public Dictionary<string, GraphicArea> ___graphicAreas;

    public Graphic()
    {
      graphicLinks = new Dictionary<string, GraphicLink>();
      graphicItems = new Dictionary<string, GraphicItem>();
      ___graphicAreas = new Dictionary<string, GraphicArea>();
    }

    ~Graphic()
    {
      if (remoteGraphic != null)
        remoteGraphic.ItemModified -= new Graphic.ItemModifiedHandler(remoteGraphic_ItemModified);
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
        remoteGraphic.ItemModified += new Graphic.ItemModifiedHandler(remoteGraphic_ItemModified);

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
      bf.Serialize(memoryStream, remoteGraphic.graphicLinks);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicLinks = bf.Deserialize(memoryStream) as Dictionary<string, GraphicLink>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteGraphic.graphicItems);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicItems = bf.Deserialize(memoryStream) as Dictionary<string, GraphicItem>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteGraphic.___graphicAreas);
      memoryStream.Seek(0, SeekOrigin.Begin);
      ___graphicAreas = bf.Deserialize(memoryStream) as Dictionary<string, GraphicArea>;
    }

    public void remoteGraphic_ItemModified(string tag, RectangleF boundingRect, Single angle)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(tag, out graphicItem))
      {
        graphicItem.X = boundingRect.X;
        graphicItem.Y = boundingRect.Y;
        graphicItem.Width = boundingRect.Width;
        graphicItem.Height = boundingRect.Height;
        graphicItem.Angle = angle;

        OnItemModified(tag, boundingRect, angle);
      }
    }
  }
}
