
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
using System.Collections.ObjectModel;

namespace SysCAD.Protocol
{

  [Serializable]
  public sealed class ClientServiceProtocol : ClientBaseProtocol
  {
    private LoadHandler loadHandler;
    private SaveHandler saveHandler;

    private ChangePermissionsHandler clientChangePermissions;

    private ChangeHandler changeHandler;

    private GetPropertyValuesHandler getPropertyValuesHandler;
    private GetSubTagsHandler getSubTagsHandler;
    private RequestPortInfoHandler requestPortInfoHandler;

    private PropertyListHandler propertyListHandler;

    private LogMessageHandler logMessageHandler;

    private AnnounceHandler announceHandler;
    private RenounceHandler renounceHandler;

    public delegate bool LoadHandler(out Int64 requestId);
    public delegate bool SaveHandler(out Int64 requestId);

    public delegate bool ChangePermissionsHandler(out Int64 requestId, Permissions permissions);

    public delegate bool ChangeHandler(out Int64 requestId, Action action);

    public delegate void GetPropertyValuesHandler(out Int64 requestId, ref ArrayList propertyList);
    public delegate void GetSubTagsHandler(out Int64 requestId, String propertyPath, out ArrayList propertyList);

    public delegate bool RequestPortInfoHandler(out Int64 requestId, Guid guid, String tag);

    public delegate PortStatus PortCheckHandler(out Int64 requestId, Guid itemGuid, String tag);
    public delegate ArrayList PropertyListHandler(out Int64 requestId, Guid guid, String tag, String path);

    public delegate void LogMessageHandler(out Int64 requestId, String message, SysCAD.Log.MessageType messageType);

    public delegate String AnnounceHandler(String clientName);
    public delegate void RenounceHandler(String clientName);

    public ClientServiceProtocol(String name,
      LoadHandler loadHandler, 
      SaveHandler saveHandler, 
      Graphic graphic,
      Model model,
      ChangePermissionsHandler clientChangePermissions, 
      GetPropertyValuesHandler getPropertyValuesHandler, 
      GetSubTagsHandler getSubTagsHandler,
      ChangeHandler changeHandler,
      RequestPortInfoHandler requestPortInfoHandler,
      PropertyListHandler propertyListHandler, 
      LogMessageHandler logMessageHandler,
      AnnounceHandler announceHandler, 
      RenounceHandler renounceHandler)
    {
      this.Name = name;

      this.graphic = graphic;
      this.model = model;

      this.loadHandler = loadHandler;
      this.saveHandler = saveHandler;

      this.clientChangePermissions = clientChangePermissions;

      this.getPropertyValuesHandler = getPropertyValuesHandler;
      this.getSubTagsHandler = getSubTagsHandler;

      this.changeHandler = changeHandler;

      this.requestPortInfoHandler = requestPortInfoHandler;

      this.propertyListHandler = propertyListHandler;

      this.logMessageHandler = logMessageHandler;

      this.announceHandler = announceHandler;
      this.renounceHandler = renounceHandler;
    }

    public bool ChangePermissions(out Int64 requestId, Permissions permissions)
    {
      return clientChangePermissions(out requestId, permissions);
    }

    public bool Change(out Int64 requestId, Action action)
    {
      return changeHandler(out requestId, action);
    }

    public void DoChanged(Int64 eventId, Int64 requestId, Actioned actioned)
    {
      OnChanged(eventId, requestId, actioned);
    }

    public void DoPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      OnPortInfoRequested(eventId, requestId, guid, tag, portInfo);
    }

    public void DoPermissionsChanged(Int64 eventId, Int64 requestId, Permissions permissions)
    {
      OnPermissionsChanged(eventId, requestId, permissions);
    }

    public void DoStep(Int64 eventId, Int64 step, DateTime time)
    {
      OnStep(eventId, step, time);
    }

    public void DoSync(Int64 eventId)
    {
      OnSync(eventId);
    }

    public void GetPropertyValues(out Int64 requestId, ref ArrayList tagPathList)
    {
      getPropertyValuesHandler(out requestId, ref tagPathList);
    }

    public void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      getSubTagsHandler(out requestId, propertyPath, out propertyList);
    }

    public bool Load(out Int64 requestId)
    {
      return loadHandler(out requestId);
    }

    public bool RequestPortInfo(out Int64 requestId, Guid guid, String tag)
    {
      return requestPortInfoHandler(out requestId, guid, tag);
    }

    public ArrayList PropertyList(out Int64 requestId, Guid guid, String tag, String path)
    {
      //todo: check path is valid.
      return propertyListHandler(out requestId, guid, tag, path);
    }

    public bool Save(out Int64 requestId)
    {
      return saveHandler(out requestId);
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      logMessageHandler(out requestId, message, messageType);
    }

    public string Announce(string clientName)
    {
      return announceHandler(clientName);
    }

    public void Renounce(string clientName)
    {
      renounceHandler(clientName);
    }
  }
}
