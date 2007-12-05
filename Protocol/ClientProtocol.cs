
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
using System.Collections.ObjectModel;

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

    //public void ServiceGraphicGroupCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    //{
    //  if (!graphicGroups.ContainsKey(guid))
    //  {
    //    GraphicGroup graphicGroup = new GraphicGroup(guid, tag);
    //    graphicGroup.Path = path;
    //    graphicGroup.BoundingRect = boundingRect;
    //    graphicGroups.Add(guid, graphicGroup);

    //    //OnGroupCreated(eventId, requestId, guid, tag, path, boundingRect);
    //  }
    //}

    public void ServiceGraphicGroupDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void ServiceGraphicGroupModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    //public void ServiceGraphicItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, NodeClass model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle tagArea, Double tagAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    //{

    //  if (!graphicNodes.ContainsKey(guid))
    //  {
    //    GraphicNode graphicNode = new GraphicNode();
    //    graphicNode.Guid = guid;
    //    graphicNode.Tag = tag;
    //    graphicNode.Path = path;
    //    graphicNode.NodeClass = model;
    //    graphicNode.Shape = stencil;
    //    graphicNode.BoundingRect = boundingRect;
    //    graphicNode.Angle = angle;
    //    graphicNode.TagArea = tagArea;
    //    graphicNode.TagAngle = tagAngle;
    //    graphicNode.FillColor = fillColor;
    //    graphicNode.MirrorX = mirrorX;
    //    graphicNode.MirrorY = mirrorY;

    //    graphicNodes.Add(guid, graphicNode);

    //    //OnItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, tagArea, tagAngle, fillColor, mirrorX, mirrorY);
    //  }
    //}

    //public void ServiceGraphicItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //{

    //  if (graphicNodes.ContainsKey(guid))
    //  {
    //    graphicNodes.Remove(guid);

    //    //OnItemDeleted(eventId, requestId, guid);
    //  }
    //}

    public void ServiceGraphicPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      OnPortInfoRequested(eventId, requestId, guid, tag, portInfo);
    }

    //public void ServiceGraphicItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, NodeClass model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle tagArea, Double tagAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    //{
    //  GraphicNode graphicItem;

    //  if (graphicNodes.TryGetValue(guid, out graphicItem))
    //  {
    //    graphicItem.Tag = tag;
    //    graphicItem.Path = path;
    //    graphicItem.NodeClass = model;
    //    graphicItem.Shape = stencil;
    //    graphicItem.BoundingRect = boundingRect;
    //    graphicItem.Angle = angle;
    //    graphicItem.TagArea = tagArea;
    //    graphicItem.TagAngle = tagAngle;
    //    graphicItem.FillColor = fillColor;
    //    graphicItem.MirrorX = mirrorX;
    //    graphicItem.MirrorY = mirrorY;

    //    //OnItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, tagArea, tagAngle, fillColor, mirrorX, mirrorY);
    //  }
    //}

    //public void ServiceGraphicLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, Int16 originPortID, String destinationPort, Int16 destinationPortID, List<Point> controlPoints, Rectangle tagArea, Double tagAngle)
    //{

    //  if (!graphicLinks.ContainsKey(guid))
    //  {
    //    GraphicLink graphicLink = new GraphicLink(guid, tag, classId, origin, originPort, originPortID, destination, destinationPort, destinationPortID, controlPoints, tagArea, tagAngle);
    //    graphicLinks.Add(guid, graphicLink);

    //    //OnLinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, originPortID, destinationPort, destinationPortID, controlPoints, tagArea, tagAngle);
    //  }
    //}

    //public void ServiceGraphicLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //{

    //  if (graphicLinks.ContainsKey(guid))
    //  {
    //    graphicLinks.Remove(guid);

    //    //OnLinkDeleted(eventId, requestId, guid);
    //  }
    //}

    //public void ServiceGraphicLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<Point> controlPoints, Rectangle tagArea, Double tagAngle)
    //{
    //  GraphicLink graphicLink;

    //  if (graphicLinks.TryGetValue(guid, out graphicLink))
    //  {
    //    graphicLink.Tag = tag;
    //    graphicLink.ModelLink = new ModelLink();
    //    graphicLink.Origin = origin;
    //    graphicLink.Destination = destination;
    //    graphicLink.OriginPort = originPort;
    //    graphicLink.DestinationPort = destinationPort;

    //    graphicLink.ControlPoints.Clear();

    //    foreach (Point controlPoint in controlPoints)
    //      graphicLink.ControlPoints.Add(controlPoint);

    //    graphicLink.TagArea = tagArea;
    //    graphicLink.TagAngle = tagAngle;

    //    //OnLinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints, tagArea, tagAngle);
    //  }
    //}

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
          graphic.Create(serviceGraphic.graphic.Get(guid));
        }
      }

      if (actioned.Modified != null)
      {
        foreach (Guid guid in actioned.Modified)
        {
          graphic.Modify(serviceGraphic.graphic.Get(guid));
        }
      }

      if (actioned.Deleted != null)
      {
        foreach (Guid guid in actioned.Deleted)
        {
          graphic.Delete(guid);
        }
      }

      OnChanged(eventId, requestId, actioned);
    }

    public void ServiceGraphicStep(Int64 eventId, Int64 step, DateTime time)
    {
      OnStep(eventId, step, time);
    }

    public void ServiceGraphicSync(Int64 eventId)
    {
      OnSync(eventId);
    }

    //public void ServiceGraphicThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    //{

    //  if (!graphicThings.ContainsKey(guid))
    //  {
    //    GraphicThing graphicThing = new GraphicThing(guid, tag);
    //    graphicThing.Path = path;
    //    graphicThing.BoundingRect = boundingRect;
    //    graphicThing.Xaml = xaml;
    //    graphicThing.Angle = angle;
    //    graphicThing.MirrorX = mirrorX;
    //    graphicThing.MirrorY = mirrorY;

    //    graphicThings.Add(guid, graphicThing);

    //    //OnThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    //  }
    //}

    //public void ServiceGraphicThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //{

    //  if (graphicThings.ContainsKey(guid))
    //  {
    //    graphicThings.Remove(guid);

    //    //OnThingDeleted(eventId, requestId, guid);
    //  }
    //}

    //public void ServiceGraphicThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    //{
    //  GraphicThing graphicThing;

    //  if (graphicThings.TryGetValue(guid, out graphicThing))
    //  {
    //    graphicThing.Tag = tag;
    //    graphicThing.Path = path;
    //    graphicThing.BoundingRect = boundingRect;
    //    graphicThing.Xaml = xaml;
    //    graphicThing.Angle = angle;
    //    graphicThing.MirrorX = mirrorX;
    //    graphicThing.MirrorY = mirrorY;

    //    //OnThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    //  }
    //}

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
