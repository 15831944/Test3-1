using System;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using System.Runtime.Remoting.Lifetime;
using System.Drawing;
using System.Threading;

namespace SysCAD.Interface
{
  [Serializable]
  public class GraphicData : MarshalByRefObject
  {
    public String name;

    public Dictionary<string, Link> links;
    public Dictionary<string, Item> items;
    public Dictionary<string, Area> ___areas;

    public GraphicData()
    {
      links = new Dictionary<string, Link>();
      items = new Dictionary<string, Item>();
      ___areas = new Dictionary<string, Area>();
    }

    public String Name
    {
      get { return name; }
    }

    //string tempTag;
    //RectangleF tempBoundingRect;

    public void ModifyItem(string tag, RectangleF boundingRect)
    {
      Item item;
      if (items.TryGetValue(tag, out item))
      {
        item.X = boundingRect.X;
        item.Y = boundingRect.Y;
        item.Width = boundingRect.Width;
        item.Height = boundingRect.Height;

        //tempTag = tag;
        //tempBoundingRect = boundingRect;

        //Thread thr = new Thread(new ThreadStart(SendOnItemModified));
        //thr.Start();
      }
    }

    //public void SendOnItemModified()
    //{
    //  OnItemModified(tempTag, tempBoundingRect);
    //}

    //public delegate void ItemModifiedHandler(string tag, RectangleF boundingRect);
    //public ItemModifiedHandler ItemModified;

    //public void OnItemModified(string tag, RectangleF boundingRect)
    //{
    //  if (ItemModified != null)
    //  {
    //    ItemModified(tag, boundingRect);
    //  }
    //}

    public override Object InitializeLifetimeService()
    {
      return null;
    }
  }

//----------------------------

  public class Graphic : GraphicData
  {
    private GraphicData remoteGraphic;

    public string connectionError = "";

    public bool Connect(string URL)
    {
      try
      {
        remoteGraphic = Activator.GetObject(typeof(GraphicData), URL) as GraphicData;
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

      //remoteGraphic.ItemModified += new GraphicData.ItemModifiedHandler(remoteGraphic_ItemModified);
    }

    //public void remoteGraphic_ItemModified(string tag, RectangleF boundingRect)
    //{
    //  Item item;
    //  if (items.TryGetValue(tag, out item))
    //  {
    //    item.X = boundingRect.X;
    //    item.Y = boundingRect.Y;
    //    item.Width = boundingRect.Width;
    //    item.height = boundingRect.Height;

    //    OnItemModified(tag, boundingRect);
    //  }
    //}

    //string tempTag;
    //RectangleF tempBoundingRect;

    //new public void ModifyItem(string tag, RectangleF boundingRect)
    //{
    //  Item item;
    //  if (items.TryGetValue(tag, out item))
    //  {
    //    item.X = boundingRect.X;
    //    item.Y = boundingRect.Y;
    //    item.Width = boundingRect.Width;
    //    item.height = boundingRect.Height;

    //    tempTag = tag;
    //    tempBoundingRect = boundingRect;

    //    Thread thr = new Thread(new ThreadStart(SendModifyItem));
    //    thr.Start();
    //  }
    //}

    //public void SendModifyItem()
    //{
    //  remoteGraphic.ModifyItem(tempTag, tempBoundingRect);
    //}
  }
}
