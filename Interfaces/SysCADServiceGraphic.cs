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
  public class SysCADServiceGraphic : ServiceGraphic
  {
    public SysCADServiceGraphic()
    {
    }

    ~SysCADServiceGraphic()
    {
    }

    protected override bool ModifyItem(uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      eventID++;
      OnItemModified(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
      return true;
    }

    protected override bool CreateItem(uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      eventID++;
      OnItemCreated(eventID, requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
      return true;
    }

    protected override bool DeleteItem(uint requestID, Guid guid)
    {
      eventID++;
      OnItemDeleted(eventID, requestID, guid);
      return true;
    }
  }
}