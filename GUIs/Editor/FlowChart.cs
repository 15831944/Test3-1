using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;

using MindFusion.FlowChartX;

using SysCAD.Interface;
using PureComponents.TreeView;
using System.Windows.Forms;
using ActiproSoftware.UIStudio.Bar;
using MindFusion.FlowChartX.LayoutSystem;
using System.Drawing.Drawing2D;

namespace SysCAD.Editor
{
  public partial class FrmFlowChart : Form
  {
    public State state = new State();

    public string currentModel;
    public string currentStencil;

    public Arrow arrowBeingModified = null;
    public int arrowBeingModifiedSelectionHandle = -1;

    private int tempBoxKey = 0;
    private int tempArrowKey = 0;

    private Form1 form1;

    private Anchor originAnchorChosen = null;
    private Anchor destinationAnchorChosen = null;


    public FrmFlowChart(Form1 form1)
    {
      this.form1 = form1;

      InitializeComponent();


      this.fcFlowChart.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.fcFlowChart_MouseWheel);

      fcFlowChart.DocExtents = new RectangleF(0.0F, 0.0F, 297.0F, 210.0F);
      fcFlowChart.Selection.Style = SelectionStyle.SemiTransparent;
    }

    ~FrmFlowChart()
    {
      state.DisconnectGraphic(
        new ClientGraphic.ItemCreatedHandler(fcFlowChart_ItemCreated),
        new ClientGraphic.ItemModifiedHandler(fcFlowChart_ItemModified),
        new ClientGraphic.ItemDeletedHandler(fcFlowChart_ItemDeleted),
        new ClientGraphic.LinkCreatedHandler(fcFlowChart_LinkCreated),
        new ClientGraphic.LinkModifiedHandler(fcFlowChart_LinkModified),
        new ClientGraphic.LinkDeletedHandler(fcFlowChart_LinkDeleted));
    }

    internal void SetProject(ClientGraphic graphic, Config config, PureComponents.TreeView.TreeView tvNavigation)
    {
      state.Graphic = graphic;
      state.Config = config;
      state.TvNavigation = tvNavigation;

      state.ConnectGraphic(
        new ClientGraphic.ItemCreatedHandler(fcFlowChart_ItemCreated), 
        new ClientGraphic.ItemModifiedHandler(fcFlowChart_ItemModified),
        new ClientGraphic.ItemDeletedHandler(fcFlowChart_ItemDeleted),
        new ClientGraphic.LinkCreatedHandler(fcFlowChart_LinkCreated),
        new ClientGraphic.LinkModifiedHandler(fcFlowChart_LinkModified),
        new ClientGraphic.LinkDeletedHandler(fcFlowChart_LinkDeleted));

      fcFlowChart.UndoManager.UndoEnabled = false;
      fcFlowChart.UseWaitCursor = true;
      fcFlowChart.ValidityChecks = false;
      fcFlowChart.Visible = false;
      fcFlowChart.Enabled = false;

      foreach (GraphicItem graphicItem in graphic.graphicItems.Values)
      {
        state.CreateItem(graphicItem, false, fcFlowChart);
      }

      foreach (GraphicLink graphicLink in graphic.graphicLinks.Values)
      {
        if (graphicLink.ClassID == "Pipe-1")
        {
          state.CreateLink(graphicLink, false, fcFlowChart);
        }
      }

      fcFlowChart.UndoManager.UndoEnabled = true;
      fcFlowChart.UseWaitCursor = false;
      fcFlowChart.ValidityChecks = true;
      fcFlowChart.Visible = true;
      fcFlowChart.Enabled = true;
    }

    internal void UnsetProject()
    {

    }

    public void ZoomToVisible()
    {
      FixDocExtents();

      float minX =  Single.MaxValue, minY =  Single.MaxValue;
      float maxX = -Single.MaxValue, maxY = -Single.MaxValue;

      bool foundVisibleObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {
        if (box.Visible)
        {
          foundVisibleObject = true;
          if (box.BoundingRect.Left < minX)   minX = box.BoundingRect.Left;
          if (box.BoundingRect.Top < minY)    minY = box.BoundingRect.Top;
          if (box.BoundingRect.Right > maxX)  maxX = box.BoundingRect.Right;
          if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
        }
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {
        if (arrow.Visible)
        {
          foundVisibleObject = true;
          if (arrow.BoundingRect.Left < minX)   minX = arrow.BoundingRect.Left;
          if (arrow.BoundingRect.Top < minY)    minY = arrow.BoundingRect.Top;
          if (arrow.BoundingRect.Right > maxX)  maxX = arrow.BoundingRect.Right;
          if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
        }
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundVisibleObject)
        fcFlowChart.ZoomToRect(RectangleF.FromLTRB(minX - width * 0.05F, minY - height * 0.05F, maxX + width * 0.05F, maxY + height * 0.05F));
      else
        fcFlowChart.ZoomToRect(fcFlowChart.DocExtents);

      fcFlowChart.Invalidate();
    }

    public void FixDocExtents()
    {
      float minX = Single.MaxValue, minY = Single.MaxValue;
      float maxX = -Single.MaxValue, maxY = -Single.MaxValue;

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

    public void ZoomToSelected()
    {
      FixDocExtents();

      float minX = Single.MaxValue, minY = Single.MaxValue;
      float maxX = -Single.MaxValue, maxY = -Single.MaxValue;

      bool foundSelectedObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {
        if (box.Selected)
        {
          foundSelectedObject = true;
          if (box.BoundingRect.Left < minX)   minX = box.BoundingRect.Left;
          if (box.BoundingRect.Top < minY)    minY = box.BoundingRect.Top;
          if (box.BoundingRect.Right > maxX)  maxX = box.BoundingRect.Right;
          if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
        }
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {
        if (arrow.Selected)
        {
          foundSelectedObject = true;
          if (arrow.BoundingRect.Left < minX)   minX = arrow.BoundingRect.Left;
          if (arrow.BoundingRect.Top < minY)    minY = arrow.BoundingRect.Top;
          if (arrow.BoundingRect.Right > maxX)  maxX = arrow.BoundingRect.Right;
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

    private void fcFlowChart_ItemCreated(uint eventID, uint requestID, Guid guid, String tag, String path, Model model, Shape shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      state.AddNode(path, tag, guid);
      state.CreateItem(state.GraphicItem(guid), true, fcFlowChart);
    }

    private void fcFlowChart_ItemModified(uint eventID, uint requestID, Guid guid, String tag, String path, Model model, Shape shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      Item item = state.Item(guid);
      if (item != null)
      {
        item.Model.BoundingRect = boundingRect;
        item.Model.RotationAngle = angle;

        ModelStencil modelShape = state.ModelShape(model);
        if (modelShape != null)
          item.Model.Shape = state.GetShapeTemplate(modelShape, mirrorX, mirrorY);
        else
          item.Model.Shape = ShapeTemplate.FromId("Decision2");

        item.Model.AnchorPattern = state.GetAnchorPattern(modelShape, item.GraphicItem);

        item.Graphic.BoundingRect = boundingRect;
        item.Graphic.RotationAngle = angle;
        item.Graphic.FillColor = fillColor;

        //item.Graphic.Shape = shape.ShapeTemplate(item.MirrorX, iItem.MirrorY);
        {
          GraphicStencil graphicShape = state.GraphicShape(shape);
          if (graphicShape != null)
            item.Graphic.Shape = state.GetShapeTemplate(graphicShape, mirrorX, mirrorY);
          else
            item.Graphic.Shape = ShapeTemplate.FromId("Decision2");
        }
      }
    }

    private void fcFlowChart_ItemDeleted(uint eventID, uint requestID, Guid guid)
    {
      state.DeleteItem(guid);
    }



    private void fcFlowChart_LinkCreated(uint eventID, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      state.CreateLink(state.GraphicLink(guid), true, fcFlowChart);
    }

    private void fcFlowChart_LinkModified(uint eventID, uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      Link Link = state.Link(guid);
      if (Link != null)
      {
        //Link.Arrow.
        //Link.Model.BoundingRect = boundingRect;
        //Link.Graphic.BoundingRect = boundingRect;
        //Link.Model.RotationAngle = angle;
        //Link.Graphic.RotationAngle = angle;
      }
    }

    private void fcFlowChart_LinkDeleted(uint eventID, uint requestID, Guid guid)
    {
      // TBD
    }

    private void fcFlowChart_ArrowModified(object sender, ArrowMouseArgs e)
    {
      form1.toolStripStatusLabel1.Text = "";

      arrowBeingModified.CustomDraw = CustomDraw.None;
      arrowBeingModifiedSelectionHandle = -1;
      arrowBeingModified = null;
      originAnchorChosen = null;
      destinationAnchorChosen = null;

      GraphicLink graphicLink = (e.Arrow.Tag as Link).graphicLink as GraphicLink;

      if (oldOriginBox != null)
      {
        (e.Arrow.Tag as Link).graphicLink.Origin = oldOriginGuid;
        e.Arrow.Origin = oldOriginBox;
        e.Arrow.OrgnAnchor = oldOriginAnchor;
      }
      else if (newOriginBox != null)
      {
        (e.Arrow.Tag as Link).graphicLink.Origin = newOriginGuid;
        e.Arrow.Origin = newOriginBox;
        e.Arrow.OrgnAnchor = newOriginAnchor;
      }

      if (oldDestinationBox != null)
      {
        (e.Arrow.Tag as Link).graphicLink.Destination = oldDestinationGuid;
        e.Arrow.Destination = oldDestinationBox;
        e.Arrow.DestAnchor = oldDestinationAnchor;
      }
      else if (newDestinationBox != null)
      {
        (e.Arrow.Tag as Link).graphicLink.Destination = newDestinationGuid;
        e.Arrow.Destination = newDestinationBox;
        e.Arrow.DestAnchor = newDestinationAnchor;
      }

      e.Arrow.SegmentCount = (short)(oldControlPoints.Count - 1);
      int i = 0;
      foreach (PointF point in oldControlPoints)
        e.Arrow.ControlPoints[i++] = point;
      e.Arrow.UpdateFromPoints();

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
      
      string originAnchor = null;
      string destinationAnchor = null;
      if (originGraphicItem != null)
        originGraphicItem.anchorIntToTag.TryGetValue(e.Arrow.OrgnAnchor, out originAnchor);
      if (destinationGraphicItem != null)
        destinationGraphicItem.anchorIntToTag.TryGetValue(e.Arrow.DestAnchor, out destinationAnchor);

      uint requestID;
      if (!state.ModifyGraphicLink(out requestID,
        graphicLink.Guid,
        graphicLink.Tag,
        graphicLink.ClassID,
        originGuid,
        destinationGuid,
        originAnchor,
        destinationAnchor,
        oldControlPoints))
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

    Guid oldOriginGuid = Guid.Empty;
    Box oldOriginBox = null;
    int oldOriginAnchor = -1;

    Guid oldDestinationGuid = Guid.Empty;
    Box oldDestinationBox = null;
    int oldDestinationAnchor = -1;

    Guid newOriginGuid = Guid.Empty;
    Box newOriginBox = null;
    int newOriginAnchor = -1;

    Guid newDestinationGuid = Guid.Empty;
    Box newDestinationBox = null;
    int newDestinationAnchor = -1;

    List<PointF> oldControlPoints = new List<PointF>();

    private void fcFlowChart_ArrowModifying(object sender, ArrowConfirmArgs e)
    {
      form1.Mode_Modify();
      if (fcFlowChart.Selection.Arrows.Count == 1) // We're playing with just one arrow...
      {
        DoModifyingStuff(e.Arrow, e.SelectionHandle);
      }
    }

    private void DoModifyingStuff(Arrow arrow, int selectionHandle)
    {
      oldControlPoints.Clear();
      foreach (PointF point in arrow.ControlPoints.Clone())
      {
        oldControlPoints.Add(point);
      }

      arrowBeingModifiedSelectionHandle = selectionHandle;
      arrowBeingModified = arrow;
      arrowBeingModified.CustomDraw = CustomDraw.Additional;
      arrowBeingModified.ZTop();

      if (arrow.Tag != null)
        oldOriginGuid = (arrow.Tag as Link).graphicLink.Origin;
      else
        oldOriginGuid = Guid.Empty;
      oldOriginBox = arrow.Origin as Box;
      oldOriginAnchor = arrow.OrgnAnchor;

      if (arrow.Tag != null)
        oldDestinationGuid = (arrow.Tag as Link).graphicLink.Destination;
      else
        oldDestinationGuid = Guid.Empty;
      oldDestinationBox = arrow.Destination as Box;
      oldDestinationAnchor = arrow.DestAnchor;

      if (arrowBeingModifiedSelectionHandle == 0)
      {
        PointF originPos = arrowBeingModified.ControlPoints[0];
        Box originBox = fcFlowChart.GetBoxAt(originPos, 2.0F);

        if ((selectionHandle == 0) && (originBox != null) && (!(arrowBeingModified.Origin is Box)))
        {
          originBox = (originBox.Tag as Item).Model;
          if (originBox != null)
          {
            int closestI = 0;
            float closestDistance = float.MaxValue;

            for (int i = 0; i < originBox.AnchorPattern.Points.Count; i++)
            {
              if (originBox.AnchorPattern.Points[i].AllowOutgoing)
              {
                float thisDistance = Distance(originPos, originBox.AnchorPattern.Points[i].X, originBox.AnchorPattern.Points[i].Y);
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
            form1.toolStripStatusLabel1.Text = (originBox.Tag as Item).GraphicItem.anchorIntToTag[newOriginAnchor];
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
        PointF destinationPos = arrowBeingModified.ControlPoints[arrowBeingModified.ControlPoints.Count - 1];
        Box destinationBox = fcFlowChart.GetBoxAt(destinationPos, 2.0F);

        if ((destinationBox != null) && (!(arrowBeingModified.Destination is Box)))
        {
          destinationBox = (destinationBox.Tag as Item).Model;
          if (destinationBox != null)
          {
            int closestI = 0;
            float closestDistance = float.MaxValue;

            for (int i = 0; i < destinationBox.AnchorPattern.Points.Count; i++)
            {
              if (destinationBox.AnchorPattern.Points[i].AllowIncoming)
              {
                AnchorPoint anchorPoint = destinationBox.AnchorPattern.Points[i];
                RectangleF nodeRect = destinationBox.BoundingRect;
                float angle = destinationBox.RotationAngle;
                PointF anchorPos = anchorPoint.AnchorToDoc(nodeRect, angle);
                float thisDistance = Distance(destinationPos, anchorPos);
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
            form1.toolStripStatusLabel1.Text = (newDestinationBox.Tag as Item).GraphicItem.anchorIntToTag[newDestinationAnchor];
          }
        }
      }
      //else if (oldDestinationBox != null)
      //{
      //  (e.Arrow.Tag as Link).graphicLink.Destination = oldDestinationGuid;
      //  e.Arrow.Destination = oldDestinationBox;
      //  e.Arrow.DestAnchor = oldDestinationAnchor;
      //}

      fcFlowChart.RecreateCacheImage();
    }

    private Arrow oldHoverArrow = null;
    private Box oldHoverBox = null;

    private Arrow hoverArrow = null;
    private Box hoverBox = null;

    public void fcFlowChart_MouseMove(object sender, MouseEventArgs e)
    {
      hoverArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)), 2.0F);

      hoverBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)), 2.0F);
      if (hoverBox != null)
        hoverBox = (hoverBox.Tag as Item).Model;


      if (e.Button == MouseButtons.Left)
      {
        if ((hoverBox != null) || (hoverArrow != null)) // we're not in free space...
        {
          if ((form1.barManager1.Commands["Mode.CreateNode"] as BarButtonCommand).Checked == true)
            form1.Mode_Modify();
        }
      }


      if (hoverArrow != null)
      {
        hoverArrow.CustomDraw = CustomDraw.Additional;
        hoverArrow.ZTop();

        if (hoverArrow.Destination is Box)
        {
          Item hoverItem = hoverArrow.Destination.Tag as Item;
          hoverItem.Text.ZIndex = hoverArrow.ZIndex - 1;
          hoverItem.Model.ZIndex = hoverArrow.ZIndex - 2;
        }

        if (hoverArrow.Origin is Box)
        {
          Item hoverItem = hoverArrow.Origin.Tag as Item;
          hoverItem.Text.ZIndex = hoverArrow.ZIndex - 1;
          hoverItem.Model.ZIndex = hoverArrow.ZIndex - 2;
        }

        hoverArrow.Visible = state.ShowLinks;
      }


      if (hoverBox != null)
      {
        Item hoverItem = hoverBox.Tag as Item;
        hoverItem.Graphic.Visible = hoverItem.Visible;
        //hoverItem.Graphic.ZTop();
        hoverItem.Model.Visible = hoverItem.Visible;
        hoverItem.Model.CustomDraw = CustomDraw.Additional;
        hoverItem.Model.ZIndex = hoverItem.Graphic.ZIndex + 1;
        hoverItem.Text.Visible = hoverItem.Visible && state.ShowTags;
        hoverItem.Text.ZIndex = hoverItem.Model.ZIndex + 1;

        foreach (Arrow arrow in hoverItem.IncomingArrows)
        {
          arrow.Visible = hoverItem.Visible;
          arrow.CustomDraw = CustomDraw.Additional;
          arrow.ZIndex = hoverItem.Text.ZIndex + 1;
        }

        foreach (Arrow arrow in hoverItem.OutgoingArrows)
        {
          arrow.Visible = hoverItem.Visible;
          arrow.CustomDraw = CustomDraw.Additional;
          arrow.ZIndex = hoverItem.Text.ZIndex + 1;
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
            oldHoverItem.Text.ZIndex = oldHoverArrow.ZIndex - 1;
            oldHoverItem.Model.ZIndex = oldHoverArrow.ZIndex - 2;
          }

          if (oldHoverArrow.Origin is Box)
          {
            Item oldHoverItem = oldHoverArrow.Origin.Tag as Item;
            oldHoverItem.Text.ZIndex = oldHoverArrow.ZIndex - 1;
            oldHoverItem.Model.ZIndex = oldHoverArrow.ZIndex - 2;
          }

          oldHoverArrow.Visible = true;
        }
      }

      if (oldHoverBox != null) // deal with old itemBox.
      {
        if (oldHoverBox != hoverBox) // we've moved on, un-hover the old one.
        {
          Item oldHoverItem = oldHoverBox.Tag as Item;
          oldHoverItem.Graphic.Visible = oldHoverItem.Visible && state.ShowGraphics;
          oldHoverItem.Model.Visible = oldHoverItem.Visible && (oldHoverItem.Model.Selected || state.ShowModels);
          oldHoverItem.Model.CustomDraw = CustomDraw.None;
          oldHoverItem.Model.ZIndex = oldHoverItem.Graphic.ZIndex + 1;
          oldHoverItem.Text.Visible = oldHoverItem.Visible && state.ShowTags;
          oldHoverItem.Text.ZIndex = oldHoverItem.Model.ZIndex + 1;

          foreach (Arrow arrow in oldHoverItem.IncomingArrows)
          {
            arrow.Visible = oldHoverItem.Visible && state.ShowLinks;
            arrow.CustomDraw = CustomDraw.None;
            arrow.ZIndex = oldHoverItem.Text.ZIndex + 1;
          }

          foreach (Arrow arrow in oldHoverItem.OutgoingArrows)
          {
            arrow.Visible = oldHoverItem.Visible && state.ShowLinks;
            arrow.CustomDraw = CustomDraw.None;
            arrow.ZIndex = oldHoverItem.Text.ZIndex + 1;
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

            form1.toolStripStatusLabel1.Text = 
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

            form1.toolStripStatusLabel1.Text = 
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

    private void refreshConnectedObjects(Arrow arrowBeingModified)
    {
      fcFlowChart.Invalidate();
      if (arrowBeingModified != null)
      {
        //Box origin = arrowBeingModified.Origin as Box;
        //Box desination = arrowBeingModified.Destination as Box;

        //if (origin != null)
        //  fcFlowChart.Invalidate(fcFlowChart.DocToClient(origin.BoundingRect));
        //if (desination != null)
        //  fcFlowChart.Invalidate(fcFlowChart.DocToClient(desination.BoundingRect));

        //if (newOriginBox != null)
        //  fcFlowChart.Invalidate(fcFlowChart.DocToClient(newOriginBox.BoundingRect));
        //if (newDestinationBox != null)
        //  fcFlowChart.Invalidate(fcFlowChart.DocToClient(newDestinationBox.BoundingRect));

        //if (oldOriginBox != null)
        //  fcFlowChart.Invalidate(fcFlowChart.DocToClient(oldOriginBox.BoundingRect));
        //if (oldDestinationBox != null)
        //  fcFlowChart.Invalidate(fcFlowChart.DocToClient(oldDestinationBox.BoundingRect));

        //fcFlowChart.Invalidate(fcFlowChart.DocToClient(arrowBeingModified.BoundingRect));
      }
    }
    
    private void fcFlowChart_DrawBox(object sender, BoxDrawArgs e)
    {
      if (arrowBeingModified != null)
      {
        PointF originPos = arrowBeingModified.ControlPoints[0];
        Box originBox = fcFlowChart.GetBoxAt(originPos, 2.0F);
        if (originBox != null)
        {
          originBox = (originBox.Tag as Item).Model;
          if (originBox != null)
          {
            if (originBox.AnchorPattern != null)
            {
              //if (originAnchorChosen == null)
              {
                float closest = float.MaxValue;

                foreach (AnchorPoint anchorPoint in originBox.AnchorPattern.Points)
                {
                  float dx = anchorPoint.X / 100.0F;
                  float dy = anchorPoint.Y / 100.0F;
                  PointF anchorPointPos = new PointF(
                  originBox.BoundingRect.Left + originBox.BoundingRect.Width * dx,
                  originBox.BoundingRect.Top + originBox.BoundingRect.Height * dy);

                  float distance = Distance(anchorPointPos, originPos);
                  if (distance < closest)
                  {
                    closest = distance;
                    originAnchorChosen = anchorPoint.Tag as Anchor;
                  }
                }

                if (state.PortCheck((originBox.Tag as Item).Guid, originAnchorChosen) == PortStatus.Available)
                {
                  float dx = originAnchorChosen.position.X / 100.0F;
                  float dy = originAnchorChosen.position.Y / 100.0F;
                  PointF anchorPointPos = new PointF(
                  originBox.BoundingRect.Left + originBox.BoundingRect.Width * dx,
                  originBox.BoundingRect.Top + originBox.BoundingRect.Height * dy);

                  PointF[] extensionPoints =
                    new PointF[] { anchorPointPos, anchorPointPos };
                  System.Drawing.Pen pen = new System.Drawing.Pen(Color.Yellow, 5.0F);
                  e.Graphics.DrawEllipse(pen, RectangleF.FromLTRB(
                    anchorPointPos.X - 2.0F,
                    anchorPointPos.Y - 2.0F,
                    anchorPointPos.X + 2.0F,
                    anchorPointPos.Y + 2.0F));
                  e.Graphics.DrawLines(pen, extensionPoints);
                }
              }
            }
          }
        }

        PointF destinationPos = arrowBeingModified.ControlPoints[arrowBeingModified.ControlPoints.Count - 1];
        Box destinationBox = fcFlowChart.GetBoxAt(destinationPos, 2.0F);
        if (destinationBox != null)
        {
          destinationBox = (destinationBox.Tag as Item).Model;
          if (destinationBox != null)
          {
            if (destinationBox.AnchorPattern != null)
            {
              //if (destinationAnchorChosen == null)
              {
                float closest = float.MaxValue;

                foreach (AnchorPoint anchorPoint in destinationBox.AnchorPattern.Points)
                {
                  float dx = anchorPoint.X / 100.0F;
                  float dy = anchorPoint.Y / 100.0F;
                  PointF anchorPointPos = new PointF(
                  destinationBox.BoundingRect.Left + destinationBox.BoundingRect.Width * dx,
                  destinationBox.BoundingRect.Top + destinationBox.BoundingRect.Height * dy);

                  float distance = Distance(anchorPointPos, destinationPos);
                  if (distance < closest)
                  {
                    closest = distance;
                    destinationAnchorChosen = anchorPoint.Tag as Anchor;
                  }
                }

                if (state.PortCheck((destinationBox.Tag as Item).Guid, destinationAnchorChosen) == PortStatus.Available)
                {
                  float dx = destinationAnchorChosen.position.X / 100.0F;
                  float dy = destinationAnchorChosen.position.Y / 100.0F;
                  PointF anchorPointPos = new PointF(
                  destinationBox.BoundingRect.Left + destinationBox.BoundingRect.Width * dx,
                  destinationBox.BoundingRect.Top + destinationBox.BoundingRect.Height * dy);

                  PointF[] extensionPoints =
                    new PointF[] { anchorPointPos, anchorPointPos };
                  System.Drawing.Pen pen = new System.Drawing.Pen(Color.Yellow, 5.0F);
                  e.Graphics.DrawEllipse(pen, RectangleF.FromLTRB(
                    anchorPointPos.X - 2.0F,
                    anchorPointPos.Y - 2.0F,
                    anchorPointPos.X + 2.0F,
                    anchorPointPos.Y + 2.0F));
                  e.Graphics.DrawLines(pen, extensionPoints);
                }

              }
            }
          }
        }
      }
    }

    private float Distance(PointF a, float bX, float bY)
    {
      return Distance(a, new PointF(bX, bY));
    }

    private float Distance(PointF a, PointF b)
    {
      return (float)Math.Sqrt(((a.X - b.X) * (a.X - b.X)) + ((a.Y - b.Y) * (a.Y - b.Y)));
    }

    internal static float wrongWrongWrongWrongGetMinObjSize(GraphicsUnit currUnit)
    {
      switch (currUnit)
      {
        case GraphicsUnit.Millimeter:
          return 2;
        case GraphicsUnit.Inch:
          return 1.0f / 6;
        case GraphicsUnit.Point:
          return 72.0f / 6;
        case GraphicsUnit.Pixel:
          return 6;
        case GraphicsUnit.Document:
          return 300.0f / 6;
        case GraphicsUnit.Display:
          return 75.0f / 6;
      }

      return 2;
    }

    private void fcFlowChart_DrawArrow(object sender, ArrowDrawArgs e)
    {
      Arrow arrow = e.Arrow;

      if (arrow != arrowBeingModified)
      {
        {
          MindFusion.FlowChartX.Node node = arrow.Destination;
          if ((node is Box) && (arrow.DestAnchor != -1))
          {
            Box box = node as Box;

            PointF anchorPointPos = GetRelativeAnchorPosition(box.BoundingRect, 
              box.AnchorPattern.Points[arrow.DestAnchor].X, 
              box.AnchorPattern.Points[arrow.DestAnchor].Y, 
              box.RotationAngle);
            //float dx = box.AnchorPattern.Points[arrow.DestAnchor].X / 100.0F;
            //float dy = box.AnchorPattern.Points[arrow.DestAnchor].Y / 100.0F;

            //PointF anchorPointPos = new PointF(
            //  box.BoundingRect.Left + box.BoundingRect.Width * dx,
            //  box.BoundingRect.Top + box.BoundingRect.Height * dy);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
          else
          {
            Color errorColor = new Color();
            if (node is Box) errorColor = Color.Yellow;
            else errorColor = Color.Red;

            System.Drawing.Pen pen = new System.Drawing.Pen(errorColor, wrongWrongWrongWrongGetMinObjSize(fcFlowChart.MeasureUnit));
            PointF p = arrow.ControlPoints[arrow.ControlPoints.Count - 1];
            RectangleF r = new RectangleF(p, SizeF.Empty);
            r.Inflate(wrongWrongWrongWrongGetMinObjSize(fcFlowChart.MeasureUnit), wrongWrongWrongWrongGetMinObjSize(fcFlowChart.MeasureUnit));
            e.Graphics.DrawEllipse(pen, r);
          }
        }

        {
          MindFusion.FlowChartX.Node node = arrow.Origin;
          if ((node is Box) && (arrow.OrgnAnchor != -1))
          {
            Box box = node as Box;

            PointF anchorPointPos = GetRelativeAnchorPosition(box.BoundingRect,
              box.AnchorPattern.Points[arrow.OrgnAnchor].X,
              box.AnchorPattern.Points[arrow.OrgnAnchor].Y,
              box.RotationAngle);
            //float dx = box.AnchorPattern.Points[arrow.OrgnAnchor].X / 100.0F;
            //float dy = box.AnchorPattern.Points[arrow.OrgnAnchor].Y / 100.0F;

            //PointF anchorPointPos = new PointF(
            //  box.BoundingRect.Left + box.BoundingRect.Width * dx,
            //  box.BoundingRect.Top + box.BoundingRect.Height * dy);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[0], anchorPointPos };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
          else
          {
            Color errorColor = new Color();
            if (node is Box) errorColor = Color.Yellow;
            else errorColor = Color.Red;

            System.Drawing.Pen pen = new System.Drawing.Pen(errorColor, wrongWrongWrongWrongGetMinObjSize(fcFlowChart.MeasureUnit));
            PointF p = arrow.ControlPoints[0];
            RectangleF r = new RectangleF(p, SizeF.Empty);
            r.Inflate(wrongWrongWrongWrongGetMinObjSize(fcFlowChart.MeasureUnit), wrongWrongWrongWrongGetMinObjSize(fcFlowChart.MeasureUnit));
            e.Graphics.DrawEllipse(pen, r);
          }
        }
      }
      else // this is an arrowbeingmodified...
      {
        {
          if (newDestinationAnchor != -1)
          {
            float dx = newDestinationBox.AnchorPattern.Points[newDestinationAnchor].X / 100.0F;
            float dy = newDestinationBox.AnchorPattern.Points[newDestinationAnchor].Y / 100.0F;

            PointF anchorPointPos = new PointF(
              newDestinationBox.BoundingRect.Left + newDestinationBox.BoundingRect.Width * dx,
              newDestinationBox.BoundingRect.Top + newDestinationBox.BoundingRect.Height * dy);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.LightBlue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
          else if (oldDestinationAnchor != -1)
          {
            float dx = oldDestinationBox.AnchorPattern.Points[oldDestinationAnchor].X / 100.0F;
            float dy = oldDestinationBox.AnchorPattern.Points[oldDestinationAnchor].Y / 100.0F;

            PointF anchorPointPos = new PointF(
              oldDestinationBox.BoundingRect.Left + oldDestinationBox.BoundingRect.Width * dx,
              oldDestinationBox.BoundingRect.Top + oldDestinationBox.BoundingRect.Height * dy);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
        }

        {
          if (newOriginAnchor != -1)
          {
            float dx = newOriginBox.AnchorPattern.Points[newOriginAnchor].X / 100.0F;
            float dy = newOriginBox.AnchorPattern.Points[newOriginAnchor].Y / 100.0F;

            PointF anchorPointPos = new PointF(
              newOriginBox.BoundingRect.Left + newOriginBox.BoundingRect.Width * dx,
              newOriginBox.BoundingRect.Top + newOriginBox.BoundingRect.Height * dy);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[0], anchorPointPos };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.LightBlue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
          else if (oldOriginAnchor != -1)
          {
            float dx = oldOriginBox.AnchorPattern.Points[oldOriginAnchor].X / 100.0F;
            float dy = oldOriginBox.AnchorPattern.Points[oldOriginAnchor].Y / 100.0F;

            PointF anchorPointPos = new PointF(
              oldOriginBox.BoundingRect.Left + oldOriginBox.BoundingRect.Width * dx,
              oldOriginBox.BoundingRect.Top + oldOriginBox.BoundingRect.Height * dy);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[0], anchorPointPos };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
        }
      }
    }

    internal static PointF rotatePointAt(PointF point, PointF pivot, float angle)
    {
      PointF[] points = new PointF[] { point };
      Matrix rotation = new Matrix();
      rotation.RotateAt(angle, pivot);
      rotation.TransformPoints(points);
      rotation.Dispose();
      return points[0];
    }

    internal static PointF getCenter(RectangleF rect)
    {
      return new PointF(rect.X + rect.Width / 2, rect.Y + rect.Height / 2);
    }

    private PointF GetRelativeAnchorPosition(RectangleF nodeRect, float x, float y, float angle)
    {
      PointF point = new PointF(nodeRect.X + nodeRect.Width * x / 100, nodeRect.Y + nodeRect.Height * y / 100);

      if (angle != 0)
      {
        point = rotatePointAt(point,
          getCenter(nodeRect), angle);
      }
      return point; 
    }

    private void fcFlowChart_BoxModified(object sender, BoxMouseArgs e)
    {
      GraphicItem graphicItem = state.GraphicItem((e.Box.Tag as Item).Guid);
      Box modelBox = (e.Box.Tag as Item).Model;

      uint requestID;
      if (!state.ModifyGraphicItem(out requestID,
        graphicItem.Guid,
        graphicItem.Tag,
        graphicItem.Path,
        graphicItem.Model,
        graphicItem.Shape,
        modelBox.BoundingRect, // this is the new boundingbox from the user move.
        modelBox.RotationAngle, // this is the new rotationangle from the user move.
        graphicItem.FillColor,
        graphicItem.MirrorX,
        graphicItem.MirrorY))
      { // failure, revert back to previous.
        modelBox.BoundingRect = graphicItem.BoundingRect;
        modelBox.RotationAngle = graphicItem.Angle;
      }

      form1.propertyGrid1.Refresh();
      
      ContextMenu propertyGridMenu = form1.propertyGrid1.ContextMenu;
      
      if (propertyGridMenu == null)
        propertyGridMenu = new ContextMenu();

      propertyGridMenu.MenuItems.Add("Test");
      form1.propertyGrid1.ContextMenu = propertyGridMenu;
    }

    private void fcFlowChart_BoxModifying(object sender, BoxConfirmArgs e)
    {
      Box graphicBox = (e.Box.Tag as Item).Graphic;
      //graphicBox.BoundingRect = (e.Box.Tag as Item).Model.BoundingRect;
      graphicBox.RotationAngle = (e.Box.Tag as Item).Model.RotationAngle;
    }

    public void NewGraphicItem(GraphicItem graphicItem, string path)
    {
      NewGraphicItem(path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
    }

    public void NewGraphicItem(String path, String model, String shape, RectangleF boundingRect, Single angle, Color fillColor, bool mirrorX, bool mirrorY)
    {
      uint requestID;
      Guid guid;

      while (state.Exists("N_" + tempBoxKey.ToString()))
        tempBoxKey++;
      
      state.CreateGraphicItem(out requestID, out guid, "N_" + tempBoxKey.ToString(), path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public GraphicLink NewGraphicLink(GraphicLink graphicLink, float dx, float dy)
    {
      while (state.Exists("A_" + tempBoxKey.ToString()))
        tempBoxKey++;

      GraphicLink newGraphicLink = new GraphicLink("A_" + tempBoxKey.ToString());
      newGraphicLink.Destination = graphicLink.Destination;
      newGraphicLink.Origin = graphicLink.Origin;

      foreach (PointF point in graphicLink.controlPoints)
      {
        newGraphicLink.controlPoints.Add(new PointF(point.X + dx, point.Y + dy));
      }

      state.CreateLink(newGraphicLink, true, fcFlowChart);

      return newGraphicLink;
    }

    private void fcFlowChart_Click(object sender, EventArgs e)
    {
      MouseEventArgs me = e as MouseEventArgs;
      hoverArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(me.Location), 2);
      hoverBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(me.Location), 2.0F);

      if (me.Button == MouseButtons.Left)
      {
        if ((hoverBox == null) && (hoverArrow == null)) // we're in free space...
        {
          if ((form1.barManager1.Commands["Mode.CreateNode"] as BarButtonCommand).Checked == true)
          {
            NewGraphicItem(
              state.CurrentPath,
              currentModel,
              currentStencil,
              new RectangleF(fcFlowChart.ClientToDoc(me.Location), state.GraphicShape(currentStencil).defaultSize),
              0.0F,
              Color.LightBlue,
              false,
              false
              );
          }
          else
          {
            form1.Mode_Modify();
          }
        }
      }

      if (me.Button == MouseButtons.Right)
      {
        ContextMenu theMenu = new ContextMenu();
        if (hoverBox != null)
        {
          theMenu.MenuItems.Add("Unfinished");
          form1.Mode_Modify();
        }
        else if (hoverArrow != null)
        {
          theMenu.MenuItems.Add("Route Arrow", new EventHandler(RouteArrow));
          theMenu.MenuItems.Add("Disconnect Origin", new EventHandler(DisconnectOrigin));
          theMenu.MenuItems.Add("Disconnect Destination", new EventHandler(DisconnectDestination));
          form1.Mode_Modify();
        }

        theMenu.MenuItems.Add("Layout Flowchart (!!Broken in an interesting way in latest release!!)", new EventHandler(LayoutFlowchart));

        theMenu.Show(fcFlowChart, me.Location);
      }
    }

    private void LayoutFlowchart(object sender, EventArgs e)
    {
      AnnealLayout layout = new AnnealLayout();
      layout.Arrange(fcFlowChart);
    }

    private void RouteArrow(object sender, EventArgs e)
    {
      hoverArrow.Route();

      {
        List<PointF> controlPoints = new List<PointF>();
        foreach (PointF point in hoverArrow.ControlPoints.Clone())
        {
          controlPoints.Add(point);
        }

        GraphicLink graphicLink = (hoverArrow.Tag as Link).graphicLink as GraphicLink;

        Item originItem = hoverArrow.Origin.Tag as Item;
        Item destinationItem = hoverArrow.Destination.Tag as Item;

        GraphicItem originGraphicItem = originItem.GraphicItem;
        GraphicItem destinationGraphicItem = destinationItem.GraphicItem;

        uint requestID;
        if (!state.ModifyGraphicLink(out requestID,
          graphicLink.Guid,
          graphicLink.Tag,
          graphicLink.ClassID,
          originItem.Guid,
          destinationItem.Guid,
          originGraphicItem.anchorIntToTag[hoverArrow.OrgnAnchor],
          destinationGraphicItem.anchorIntToTag[hoverArrow.DestAnchor],
          controlPoints))
        { // failure, revert back to previous.
          // do something here...
        }
      }
    }

    private void DisconnectOrigin(object sender, EventArgs e)
    {
      hoverArrow.OrgnAnchor = -1;
      hoverArrow.Origin = fcFlowChart.Dummy;

      hoverArrow.SegmentCount = (short)((hoverArrow.Tag as Link).graphicLink.controlPoints.Count - 1);
      int i = 0;
      foreach (PointF point in (hoverArrow.Tag as Link).graphicLink.controlPoints)
        hoverArrow.ControlPoints[i++] = point;
      hoverArrow.UpdateFromPoints();
    }

    private void DisconnectDestination(object sender, EventArgs e)
    {
      hoverArrow.DestAnchor = -1;
      hoverArrow.Destination = fcFlowChart.Dummy;

      hoverArrow.SegmentCount = (short)((hoverArrow.Tag as Link).graphicLink.controlPoints.Count - 1);
      int i = 0;
      foreach (PointF point in (hoverArrow.Tag as Link).graphicLink.controlPoints)
        hoverArrow.ControlPoints[i++] = point;
      hoverArrow.UpdateFromPoints();
    }

    private void fcFlowChart_BoxDeleting(object sender, BoxConfirmArgs e)
    {
      e.Confirm = false; // Tell flowchart not to deal with this.
      state.Remove(fcFlowChart);
    }

    private void fcFlowChart_ArrowDeleting(object sender, ArrowConfirmArgs e)
    {
      e.Confirm = false;
      MessageBox.Show("Not Implemented");
    }

    private void fcFlowChart_ArrowCreated(object sender, ArrowEventArgs e)
    {
      tempArrowKey++;
      string newLinkTag = "A_" + tempArrowKey.ToString();
      e.Arrow.Text = newLinkTag;

      Box destinationBox = e.Arrow.Destination as Box;
      Box originBox = e.Arrow.Origin as Box;
      GraphicLink newGraphicLink = new GraphicLink(newLinkTag);

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

      newGraphicLink.controlPoints = new List<PointF>();
      foreach (PointF point in e.Arrow.ControlPoints)
      {
        newGraphicLink.controlPoints.Add(point);
      }

      fcFlowChart.DeleteObject(e.Arrow);

      state.CreateLink(newGraphicLink, true, fcFlowChart);
    }

    private void fcFlowChart_MouseDown(object sender, MouseEventArgs e)
    {
    }

    private void fcFlowChart_ArrowCreating(object sender, AttachConfirmArgs e)
    {
      DoModifyingStuff(e.Arrow, e.Arrow.ControlPoints.Count - 1);
      //arrowBeingModified = e.Arrow;
      //arrowBeingModified.CustomDraw = CustomDraw.Additional;
      //arrowBeingModified.ZTop();
      //fcFlowChart.RecreateCacheImage();
    }
  }
}