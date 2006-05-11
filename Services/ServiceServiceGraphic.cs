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
  public class ServiceServiceGraphic : ServiceGraphic
  {
    public ServiceServiceGraphic()
    {
    }

    ~ServiceServiceGraphic()
    {
    }

    protected override bool ModifyItem(uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      // decide whether to do it.
      // do it.
      // fire off ItemModified event.
      // return true or false.

      OnItemModified(requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);

      return true;
    }
  }
}