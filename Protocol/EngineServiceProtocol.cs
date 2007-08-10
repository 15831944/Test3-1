
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
  public sealed class EngineServiceProtocol : BaseProtocol
  {

    private ChangeStateHandler changeStateHandler;

    private CreateGroupHandler createGroupHandler;

    private CreateItemHandler createItemHandler;

    private CreateLinkHandler createLinkHandler;

    private CreateThingHandler createThingHandler;
    private DeleteGroupHandler deleteGroupHandler;
    private DeleteItemHandler deleteItemHandler;
    private DeleteLinkHandler deleteLinkHandler;
    private DeleteThingHandler deleteThingHandler;

    private GetPropertyValuesHandler getPropertyValuesHandler;
    private GetSubTagsHandler getSubTagsHandler;

    private LoadHandler loadHandler;
    private ModifyGroupHandler modifyGroupHandler;
    private ModifyItemHandler modifyItemHandler;
    private ModifyItemPathHandler modifyItemPathHandler;
    private ModifyLinkHandler modifyLinkHandler;
    private ModifyThingHandler modifyThingHandler;
    private ModifyThingPathHandler modifyThingPathHandler;

    private PortCheckHandler portCheckHandler;

    private PropertyListHandler propertyListHandler;
    private SaveHandler saveHandler;

    private LogMessageHandler logMessageHandler;

    public delegate bool ChangeStateHandler(out Int64 requestId, RunStates runState);

    public delegate bool CreateGroupHandler(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect);
    public delegate bool CreateItemHandler(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool CreateLinkHandler(out Int64 requestId, out Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool CreateThingHandler(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);

    public delegate bool DeleteGroupHandler(out Int64 requestId, Guid guid);
    public delegate bool DeleteItemHandler(out Int64 requestId, Guid guid);
    public delegate bool DeleteLinkHandler(out Int64 requestId, Guid guid);
    public delegate bool DeleteThingHandler(out Int64 requestId, Guid guid);

    public delegate void GetPropertyValuesHandler(out Int64 requestId, ref ArrayList propertyList);
    public delegate void GetSubTagsHandler(out Int64 requestId, String propertyPath, out ArrayList propertyList);
    public delegate bool LoadHandler(out Int64 requestId);

    public delegate bool ModifyGroupHandler(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect);

    public delegate bool ModifyItemHandler(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool ModifyItemPathHandler(out Int64 requestId, Guid guid, String path);

    public delegate bool ModifyLinkHandler(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);

    public delegate bool ModifyThingHandler(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY);
    public delegate bool ModifyThingPathHandler(out Int64 requestId, Guid guid, String path);

    public delegate PortStatus PortCheckHandler(out Int64 requestId, Guid itemGuid, Anchor anchor);
    public delegate ArrayList PropertyListHandler(out Int64 requestId, Guid guid, String tag, String path);
    public delegate bool SaveHandler(out Int64 requestId);

    public delegate void LogMessageHandler(out Int64 requestId, String message, SysCAD.Log.MessageType messageType);

    public EngineServiceProtocol(String name,
      Dictionary<Guid, GraphicGroup> graphicGroups, Dictionary<Guid, GraphicLink> graphicLinks, Dictionary<Guid, GraphicItem> graphicItems, Dictionary<Guid, GraphicThing> graphicThings,
      LoadHandler loadHandler, SaveHandler saveHandler,
      ChangeStateHandler changeStateHandler, GetPropertyValuesHandler getPropertyValuesHandler, GetSubTagsHandler getSubTagsHandler,
      CreateGroupHandler createGroupHandler, ModifyGroupHandler modifyGroupHandler, DeleteGroupHandler deleteGroupHandler,
      CreateItemHandler createItemHandler, ModifyItemHandler modifyItemHandler, ModifyItemPathHandler modifyItemPathHandler, DeleteItemHandler deleteItemHandler,
      CreateLinkHandler createLinkHandler, ModifyLinkHandler modifyLinkHandler, DeleteLinkHandler deleteLinkHandler,
      CreateThingHandler createThingHandler, ModifyThingHandler modifyThingHandler, ModifyThingPathHandler modifyThingPathHandler, DeleteThingHandler deleteThingHandler,
      PortCheckHandler portCheckHandler, PropertyListHandler propertyListHandler, LogMessageHandler logMessageHandler)
    {
      this.Name = name;

      this.graphicGroups = graphicGroups;

      this.graphicLinks = graphicLinks;
      this.graphicItems = graphicItems;
      this.graphicThings = graphicThings;

      this.loadHandler = loadHandler;
      this.saveHandler = saveHandler;

      this.changeStateHandler = changeStateHandler;

      this.getPropertyValuesHandler = getPropertyValuesHandler;
      this.getSubTagsHandler = getSubTagsHandler;

      this.createGroupHandler = createGroupHandler;
      this.modifyGroupHandler = modifyGroupHandler;
      this.deleteGroupHandler = deleteGroupHandler;

      this.createItemHandler = createItemHandler;
      this.modifyItemHandler = modifyItemHandler;
      this.modifyItemPathHandler = modifyItemPathHandler;
      this.deleteItemHandler = deleteItemHandler;

      this.createLinkHandler = createLinkHandler;
      this.modifyLinkHandler = modifyLinkHandler;
      this.deleteLinkHandler = deleteLinkHandler;

      this.createThingHandler = createThingHandler;
      this.modifyThingHandler = modifyThingHandler;
      this.modifyThingPathHandler = modifyThingPathHandler;
      this.deleteThingHandler = deleteThingHandler;

      this.portCheckHandler = portCheckHandler;

      this.propertyListHandler = propertyListHandler;

      this.logMessageHandler = logMessageHandler;
    }

    public bool ChangeState(out Int64 requestId, RunStates runState)
    {
      return changeStateHandler(out requestId, runState);
    }

    public bool CreateGroup(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect)
    {
      return createGroupHandler(out requestId, out guid, tag, path, boundingRect);
    }

    public bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return createItemHandler(out requestId, out guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool CreateLink(out Int64 requestId, out Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return createLinkHandler(out requestId, out guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      return createThingHandler(out requestId, out guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool DeleteGroup(out Int64 requestId, Guid guid)
    {
      return deleteGroupHandler(out requestId, guid);
    }

    public bool DeleteItem(out Int64 requestId, Guid guid)
    {
      return deleteItemHandler(out requestId, guid);
    }

    public bool DeleteLink(out Int64 requestId, Guid guid)
    {
      return deleteLinkHandler(out requestId, guid);
    }

    public bool DeleteThing(out Int64 requestId, Guid guid)
    {
      return deleteThingHandler(out requestId, guid);
    }

    public void DoGroupCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      OnGroupCreated(eventId, requestId, guid, tag, path, boundingRect);
    }

    public void DoGroupDeleted(Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void DoGroupModified(Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void DoItemBoundingRectModified(Int64 eventId, Int64 requestId, Guid guid, RectangleF boundingRect)
    {
      GraphicItem graphicItem;

      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.BoundingRect = boundingRect;

        OnItemModified(eventId, requestId, guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.TextArea, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }
    }

    public void DoItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      OnItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, mirrorX, mirrorY);
    }

    public void DoItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      OnItemDeleted(eventId, requestId, guid);
    }

    public void DoItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      OnItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, mirrorX, mirrorY);
    }

    public void DoItemPathModified(Int64 eventId, Int64 requestId, Guid guid, String path)
    {
      OnItemPathModified(eventId, requestId, guid, path);
    }

    public void DoLinkControlPointsModified(Int64 eventId, Int64 requestId, Guid guid, List<PointF> controlPoints)
    {
      GraphicLink graphicLink;

      if (graphicLinks.TryGetValue(guid, out graphicLink))
      {
        graphicLink.ControlPoints.Clear();

        foreach (PointF controlPoint in controlPoints)
          graphicLink.ControlPoints.Add(controlPoint);

        OnLinkModified(eventId, requestId, guid, graphicLink.Tag, graphicLink.ClassID, graphicLink.Origin, graphicLink.Destination, graphicLink.OriginPort, graphicLink.DestinationPort, graphicLink.ControlPoints);
      }
    }

    public void DoLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      OnLinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public void DoLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        OnLinkDeleted(eventId, requestId, guid);
      }
    }

    public void DoLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
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

    public void DoStateChanged(Int64 eventId, Int64 requestId, RunStates runState)
    {
      OnStateChanged(eventId, requestId, runState);
    }

    public void DoStep(Int64 eventId, Int64 step, DateTime time)
    {
      OnStep(eventId, step, time);
    }

    public void DoSync(Int64 eventId)
    {
      OnSync(eventId);
    }

    public void DoThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {

      if (!graphicThings.ContainsKey(guid))
      {
        GraphicThing graphicThing = new GraphicThing(guid, tag);
        graphicThing.Path = path;
        graphicThing.BoundingRect = (ARectangleF)boundingRect;
        graphicThing.Xaml = xaml;
        graphicThing.Angle = angle;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        graphicThings.Add(guid, graphicThing);

        OnThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void DoThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        OnThingDeleted(eventId, requestId, guid);
      }
    }

    public void DoThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      GraphicThing graphicThing;

      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Tag = tag;
        graphicThing.Path = path;
        graphicThing.BoundingRect = (ARectangleF)boundingRect;
        graphicThing.Xaml = xaml;
        graphicThing.Angle = angle;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        OnThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void DoThingPathModified(Int64 eventId, Int64 requestId, Guid guid, String path)
    {
      GraphicThing graphicThing;

      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Path = path;

        OnThingModified(eventId, requestId, guid, graphicThing.Tag, graphicThing.Path, graphicThing.BoundingRect, graphicThing.Xaml, graphicThing.Angle, graphicThing.MirrorX, graphicThing.MirrorY);
      }
    }

    public void GetPropertyValues(out Int64 requestId, ref ArrayList tagPathList)
    {
      getPropertyValuesHandler(out requestId, ref tagPathList);
    }

    public void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      getSubTagsHandler(out requestId, propertyPath, out propertyList);
    }

    public bool Load(String filename)
    {
      SoapFormatter sf = new SoapFormatter();
      StreamReader streamRdr = new StreamReader(filename);
      Stream stream = streamRdr.BaseStream;

      this.graphicGroups = graphicGroups;

      this.graphicLinks = graphicLinks;
      this.graphicItems = graphicItems;
      this.graphicThings = graphicThings;

      graphicGroups = (Dictionary<Guid, GraphicGroup>)sf.Deserialize(stream);

      graphicLinks = (Dictionary<Guid, GraphicLink>)sf.Deserialize(stream);
      graphicItems = (Dictionary<Guid, GraphicItem>)sf.Deserialize(stream);
      graphicThings = (Dictionary<Guid, GraphicThing>)sf.Deserialize(stream);
      stream.Close();

      Int64 requestId;
      return loadHandler(out requestId);
    }

    public bool ModifyGroup(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      return modifyGroupHandler(out requestId, guid, tag, path, boundingRect);
    }

    public bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return modifyItemHandler(out requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool ModifyItemPath(out Int64 requestId, Guid guid, String path)
    {
      return modifyItemPathHandler(out requestId, guid, path);
    }

    public bool ModifyLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return modifyLinkHandler(out requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      return modifyThingHandler(out requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool ModifyThingPath(out Int64 requestId, Guid guid, String path)
    {
      return modifyThingPathHandler(out requestId, guid, path);
    }

    public PortStatus PortCheck(out Int64 requestId, Guid itemGuid, Anchor anchor)
    {
      return portCheckHandler(out requestId, itemGuid, anchor);
    }

    public ArrayList PropertyList(out Int64 requestId, Guid guid, String tag, String path)
    {
      //todo: check path is valid.
      return propertyListHandler(out requestId, guid, tag, path);
    }

    public bool Save(String filename)
    {
      SoapFormatter sf = new SoapFormatter();
      StreamWriter streamWriter = new StreamWriter(filename);
      Stream stream = streamWriter.BaseStream;
      sf.Serialize(stream, graphicGroups);
      sf.Serialize(stream, graphicLinks);
      sf.Serialize(stream, graphicItems);
      sf.Serialize(stream, graphicThings);
      stream.Close();

      Int64 requestId;
      return saveHandler(out requestId);
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      logMessageHandler(out requestId, message, messageType);
    }
  }
}
