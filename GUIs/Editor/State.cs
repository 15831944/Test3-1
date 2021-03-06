using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Drawing;
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
using System.Text;
using System.Text.RegularExpressions;
using System.Xml;
using MindFusion.FlowChartX;
using SysCAD;
using SysCAD.Protocol;

namespace SysCAD.Editor
{
  internal class State
  {
    private FlowChart flowChart;
    private ClientProtocol clientProtocol;
    private Config config;

    private delegate void CreateAreaDelegate(GraphicArea graphicArea);
    private delegate void CreateNodeDelegate(ModelNode modelNode, GraphicNode graphicNode);
    private delegate void CreateLinkDelegate(ModelLink modelLink, GraphicLink graphicLink);

    private delegate void ModifyAreaDelegate(GraphicArea graphicArea);
    private delegate void ModifyNodeDelegate(Guid guid);
    private delegate void ModifyLinkDelegate(Guid guid);

    private delegate void DeleteNodeDelegate(Guid guid);
    private delegate void DeleteLinkDelegate(Guid guid);

    private Dictionary<Guid, EditorArea> editorAreas = new Dictionary<Guid, EditorArea>();
    private Dictionary<Guid, EditorNode> editorNodes = new Dictionary<Guid, EditorNode>();
    private Dictionary<Guid, EditorLink> editorLinks = new Dictionary<Guid, EditorLink>();

    private Dictionary<String, Bitmap> modelUnselectedThumbnails = new Dictionary<String, Bitmap>();
    private Dictionary<String, Bitmap> modelSelectedThumbnails = new Dictionary<String, Bitmap>();

    Int64 requestId;

    public Dictionary<String, Bitmap> ModelUnselectedThumbnails
    {
      get { return modelUnselectedThumbnails; }
      set { modelUnselectedThumbnails = value; }
    }

    public Dictionary<String, Bitmap> ModelSelectedThumbnails
    {
      get { return modelSelectedThumbnails; }
      set { modelSelectedThumbnails = value; }
    }

    private Dictionary<String, Bitmap> graphicUnselectedThumbnails = new Dictionary<String, Bitmap>();
    private Dictionary<String, Bitmap> graphicSelectedThumbnails = new Dictionary<String, Bitmap>();

    public Dictionary<String, Bitmap> GraphicUnselectedThumbnails
    {
      get { return graphicUnselectedThumbnails; }
      set { graphicUnselectedThumbnails = value; }
    }

    public Dictionary<String, Bitmap> GraphicSelectedThumbnails
    {
      get { return graphicSelectedThumbnails; }
      set { graphicSelectedThumbnails = value; }
    }

    bool projectOpen;

    ClientBaseProtocol.Permissions permissions = new ClientBaseProtocol.Permissions(false, false, false);

    private bool selectItems = true;

    private bool selectLinks = true;
    private bool showGraphics = true;
    private bool showLinks = true;

    private bool showModels = false;

    private bool showAreas = true;

    private bool showTags = true;
    Int64 step = Int64.MinValue;

    public List<Guid> newElementSelectionList = new List<Guid>(); // List of elements that have to be selected -- i.e. after paste.

    DateTime time = DateTime.MinValue;

    private PureComponents.TreeView.TreeView tvNavigation;

    public Arrow Arrow(Guid guid)
    {
      EditorLink link;
      editorLinks.TryGetValue(guid, out link);
      return link.Arrow;
    }

    static public AnchorPattern GetAnchorPattern(ModelStencil modelStencil, EditorNode item)
    {
      AnchorPointCollection anchorPointCollection = new AnchorPointCollection();

      if ((modelStencil != null) && (modelStencil.Anchors != null))
      {
        item.anchorIntToTag.Clear();
        item.anchorTagToInt.Clear();
        int anchorInt = 0;

        foreach (Anchor anchor in modelStencil.Anchors)
        {
          int anchorPtInt = 0;
          foreach (SysCAD.Protocol.Point point in anchor.Positions)
          {
            Double x = point.X;
            if (item.MirrorX)
              x = 100.0 - x;
            Double y = point.Y;
            if (item.MirrorY)
              y = 100.0 - y;

            MarkStyle markStyle = MarkStyle.Circle; // Use circle in any other case.
            switch (anchor.Look)
            {
              case 0: markStyle = MarkStyle.Circle; break;
              case 1: markStyle = MarkStyle.Cross; break;
              case 2: markStyle = MarkStyle.Rectangle; break;
              case 3: markStyle = MarkStyle.X; break;
            }
            AnchorPoint anchorPoint = new AnchorPoint((short)x, (short)y, true, true, markStyle, System.Drawing.Color.Green);
            anchorPoint.Tag = anchor;
            anchorPointCollection.Add(anchorPoint);

            item.anchorIntToTag.Add(anchorInt, anchor.Tag + anchorPtInt.ToString());
            item.anchorTagToInt.Add(anchor.Tag + anchorPtInt.ToString(), anchorInt);

            anchorInt++;
            anchorPtInt++;
          }
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

        return (new ShapeTemplate(elementTemplate, decorationTemplate, new ElementTemplate[0], FillMode.Alternate, (String)stencil.Tags[0]));
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

        return (new ShapeTemplate(elementTemplate, decorationTemplate, null, FillMode.Alternate, stencil.Tag));
      }

      else
        return null;
    }

    public void SetArrow(State state, Arrow arrow, GraphicLink graphicLink, ModelLink modelLink)
    {
      EditorLink link = new EditorLink(state, graphicLink, modelLink);
      link.Arrow = arrow;
      editorLinks.Add(graphicLink.Guid, link);
    }

    static public void SetControlPoints(Arrow arrow, List<SysCAD.Protocol.Point> points, double dx, double dy)
    {
      List<SysCAD.Protocol.Point> offsetPoints = new List<SysCAD.Protocol.Point>();

      foreach (SysCAD.Protocol.Point point in points)
      {
        offsetPoints.Add(new SysCAD.Protocol.Point(point.X + dx, point.Y + dy));
      }

      SetControlPoints(arrow, offsetPoints);
    }

    static public void SetControlPoints(Arrow arrow, List<SysCAD.Protocol.Point> points)
    {
      if (points.Count % 2 == 0)
        arrow.SegmentCount = (short)(points.Count); // extra point required.
      else
        arrow.SegmentCount = (short)(points.Count - 1);

      int i = 0;
      SysCAD.Protocol.Point keepPoint = SysCAD.Protocol.Point.Empty;

      foreach (SysCAD.Protocol.Point point in points)
      {
        arrow.ControlPoints[i++] = point.ToPointF();
        keepPoint = point;
      }

      while (i <= arrow.SegmentCount)
      {
        arrow.ControlPoints[i++] = keepPoint.ToPointF();
      }
      arrow.UpdateFromPoints();
    }

    //internal bool CreateGraphicItem(out Int64 requestId, out Guid guid, String tag, String path, String model, String shape, SysCAD.Protocol.Rectangle boundingRect, Double angle, SysCAD.Protocol.Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    //{
    //  return clientProtocol.CreateItem(out requestId, out guid, tag, path, model, shape, boundingRect, angle, textArea, textAngle, fillColor, fillMode, mirrorX, mirrorY);
    //}

    //internal bool CreateGraphicLink(out Int64 requestId, out Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, Int16 originPortID, String destinationPort, Int16 destinationPortID, List<SysCAD.Protocol.Point> controlPoints, SysCAD.Protocol.Rectangle textArea, Double textAngle)
    //{
    //  return clientProtocol.CreateLink(out requestId, out guid, tag, classId, origin, destination, originPort, originPortID, destinationPort, destinationPortID, controlPoints, textArea, textAngle);
    //}

    internal void CreateArea(GraphicArea graphicArea)
    {
      if (flowChart.InvokeRequired)
      {
        flowChart.BeginInvoke(new CreateAreaDelegate(CreateArea), new object[] { graphicArea });
      }
      else
      {
        Box box = null;
        PureComponents.TreeView.Node node = null;

        node = tvNavigation.AddNodeByPath(graphicArea.Path + graphicArea.Tag, graphicArea.Guid.ToString());
        node.AllowDrop = false;
        node.Tooltip = "";
        tvNavigation.AddSelectedNode(node);

        bool isVisible = true;
        if (node.Parent != null) // if we're not root, make visibility same as parent.
          isVisible = node.Parent.IsSelected;

        box = flowChart.CreateBox((float)graphicArea.X, (float)graphicArea.Y, (float)graphicArea.Width, (float)graphicArea.Height);
        box.ToolTip = "";// graphicArea.Tag;
        box.Style = BoxStyle.Rectangle;

        // Make areas unmodifiable -- for now.
        //box.Locked = true;

        EditorArea area = new EditorArea(this, graphicArea);
        area.Box = box;
        editorAreas.Add(area.Guid, area);

        box.Tag = area;
        node.Tag = area;

        area.Locked = true;
        area.Visible = isVisible;
      }
    }

    public bool creatingNode = false; // Allows us to turn off some notifications during the creation process (specifically selectionChanged.)

    internal void CreateNode(GraphicNode graphicNode)
    {
      CreateNode(null, graphicNode);
    }

    internal void CreateNode(ModelNode modelNode, GraphicNode graphicNode)
    {
      if (flowChart.InvokeRequired)
      {
        flowChart.BeginInvoke(new CreateNodeDelegate(CreateNode), new object[] { modelNode, graphicNode });
      }
      else
      {
        creatingNode = true; // turn off flowchart events while we're mid-create.

        EditorNode editorNode = new EditorNode(this, graphicNode, modelNode);

        PureComponents.TreeView.Node treeViewNode = null;
        {
          treeViewNode = tvNavigation.AddNodeByPath(graphicNode.Path + graphicNode.Tag, graphicNode.Guid.ToString());
          treeViewNode.AllowDrop = false;

          treeViewNode.Tag = editorNode;

          NewElementSelection();
        }

        EditorArea editorArea = null;
        bool isVisible = false;
        if (treeViewNode.Parent != null) // if we're not root, make visibility same as parent.
        {
          if (treeViewNode.Parent.Tag is EditorArea)
            editorArea = treeViewNode.Parent.Tag as EditorArea;

          isVisible = treeViewNode.Parent.IsSelected;
        }

        ModelStencil modelStencil;
        GraphicStencil graphicStencil = GraphicStencil(graphicNode.Stencil);


        Box textBox = null, graphicBox = null, modelBox = null, hiddenBox = null;

        {
          SysCAD.Protocol.Rectangle textArea = graphicNode.TagArea;

          if (textArea.IsEmpty) // We haven't got a TagArea stored in the item yet.
          {
            if (graphicStencil != null)
            {
              textArea = new SysCAD.Protocol.Rectangle(graphicNode.X + graphicStencil.TagArea.X / graphicStencil.DefaultSize.Width * graphicNode.Width,
                                        graphicNode.Y + graphicStencil.TagArea.Y / graphicStencil.DefaultSize.Height * graphicNode.Height,
                                        graphicStencil.TagArea.Width / graphicStencil.DefaultSize.Width * graphicNode.Width,
                                        graphicStencil.TagArea.Height / graphicStencil.DefaultSize.Height * graphicNode.Height);
            }
            else
            {
              textArea = new SysCAD.Protocol.Rectangle(graphicNode.X,
                                        graphicNode.Y + 1.1F * graphicNode.Height,
                                        graphicNode.Width,
                                        graphicNode.Height / 2.0F);
            }
          }

          textBox = flowChart.CreateBox((float)textArea.X, (float)textArea.Y, (float)textArea.Width, (float)textArea.Height);
          textBox.RotationAngle = (float)graphicNode.TagAngle;
          textBox.FillColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
          textBox.FrameColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
          textBox.RotateContents = true;
          textBox.Style = BoxStyle.Shape;
          textBox.Shape = ShapeTemplate.FromId("Rectangle");
          textBox.EnabledHandles = Handles.ResizeTopLeft | Handles.ResizeTopRight |
            Handles.ResizeBottomRight | Handles.ResizeBottomLeft | Handles.ResizeTopCenter |
            Handles.ResizeMiddleRight | Handles.ResizeBottomCenter | Handles.ResizeMiddleLeft |
            Handles.Move;
          textBox.Text = graphicNode.Tag;
          textBox.Tag = editorNode;
          editorNode.TextBox = textBox;
          // Don't set this yet, happens after every zoom because it needs to be scaled.
          //textBox.Font = graphicNode.TagFont.BaseFont;
        }

        if (graphicNode.Stencil != null)
        {
          graphicBox = flowChart.CreateBox((float)graphicNode.X, (float)graphicNode.Y, (float)graphicNode.Width, (float)graphicNode.Height);
          graphicBox.RotationAngle = (float)graphicNode.Angle;
          graphicBox.ToolTip = graphicNode.Tag + "\n\nClassID: " + "graphicNode.Model";
          graphicBox.Style = BoxStyle.Shape;

          graphicBox.Tag = editorNode;
          editorNode.GraphicBox = graphicBox;

          if (GraphicStencil(graphicNode.Stencil) == null)
          // can't use graphicStencil because the above GraphicShape call will find a stencil even if the shape doesn't exist.
          {
            clientProtocol.LogMessage(out requestId, "GraphicStencil not found in library for stencil \'" + graphicNode.Stencil + "\'", SysCAD.Log.MessageType.Warning);
          }

          if (graphicStencil != null)
          {
            graphicBox.Shape = GetShapeTemplate(graphicStencil, graphicNode.MirrorX, graphicNode.MirrorY);
            // An attempt to deal with un-clickable non-model graphics...  failed, will have to try something else.
            // perhaps catch the click and look underneath expecially for graphicboxes???
            //if (graphicBox.Shape.Outline.Length == 0)
            //{
            //  ElementTemplate[] background = new ElementTemplate[4];
            //  background[0] = new LineTemplate(  0.0F,   0.0F,   0.0F, 100.0F);
            //  background[1] = new LineTemplate(  0.0F, 100.0F, 100.0F, 100.0F);
            //  background[2] = new LineTemplate(100.0F, 100.0F, 100.0F,   0.0F);
            //  background[3] = new LineTemplate(100.0F,   0.0F,   0.0F,   0.0F);

            //  graphicBox.Shape = new ShapeTemplate(background, graphicBox.Shape.Decorations, graphicBox.Shape.TagArea, FillMode.Alternate);
            //  graphicBox.FillColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
            //  graphicBox.FrameColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
            //}
          }
          else
            graphicBox.Shape = ShapeTemplate.FromId("Decision2");

          if (graphicNode.FillColor.IsEmpty)
            graphicNode.FillColor = graphicBox.FillColor;

          else
            graphicBox.FillColor = graphicNode.FillColor;
        }

        creatingNode = false; // we want any events to happen for the last created piece.

        if (modelNode != null)
        {
          modelBox = flowChart.CreateBox((float)graphicNode.X, (float)graphicNode.Y, (float)graphicNode.Width, (float)graphicNode.Height);
          modelBox.RotationAngle = (float)graphicNode.Angle;
          modelBox.ToolTip = graphicNode.Tag + "\n\nClassID: " + "graphicNode.Model";
          modelBox.Style = BoxStyle.Shape;
          modelBox.CustomDraw = CustomDraw.Additional;

          modelBox.Tag = editorNode;
          editorNode.ModelBox = modelBox;

          modelStencil = ModelStencil(modelNode.NodeClass);

          if (modelStencil == null)
          {
            clientProtocol.LogMessage(out requestId, "ModelStencil not found in library for shape \'" + modelNode.NodeClass + "\'", SysCAD.Log.MessageType.Error);
          }


          if (modelStencil != null)
          {
            modelBox.Shape = GetShapeTemplate(modelStencil, graphicNode.MirrorX, graphicNode.MirrorY);

            //RequestPortInfo test code.
            //{
            //  Int64 requestId2;
            //  RequestGraphicPortInfo(out requestId2, graphicNode.Guid, ((Anchor)(modelStencil.Anchors[0])).Tag);
            //}
          }
          else
            modelBox.Shape = ShapeTemplate.FromId("Decision2");

          modelBox.AnchorPattern = GetAnchorPattern(modelStencil, editorNode);

          modelBox.FillColor = System.Drawing.Color.FromArgb(220, 222, 184, 136);
          modelBox.FrameColor = System.Drawing.Color.FromArgb(255, 111, 92, 68);

          graphicBox.EnabledHandles = Handles.None;
          graphicBox.HandlesStyle = HandlesStyle.Invisible;

          if (modelBox.Shape.Outline.Length == 0) // we're going to need an invisible box to use for hit-testing.
          {
            hiddenBox = flowChart.CreateBox((float)graphicNode.X, (float)graphicNode.Y, (float)graphicNode.Width, (float)graphicNode.Height);
            hiddenBox.RotationAngle = (float)graphicNode.Angle;
            hiddenBox.ToolTip = modelNode.Tag + "\n\nClassID: " + "modelNode.Model";
            hiddenBox.Style = BoxStyle.Shape; //BoxStyle.Ellipse; // not sure whether to go with ellipse or rectangle here (without corners, it'll get in the way less...)
            hiddenBox.Shape = GetShapeTemplate(ModelStencil("Tank-1"), graphicNode.MirrorX, graphicNode.MirrorY);
            hiddenBox.FillColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
            hiddenBox.FrameColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
            hiddenBox.ZBottom();

            hiddenBox.Tag = editorNode;
            editorNode.HiddenBox = hiddenBox;

            graphicBox.AttachTo(hiddenBox, 0, 0, 100, 100);
          }
        }
        else
        {
          if ((graphicBox != null) && (graphicBox.Shape.Outline.Length == 0)) // we're going to need an invisible box to use for hit-testing.
          {
            hiddenBox = flowChart.CreateBox((float)graphicNode.X, (float)graphicNode.Y, (float)graphicNode.Width, (float)graphicNode.Height);
            hiddenBox.RotationAngle = (float)graphicNode.Angle;
            hiddenBox.ToolTip = graphicNode.Tag + "\n\nClassID: " + "graphicNode.Model";
            hiddenBox.Style = BoxStyle.Shape; //BoxStyle.Ellipse; // not sure whether to go with ellipse or rectangle here (without corners, it'll get in the way less...)
            hiddenBox.Shape = GetShapeTemplate(ModelStencil("Tank-1"), graphicNode.MirrorX, graphicNode.MirrorY);
            hiddenBox.FillColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
            hiddenBox.FrameColor = System.Drawing.Color.FromArgb(0, System.Drawing.Color.Black);
            hiddenBox.ZBottom();

            hiddenBox.Tag = editorNode;
            editorNode.HiddenBox = hiddenBox;

            graphicBox.AttachTo(hiddenBox, 0, 0, 100, 100);
          }
        }

        if (modelBox != null)
        {
          textBox.AttachTo(modelBox, AttachToNode.BottomCenter);
          graphicBox.AttachTo(modelBox, 0, 0, 100, 100);
          if (editorArea != null)
            modelBox.AttachTo(editorArea.Box, AttachToNode.TopLeft);
        }
        else if (graphicBox != null)
        {
          textBox.AttachTo(graphicBox, AttachToNode.BottomCenter);
          if (hiddenBox != null)
            hiddenBox.AttachTo(graphicBox, 0, 0, 100, 100);
          if (editorArea != null)
            graphicBox.AttachTo(editorArea.Box, AttachToNode.TopLeft);
        }

        editorNode.Visible = isVisible;
        editorNodes.Add(editorNode.Guid, editorNode);
      }
    }

    public ModelStencil ModelStencil(NodeClass nodeClass)
    {
      ModelStencil modelStencil;
      config.ModelStencils.TryGetValue(nodeClass, out modelStencil);
      return modelStencil;
    }

    private void NewElementSelection()
    {
      if (newElementSelectionList.Count > 0)
      {
        foreach (Guid guid in newElementSelectionList)
        {
          if (editorNodes.ContainsKey(guid))
            editorNodes[guid].ModelBox.Selected = true;
          if (editorLinks.ContainsKey(guid))
            editorLinks[guid].Arrow.Selected = true;
        }
      }
    }

    internal void CreateLink(ModelLink modelLink, GraphicLink graphicLink)
    {

      if (flowChart.InvokeRequired)
      {
        flowChart.BeginInvoke(new CreateLinkDelegate(CreateLink), new object[] { modelLink, graphicLink });
      }
      else
      {
        bool isVisible = false;

        {
          EditorNode destinationNode, originNode;
          if (
            (editorNodes.TryGetValue(graphicLink.Origin, out originNode))
            &&
            (editorNodes.TryGetValue(graphicLink.Destination, out destinationNode))
            )
          {
            isVisible = ((originNode.Visible) || (destinationNode.Visible));
          }
        }

        Box textBox = null;

        {
          SysCAD.Protocol.Rectangle textArea = graphicLink.TagArea;

          if (textArea.IsEmpty) // We haven't got a TagArea stored in the item yet.
          {
            SysCAD.Protocol.Point pointOrigin = new SysCAD.Protocol.Point();
            SysCAD.Protocol.Point pointDestination = new SysCAD.Protocol.Point();
            SysCAD.Protocol.Point pointCenter = new SysCAD.Protocol.Point();

            if (graphicLink.ControlPoints != null && graphicLink.ControlPoints.Count > 1)
            {
              pointOrigin = graphicLink.ControlPoints[0];
              pointDestination = graphicLink.ControlPoints[graphicLink.ControlPoints.Count - 1];
              pointCenter = new SysCAD.Protocol.Point((pointDestination.X + pointOrigin.X) / 2.0F, (pointDestination.Y + pointOrigin.Y) / 2.0F);
            }

            textArea = new SysCAD.Protocol.Rectangle(pointCenter.X,
                                      pointCenter.Y + 4.0,
                                      20.0,
                                      4.0);
          }

          textBox = flowChart.CreateBox((float)textArea.X, (float)textArea.Y, (float)textArea.Width, (float)textArea.Height);
          textBox.RotationAngle = (float)graphicLink.TagAngle;
          textBox.FillColor = System.Drawing.Color.FromArgb(1, System.Drawing.Color.Black);
          textBox.FrameColor = System.Drawing.Color.FromArgb(1, System.Drawing.Color.Black);
          textBox.RotateContents = true;
          textBox.Style = BoxStyle.Shape;
          textBox.Shape = ShapeTemplate.FromId("Rectangle");
          textBox.EnabledHandles = Handles.ResizeTopLeft | Handles.ResizeTopRight |
            Handles.ResizeBottomRight | Handles.ResizeBottomLeft | Handles.ResizeTopCenter |
            Handles.ResizeMiddleRight | Handles.ResizeBottomCenter | Handles.ResizeMiddleLeft |
            Handles.Move;
          textBox.Text = graphicLink.Tag;
        }

        Arrow arrow = flowChart.CreateArrow(new PointF(0.0F, 0.0F), new PointF(10.0F, 10.0F));
        arrow.CustomDraw = CustomDraw.Additional;

        {
          switch (modelLink.LinkClass)
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

          EditorNode origin = null;
          EditorNode destination = null;

          origin = Item(graphicLink.Origin);
          destination = Item(graphicLink.Destination);

          if (origin == null)
          {
            clientProtocol.LogMessage(out requestId, "Origin Guid " + graphicLink.Origin + " not found for link " + graphicLink.Tag, SysCAD.Log.MessageType.Error);
            return;
          }

          if (destination == null)
          {
            clientProtocol.LogMessage(out requestId, "Destination Guid " + graphicLink.Destination + " not found for link " + graphicLink.Tag, SysCAD.Log.MessageType.Error);
            return;
          }

          SysCAD.Protocol.Point pointOrigin = new SysCAD.Protocol.Point();
          SysCAD.Protocol.Point pointDestination = new SysCAD.Protocol.Point();

          if (graphicLink.ControlPoints != null && graphicLink.ControlPoints.Count > 1)
          {
            pointOrigin = graphicLink.ControlPoints[0];
            pointDestination = graphicLink.ControlPoints[graphicLink.ControlPoints.Count - 1];
          }

          if (origin != null)
          {
            if (origin.HiddenBox != null)
              arrow.Origin = origin.HiddenBox;
            else
              arrow.Origin = origin.ModelBox;
          }

          if (destination != null)
          {
            if (destination.HiddenBox != null)
              arrow.Destination = destination.HiddenBox;
            else
              arrow.Destination = destination.ModelBox;
          }

          if ((modelLink.OriginPort != null) && ((origin.ModelBox.Tag as EditorNode).anchorTagToInt.ContainsKey(modelLink.OriginPort + graphicLink.OriginPortID.ToString())))
            arrow.OrgnAnchor = (origin.ModelBox.Tag as EditorNode).anchorTagToInt[modelLink.OriginPort + graphicLink.OriginPortID.ToString()];
          else
          {
            Console.WriteLine("(origin.ModelBox.Tag as EditorNode).modelNode : " +
              (origin.ModelBox.Tag as EditorNode).ModelNode.Guid.ToString() + " : " +
              (origin.ModelBox.Tag as EditorNode).ModelNode.NodeClass.ToString() + " : " +
              (origin.ModelBox.Tag as EditorNode).ModelNode.Tag.ToString());

            Console.WriteLine("modelLink.OriginPort : " + modelLink.OriginPort);
            Console.WriteLine("graphicLink.OriginPortID.ToString() : " + graphicLink.OriginPortID.ToString());
            Console.WriteLine("anchorTagToInt Key:Value pairs:");
            foreach (string key in (origin.ModelBox.Tag as EditorNode).anchorTagToInt.Keys)
            {
              Console.WriteLine("  " + key + " : " + (origin.ModelBox.Tag as EditorNode).anchorTagToInt[key].ToString());
            }

            Console.WriteLine();
            Console.WriteLine();

            arrow.DestAnchor = -1;
          }

          if ((modelLink.DestinationPort != null) && ((destination.ModelBox.Tag as EditorNode).anchorTagToInt.ContainsKey(modelLink.DestinationPort + graphicLink.DestinationPortID.ToString())))
            arrow.DestAnchor = (destination.ModelBox.Tag as EditorNode).anchorTagToInt[modelLink.DestinationPort + graphicLink.DestinationPortID.ToString()];
          else
          {
            Console.WriteLine("(destination.ModelBox.Tag as EditorNode).modelNode : " +
              (destination.ModelBox.Tag as EditorNode).ModelNode.Guid.ToString() + " : " +
              (destination.ModelBox.Tag as EditorNode).ModelNode.NodeClass.ToString() + " : " +
              (destination.ModelBox.Tag as EditorNode).ModelNode.Tag.ToString());

            Console.WriteLine("modelLink.DestinationPort : " + modelLink.DestinationPort);
            Console.WriteLine("graphicLink.DestinationPortID.ToString() : " + graphicLink.DestinationPortID.ToString());
            Console.WriteLine("anchorTagToInt Key:Value pairs:");
            foreach (string key in (destination.ModelBox.Tag as EditorNode).anchorTagToInt.Keys)
            {
              Console.WriteLine("  " + key + " : " + (destination.ModelBox.Tag as EditorNode).anchorTagToInt[key].ToString());
            }

            Console.WriteLine();
            Console.WriteLine();

            arrow.DestAnchor = -1;
          }

          String originTag = "";

          if (origin != null) originTag = origin.Tag;

          String destinationTag = "";

          if (destination != null) destinationTag = destination.Tag;

          String originPort = "";
          if (modelLink.OriginPort != null) originPort = modelLink.OriginPort;

          String destinationPort = "";
          if (modelLink.DestinationPort != null) destinationPort = modelLink.DestinationPort;

          arrow.ToolTip = "Tag:" + graphicLink.Tag +
            "\nSrc: " + originTag + ":" + originPort +
            "\nDst: " + destinationTag + ":" + destinationPort;
          arrow.ArrowHead = ArrowHead.Triangle;
          arrow.Style = ArrowStyle.Cascading;

          if (graphicLink.ControlPoints != null && graphicLink.ControlPoints.Count > 1)
          {
            SetControlPoints(arrow, graphicLink.ControlPoints);
          }

          if (graphicLink.ControlPoints[0].X == graphicLink.ControlPoints[1].X)
          {
            arrow.CascadeOrientation = Orientation.Auto;
          }
          else
          {
            arrow.CascadeOrientation = Orientation.Auto;
          }
        }

        EditorLink editorLink = new EditorLink(this, graphicLink, modelLink);
        editorLink.Arrow = arrow;
        editorLink.TextBox = textBox;
        editorLink.UpdateVisibility();

        arrow.Tag = editorLink;
        textBox.Tag = editorLink;

        editorLinks.Add(editorLink.Guid, editorLink);

        NewElementSelection();
      }
    }

    internal void DeleteNode(Guid guid)
    {

      if (flowChart.InvokeRequired)
      {
        flowChart.BeginInvoke(new DeleteNodeDelegate(DeleteNode), new object[] { guid });
      }

      else
      {
        // This isn't perfect -- I'm only handling the GraphicNode delete and ignoring the ModelNode delete.
        PureComponents.TreeView.Node treeViewNode = tvNavigation.GetNodeByKey(guid.ToString());

        if (treeViewNode != null)
          treeViewNode.Remove();


        EditorNode editorItem;

        if (editorNodes.TryGetValue(guid, out editorItem))
        {
          if (editorItem.ModelBox != null) flowChart.DeleteObject(editorItem.ModelBox);
          if (editorItem.GraphicBox != null) flowChart.DeleteObject(editorItem.GraphicBox);
          if (editorItem.TextBox != null) flowChart.DeleteObject(editorItem.TextBox);
          if (editorItem.HiddenBox != null) flowChart.DeleteObject(editorItem.HiddenBox);
          editorNodes.Remove(guid);
        }
      }
    }

    internal void DeleteLink(Guid guid)
    {

      if (flowChart.InvokeRequired)
      {
        flowChart.BeginInvoke(new DeleteLinkDelegate(DeleteLink), new object[] { guid });
      }

      else
      {
        EditorLink link;

        if (editorLinks.TryGetValue(guid, out link))
        {
          flowChart.DeleteObject(link.Arrow);
          editorLinks.Remove(guid);
        }
      }
    }

    internal bool Exists(Guid guid)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      EditorLink link;
      editorLinks.TryGetValue(guid, out link);

      return ((link != null) || (item != null));
    }

    internal bool Exists(String tag)
    {

      foreach (EditorNode item in editorNodes.Values)
      {

        if (item.Tag == tag)
          return true;
      }

      foreach (EditorLink link in editorLinks.Values)
      {

        if (link.Tag == tag)
          return true;
      }

      return false;
    }

    internal static List<SysCAD.Protocol.Point> GetControlPoints(MindFusion.FlowChartX.PointCollection pointCollection)
    {
      List<SysCAD.Protocol.Point> list = new List<SysCAD.Protocol.Point>(pointCollection.Capacity);

      foreach (PointF point in pointCollection)
      {
        list.Add(new SysCAD.Protocol.Point(point));
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

    internal GraphicNode GraphicNode(Guid guid)
    {
      GraphicNode graphicNode = null;
      clientProtocol.graphic.Nodes.TryGetValue(guid, out graphicNode);
      return graphicNode;
    }

    internal GraphicNode GraphicNode(Box box)
    {
      GraphicNode graphicNode = null;
      if (box.Tag is EditorNode)
        graphicNode = GraphicNode((box.Tag as EditorNode).Guid);
      return graphicNode;
    }

    internal ModelNode ModelNode(Box box)
    {
      ModelNode modelNode = null;

      if (box.Tag is EditorNode)
      {
        GraphicNode graphicNode = null;
        clientProtocol.graphic.Nodes.TryGetValue((box.Tag as EditorNode).Guid, out graphicNode);
        if (graphicNode != null)
          clientProtocol.model.Nodes.TryGetValue(graphicNode.ModelGuid, out modelNode);
      }

      return modelNode;
    }

    internal GraphicArea GraphicArea(Guid guid)
    {
      GraphicArea graphicArea = null;
      clientProtocol.graphic.Areas.TryGetValue(guid, out graphicArea);
      return graphicArea;
    }

    internal GraphicArea GraphicArea(Box box)
    {
      GraphicArea graphicArea = null;

      if (box.Tag is EditorNode)
        clientProtocol.graphic.Areas.TryGetValue((box.Tag as EditorNode).Guid, out graphicArea);
      return graphicArea;
    }

    internal GraphicNode GraphicItem(Guid guid)
    {
      GraphicNode graphicItem = null;
      clientProtocol.graphic.Nodes.TryGetValue(guid, out graphicItem);
      return graphicItem;
    }

    internal GraphicLink GraphicLink(Arrow arrow)
    {
      GraphicLink graphicLink = null;

      if ((arrow != null) && (arrow.Tag != null))
        clientProtocol.graphic.Links.TryGetValue((arrow.Tag as EditorLink).Guid, out graphicLink);
      return graphicLink;
    }

    internal GraphicLink GraphicLink(Guid guid)
    {
      GraphicLink graphicLink = null;
      clientProtocol.graphic.Links.TryGetValue(guid, out graphicLink);
      return graphicLink;
    }

    internal GraphicStencil GraphicStencil(String stencilName)
    {
      GraphicStencil graphicStencil;
      config.GraphicStencils.TryGetValue(stencilName, out graphicStencil);
      return graphicStencil;
    }

    internal bool IsItem(Guid guid)
    {
      return clientProtocol.graphic.Nodes.ContainsKey(guid);
    }

    internal bool IsLink(Guid guid)
    {
      return clientProtocol.graphic.Links.ContainsKey(guid);
    }

    internal EditorNode Item(Guid guid)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);
      return item;
    }

    internal void ItemSelected(Guid guid, bool selected)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        item.Selected = selected;
        item.ModelBox.Selected = selected && item.Visible;
      }
    }

    internal void SetVisible(String keyGuid, bool visible)
    {
      if (keyGuid != null)
      {
        Guid guid = new Guid(keyGuid);
        EditorNode node;
        EditorArea area;

        if (editorAreas.TryGetValue(guid, out area))
        {
          area.Visible = visible;
        }

        if (editorNodes.TryGetValue(guid, out node))
        {
          node.Visible = visible;
        }

      }
    }

    internal EditorLink Link(Guid guid)
    {
      EditorLink link;
      editorLinks.TryGetValue(guid, out link);
      return link;
    }

    static internal double Mirrored(double x, bool mirrored)
    {

      if (mirrored)
        return 100.0 - x;

      else
        return x;
    }

    internal ModelStencil ModelStencil(String stencilName)
    {
      ModelStencil modelStencil;
      config.ModelStencils.TryGetValue(stencilName, out modelStencil);
      return modelStencil;
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
        EditorNode item = box.Tag as EditorNode;

        if (item != null)
        {
          item.Remove(flowChart);
          editorNodes.Remove(item.Guid);
        }
      }

      ArrowCollection arrowSelection = flowChart.Selection.Arrows.Clone();

      foreach (Arrow arrow in arrowSelection)
      {
        EditorLink link = arrow.Tag as EditorLink;

        if (link != null)
        {
          link.Remove(flowChart);
          editorLinks.Remove(link.Guid);
        }
      }
    }

    internal void SetAngle(Guid guid, float angle)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        item.ModelBox.RotationAngle = angle;
        item.GraphicBox.RotationAngle = angle;
      }
    }

    internal void SetFillColor(Guid guid, System.Drawing.Color fillColor)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        item.GraphicBox.FillColor = fillColor;
      }
    }

    internal void SetHeight(Guid guid, float height)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        SysCAD.Protocol.Rectangle boundingRect = new SysCAD.Protocol.Rectangle(item.ModelBox.BoundingRect);
        boundingRect.Height = height;
        item.ModelBox.BoundingRect = boundingRect;
        item.GraphicBox.BoundingRect = boundingRect;
      }
    }

    internal void SetMirrorX(Guid guid, bool mirrorX)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        GraphicNode graphicNode;

        if (clientProtocol.graphic.Nodes.TryGetValue(guid, out graphicNode))
        {
          graphicNode.MirrorX = mirrorX;

          GraphicStencil graphicStencil = GraphicStencil(graphicNode.Stencil);

          if (graphicStencil != null)
          {
            item.GraphicBox.Shape = GetShapeTemplate(graphicStencil, graphicNode.MirrorX, graphicNode.MirrorY);
          }
        }
      }
    }

    internal void SetMirrorY(Guid guid, bool mirrorY)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        GraphicNode graphicNode;

        if (clientProtocol.graphic.Nodes.TryGetValue(guid, out graphicNode))
        {
          graphicNode.MirrorY = mirrorY;

          GraphicStencil graphicStencil = GraphicStencil(graphicNode.Stencil);

          if (graphicStencil != null)
          {
            item.GraphicBox.Shape = GetShapeTemplate(graphicStencil, graphicNode.MirrorX, graphicNode.MirrorY);
          }
        }
      }
    }

    internal void SetWidth(Guid guid, float width)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        SysCAD.Protocol.Rectangle boundingRect = new SysCAD.Protocol.Rectangle(item.ModelBox.BoundingRect);
        boundingRect.Width = width;
        item.ModelBox.BoundingRect = boundingRect;
        item.GraphicBox.BoundingRect = boundingRect;
      }
    }

    internal void SetX(Guid guid, float x)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        SysCAD.Protocol.Rectangle boundingRect = new SysCAD.Protocol.Rectangle(item.ModelBox.BoundingRect);
        boundingRect.X = x;
        item.ModelBox.BoundingRect = boundingRect;
        item.GraphicBox.BoundingRect = boundingRect;
      }
    }

    internal void SetY(Guid guid, float y)
    {
      EditorNode item;
      editorNodes.TryGetValue(guid, out item);

      if (item != null)
      {
        SysCAD.Protocol.Rectangle boundingRect = new SysCAD.Protocol.Rectangle(item.ModelBox.BoundingRect);
        boundingRect.Y = y;
        item.ModelBox.BoundingRect = boundingRect;
        item.GraphicBox.BoundingRect = boundingRect;
      }
    }

    internal void PermissionsChanged(ClientBaseProtocol.Permissions permissions)
    {
      this.permissions = permissions;
    }

    internal void Step(Int64 step, DateTime time)
    {
      this.step = step;
      this.time = time;
    }

    static private ElementTemplate MirroredElement(object element, bool mirrorX, bool mirrorY)
    {
      SysCAD.Protocol.Line line = element as SysCAD.Protocol.Line;

      if (line != null) return new LineTemplate((float)Mirrored(line.x1, mirrorX), (float)Mirrored(line.y1, mirrorY),
            (float)Mirrored(line.x2, mirrorX), (float)Mirrored(line.y2, mirrorY));

      Arc arc = element as Arc;

      if (arc != null) return new ArcTemplate((float)Mirrored(arc.x, mirrorX), (float)Mirrored(arc.y, mirrorY),
        (float)arc.w, (float)arc.h, (float)arc.a, (float)arc.s);

      Bezier bezier = element as Bezier;

      if (bezier != null) return new BezierTemplate((float)Mirrored(bezier.x1, mirrorX), (float)Mirrored(bezier.y1, mirrorY),
            (float)Mirrored(bezier.x2, mirrorX), (float)Mirrored(bezier.y2, mirrorY),
            (float)Mirrored(bezier.x3, mirrorX), (float)Mirrored(bezier.y3, mirrorY),
            (float)Mirrored(bezier.x4, mirrorX), (float)Mirrored(bezier.y4, mirrorY));

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

    public FlowChart FlowChart
    {
      get { return flowChart; }
      set { flowChart = value; }
    }

    public ClientProtocol ClientProtocol
    {
      get { return clientProtocol; }
      set { clientProtocol = value; }
    }

    public Config Config
    {
      get { return config; }
      set
      {
        config = value;

        FlowChart flowchart = new FlowChart();

        foreach (String key in config.ModelStencils.Keys)
        {
          ModelStencil stencil = config.ModelStencils[key];
          flowchart.DocExtents = flowchart.ClientToDoc(new System.Drawing.Rectangle(0, 0, 17, 17));
          flowchart.ShadowsStyle = ShadowsStyle.None;
          flowchart.BackColor = System.Drawing.SystemColors.Window;
          flowchart.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
          RectangleF boxRect = flowchart.ClientToDoc(new System.Drawing.Rectangle(1, 1, 13, 13));
          Box box = flowchart.CreateBox(boxRect.X, boxRect.Y, boxRect.Width, boxRect.Height);
          box.Style = BoxStyle.Shape;
          box.Shape = GetShapeTemplate(stencil, false, false);
          box.FillColor = System.Drawing.Color.FromArgb(220, 222, 184, 136);
          box.FrameColor = System.Drawing.Color.FromArgb(255, 111, 92, 68);
          box.Locked = true;

          flowchart.BackColor = System.Drawing.SystemColors.Window;
          modelUnselectedThumbnails.Add(key, flowchart.CreateImage());

          flowchart.BackColor = System.Drawing.SystemColors.Highlight;
          modelSelectedThumbnails.Add(key, flowchart.CreateImage());

          flowchart.DeleteObject(box);
        }

        foreach (String key in config.GraphicStencils.Keys)
        {
          GraphicStencil stencil = config.GraphicStencils[key];
          flowchart.DocExtents = flowchart.ClientToDoc(new System.Drawing.Rectangle(0, 0, 17, 17));
          flowchart.ShadowsStyle = ShadowsStyle.None;
          flowchart.BackColor = System.Drawing.SystemColors.Window;
          flowchart.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
          RectangleF boxRect = flowchart.ClientToDoc(new System.Drawing.Rectangle(1, 1, 13, 13));
          Box box = flowchart.CreateBox(boxRect.X, boxRect.Y, boxRect.Width, boxRect.Height);
          box.Style = BoxStyle.Shape;
          box.Shape = GetShapeTemplate(stencil, false, false);
          box.Locked = true;

          flowchart.BackColor = System.Drawing.SystemColors.Window;
          graphicUnselectedThumbnails.Add(key, flowchart.CreateImage());

          flowchart.BackColor = System.Drawing.SystemColors.Highlight;
          graphicSelectedThumbnails.Add(key, flowchart.CreateImage());

          flowchart.DeleteObject(box);
        }
      }
    }

    public IEnumerable<GraphicStencil> GraphicStencilValues
    {
      get { return config.GraphicStencils.Values; }
    }

    public IEnumerable<ModelStencil> ModelStencilValues
    {
      get { return config.ModelStencils.Values; }
    }

    public Dictionary<String, GraphicStencil> GraphicStencils
    {
      get { return config.GraphicStencils; }
    }

    public Dictionary<String, ModelStencil> ModelStencils
    {
      get { return config.ModelStencils; }
    }

    public bool ProjectAttached
    {
      get { return projectOpen; }
      set { projectOpen = value; }
    }

    public ClientBaseProtocol.Permissions Permissions
    {
      get { return permissions; }
      set { permissions = value; }
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

    public bool ShowAreas
    {
      get { return showAreas; }
      set { showAreas = value; }
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

    internal IEnumerable<GraphicNode> GraphicItems
    {
      get { return clientProtocol.graphic.Nodes.Values; }
    }

    internal IEnumerable<GraphicLink> GraphicLinks
    {
      get { return clientProtocol.graphic.Links.Values; }
    }

    internal IEnumerable<EditorArea> Areas
    {
      get { return editorAreas.Values; }
    }

    internal IEnumerable<EditorNode> Nodes
    {
      get { return editorNodes.Values; }
    }

    internal IEnumerable<EditorLink> Links
    {
      get { return editorLinks.Values; }
    }

    internal void ConnectGraphic(
      ClientProtocol.PermissionsChangedHandler permissionsChangedHandler,
      ClientProtocol.StepHandler stepHandler,
      ClientProtocol.SyncHandler syncHandler,
      ClientProtocol.ChangedHandler changedHandler,
      ClientProtocol.PortInfoRequestedHandler portInfoRequestedHandler)
    {
      clientProtocol.PermissionsChanged += permissionsChangedHandler;

      clientProtocol.Step += stepHandler;

      clientProtocol.Sync += syncHandler;

      clientProtocol.Changed += changedHandler;

      clientProtocol.PortInfoRequested += portInfoRequestedHandler;
    }

    internal void DisconnectGraphic(
      ClientProtocol.PermissionsChangedHandler permissionsChangedHandler,
      ClientProtocol.StepHandler stepHandler,
      ClientProtocol.SyncHandler syncHandler,
      ClientProtocol.ChangedHandler changedHandler,
      ClientProtocol.PortInfoRequestedHandler portInfoRequestedHandler)
    {
      clientProtocol.PermissionsChanged -= permissionsChangedHandler;
      clientProtocol.Step -= stepHandler;
      clientProtocol.Sync -= syncHandler;
      clientProtocol.Changed -= changedHandler;
      clientProtocol.PortInfoRequested -= portInfoRequestedHandler;
    }

    internal void ModifyNode(Guid guid)
    {
      if (flowChart.InvokeRequired)
      {
        flowChart.BeginInvoke(new ModifyNodeDelegate(ModifyNode), new object[] { guid });
      }
      else
      {
        GraphicNode graphicNode;

        if (ClientProtocol.graphic.Nodes.TryGetValue(guid, out graphicNode))
        {
          ModelNode modelNode;
          if (ClientProtocol.model.Nodes.TryGetValue(graphicNode.ModelGuid, out modelNode))
          {
            ModifyNode(modelNode, graphicNode);
          }
          else
          {
            ClientProtocol.LogMessage(out requestId, "Modify: ModelNode missing for GraphcNode (Tag: " + graphicNode.Tag + ", Guid: " + graphicNode.Guid + ")", SysCAD.Log.MessageType.Error);
          }
        }
      }
    }

    internal void ModifyLink(Guid guid)
    {
      if (flowChart.InvokeRequired)
      {
        flowChart.BeginInvoke(new ModifyNodeDelegate(ModifyLink), new object[] { guid });
      }
      else
      {
        GraphicLink graphicLink;

        if (ClientProtocol.graphic.Links.TryGetValue(guid, out graphicLink))
        {
          ModelLink modelLink;
          if (ClientProtocol.model.Links.TryGetValue(graphicLink.ModelGuid, out modelLink))
          {
            ModifyLink(modelLink, graphicLink);
          }
          else
          {
            ClientProtocol.LogMessage(out requestId, "Modify: ModelLink missing for GraphcLink (Tag: " + graphicLink.Tag + ", Guid: " + graphicLink.Guid + ")", SysCAD.Log.MessageType.Error);
          }
        }
      }
    }

    private void ModifyNode(ModelNode modelNode, GraphicNode graphicNode)
    {
      EditorNode editorNode = null;

      if (!editorNodes.TryGetValue(graphicNode.Guid, out editorNode))
      {
        clientProtocol.LogMessage(out requestId, "ModifyNode: EditorNode not found in library for GraphicNode \'" + graphicNode.Guid + ", " + graphicNode.Tag + "\'", SysCAD.Log.MessageType.Error);
        return;
      }

      editorNode.GraphicNode = graphicNode;
      editorNode.ModelNode = modelNode;

      Box graphicBox = editorNode.GraphicBox;
      Box modelBox = editorNode.ModelBox;
      Box textBox = editorNode.TextBox;
      Box hiddenBox = editorNode.HiddenBox;

      if (graphicBox != null)
      {
        graphicBox.BoundingRect = graphicNode.BoundingRect;
        graphicBox.RotationAngle = (float)graphicNode.Angle;
        graphicBox.FillColor = graphicNode.FillColor;

        {
          GraphicStencil graphicStencil = GraphicStencil(graphicNode.Stencil);

          if (graphicStencil != null)
            graphicBox.Shape = State.GetShapeTemplate(graphicStencil, graphicNode.MirrorX, graphicNode.MirrorY);
          else
            graphicBox.Shape = ShapeTemplate.FromId("Decision2");
        }
      }
      if (modelBox != null)
      {
        modelBox.BoundingRect = graphicNode.BoundingRect;
        modelBox.RotationAngle = (float)graphicNode.Angle;

        ModelStencil modelStencil = ModelStencil(modelNode.NodeClass);

        if (modelStencil != null)
          modelBox.Shape = State.GetShapeTemplate(modelStencil, graphicNode.MirrorX, graphicNode.MirrorY);
        else
          modelBox.Shape = ShapeTemplate.FromId("Decision2");

        modelBox.AnchorPattern = GetAnchorPattern(modelStencil, editorNode);

      }
      if (textBox != null)
      {
        textBox.BoundingRect = graphicNode.TagArea;
        textBox.RotationAngle = (float)graphicNode.TagAngle;
        // We're only rotating the boundingbox, not the action text.
      }
      if (hiddenBox != null)
      {
        hiddenBox.BoundingRect = graphicNode.BoundingRect;
        hiddenBox.RotationAngle = (float)graphicNode.Angle;
      }


      foreach (Arrow arrow in editorNode.IncomingArrows)
      {
        ModifyLink((arrow.Tag as EditorLink).Guid);
      }

      foreach (Arrow arrow in editorNode.OutgoingArrows)
      {
        ModifyLink((arrow.Tag as EditorLink).Guid);
      }

      editorNode.UpdateVisibility();
      //if (
      //else
      //{
      //  clientProtocol.LogMessage(out requestId, "ModifyNode: One of the *boxes missing for GraphicNode \'" + graphicNode.Guid + ", " + graphicNode.Tag + "\'", SysCAD.Log.MessageType.Error);
      //  return;
      //}
    }

    private void ModifyLink(ModelLink modelLink, GraphicLink graphicLink)
    {
      EditorLink editorLink = null;

      if (!editorLinks.TryGetValue(graphicLink.Guid, out editorLink))
      {
        clientProtocol.LogMessage(out requestId, "ModifyLink: EditorLink not found in library for graphicLink \'" + graphicLink.Guid + ", " + graphicLink.Tag + "\'", SysCAD.Log.MessageType.Error);
        return;
      }

      Arrow arrow = editorLink.Arrow;
      Box text = editorLink.TextBox;

      if ((arrow != null) && (text != null))
      {
        State.SetControlPoints(arrow, graphicLink.ControlPoints);

        text.BoundingRect = graphicLink.TagArea;
        text.RotationAngle = (float)graphicLink.TagAngle;
      }
      else
      {
        clientProtocol.LogMessage(out requestId, "ModifyLink: One of the objects missing for GraphicNode \'" + graphicLink.Guid + ", " + graphicLink.Tag + "\'", SysCAD.Log.MessageType.Error);
        return;
      }
    }

    internal static bool CompareControlPoints(PointCollection pointCollection, List<SysCAD.Protocol.Point> list)
    {
      if (pointCollection.Count == list.Count) return false;

      for (int i = 0; i < list.Count; i++)
      {
        if (pointCollection[i].X != list[i].X) return false;
        if (pointCollection[i].Y != list[i].Y) return false;
      }

      return true;
    }
  }
}
