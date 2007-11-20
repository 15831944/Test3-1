
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

using System.Drawing.Drawing2D;

namespace SysCAD.Protocol
{

  [Serializable]
  public sealed class EngineProtocol : EngineBaseProtocol
  {

    private String connectionError = String.Empty;
    private EngineServiceProtocol serviceGraphic;

    public String engineName = String.Empty;

    private EngineServiceProtocol.PortInfoRequestedHandler serviceGraphicPortInfoRequestedHandler = null;

    Uri url = null;

    public EngineProtocol()
    {
    }

    public bool ChangeState(out Int64 requestId, RunState runState)
    {
      return serviceGraphic.ChangeState(out requestId, runState);
    }

    //[EnvironmentPermissionAttribute(SecurityAction.LinkDemand, Unrestricted = true)]
    public bool Connect(String engineName)
    {

      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as EngineServiceProtocol;

        Name = serviceGraphic.Name; // Force a test of the connection.

        serviceGraphicPortInfoRequestedHandler = new EngineServiceProtocol.PortInfoRequestedHandler(ServiceGraphicPortInfoRequested);

        serviceGraphic.PortInfoRequested += serviceGraphicPortInfoRequestedHandler;

        Syncxxx();

        this.engineName = Announce(engineName);

        connectionError = "";
        return true;
      }

      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        connectionError = remotingException.Message;
        return false;
      }
    }

    private string Announce(string name)
    {
      return serviceGraphic.Announce(name);
    }

    private void Renounce(string name)
    {
      try
      {
        serviceGraphic.Renounce(name);
      }
      catch (Exception)
      {
      }
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      serviceGraphic.LogMessage(out requestId, message, messageType);
    }

    public void Syncxxx()
    {
      MemoryStream memoryStream;
      BinaryFormatter bf = new BinaryFormatter();

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphic);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphic = bf.Deserialize(memoryStream) as Graphic;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.model);
      memoryStream.Seek(0, SeekOrigin.Begin);
      model = bf.Deserialize(memoryStream) as Model;
    }

    public bool TestUrl(Uri url)
    {

      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as EngineServiceProtocol;

        Name = serviceGraphic.Name; // Force a test of the connection.

        this.url = url;
        connectionError = "";
        return true;
      }

      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        url = null;
        connectionError = remotingException.Message;
        return false;
      }
    }

    public bool RequestPortInfo(out Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      return serviceGraphic.RequestPortInfo(out requestId, guid, tag, portInfo);
    }

    public void ServiceGraphicPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag)
    {
      OnPortInfoRequested(eventId, requestId, guid, tag);
    }

    ~EngineProtocol()
    {
      if (serviceGraphic != null)
      {
        Renounce(engineName);

        try
        {
          if (serviceGraphicPortInfoRequestedHandler != null) serviceGraphic.PortInfoRequested -= serviceGraphicPortInfoRequestedHandler;
        }
        catch (InvalidOperationException) { }

      }
    }
  }
}
