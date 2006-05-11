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

    public delegate void ItemModifiedHandler(uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);

    public ItemModifiedHandler ItemModified;

    public void OnItemModified(uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemModified != null)
      {
        ItemModified(requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }
  }
}