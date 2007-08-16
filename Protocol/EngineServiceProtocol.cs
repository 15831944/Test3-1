
using System;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using System.Runtime.Remoting.Lifetime;
using System.Drawing;
using System.Threading;

//using System.Runtime.Remoting.Channels.Ipc;
//using System.Runtime.Remoting.Channels;
using System.Collections;

//using System.Runtime.Remoting.Channels.Tcp;
using System.Drawing.Drawing2D;
using System.Runtime.Serialization.Formatters.Soap;

namespace SysCAD.Protocol
{

  [Serializable]
  public sealed class EngineServiceProtocol : EngineBaseProtocol
  {
    public delegate void LogMessageHandler(out Int64 requestId, String message, SysCAD.Log.MessageType messageType);
    public delegate bool StateChangedHandler(out long requestId, RunState runState);

    private LogMessageHandler logMessageHandler;
    private StateChangedHandler stateChangedHandler;

    public EngineServiceProtocol(String name,
      Dictionary<Guid, GraphicGroup> graphicGroups, Dictionary<Guid, GraphicLink> graphicLinks, Dictionary<Guid, GraphicItem> graphicItems, Dictionary<Guid, GraphicThing> graphicThings,
      LogMessageHandler logMessageHandler, StateChangedHandler stateChangedHandler)
    {
      this.Name = name;

      this.graphicGroups = graphicGroups;

      this.graphicLinks = graphicLinks;
      this.graphicItems = graphicItems;
      this.graphicThings = graphicThings;

      this.logMessageHandler = logMessageHandler;
      this.stateChangedHandler = stateChangedHandler;
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      logMessageHandler(out requestId, message, messageType);
    }

    public bool ChangeState(out long requestId, RunState runState)
    {
      return stateChangedHandler(out requestId, runState);
    }
  }
}
