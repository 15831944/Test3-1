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
        serviceGraphic.ItemModified -= new ServiceGraphic.ItemModifiedHandler(remoteGraphic_ItemModified);
    }

    public bool ModifyItem(out uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.ModifyItem(out requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public bool Connect(string URL)
    {
      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseGraphic), URL) as ServiceGraphic;

        name = serviceGraphic.Name; // Force a test of the connection.
        serviceGraphic.ItemModified += new ServiceGraphic.ItemModifiedHandler(remoteGraphic_ItemModified);

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

    public void remoteGraphic_ItemModified(uint eventID, uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.X = boundingRect.X;
        graphicItem.Y = boundingRect.Y;
        graphicItem.Width = boundingRect.Width;
        graphicItem.Height = boundingRect.Height;
        graphicItem.Angle = angle;

        OnItemModified(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }
  }
}