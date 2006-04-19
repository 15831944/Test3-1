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

namespace SysCAD.Editor
{
  public partial class FrmFlowChart : Form
  {
    public State state = new State();

    public string currentModelShape;
    public string currentGraphicShape;

    private int tempBoxKey = 0;
    private int tempArrowKey = 0;

    public FrmFlowChart()
    {
      InitializeComponent();

      fcFlowChart.DocExtents = new RectangleF(0.0F, 0.0F, 297.0F, 210.0F);
      fcFlowChart.Selection.Style = SelectionStyle.SemiTransparent;
    }

    ~FrmFlowChart()
    {
      state.DisconnectGraphicItemModified(new Graphic.ItemModifiedHandler(fcFlowChart_ItemModified));
    }

    internal void SetProject(Graphic graphic, Config config, PureComponents.TreeView.TreeView tvNavigation)
    {
      state.Graphic = graphic;
      state.Config = config;
      state.TvNavigation = tvNavigation;

      state.ConnectGraphicItemModified(new Graphic.ItemModifiedHandler(fcFlowChart_ItemModified));

      fcFlowChart.UndoManager.UndoEnabled = false;
      fcFlowChart.UseWaitCursor = true;
      fcFlowChart.ValidityChecks = false;
      fcFlowChart.Visible = false;
      fcFlowChart.Enabled = false;

      foreach (GraphicItem graphicItem in graphic.graphicItems.Values)
      {
        state.newItem(graphicItem, false, fcFlowChart);
      }

      foreach (GraphicLink graphicLink in graphic.graphicLinks.Values)
      {
        if (graphicLink.ClassID == "Pipe-1")
        {
          Arrow arrow = fcFlowChart.CreateArrow(new PointF(0.0F, 0.0F), new PointF(10.0F, 10.0F));
          state.newGraphicLink(graphicLink, arrow, false);
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
        fcFlowChart.ZoomToRect(new RectangleF(minX - width * 0.05F, minY - height * 0.05F, width * 1.1F, height * 1.1F));
      else
        fcFlowChart.ZoomToRect(new RectangleF(fcFlowChart.DocExtents.Left, fcFlowChart.DocExtents.Top, fcFlowChart.DocExtents.Width, fcFlowChart.DocExtents.Height));

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
        fcFlowChart.DocExtents= new RectangleF(minX - width * 0.05F, minY - height * 0.05F, width * 1.1F, height * 1.1F);
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
        fcFlowChart.ZoomToRect(new RectangleF(minX - width * 0.05F, minY - height * 0.05F, width * 1.1F, height * 1.1F));
      else
        fcFlowChart.ZoomToRect(new RectangleF(fcFlowChart.DocExtents.Left, fcFlowChart.DocExtents.Top, fcFlowChart.DocExtents.Width, fcFlowChart.DocExtents.Height));
    }

    private void fcFlowChart_ItemModified(string tag, RectangleF boundingRect, Single angle)
    {
      Item item = state.Item(tag);
      if (item != null)
      {
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
        item.Model.RotationAngle = angle;
        item.Graphic.RotationAngle = angle;
      }
    }

    private void fcFlowChart_ArrowAttaching(object sender, AttachConfirmArgs e)
    {
      if (e.ChangingOrigin) // Origin is trying to be being changed.
      {
        if (e.Origin==null)
          e.Confirm = false; // Trying to be disconnected, ignore.
        else
          e.Confirm = true; // Being attached to another node, OK.
      }
      else // Destination is trying to be being changed.
      {
        if (e.Destination==null)
          e.Confirm = false; // Trying to be disconnected, ignore.
        else
          e.Confirm = true; // Being attached to another node, OK.
      }
    }

    MindFusion.FlowChartX.Node savedOrigin;
    MindFusion.FlowChartX.Node savedDestination;

    int savedOriginAnchor;
    int savedDestinationAnchor;

    private void fcFlowChart_ArrowModified(object sender, ArrowMouseArgs e)
    {
      if (fcFlowChart.Selection.Arrows.Count == 1) // We're playing with just one arrow...
      {
        PointCollection controlPoints = e.Arrow.ControlPoints.Clone();

        if (savedOrigin != null)
        {
          e.Arrow.Origin = savedOrigin;
          e.Arrow.OrgnAnchor = savedOriginAnchor;
        }

        if (savedDestination != null)
        {
          e.Arrow.Destination = savedDestination;
          e.Arrow.DestAnchor = savedDestinationAnchor;
        }

        e.Arrow.ControlPoints.Clear();
        foreach (PointF point in controlPoints)
        {
          e.Arrow.ControlPoints.Add(point);
        }
        e.Arrow.UpdateFromPoints();
      }
    }

    private void fcFlowChart_ArrowModifying(object sender, ArrowMouseArgs e)
    {
      if (fcFlowChart.Selection.Arrows.Count == 1) // We're playing with just one arrow...
      {
        savedOrigin = null;
        savedDestination = null;

        Box originBox = (e.Arrow.Origin as Box);
        if (originBox != null)
        {
          Item originItem = originBox.Tag as Item;
          if (originItem != null)
          {
            savedOrigin = originItem.Model;
            savedOriginAnchor = e.Arrow.OrgnAnchor;
          }
        }

        Box destinationBox = (e.Arrow.Destination as Box);
        if (destinationBox != null)
        {
          Item destinationItem = destinationBox.Tag as Item;
          if (destinationItem != null)
          {
            savedDestination = destinationItem.Model;
            savedDestinationAnchor = e.Arrow.DestAnchor;
          }
        }

        // Failed attempt to improve arrow manipulation behaviour.
        //PointF originBoxCenter = new PointF((originBox.BoundingRect.Left + originBox.BoundingRect.Right) / 2.0F,
        //                                    (originBox.BoundingRect.Top + originBox.BoundingRect.Bottom) / 2.0F);
        //SizeF originBoxOffset = new SizeF(Math.Abs(e.Arrow.ControlPoints[0].X - originBoxCenter.X),
        //                                 Math.Abs(e.Arrow.ControlPoints[0].Y - originBoxCenter.Y));
        //if (originBoxOffset.Width < originBoxOffset.Height)
        //{ // more below/above than to the side.
        //  //if (e.Arrow.PrpStartOrientation == MindFusion.FlowChartX.Orientation.Horizontal)
        //    e.Arrow.PrpStartOrientation = MindFusion.FlowChartX.Orientation.Vertical;
        //}
        //if (originBoxOffset.Width > originBoxOffset.Height)
        //{ // cmove to the side than above/below.
        //  //if (e.Arrow.PrpStartOrientation == MindFusion.FlowChartX.Orientation.Vertical)
        //    e.Arrow.PrpStartOrientation = MindFusion.FlowChartX.Orientation.Horizontal;
        //}
      }
    }

    private Item oldHoverItemBox = null;
    private Arrow oldHoverArrow = null;

    public void fcFlowChart_MouseMove(object sender, MouseEventArgs e)
    {
      //SuspendLayout();

      Box hoverBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)));
      Item hoverItemBox = null;
      if (hoverBox != null)
        hoverItemBox = hoverBox.Tag as Item;

      Arrow hoverArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)), 1);


      if (oldHoverItemBox != null) // deal with old itemBox.
      {
        if (oldHoverItemBox == hoverItemBox) // nothings changed.
          return;
        else // we've moved on, un-hover the old one.
        {
          oldHoverItemBox.Graphic.Visible = state.ShowGraphics;
          oldHoverItemBox.Graphic.ZBottom();
          oldHoverItemBox.Model.Visible = oldHoverItemBox.Model.Selected || state.ShowModels;
          oldHoverItemBox.Model.ZTop();

          foreach (Arrow arrow in oldHoverItemBox.IncomingArrows)
          {
            arrow.Visible = state.ShowLinks;
            arrow.ZTop();
          }

          foreach (Arrow arrow in oldHoverItemBox.OutgoingArrows)
          {
            arrow.Visible = state.ShowLinks;
            arrow.ZTop();
          }
        }
      }

      if (oldHoverArrow != null) // deal with old arrow.
      {
        if (oldHoverArrow == hoverArrow) // nothings changed...
          return;
        else
        {
          Box originBox = (oldHoverArrow.Origin as Box);
          if (originBox != null)
          {
            Item originItemBox = originBox.Tag as Item;
            if (originItemBox != null)
            {
              originItemBox.Graphic.Visible = state.ShowGraphics;
              originItemBox.Graphic.ZBottom();
              originItemBox.Model.Visible = originItemBox.Model.Selected || state.ShowModels;
              originItemBox.Model.ZTop();
            }
          }

          Box destinationBox = (oldHoverArrow.Destination as Box);
          if (destinationBox != null)
          {
            Item destinationItemBox = destinationBox.Tag as Item;
            if (destinationItemBox != null)
            {
              destinationItemBox.Graphic.Visible = state.ShowGraphics;
              destinationItemBox.Graphic.ZBottom();
              destinationItemBox.Model.Visible = destinationItemBox.Model.Selected || state.ShowModels;
              destinationItemBox.Model.ZTop();
            }
          }
        }

        oldHoverArrow.Visible = state.ShowLinks;
        oldHoverArrow.ZTop();
      }

      if (hoverItemBox != null)
      {
        hoverItemBox.Graphic.Visible = true;
        hoverItemBox.Graphic.ZBottom();
        hoverItemBox.Model.Visible = true;
        hoverItemBox.Model.ZTop();

        foreach (Arrow arrow in hoverItemBox.IncomingArrows)
        {
          arrow.Visible = true;
          arrow.ZTop();
        }

        foreach (Arrow arrow in hoverItemBox.OutgoingArrows)
        {
          arrow.Visible = true;
          arrow.ZTop();
        }
      }
      else if (hoverArrow != null)
      {
        Box originBox = (hoverArrow.Origin as Box);
        if (originBox != null)
        {
          Item originItemBox = originBox.Tag as Item;
          if (originItemBox != null)
          {
            originItemBox.Graphic.Visible = true;
            originItemBox.Graphic.ZBottom();
            originItemBox.Model.Visible = true;
            originItemBox.Model.ZTop();
          }
        }

        Box destinationBox = (hoverArrow.Destination as Box);
        if (destinationBox != null)
        {
          Item destinationItemBox = destinationBox.Tag as Item;
          if (destinationItemBox != null)
          {
            destinationItemBox.Graphic.Visible = true;
            destinationItemBox.Graphic.ZBottom();
            destinationItemBox.Model.Visible = true;
            destinationItemBox.Model.ZTop();
          }
        }

        hoverArrow.Visible = true;
        hoverArrow.ZTop();
      }

      oldHoverItemBox = hoverItemBox;
      oldHoverArrow = hoverArrow;

      //ResumeLayout(false);
    }

    private void fcFlowChart_DrawBox(object sender, BoxDrawArgs e)
    {
      foreach (Arrow arrow in e.Box.IncomingArrows)
      {
        float dx = 0.5F;
        float dy = 0.5F;

        if (arrow.DestAnchor != -1)
        {
          dx = e.Box.AnchorPattern.Points[arrow.DestAnchor].X / 100.0F;
          dy = e.Box.AnchorPattern.Points[arrow.DestAnchor].Y / 100.0F;
        }

        PointF anchorPoint = new PointF(
          e.Box.BoundingRect.Left + e.Box.BoundingRect.Width * dx,
          e.Box.BoundingRect.Top + e.Box.BoundingRect.Height * dy);

        PointF[] extensionPoints =
          new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPoint };

        System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

        e.Graphics.DrawLines(pen, extensionPoints);
      }

      foreach (Arrow arrow in e.Box.OutgoingArrows)
      {
        float dx = 0.5F;
        float dy = 0.5F;

        if (arrow.OrgnAnchor != -1)
        {
          dx = e.Box.AnchorPattern.Points[arrow.OrgnAnchor].X / 100.0F;
          dy = e.Box.AnchorPattern.Points[arrow.OrgnAnchor].Y / 100.0F;
        }

        PointF anchorPoint = new PointF(
          e.Box.BoundingRect.Left + e.Box.BoundingRect.Width * dx,
          e.Box.BoundingRect.Top + e.Box.BoundingRect.Height * dy);

        PointF[] extensionPoints =
          new PointF[] { arrow.ControlPoints[0], anchorPoint };

        System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

        e.Graphics.DrawLines(pen, extensionPoints);
      }
    }

    private void fcFlowChart_BoxModified(object sender, BoxMouseArgs e)
    {
      fcFlowChart_BoxModifying(sender, e);
    }

    private void fcFlowChart_BoxModifying(object sender, BoxMouseArgs e)
    {
      Box graphicBox = (e.Box.Tag as Item).Graphic;
      graphicBox.BoundingRect = (e.Box.Tag as Item).Model.BoundingRect;
      graphicBox.RotationAngle = (e.Box.Tag as Item).Model.RotationAngle;
    }

    public GraphicItem NewGraphicItem(GraphicItem graphicItem, string area)
    {
      return NewGraphicItem(graphicItem.BoundingRect, graphicItem.Model, graphicItem.Shape, 
        graphicItem.MirrorX, graphicItem.MirrorY, graphicItem.fillColor, area);
    }

    public GraphicItem NewGraphicItem(RectangleF rect, string model, string shape, bool mirrorX, bool mirrorY, Color fillColor, string area)
    {
      while (state.Exists("N_" + tempBoxKey.ToString()))
        tempBoxKey++;
      GraphicItem newGraphicItem = state.NewGraphicItem("N_" + tempBoxKey.ToString(), area);
      newGraphicItem.X = rect.X - rect.Width;
      newGraphicItem.Y = rect.Y - rect.Height;
      newGraphicItem.Width = rect.Width;
      newGraphicItem.Height = rect.Height;
      newGraphicItem.Model = model;
      newGraphicItem.Shape = shape;
      newGraphicItem.MirrorX = mirrorX;
      newGraphicItem.MirrorY = mirrorY;
      newGraphicItem.fillColor = fillColor;

      state.newItem(newGraphicItem, true, fcFlowChart);

      return newGraphicItem;
    }

    public GraphicLink NewGraphicLink(GraphicLink graphicLink, Arrow arrow, float dx, float dy)
    {
      while (state.Exists("A_" + tempBoxKey.ToString()))
        tempBoxKey++;

      GraphicLink newGraphicLink = new GraphicLink("A_" + tempBoxKey.ToString());
      newGraphicLink.Destination = graphicLink.Destination;
      newGraphicLink.Source = graphicLink.Source;

      foreach (PointF point in graphicLink.controlPoints)
      {
        newGraphicLink.controlPoints.Add(new PointF(point.X + dx, point.Y + dy));
      }

      if (arrow == null)
        arrow = fcFlowChart.CreateArrow(new PointF(0.0F, 0.0F), new PointF(10.0F, 10.0F));

      state.newGraphicLink(newGraphicLink, arrow, true);

      return newGraphicLink;
    }

    Box overBox;
    Arrow overArrow;

    private void fcFlowChart_Click(object sender, EventArgs e)
    {
      MouseEventArgs me = e as MouseEventArgs;
      overBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(me.Location));
      overArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(me.Location), 2);

      if (me.Button == MouseButtons.Left)
      {
        if ((overBox == null) && (overArrow == null)) // we're in free space...
        {
          if (fcFlowChart.Behavior == BehaviorType.CreateBox)
          {
            NewGraphicItem(new RectangleF(fcFlowChart.ClientToDoc(me.Location), 
              state.GraphicStencil(currentGraphicShape).defaultSize), 
              currentModelShape,
              currentGraphicShape,
              false,
              false,
              Color.LightBlue,
              state.CurrentArea);
          }
        }
      }
      if (me.Button == MouseButtons.Right)
      {
        if (overBox != null)
        {
          ContextMenu boxMenu = new ContextMenu();
          boxMenu.MenuItems.Add("Unfinished");
          boxMenu.Show(fcFlowChart, me.Location);
        }
        else if (overArrow != null)
        {
          ContextMenu arrowMenu = new ContextMenu();
          arrowMenu.MenuItems.Add("Route arrow", new EventHandler(RouteArrow));
          arrowMenu.Show(fcFlowChart, me.Location);
        }
      }
    }

    private void RouteArrow(object sender, EventArgs e)
    {
      overArrow.Route();
    }

    private void fcFlowChart_BoxDeleting(object sender, BoxConfirmArgs e)
    {
      e.Confirm = false;
      state.Remove(e.Box.Tag as Item, fcFlowChart);
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
      Box sourceBox = e.Arrow.Origin as Box;
      GraphicLink newGraphicLink = new GraphicLink(newLinkTag);

      if (destinationBox != null)
        newGraphicLink.Destination = destinationBox.Text;

      if (sourceBox != null)
        newGraphicLink.Source = sourceBox.Text;

      foreach (PointF point in e.Arrow.ControlPoints)
      {
        newGraphicLink.controlPoints.Add(point);
      }

      state.newGraphicLink(newGraphicLink, e.Arrow, true);
    }
  }
}