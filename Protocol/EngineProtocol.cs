
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
  public sealed class EngineProtocol : BaseProtocol
  {

    private String connectionError = String.Empty;
    private EngineServiceProtocol serviceGraphic;

    private EngineServiceProtocol.GroupCreatedHandler serviceGraphicGroupCreatedHandler = null;
    private EngineServiceProtocol.GroupDeletedHandler serviceGraphicGroupDeletedHandler = null;
    private EngineServiceProtocol.GroupModifiedHandler serviceGraphicGroupModifiedHandler = null;

    private EngineServiceProtocol.ItemCreatedHandler serviceGraphicItemCreatedHandler = null;
    private EngineServiceProtocol.ItemDeletedHandler serviceGraphicItemDeletedHandler = null;
    private EngineServiceProtocol.ItemModifiedHandler serviceGraphicItemModifiedHandler = null;
    private EngineServiceProtocol.ItemPathModifiedHandler serviceGraphicItemPathModifiedHandler = null;

    private EngineServiceProtocol.LinkCreatedHandler serviceGraphicLinkCreatedHandler = null;
    private EngineServiceProtocol.LinkDeletedHandler serviceGraphicLinkDeletedHandler = null;
    private EngineServiceProtocol.LinkModifiedHandler serviceGraphicLinkModifiedHandler = null;

    private EngineServiceProtocol.StateChangedHandler serviceGraphicStateChangedHandler = null;

    private EngineServiceProtocol.StepHandler serviceGraphicStepHandler = null;

    private EngineServiceProtocol.SyncHandler serviceGraphicSyncHandler = null;

    private EngineServiceProtocol.ThingCreatedHandler serviceGraphicThingCreatedHandler = null;
    private EngineServiceProtocol.ThingDeletedHandler serviceGraphicThingDeletedHandler = null;
    private EngineServiceProtocol.ThingModifiedHandler serviceGraphicThingModifiedHandler = null;

    Uri url = null;

    public EngineProtocol()
    {
    }

    public bool ChangeState(out Int64 requestId, RunStates runState)
    {
      return serviceGraphic.ChangeState(out requestId, runState);
    }

    //[EnvironmentPermissionAttribute(SecurityAction.LinkDemand, Unrestricted = true)]
    public bool Connect()
    {

      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as EngineServiceProtocol;

        Name = serviceGraphic.Name; // Force a test of the connection.

        serviceGraphicStateChangedHandler = new EngineServiceProtocol.StateChangedHandler(ServiceGraphicStateChanged);

        serviceGraphicStepHandler = new EngineServiceProtocol.StepHandler(ServiceGraphicStep);

        serviceGraphicSyncHandler = new EngineServiceProtocol.SyncHandler(ServiceGraphicSync);

        serviceGraphicGroupCreatedHandler = new EngineServiceProtocol.GroupCreatedHandler(ServiceGraphicGroupCreated);
        serviceGraphicGroupModifiedHandler = new EngineServiceProtocol.GroupModifiedHandler(ServiceGraphicGroupModified);
        serviceGraphicGroupDeletedHandler = new EngineServiceProtocol.GroupDeletedHandler(ServiceGraphicGroupDeleted);

        serviceGraphicItemCreatedHandler = new EngineServiceProtocol.ItemCreatedHandler(ServiceGraphicItemCreated);
        serviceGraphicItemModifiedHandler = new EngineServiceProtocol.ItemModifiedHandler(ServiceGraphicItemModified);
        serviceGraphicItemPathModifiedHandler = new EngineServiceProtocol.ItemPathModifiedHandler(ServiceGraphicItemPathModified);
        serviceGraphicItemDeletedHandler = new EngineServiceProtocol.ItemDeletedHandler(ServiceGraphicItemDeleted);

        serviceGraphicLinkCreatedHandler = new EngineServiceProtocol.LinkCreatedHandler(ServiceGraphicLinkCreated);
        serviceGraphicLinkModifiedHandler = new EngineServiceProtocol.LinkModifiedHandler(ServiceGraphicLinkModified);
        serviceGraphicLinkDeletedHandler = new EngineServiceProtocol.LinkDeletedHandler(ServiceGraphicLinkDeleted);

        serviceGraphicThingCreatedHandler = new EngineServiceProtocol.ThingCreatedHandler(ServiceGraphicThingCreated);
        serviceGraphicThingModifiedHandler = new EngineServiceProtocol.ThingModifiedHandler(ServiceGraphicThingModified);
        serviceGraphicThingDeletedHandler = new EngineServiceProtocol.ThingDeletedHandler(ServiceGraphicThingDeleted);

        serviceGraphic.StateChanged += serviceGraphicStateChangedHandler;

        serviceGraphic.Step += serviceGraphicStepHandler;

        serviceGraphic.Sync += serviceGraphicSyncHandler;

        serviceGraphic.GroupCreated += serviceGraphicGroupCreatedHandler;
        serviceGraphic.GroupModified += serviceGraphicGroupModifiedHandler;
        serviceGraphic.GroupDeleted += serviceGraphicGroupDeletedHandler;

        serviceGraphic.ItemCreated += serviceGraphicItemCreatedHandler;
        serviceGraphic.ItemModified += serviceGraphicItemModifiedHandler;
        serviceGraphic.ItemPathModified += serviceGraphicItemPathModifiedHandler;
        serviceGraphic.ItemDeleted += serviceGraphicItemDeletedHandler;

        serviceGraphic.LinkCreated += serviceGraphicLinkCreatedHandler;
        serviceGraphic.LinkModified += serviceGraphicLinkModifiedHandler;
        serviceGraphic.LinkDeleted += serviceGraphicLinkDeletedHandler;

        serviceGraphic.ThingCreated += serviceGraphicThingCreatedHandler;
        serviceGraphic.ThingModified += serviceGraphicThingModifiedHandler;
        serviceGraphic.ThingDeleted += serviceGraphicThingDeletedHandler;

        Syncxxx();

        connectionError = "";
        return true;
      }

      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        connectionError = remotingException.Message;
        return false;
      }
    }

    public bool CreateGroup(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect)
    {
      return serviceGraphic.CreateGroup(out requestId, out guid, tag, path, boundingRect);
    }

    public bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.CreateItem(out requestId, out guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool CreateLink(out Int64 requestId, out Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return serviceGraphic.CreateLink(out requestId, out guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.CreateThing(out requestId, out guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool DeleteGroup(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteGroup(out requestId, guid);
    }

    public bool DeleteItem(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteItem(out requestId, guid);
    }

    public bool DeleteLink(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteLink(out requestId, guid);
    }

    public bool DeleteThing(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteThing(out requestId, guid);
    }

    public void GetPropertyValues(out Int64 requestId, ref ArrayList tagPathList)
    {
      serviceGraphic.GetPropertyValues(out requestId, ref tagPathList);
    }

    public void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      serviceGraphic.GetSubTags(out requestId, propertyPath, out propertyList);
    }

    public bool Load(String filename)
    {
      return serviceGraphic.Load(filename);
    }

    public bool ModifyGroup(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      return serviceGraphic.ModifyGroup(out requestId, guid, tag, path, boundingRect);
    }

    public bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.ModifyItem(out requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool ModifyItemPath(out Int64 requestId, Guid guid, String path)
    {
      return serviceGraphic.ModifyItemPath(out requestId, guid, path);
    }

    public bool ModifyLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return serviceGraphic.ModifyLink(out requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.ModifyThing(out requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool ModifyThingPath(out Int64 requestId, Guid guid, String path)
    {
      return serviceGraphic.ModifyThingPath(out requestId, guid, path);
    }

    public PortStatus PortCheck(out Int64 requestId, Guid itemGuid, Anchor anchor)
    {
      return serviceGraphic.PortCheck(out requestId, itemGuid, anchor);
    }

    public ArrayList PropertyList(out Int64 requestId, Guid guid, String tag, String path)
    {
      return serviceGraphic.PropertyList(out requestId, guid, tag, path);
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      serviceGraphic.LogMessage(out requestId, message, messageType);
    }

    public bool Save(String filename)
    {
      return serviceGraphic.Save(filename);
    }

    public void ServiceGraphicGroupCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      if (!graphicGroups.ContainsKey(guid))
      {
        GraphicGroup graphicGroup = new GraphicGroup(guid, tag);
        graphicGroup.Path = path;
        graphicGroup.BoundingRect = boundingRect;
        
        graphicGroups.Add(guid, graphicGroup);

        //cnm Scd9Create

        OnGroupCreated(eventId, requestId, guid, tag, path, boundingRect);
      }
    }

    public void ServiceGraphicGroupDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void ServiceGraphicGroupModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void ServiceGraphicItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {

      if (!graphicItems.ContainsKey(guid))
      {
        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.TextArea = textArea;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        //cnm Scd9Create

        OnItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicItems.ContainsKey(guid))
      {
        graphicItems.Remove(guid);

        OnItemDeleted(eventId, requestId, guid);
      }
    }

    public void ServiceGraphicItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      GraphicItem graphicItem;

      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.Tag = tag;
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.TextArea = textArea;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        OnItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicItemPathModified(Int64 eventId, Int64 requestId, Guid guid, String path)
    {
      GraphicItem graphicItem;

      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.Path = path;

        OnItemPathModified(eventId, requestId, guid, path);
      }
    }

    public void ServiceGraphicLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {

      if (!graphicLinks.ContainsKey(guid))
      {
        GraphicLink graphicLink = new GraphicLink(guid, tag, classId, origin, originPort, destination, destinationPort, controlPoints);
        graphicLinks.Add(guid, graphicLink);

        OnLinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void ServiceGraphicLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        OnLinkDeleted(eventId, requestId, guid);
      }
    }

    public void ServiceGraphicLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      GraphicLink graphicLink;

      if (graphicLinks.TryGetValue(guid, out graphicLink))
      {
        graphicLink.Tag = tag;
        graphicLink.ClassID = classId;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        graphicLink.OriginPort = originPort;
        graphicLink.DestinationPort = destinationPort;

        graphicLink.ControlPoints.Clear();

        foreach (PointF controlPoint in controlPoints)
          graphicLink.ControlPoints.Add(controlPoint);

        OnLinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void ServiceGraphicStateChanged(Int64 eventId, Int64 requestId, RunStates runState)
    {
      OnStateChanged(eventId, requestId, runState);
    }

    public void ServiceGraphicStep(Int64 eventId, Int64 step, DateTime time)
    {
      OnStep(eventId, step, time);
    }

    public void ServiceGraphicSync(Int64 eventId)
    {
      OnSync(eventId);
    }

    public void ServiceGraphicThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {

      if (!graphicThings.ContainsKey(guid))
      {
        GraphicThing graphicThing = new GraphicThing(guid, tag);
        graphicThing.Path = path;
        graphicThing.BoundingRect = boundingRect;
        graphicThing.Xaml = xaml;
        graphicThing.Angle = angle;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        graphicThings.Add(guid, graphicThing);

        OnThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        OnThingDeleted(eventId, requestId, guid);
      }
    }

    public void ServiceGraphicThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      GraphicThing graphicThing;

      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Tag = tag;
        graphicThing.Path = path;
        graphicThing.BoundingRect = boundingRect;
        graphicThing.Xaml = xaml;
        graphicThing.Angle = angle;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        OnThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void Syncxxx()
    {
      MemoryStream memoryStream;
      BinaryFormatter bf = new BinaryFormatter();

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicGroups);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicGroups = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicGroup>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicLinks);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicLinks = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicLink>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicItems);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicItems = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicItem>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicThings);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicThings = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicThing>;
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

    ~EngineProtocol()
    {

      if (serviceGraphic != null)
      {

        try
        {

          if (serviceGraphicStateChangedHandler != null) serviceGraphic.StateChanged -= serviceGraphicStateChangedHandler;
        }

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

          if (serviceGraphicGroupCreatedHandler != null) serviceGraphic.GroupCreated -= serviceGraphicGroupCreatedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicGroupModifiedHandler != null) serviceGraphic.GroupModified -= serviceGraphicGroupModifiedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicGroupDeletedHandler != null) serviceGraphic.GroupDeleted -= serviceGraphicGroupDeletedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicItemCreatedHandler != null) serviceGraphic.ItemCreated -= serviceGraphicItemCreatedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicItemModifiedHandler != null) serviceGraphic.ItemModified -= serviceGraphicItemModifiedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicItemPathModifiedHandler != null) serviceGraphic.ItemPathModified -= serviceGraphicItemPathModifiedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicItemDeletedHandler != null) serviceGraphic.ItemDeleted -= serviceGraphicItemDeletedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicLinkCreatedHandler != null) serviceGraphic.LinkCreated -= serviceGraphicLinkCreatedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicLinkModifiedHandler != null) serviceGraphic.LinkModified -= serviceGraphicLinkModifiedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicLinkDeletedHandler != null) serviceGraphic.LinkDeleted -= serviceGraphicLinkDeletedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicThingCreatedHandler != null) serviceGraphic.ThingCreated -= serviceGraphicThingCreatedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicThingModifiedHandler != null) serviceGraphic.ThingModified -= serviceGraphicThingModifiedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicThingDeletedHandler != null) serviceGraphic.ThingDeleted -= serviceGraphicThingDeletedHandler;
        }

        catch (InvalidOperationException) { }
      }
    }
  }
}
