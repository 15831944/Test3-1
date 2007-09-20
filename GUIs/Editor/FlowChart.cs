
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;

using MindFusion.FlowChartX;

using SysCAD.Protocol;
using PureComponents.TreeView;
using System.Windows.Forms;
using ActiproSoftware.UIStudio.Bar;
using MindFusion.FlowChartX.LayoutSystem;
using System.Drawing.Drawing2D;
using System.Collections;
using System.Globalization;

namespace SysCAD.Editor
{

  internal partial class FrmFlowChart : Form
  {

    private Arrow arrowBeingModified;
    private int arrowBeingModifiedSelectionHandle = -1;

    private String currentModel;
    private String currentStencil;
    private Anchor destinationAnchorChosen;

    private EditorForm form1;

    private Arrow hoverArrow;
    private Box hoverBox;

    System.Drawing.Point mousePressed;
    int newDestinationAnchor = -1;
    Box newDestinationBox;

    Guid newDestinationGuid = Guid.Empty;
    int newOriginAnchor = -1;
    Box newOriginBox;

    Guid newOriginGuid = Guid.Empty;

    List<SysCAD.Protocol.Point> oldControlPoints = new List<SysCAD.Protocol.Point>();
    int oldDestinationAnchor = -1;
    Box oldDestinationBox;

    Guid oldDestinationGuid = Guid.Empty;

    private Arrow oldHoverArrow;
    private Box oldHoverBox;
    int oldOriginAnchor = -1;
    Box oldOriginBox;

    Guid oldOriginGuid = Guid.Empty;

    private Anchor originAnchorChosen;

    private State state = new State();
    private int tempArrowKey;

    private int tempBoxKey;

    public FrmFlowChart(EditorForm form1)
    {
      this.form1 = form1;
      InitializeComponent();

      this.fcFlowChart.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.fcFlowChart_MouseWheel);

      fcFlowChart.DocExtents = new RectangleF(0.0F, 0.0F, 297.0F, 210.0F);
      fcFlowChart.Selection.Style = SelectionStyle.SemiTransparent;
    }

    public void FixDocExtents()
    {
      float minX = float.MaxValue, minY = float.MaxValue;
      float maxX = -float.MaxValue, maxY = -float.MaxValue;

      bool foundObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {
        foundObject = true;

        if (box.BoundingRect.Left < minX) minX = box.BoundingRect.Left;

        if (box.BoundingRect.Top < minY) minY = box.BoundingRect.Top;

        if (box.BoundingRect.Right > maxX) maxX = box.BoundingRect.Right;

        if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {
        foundObject = true;

        if (arrow.BoundingRect.Left < minX) minX = arrow.BoundingRect.Left;

        if (arrow.BoundingRect.Top < minY) minY = arrow.BoundingRect.Top;

        if (arrow.BoundingRect.Right > maxX) maxX = arrow.BoundingRect.Right;

        if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundObject)
        fcFlowChart.DocExtents = RectangleF.FromLTRB(minX - width * 0.05F, minY - height * 0.05F, maxX + width * 0.05F, maxY + height * 0.05F);
    }

    public void NewGraphicItem(GraphicItem graphicItem, String path)
    {
      Guid guid;
      NewGraphicItem(out guid, graphicItem, path);
    }

    public void NewGraphicItem(out Guid guid, GraphicItem graphicItem, String path)
    {
      if (graphicItem != null)
        NewGraphicItem(out guid, path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.TextArea, graphicItem.TextAngle, graphicItem.FillColor, graphicItem.FillMode, graphicItem.MirrorX, graphicItem.MirrorY);
      else
        guid = Guid.Empty;
    }

    public void NewGraphicItem(out Guid guid, String path, String model, String shape, SysCAD.Protocol.Rectangle boundingRect, Double angle, SysCAD.Protocol.Rectangle textArea, Double textAngle, Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      Int64 requestId;

      while (state.Exists("N_" + tempBoxKey.ToString(CultureInfo.InvariantCulture)))
        tempBoxKey++;

      state.CreateGraphicItem(out requestId, out guid, "N_" + tempBoxKey.ToString(), path, model, shape, boundingRect, angle, textArea, textAngle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public void NewGraphicLink(out Guid guid, GraphicLink graphicLink)
    {
      if (graphicLink != null)
        NewGraphicLink(out guid, graphicLink.ClassID, graphicLink.Origin, graphicLink.Destination, graphicLink.OriginPort, graphicLink.DestinationPort, graphicLink.ControlPoints, graphicLink.TextArea, graphicLink.TextAngle);
      else
        guid = Guid.Empty;
    }

    public void NewGraphicLink(out Guid guid, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<SysCAD.Protocol.Point> controlPoints, SysCAD.Protocol.Rectangle textArea, Double textAngle)
    {
      Int64 requestId;

      state.CreateGraphicLink(out requestId, out guid, "A_" + tempBoxKey.ToString(), classId, origin, destination, originPort, destinationPort, controlPoints, textArea, textAngle);
    }

    //public GraphicLink NewGraphicLink(out Guid guid, GraphicLink graphicLink, float dx, float dy)
    //{
    //  while (state.Exists("A_" + tempBoxKey.ToString(CultureInfo.InvariantCulture)))
    //    tempBoxKey++;

    //  GraphicLink newGraphicLink = new GraphicLink("A_" + tempBoxKey.ToString());
    //  newGraphicLink.Destination = graphicLink.Destination;
    //  newGraphicLink.Origin = graphicLink.Origin;

    //  foreach (PointF point in graphicLink.ControlPoints)
    //  {
    //    newGraphicLink.ControlPoints.Add(new PointF(point.X + dx, point.Y + dy));
    //  }

    //  state.CreateLink(newGraphicLink, true, fcFlowChart);

    //  return newGraphicLink;
    //}

    public void NewGraphicThing(GraphicThing graphicThing, String path)
    {
      Guid guid;
      NewGraphicThing(out guid, graphicThing, path);
    }

    public void NewGraphicThing(out Guid guid, GraphicThing graphicThing, String path)
    {
      if (graphicThing != null)
        NewGraphicThing(out guid, path, graphicThing.BoundingRect, graphicThing.Xaml, graphicThing.Angle, graphicThing.MirrorX, graphicThing.MirrorY);
      else
        guid = Guid.Empty;
    }

    public void NewGraphicThing(out Guid guid, String path, SysCAD.Protocol.Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      Int64 requestId;

      while (state.Exists("N_" + tempBoxKey.ToString(CultureInfo.InvariantCulture)))
        tempBoxKey++;

      state.CreateGraphicThing(out requestId, out guid, "N_" + tempBoxKey.ToString(), path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public void SetSizes()
    {
      float zoomFactor = fcFlowChart.ZoomFactor;

      fcFlowChart.SelHandleSize = 100.0F / zoomFactor;
      fcFlowChart.MergeThreshold = 100.0F / zoomFactor;

      fcFlowChart.ArrowHeadSize = 150.0F / zoomFactor;

      if (80.0F / zoomFactor > getMinArrowheadSize(fcFlowChart.MeasureUnit))
        fcFlowChart.ArrowIntermSize = 80.0F / zoomFactor;

      else
        fcFlowChart.ArrowIntermSize = getMinArrowheadSize(fcFlowChart.MeasureUnit);
      fcFlowChart.ArrowBaseSize = 150.0F / zoomFactor;

      foreach (Link link in state.Links)
      {
        link.Arrow.ArrowHeadSize = 150.0F / zoomFactor;

        if (80.0F / zoomFactor > getMinArrowheadSize(fcFlowChart.MeasureUnit))
          link.Arrow.IntermHeadSize = 120.0F / zoomFactor;

        else
          link.Arrow.IntermHeadSize = getMinArrowheadSize(fcFlowChart.MeasureUnit);
        link.Arrow.ArrowBaseSize = 150.0F / zoomFactor;
      }

      fcFlowChart.Font = new System.Drawing.Font("Microsoft Sans Serif", zoomFactor / 10.0F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));

      foreach (Item item in state.Items)
      {
        item.Text.Font = new System.Drawing.Font("Microsoft Sans Serif", zoomFactor / 10.0F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      }

      //foreach (Thing thing in state.Things)
      //{
      //  thing.Box.Font = new System.Drawing.Font("Microsoft Sans Serif", zoomFactor / 10.0F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      //}

      fcFlowChart.Invalidate();
    }

    public void ZoomToSelected()
    {
      FixDocExtents();

      float minX = float.MaxValue, minY = float.MaxValue;
      float maxX = -float.MaxValue, maxY = -float.MaxValue;

      bool foundSelectedObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {

        if (box.Selected)
        {
          foundSelectedObject = true;

          if (box.BoundingRect.Left < minX) minX = box.BoundingRect.Left;

          if (box.BoundingRect.Top < minY) minY = box.BoundingRect.Top;

          if (box.BoundingRect.Right > maxX) maxX = box.BoundingRect.Right;

          if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
        }
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {

        if (arrow.Selected)
        {
          foundSelectedObject = true;

          if (arrow.BoundingRect.Left < minX) minX = arrow.BoundingRect.Left;

          if (arrow.BoundingRect.Top < minY) minY = arrow.BoundingRect.Top;

          if (arrow.BoundingRect.Right > maxX) maxX = arrow.BoundingRect.Right;

          if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
        }
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundSelectedObject)
        fcFlowChart.ZoomToRect(RectangleF.FromLTRB(minX - width * 0.05F, minY - height * 0.05F, maxX + width * 0.05F, maxY + height * 0.05F));

      else
        fcFlowChart.ZoomToRect(fcFlowChart.DocExtents);
    }

    public void ZoomToVisible()
    {
      FixDocExtents();

      float minX = float.MaxValue, minY = float.MaxValue;
      float maxX = -float.MaxValue, maxY = -float.MaxValue;

      bool foundVisibleObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {

        if (box.Visible)
        {
          foundVisibleObject = true;

          if (box.BoundingRect.Left < minX) minX = box.BoundingRect.Left;

          if (box.BoundingRect.Top < minY) minY = box.BoundingRect.Top;

          if (box.BoundingRect.Right > maxX) maxX = box.BoundingRect.Right;

          if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
        }
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {

        if (arrow.Visible)
        {
          foundVisibleObject = true;

          if (arrow.BoundingRect.Left < minX) minX = arrow.BoundingRect.Left;

          if (arrow.BoundingRect.Top < minY) minY = arrow.BoundingRect.Top;

          if (arrow.BoundingRect.Right > maxX) maxX = arrow.BoundingRect.Right;

          if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
        }
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundVisibleObject)
        fcFlowChart.ZoomToRect(RectangleF.FromLTRB(minX - width * 0.05F, minY - height * 0.05F, maxX + width * 0.05F, maxY + height * 0.05F));

      else
        fcFlowChart.ZoomToRect(fcFlowChart.DocExtents);

      SetSizes();

      foreach (Box box in fcFlowChart.Boxes)
      {

        if ((box.Visible) && (box.Tag is Thing))
        {
          //Thing thing = box.Tag as Thing;
          //box.Image = State.GetImage(thing.GraphicThing, fcFlowChart);
        }
      }
    }

    internal static SysCAD.Protocol.Point getCenter(SysCAD.Protocol.Rectangle rect)
    {
      return new SysCAD.Protocol.Point(rect.X + rect.Width / 2, rect.Y + rect.Height / 2);
    }

    internal static float getMinArrowheadSize(GraphicsUnit currUnit)
    {

      switch (currUnit)
      {

        case GraphicsUnit.Millimeter:
          return 1;

        case GraphicsUnit.Inch:
          return 1.0f / 12;

        case GraphicsUnit.Point:
          return 72.0f / 12;

        case GraphicsUnit.Pixel:
          return 4;

        case GraphicsUnit.Document:
          return 300.0f / 12;

        case GraphicsUnit.Display:
          return 75.0f / 12;
        default:
          return 0.0F;
      }
    }

    internal bool MergePoints(SysCAD.Protocol.Point point1, SysCAD.Protocol.Point point2)
    {
      return
        Math.Abs(point1.X - point2.X) <= fcFlowChart.MergeThreshold &&
        Math.Abs(point1.Y - point2.Y) <= fcFlowChart.MergeThreshold;
    }

    internal static SysCAD.Protocol.Point rotatePointAt(SysCAD.Protocol.Point point, SysCAD.Protocol.Point pivot, Double angle)
    {
      PointF[] points = new PointF[] { point.ToPointF() };
      Matrix rotation = new Matrix();
      rotation.RotateAt((float)angle, pivot.ToPointF());
      rotation.TransformPoints(points);
      rotation.Dispose();
      return new SysCAD.Protocol.Point(points[0]);
    }

    internal void SetProject(ClientProtocol clientProtocol, Config config, PureComponents.TreeView.TreeView tvNavigation)
    {
      state.ClientProtocol = clientProtocol;
      state.Config = config;
      state.TVNavigation = tvNavigation;

      state.ConnectGraphic(
        new ClientProtocol.PermissionsChangedHandler(fcFlowChart_PermissionsChanged),
        new ClientProtocol.StepHandler(fcFlowChart_Step),
        new ClientProtocol.SyncHandler(fcFlowChart_Sync),
        new ClientProtocol.GroupCreatedHandler(fcFlowChart_GroupCreated),
        new ClientProtocol.GroupModifiedHandler(fcFlowChart_GroupModified),
        new ClientProtocol.GroupDeletedHandler(fcFlowChart_GroupDeleted),
        new ClientProtocol.ItemCreatedHandler(fcFlowChart_ItemCreated),
        new ClientProtocol.PortInfoRequestedHandler(fcFlowChart_PortInfoRequested),
        new ClientProtocol.ItemModifiedHandler(fcFlowChart_ItemModified),
        new ClientProtocol.ItemDeletedHandler(fcFlowChart_ItemDeleted),
        new ClientProtocol.LinkCreatedHandler(fcFlowChart_LinkCreated),
        new ClientProtocol.LinkModifiedHandler(fcFlowChart_LinkModified),
        new ClientProtocol.LinkDeletedHandler(fcFlowChart_LinkDeleted),
        new ClientProtocol.ThingCreatedHandler(fcFlowChart_ThingCreated),
        new ClientProtocol.ThingModifiedHandler(fcFlowChart_ThingModified),
        new ClientProtocol.ThingDeletedHandler(fcFlowChart_ThingDeleted));

      fcFlowChart.UndoManager.UndoEnabled = false;
      fcFlowChart.UseWaitCursor = true;
      fcFlowChart.ValidityChecks = false;
      fcFlowChart.Visible = false;
      fcFlowChart.Enabled = false;



      foreach (GraphicGroup graphicGroup in clientProtocol.graphicGroups.Values)
      {
        state.CreateGroup(graphicGroup, false, fcFlowChart);
      }

      foreach (GraphicItem graphicItem in clientProtocol.graphicItems.Values)
      {
        state.CreateItem(graphicItem, false, fcFlowChart);
      }

      foreach (GraphicLink graphicLink in clientProtocol.graphicLinks.Values)
      {
        state.CreateLink(graphicLink, false, fcFlowChart);
      }

      foreach (GraphicThing graphicThing in clientProtocol.graphicThings.Values)
      {
        state.CreateThing(graphicThing, false, fcFlowChart);
      }

      fcFlowChart.UndoManager.UndoEnabled = true;
      fcFlowChart.UseWaitCursor = false;
      fcFlowChart.ValidityChecks = true;
      fcFlowChart.Visible = true;
      fcFlowChart.Enabled = true;
    }

    internal void UnSetProject()
    {
      state.DisconnectGraphic(
          fcFlowChart_PermissionsChanged,
          fcFlowChart_Step,
          fcFlowChart_Sync,
          fcFlowChart_GroupCreated,
          fcFlowChart_GroupModified,
          fcFlowChart_GroupDeleted,
          fcFlowChart_ItemCreated,
          fcFlowChart_PortInfoRequested,
          fcFlowChart_ItemModified,
          fcFlowChart_ItemDeleted,
          fcFlowChart_LinkCreated,
          fcFlowChart_LinkModified,
          fcFlowChart_LinkDeleted,
          fcFlowChart_ThingCreated,
          fcFlowChart_ThingModified,
          fcFlowChart_ThingDeleted);
    }

    private void DisconnectDestination(object sender, EventArgs e)
    {
      hoverArrow.DestAnchor = -1;
      hoverArrow.Destination = fcFlowChart.Dummy;

      State.SetControlPoints(hoverArrow, (hoverArrow.Tag as Link).GraphicLink.ControlPoints);
    }

    private void DisconnectOrigin(object sender, EventArgs e)
    {
      hoverArrow.OrgnAnchor = -1;
      hoverArrow.Origin = fcFlowChart.Dummy;

      State.SetControlPoints(hoverArrow, (hoverArrow.Tag as Link).GraphicLink.ControlPoints);
    }

    static private Double Distance(SysCAD.Protocol.Point a, SysCAD.Protocol.Point b)
    {
      return Math.Sqrt(((a.X - b.X) * (a.X - b.X)) + ((a.Y - b.Y) * (a.Y - b.Y)));
    }

    private void DoArrowModifyingOperations(Arrow arrow, int selectionHandle)
    {
      oldControlPoints = State.GetControlPoints(arrow.ControlPoints);

      arrowBeingModifiedSelectionHandle = selectionHandle;
      arrowBeingModified = arrow;
      arrowBeingModified.CustomDraw = CustomDraw.Additional;
      arrowBeingModified.ZTop();

      if (arrow.Tag != null)
        oldOriginGuid = (arrow.Tag as Link).GraphicLink.Origin;

      else
        oldOriginGuid = Guid.Empty;
      oldOriginBox = arrow.Origin as Box;
      oldOriginAnchor = arrow.OrgnAnchor;

      if (arrow.Tag != null)
        oldDestinationGuid = (arrow.Tag as Link).GraphicLink.Destination;

      else
        oldDestinationGuid = Guid.Empty;
      oldDestinationBox = arrow.Destination as Box;
      oldDestinationAnchor = arrow.DestAnchor;

      if (arrowBeingModifiedSelectionHandle == 0)
      {
        SysCAD.Protocol.Point originPos = new SysCAD.Protocol.Point(arrowBeingModified.ControlPoints[0]);
        Box originBox = fcFlowChart.GetBoxAt(originPos.ToPointF(), 2.0F);

        if ((selectionHandle == 0) && (originBox != null) && (!(arrowBeingModified.Origin is Box)))
        {
          originBox = (originBox.Tag as Item).Model;

          if (originBox != null)
          {
            int closestI = 0;
            Double closestDistance = Double.MaxValue;

            for (int i = 0; i < originBox.AnchorPattern.Points.Count; i++)
            {

              if (originBox.AnchorPattern.Points[i].AllowOutgoing)
              {
                SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(originBox.BoundingRect),
                  originBox.AnchorPattern.Points[i].X,
                  originBox.AnchorPattern.Points[i].Y,
                  originBox.RotationAngle);
                Double thisDistance = Distance(originPos, anchorPointPos);

                if (thisDistance < closestDistance)
                {
                  closestDistance = thisDistance;
                  closestI = i;
                }
              }
            }

            newOriginGuid = (originBox.Tag as Item).Guid;
            newOriginBox = originBox;
            newOriginAnchor = closestI;

            form1.ToolStripStatusLabel.Text = (originBox.Tag as Item).GraphicItem.anchorIntToTag[newOriginAnchor];
          }
        }
      }
      //else if (oldOriginBox != null)
      //{
      //  (e.Arrow.Tag as Link).graphicLink.Origin = oldOriginGuid;
      //  e.Arrow.Origin = oldOriginBox;
      //  e.Arrow.OrgnAnchor = oldOriginAnchor;
      //}

      if (selectionHandle == arrowBeingModified.ControlPoints.Count - 1)
      {
        SysCAD.Protocol.Point destinationPos = new SysCAD.Protocol.Point(arrowBeingModified.ControlPoints[arrowBeingModified.ControlPoints.Count - 1]);
        Box destinationBox = fcFlowChart.GetBoxAt(destinationPos.ToPointF(), 2.0F);

        if ((destinationBox != null) && (!(arrowBeingModified.Destination is Box)))
        {
          destinationBox = (destinationBox.Tag as Item).Model;

          if (destinationBox != null)
          {
            int closestI = 0;
            Double closestDistance = Double.MaxValue;

            for (int i = 0; i < destinationBox.AnchorPattern.Points.Count; i++)
            {

              if (destinationBox.AnchorPattern.Points[i].AllowIncoming)
              {
                SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(destinationBox.BoundingRect),
                  destinationBox.AnchorPattern.Points[i].X,
                  destinationBox.AnchorPattern.Points[i].Y,
                  destinationBox.RotationAngle);
                Double thisDistance = Distance(destinationPos, anchorPointPos);

                if (thisDistance < closestDistance)
                {
                  closestDistance = thisDistance;
                  closestI = i;
                }
              }
            }

            newDestinationGuid = (destinationBox.Tag as Item).Guid;
            newDestinationBox = destinationBox;
            newDestinationAnchor = closestI;

            form1.ToolStripStatusLabel.Text = (newDestinationBox.Tag as Item).GraphicItem.anchorIntToTag[newDestinationAnchor];
          }
        }
      }

      if (newOriginBox == newDestinationBox) // refliexive, disconnect modifying end.
      {

        if (selectionHandle == 0)
        {
          newOriginGuid = Guid.Empty;
          newOriginBox = null;
          newOriginAnchor = -1;

          form1.ToolStripStatusLabel.Text = "";
        }

        if (selectionHandle == arrowBeingModified.ControlPoints.Count - 1)
        {
          newDestinationGuid = Guid.Empty;
          newDestinationBox = null;
          newDestinationAnchor = -1;

          form1.ToolStripStatusLabel.Text = "";
        }
      }

      fcFlowChart.RecreateCacheImage();
    }

    private void EditThing(object sender, EventArgs e)
    {
      Thing thing = (hoverBox.Tag as Thing);
      GraphicThing graphicThing = thing.GraphicThing;

      //ThingEditor.ThingEditorForm thingEditor = new ThingEditor.ThingEditorForm(graphicThing);
      //thingEditor.ShowDialog();
      //graphicThing = thingEditor.graphicThing;

      throw new NotImplementedException("The method or operation is not implemented.");

      Int64 requestId;

      if (state.ModifyGraphicThing(out requestId,
        graphicThing.Guid,
        graphicThing.Tag,
        graphicThing.Path,
        graphicThing.BoundingRect,
        graphicThing.Xaml,
        graphicThing.Angle,
        graphicThing.MirrorX,
        graphicThing.MirrorY))
      {
        //hoverBox.Image = State.GetImage(graphicThing, fcFlowChart);
      }

      form1.GraphicPropertyGrid.Refresh();

      ContextMenu propertyGridMenu = form1.GraphicPropertyGrid.ContextMenu;

      if (propertyGridMenu == null)
        propertyGridMenu = new ContextMenu();

      propertyGridMenu.MenuItems.Add("Test");

      form1.GraphicPropertyGrid.ContextMenu = propertyGridMenu;
    }

    private void fcFlowChart_ArrowCreated(object sender, ArrowEventArgs e)
    {
      tempArrowKey++;
      String newLinkTag = "A_" + tempArrowKey.ToString(CultureInfo.InvariantCulture);
      e.Arrow.Text = newLinkTag;

      GraphicLink newGraphicLink = new GraphicLink(newLinkTag);

      newGraphicLink.ClassID = "Pipe-1";

      if (newDestinationBox != null)
      {
        newGraphicLink.Destination = (newDestinationBox.Tag as Item).Guid;

        if (newDestinationAnchor == -1)
          newGraphicLink.DestinationPort = "";

        else
          newGraphicLink.DestinationPort = (newDestinationBox.Tag as Item).GraphicItem.anchorIntToTag[newDestinationAnchor];
      }

      if (oldOriginBox != null)
      {
        newGraphicLink.Origin = (oldOriginBox.Tag as Item).Guid;

        if (oldOriginAnchor == -1)
          newGraphicLink.OriginPort = "";

        else
          newGraphicLink.OriginPort = (oldOriginBox.Tag as Item).GraphicItem.anchorIntToTag[oldOriginAnchor];
      }

      newGraphicLink.ControlPoints = State.GetControlPoints(e.Arrow.ControlPoints);

      fcFlowChart.DeleteObject(e.Arrow);

      form1.ToolStripStatusLabel.Text = "";

      arrowBeingModified.CustomDraw = CustomDraw.None;
      arrowBeingModifiedSelectionHandle = -1;
      arrowBeingModified = null;
      originAnchorChosen = null;
      destinationAnchorChosen = null;

      state.CreateLink(newGraphicLink, true, fcFlowChart);
    }

    private void fcFlowChart_ArrowCreating(object sender, AttachConfirmArgs e)
    {
      SysCAD.Protocol.Point originPos = new SysCAD.Protocol.Point(e.Arrow.ControlPoints[0]);
      Box originBox = fcFlowChart.GetBoxAt(originPos.ToPointF(), 2.0F);

      originBox = (originBox.Tag as Item).Model;

      if (originBox != null)
      {
        int closestI = 0;
        Double closestDistance = Double.MaxValue;

        for (int i = 0; i < originBox.AnchorPattern.Points.Count; i++)
        {

          if (originBox.AnchorPattern.Points[i].AllowOutgoing)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(originBox.BoundingRect),
              originBox.AnchorPattern.Points[i].X,
              originBox.AnchorPattern.Points[i].Y,
              originBox.RotationAngle);

            Double thisDistance = Distance(originPos, anchorPointPos);

            if (thisDistance < closestDistance)
            {
              closestDistance = thisDistance;
              closestI = i;
            }
          }
        }

        newOriginGuid = (originBox.Tag as Item).Guid;
        newOriginBox = originBox;

        newOriginAnchor = closestI;
      }

      DoArrowModifyingOperations(e.Arrow, e.Arrow.ControlPoints.Count - 1);
      //arrowBeingModified = e.Arrow;
      //arrowBeingModified.CustomDraw = CustomDraw.Additional;
      //arrowBeingModified.ZTop();
      //fcFlowChart.RecreateCacheImage();
    }

    private void fcFlowChart_ArrowDeleting(object sender, ArrowConfirmArgs e)
    {
      e.Confirm = false;
      MessageBox.Show("Not Implemented");
    }

    private void fcFlowChart_ArrowModified(object sender, ArrowMouseArgs e)
    {
      {
        int i = 2;

        while ((oldControlPoints.Count > 3) && (i < oldControlPoints.Count - 1))
        {

          if (MergePoints(oldControlPoints[i - 1], oldControlPoints[i]))
          {
            oldControlPoints.RemoveAt(i - 1);
            oldControlPoints.RemoveAt(i - 1);
            i = 2;
          }

          else
          {
            i++;
          }
        }
      }

      if (MergePoints(oldControlPoints[0], oldControlPoints[1]))
      {
        oldControlPoints.RemoveAt(0);

        if (e.Arrow.CascadeOrientation == MindFusion.FlowChartX.Orientation.Horizontal)
          e.Arrow.CascadeOrientation = MindFusion.FlowChartX.Orientation.Vertical;

        else
          e.Arrow.CascadeOrientation = MindFusion.FlowChartX.Orientation.Horizontal;
      }

      if (MergePoints(oldControlPoints[oldControlPoints.Count - 1], oldControlPoints[oldControlPoints.Count - 2]))
      {
        oldControlPoints.RemoveAt(oldControlPoints.Count - 1);
      }

      form1.ToolStripStatusLabel.Text = "";

      arrowBeingModified.CustomDraw = CustomDraw.None;
      arrowBeingModifiedSelectionHandle = -1;
      arrowBeingModified = null;
      originAnchorChosen = null;
      destinationAnchorChosen = null;

      GraphicLink graphicLink = (e.Arrow.Tag as Link).GraphicLink as GraphicLink;

      if (oldOriginBox != null)
      {
        (e.Arrow.Tag as Link).GraphicLink.Origin = oldOriginGuid;
        e.Arrow.Origin = oldOriginBox;
        e.Arrow.OrgnAnchor = oldOriginAnchor;
      }

      else if (newOriginBox != null)
      {
        (e.Arrow.Tag as Link).GraphicLink.Origin = newOriginGuid;
        e.Arrow.Origin = newOriginBox;
        e.Arrow.OrgnAnchor = newOriginAnchor;
      }

      if (oldDestinationBox != null)
      {
        (e.Arrow.Tag as Link).GraphicLink.Destination = oldDestinationGuid;
        e.Arrow.Destination = oldDestinationBox;
        e.Arrow.DestAnchor = oldDestinationAnchor;
      }

      else if (newDestinationBox != null)
      {
        (e.Arrow.Tag as Link).GraphicLink.Destination = newDestinationGuid;
        e.Arrow.Destination = newDestinationBox;
        e.Arrow.DestAnchor = newDestinationAnchor;
      }

      State.SetControlPoints(e.Arrow, oldControlPoints);

      Item originItem = e.Arrow.Origin.Tag as Item;
      Item destinationItem = e.Arrow.Destination.Tag as Item;

      GraphicItem originGraphicItem = null;
      GraphicItem destinationGraphicItem = null;
      Guid originGuid = Guid.Empty;
      Guid destinationGuid = Guid.Empty;

      if (originItem != null)
      {
        originGraphicItem = originItem.GraphicItem;
        originGuid = originItem.Guid;
      }

      if (destinationItem != null)
      {
        destinationGraphicItem = destinationItem.GraphicItem;
        destinationGuid = destinationItem.Guid;
      }

      String originAnchor = null;
      String destinationAnchor = null;

      if (originGraphicItem != null)
        originGraphicItem.anchorIntToTag.TryGetValue(e.Arrow.OrgnAnchor, out originAnchor);

      if (destinationGraphicItem != null)
        destinationGraphicItem.anchorIntToTag.TryGetValue(e.Arrow.DestAnchor, out destinationAnchor);

      Int64 requestId;

      if (!state.ModifyGraphicLink(out requestId,
        graphicLink.Guid,
        graphicLink.Tag,
        graphicLink.ClassID,
        originGuid,
        destinationGuid,
        originAnchor,
        destinationAnchor,
        oldControlPoints,
        graphicLink.TextArea,
        graphicLink.TextAngle))
      { // failure, revert back to previous.
        // do something here...
      }

      oldOriginGuid = Guid.Empty;
      oldOriginBox = null;
      oldOriginAnchor = -1;

      oldDestinationGuid = Guid.Empty;
      oldDestinationBox = null;
      oldDestinationAnchor = -1;

      newOriginGuid = Guid.Empty;
      newOriginBox = null;
      newOriginAnchor = -1;

      newDestinationGuid = Guid.Empty;
      newDestinationBox = null;
      newDestinationAnchor = -1;
    }

    private void fcFlowChart_ArrowModifying(object sender, ArrowConfirmArgs e)
    {

      form1.ModeModify();

      if (fcFlowChart.Selection.Arrows.Count == 1) // We're playing with just one arrow...
      {
        DoArrowModifyingOperations(e.Arrow, e.SelectionHandle);
      }
    }

    private void fcFlowChart_BoxDeleting(object sender, BoxConfirmArgs e)
    {
      e.Confirm = false; // Tell flowchart not to deal with this.
      DeleteSelection();
    }

    private void fcFlowChart_Click(object sender, EventArgs e)
    {
      Guid guid;
      MouseEventArgs me = e as MouseEventArgs;
      mousePressed = me.Location;
      hoverArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(me.Location), 2);
      hoverBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(me.Location), 2.0F);

      if (
          (me.Button == MouseButtons.Left)
          &&
          ((hoverBox == null) && (hoverArrow == null)) // we're in free space...
          &&
          ((form1.BarManager1.Commands["Mode.CreateNode"] as BarButtonCommand).Checked == true)
          &&
          ((currentModel != null) && (currentStencil != null))
          )
      {
        state.newElementSelectionList.Clear();
        NewGraphicItem(
          out guid,
          state.CurrentPath,
          currentModel,
          currentStencil,
          new SysCAD.Protocol.Rectangle(fcFlowChart.ClientToDoc(me.Location), state.GraphicShape(currentStencil).defaultSize),
          0.0F,
          state.GraphicShape(currentStencil).TextArea, // ???? I think this needs to be converted from relative (stencil) to absolute (graphicItem)
          0.0F,
          Color.LightBlue,
          FillMode.Alternate,
          false,
          false
          );
      }

      else
      {

        form1.ModeModify();
      }

      if (me.Button == MouseButtons.Right)
      {
        ContextMenu theMenu = new ContextMenu();

        if (hoverBox != null)
        {

          if (hoverBox.Tag is Item)
          {
            theMenu.MenuItems.Add("Route Links", new EventHandler(RouteLinks));
            theMenu.MenuItems.Add("Raise to Top", new EventHandler(RaiseItemToTop));
            theMenu.MenuItems.Add("Send to Bottom", new EventHandler(SendItemToBottom));

            form1.ModeModify();
          }

          else if (hoverBox.Tag is Thing)
          {
            theMenu.MenuItems.Add("Edit Thing", new EventHandler(EditThing));
            theMenu.MenuItems.Add("Raise to Top", new EventHandler(RaiseThingToTop));
            theMenu.MenuItems.Add("Send to Bottom", new EventHandler(SendThingToBottom));

            form1.ModeModify();
          }
        }

        else if (hoverArrow != null)
        {
          theMenu.MenuItems.Add("Route Link", new EventHandler(RouteLink));
          theMenu.MenuItems.Add("Disconnect Origin", new EventHandler(DisconnectOrigin));
          theMenu.MenuItems.Add("Disconnect Destination", new EventHandler(DisconnectDestination));

          form1.ModeModify();
        }

        theMenu.MenuItems.Add("Layout Flowchart (!!Broken in an interesting way in latest release!!)", new EventHandler(LayoutFlowchart));
        theMenu.MenuItems.Add("Insert Annotation", new EventHandler(InsertAnnotation));

        theMenu.Show(fcFlowChart, me.Location);
      }
    }

    private void fcFlowChart_DrawArrow(object sender, ArrowDrawArgs e)
    {
      Arrow arrow = e.Arrow;

      if (arrow != arrowBeingModified)
      {

        if (arrowBeingModified == null)
        {
          {
            MindFusion.FlowChartX.Node node = arrow.Destination;
            Box box = null;

            if (node != null)
              box = node as Box;

            if ((box != null) && (arrow.DestAnchor != -1))
            {

              PointF anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(box.BoundingRect),
                box.AnchorPattern.Points[arrow.DestAnchor].X,
                box.AnchorPattern.Points[arrow.DestAnchor].Y,
                box.RotationAngle).ToPointF();

              PointF[] extensionPoints =
                new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos };

              System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);
              e.Graphics.DrawLines(pen, extensionPoints);

              pen = new System.Drawing.Pen(Color.Green, 0.0F);
              SysCAD.Protocol.Rectangle anchorPointRect = new SysCAD.Protocol.Rectangle(anchorPointPos, SysCAD.Protocol.Size.Empty);
              anchorPointRect.Inflate((double)fcFlowChart.SelHandleSize, (double)fcFlowChart.SelHandleSize);
              e.Graphics.DrawEllipse(pen, anchorPointRect);
            }

            else
            {
              Color errorColor = new Color();

              if (box != null) errorColor = Color.Yellow;

              else errorColor = Color.Red;

              System.Drawing.Pen pen = new System.Drawing.Pen(errorColor, fcFlowChart.SelHandleSize);
              PointF controlPoint = arrow.ControlPoints[arrow.ControlPoints.Count - 1];
              SysCAD.Protocol.Rectangle controlPointRect = new SysCAD.Protocol.Rectangle(controlPoint, SysCAD.Protocol.Size.Empty);
              controlPointRect.Inflate(fcFlowChart.SelHandleSize, fcFlowChart.SelHandleSize);
              e.Graphics.DrawEllipse(pen, controlPointRect);
            }
          }

          {
            MindFusion.FlowChartX.Node node = arrow.Origin;
            Box box = null;

            if (node != null)
              box = node as Box;

            if ((box != null) && (arrow.OrgnAnchor != -1))
            {
              PointF anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(box.BoundingRect),
                box.AnchorPattern.Points[arrow.OrgnAnchor].X,
                box.AnchorPattern.Points[arrow.OrgnAnchor].Y,
                box.RotationAngle).ToPointF();

              PointF[] extensionPoints =
                new PointF[] { arrow.ControlPoints[0], anchorPointPos };

              System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);
              e.Graphics.DrawLines(pen, extensionPoints);

              pen = new System.Drawing.Pen(Color.Green, 0.0F);
              SysCAD.Protocol.Rectangle r = new SysCAD.Protocol.Rectangle(anchorPointPos, SysCAD.Protocol.Size.Empty);
              r.Inflate(fcFlowChart.SelHandleSize, fcFlowChart.SelHandleSize);
              e.Graphics.DrawEllipse(pen, r);
            }

            else
            {
              Color errorColor = new Color();

              if (box != null) errorColor = Color.Yellow;

              else errorColor = Color.Red;

              System.Drawing.Pen pen = new System.Drawing.Pen(errorColor, fcFlowChart.SelHandleSize);
              PointF p = arrow.ControlPoints[0];
              SysCAD.Protocol.Rectangle r = new SysCAD.Protocol.Rectangle(p, SysCAD.Protocol.Size.Empty);
              r.Inflate(fcFlowChart.SelHandleSize, fcFlowChart.SelHandleSize);
              e.Graphics.DrawEllipse(pen, r);
            }
          }
        }
      }

      else // this is an arrowbeingmodified...
      {
        {

          if (newDestinationAnchor != -1)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(newDestinationBox.BoundingRect),
              newDestinationBox.AnchorPattern.Points[newDestinationAnchor].X,
              newDestinationBox.AnchorPattern.Points[newDestinationAnchor].Y,
              newDestinationBox.RotationAngle);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos.ToPointF() };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.LightBlue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }

          else if (oldDestinationAnchor != -1)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(oldDestinationBox.BoundingRect),
              oldDestinationBox.AnchorPattern.Points[oldDestinationAnchor].X,
              oldDestinationBox.AnchorPattern.Points[oldDestinationAnchor].Y,
              oldDestinationBox.RotationAngle);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos.ToPointF() };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
        }

        {

          if (newOriginAnchor != -1)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(newOriginBox.BoundingRect),
              newOriginBox.AnchorPattern.Points[newOriginAnchor].X,
              newOriginBox.AnchorPattern.Points[newOriginAnchor].Y,
              newOriginBox.RotationAngle);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[0], anchorPointPos.ToPointF() };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.LightBlue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }

          else if (oldOriginAnchor != -1)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(oldOriginBox.BoundingRect),
              oldOriginBox.AnchorPattern.Points[oldOriginAnchor].X,
              oldOriginBox.AnchorPattern.Points[oldOriginAnchor].Y,
              oldOriginBox.RotationAngle);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[0], anchorPointPos.ToPointF() };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
        }
      }
    }

    private void fcFlowChart_DrawBox(object sender, BoxDrawArgs e)
    {

      if (arrowBeingModified != null)
      {
        SysCAD.Protocol.Point originPos = new SysCAD.Protocol.Point(arrowBeingModified.ControlPoints[0]);
        Box originBox = fcFlowChart.GetBoxAt(originPos.ToPointF(), 2.0F);

        if (originBox != null)
        {
          originBox = (originBox.Tag as Item).Model;

          if (originBox != null)
          {

            if (originBox.AnchorPattern != null)
            {
              //if (originAnchorChosen == null)
              {
                Double closest = Double.MaxValue;

                foreach (AnchorPoint anchorPoint in originBox.AnchorPattern.Points)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(originBox.BoundingRect),
                    anchorPoint.X,
                    anchorPoint.Y,
                    originBox.RotationAngle);

                  Double distance = Distance(anchorPointPos, originPos);

                  if (distance < closest)
                  {
                    closest = distance;
                    originAnchorChosen = anchorPoint.Tag as Anchor;
                  }
                }

                Int64 requestId;

               // if (state.PortCheck(out requestId, (originBox.Tag as Item).Guid, originAnchorChosen) == PortStatus.Available)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(originBox.BoundingRect),
                    (originAnchorChosen.Positions[0] as SysCAD.Protocol.Point).X,
                    (originAnchorChosen.Positions[0] as SysCAD.Protocol.Point).Y,
                    originBox.RotationAngle);

                  PointF[] extensionPoints =
                    new PointF[] { anchorPointPos.ToPointF(), anchorPointPos.ToPointF() };
                  System.Drawing.Pen pen = new System.Drawing.Pen(Color.Yellow, fcFlowChart.SelHandleSize);
                  e.Graphics.DrawEllipse(pen, RectangleF.FromLTRB(
                    anchorPointPos.ToPointF().X - fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().Y - fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().X + fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().Y + fcFlowChart.SelHandleSize));
                  e.Graphics.DrawLines(pen, extensionPoints);
                }
              }
            }
          }
        }

        SysCAD.Protocol.Point destinationPos = new SysCAD.Protocol.Point(arrowBeingModified.ControlPoints[arrowBeingModified.ControlPoints.Count - 1]);
        Box destinationBox = fcFlowChart.GetBoxAt(destinationPos.ToPointF(), 2.0F);

        if (destinationBox != null)
        {
          destinationBox = (destinationBox.Tag as Item).Model;

          if (destinationBox != null)
          {

            if (destinationBox.AnchorPattern != null)
            {
              //if (destinationAnchorChosen == null)
              {
                Double closest = Double.MaxValue;

                foreach (AnchorPoint anchorPoint in destinationBox.AnchorPattern.Points)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(destinationBox.BoundingRect),
                    anchorPoint.X,
                    anchorPoint.Y,
                    destinationBox.RotationAngle);

                  Double distance = Distance(anchorPointPos, destinationPos);

                  if (distance < closest)
                  {
                    closest = distance;
                    destinationAnchorChosen = anchorPoint.Tag as Anchor;
                  }
                }

                Int64 requestId;

                //if (state.PortCheck(out requestId, (destinationBox.Tag as Item).Guid, destinationAnchorChosen) == PortStatus.Available)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(destinationBox.BoundingRect),
                    (float)((originAnchorChosen.Positions[0] as SysCAD.Protocol.Point).X),
                    (float)((originAnchorChosen.Positions[0] as SysCAD.Protocol.Point).Y),
                    destinationBox.RotationAngle);

                  PointF[] extensionPoints =
                    new PointF[] { anchorPointPos.ToPointF(), anchorPointPos.ToPointF() };
                  System.Drawing.Pen pen = new System.Drawing.Pen(Color.Yellow, fcFlowChart.SelHandleSize);
                  e.Graphics.DrawEllipse(pen, RectangleF.FromLTRB(
                    anchorPointPos.ToPointF().X - fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().Y - fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().X + fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().Y + fcFlowChart.SelHandleSize));
                  e.Graphics.DrawLines(pen, extensionPoints);
                }

              }
            }
          }
        }
      }
    }

    private void fcFlowChart_GroupCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect)
    {
      state.CreateGroup(state.GraphicGroup(guid), true, fcFlowChart);
    }

    private void fcFlowChart_GroupDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    private void fcFlowChart_GroupModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    private void fcFlowChart_ItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape shape, SysCAD.Protocol.Rectangle boundingRect, Double angle, SysCAD.Protocol.Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      state.CreateItem(state.GraphicItem(guid), state.TVNavigation.GetNodeByPath(path).IsSelected, fcFlowChart);
    }

    private void fcFlowChart_ItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      state.DeleteItem(guid, fcFlowChart);
    }

    private void fcFlowChart_PortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    private void fcFlowChart_ItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape shape, SysCAD.Protocol.Rectangle boundingRect, Double angle, SysCAD.Protocol.Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      Item item = state.Item(guid);

      if (item != null)
      {
        item.Model.BoundingRect = boundingRect;
        item.Model.RotationAngle = (float)angle;

        ModelStencil modelShape = state.ModelShape(model);

        if (modelShape != null)
          item.Model.Shape = State.GetShapeTemplate(modelShape, mirrorX, mirrorY);
        else
          item.Model.Shape = ShapeTemplate.FromId("Decision2");

        item.Model.AnchorPattern = State.GetAnchorPattern(modelShape, item.GraphicItem);

        item.Graphic.BoundingRect = boundingRect;
        item.Graphic.RotationAngle = (float)angle;
        item.Graphic.FillColor = fillColor;

        //item.Graphic.Shape = shape.ShapeTemplate(item.MirrorX, iItem.MirrorY);
        {
          GraphicStencil graphicStencil = state.GraphicShape(shape, model);

          if (graphicStencil != null)
            item.Graphic.Shape = State.GetShapeTemplate(graphicStencil, mirrorX, mirrorY);
          else
            item.Graphic.Shape = ShapeTemplate.FromId("Decision2");
        }
      }
    }

    private void fcFlowChart_LinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<SysCAD.Protocol.Point> controlPoints, SysCAD.Protocol.Rectangle textArea, Double textAngle)
    {
      state.CreateLink(state.GraphicLink(guid), true, fcFlowChart);
    }

    private void fcFlowChart_LinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      // TBD
    }

    private void fcFlowChart_LinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<SysCAD.Protocol.Point> controlPoints, SysCAD.Protocol.Rectangle textArea, Double textAngle)
    {
      Link link = state.Link(guid);

      if (link != null)
      {
        GraphicLink graphicLink = link.GraphicLink;
        graphicLink.ClassID = classId;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        graphicLink.OriginPort = originPort;
        graphicLink.DestinationPort = destinationPort;
        graphicLink.ControlPoints = controlPoints;

        graphicLink.TextArea = textArea;
        graphicLink.TextAngle = textAngle;

        Arrow arrow = link.Arrow;
        Item originItem = null;
        Item destinationItem = null;

        if (graphicLink.Origin != Guid.Empty) originItem = state.Item(graphicLink.Origin);

        if (graphicLink.Destination != Guid.Empty) destinationItem = state.Item(graphicLink.Destination);

        if (originItem != null)
          arrow.Origin = originItem.Model;

        if (destinationItem != null)
          arrow.Destination = destinationItem.Model;

        if ((graphicLink.OriginPort != null) && ((originItem.Model.Tag as Item).GraphicItem.anchorTagToInt.ContainsKey(graphicLink.OriginPort)))
          arrow.OrgnAnchor = (originItem.Model.Tag as Item).GraphicItem.anchorTagToInt[graphicLink.OriginPort];

        else
          arrow.OrgnAnchor = -1;

        if ((graphicLink.DestinationPort != null) && ((destinationItem.Model.Tag as Item).GraphicItem.anchorTagToInt.ContainsKey(graphicLink.DestinationPort)))
          arrow.DestAnchor = (destinationItem.Model.Tag as Item).GraphicItem.anchorTagToInt[graphicLink.DestinationPort];

        else
          arrow.DestAnchor = -1;

        String toolTipOriginPort = graphicLink.OriginPort;
        String toolTipDestinationPort = graphicLink.DestinationPort;

        if (toolTipOriginPort == null) toolTipOriginPort = "*";

        if (toolTipDestinationPort == null) toolTipDestinationPort = "*";

        String originItemTag = "--";

        if (originItem != null)
          originItemTag = originItem.Tag;

        String destinationItemTag = "--";

        if (destinationItem != null)
          destinationItemTag = destinationItem.Tag;

        arrow.ToolTip = "Tag:" + graphicLink.Tag +
          "\nSrc: " + originItemTag + ":" + toolTipOriginPort +
          "\nDst: " + destinationItemTag + ":" + toolTipDestinationPort;
        arrow.ArrowHead = ArrowHead.Triangle;
        arrow.Style = ArrowStyle.Cascading;

        if (graphicLink.ControlPoints != null && graphicLink.ControlPoints.Count > 1)
        {
          State.SetControlPoints(arrow, graphicLink.ControlPoints);
        }
      }
    }

    private void fcFlowChart_MouseMove(object sender, MouseEventArgs e)
    {
      hoverArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)), 2.0F);

      hoverBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)), 2.0F);

      if ((hoverBox != null) && (hoverBox.Tag is Item))
        hoverBox = (hoverBox.Tag as Item).Model;

      if (e.Button == MouseButtons.Left)
      {

        if ((hoverBox != null) || (hoverArrow != null)) // we're not in free space...
        {

          if ((form1.BarManager1.Commands["Mode.CreateNode"] as BarButtonCommand).Checked == true)

            form1.ModeModify();
        }
      }

      if (hoverArrow != null)
      {
        hoverArrow.CustomDraw = CustomDraw.Additional;
        hoverArrow.ZTop();

        if (hoverArrow.Destination is Box)
        {
          Item hoverItem = hoverArrow.Destination.Tag as Item;
          hoverItem.Model.ZIndex = hoverArrow.ZIndex - 100000;
          hoverItem.Graphic.ZIndex = hoverArrow.ZIndex - 200000;
          hoverItem.Text.ZIndex = hoverArrow.ZIndex - 300000;
        }

        if (hoverArrow.Origin is Box)
        {
          Item hoverItem = hoverArrow.Origin.Tag as Item;
          hoverItem.Model.ZIndex = hoverArrow.ZIndex - 100000;
          hoverItem.Graphic.ZIndex = hoverArrow.ZIndex - 200000;
          hoverItem.Text.ZIndex = hoverArrow.ZIndex - 300000;
        }

        hoverArrow.Visible = state.ShowLinks;
      }

      if ((hoverBox != null) && (hoverBox.Tag is Item))
      {
        Item hoverItem = hoverBox.Tag as Item;
        hoverItem.Graphic.Visible = hoverItem.Visible;
        //hoverItem.Graphic.ZTop();
        hoverItem.Model.Visible = hoverItem.Visible;
        hoverItem.Model.CustomDraw = CustomDraw.Additional;
        hoverItem.Model.ZIndex = hoverItem.Graphic.ZIndex + 100000;
        hoverItem.Text.ZIndex = hoverItem.Graphic.ZIndex - 100000;
        hoverItem.Text.Visible = hoverItem.Visible && state.ShowTags;

        foreach (Arrow arrow in hoverItem.IncomingArrows)
        {
          arrow.Visible = hoverItem.Visible;
          arrow.CustomDraw = CustomDraw.Additional;
          arrow.ZIndex = hoverItem.Text.ZIndex - 100000;
        }

        foreach (Arrow arrow in hoverItem.OutgoingArrows)
        {
          arrow.Visible = hoverItem.Visible;
          arrow.CustomDraw = CustomDraw.Additional;
          arrow.ZIndex = hoverItem.Text.ZIndex - 100000;
        }
      }

      if (oldHoverArrow != null)
      {

        if (oldHoverArrow != hoverArrow) // we've moved on, un-hover the old one.
        {
          oldHoverArrow.CustomDraw = CustomDraw.None;

          if (oldHoverArrow.Destination is Box)
          {
            Item oldHoverItem = oldHoverArrow.Destination.Tag as Item;
            oldHoverItem.Model.ZIndex = oldHoverArrow.ZIndex - 100000;
            oldHoverItem.Graphic.ZIndex = oldHoverArrow.ZIndex - 200000;
            oldHoverItem.Text.ZIndex = oldHoverArrow.ZIndex - 300000;
          }

          if (oldHoverArrow.Origin is Box)
          {
            Item oldHoverItem = oldHoverArrow.Origin.Tag as Item;
            oldHoverItem.Model.ZIndex = oldHoverArrow.ZIndex - 100000;
            oldHoverItem.Graphic.ZIndex = oldHoverArrow.ZIndex - 200000;
            oldHoverItem.Text.ZIndex = oldHoverArrow.ZIndex - 300000;
          }

          oldHoverArrow.Visible = true;
        }
      }

      if (oldHoverBox != null) // deal with old itemBox.
      {

        if (oldHoverBox != hoverBox) // we've moved on, un-hover the old one.
        {

          if (oldHoverBox.Tag is Item)
          {
            Item oldHoverItem = oldHoverBox.Tag as Item;
            oldHoverItem.Graphic.Visible = oldHoverItem.Visible && state.ShowGraphics;
            oldHoverItem.Model.Visible = oldHoverItem.Visible && (oldHoverItem.Model.Selected || state.ShowModels);
            oldHoverItem.Model.CustomDraw = CustomDraw.None;
            oldHoverItem.Model.ZIndex = oldHoverItem.Graphic.ZIndex + 100000;
            oldHoverItem.Text.ZIndex = oldHoverItem.Graphic.ZIndex - 100000;
            oldHoverItem.Text.Visible = oldHoverItem.Visible && state.ShowTags;

            foreach (Arrow arrow in oldHoverItem.IncomingArrows)
            {
              arrow.Visible = oldHoverItem.Visible && state.ShowLinks;
              arrow.CustomDraw = CustomDraw.None;
              arrow.ZIndex = oldHoverItem.Text.ZIndex - 100000;
            }

            foreach (Arrow arrow in oldHoverItem.OutgoingArrows)
            {
              arrow.Visible = oldHoverItem.Visible && state.ShowLinks;
              arrow.CustomDraw = CustomDraw.None;
              arrow.ZIndex = oldHoverItem.Text.ZIndex - 100000;
            }
          }

          else if (oldHoverBox.Tag is Thing)
          {
            // Unhover the old thing.
          }
        }
      }

      oldHoverArrow = hoverArrow;
      oldHoverBox = hoverBox;
    }

    void fcFlowChart_MouseWheel(object sender, MouseEventArgs args)
    {

      if (arrowBeingModified != null)
      {

        if (newOriginAnchor != -1)
        {

          if (arrowBeingModifiedSelectionHandle == 0)
          {

            if (args.Delta > 0)
              newOriginAnchor++;

            else
              newOriginAnchor--;

            if (newOriginAnchor >= newOriginBox.AnchorPattern.Points.Count)
              newOriginAnchor = 0;

            if (newOriginAnchor < 0)
              newOriginAnchor = newOriginBox.AnchorPattern.Points.Count - 1;

            form1.ToolStripStatusLabel.Text =
              (newOriginBox.Tag as Item).GraphicItem.anchorIntToTag[newOriginAnchor];
          }
        }

        if (newDestinationAnchor != -1)
        {

          if (arrowBeingModifiedSelectionHandle > 0)
          {

            if (args.Delta > 0)
              newDestinationAnchor++;

            else
              newDestinationAnchor--;

            if (newDestinationAnchor >= newDestinationBox.AnchorPattern.Points.Count)
              newDestinationAnchor = 0;

            if (newDestinationAnchor < 0)
              newDestinationAnchor = newDestinationBox.AnchorPattern.Points.Count - 1;

            form1.ToolStripStatusLabel.Text =
              (newDestinationBox.Tag as Item).GraphicItem.anchorIntToTag[newDestinationAnchor];
          }
          refreshConnectedObjects(arrowBeingModified);
        }
      }

      else
      {
        FlowChart fcSender = sender as FlowChart;

        float newScrollY = fcSender.ScrollY - args.Delta / 5;

        if (newScrollY > fcSender.DocExtents.Top)
          fcSender.ScrollY = newScrollY;
      }
    }

    private void fcFlowChart_PermissionsChanged(Int64 eventId, Int64 requestId, ClientBaseProtocol.Permissions permissions)
    {
      state.PermissionsChanged(permissions);

      form1.PermissionsChanged(permissions);
    }

    private void fcFlowChart_Step(Int64 eventId, Int64 step, DateTime time)
    {
      state.Step(step, time);
    }

    private void fcFlowChart_Sync(Int64 eventId)
    {

      form1.LoadProject(state.ClientProtocol, state.Config);
    }

    private void fcFlowChart_ThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      state.CreateThing(state.GraphicThing(guid), true, fcFlowChart);
    }

    private void fcFlowChart_ThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      state.DeleteThing(guid, fcFlowChart);
    }

    private void fcFlowChart_ThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      Thing thing = state.Thing(guid);

      if (thing != null)
      {
        thing.Box.BoundingRect = boundingRect;
        thing.Box.RotationAngle = (float)angle;
      }
    }

    private void fcFlowChartBoxModified(object sender, BoxMouseArgs e)
    {

      if (e.Box.Tag is Item)
      {
        GraphicItem graphicItem = state.GraphicItem((e.Box.Tag as Item).Guid);
        Box modelBox = (e.Box.Tag as Item).Model;
        Box textBox = (e.Box.Tag as Item).Text;

        Int64 requestId;

        if (!state.ModifyGraphicItem(out requestId,
          graphicItem.Guid,
          graphicItem.Tag,
          graphicItem.Path,
          graphicItem.Model,
          graphicItem.Shape,
          new SysCAD.Protocol.Rectangle(modelBox.BoundingRect), // this is the new boundingbox from the user move.
          modelBox.RotationAngle, // this is the new rotationangle from the user move.
          new SysCAD.Protocol.Rectangle(textBox.BoundingRect),
          textBox.RotationAngle,
          graphicItem.FillColor,
          graphicItem.FillMode,
          graphicItem.MirrorX,
          graphicItem.MirrorY))
        { // failure, revert back to previous.
          modelBox.BoundingRect = graphicItem.BoundingRect;
          modelBox.RotationAngle = (float)graphicItem.Angle;
        }

        ArrowCollection incomingArrows = modelBox.IncomingArrows.Clone();

        foreach (Arrow arrow in incomingArrows)
        {
          GraphicLink graphicLink = state.GraphicLink((arrow.Tag as Link).Guid);

          if (!state.ModifyGraphicLink(out requestId,
            graphicLink.Guid,
            graphicLink.Tag,
            graphicLink.ClassID,
            graphicLink.Origin,
            graphicLink.Destination,
            graphicLink.OriginPort,
            graphicLink.DestinationPort,
            State.GetControlPoints(arrow.ControlPoints),
            graphicLink.TextArea,
            graphicLink.TextAngle))
          {
            State.SetControlPoints(arrow, graphicLink.ControlPoints);
          }
        }

        ArrowCollection outgoingArrows = modelBox.OutgoingArrows.Clone();

        foreach (Arrow arrow in outgoingArrows)
        {
          GraphicLink graphicLink = state.GraphicLink((arrow.Tag as Link).Guid);

          if (!state.ModifyGraphicLink(out requestId,
            graphicLink.Guid,
            graphicLink.Tag,
            graphicLink.ClassID,
            graphicLink.Origin,
            graphicLink.Destination,
            graphicLink.OriginPort,
            graphicLink.DestinationPort,
            State.GetControlPoints(arrow.ControlPoints),
            graphicLink.TextArea,
            graphicLink.TextAngle))
          {
            State.SetControlPoints(arrow, graphicLink.ControlPoints);
          }
        }
      }

      else if (e.Box.Tag is Thing)
      {
        GraphicThing graphicThing = state.GraphicThing((e.Box.Tag as Thing).Guid);
        Box box = (e.Box.Tag as Thing).Box;

        Int64 requestId;

        if (!state.ModifyGraphicThing(out requestId,
            graphicThing.Guid,
            graphicThing.Tag,
            graphicThing.Path,
            new SysCAD.Protocol.Rectangle(box.BoundingRect), // this is the new boundingbox from the user move.
            graphicThing.Xaml,
            box.RotationAngle, // this is the new rotationangle from the user move.
            graphicThing.MirrorX,
            graphicThing.MirrorY))
        {
          box.BoundingRect = graphicThing.BoundingRect;
          box.RotationAngle = (float)graphicThing.Angle;
        }

        else
        {
          graphicThing.BoundingRect = new SysCAD.Protocol.Rectangle(box.BoundingRect);
          graphicThing.Angle = box.RotationAngle;
        }

        //box.Image = State.GetImage(graphicThing, fcFlowChart);
      }

      form1.GraphicPropertyGrid.Refresh();

      ContextMenu propertyGridMenu = form1.GraphicPropertyGrid.ContextMenu;

      if (propertyGridMenu == null)
        propertyGridMenu = new ContextMenu();

      propertyGridMenu.MenuItems.Add("Test");

      form1.GraphicPropertyGrid.ContextMenu = propertyGridMenu;
    }

    private void fcFlowChartBoxModifying(object sender, BoxConfirmArgs e)
    {

      if (e.Box.Tag is Item)
      {
        Box graphicBox = (e.Box.Tag as Item).Graphic;
        //graphicBox.BoundingRect = (e.Box.Tag as Item).Model.BoundingRect;
        graphicBox.RotationAngle = (e.Box.Tag as Item).Model.RotationAngle;
      }
    }

    static private SysCAD.Protocol.Point GetRelativeAnchorPosition(SysCAD.Protocol.Rectangle nodeRect, Double x, Double y, Double angle)
    {
      SysCAD.Protocol.Point point = new SysCAD.Protocol.Point(nodeRect.X + nodeRect.Width * x / 100.0, nodeRect.Y + nodeRect.Height * y / 100.0);

      if (angle != 0)
      {
        point = rotatePointAt(point, getCenter(nodeRect), angle);
      }
      return point;
    }

    private void InsertAnnotation(object sender, EventArgs e)
    {
      InsertAnnotationDialog insertAnnotationDialog = new InsertAnnotationDialog(state);

      if (insertAnnotationDialog.ShowDialog(this) == DialogResult.OK)
      {
        GraphicThing graphicThing = new GraphicThing(insertAnnotationDialog.ThingTag);
        PointF pointF = fcFlowChart.ClientToDoc(mousePressed);
        graphicThing.Xaml = insertAnnotationDialog.ThingXaml;
        graphicThing.X = pointF.X;
        graphicThing.Y = pointF.Y;
        graphicThing.Width = insertAnnotationDialog.DefaultWidth;
        graphicThing.Height = insertAnnotationDialog.DefaultHeight;

        NewGraphicThing(graphicThing, state.TVNavigation.SelectedNode.FullPath + state.TVNavigation.PathSeparator);
      }
    }

    private void LayoutFlowchart(object sender, EventArgs e)
    {
      AnnealLayout layout = new AnnealLayout();
      layout.Arrange(fcFlowChart);
    }

    private void RaiseItemToTop(object sender, EventArgs e)
    {
      (hoverBox.Tag as Item).Graphic.ZTop();
      (hoverBox.Tag as Item).Model.ZTop();
      fcFlowChart.Invalidate();
    }

    private void RaiseThingToTop(object sender, EventArgs e)
    {
      (hoverBox.Tag as Thing).Box.ZTop();
    }

    private void refreshConnectedObjects(Arrow arrowBeingModified)
    {
      fcFlowChart.Invalidate();
    }

    private void RouteLink(Arrow arrow)
    {
      arrow.Route();

      {
        List<SysCAD.Protocol.Point> controlPoints = State.GetControlPoints(arrow.ControlPoints);

        if (Math.Abs(controlPoints[0].X - controlPoints[1].X) <= fcFlowChart.MergeThreshold)
          arrow.CascadeOrientation = MindFusion.FlowChartX.Orientation.Vertical;

        else if (Math.Abs(controlPoints[0].Y - controlPoints[1].Y) <= fcFlowChart.MergeThreshold)
          arrow.CascadeOrientation = MindFusion.FlowChartX.Orientation.Horizontal;

        GraphicLink graphicLink = (arrow.Tag as Link).GraphicLink as GraphicLink;

        Item originItem = arrow.Origin.Tag as Item;
        Item destinationItem = arrow.Destination.Tag as Item;

        GraphicItem originGraphicItem = originItem.GraphicItem;
        GraphicItem destinationGraphicItem = destinationItem.GraphicItem;

        Int64 requestId;

        String originPort = "";
        String destinationPort = "";

        if (arrow.OrgnAnchor != -1)
          originPort = originGraphicItem.anchorIntToTag[arrow.OrgnAnchor];

        if (arrow.DestAnchor != -1)
          destinationPort = destinationGraphicItem.anchorIntToTag[arrow.DestAnchor];

        if (!state.ModifyGraphicLink(out requestId,
          graphicLink.Guid,
          graphicLink.Tag,
          graphicLink.ClassID,
          originItem.Guid,
          destinationItem.Guid,
          originPort,
          destinationPort,
          controlPoints,
          graphicLink.TextArea,
          graphicLink.TextAngle))
        { // failure, revert back to previous.
          // do something here...
        }
      }

      fcFlowChart.Invalidate();
    }

    private void RouteLink(object sender, EventArgs e)
    {
      RouteLink(hoverArrow);
    }

    private void RouteLinks(object sender, EventArgs e)
    {
      ArrowCollection incomingArrows = (hoverBox.Tag as Item).IncomingArrows.Clone();
      ArrowCollection outgoingArrows = (hoverBox.Tag as Item).OutgoingArrows.Clone();

      foreach (Arrow arrow in incomingArrows)
      {
        RouteLink(arrow);
      }

      foreach (Arrow arrow in outgoingArrows)
      {
        RouteLink(arrow);
      }
    }

    private void SendItemToBottom(object sender, EventArgs e)
    {
      (hoverBox.Tag as Item).Graphic.ZBottom();
      (hoverBox.Tag as Item).Model.ZBottom();
      fcFlowChart.Invalidate();
    }

    private void SendThingToBottom(object sender, EventArgs e)
    {
      (hoverBox.Tag as Thing).Box.ZBottom();
    }

    public String CurrentModel
    {
      get { return currentModel; }
      set { currentModel = value; }
    }

    public String CurrentStencil
    {
      get { return currentStencil; }
      set { currentStencil = value; }
    }

    public FlowChart FlowChart
    {
      get { return fcFlowChart; }
    }

    public State State
    {
      get { return state; }
    }

    internal void DeleteSelection()
    {
      List<Item> items = new List<Item>();
      List<Link> links = new List<Link>();
      Int64 requestId;

      foreach (Box box in fcFlowChart.Selection.Boxes)
      {
        Item item = box.Tag as Item;
        if (!items.Contains(item))
        {
          items.Add(item);
        }
      }

      foreach (Arrow arrow in fcFlowChart.Selection.Arrows)
      {
        Link link = arrow.Tag as Link;
        if (!links.Contains(link))
        {
          links.Add(link);
        }
      }

      foreach (Item item in items)
      {
        state.DeleteGraphicItem(out requestId, item.Guid);
      }

      foreach (Link link in links)
      {
        state.DeleteGraphicLink(out requestId, link.Guid);
      }
    }
  }
}
