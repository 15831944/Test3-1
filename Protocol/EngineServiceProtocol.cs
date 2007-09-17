
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
    public delegate bool RequestPortInfoHandler(out Int64 requestId, Guid guid, String tag, PortInfo portInfo);

    private LogMessageHandler logMessageHandler;
    private StateChangedHandler stateChangedHandler;
    private RequestPortInfoHandler requestPortInfoHandler;

    public EngineServiceProtocol(String name,
      Dictionary<Guid, GraphicGroup> graphicGroups, Dictionary<Guid, GraphicLink> graphicLinks, Dictionary<Guid, GraphicItem> graphicItems, Dictionary<Guid, GraphicThing> graphicThings,
      LogMessageHandler logMessageHandler, StateChangedHandler stateChangedHandler, RequestPortInfoHandler requestPortInfoHandler)
    {
      this.Name = name;

      this.graphicGroups = graphicGroups;

      this.graphicLinks = graphicLinks;
      this.graphicItems = graphicItems;
      this.graphicThings = graphicThings;

      this.logMessageHandler = logMessageHandler;
      this.stateChangedHandler = stateChangedHandler;
      this.requestPortInfoHandler = requestPortInfoHandler;
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      logMessageHandler(out requestId, message, messageType);
    }

    public bool ChangeState(out long requestId, RunState runState)
    {
      return stateChangedHandler(out requestId, runState);
    }

    public bool RequestPortInfo(out Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      return requestPortInfoHandler(out requestId, guid, tag, portInfo);
    }

    public void DoPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag)
    {
      OnPortInfoRequested(eventId, requestId, guid, tag);
    }
  }
}
