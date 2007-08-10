
using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;
using SysCAD;
using System.Collections;
using System.Drawing.Drawing2D;

//using System.Windows;
//using System.Windows.Controls;

//using System.Windows.Data;
//using System.Windows.Documents;
//using System.Windows.Media;
//using System.Windows.Media.Imaging;
//using System.Windows.Media.Animation;
//using System.Windows.Shapes;
using System.IO;
using System.Xml;
using System.Text.RegularExpressions;

namespace SysCAD.Editor
{

  internal class State
  {

    private ClientProtocol clientProtocol;
    private Config config;

    private Dictionary<Guid, Group> groups = new Dictionary<Guid, Group>();
    private Dictionary<Guid, Item> items = new Dictionary<Guid, Item>();
    private Dictionary<Guid, Link> links = new Dictionary<Guid, Link>();

    BaseProtocol.RunStates runState;

    private bool selectItems = true;

    private bool selectLinks = true;
    private bool showGraphics = true;
    private bool showLinks = true;

    private bool showModels = true;
    private bool showTags = true;
    Int64 step = Int64.MinValue;

    public List<Guid> newElementSelectionList = new List<Guid>(); // List of elements that have to be selected -- i.e. after paste.

    private Dictionary<Guid, Thing> things = new Dictionary<Guid, Thing>();
    DateTime time = DateTime.MinValue;

    private PureComponents.TreeView.TreeView tvNavigation;

    //internal MemoryStream testXAML()
    //{
    //  //Page page = new Page();
    //  //page.

    //      //
    //  // Create the Geometry to draw.
    //  //
    //  GeometryGroup ellipses = new GeometryGroup();
    //  ellipses.Children.Add(new EllipseGeometry(new System.Windows.Point(50,50), 100, 20));
    //  ellipses.Children.Add(new EllipseGeometry(new System.Windows.Point(50, 50), 20, 100));

    //  //
    //  // Create a GeometryDrawing.
    //  //
    //  GeometryDrawing aGeometryDrawing = new GeometryDrawing();
    //  aGeometryDrawing.Geometry = ellipses;

    //  // Paint the drawing with a gradient.
    //  aGeometryDrawing.Brush = 
    //      new System.Windows.Media.LinearGradientBrush(
    //          Colors.Blue, 
    //          System.Windows.Media.Color.FromRgb(204,204,255), 
    //          new System.Windows.Point(0,0), 
    //          new System.Windows.Point(1,1));

    //  // Outline the drawing with a solid color.
    //  aGeometryDrawing.Pen = new System.Windows.Media.Pen(System.Windows.Media.Brushes.Green, 10);

    //  //
    //  // Use a DrawingImage and an Image control
    //  // to display the drawing.
    //  //
    //  DrawingImage geometryImage = new DrawingImage(aGeometryDrawing);

    //  // Freeze the DrawingImage for performance benefits.
    //  geometryImage.Freeze();

    //  System.Windows.Controls.Image anImage = new System.Windows.Controls.Image();
    //  anImage.SnapsToDevicePixels = true;
    //  anImage.Source = geometryImage;
    //  anImage.HorizontalAlignment = HorizontalAlignment.Left;
    //  anImage.Arrange(new Rect(0, 0, 96, 96));

    //  RenderTargetBitmap renderTargetBitmap = new RenderTargetBitmap(100, 100, 100, 100, PixelFormats.Default);
    //  renderTargetBitmap.Render(anImage);

    //  PngBitmapEncoder pngBitmapEncoder = new PngBitmapEncoder();
    //  pngBitmapEncoder.Frames.Add(BitmapFrame.Create(renderTargetBitmap));
    //  MemoryStream stream = new MemoryStream();
    //  pngBitmapEncoder.Save(stream);
    //  //FileStream fileStream = new FileStream("c:\\test.png", FileMode.Create);
    //  //pngBitmapEncoder.Save(fileStream);
    //  //fileStream.Flush();
    //  //fileStream.Close();

    //  return stream;

    //  //
    //  // Place the image inside a border and
    //  // add it to the page.
    //  //
    //  //Border exampleBorder = new Border();
    //  //exampleBorder.Child = anImage;
    //  //exampleBorder.BorderBrush = System.Windows.Media.Brushes.Gray;
    //  //exampleBorder.BorderThickness = new Thickness(1);
    //  //exampleBorder.HorizontalAlignment = HorizontalAlignment.Left;
    //  //exampleBorder.VerticalAlignment = VerticalAlignment.Top;
    //  //exampleBorder.Margin = new Thickness(10);

    //  //this.Margin = new Thickness(20);
    //  //this.Background = Brushes.White;
    //  //this.Content = exampleBorder;
    //}

    private delegate void CreateGroupDelegate(GraphicGroup graphicGroup, bool isVisible, FlowChart flowchart);

    private delegate void CreateItemDelegate(GraphicItem graphicItem, bool isVisible, FlowChart flowchart);

    private delegate void CreateLinkDelegate(GraphicLink graphicLink, bool isVisible, FlowChart flowchart);

    private delegate void CreateThingDelegate(GraphicThing graphicThing, bool isVisible, FlowChart flowchart);

    private delegate void DeleteItemDelegate(Guid guid, FlowChart flowchart);

    private delegate void DeleteLinkDelegate(Guid guid, FlowChart flowchart);

    private delegate void DeleteThingDelegate(Guid guid, FlowChart flowchart);

    public Arrow Arrow(Guid guid)
    {
      Link link;
      links.TryGetValue(guid, out link);
      return link.Arrow;
    }

    static public AnchorPattern GetAnchorPattern(ModelStencil modelStencil, GraphicItem graphicItem)
    {
      AnchorPointCollection anchorPointCollection = new AnchorPointCollection();

      if ((modelStencil != null) && (modelStencil.Anchors != null))
      {
        graphicItem.anchorIntToTag.Clear();
        graphicItem.anchorTagToInt.Clear();
        int anchorInt = 0;

        foreach (Anchor anchor in modelStencil.Anchors)
        {
          graphicItem.anchorIntToTag.Add(anchorInt, anchor.Tag);
          graphicItem.anchorTagToInt.Add(anchor.Tag, anchorInt);
          anchorInt++;
          float x = anchor.Position.X;

          if (graphicItem.MirrorX) x = 100.0F - x;
          float y = anchor.Position.Y;

          if (graphicItem.MirrorY) y = 100.0F - y;
          AnchorPoint anchorPoint = new AnchorPoint((short)x, (short)y, true, true, MarkStyle.Circle, System.Drawing.Color.Green);
          anchorPoint.Tag = anchor;
          anchorPointCollection.Add(anchorPoint);
        }
      }
      return new AnchorPattern(anchorPointCollection);
    }

    static public ShapeTemplate GetShapeTemplate(GraphicStencil stencil, bool mirrorX, bool mirrorY)
    {
      int i;

      if (stencil != null)
      {
        ElementTemplate[] elementTemplate = new ElementTemplate[stencil.Elements.Count];
        i = 0;

        foreach (Element element in stencil.Elements)
        {
          elementTemplate[i] = MirroredElement(element, mirrorX, mirrorY);
          i++;
        }

        ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.Decorations.Count];
        i = 0;

        foreach (Element decoration in stencil.Decorations)
        {
          decorationTemplate[i] = MirroredElement(decoration, mirrorX, mirrorY);
          i++;
        }

        return (new ShapeTemplate(elementTemplate, decorationTemplate, new ElementTemplate[0], stencil.fillMode, stencil.Tag));
      }

      else
        return null;
    }

    static public ShapeTemplate GetShapeTemplate(ModelStencil stencil, bool mirrorX, bool mirrorY)
    {
      int i;

      if (stencil != null)
      {
        ElementTemplate[] elementTemplate = new ElementTemplate[stencil.Elements.Count];
        i = 0;

        foreach (Element element in stencil.Elements)
        {
          elementTemplate[i] = MirroredElement(element, mirrorX, mirrorY);
          i++;
        }

        ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.Decorations.Count];
        i = 0;

        foreach (Element decoration in stencil.Decorations)
        {
          decorationTemplate[i] = MirroredElement(decoration, mirrorX, mirrorY);
          i++;
        }

        return (new ShapeTemplate(elementTemplate, decorationTemplate, null, stencil.FillMode, stencil.Tag));
      }

      else
        return null;
    }

    //    public static System.Drawing.Image GetImage(GraphicThing graphicThing, FlowChart flowchart)
    //    {
    //      StringReader sr = new StringReader(PreprocessXaml(graphicThing.Xaml));
    //      XmlReader xr = new XmlTextReader(sr);

    //      System.Drawing.Rectangle clientRect = flowchart.DocToClient(graphicThing.BoundingRect);

    //      Viewbox viewbox = new Viewbox();
    //      viewbox.Stretch = Stretch.Fill;
    //      viewbox.Child = System.Windows.Markup.XamlReader.Load(xr) as System.Windows.Controls.Canvas;
    //      viewbox.Measure(new System.Windows.Size(clientRect.Width, clientRect.Height));
    //      viewbox.Arrange(new Rect(0, 0, clientRect.Width, clientRect.Height));
    //      viewbox.UpdateLayout();

    //      RenderTargetBitmap renderTargetBitmap = new RenderTargetBitmap(clientRect.Width, clientRect.Height, 96, 96, PixelFormats.Default);
    //      renderTargetBitmap.Render(viewbox);

    //      MemoryStream stream = new MemoryStream();
    //      {
    //        PngBitmapEncoder pngBitmapEncoder = new PngBitmapEncoder();
    //        pngBitmapEncoder.Frames.Add(BitmapFrame.Create(renderTargetBitmap));
    //        pngBitmapEncoder.Save(stream);
    //      }

    //      {
    //        PngBitmapEncoder pngBitmapEncoder = new PngBitmapEncoder();
    //        pngBitmapEncoder.Frames.Add(BitmapFrame.Create(renderTargetBitmap));
    //        FileStream fileStream = new FileStream("c:\\test.png", FileMode.Create);
    //        pngBitmapEncoder.Save(fileStream);
    //        fileStream.Flush();
    //        fileStream.Close();
    //      }

    //      return System.Drawing.Image.FromStream(stream);
    //    }

    public static String PreprocessXaml(String xaml)
    {
      //String xaml = unProcessedXaml;
      //Regex regex = new Regex(@"\[\[[^\]]*\]\]");
      MatchEvaluator myEvaluator = new MatchEvaluator(ProcessMatch);
      return Regex.Replace(xaml, @"\[\[[^\]]*\]\]", myEvaluator);
      //MatchCollection matches = regex.Matches(xaml);
      //foreach (Match match in matches)
      //{
      //}
      //return xaml;
    }

    public void SetArrow(Guid guid, String tag, Arrow arrow, GraphicLink graphicLink)
    {
      Link link = new Link(guid, tag, graphicLink);
      link.Arrow = arrow;
      links.Add(guid, link);
    }

    static public void SetControlPoints(Arrow arrow, List<PointF> points)
    {
      arrow.SegmentCount = (short)(points.Count - 1);
      int i = 0;
      PointF keepPoint = PointF.Empty;

      foreach (PointF point in points)
      {
        arrow.ControlPoints[i++] = point;
        keepPoint = point;
      }

      while (i <= arrow.SegmentCount)
      {
        arrow.ControlPoints[i++] = keepPoint;
      }
      arrow.UpdateFromPoints();
    }

    internal bool ChangeState(out Int64 requestId, BaseProtocol.RunStates runState)
    {
      return clientProtocol.ChangeState(out requestId, runState);
    }

    internal void ConnectGraphic(
      ClientProtocol.StateChangedHandler stateChangedHandler,
      ClientProtocol.StepHandler stepHandler,
      ClientProtocol.SyncHandler syncHandler,
      ClientProtocol.GroupCreatedHandler GroupCreatedHandler,
      ClientProtocol.GroupModifiedHandler GroupModifiedHandler,
      ClientProtocol.GroupDeletedHandler GroupDeletedHandler,
      ClientProtocol.ItemCreatedHandler itemCreatedHandler,
      ClientProtocol.ItemModifiedHandler itemModifiedHandler,
      ClientProtocol.ItemDeletedHandler itemDeletedHandler,
      ClientProtocol.LinkCreatedHandler linkCreatedHandler,
      ClientProtocol.LinkModifiedHandler linkModifiedHandler,
      ClientProtocol.LinkDeletedHandler linkDeletedHandler,
      ClientProtocol.ThingCreatedHandler thingCreatedHandler,
      ClientProtocol.ThingModifiedHandler thingModifiedHandler,
      ClientProtocol.ThingDeletedHandler thingDeletedHandler)
    {
      clientProtocol.StateChanged += stateChangedHandler;

      clientProtocol.Step += stepHandler;

      clientProtocol.Sync += syncHandler;

      clientProtocol.GroupCreated += GroupCreatedHandler;
      clientProtocol.GroupModified += GroupModifiedHandler;
      clientProtocol.GroupDeleted += GroupDeletedHandler;

      clientProtocol.ItemCreated += itemCreatedHandler;
      clientProtocol.ItemModified += itemModifiedHandler;
      clientProtocol.ItemDeleted += itemDeletedHandler;

      clientProtocol.LinkCreated += linkCreatedHandler;
      clientProtocol.LinkModified += linkModifiedHandler;
      clientProtocol.LinkDeleted += linkDeletedHandler;

      clientProtocol.ThingCreated += thingCreatedHandler;
      clientProtocol.ThingModified += thingModifiedHandler;
      clientProtocol.ThingDeleted += thingDeletedHandler;
    }

    internal bool CreateGraphicItem(out Int64 requestId, out Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return clientProtocol.CreateItem(out requestId, out guid, tag, path, model, shape, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    }

    internal bool CreateGraphicLink(out Int64 requestId, out Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return clientProtocol.CreateLink(out requestId, out guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    internal bool CreateGraphicThing(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      return clientProtocol.CreateThing(out requestId, out guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    internal void CreateGroup(GraphicGroup graphicGroup, bool isVisible, FlowChart flowchart)
    {

      if (flowchart.InvokeRequired)
      {
        flowchart.BeginInvoke(new CreateGroupDelegate(CreateGroup), new object[] { graphicGroup, isVisible, flowchart });
      }

      else
      {
        Box box = null;

        {
          box = flowchart.CreateBox(graphicGroup.X, graphicGroup.Y, graphicGroup.Width, graphicGroup.Height);
          box.ToolTip = graphicGroup.Tag;
          box.Style = BoxStyle.Rectangle;

          box.FillColor = System.Drawing.Color.FromArgb(100, System.Drawing.Color.BurlyWood);
          box.FrameColor = System.Drawing.Color.FromArgb(150, System.Drawing.Color.BurlyWood);
          box.Visible = isVisible;
          
          // Make groups unmodifiable -- for now.
          box.Locked = true;
        }

        Group group = new Group(graphicGroup.Guid, graphicGroup.Tag, box, isVisible, graphicGroup);

        box.Tag = group;

        groups.Add(group.Guid, group);

        PureComponents.TreeView.Node node = tvNavigation.AddNodeByPath(graphicGroup.Path + graphicGroup.Tag, graphicGroup.Guid.ToString());
        node.Tag = group;
        node.AllowDrop = false;

        tvNavigation.AddSelectedNode(node);

        if ((isVisible) && (node.Parent != null))
          node.Parent.Select();

      }
    }

    public bool creatingItem = false; // Allows us to turn off some notifications during the creation process (specifically selectionChagned.)

    internal void CreateItem(GraphicItem graphicItem, bool isVisible, FlowChart flowchart)
    {
      creatingItem = true; // turn off flowchart events while we're mid-create.

      if (flowchart.InvokeRequired)
      {
        flowchart.BeginInvoke(new CreateItemDelegate(CreateItem), new object[] { graphicItem, isVisible, flowchart });
      }

      else
      {
        ModelStencil modelShape = ModelShape(graphicItem.Model);
        GraphicStencil graphicStencil = GraphicShape(graphicItem.Shape, graphicItem.Model);

        Box textBox=null, graphicBox=null, modelBox=null;

        {
          if (graphicStencil != null)
          {
            RectangleF textArea = graphicStencil.TextArea;
            textBox = flowchart.CreateBox(
                                      graphicItem.X + textArea.X / graphicStencil.defaultSize.Width * graphicItem.Width,
                                      graphicItem.Y + textArea.Y / graphicStencil.defaultSize.Height * graphicItem.Height,
                                      textArea.Width / graphicStencil.defaultSize.Width * graphicItem.Width,
                                      textArea.Height / graphicStencil.defaultSize.Height * graphicItem.Height);
          }
          else
          {
            textBox = flowchart.CreateBox(
                                      graphicItem.X,
                                      graphicItem.Y + graphicItem.Height,
                                      graphicItem.Width,
                                      graphicItem.Height / 2.0F);
          }

          textBox.FillColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
          textBox.FrameColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
          textBox.Style = BoxStyle.Shape;
          textBox.Shape = ShapeTemplate.FromId("Rectangle");
          textBox.EnabledHandles = Handles.ResizeTopLeft | Handles.ResizeTopRight |
            Handles.ResizeBottomRight | Handles.ResizeBottomLeft | Handles.ResizeTopCenter |
            Handles.ResizeMiddleRight | Handles.ResizeBottomCenter | Handles.ResizeMiddleLeft |
            Handles.Move;
          textBox.Visible = ShowTags && isVisible;
          textBox.Text = graphicItem.Tag;
        }

        {
          graphicBox = flowchart.CreateBox(graphicItem.X, graphicItem.Y, graphicItem.Width, graphicItem.Height);
          graphicBox.RotationAngle = graphicItem.Angle;
          graphicBox.ToolTip = graphicItem.Tag + "\n\nClassID: " + graphicItem.Model;
          graphicBox.Style = BoxStyle.Shape;

          if (graphicStencil != null)
            graphicBox.Shape = GetShapeTemplate(graphicStencil, graphicItem.MirrorX, graphicItem.MirrorY);
          else
            graphicBox.Shape = ShapeTemplate.FromId("Decision2");

          graphicBox.EnabledHandles = Handles.None;
          graphicBox.HandlesStyle = HandlesStyle.Invisible;
          graphicBox.Visible = ShowGraphics && isVisible;

          if (graphicItem.FillColor.IsEmpty)
            graphicItem.FillColor = graphicBox.FillColor;

          else
            graphicBox.FillColor = graphicItem.FillColor;
        }

        creatingItem = false; // we want any events to happen for the last created piece.

        {
          modelBox = flowchart.CreateBox(graphicItem.X, graphicItem.Y, graphicItem.Width, graphicItem.Height);
          modelBox.RotationAngle = graphicItem.Angle;
          modelBox.ToolTip = graphicItem.Tag + "\n\nClassID: " + graphicItem.Model;
          modelBox.Style = BoxStyle.Shape;

          //modelBox.Image = System.Drawing.Image.FromStream(testXAML());

          if (modelShape != null)
            modelBox.Shape = GetShapeTemplate(modelShape, graphicItem.MirrorX, graphicItem.MirrorY);
          else
            modelBox.Shape = ShapeTemplate.FromId("Decision2");

          modelBox.AnchorPattern = GetAnchorPattern(modelShape, graphicItem);

          modelBox.FillColor = System.Drawing.Color.FromArgb(150, System.Drawing.Color.BurlyWood);
          modelBox.FrameColor = System.Drawing.Color.FromArgb(200, System.Drawing.Color.BurlyWood);
          modelBox.Visible = ShowModels && isVisible;
        }

        if (textBox != null && graphicBox != null && modelBox != null)
        {
          textBox.AttachTo(modelBox, AttachToNode.BottomCenter);

          graphicBox.AttachTo(modelBox, 0, 0, 100, 100);

          Item item = new Item(graphicItem.Guid, graphicItem.Tag, modelBox, graphicBox, textBox, isVisible, graphicItem);

          modelBox.Tag = item;
          graphicBox.Tag = item;
          textBox.Tag = item;

          items.Add(item.Guid, item);

          PureComponents.TreeView.Node node =
            tvNavigation.AddNodeByPath(graphicItem.Path + graphicItem.Tag, graphicItem.Guid.ToString());
          node.Tag = item;
          node.AllowDrop = false;

          NewElementSelection();

          //tvNavigation.AddSelectedNode(node);

          //if ((isVisible) && (node.Parent != null))
          //  node.Parent.Select();
        }
      }
    }

    private void NewElementSelection()
    {
      if (newElementSelectionList.Count > 0)
      {
        foreach (Guid guid in newElementSelectionList)
        {
          if (items.ContainsKey(guid))
            items[guid].Model.Selected = true;
          if (links.ContainsKey(guid))
            links[guid].Arrow.Selected = true;
          if (things.ContainsKey(guid))
            things[guid].Box.Selected = true;
        }
      }
    }
    
    internal void CreateLink(GraphicLink graphicLink, bool isVisible, FlowChart flowchart)
    {

      if (flowchart.InvokeRequired)
      {
        flowchart.BeginInvoke(new CreateLinkDelegate(CreateLink), new object[] { graphicLink, isVisible, flowchart });
      }

      else
      {
        Arrow arrow = flowchart.CreateArrow(new PointF(0.0F, 0.0F), new PointF(10.0F, 10.0F));

        switch (graphicLink.ClassID)
        {

          case "Pipe-1":
            break;

          case "CtrlLink":
            arrow.PenColor = System.Drawing.Color.Gray;
            break;
          default:
            arrow.PenColor = System.Drawing.Color.Red;
            break;
        }

        Item origin = null;
        Item destination = null;

        if (graphicLink.Origin != Guid.Empty) origin = item(graphicLink.Origin);

        if (graphicLink.Destination != Guid.Empty) destination = item(graphicLink.Destination);

        PointF pointOrigin = new PointF();
        PointF pointDestination = new PointF();

        if (graphicLink.ControlPoints != null && graphicLink.ControlPoints.Count > 1)
        {
          pointOrigin = graphicLink.ControlPoints[0];
          pointDestination = graphicLink.ControlPoints[graphicLink.ControlPoints.Count - 1];
        }

        if (origin != null)
          arrow.Origin = origin.Model;

        if (destination != null)
          arrow.Destination = destination.Model;

        if ((graphicLink.OriginPort != null) && ((origin.Model.Tag as Item).GraphicItem.anchorTagToInt.ContainsKey(graphicLink.OriginPort)))
          arrow.OrgnAnchor = (origin.Model.Tag as Item).GraphicItem.anchorTagToInt[graphicLink.OriginPort];

        else
          arrow.OrgnAnchor = -1;

        if ((graphicLink.DestinationPort != null) && ((destination.Model.Tag as Item).GraphicItem.anchorTagToInt.ContainsKey(graphicLink.DestinationPort)))
          arrow.DestAnchor = (destination.Model.Tag as Item).GraphicItem.anchorTagToInt[graphicLink.DestinationPort];

        else
          arrow.DestAnchor = -1;

        String originTag = "";

        if (origin != null) originTag = origin.Tag;

        String destinationTag = "";

        if (destination != null) destinationTag = destination.Tag;

        arrow.ToolTip = "Tag:" + graphicLink.Tag +
          "\nSrc: " + origin.Tag + ":" + graphicLink.OriginPort +
          "\nDst: " + destination.Tag + ":" + graphicLink.DestinationPort;
        arrow.ArrowHead = ArrowHead.Triangle;
        arrow.Style = ArrowStyle.Cascading;

        if (graphicLink.ControlPoints != null && graphicLink.ControlPoints.Count > 1)
        {
          SetControlPoints(arrow, graphicLink.ControlPoints);
        }

        Link link = new Link(graphicLink.Guid, graphicLink.Tag, graphicLink);
        link.Arrow = arrow;
        link.Visible = true;

        arrow.Tag = link;

        arrow.Visible = ShowLinks && isVisible;

        links.Add(link.Guid, link);

        NewElementSelection();
      }
    }

    internal void CreateThing(GraphicThing graphicThing, bool isVisible, FlowChart flowchart)
    {

      if (flowchart.InvokeRequired)
      {
        flowchart.BeginInvoke(new CreateThingDelegate(CreateThing), new object[] { graphicThing, isVisible, flowchart });
      }

      else
      {
        flowchart.SuspendLayout();

        Box box = flowchart.CreateBox(graphicThing.X, graphicThing.Y, graphicThing.Width, graphicThing.Height);
        box.RotationAngle = graphicThing.Angle;
        box.ToolTip = graphicThing.Tag;
        box.Style = BoxStyle.Rectangle;

        box.FillColor = System.Drawing.Color.FromArgb(0, 0, 0, 0);
        box.FrameColor = System.Drawing.Color.FromArgb(0, 0, 0, 0);

        box.Visible = isVisible;

        box.ZBottom();

        Thing thing = new Thing(graphicThing.Guid, graphicThing.Tag, box, isVisible, graphicThing);

        box.Tag = thing;

        //box.Image = State.GetImage(graphicThing, flowchart);

        things.Add(thing.Guid, thing);

        flowchart.ResumeLayout();

        PureComponents.TreeView.Node node =
          tvNavigation.AddNodeByPath(graphicThing.Path + graphicThing.Tag, graphicThing.Guid.ToString());
        node.Tag = thing;
        node.AllowDrop = false;

        node.NodeStyle = new PureComponents.TreeView.NodeStyle();
        node.NodeStyle.SelectedForeColor = System.Drawing.Color.Green;
        node.NodeStyle.ForeColor = System.Drawing.Color.Green;

        NewElementSelection();
      }
    }

    internal bool DeleteGraphicItem(out Int64 requestId, Guid guid)
    {
      return clientProtocol.DeleteItem(out requestId, guid);
    }

    internal bool DeleteGraphicLink(out Int64 requestId, Guid guid)
    {
      return clientProtocol.DeleteLink(out requestId, guid);
    }

    internal bool DeleteGraphicThing(out Int64 requestId, Guid guid)
    {
      return clientProtocol.DeleteThing(out requestId, guid);
    }

    internal void DeleteItem(Guid guid, FlowChart flowchart)
    {

      if (flowchart.InvokeRequired)
      {
        flowchart.BeginInvoke(new DeleteItemDelegate(DeleteItem), new object[] { guid, flowchart });
      }

      else
      {
        tvNavigation.GetNodeByKey(guid.ToString()).Remove();

        //TBD: unlink connected links first
        Item item;

        if (items.TryGetValue(guid, out item))
        {
          flowchart.DeleteObject(item.Model);
          flowchart.DeleteObject(item.Graphic);
          flowchart.DeleteObject(item.Text);
          items.Remove(guid);
        }
      }
    }

    internal void DeleteLink(Guid guid, FlowChart flowchart)
    {

      if (flowchart.InvokeRequired)
      {
        flowchart.BeginInvoke(new DeleteLinkDelegate(DeleteLink), new object[] { guid, flowchart });
      }

      else
      {
        Link link;

        if (links.TryGetValue(guid, out link))
        {
          flowchart.DeleteObject(link.Arrow);
          links.Remove(guid);
        }
      }
    }

    internal void DeleteThing(Guid guid, FlowChart flowchart)
    {

      if (flowchart.InvokeRequired)
      {
        flowchart.BeginInvoke(new DeleteThingDelegate(DeleteThing), new object[] { guid, flowchart });
      }

      else
      {
        tvNavigation.Nodes.Remove(tvNavigation.GetNodeByKey(guid.ToString()));

        Thing thing;

        if (things.TryGetValue(guid, out thing))
        {
          flowchart.DeleteObject(thing.Box);
          things.Remove(guid);
        }
      }
    }

    internal void DisconnectGraphic(
      ClientProtocol.StateChangedHandler stateChangedHandler,
      ClientProtocol.StepHandler stepHandler,
      ClientProtocol.SyncHandler syncHandler,
      ClientProtocol.GroupCreatedHandler GroupCreatedHandler,
      ClientProtocol.GroupModifiedHandler GroupModifiedHandler,
      ClientProtocol.GroupDeletedHandler GroupDeletedHandler,
      ClientProtocol.ItemCreatedHandler itemCreatedHandler,
      ClientProtocol.ItemModifiedHandler itemModifiedHandler,
      ClientProtocol.ItemDeletedHandler itemDeletedHandler,
      ClientProtocol.LinkCreatedHandler linkCreatedHandler,
      ClientProtocol.LinkModifiedHandler linkModifiedHandler,
      ClientProtocol.LinkDeletedHandler linkDeletedHandler,
      ClientProtocol.ThingCreatedHandler thingCreatedHandler,
      ClientProtocol.ThingModifiedHandler thingModifiedHandler,
      ClientProtocol.ThingDeletedHandler thingDeletedHandler)
    {
      clientProtocol.StateChanged -= stateChangedHandler;

      clientProtocol.Step -= stepHandler;

      clientProtocol.Sync -= syncHandler;

      clientProtocol.GroupCreated -= GroupCreatedHandler;
      clientProtocol.GroupModified -= GroupModifiedHandler;
      clientProtocol.GroupDeleted -= GroupDeletedHandler;

      clientProtocol.ItemCreated -= itemCreatedHandler;
      clientProtocol.ItemModified -= itemModifiedHandler;
      clientProtocol.ItemDeleted -= itemDeletedHandler;

      clientProtocol.LinkCreated -= linkCreatedHandler;
      clientProtocol.LinkModified -= linkModifiedHandler;
      clientProtocol.LinkDeleted -= linkDeletedHandler;

      clientProtocol.ThingCreated -= thingCreatedHandler;
      clientProtocol.ThingModified -= thingModifiedHandler;
      clientProtocol.ThingDeleted -= thingDeletedHandler;
    }

    internal bool Exists(Guid guid)
    {
      Item item;
      items.TryGetValue(guid, out item);

      Link link;
      links.TryGetValue(guid, out link);

      Thing thing;
      things.TryGetValue(guid, out thing);

      return ((link != null) || (item != null) || (thing != null));
    }

    internal bool Exists(String tag)
    {

      foreach (Item item in items.Values)
      {

        if (item.Tag == tag)
          return true;
      }

      foreach (Link link in links.Values)
      {

        if (link.Tag == tag)
          return true;
      }

      foreach (Thing thing in things.Values)
      {

        if (thing.Tag == tag)
          return true;
      }

      return false;
    }

    internal static List<PointF> GetControlPoints(MindFusion.FlowChartX.PointCollection pointCollection)
    {
      List<PointF> list = new List<PointF>(pointCollection.Capacity);

      foreach (PointF point in pointCollection)
      {
        list.Add(point);
      }

      return list;
    }

    internal void GetPropertyValues(out Int64 requestId, ref ArrayList tagPathList)
    {
      clientProtocol.GetPropertyValues(out requestId, ref tagPathList);
    }

    internal void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      clientProtocol.GetSubTags(out requestId, propertyPath, out propertyList);
    }

    internal GraphicItem GraphicItem(Box box)
    {
      GraphicItem graphicItem = null;

      if (box.Tag is Item)
        clientProtocol.graphicItems.TryGetValue((box.Tag as Item).Guid, out graphicItem);
      return graphicItem;
    }

    internal GraphicItem GraphicItem(Guid guid)
    {
      GraphicItem graphicItem;
      clientProtocol.graphicItems.TryGetValue(guid, out graphicItem);
      return graphicItem;
    }

    internal GraphicLink GraphicLink(Arrow arrow)
    {
      GraphicLink graphicLink = null;

      if ((arrow != null) && (arrow.Tag != null))
        clientProtocol.graphicLinks.TryGetValue((arrow.Tag as Link).Guid, out graphicLink);
      return graphicLink;
    }

    internal GraphicLink GraphicLink(Guid guid)
    {
      GraphicLink graphicLink;
      clientProtocol.graphicLinks.TryGetValue(guid, out graphicLink);
      return graphicLink;
    }

    internal GraphicStencil GraphicShape(String stencilName)
    {
      GraphicStencil graphicStencil;
      config.GraphicStencils.TryGetValue(stencilName, out graphicStencil);
      return graphicStencil;
    }

    internal GraphicStencil GraphicShape(String stencilName, String modelName)
    {
      GraphicStencil graphicStencil;
      config.GraphicStencils.TryGetValue(stencilName, out graphicStencil);
      if (graphicStencil == null)
        config.GraphicStencils.TryGetValue(modelName, out graphicStencil);
      return graphicStencil;
    }

    internal GraphicThing GraphicThing(Box box)
    {
      GraphicThing graphicThing = null;

      if (box.Tag is Thing)
        clientProtocol.graphicThings.TryGetValue((box.Tag as Thing).Guid, out graphicThing);
      return graphicThing;
    }

    internal GraphicThing GraphicThing(Guid guid)
    {
      GraphicThing graphicThing;
      clientProtocol.graphicThings.TryGetValue(guid, out graphicThing);
      return graphicThing;
    }

    internal bool IsItem(Guid guid)
    {
      return clientProtocol.graphicItems.ContainsKey(guid);
    }

    internal bool IsLink(Guid guid)
    {
      return clientProtocol.graphicLinks.ContainsKey(guid);
    }

    internal bool IsThing(Guid guid)
    {
      return clientProtocol.graphicThings.ContainsKey(guid);
    }

    internal Item Item(Guid guid)
    {
      Item item;
      items.TryGetValue(guid, out item);
      return item;
    }

    internal void ItemSelected(Guid guid, bool selected)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        item.Selected = selected;
        item.Model.Selected = selected && item.Visible;
      }
    }

    internal void SetVisible(Guid guid, bool visible)
    {
      Item item;
      Group group;

      if (groups.TryGetValue(guid, out group))
      {
        group.Visible = visible;
        group.Box.Visible = false; // visible;// && group.Box.Selected;
        group.Box.ZBottom();
      }

      if (items.TryGetValue(guid, out item))
      {
        item.Visible = visible;
        item.Model.Visible = visible && (item.Model.Selected || ShowModels);
        item.Graphic.Visible = visible && ShowGraphics;
        item.Text.Visible = visible && ShowTags;

        foreach (Arrow arrowDestination in item.Model.IncomingArrows)
        {
          arrowDestination.Visible = visible && ShowLinks;
        }

        foreach (Arrow arrowOrigin in item.Model.OutgoingArrows)
        {
          arrowOrigin.Visible = visible && ShowLinks;
        }
      }

      Thing thing;

      if (things.TryGetValue(guid, out thing))
      {
        thing.Visible = visible;
        thing.Box.Visible = visible;
      }
    }

    internal Link Link(Guid guid)
    {
      Link link;
      links.TryGetValue(guid, out link);
      return link;
    }

    static internal float Mirrored(float x, bool mirrored)
    {

      if (mirrored)
        return 100.0F - x;

      else
        return x;
    }

    internal ModelStencil ModelShape(String stencilName)
    {
      ModelStencil modelStencil;
      config.ModelStencils.TryGetValue(stencilName, out modelStencil);
      return modelStencil;
    }

    internal bool ModifyGraphicItem(out Int64 requestId, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return clientProtocol.ModifyItem(out requestId, guid, tag, path, model, shape, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    }

    internal bool ModifyGraphicItemPath(out Int64 requestId, Guid guid, String path)
    {
      return clientProtocol.ModifyItemPath(out requestId, guid, path);
    }

    internal bool ModifyGraphicLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return clientProtocol.ModifyLink(out requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    internal bool ModifyGraphicThing(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      return clientProtocol.ModifyThing(out requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    internal bool ModifyGraphicThingPath(out Int64 requestId, Guid guid, String path)
    {
      return clientProtocol.ModifyThingPath(out requestId, guid, path);
    }

    internal PortStatus PortCheck(out Int64 requestId, Guid itemGuid, Anchor anchor)
    {
      return clientProtocol.PortCheck(out requestId, itemGuid, anchor);
    }

    internal ArrayList PropertyList(out Int64 requestId, Guid guid, String tag, String path)
    {
      return clientProtocol.PropertyList(out requestId, guid, tag, path);
    }

    internal void Remove(FlowChart flowChart)
    {
      BoxCollection boxSelection = flowChart.Selection.Boxes.Clone();

      foreach (Box box in boxSelection)
      {
        Item item = box.Tag as Item;

        if (item != null)
        {
          item.Remove(flowChart);
          items.Remove(item.Guid);
        }
      }

      ArrowCollection arrowSelection = flowChart.Selection.Arrows.Clone();

      foreach (Arrow arrow in arrowSelection)
      {
        Link link = arrow.Tag as Link;

        if (link != null)
        {
          link.Remove(flowChart);
          links.Remove(link.Guid);
        }
      }
    }

    internal void SetAngle(Guid guid, float angle)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        item.Model.RotationAngle = angle;
        item.Graphic.RotationAngle = angle;
      }
    }

    internal void SetFillColor(Guid guid, System.Drawing.Color fillColor)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        item.Graphic.FillColor = fillColor;
      }
    }

    internal void SetHeight(Guid guid, float height)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Height = height;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetMirrorX(Guid guid, bool mirrorX)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        GraphicItem graphicItem;

        if (clientProtocol.graphicItems.TryGetValue(guid, out graphicItem))
        {
          graphicItem.MirrorX = mirrorX;

          GraphicStencil graphicStencil = GraphicShape(graphicItem.Shape, graphicItem.Model);

          if (graphicStencil != null)
          {
            item.Graphic.Shape = GetShapeTemplate(graphicStencil, graphicItem.MirrorX, graphicItem.MirrorY);
          }
        }
      }
    }

    internal void SetMirrorY(Guid guid, bool mirrorY)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        GraphicItem graphicItem;

        if (clientProtocol.graphicItems.TryGetValue(guid, out graphicItem))
        {
          graphicItem.MirrorY = mirrorY;

          GraphicStencil graphicStencil = GraphicShape(graphicItem.Shape, graphicItem.Model);

          if (graphicStencil != null)
          {
            item.Graphic.Shape = GetShapeTemplate(graphicStencil, graphicItem.MirrorX, graphicItem.MirrorY);
          }
        }
      }
    }

    internal void SetTag(Guid guid, String tag)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        item.Tag = tag;
        item.Text.Text = tag;
      }
    }

    internal void SetWidth(Guid guid, float width)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Width = width;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetX(Guid guid, float x)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.X = x;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetY(Guid guid, float y)
    {
      Item item;
      items.TryGetValue(guid, out item);

      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Y = y;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void StateChanged(BaseProtocol.RunStates runState)
    {
      this.runState = runState;
    }

    internal void Step(Int64 step, DateTime time)
    {
      this.step = step;
      this.time = time;
    }

    internal Thing Thing(Guid guid)
    {
      Thing thing;
      things.TryGetValue(guid, out thing);
      return thing;
    }

    private Item item(Guid guid)
    {
      Item item;
      items.TryGetValue(guid, out item);
      return item;
    }

    static private ElementTemplate MirroredElement(object element, bool mirrorX, bool mirrorY)
    {
      SysCAD.Protocol.Line line = element as SysCAD.Protocol.Line;

      if (line != null) return new LineTemplate(Mirrored(line.x1, mirrorX), Mirrored(line.y1, mirrorY),
                                                Mirrored(line.x2, mirrorX), Mirrored(line.y2, mirrorY));

      Arc arc = element as Arc;

      if (arc != null) return new ArcTemplate(Mirrored(arc.x, mirrorX), Mirrored(arc.y, mirrorY),
                                              arc.w, arc.h, arc.a, arc.s);

      Bezier bezier = element as Bezier;

      if (bezier != null) return new BezierTemplate(Mirrored(bezier.x1, mirrorX), Mirrored(bezier.y1, mirrorY),
                                                    Mirrored(bezier.x2, mirrorX), Mirrored(bezier.y2, mirrorY),
                                                    Mirrored(bezier.x3, mirrorX), Mirrored(bezier.y3, mirrorY),
                                                    Mirrored(bezier.x4, mirrorX), Mirrored(bezier.y4, mirrorY));

      return null;
    }

    private static String ProcessMatch(Match match)
    {
      char[] splitter = { '[', ',', ']' };

      String resultStr = String.Empty;
      String matchStr = match.ToString();
      String[] strings = matchStr.Split(splitter, StringSplitOptions.RemoveEmptyEntries);

      // Just randomly throw in some information until the connection to SysCAD9 is finished.\
      switch (strings[6].Trim())
      {

        case "Hex":
          {
            int minResultValue;
            int maxResultValue;

            if (strings[3].Trim()[0] == '#')
              minResultValue = Int32.Parse(strings[3].Trim().TrimStart('#'), System.Globalization.NumberStyles.HexNumber);

            else
              minResultValue = Int32.Parse(strings[3].Trim());

            if (strings[4].Trim()[0] == '#')
              maxResultValue = Int32.Parse(strings[4].Trim().TrimStart('#'), System.Globalization.NumberStyles.HexNumber);

            else
              maxResultValue = Int32.Parse(strings[4].Trim());

            Random rand = new Random();
            resultStr = rand.Next(minResultValue, maxResultValue + 1).ToString("X6");
          }
          break;

        case "Integer":
          {
            int minResultValue;
            int maxResultValue;

            if (strings[3].Trim()[0] == '#')
              minResultValue = Int32.Parse(strings[3].Trim().TrimStart('#'), System.Globalization.NumberStyles.HexNumber);

            else
              minResultValue = Int32.Parse(strings[3].Trim());

            if (strings[4].Trim()[0] == '#')
              maxResultValue = Int32.Parse(strings[4].Trim().TrimStart('#'), System.Globalization.NumberStyles.HexNumber);

            else
              maxResultValue = Int32.Parse(strings[4].Trim());

            Random rand = new Random();
            resultStr = rand.Next(minResultValue, maxResultValue + 1).ToString();
          }
          break;

        case "Float":
          {

            Double minResultValue;

            Double maxResultValue;

            minResultValue = Double.Parse(strings[3].Trim());
            maxResultValue = Double.Parse(strings[4].Trim());

            Random rand = new Random();
            resultStr = (rand.NextDouble() * (maxResultValue - minResultValue) + minResultValue).ToString();
          }
          break;

        default:
          resultStr = "!!Error!!";
          break;
      }

      return resultStr;
    }

    public ClientProtocol ClientProtocol
    {
      get { return clientProtocol; }
      set { clientProtocol = value; }
    }

    public Config Config
    {
      get { return config; }
      set { config = value; }
    }

    public IEnumerable<GraphicStencil> GraphicStencils
    {
      get { return config.GraphicStencils.Values; }
    }

    public IEnumerable<ModelStencil> ModelStencils
    {
      get { return config.ModelStencils.Values; }
    }

    public bool SelectItems
    {
      get { return selectItems; }
      set { selectItems = value; }
    }

    public bool SelectLinks
    {
      get { return selectLinks; }
      set { selectLinks = value; }
    }

    public bool ShowGraphics
    {
      get { return showGraphics; }
      set { showGraphics = value; }
    }

    public bool ShowLinks
    {
      get { return showLinks; }
      set { showLinks = value; }
    }

    public bool ShowModels
    {
      get { return showModels; }
      set { showModels = value; }
    }

    public bool ShowTags
    {
      get { return showTags; }
      set { showTags = value; }
    }

    public PureComponents.TreeView.TreeView TVNavigation
    {
      get { return tvNavigation; }
      set { tvNavigation = value; }
    }

    internal String CurrentPath
    {
      get
      {

        if (tvNavigation.SelectedNode != null)
          return tvNavigation.SelectedNode.FullPath + tvNavigation.PathSeparator;

        else
          return tvNavigation.PathSeparator;
      }
    }

    internal IEnumerable<GraphicItem> GraphicItems
    {
      get { return clientProtocol.graphicItems.Values; }
    }

    internal IEnumerable<GraphicLink> GraphicLinks
    {
      get { return clientProtocol.graphicLinks.Values; }
    }

    internal IEnumerable<GraphicThing> GraphicThings
    {
      get { return clientProtocol.graphicThings.Values; }
    }

    internal IEnumerable<Item> Items
    {
      get { return items.Values; }
    }

    internal IEnumerable<Link> Links
    {
      get { return links.Values; }
    }

    internal IEnumerable<Thing> Things
    {
      get { return things.Values; }
    }
  }
}
