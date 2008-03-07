
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

//using System.Security.Permissions;
using System.Drawing.Drawing2D;
using System.Runtime.Remoting;

namespace SysCAD.Protocol
{
  [Serializable]
  public class ConnectionLostException : SystemException
  {
    Exception originalException = null;
    Exception subsequentException = null;

    public ConnectionLostException(Exception originalException, Exception subsequentException)
    {
      this.originalException = originalException;
      this.subsequentException = subsequentException;
    }
  }
  
  [Serializable]
  public sealed class ClientProtocol : ClientBaseProtocol
  {
    public String connectionError = String.Empty;
    public String clientName = String.Empty;

    private ClientServiceProtocol serviceGraphic;

    private ClientServiceProtocol.PortInfoRequestedHandler serviceGraphicPortInfoRequestedHandler = null;

    private ClientServiceProtocol.PermissionsChangedHandler serviceGraphicPermissionsChangedHandler = null;

    private ClientServiceProtocol.StepHandler serviceGraphicStepHandler = null;

    private ClientServiceProtocol.SyncHandler serviceGraphicSyncHandler = null;

    private ClientServiceProtocol.ChangedHandler serviceGraphicChangedHandler = null;

    Uri url = null;

    private Stack<Action> undoList = new Stack<Action>();
    private Stack<Action> redoList = new Stack<Action>();

    public bool UndoAvailable
    {
      get { return undoList.Count > 0; }
    }

    public bool RedoAvailable
    {
      get { return redoList.Count > 0; }
    }

    public ClientProtocol()
    {
    }

    //[EnvironmentPermissionAttribute(SecurityAction.LinkDemand, Unrestricted = true)]
    public bool Connect(String clientName)
    {
      this.clientName = clientName;
      return Connect();
    }

    public bool Connect()
    {
      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as ClientServiceProtocol;
        Name = serviceGraphic.Name; // Force a test of the connection.
        Path = serviceGraphic.Path;

        serviceGraphicPermissionsChangedHandler = new ClientServiceProtocol.PermissionsChangedHandler(ServiceGraphicPermissionsChanged);
        serviceGraphicStepHandler = new ClientServiceProtocol.StepHandler(ServiceGraphicStep);
        serviceGraphicSyncHandler = new ClientServiceProtocol.SyncHandler(ServiceGraphicSync);
        serviceGraphicPortInfoRequestedHandler = new ClientServiceProtocol.PortInfoRequestedHandler(ServiceGraphicPortInfoRequested);
        serviceGraphicChangedHandler = new ClientServiceProtocol.ChangedHandler(ServiceGraphicChanged);

        serviceGraphic.PermissionsChanged += serviceGraphicPermissionsChangedHandler;
        serviceGraphic.Changed += serviceGraphicChangedHandler;
        serviceGraphic.Step += serviceGraphicStepHandler;
        serviceGraphic.Sync += serviceGraphicSyncHandler;
        serviceGraphic.PortInfoRequested += serviceGraphicPortInfoRequestedHandler;
        Syncxxx();
        this.clientName = Announce(clientName);
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
      try
      {
        return serviceGraphic.Announce(name);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          return serviceGraphic.Announce(name);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    private void Renounce(string name)
    {
      try
      {
        serviceGraphic.Renounce(name);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          serviceGraphic.Renounce(name);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public bool Undo(out Int64 requestId)
    {
      Action undoAction = undoList.Pop();

      Action redoAction = new Action();
      foreach (Item item in undoAction.Modify)
      {
        if (item is GraphicNode)
          redoAction.Modify.Add(graphic.Nodes[item.Guid].Clone());
        if (item is GraphicLink)
          redoAction.Modify.Add(graphic.Links[item.Guid].Clone());
      }
      redoList.Push(redoAction);

      return serviceGraphic.Change(out requestId, undoAction);
    }

    public bool Redo(out Int64 requestId)
    {
      Action redoAction = redoList.Pop();
      
      Action undoAction = new Action();
      foreach (Item item in redoAction.Modify)
      {
        if (item is GraphicNode)
          undoAction.Modify.Add(graphic.Nodes[item.Guid].Clone());
        if (item is GraphicLink)
          undoAction.Modify.Add(graphic.Links[item.Guid].Clone());
      }
      undoList.Push(undoAction);

      return serviceGraphic.Change(out requestId, redoAction);
    }

    public bool Change(out Int64 requestId, Action action)
    {
      try
      {
        redoList.Clear();

        Action undoAction = new Action();
        foreach (Item item in action.Modify)
        {
          if (item is GraphicNode)
            undoAction.Modify.Add(graphic.Nodes[item.Guid].Clone());
          if (item is GraphicLink)
            undoAction.Modify.Add(graphic.Links[item.Guid].Clone());
        }
        undoList.Push(undoAction);

        return serviceGraphic.Change(out requestId, action);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          return serviceGraphic.Change(out requestId, action);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public void GetPropertyValues(out Int64 requestId, ref ArrayList tagPathList)
    {
      try
      {
        serviceGraphic.GetPropertyValues(out requestId, ref tagPathList);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          serviceGraphic.GetPropertyValues(out requestId, ref tagPathList);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      try
      {
        serviceGraphic.GetSubTags(out requestId, propertyPath, out propertyList);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          serviceGraphic.GetSubTags(out requestId, propertyPath, out propertyList);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public bool Load(out Int64 requestId)
    {
      try
      {
        return serviceGraphic.Load(out requestId);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          return serviceGraphic.Load(out requestId);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public bool RequestPortInfo(out Int64 requestId, Guid guid, String tag)
    {
      try
      {
        return serviceGraphic.RequestPortInfo(out requestId, guid, tag);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          return serviceGraphic.RequestPortInfo(out requestId, guid, tag);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public ArrayList PropertyList(out Int64 requestId, Guid guid, String tag, String path)
    {
      try
      {
        return serviceGraphic.PropertyList(out requestId, guid, tag, path);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          return serviceGraphic.PropertyList(out requestId, guid, tag, path);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      try
      {
        serviceGraphic.LogMessage(out requestId, message, messageType);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          serviceGraphic.LogMessage(out requestId, message, messageType);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public bool Save(out Int64 requestId)
    {
      try
      {
        return serviceGraphic.Save(out requestId);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          return serviceGraphic.Save(out requestId);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    public bool SaveAs(String name, String path, out Int64 requestId)
    {
      try
      {
        return serviceGraphic.SaveAs(name, path, out requestId);
      }
      catch (Exception originalException)
      {
        try
        {
          Connect();
          return serviceGraphic.SaveAs(name, path, out requestId);
        }
        catch (Exception subsequentException)
        {
          throw new ConnectionLostException(originalException, subsequentException);
        }
      }
    }

    //public void ServiceGraphicAreaCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    //{
    //  if (!graphicAreas.ContainsKey(guid))
    //  {
    //    GraphicArea graphicArea = new GraphicArea(guid, tag);
    //    graphicArea.Path = path;
    //    graphicArea.BoundingRect = boundingRect;
    //    graphicAreas.Add(guid, graphicArea);

    //    //OnAreaCreated(eventId, requestId, guid, tag, path, boundingRect);
    //  }
    //}

    public void ServiceGraphicAreaDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void ServiceGraphicAreaModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void ServiceGraphicPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      OnPortInfoRequested(eventId, requestId, guid, tag, portInfo);
    }

    public void ServiceGraphicPermissionsChanged(Int64 eventId, Int64 requestId, Permissions permissions)
    {
      OnPermissionsChanged(eventId, requestId, permissions);
    }

    public void ServiceGraphicChanged(Int64 eventId, Int64 requestId, Actioned actioned)
      {
      if (actioned.Created != null)
        {
        foreach (Guid guid in actioned.Created)
          {
          Item item = serviceGraphic.Get(guid);

          if (item is GraphicItem)
            graphic.Create(item as GraphicItem);

          if (item is ModelItem)
            model.Create(item as ModelItem);
          }

        if (actioned.Modified != null)
          {
          foreach (Guid guid in actioned.Modified)
            {
            Item item = serviceGraphic.Get(guid);

            if (item is GraphicItem)
              graphic.Modify(item as GraphicItem);

          if (item is ModelItem)
            model.Modify(item as ModelItem);
            }
          }

        if (actioned.Deleted != null)
          {
          foreach (Guid guid in actioned.Deleted)
            {
            if (graphic.Get(guid) != null)
              graphic.Delete(guid);

            if (model.Get(guid) != null)
              model.Delete(guid);
            }
          }

        OnChanged(eventId, requestId, actioned);
        }
      }

    public void ServiceGraphicStep(Int64 eventId, Int64 step, DateTime time)
    {
      OnStep(eventId, step, time);
    }

    public void ServiceGraphicSync(Int64 eventId)
    {
      OnSync(eventId);
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
        serviceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as ClientServiceProtocol;

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

    ~ClientProtocol()
    {
      if (serviceGraphic != null)
      {
        Renounce(clientName);

        try { if (serviceGraphicPermissionsChangedHandler != null) serviceGraphic.PermissionsChanged -= serviceGraphicPermissionsChangedHandler; }
        catch (InvalidOperationException) { }

        try { if (serviceGraphicChangedHandler != null) serviceGraphic.Changed -= serviceGraphicChangedHandler; }
        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicStepHandler != null) serviceGraphic.Step -= serviceGraphicStepHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicSyncHandler != null) serviceGraphic.Sync -= serviceGraphicSyncHandler;
        }

        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicPortInfoRequestedHandler != null) serviceGraphic.PortInfoRequested -= serviceGraphicPortInfoRequestedHandler;
        }
        catch (InvalidOperationException) { }
      }
    }
  }
}
