using System;
using System.Collections.Generic;
using System.Text;
using SysCAD.Protocol;
using System.Collections;
using SysCAD.Log;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
using System.Drawing;
using System.Runtime.Remoting;
using MindFusion.FlowChartX;
using MindFusion.FlowChartX.LayoutSystem;
using System.Globalization;
using System.Collections.ObjectModel;

namespace SysCAD.Service
  {
  [Serializable]
  public class Project : ILog
    {
    private Int64 requestId;
    private Int64 eventId;

    string name;
    string path;

    Graphic graphic;
    Model model;

    [NonSerializedAttribute]
    FlowChart flowChart;

    [NonSerializedAttribute]
    ConfigData configData;

    [NonSerializedAttribute]
    SysCAD.Log.LogView logView;

    private ClientServiceProtocol clientClientServiceProtocol;
    private EngineServiceProtocol engineClientServiceProtocol;

    private Dictionary<String, Box> clientBoxes = new Dictionary<String, Box>();
    private Dictionary<String, Box> engineBoxes = new Dictionary<String, Box>();

    private Dictionary<String, Arrow> clientArrows = new Dictionary<String, Arrow>();
    private Dictionary<String, Arrow> engineArrows = new Dictionary<String, Arrow>();

    [NonSerializedAttribute]
    private Box projectBox;

    public Project(string name, string path)
      {
      this.name = name;
      this.path = path;

      graphic = new Graphic();
      model = new Model();
      }

    public string Name
      {
      get { return name; }
      set { name = value; }
      }

    public string Path
      {
      get { return path; }
      set { path = value; }
      }

    public Graphic Graphic
      {
      get { return graphic; }
      set { graphic = value; }
      }

    public Model Model
      {
      get { return model; }
      set { model = value; }
      }

    public FlowChart FlowChart
      {
      get { return flowChart; }
      set
        {
        flowChart = value;
        if (flowChart != null)
          {
          flowChart.DocExtents = new RectangleF(0.0F, 0.0F, 1000.0F, 1000.0F);

          projectBox = flowChart.CreateBox(0.0F, 0.0F, 40.0F, 20.0F);
          projectBox.Selected = false;
          projectBox.Text = name;
          projectBox.Shape = ShapeTemplate.Decision;
          }

        RedrawAll();
        }
      }

    public ConfigData ConfigData
      {
      get { return configData; }
      set { configData = value; }
      }

    public SysCAD.Log.LogView LogView
      {
      get { return logView; }
      set { logView = value; }
      }

    public bool LoadGraphics(out Int64 requestId)
      {
      this.requestId++;
      requestId = this.requestId;

      try
        {
          {
          BinaryFormatter bf = new BinaryFormatter();
          StreamReader streamRdr = new StreamReader(path + "\\Graphic.10");
          Stream stream = streamRdr.BaseStream;
          graphic = (Graphic)bf.Deserialize(stream);
          stream.Close();
          }

            {
            BinaryFormatter bf = new BinaryFormatter();
            StreamReader streamRdr = new StreamReader(path + "\\Model.10");
            Stream stream = streamRdr.BaseStream;
            model = (Model)bf.Deserialize(stream);
            stream.Close();
            }

            if (graphic == null) graphic = new Graphic();
            if (model == null) model = new Model();

            return true;
        }
      catch (Exception)
        {
        if (graphic == null)
          {
          graphic = new Graphic();
          LogMessage(out requestId, "LoadGraphics: Graphic.10 file not found or corrupt for project " + name + ".  Creating empty one.", MessageType.Error);
          }

        if (model == null)
          {
          model = new Model();
          LogMessage(out requestId, "LoadGraphics: Model.10 file not found or corrupt for project " + name + ".  Creating empty one.", MessageType.Error);
          }

        return false;
        }
      }

    public bool SaveGraphics(out Int64 requestId)
      {
      this.requestId++;
      requestId = this.requestId;

      try
        {
          {
          BinaryFormatter bf = new BinaryFormatter();
          StreamWriter streamWriter = new StreamWriter(path + "Graphic.10");
          Stream stream = streamWriter.BaseStream;
          bf.Serialize(stream, graphic);
          stream.Close();
          }

            {
            BinaryFormatter bf = new BinaryFormatter();
            StreamWriter streamWriter = new StreamWriter(path + "Model.10");
            Stream stream = streamWriter.BaseStream;
            bf.Serialize(stream, model);
            stream.Close();
            }

            return true;
        }
      catch (Exception)
        {
        return false;
        }
      }

    public bool Change(out Int64 requestId, SysCAD.Protocol.Action action)
      {
      this.requestId++;
      requestId = this.requestId;

      Collection<ModelItem> modelCreate = new Collection<ModelItem>();
      Collection<ModelItem> modelModify = new Collection<ModelItem>();
      Collection<Guid> modelDelete = new Collection<Guid>();

      Collection<GraphicItem> graphicCreate = new Collection<GraphicItem>();
      Collection<GraphicItem> graphicModify = new Collection<GraphicItem>();
      Collection<Guid> graphicDelete = new Collection<Guid>();

        { // Check if OK to go ahead.
        if (action.Create != null)
          {
          foreach (Item item in action.Create)
            {
            if (Exists(item))
              {
              LogMessage(out requestId, "Create: Item " + item.Guid + ", " + item.Tag + " already exists.", MessageType.Error);
              return false; // Can't create an item if guid already exists.
              }
            else
              {
              if (item is ModelItem)
                modelCreate.Add(item as ModelItem);
              else if (item is GraphicItem)
                {
                if (item is GraphicLink)
                  {
                  GraphicLink graphicLink = item as GraphicLink;
                  if (graphicLink.ControlPoints.Count > 2) // Check for duplicate points.
                    {
                    List<SysCAD.Protocol.Point> newControlPoints = new List<SysCAD.Protocol.Point>();
                    SysCAD.Protocol.Point pointA = graphicLink.ControlPoints[0];
                    SysCAD.Protocol.Point pointB = graphicLink.ControlPoints[1];
                    SysCAD.Protocol.Point pointC = graphicLink.ControlPoints[2];

                    int i = 3;
                    while (i < graphicLink.ControlPoints.Count)
                      {
                      if (((pointA.X == pointB.X) && (pointB.X == pointC.X) && (pointC.X == pointA.X))
                        || ((pointA.Y == pointB.Y) && (pointB.Y == pointC.Y) && (pointC.Y == pointA.Y)))
                        {
                        pointB = pointC;
                        pointC = graphicLink.ControlPoints[i];
                        i++;
                        }
                      if (i < graphicLink.ControlPoints.Count)
                      {
                      newControlPoints.Add(pointA);
                      pointA = pointB;
                      pointB = pointC;
                      pointC = graphicLink.ControlPoints[i];
                      i++;
                      }
                      }
                    newControlPoints.Add(pointA);
                    newControlPoints.Add(pointB);
                    newControlPoints.Add(pointC);
                    //if (newControlPoints.Count % 2 == 0)
                    //  newControlPoints.Add(pointC);
                    graphicLink.ControlPoints = newControlPoints;
                    }
                  }
                
                graphicCreate.Add(item as GraphicItem);
                }
              }
            }
          }

        if (action.Modify != null)
          {
          foreach (Item item in action.Modify)
            {
            if (!Exists(item))
              {
              LogMessage(out requestId, "Modify: Item " + item.Guid + ", " + item.Tag + " doesn't exist.", MessageType.Error);
              return false; // Can't modify an item if guid doesn't exist.
              }
            else
              {
              if (item is ModelItem)
                modelModify.Add(item as ModelItem);
              else if (item is GraphicItem)
                graphicModify.Add(item as GraphicItem);
              }
            }
          }

        if (action.Delete != null)
          {
          foreach (Guid guid in action.Delete)
            {
            if (!Exists(guid))
              {
              LogMessage(out requestId, "Delete: Item " + guid + " doesn't exist.", MessageType.Error);
              return false; // Can't modify an item if guid doesn't exist.
              }
            else
              {
              if (ModelExists(guid))
                {
                modelDelete.Add(guid);
                }
              else if (GraphicExists(guid))
                {
                graphicDelete.Add(guid);
                }
              }
            }
          }
        }

        //!!!if (engineClientServiceProtocol.Change(eventId, requestId, modelCreate, modelModify, modelDelete))
        //!!!{

        Actioned actioned = new Actioned();

        foreach (ModelItem modelItem in modelCreate)
          {
          model.Create(modelItem);
          actioned.Created.Add(modelItem.Guid);
          }

        foreach (ModelItem modelItem in modelModify)
          {
          model.Modify(modelItem);
          actioned.Modified.Add(modelItem.Guid);
          }

        foreach (Guid guid in modelDelete)
          {
          model.Delete(guid);
          actioned.Deleted.Add(guid);
          }

        foreach (GraphicItem graphicItem in graphicCreate)
          {
          graphic.Create(graphicItem);
          actioned.Created.Add(graphicItem.Guid);
          }

        foreach (GraphicItem graphicItem in graphicModify)
          {
          graphic.Modify(graphicItem);
          actioned.Modified.Add(graphicItem.Guid);
          }

        foreach (Guid guid in graphicDelete)
          {
          graphic.Delete(guid);
          actioned.Deleted.Add(guid);
          }

          { // Do Change here.
          eventId++;
          clientClientServiceProtocol.DoChanged(eventId, requestId, actioned);
          }

          return true;
      //!!!}
      //!!!else
      //!!!{
      //!!!  return false;
      //!!!}
      }
    private bool Exists(Guid guid)
      {
      return ((graphic.Nodes.ContainsKey(guid))
        || (graphic.Links.ContainsKey(guid))
        || (model.Nodes.ContainsKey(guid))
        || (model.Links.ContainsKey(guid)));
      }

    private bool ModelExists(Guid guid)
      {
      return ((model.Nodes.ContainsKey(guid))
        || (model.Links.ContainsKey(guid)));
      }

    private bool GraphicExists(Guid guid)
      {
      return ((graphic.Nodes.ContainsKey(guid))
        || (graphic.Links.ContainsKey(guid)));
      }

    private bool Exists(Item item)
      {
      return Exists(item.Guid);
      }

    //bool CreateGroup(out Int64 requestId, out Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect)
    //{
    //  // Need to check for runstate here, and decide if we'll fire DoGroupCreated.
    //  // This is required in case a rogue client tries to create an Group even when not supposed to.
    //  // This applies to all three create*, and all three delete* events.
    //  if (true)
    //  { // We're going to do it.
    //    // Create the Group.
    //    this.requestId++;
    //    requestId = this.requestId;
    //    guid = Guid.NewGuid();

    //    GraphicGroup graphicGroup = new GraphicGroup(guid, tag);
    //    graphicGroup.Path = path;
    //    graphicGroup.BoundingRect = boundingRect;

    //    graphic.Groups.Add(guid, graphicGroup);

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoGroupCreated(eventId, requestId, guid, tag, path, boundingRect);

    //    return true;
    //  }
    //  //else
    //  //{ // We're not going to do it.
    //  //  return false;
    //  //}
    //}

    //public delegate void DoItemCreatedDelegate(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Guid modelGuid, NodeClass model, Shape stencil, SysCAD.Protocol.Rectangle boundingRect, Double angle, SysCAD.Protocol.Rectangle tagArea, Double tagAngle, Boolean tagVisible, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY);

    //bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Guid modelGuid, NodeClass model, Shape stencil, SysCAD.Protocol.Rectangle boundingRect, Double angle, SysCAD.Protocol.Rectangle tagArea, Double tagAngle, Boolean tagVisible, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    //{
    //  // Need to check for runstate here, and decide if we'll fire DoItemCreated.
    //  // This is required in case a rogue client tries to create an item even when not supposed to.
    //  // This applies to all three create*, and all three delete* events.
    //  if (true)
    //  { // We're going to do it.
    //    // Create the item.
    //    this.requestId++;
    //    requestId = this.requestId;
    //    guid = Guid.NewGuid();

    //    GraphicNode graphicNode = new GraphicNode(guid, tag, path, modelGuid, stencil, boundingRect, angle, tagArea, tagAngle, tagVisible, fillColor, fillMode, mirrorX, mirrorY);

    //    graphic.Nodes.Add(guid, graphicNode);

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, tagArea, tagAngle, fillColor, fillMode, mirrorX, mirrorY);

    //    return true;
    //  }
    //  //else
    //  //{ // We're not going to do it.
    //  //  return false;
    //  //}
    //}

    //bool CreateLink(out Int64 requestId, out Guid guid, Guid modelGuid, String tag, LinkClass linkClass, Guid origin, Guid modelOrigin,
    //  Guid destination, Guid modelDestination, String originPort, Int16 originPortID,
    //  String destinationPort, Int16 destinationPortID, List<SysCAD.Protocol.Point> controlPoints,
    //  SysCAD.Protocol.Rectangle tagArea, Double tagAngle, Boolean tagVisible)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  GraphicNode originGraphicNode;
    //  GraphicNode destinationGraphicNode;

    //  graphic.Nodes.TryGetValue(origin, out originGraphicNode);
    //  graphic.Nodes.TryGetValue(destination, out destinationGraphicNode);

    //  if ((originGraphicNode != null) && (destinationGraphicNode != null)) // Decide whether to create a link.
    //  {
    //    ModelNode originModelNode;
    //    ModelNode destinationModelNode;

    //    model.Nodes.TryGetValue(originGraphicNode.ModelGuid, out originModelNode);
    //    model.Nodes.TryGetValue(destinationGraphicNode.ModelGuid, out destinationModelNode);

    //    if ((originModelNode != null) && (destinationModelNode != null)) // Decide whether to create a link.
    //    { // We're going to do it.
    //      // Create the item.


    //      // Calculate the closest anchorpointid for unknown (-1) id's.
    //      // The following is close, but I need to convert the link controlpoint to a relative position to the item
    //      // before comparing to the anchor location...

    //      //// Instead, for now:
    //      //if (originPortID == -1) // unknown anchor id...
    //      //  originPortID = 0;

    //      //if (destinationPortID == -1) // unknown anchor id...
    //      //  destinationPortID = 0;

    //      if (originPortID == -1) // unknown anchor id...
    //      {
    //        Double minDistance = Double.MaxValue;
    //        Int16 minID = 0;
    //        ModelStencil modelStencil;
    //        configData.ModelStencils.TryGetValue(originModelNode.NodeClass, out modelStencil);
    //        if (modelStencil != null)
    //        {
    //          foreach (Anchor anchor in modelStencil.Anchors)
    //          {
    //            if (anchor.Tag == originPort)
    //            {
    //              Double distance = 0.0;
    //              Int16 ID = 0;
    //              foreach (SysCAD.Protocol.Point position in anchor.Positions)
    //              {
    //                SysCAD.Protocol.Point anchorPoint =
    //                  new SysCAD.Protocol.Point(originGraphicNode.BoundingRect.X + originGraphicNode.BoundingRect.Width * position.X / 100.0,
    //                                            originGraphicNode.BoundingRect.Y + originGraphicNode.BoundingRect.Height * position.Y / 100.0);
    //                SysCAD.Protocol.Point originPoint = controlPoints[0];
    //                distance = Math.Sqrt((originPoint.X - anchorPoint.X) * (originPoint.X - anchorPoint.X) + (originPoint.Y - anchorPoint.Y) * (originPoint.Y - anchorPoint.Y));
    //                if (distance < minDistance)
    //                {
    //                  minDistance = distance;
    //                  minID = ID;
    //                }
    //                ID++;
    //              }
    //            }
    //          }
    //        }
    //        originPortID = minID;
    //      }


    //      if (destinationPortID == -1) // unknown anchor id...
    //      {
    //        Double minDistance = Double.MaxValue;
    //        Int16 minID = 0;
    //        ModelStencil modelStencil;
    //        configData.ModelStencils.TryGetValue(destinationModelNode.NodeClass, out modelStencil);
    //        if (modelStencil != null)
    //        {
    //          foreach (Anchor anchor in modelStencil.Anchors)
    //          {
    //            if (anchor.Tag == destinationPort)
    //            {
    //              Double distance = 0.0;
    //              Int16 ID = 0;
    //              foreach (SysCAD.Protocol.Point position in anchor.Positions)
    //              {
    //                SysCAD.Protocol.Point anchorPoint =
    //                  new SysCAD.Protocol.Point(destinationGraphicNode.BoundingRect.X + destinationGraphicNode.BoundingRect.Width * position.X / 100.0,
    //                                            destinationGraphicNode.BoundingRect.Y + destinationGraphicNode.BoundingRect.Height * position.Y / 100.0);
    //                SysCAD.Protocol.Point destinationPoint = controlPoints[controlPoints.Count - 1];
    //                distance = Math.Sqrt((destinationPoint.X - anchorPoint.X) * (destinationPoint.X - anchorPoint.X) + (destinationPoint.Y - anchorPoint.Y) * (destinationPoint.Y - anchorPoint.Y));
    //                if (distance < minDistance)
    //                {
    //                  minDistance = distance;
    //                  minID = ID;
    //                }
    //                ID++;
    //              }
    //            }
    //          }
    //        }
    //        destinationPortID = minID;
    //      }


    //      guid = Guid.NewGuid();

    //      GraphicLink graphicLink = new GraphicLink(guid, modelGuid, tag, origin, originPortID, destination, destinationPortID, controlPoints, tagArea, tagAngle, tagVisible);
    //      graphic.Links.Add(guid, graphicLink);

    //      // Raise event(s).
    //      eventId++;
    //      //clientClientServiceProtocol.DoLinkCreated(eventId, requestId, guid, tag, classID, origin, destination, originPort, originPortID, destinationPort, destinationPortID, controlPoints, tagArea, tagAngle);

    //      return true;
    //    }
    //  }

    //  //else
    //  { // We're not going to do it.
    //    if (originGraphicNode == null)
    //      logView.Message("Failed to create link " + tag + ".  Origin: " + origin.ToString() + " missing.", MessageType.Warning);

    //    if (destinationGraphicNode == null)
    //      logView.Message("Failed to create link " + tag + ".  Destination: " + destination.ToString() + " missing.", MessageType.Warning);

    //    guid = Guid.Empty;

    //    return false;
    //  }
    //}

    //bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    //{
    //  if (true) // Decide whether to create an Thing.
    //  { // We're going to do it.
    //    // Create the Thing.

    //    this.requestId++;
    //    requestId = this.requestId;
    //    guid = Guid.NewGuid();

    //    GraphicThing graphicThing = new GraphicThing(guid, tag);
    //    graphicThing.Path = path;
    //    graphicThing.BoundingRect = boundingRect;
    //    graphicThing.Xaml = xaml;
    //    graphicThing.Angle = angle;
    //    graphicThing.MirrorX = mirrorX;
    //    graphicThing.MirrorY = mirrorY;

    //    graphic.Things.Add(guid, graphicThing);

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

    //    return true;
    //  }
    //  //else
    //  //{ // We're not going to do it.
    //  //  return false;
    //  //}
    //}

    //bool DeleteGroup(out Int64 requestId, Guid guid)
    //{
    //  throw new NotImplementedException("The method or operation is not implemented.");
    //}

    //bool DeleteItem(out Int64 requestId, Guid guid)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Nodes.ContainsKey(guid))
    //  { // We're going to do it.

    //    // Delete the item.
    //    graphic.Nodes.Remove(guid);

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoItemDeleted(eventId, requestId, guid);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    //bool DeleteLink(out Int64 requestId, Guid guid)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Links.ContainsKey(guid))
    //  { // We're going to do it.
    //    // Delete the item.

    //    graphic.Links.Remove(guid);

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoLinkDeleted(eventId, requestId, guid);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    //bool DeleteThing(out Int64 requestId, Guid guid)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Things.ContainsKey(guid))
    //  { // We're going to do it.
    //    // Delete the Thing.

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoThingDeleted(eventId, requestId, guid);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    //bool ModifyGroup(out Int64 requestId, Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect)
    //{
    //  throw new NotImplementedException("The method or operation is not implemented.");
    //}

    public delegate void DoPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag);

    public bool ClientRequestPortInfo(out Int64 requestId, Guid guid, String tag)
      {

      this.requestId++;
      requestId = this.requestId;

      if (graphic.Nodes.ContainsKey(guid))
        {
        // Raise event(s).
        eventId++;
        DoPortInfoRequested doPortInfoRequested = new DoPortInfoRequested(engineClientServiceProtocol.DoPortInfoRequested);
        doPortInfoRequested.BeginInvoke(eventId, requestId, guid, tag, null, null);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    public bool EngineRequestPortInfo(out Int64 requestId, Guid guid, String tag, PortInfo portInfo)
      {
      this.requestId++;
      requestId = this.requestId;

      if (graphic.Nodes.ContainsKey(guid))
        {
        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoPortInfoRequested(eventId, requestId, guid, tag, portInfo);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    //bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, NodeClass model, Shape stencil, SysCAD.Protocol.Rectangle boundingRect, Double angle, SysCAD.Protocol.Rectangle tagArea, Double tagAngle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Nodes.ContainsKey(guid))
    //  { // We're going to do it.

    //    // Need to get hold of a valid pDoc pointer... *********

    //    //Individual changes would go something like this: *********
    //    //int length = tag.Length;
    //    //wchar_t * tagwc = (wchar_t*)(void*)Marshal.StringToHGlobalUni(tag);

    //    //char * tagc = (char *)malloc(length+1);
    //    //tagc[length] = 0;
    //    //for (int i=0; i<length; i++)
    //    //  tagc[i] = (char)tagwc[i];

    //    //pDoc.GCB.DoModify(tagc, 
    //    //                    angle,
    //    //                    boundingRect, 
    //    //                    fillColor,
    //    //                    fillMode,
    //    //                    mirrorX,
    //    //                    mirrorY);

    //    // Modify the item.
    //    GraphicNode graphicNode = graphic.Nodes[guid];

    //    graphicNode.Tag = tag;
    //    graphicNode.Path = path;
    //    graphicNode.Shape = stencil;
    //    graphicNode.BoundingRect = boundingRect;
    //    graphicNode.Angle = angle;
    //    graphicNode.TagArea = tagArea;
    //    graphicNode.TagAngle = tagAngle;
    //    graphicNode.FillColor = fillColor;
    //    graphicNode.FillMode = fillMode;
    //    graphicNode.MirrorX = mirrorX;
    //    graphicNode.MirrorY = mirrorY;

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, tagArea, tagAngle, fillColor, fillMode, mirrorX, mirrorY);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    //bool ModifyItemPath(out Int64 requestId, Guid guid, String path)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Nodes.ContainsKey(guid))
    //  { // We're going to do it.
    //    // Modify the item.

    //    GraphicNode graphicItem = graphic.Nodes[guid];

    //    graphicItem.Path = path;

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoItemModified(eventId, requestId, guid, graphicItem.Tag, path, graphicItem.NodeClass, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.TagArea, graphicItem.TagAngle, graphicItem.FillColor, graphicItem.FillMode, graphicItem.MirrorX, graphicItem.MirrorY);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    //bool ModifyItemBoundingRect(out Int64 requestId, Guid guid, SysCAD.Protocol.Rectangle boundingRect)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Nodes.ContainsKey(guid))
    //  { // We're going to do it.
    //    // Modify the item.

    //    GraphicNode graphicNode = graphic.Nodes[guid];

    //    graphicNode.BoundingRect = boundingRect;

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoItemModified(eventId, requestId, guid, graphicNode.Tag, graphicNode.Path, graphicNode.NodeClass, graphicNode.Shape, boundingRect, graphicNode.Angle, graphicNode.TagArea, graphicNode.TagAngle, graphicNode.FillColor, graphicNode.FillMode, graphicNode.MirrorX, graphicNode.MirrorY);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    //bool ModifyLink(out Int64 requestId, Guid guid, Guid modelGuid, String tag, String classID, Guid origin, Guid modelOrigin, Guid destination, Guid modelDestination,
    //  String originPort, Int16 originPortID, String destinationPort, Int16 destinationPortID, List<SysCAD.Protocol.Point> controlPoints, SysCAD.Protocol.Rectangle tagArea, Double tagAngle)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Links.ContainsKey(guid))
    //  { // We're going to do it.
    //    // Modify the item.

    //    GraphicLink graphicLink = graphic.Links[guid];

    //    graphicLink.Tag = tag;
    //    graphicLink.ModelGuid = modelGuid;
    //    graphicLink.Origin = origin;
    //    graphicLink.Destination = destination;
    //    graphicLink.OriginPortID = originPortID;
    //    graphicLink.DestinationPortID = destinationPortID;

    //    graphicLink.ControlPoints.Clear();

    //    foreach (SysCAD.Protocol.Point controlPoint in controlPoints)
    //      graphicLink.ControlPoints.Add(controlPoint);

    //    graphicLink.TagArea = tagArea;
    //    graphicLink.TagAngle = tagAngle;

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoLinkModified(eventId, requestId, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints, tagArea, tagAngle);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    //bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Things.ContainsKey(guid))
    //  { // We're going to do it.
    //    // Modify the Thing.

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    //bool ModifyThingPath(out Int64 requestId, Guid guid, String path)
    //{
    //  this.requestId++;
    //  requestId = this.requestId;

    //  if (graphic.Things.ContainsKey(guid))
    //  { // We're going to do it.
    //    // Modify the item.

    //    GraphicThing graphicThing = graphic.Things[guid];

    //    graphicThing.Path = path;

    //    // Raise event(s).
    //    eventId++;
    //    //clientClientServiceProtocol.DoThingModified(eventId, requestId, guid, graphicThing.Tag, path, graphicThing.BoundingRect, graphicThing.Xaml, graphicThing.Angle, graphicThing.MirrorX, graphicThing.MirrorY);

    //    return true;
    //  }
    //  else
    //  { // We're not going to do it.
    //    return false;
    //  }
    //}

    public ArrayList PropertyListCheck(out Int64 requestId, Guid guid, String tag, String path)
      {
      this.requestId++;
      requestId = this.requestId;

      //char* dest = new char[tag.Length+1];
      //strcpy(dest, static_cast<LPCTSTR>(const_cast<void*>(static_cast<const void*>(System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(tag)))));
      //ScdMainWnd().PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS, (LPARAM)dest);

      ArrayList list = new ArrayList();
      // Generate list of properties at this level in properties tree.
      return list;
      }

    public bool ChangePermissions(out Int64 requestId, ClientBaseProtocol.Permissions permissions)
      {
      if (true) // Decide whether to allow runstate change
        { // We're going to do it.
        // Change the runstate.

        this.requestId++;
        requestId = this.requestId;

        LogMessage(out requestId, "ChangePermissions not implemented yet.", MessageType.Error);

        //// Raise event(s).
        //eventId++;
        //clientClientServiceProtocol.DoPermissionsChanged(eventId, requestId, permissions);

        return true;
        }
      //else
      //{ // We're not going to do it.
      //  return false;
      //}
      }

    public void GetPropertyValues(out Int64 requestId, ref ArrayList propertyList)
      {
      this.requestId++;
      requestId = this.requestId;
      throw new NotImplementedException("The method or operation is not implemented.");
      // Return modified ArrayList with tag details included.
      }

    public void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
      {
      this.requestId++;
      requestId = this.requestId;

      propertyList = new ArrayList();
      Random random = new Random();

      // Return ArrayList with tag details included.
      for (int i = 0; i < 10; i++)
        {
        StringBuilder builder = new StringBuilder();
        System.Char ch;
        int n = random.Next(5, 10);
        for (int j = 0; j < n; j++)
          {
          ch = Convert.ToChar(random.Next(33, 126));
          builder.Append(ch);
          }
        switch (random.Next(0, 3))
          {
          case 0:
            propertyList.Add(new ModelProperty(builder.ToString(), random.Next()));
            break;
          case 1:
            propertyList.Add(new ModelProperty(builder.ToString(), random.NextDouble()));
            break;
          case 2:
            propertyList.Add(new ModelProperty(builder.ToString(), builder.ToString()));
            break;
          case 3:
            propertyList.Add(new ModelProperty(builder.ToString(), Color.FromArgb(random.Next())));
            break;
          }
        }
      }

    public void LogMessage(out Int64 requestId, String message, MessageType messageType)
      {
      this.requestId++;
      requestId = this.requestId;

      if (logView != null)
        logView.Message(message, messageType);
      }

    //private void LogNote(string message)
    //{
    //  Message(message, MessageType.Note);
    //  Console.WriteLine(message);
    //}

    public void Message(string msg, MessageType msgType, MessageSource src)
      {
      logView.Message(msg, msgType, src);
      }

    public void Message(string msg, MessageType msgType)
      {
      logView.Message(msg, msgType, new MessageSource(name));
      }

    public void SetSource(MessageSource src)
      {
      logView.SetSource(src);
      }

    public void RemoveSource()
      {
      logView.RemoveSource();
      }

    public bool Active
      {
      get { return logView.Enabled; }
      set { logView.Enabled = value; }
      }

    public bool StateChanged(out Int64 requestId, EngineBaseProtocol.RunState runState)
      {
      this.requestId++;
      requestId = this.requestId;

      ClientBaseProtocol.Permissions permissions = null;
      switch (runState)
        {
        case EngineBaseProtocol.RunState.Edit:
          permissions = new ClientBaseProtocol.Permissions(true, true, true);
          break;
        case EngineBaseProtocol.RunState.Idle:
          permissions = new ClientBaseProtocol.Permissions(false, true, false);
          break;
        case EngineBaseProtocol.RunState.Run:
          permissions = new ClientBaseProtocol.Permissions(false, true, false);
          break;
        }

      eventId++;
      clientClientServiceProtocol.PermissionsChanged(eventId, requestId, permissions);
      return true;
      }


    internal void ConnectClientServiceProtocol()
      {
      ClientServiceProtocol.LoadHandler clientLoad = new ClientServiceProtocol.LoadHandler(LoadGraphics);
      ClientServiceProtocol.SaveHandler clientSave = new ClientServiceProtocol.SaveHandler(SaveGraphics);

      ClientServiceProtocol.ChangePermissionsHandler clientChangePermissions = new ClientServiceProtocol.ChangePermissionsHandler(ChangePermissions);

      ClientServiceProtocol.GetPropertyValuesHandler clientGetPropertyValues = new ClientServiceProtocol.GetPropertyValuesHandler(GetPropertyValues);
      ClientServiceProtocol.GetSubTagsHandler clientGetSubTags = new ClientServiceProtocol.GetSubTagsHandler(GetSubTags);

      ClientServiceProtocol.ChangeHandler clientChange = new ClientServiceProtocol.ChangeHandler(Change);

      ClientServiceProtocol.RequestPortInfoHandler clientRequestPortInfo = new ClientServiceProtocol.RequestPortInfoHandler(ClientRequestPortInfo);

      ClientServiceProtocol.PropertyListHandler clientPropertyListCheck = new ClientServiceProtocol.PropertyListHandler(PropertyListCheck);

      ClientServiceProtocol.LogMessageHandler clientLogMessage = new ClientServiceProtocol.LogMessageHandler(LogMessage);

      ClientServiceProtocol.AnnounceHandler clientAnnounce = new ClientServiceProtocol.AnnounceHandler(ClientAnnounce);
      ClientServiceProtocol.RenounceHandler clientRenounce = new ClientServiceProtocol.RenounceHandler(ClientRenounce);

      clientClientServiceProtocol = new ClientServiceProtocol(name,
                                                              path,
                                                              clientLoad,
                                                              clientSave,
                                                              graphic,
                                                              model,
                                                              clientChangePermissions,
                                                              clientGetPropertyValues,
                                                              clientGetSubTags,
                                                              clientChange,
                                                              clientRequestPortInfo,
                                                              clientPropertyListCheck,
                                                              clientLogMessage,
                                                              clientAnnounce,
                                                              clientRenounce);

      RemotingServices.Marshal(clientClientServiceProtocol, "Client/" + Name);


      EngineServiceProtocol.LogMessageHandler engineLogMessage = new EngineServiceProtocol.LogMessageHandler(LogMessage);
      EngineServiceProtocol.StateChangedHandler engineStateChanged = new EngineServiceProtocol.StateChangedHandler(StateChanged);
      EngineServiceProtocol.RequestPortInfoHandler engineRequestPortInfo = new EngineServiceProtocol.RequestPortInfoHandler(EngineRequestPortInfo);

      EngineServiceProtocol.AnnounceHandler engineAnnounce = new EngineServiceProtocol.AnnounceHandler(EngineAnnounce);
      EngineServiceProtocol.RenounceHandler engineRenounce = new EngineServiceProtocol.RenounceHandler(EngineRenounce);

      engineClientServiceProtocol = new EngineServiceProtocol(name,
                                                              path,
                                                              graphic,
                                                              model,
                                                              engineLogMessage,
                                                              engineStateChanged,
                                                              engineRequestPortInfo,
                                                              engineAnnounce,
                                                              engineRenounce);

      RemotingServices.Marshal(engineClientServiceProtocol, "Engine/" + Name);
      }

    private delegate String ClientAnnounceDelegate(String clientName);

    public String ClientAnnounce(String clientName)
      {
      if (flowChart.InvokeRequired)
        {
        return flowChart.Invoke(new ClientAnnounceDelegate(ClientAnnounce), new object[] { clientName }).ToString();
        }
      else
        {
        Box testBox;
        int i = 0;
        String fullName = clientName + i.ToString(CultureInfo.InvariantCulture);
        while (clientBoxes.TryGetValue(fullName, out testBox))
          {
          i++;
          fullName = clientName + i.ToString(CultureInfo.InvariantCulture);
          }

        Box box = flowChart.CreateBox(0.0F, 0.0F, 40.0F, 20.0F);
        box.Selected = false;
        box.Shape = ShapeTemplate.Decision;
        box.Text = fullName;
        clientBoxes.Add(fullName, box);

        Arrow arrow = flowChart.CreateArrow(projectBox, box);
        arrow.Selected = false;
        clientArrows.Add(fullName, arrow);

        RedrawAll();

        return fullName;
        }
      }

    private delegate void ClientRenounceDelegate(String clientName);

    public void ClientRenounce(String clientName)
      {
      if (flowChart.InvokeRequired)
        {
        flowChart.BeginInvoke(new ClientRenounceDelegate(ClientRenounce), new object[] { clientName });
        }
      else
        {
        Box testBox;
        if (clientBoxes.TryGetValue(clientName, out testBox))
          {
          Box clientBox = clientBoxes[clientName];
          Arrow clientArrow = clientArrows[clientName];

          flowChart.DeleteObject(clientArrow);
          flowChart.DeleteObject(clientBox);

          clientBoxes.Remove(clientName);
          clientArrows.Remove(clientName);
          }

        try
          {
          RedrawAll();
          }
        catch (Exception)
          { // this fails sometimes on close due to the garbage collection timing.
          }
        }
      }

    private delegate string EngineAnnounceDelegate(String engineName);

    public string EngineAnnounce(String engineName)
      {
      if (flowChart.InvokeRequired)
        {
        return flowChart.Invoke(new EngineAnnounceDelegate(EngineAnnounce), new object[] { engineName }).ToString();
        }
      else
        {
        Box testBox;
        int i = 0;
        String fullName = engineName + i.ToString(CultureInfo.InvariantCulture);
        while (engineBoxes.TryGetValue(fullName, out testBox))
          {
          i++;
          fullName = engineName + i.ToString(CultureInfo.InvariantCulture);
          }

        Box box = flowChart.CreateBox(0.0F, 0.0F, 40.0F, 20.0F);
        box.Selected = false;
        box.Shape = ShapeTemplate.Decision;
        box.Text = fullName;
        engineBoxes.Add(fullName, box);

        Arrow arrow = flowChart.CreateArrow(projectBox, box);
        arrow.Selected = false;
        engineArrows.Add(fullName, arrow);

        RedrawAll();

        return fullName;
        }
      }

    private delegate void EngineRenounceDelegate(String engineName);

    public void EngineRenounce(String engineName)
      {
      if (flowChart.InvokeRequired)
        {
        flowChart.BeginInvoke(new EngineRenounceDelegate(EngineRenounce), new object[] { engineName });
        }
      else
        {
        Box testBox;
        if (engineBoxes.TryGetValue(engineName, out testBox))
          {
          Box engineBox = engineBoxes[engineName];
          Arrow engineArrow = engineArrows[engineName];

          flowChart.DeleteObject(engineArrow);
          flowChart.DeleteObject(engineBox);

          engineBoxes.Remove(engineName);
          engineArrows.Remove(engineName);
          }

        RedrawAll();
        }
      }

    private void RedrawAll()
      {
      TreeLayout tl = new TreeLayout();
      tl.Type = TreeLayoutType.Centered;
      tl.Direction = TreeLayoutDirection.TopToBottom;
      tl.Anchoring = Anchoring.Keep;
      tl.Arrange(flowChart);

      flowChart.ZoomToFit();
      }

    }
  }
