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
    public BOD bod = new BOD();

    private PureComponents.TreeView.TreeView tvNavigation;

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

    internal void SetProject(Graphic graphic, Config config, PureComponents.TreeView.TreeView tvNavigation)
    {
      this.bod.graphic = graphic;
      this.bod.config = config;
      this.tvNavigation = tvNavigation;

      SuspendLayout();
      fcFlowChart.UndoManager.UndoEnabled = false;
      fcFlowChart.UseWaitCursor = true;
      fcFlowChart.ValidityChecks = false;
      fcFlowChart.Visible = false;
      fcFlowChart.Enabled = false;

      foreach (Item item in graphic.items.Values)
      {
        NewGraphicItem(item, null, false);
      }

      foreach (Link link in graphic.links.Values)
      {
        if (link.ClassID == "Pipe-1")
        {
          Arrow arrow = fcFlowChart.CreateArrow(new PointF(0.0F, 0.0F), new PointF(10.0F, 10.0F));
          bod.newLink(link, arrow, false);
        }
      }

      fcFlowChart.UndoManager.UndoEnabled = true;
      fcFlowChart.UseWaitCursor = false;
      fcFlowChart.ValidityChecks = true;
      fcFlowChart.Visible = true;
      fcFlowChart.Enabled = true;

      FixDocExtents();

      ResumeLayout(false);

      ZoomToVisible();
    }

    private void NewGraphicItem(Item item, Box graphicBox, bool isVisible)
    {
      Box modelBox = fcFlowChart.CreateBox(0.0F, 0.0F, 10.0F, 10.0F);
      if (graphicBox == null)
        graphicBox = fcFlowChart.CreateBox(0.0F, 0.0F, 10.0F, 10.0F);
      bod.newThing(item, modelBox, graphicBox, isVisible, fcFlowChart);
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
          BODThing originThing = originBox.Tag as BODThing;
          if (originThing != null)
          {
            savedOrigin = originThing.Model;
            savedOriginAnchor = e.Arrow.OrgnAnchor;
          }
        }

        Box destinationBox = (e.Arrow.Destination as Box);
        if (destinationBox != null)
        {
          BODThing destinationThing = destinationBox.Tag as BODThing;
          if (destinationThing != null)
          {
            savedDestination = destinationThing.Model;
            savedDestinationAnchor = e.Arrow.DestAnchor;
          }
        }
      }
    }

    private BODThing oldHoverItemBox = null;
    private Arrow oldHoverArrow = null;

    public void fcFlowChart_MouseMove(object sender, MouseEventArgs e)
    {
      SuspendLayout();

      Box hoverBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)));
      BODThing hoverItemBox = null;
      if (hoverBox != null)
        hoverItemBox = hoverBox.Tag as BODThing;

      Arrow hoverArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)), 1);


      if (oldHoverItemBox != null) // deal with old itemBox.
      {
        if (oldHoverItemBox == hoverItemBox) // nothings changed.
          return;
        else // we've moved on, un-hover the old one.
        {
          oldHoverItemBox.Graphic.ZBottom();
          oldHoverItemBox.Graphic.Visible = bod.ShowGraphics;
          oldHoverItemBox.Model.ZTop();
          oldHoverItemBox.Model.Visible = oldHoverItemBox.Model.Selected || bod.ShowModels;

          foreach (Arrow arrow in oldHoverItemBox.IncomingArrows)
          {
            arrow.ZTop();
            arrow.Visible = bod.ShowLinks;
          }

          foreach (Arrow arrow in oldHoverItemBox.OutgoingArrows)
          {
            arrow.ZTop();
            arrow.Visible = bod.ShowLinks;
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
            BODThing originItemBox = originBox.Tag as BODThing;
            if (originItemBox != null)
            {
              originItemBox.Graphic.ZBottom();
              originItemBox.Graphic.Visible = bod.ShowGraphics;
              originItemBox.Model.ZTop();
              originItemBox.Model.Visible = originItemBox.Model.Selected || bod.ShowModels;
            }
          }

          Box destinationBox = (oldHoverArrow.Destination as Box);
          if (destinationBox != null)
          {
            BODThing destinationItemBox = destinationBox.Tag as BODThing;
            if (destinationItemBox != null)
            {
              destinationItemBox.Graphic.ZBottom();
              destinationItemBox.Graphic.Visible = bod.ShowGraphics;
              destinationItemBox.Model.ZTop();
              destinationItemBox.Model.Visible = destinationItemBox.Model.Selected || bod.ShowModels;
            }
          }
        }

        oldHoverArrow.ZTop();
        oldHoverArrow.Visible = bod.ShowLinks;
      }

      if (hoverItemBox != null)
      {
        hoverItemBox.Graphic.ZBottom();
        hoverItemBox.Graphic.Visible = true;
        hoverItemBox.Model.ZTop();
        hoverItemBox.Model.Visible = true;

        foreach (Arrow arrow in hoverItemBox.IncomingArrows)
        {
          arrow.ZTop();
          arrow.Visible = true;
        }

        foreach (Arrow arrow in hoverItemBox.OutgoingArrows)
        {
          arrow.ZTop();
          arrow.Visible = true;
        }
      }
      else if (hoverArrow != null)
      {
        Box originBox = (hoverArrow.Origin as Box);
        if (originBox != null)
        {
          BODThing originItemBox = originBox.Tag as BODThing;
          if (originItemBox != null)
          {
            originItemBox.Graphic.ZBottom();
            originItemBox.Graphic.Visible = true;
            originItemBox.Model.ZTop();
            originItemBox.Model.Visible = true;
          }
        }

        Box destinationBox = (hoverArrow.Destination as Box);
        if (destinationBox != null)
        {
          BODThing destinationItemBox = destinationBox.Tag as BODThing;
          if (destinationItemBox != null)
          {
            destinationItemBox.Graphic.ZBottom();
            destinationItemBox.Graphic.Visible = true;
            destinationItemBox.Model.ZTop();
            destinationItemBox.Model.Visible = true;
          }
        }

        hoverArrow.ZTop();
        hoverArrow.Visible = true;
      }

      oldHoverItemBox = hoverItemBox;
      oldHoverArrow = hoverArrow;

      ResumeLayout(false);
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
      Box graphicBox = (e.Box.Tag as BODThing).Graphic;
      graphicBox.RotationAngle = (e.Box.Tag as BODThing).Model.RotationAngle;
    }

    private void fcFlowChart_BoxModifying(object sender, BoxMouseArgs e)
    {
      Box graphicBox = (e.Box.Tag as BODThing).Graphic;
      graphicBox.RotationAngle = (e.Box.Tag as BODThing).Model.RotationAngle;
    }

    private void fcFlowChart_BoxCreated(object sender, BoxEventArgs e)
    {
    }

    private void NewItem(RectangleF rect, Box box)
    {
      while (bod.Exists("N_" + tempBoxKey.ToString()))
        tempBoxKey++;
      Item newItem = new Item("N_" + tempBoxKey.ToString());
      bod.graphic.items.Add("N_" + tempBoxKey.ToString(), newItem);
      newItem.X = rect.X - rect.Width;
      newItem.Y = rect.Y - rect.Height;
      newItem.Width = rect.Width*2.0F;
      newItem.Height = rect.Height*2.0F;
      newItem.Model = currentModelShape;
      newItem.Shape = currentGraphicShape;

      tvNavigation.Nodes.Add("N_" + tempBoxKey.ToString(), "N_" + tempBoxKey.ToString());

      NewGraphicItem(newItem, box, true);

      tvNavigation.GetNodeByKey("N_" + tempBoxKey.ToString()).Checked = true;
    }

    private void fcFlowChart_Click(object sender, EventArgs e)
    {
      MouseEventArgs me = e as MouseEventArgs;
      Box overBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(me.Location));
      Arrow overArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(me.Location), 1);

      if ((overBox == null) && (overArrow == null)) // we're in free space...
      {
        if (fcFlowChart.Behavior == BehaviorType.CreateBox)
        {
          NewItem(new RectangleF(fcFlowChart.ClientToDoc(me.Location), bod.config.graphicStencils[currentGraphicShape].defaultSize), null);          
        }
      }
    }

    private void FrmFlowChart_Resize(object sender, EventArgs e)
    {
      SuspendLayout();
      fcFlowChart.Size = Size;
      fcFlowChart.Refresh();
      ResumeLayout();
    }

    private void fcFlowChart_BoxDeleted(object sender, BoxEventArgs e)
    {
      bod.Remove(e.Box.Tag as BODThing, fcFlowChart);
    }

    private void fcFlowChart_ArrowDeleted(object sender, ArrowEventArgs e)
    {

    }

    private void fcFlowChart_ArrowCreated(object sender, ArrowEventArgs e)
    {
      tempArrowKey++;
      string newLinkTag = "A_" + tempArrowKey.ToString();
      e.Arrow.Text = newLinkTag;

      Box destinationBox = e.Arrow.Destination as Box;
      Box sourceBox = e.Arrow.Origin as Box;
      Link newLink = new Link(newLinkTag);

      if (destinationBox != null)
        newLink.Destination = destinationBox.Text;

      if (sourceBox != null)
        newLink.Origin = sourceBox.Text;

      foreach (PointF point in e.Arrow.ControlPoints)
      {
        newLink.controlPoints.Add(point);
      }

      bod.newLink(newLink, e.Arrow, true);
    }

    private void fcFlowChart_BoxCreating(object sender, BoxConfirmArgs e)
    {
    }
  }
}