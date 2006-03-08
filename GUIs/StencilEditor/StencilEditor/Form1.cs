using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using MindFusion.FlowChartX;
using System.IO;
using SysCAD.Interface;
using System.Xml.Serialization;
using System.Runtime.Serialization.Formatters.Soap;

namespace StencilEditor
{
  public enum AnchorDirection {incoming, outgoing, bidirectional}

  public partial class Form1 : Form
  {
    LinkedList<Arrow> elementList;
    LinkedList<Arrow> decorationList;
    LinkedList<Arrow> textAreaList;

    LinkedList<Box> anchorPointList;

    PointF tagPosition;
    SizeF defaultSize;
    String groupName;

    float gridSize = 2.0F;

    Box canvas1, canvas2;
    Box tagPositionBox;
    Box example1, example2, example3;

    Color savedFrameColor;

    private ErrorProvider ep = new ErrorProvider();
    private Color originalBackColor;

    public Form1()
    {
      InitializeComponent();

      reset();
      Initialise();
    }

    private void reset()
    {
      originalBackColor = DefaultSizeXTextBox.BackColor;

      elementList = new LinkedList<Arrow>();
      decorationList = new LinkedList<Arrow>();
      textAreaList = new LinkedList<Arrow>();
      anchorPointList = new LinkedList<Box>();
      tagPosition = new PointF(50.0F, 50.0F);
      defaultSize = new SizeF(10.0F, 10.0F);
      groupName = "";

      flowChart1.ClearAll();
      flowChart2.ClearAll();

      flowChart1.DocExtents = new RectangleF(-25.0F, -25.0F, 200.0F, 150.0F);
      flowChart2.DocExtents = new RectangleF(-25.0F, -25.0F, 200.0F, 150.0F);

      canvas1 = flowChart1.CreateBox(0.0F, 0.0F, 100.0F, 100.0F);
      canvas1.ZBottom();
      canvas1.CustomDraw = CustomDraw.ShadowOnly;
      canvas1.FillColor = Color.FromArgb(30, Color.Black);
      canvas1.FrameColor = Color.FromArgb(50, Color.Black);
      canvas1.Style = BoxStyle.Shape;
      canvas1.Shape = ShapeTemplate.FromId("Rectangle");
      canvas1.Locked = true;

      canvas2 = flowChart2.CreateBox(0.0F, 0.0F, 100.0F, 100.0F);
      canvas2.ZBottom();
      canvas2.CustomDraw = CustomDraw.ShadowOnly;
      canvas2.FillColor = Color.FromArgb(30, Color.Black);
      canvas2.FrameColor = Color.FromArgb(50, Color.Black);
      canvas2.Style = BoxStyle.Shape;
      canvas2.Shape = ShapeTemplate.FromId("Rectangle");
      canvas2.Locked = true;

      tagPositionBox = flowChart1.CreateBox(tagPosition.X - (gridSize / 2.0F), tagPosition.Y - (gridSize / 2.0F), gridSize * 2.0F, gridSize * 2.0F);
      tagPositionBox.HandlesStyle = HandlesStyle.MoveOnly;
      tagPositionBox.Style = BoxStyle.Shape;
      tagPositionBox.Shape = ShapeTemplate.FromId("Or");
      tagPositionBox.CustomDraw = CustomDraw.ShadowOnly;

      textAreaList.Clear();
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 200.0F, tagPosition.Y - 0.0F), new PointF(tagPosition.X + 200.0F, tagPosition.Y - 0.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 200.0F, tagPosition.Y - 0.0F), new PointF(tagPosition.X + 200.0F, tagPosition.Y + 0.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 200.0F, tagPosition.Y + 0.0F), new PointF(tagPosition.X - 200.0F, tagPosition.Y + 0.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 200.0F, tagPosition.Y + 0.0F), new PointF(tagPosition.X - 200.0F, tagPosition.Y - 0.0F)));

      example1 = flowChart2.CreateBox(0.0F, 0.0F, 100.0F, 100.0F);
      example1.CustomDraw = CustomDraw.ShadowOnly;
      example1.Style = BoxStyle.Shape;
      example1.Text = "SAMPLETAG";
      example1.TextFormat.FormatFlags = StringFormatFlags.NoWrap;
      example1.Locked = true;

      example2 = flowChart2.CreateBox(110.0F, 5.0F, 50.0F, 50.0F);
      example2.CustomDraw = CustomDraw.ShadowOnly;
      example2.Style = BoxStyle.Shape;
      example2.Text = "SAMPLETAG";
      example2.TextFormat.FormatFlags = StringFormatFlags.NoWrap;
      example2.Locked = true;

      example3 = flowChart2.CreateBox(110.0F, 70.0F, 25.0F, 25.0F);
      example3.CustomDraw = CustomDraw.ShadowOnly;
      example3.Style = BoxStyle.Shape;
      example3.Text = "SAMPLETAG";
      example3.TextFormat.FormatFlags = StringFormatFlags.NoWrap;
      example3.Locked = true;

      savedFrameColor = example1.FrameColor;

      DefaultSizeXTextBox.Text = "10.0";
      DefaultSizeYTextBox.Text = "10.0";

      SetShape();

      flowChart1.GridSizeX = gridSize;
      flowChart1.GridSizeY = gridSize;
      flowChart1.GridStyle = MindFusion.FlowChartX.GridStyle.Lines;
      flowChart1.ShowGrid = true;

      flowChart1.ZoomToRect(new RectangleF(-25.0F, -25.0F, 200.0F, 150.0F));
      flowChart2.ZoomToRect(new RectangleF(-25.0F, -25.0F, 200.0F, 150.0F));
    }

    public void Initialise()
    {
      Box tl = flowChart1.CreateBox(0.0F + (gridSize / 2.0F), 0.0F + (gridSize / 2.0F), 0.000001F, 0.000001F);
      tl.Locked = true;
      Box tr = flowChart1.CreateBox(100.0F + (gridSize / 2.0F), 0.0F + (gridSize / 2.0F), 0.000001F, 0.000001F);
      tr.Locked = true;
      Box br = flowChart1.CreateBox(100.0F + (gridSize / 2.0F), 100.0F + (gridSize / 2.0F), 0.000001F, 0.000001F);
      br.Locked = true;
      Box bl = flowChart1.CreateBox(0.0F + (gridSize / 2.0F), 100.0F + (gridSize / 2.0F), 0.000001F, 0.000001F);
      bl.Locked = true;

      flowChart1.ArrowColor = Color.Red;

      elementList.AddLast(flowChart1.CreateArrow(tl, tr));
      elementList.AddLast(flowChart1.CreateArrow(tr, br));
      elementList.AddLast(flowChart1.CreateArrow(br, bl));
      elementList.AddLast(flowChart1.CreateArrow(bl, tl));
    }

    Node origin;
    Node destination;
    PointF mousePoint;

    private void flowChart1_ArrowModified(object sender, ArrowMouseArgs e)
    {
      if (elementList.Find(e.Arrow) != null)
      {
        if ((e.SelectionHandle == 0) || (e.SelectionHandle == e.Arrow.ControlPoints.Count - 1))
        {
          mousePoint = new PointF((float)(Math.Round((e.X + (gridSize / 2.0F)) / gridSize) * gridSize - (gridSize / 2.0F)),
                                  (float)(Math.Round((e.Y + (gridSize / 2.0F)) / gridSize) * gridSize - (gridSize / 2.0F)));

          PointCollection points = e.Arrow.ControlPoints.Clone();

          if (e.SelectionHandle == 0)
          {
            points.SetAt(0, mousePoint);

            origin.BoundingRect = new RectangleF(mousePoint, new SizeF(0.000001F, 0.000001F));
          }
          else if (e.SelectionHandle == e.Arrow.ControlPoints.Count - 1)
          {
            points.SetAt(points.Count - 1, mousePoint);

            destination.BoundingRect = new RectangleF(mousePoint, new SizeF(0.000001F, 0.000001F));
          }

          origin.BoundingRect = new RectangleF(e.Arrow.ControlPoints[0], new SizeF(0.000001F, 0.000001F));
          destination.BoundingRect = new RectangleF(e.Arrow.ControlPoints[e.Arrow.ControlPoints.Count - 1], new SizeF(0.000001F, 0.000001F));

          e.Arrow.Origin = origin;
          e.Arrow.Destination = destination;

          e.Arrow.ControlPoints.Clear();
          foreach (PointF point in points.getArray())
          {
            e.Arrow.ControlPoints.Add(point);
          }
          e.Arrow.UpdateFromPoints();
        }
      }
      SetShape();
    }

    private void flowChart1_ArrowModifying(object sender, ArrowMouseArgs e)
    {
      if (elementList.Find(e.Arrow)!=null)
      {
        if ((e.SelectionHandle == 0) || (e.SelectionHandle == e.Arrow.ControlPoints.Count - 1))
        {
          mousePoint = new PointF((float)(Math.Round((e.X + (gridSize / 2.0F)) / gridSize) * gridSize - (gridSize / 2.0F)),
                                  (float)(Math.Round((e.Y + (gridSize / 2.0F)) / gridSize) * gridSize - (gridSize / 2.0F)));

          origin = e.Arrow.Origin;
          destination = e.Arrow.Destination;

          PointCollection points = e.Arrow.ControlPoints.Clone();

          if (e.SelectionHandle == 0)
          {
            points.SetAt(0, mousePoint);

            origin.BoundingRect = new RectangleF(mousePoint, new SizeF(0.000001F, 0.000001F));
          }
          else if (e.SelectionHandle == e.Arrow.ControlPoints.Count - 1)
          {
            points.SetAt(points.Count - 1, mousePoint);

            destination.BoundingRect = new RectangleF(mousePoint, new SizeF(0.000001F, 0.000001F));
          }

          e.Arrow.Origin = origin;
          e.Arrow.Destination = destination;

          e.Arrow.ControlPoints.Clear();
          foreach (PointF point in points.getArray())
          {
            e.Arrow.ControlPoints.Add(point);
          }
          e.Arrow.UpdateFromPoints();
        }
      }
      SetShape();
    }

    public void SetShape()
    {
      DefaultSizeXTextBox.Enabled = graphicStencilButton.Checked;
      DefaultSizeYTextBox.Enabled = graphicStencilButton.Checked;

      ElementTemplate[] elements = new ElementTemplate[elementList.Count];
      LinkedListNode<Arrow> element = elementList.First;
      for (int i = 0; i < elementList.Count; i++)
      {
        Arrow arrow = element.Value;
        if (arrow.Style == ArrowStyle.Polyline)
        {
          elements[i] = new LineTemplate(arrow.ControlPoints[0].X - (gridSize / 2.0F), arrow.ControlPoints[0].Y - (gridSize / 2.0F),
                                         arrow.ControlPoints[1].X - (gridSize / 2.0F), arrow.ControlPoints[1].Y - (gridSize / 2.0F));
        }
        if (arrow.Style == ArrowStyle.Bezier)
        {
          elements[i] = new BezierTemplate(arrow.ControlPoints[0].X - (gridSize / 2.0F), arrow.ControlPoints[0].Y - (gridSize / 2.0F),
                                           arrow.ControlPoints[1].X - (gridSize / 2.0F), arrow.ControlPoints[1].Y - (gridSize / 2.0F),
                                           arrow.ControlPoints[2].X - (gridSize / 2.0F), arrow.ControlPoints[2].Y - (gridSize / 2.0F),
                                           arrow.ControlPoints[3].X - (gridSize / 2.0F), arrow.ControlPoints[3].Y - (gridSize / 2.0F));
        }
        element = element.Next;
      }

      ElementTemplate[] decorations = new ElementTemplate[decorationList.Count];
      LinkedListNode<Arrow> decoration = decorationList.First;
      for (int i = 0; i < decorationList.Count; i++)
      {
        Arrow arrow = decoration.Value;
        if (arrow.Style == ArrowStyle.Polyline)
        {
          decorations[i] = new LineTemplate(arrow.ControlPoints[0].X - (gridSize / 2.0F), arrow.ControlPoints[0].Y - 2.5F,
                                            arrow.ControlPoints[1].X - (gridSize / 2.0F), arrow.ControlPoints[1].Y - 2.5F);
        }
        if (arrow.Style == ArrowStyle.Bezier)
        {
          decorations[i] = new BezierTemplate(arrow.ControlPoints[0].X - (gridSize / 2.0F), arrow.ControlPoints[0].Y - (gridSize / 2.0F),
                                              arrow.ControlPoints[1].X - (gridSize / 2.0F), arrow.ControlPoints[1].Y - (gridSize / 2.0F),
                                              arrow.ControlPoints[2].X - (gridSize / 2.0F), arrow.ControlPoints[2].Y - (gridSize / 2.0F),
                                              arrow.ControlPoints[3].X - (gridSize / 2.0F), arrow.ControlPoints[3].Y - (gridSize / 2.0F));
        }
        decoration = decoration.Next;
      }

      ElementTemplate[] textAreas = new ElementTemplate[textAreaList.Count];
      LinkedListNode<Arrow> textArea = textAreaList.First;
      for (int i = 0; i < textAreaList.Count; i++)
      {
        Arrow arrow = textArea.Value;
        if (arrow.Style == ArrowStyle.Polyline)
        {
          textAreas[i] = new LineTemplate(arrow.ControlPoints[0].X - (gridSize / 2.0F), arrow.ControlPoints[0].Y - 2.5F,
                                            arrow.ControlPoints[1].X - (gridSize / 2.0F), arrow.ControlPoints[1].Y - 2.5F);
        }
        if (arrow.Style == ArrowStyle.Bezier)
        {
          textAreas[i] = new BezierTemplate(arrow.ControlPoints[0].X - (gridSize / 2.0F), arrow.ControlPoints[0].Y - (gridSize / 2.0F),
                                              arrow.ControlPoints[1].X - (gridSize / 2.0F), arrow.ControlPoints[1].Y - (gridSize / 2.0F),
                                              arrow.ControlPoints[2].X - (gridSize / 2.0F), arrow.ControlPoints[2].Y - (gridSize / 2.0F),
                                              arrow.ControlPoints[3].X - (gridSize / 2.0F), arrow.ControlPoints[3].Y - (gridSize / 2.0F));
        }
        textArea = textArea.Next;
      }

      if (modelStencilButton.Checked)
      {
        example1.FrameColor = example1.FillColor;
        example1.Text = "";
        example2.FrameColor = example2.FillColor;
        example2.Text = "";
        example3.FrameColor = example3.FillColor;
        example3.Text = "";
        tagPositionBox.Visible = false;
      }
      else if (graphicStencilButton.Checked)
      {
        example1.FrameColor = savedFrameColor;
        example1.Text = "SAMPLETAG";
        example2.FrameColor = savedFrameColor;
        example2.Text = "SAMPLETAG";
        example3.FrameColor = savedFrameColor;
        example3.Text = "SAMPLETAG";
        tagPositionBox.Visible = true;
      }

      ShapeTemplate shape = new ShapeTemplate(elements, decorations, textAreas, FillMode.Winding);

      example1.Shape = shape;
      example2.Shape = shape;
      example3.Shape = shape;

      foreach (Box box in anchorPointList)
      {
        box.Visible = modelStencilButton.Checked;
      }

      flowChart2.Refresh();
      flowChart1.Refresh();
    }

    Arrow contextArrow;
    Box contextBox;

    public float Distance(PointF p1, PointF p2)
    {
      return (float)(Math.Sqrt(Math.Pow(p2.X - p1.X, 2) + Math.Pow(p2.Y - p1.Y, 2)));
    }

    PointF mouseClickPos;

    private void flowChart1_MouseClick(object sender, MouseEventArgs e)
    {
      mouseClickPos = flowChart1.ClientToDoc(e.Location);

      if (e.Button == MouseButtons.Right)
      {
        ContextMenu contextMenu = new ContextMenu();

        contextArrow = flowChart1.GetArrowAt(mouseClickPos, 1.0F);
        contextBox = flowChart1.GetBoxAt(mouseClickPos);
        if (contextArrow is Arrow)
        {
          MenuItem fillAreaMenu = new MenuItem("FillArea...");
          fillAreaMenu.MenuItems.Add(new MenuItem("&Split", new EventHandler(SplitFillAreaLine)));
          fillAreaMenu.MenuItems.Add(new MenuItem("&Remove", new EventHandler(RemoveFillAreaLine)));
          if (contextArrow.Style == ArrowStyle.Bezier)
            fillAreaMenu.MenuItems.Add(new MenuItem("Convert to &Line", new EventHandler(ConvertToLine)));
          if (contextArrow.Style == ArrowStyle.Polyline)
            fillAreaMenu.MenuItems.Add(new MenuItem("Convert to &Bezier", new EventHandler(ConvertToBezier)));
          contextMenu.MenuItems.Add(fillAreaMenu);
        }

        MenuItem decorationMenu = new MenuItem("Decoration...");
        decorationMenu.MenuItems.Add(new MenuItem("Insert &Line", new EventHandler(InsertDecorationLine)));
        decorationMenu.MenuItems.Add(new MenuItem("Insert &Bezier", new EventHandler(InsertDecorationBezier)));
        if (contextArrow != null)
          decorationMenu.MenuItems.Add(new MenuItem("&Remove", new EventHandler(RemoveDecoration)));
        contextMenu.MenuItems.Add(decorationMenu);

        if (modelStencilButton.Checked)
        {
          MenuItem anchorMenu = new MenuItem("Anchor Points...");
          anchorMenu.MenuItems.Add(new MenuItem("Insert &In", new EventHandler(InsertAnchorPointIn)));
          anchorMenu.MenuItems.Add(new MenuItem("Insert &Out", new EventHandler(InsertAnchorPointOut)));
          anchorMenu.MenuItems.Add(new MenuItem("Insert &In/Out", new EventHandler(InsertAnchorPointInOut)));
          if (anchorPointList.Contains(contextBox))
          {
            anchorMenu.MenuItems.Add(new MenuItem("&Remove", new EventHandler(RemoveAnchorPoint)));
          }
          contextMenu.MenuItems.Add(anchorMenu);
        }

        contextMenu.Show(flowChart1, e.Location);
      }
    }

    public void SplitFillAreaLine(object sender, EventArgs e)
    {
      Box originBox = contextArrow.Origin as Box;
      Box destinationBox = contextArrow.Destination as Box;
      Box newBox = flowChart1.CreateBox(mouseClickPos.X,
                                        mouseClickPos.Y, 
                                        originBox.BoundingRect.Width, 
                                        originBox.BoundingRect.Height);

      contextArrow.Destination = newBox;

      flowChart1.ArrowStyle = contextArrow.Style;
      Arrow newArrow = flowChart1.CreateArrow(newBox, destinationBox);
      newArrow.FrameColor = Color.Red;
      
      elementList.AddAfter(elementList.Find(contextArrow), newArrow);

      ReshapeArrow(contextArrow, originBox.BoundingRect, newBox.BoundingRect);
      ReshapeArrow(newArrow, newBox.BoundingRect, destinationBox.BoundingRect);

      SetShape();
    }

    private void ReshapeArrow(Arrow contextArrow, RectangleF originRect, RectangleF destinationRect)
    {
      float srX = originRect.X;
      float srY = originRect.Y;

      float erX = destinationRect.X;
      float erY = destinationRect.Y;

      float drX = erX - srX;
      float drY = erY - srY;

      PointCollection points = contextArrow.ControlPoints.Clone();

      float saX = points[0].X;
      float saY = points[0].Y;

      float eaX = points[points.Count - 1].X;
      float eaY = points[points.Count - 1].Y;

      float daX = eaX - saX;
      float daY = eaY - saY;
      
      contextArrow.ControlPoints.Clear();
      foreach (PointF point in points.getArray())
      {
        PointF tempPoint = point;
        tempPoint.X = ((point.X - srX) * drX / daX) + srX;
        tempPoint.Y = ((point.Y - srY) * drY / daY) + srY;
        contextArrow.ControlPoints.Add(tempPoint);
      }
      contextArrow.UpdateFromPoints();
    }

    public void InsertAnchorPointIn(object sender, EventArgs e)
    {
      Box newBox = flowChart1.CreateBox(20.0F, 20.0F, gridSize * 2.0F, gridSize * 2.0F);
      newBox.HandlesStyle = HandlesStyle.MoveOnly;
      newBox.Style = BoxStyle.Shape;
      newBox.Shape = ShapeTemplate.FromId("Or");
      newBox.CustomDraw = CustomDraw.ShadowOnly;
      newBox.FillColor = Color.Blue;
      anchorPointList.AddLast(newBox);
      SetShape();
    }

    public void InsertAnchorPointOut(object sender, EventArgs e)
    {
      Box newBox = flowChart1.CreateBox(20.0F, 20.0F, gridSize * 2.0F, gridSize * 2.0F);
      newBox.HandlesStyle = HandlesStyle.MoveOnly;
      newBox.Style = BoxStyle.Shape;
      newBox.Shape = ShapeTemplate.FromId("Or");
      newBox.CustomDraw = CustomDraw.ShadowOnly;
      newBox.FillColor = Color.Green;
      anchorPointList.AddLast(newBox);
      SetShape();
    }

    public void InsertAnchorPointInOut(object sender, EventArgs e)
    {
      Box newBox = flowChart1.CreateBox(20.0F, 20.0F, gridSize * 2.0F, gridSize * 2.0F);
      newBox.HandlesStyle = HandlesStyle.MoveOnly;
      newBox.Style = BoxStyle.Shape;
      newBox.Shape = ShapeTemplate.FromId("Or");
      newBox.CustomDraw = CustomDraw.ShadowOnly;
      newBox.FillColor = Color.Yellow;
      anchorPointList.AddLast(newBox);
      SetShape();
    }

    public void RemoveAnchorPoint(object sender, EventArgs e)
    {
      anchorPointList.Remove(contextBox);
      flowChart1.DeleteObject(contextBox);
      SetShape();
    }

    public void RemoveFillAreaLine(object sender, EventArgs e)
    {
      LinkedListNode<Arrow> prevArrow = elementList.Find(contextArrow).Previous;
      LinkedListNode<Arrow> nextArrow = elementList.Find(contextArrow).Next;

      if (prevArrow == null)
        prevArrow = elementList.Last;
      if (nextArrow == null)
        nextArrow = elementList.First;

      nextArrow.Value.Origin = prevArrow.Value.Destination;

      flowChart1.DeleteObject(contextArrow);
      elementList.Remove(contextArrow);

      SetShape();
    }

    public void ConvertToLine(object sender, EventArgs e)
    {
      contextArrow.Style = ArrowStyle.Polyline;
      flowChart1.ActiveObject = contextArrow;
    }

    public void ConvertToBezier(object sender, EventArgs e)
    {
      contextArrow.Style = ArrowStyle.Bezier;
      flowChart1.ActiveObject = contextArrow;
    }

    public void InsertDecorationLine(object sender, EventArgs e)
    {
      flowChart1.ArrowStyle = ArrowStyle.Polyline;
      Arrow newArrow = flowChart1.CreateArrow(new PointF(10.0F, 10.0F), new PointF(20.0F, 20.0F));
      decorationList.AddLast(newArrow);
      newArrow.FrameColor = Color.FromKnownColor(KnownColor.Green);
      SetShape();
    }

    public void InsertDecorationBezier(object sender, EventArgs e)
    {
      flowChart1.ArrowStyle = ArrowStyle.Bezier;
      Arrow newArrow = flowChart1.CreateArrow(new PointF(10.0F, 10.0F), new PointF(20.0F, 20.0F));
      decorationList.AddLast(newArrow);
      newArrow.FrameColor = Color.FromKnownColor(KnownColor.Green);
      SetShape();
    }

    public void RemoveDecoration(object sender, EventArgs e)
    {
      flowChart1.DeleteObject(contextArrow);
      decorationList.Remove(contextArrow);

      SetShape();
    }

    private void toolStripButton1_Click(object sender, EventArgs e)
    {
      foreach (Arrow arrow in elementList)
      {
        arrow.Visible = showFillAreaLinesButton.Checked;
      }
    }

    private void toolStripButton2_Click(object sender, EventArgs e)
    {
      foreach (Arrow arrow in decorationList)
      {
        arrow.Visible = showDecorationLinesButton.Checked;
      }
    }

    private void toolStripButton4_Click(object sender, EventArgs e)
    {
      canvas1.Visible = showCanvasExtentsButton.Checked;
      canvas2.Visible = showCanvasExtentsButton.Checked;
    }

    private void flowChart1_ArrowDeleting(object sender, ArrowConfirmArgs e)
    {
      e.Confirm = false;
    }

    private void flowChart1_SelectionChanged(object sender, EventArgs e)
    {
      if (flowChart1.Selection.Objects.Count > 1)
        flowChart1.Selection.Clear();
    }

    private void NewStencilButton_Click(object sender, EventArgs e)
    {
      reset();
    }

    private void flowChart1_BoxDeleting(object sender, BoxConfirmArgs e)
    {
      e.Confirm = false;
    }

    private void flowChart1_BoxModified(object sender, BoxMouseArgs e)
    {
      tagPosition.X = (tagPositionBox.BoundingRect.Left + tagPositionBox.BoundingRect.Right) / 2.0F;
      tagPosition.Y = (tagPositionBox.BoundingRect.Top + tagPositionBox.BoundingRect.Bottom) / 2.0F;

      textAreaList.Clear();
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 200.0F, tagPosition.Y - 0.0F), new PointF(tagPosition.X + 200.0F, tagPosition.Y - 0.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 200.0F, tagPosition.Y - 0.0F), new PointF(tagPosition.X + 200.0F, tagPosition.Y + 0.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 200.0F, tagPosition.Y + 0.0F), new PointF(tagPosition.X - 200.0F, tagPosition.Y + 0.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 200.0F, tagPosition.Y + 0.0F), new PointF(tagPosition.X - 200.0F, tagPosition.Y - 0.0F)));
      
      SetShape();
    }

    private void loadStencilButton_Click(object sender, EventArgs e)
    {
      OpenFileDialog openDialog = new OpenFileDialog();
      openDialog.CheckPathExists = true;
      openDialog.CheckFileExists = true;
      openDialog.DereferenceLinks = true;
      if (modelStencilButton.Checked)
      {
        openDialog.Filter = "Graphic Stencil Files|*.modelstencil";
      }
      else if (graphicStencilButton.Checked)
      {
        openDialog.Filter = "Graphic Stencil Files|*.graphicstencil";
      }

      if (openDialog.ShowDialog() == DialogResult.OK)
      {
        elementList.Clear();
        decorationList.Clear();
        textAreaList.Clear();
        anchorPointList.Clear();

        reset();

        if (modelStencilButton.Checked)
        {
          Stream fileStream = openDialog.OpenFile();

          SoapFormatter sf = new SoapFormatter();
          ModelStencil stencil = (ModelStencil)sf.Deserialize(fileStream);

          fileStream.Close();

          Box origin;
          Box destination = new Box(flowChart1);
          Arrow firstArrow = new Arrow(flowChart1);

          foreach (Element element in stencil.elements)
          {
            flowChart1.ArrowColor = Color.Red;

            if (element == stencil.elements[0])
            { //First element, need to create origin and destination.
              if (element is Line)
              {
                origin = flowChart1.CreateBox((element as Line).x1, (element as Line).y1, 0.000001F, 0.000001F);
                destination = flowChart1.CreateBox((element as Line).x2, (element as Line).y2, 0.000001F, 0.000001F);

                firstArrow = flowChart1.CreateArrow(origin, destination);
                firstArrow.Style = ArrowStyle.Polyline;
                elementList.AddLast(firstArrow);
              }
              else if (element is Bezier)
              {
                origin = flowChart1.CreateBox((element as Bezier).x1, (element as Bezier).y1, 0.000001F, 0.000001F);
                destination = flowChart1.CreateBox((element as Bezier).x4, (element as Bezier).y4, 0.000001F, 0.000001F);

                firstArrow = flowChart1.CreateArrow(origin, destination);
                firstArrow.Style = ArrowStyle.Bezier;

                firstArrow.ControlPoints.SetAt(0, new PointF((element as Bezier).x1, (element as Bezier).y1));
                firstArrow.ControlPoints.SetAt(1, new PointF((element as Bezier).x2, (element as Bezier).y2));
                firstArrow.ControlPoints.SetAt(2, new PointF((element as Bezier).x3, (element as Bezier).y3));
                firstArrow.ControlPoints.SetAt(3, new PointF((element as Bezier).x4, (element as Bezier).y4));

                firstArrow.UpdateFromPoints();
              }
            }
            else if (element == stencil.elements[stencil.elements.Count-1])
            { // Last element, need to use origin from first element as destination.
              if (element is Line)
              {
                origin = destination;
                destination = firstArrow.Origin as Box;

                Arrow arrow = flowChart1.CreateArrow(origin, destination);
                arrow.Style = ArrowStyle.Polyline;
                elementList.AddLast(arrow);
              }
              else if (element is Bezier)
              {
                origin = destination;
                destination = firstArrow.Origin as Box;

                Arrow arrow = flowChart1.CreateArrow(origin, destination);
                arrow.Style = ArrowStyle.Bezier;
                elementList.AddLast(arrow);

                arrow.ControlPoints.SetAt(0, new PointF((element as Bezier).x1, (element as Bezier).y1));
                arrow.ControlPoints.SetAt(1, new PointF((element as Bezier).x2, (element as Bezier).y2));
                arrow.ControlPoints.SetAt(2, new PointF((element as Bezier).x3, (element as Bezier).y3));
                arrow.ControlPoints.SetAt(3, new PointF((element as Bezier).x4, (element as Bezier).y4));

                arrow.UpdateFromPoints();
              }
            }
            else
            {
              if (element is Line)
              {
                origin = destination;
                destination = flowChart1.CreateBox((element as Line).x2, (element as Line).y2, 0.000001F, 0.000001F);

                Arrow arrow = flowChart1.CreateArrow(origin, destination);
                arrow.Style = ArrowStyle.Polyline;
                elementList.AddLast(arrow);
              }
              else if (element is Bezier)
              {
                origin = destination;
                destination = flowChart1.CreateBox((element as Bezier).x4, (element as Bezier).y4, 0.000001F, 0.000001F);

                Arrow arrow = flowChart1.CreateArrow(origin, destination);
                arrow.Style = ArrowStyle.Bezier;
                elementList.AddLast(arrow);

                arrow.ControlPoints.SetAt(0, new PointF((element as Bezier).x1, (element as Bezier).y1));
                arrow.ControlPoints.SetAt(1, new PointF((element as Bezier).x2, (element as Bezier).y2));
                arrow.ControlPoints.SetAt(2, new PointF((element as Bezier).x3, (element as Bezier).y3));
                arrow.ControlPoints.SetAt(3, new PointF((element as Bezier).x4, (element as Bezier).y4));

                arrow.UpdateFromPoints();
              }
            }
          }

          foreach (Element element in stencil.decorations)
          {
            flowChart1.ArrowColor = Color.Green;

            if (element is Line)
            {
              Arrow arrow = flowChart1.CreateArrow(new PointF((element as Line).x1, (element as Line).y1),
                                                   new PointF((element as Line).x2, (element as Line).y2));
              arrow.Style = ArrowStyle.Polyline;
              decorationList.AddLast(arrow);
            }
            else if (element is Bezier)
            {
              Arrow arrow = flowChart1.CreateArrow(new PointF((element as Bezier).x1, (element as Bezier).y1), 
                                                   new PointF((element as Bezier).x4, (element as Bezier).y4));
              arrow.Style = ArrowStyle.Bezier;
              decorationList.AddLast(arrow);

              arrow.ControlPoints.SetAt(0, new PointF((element as Bezier).x1, (element as Bezier).y1));
              arrow.ControlPoints.SetAt(1, new PointF((element as Bezier).x2, (element as Bezier).y2));
              arrow.ControlPoints.SetAt(2, new PointF((element as Bezier).x3, (element as Bezier).y3));
              arrow.ControlPoints.SetAt(3, new PointF((element as Bezier).x4, (element as Bezier).y4));

              arrow.UpdateFromPoints();
            }
          }

          foreach (Anchor anchor in stencil.anchors)
          {
            Box newBox = flowChart1.CreateBox(anchor.position.X - gridSize, anchor.position.Y - gridSize, gridSize * 2.0F, gridSize * 2.0F);
            newBox.HandlesStyle = HandlesStyle.MoveOnly;
            newBox.Style = BoxStyle.Shape;
            newBox.Shape = ShapeTemplate.FromId("Or");
            newBox.CustomDraw = CustomDraw.ShadowOnly;
            if ((anchor.maxIn > 0) && (anchor.maxOut == 0))
              newBox.FillColor = Color.Blue;
            else if ((anchor.maxIn == 0) && (anchor.maxOut > 0))
              newBox.FillColor = Color.Green;
            else if ((anchor.maxIn > 0) && (anchor.maxOut > 0))
              newBox.FillColor = Color.Yellow;
            anchorPointList.AddLast(newBox);
            SetShape();
          }

          groupName = stencil.groupName;
        }
        else if (graphicStencilButton.Checked)
        {
          Stream fileStream = openDialog.OpenFile();

          SoapFormatter sf = new SoapFormatter();
          GraphicStencil stencil = (GraphicStencil)sf.Deserialize(fileStream);

          fileStream.Close();

          Box origin;
          Box destination = new Box(flowChart1);
          Arrow firstArrow = new Arrow(flowChart1);

          foreach (Element element in stencil.elements)
          {
            flowChart1.ArrowColor = Color.Red;

            if (element == stencil.elements[0])
            { //First element, need to create origin and destination.
              if (element is Line)
              {
                origin = flowChart1.CreateBox((element as Line).x1, (element as Line).y1, 0.000001F, 0.000001F);
                destination = flowChart1.CreateBox((element as Line).x2, (element as Line).y2, 0.000001F, 0.000001F);

                firstArrow = flowChart1.CreateArrow(origin, destination);
                firstArrow.Style = ArrowStyle.Polyline;
                elementList.AddLast(firstArrow);
              }
              else if (element is Bezier)
              {
                origin = flowChart1.CreateBox((element as Bezier).x1, (element as Bezier).y1, 0.000001F, 0.000001F);
                destination = flowChart1.CreateBox((element as Bezier).x4, (element as Bezier).y4, 0.000001F, 0.000001F);

                firstArrow = flowChart1.CreateArrow(origin, destination);
                firstArrow.Style = ArrowStyle.Bezier;

                firstArrow.ControlPoints.SetAt(0, new PointF((element as Bezier).x1, (element as Bezier).y1));
                firstArrow.ControlPoints.SetAt(1, new PointF((element as Bezier).x2, (element as Bezier).y2));
                firstArrow.ControlPoints.SetAt(2, new PointF((element as Bezier).x3, (element as Bezier).y3));
                firstArrow.ControlPoints.SetAt(3, new PointF((element as Bezier).x4, (element as Bezier).y4));

                firstArrow.UpdateFromPoints();
              }
            }
            else if (element == stencil.elements[stencil.elements.Count - 1])
            { // Last element, need to use origin from first element as destination.
              if (element is Line)
              {
                origin = destination;
                destination = firstArrow.Origin as Box;

                Arrow arrow = flowChart1.CreateArrow(origin, destination);
                arrow.Style = ArrowStyle.Polyline;
                elementList.AddLast(arrow);
              }
              else if (element is Bezier)
              {
                origin = destination;
                destination = firstArrow.Origin as Box;

                Arrow arrow = flowChart1.CreateArrow(origin, destination);
                arrow.Style = ArrowStyle.Bezier;
                elementList.AddLast(arrow);

                arrow.ControlPoints.SetAt(0, new PointF((element as Bezier).x1, (element as Bezier).y1));
                arrow.ControlPoints.SetAt(1, new PointF((element as Bezier).x2, (element as Bezier).y2));
                arrow.ControlPoints.SetAt(2, new PointF((element as Bezier).x3, (element as Bezier).y3));
                arrow.ControlPoints.SetAt(3, new PointF((element as Bezier).x4, (element as Bezier).y4));

                arrow.UpdateFromPoints();
              }
            }
            else
            {
              if (element is Line)
              {
                origin = destination;
                destination = flowChart1.CreateBox((element as Line).x2, (element as Line).y2, 0.000001F, 0.000001F);

                Arrow arrow = flowChart1.CreateArrow(origin, destination);
                arrow.Style = ArrowStyle.Polyline;
                elementList.AddLast(arrow);
              }
              else if (element is Bezier)
              {
                origin = destination;
                destination = flowChart1.CreateBox((element as Bezier).x4, (element as Bezier).y4, 0.000001F, 0.000001F);

                Arrow arrow = flowChart1.CreateArrow(origin, destination);
                arrow.Style = ArrowStyle.Bezier;
                elementList.AddLast(arrow);

                arrow.ControlPoints.SetAt(0, new PointF((element as Bezier).x1, (element as Bezier).y1));
                arrow.ControlPoints.SetAt(1, new PointF((element as Bezier).x2, (element as Bezier).y2));
                arrow.ControlPoints.SetAt(2, new PointF((element as Bezier).x3, (element as Bezier).y3));
                arrow.ControlPoints.SetAt(3, new PointF((element as Bezier).x4, (element as Bezier).y4));

                arrow.UpdateFromPoints();
              }
            }
          }

          foreach (Element element in stencil.decorations)
          {
            flowChart1.ArrowColor = Color.Green;

            if (element is Line)
            {
              Arrow arrow = flowChart1.CreateArrow(new PointF((element as Line).x1, (element as Line).y1),
                                                   new PointF((element as Line).x2, (element as Line).y2));
              arrow.Style = ArrowStyle.Polyline;
              decorationList.AddLast(arrow);
            }
            else if (element is Bezier)
            {
              Arrow arrow = flowChart1.CreateArrow(new PointF((element as Bezier).x1, (element as Bezier).y1),
                                                   new PointF((element as Bezier).x4, (element as Bezier).y4));
              arrow.Style = ArrowStyle.Bezier;
              decorationList.AddLast(arrow);

              arrow.ControlPoints.SetAt(0, new PointF((element as Bezier).x1, (element as Bezier).y1));
              arrow.ControlPoints.SetAt(1, new PointF((element as Bezier).x2, (element as Bezier).y2));
              arrow.ControlPoints.SetAt(2, new PointF((element as Bezier).x3, (element as Bezier).y3));
              arrow.ControlPoints.SetAt(3, new PointF((element as Bezier).x4, (element as Bezier).y4));

              arrow.UpdateFromPoints();
            }
          }

          groupName = stencil.groupName;

          float x = 0.0F;
          float y = 0.0F;
          uint count = 0;

          foreach (Element element in stencil.textAreas)
          {
            if (element is Line)
            {
              x += (element as Line).x1 + (element as Line).x2;
              y += (element as Line).x1 + (element as Line).x2;
            }
            else if (element is Bezier)
            {
              x += (element as Bezier).x1 + (element as Bezier).x2 + (element as Bezier).x3 + (element as Bezier).x4;
              y += (element as Bezier).x1 + (element as Bezier).x2 + (element as Bezier).x3 + (element as Bezier).x4;
            }
            count++;
          }
          x /= count;
          y /= count;

          tagPosition.X = x;
          tagPosition.Y = y;
          tagPositionBox.BoundingRect = new RectangleF(tagPosition.X - (gridSize / 2.0F), tagPosition.Y - (gridSize / 2.0F), gridSize * 2.0F, gridSize * 2.0F);

          textAreaList.Clear();
          textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 200.0F, tagPosition.Y - 0.0F), new PointF(tagPosition.X + 200.0F, tagPosition.Y - 0.0F)));
          textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 200.0F, tagPosition.Y - 0.0F), new PointF(tagPosition.X + 200.0F, tagPosition.Y + 0.0F)));
          textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 200.0F, tagPosition.Y + 0.0F), new PointF(tagPosition.X - 200.0F, tagPosition.Y + 0.0F)));
          textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 200.0F, tagPosition.Y + 0.0F), new PointF(tagPosition.X - 200.0F, tagPosition.Y - 0.0F)));
        }
      }
      SetShape();
    }

    private void saveStencilButton_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveDialog = new SaveFileDialog();
      saveDialog.AddExtension = true;
      saveDialog.CheckPathExists = true;
      saveDialog.CreatePrompt = true;
      if (modelStencilButton.Checked)
      {
        saveDialog.DefaultExt = "modelstencil";
        saveDialog.Filter = "Model Stencil Files|*.modelstencil";
      }
      else if (graphicStencilButton.Checked)
      {
        saveDialog.DefaultExt = "graphicstencil";
        saveDialog.Filter = "Graphic Stencil Files|*.graphicstencil";
      }

      if (saveDialog.ShowDialog() == DialogResult.OK)
      {
        if (modelStencilButton.Checked)
        {
          ModelStencil stencil = new ModelStencil();

          foreach (Arrow arrow in elementList)
          {
            if (arrow.Style == ArrowStyle.Polyline)
              stencil.elements.Add(new Line(arrow.ControlPoints.GetAt(0).X,
                                            arrow.ControlPoints.GetAt(0).Y,
                                            arrow.ControlPoints.GetAt(1).X,
                                            arrow.ControlPoints.GetAt(1).Y));
            else if (arrow.Style == ArrowStyle.Bezier)
              stencil.elements.Add(new Bezier(arrow.ControlPoints.GetAt(0).X,
                                              arrow.ControlPoints.GetAt(0).Y,
                                              arrow.ControlPoints.GetAt(1).X,
                                              arrow.ControlPoints.GetAt(1).Y,
                                              arrow.ControlPoints.GetAt(2).X,
                                              arrow.ControlPoints.GetAt(2).Y,
                                              arrow.ControlPoints.GetAt(3).X,
                                              arrow.ControlPoints.GetAt(3).Y));
          }

          foreach (Arrow arrow in decorationList)
          {
            if (arrow.Style == ArrowStyle.Polyline)
              stencil.decorations.Add(new Line(arrow.ControlPoints.GetAt(0).X,
                                               arrow.ControlPoints.GetAt(0).Y,
                                               arrow.ControlPoints.GetAt(1).X,
                                               arrow.ControlPoints.GetAt(1).Y));
            else if (arrow.Style == ArrowStyle.Bezier)
              stencil.decorations.Add(new Bezier(arrow.ControlPoints.GetAt(0).X,
                                                 arrow.ControlPoints.GetAt(0).Y,
                                                 arrow.ControlPoints.GetAt(1).X,
                                                 arrow.ControlPoints.GetAt(1).Y,
                                                 arrow.ControlPoints.GetAt(2).X,
                                                 arrow.ControlPoints.GetAt(2).Y,
                                                 arrow.ControlPoints.GetAt(3).X,
                                                 arrow.ControlPoints.GetAt(3).Y));
          }

          foreach (Box box in anchorPointList)
          {
            uint minIn = 0;
            uint maxIn = 0;
            uint minOut = 0;
            uint maxOut = 0;
            if (box.FillColor==Color.Blue)
            {
              maxIn = 1000;
            }
            else if (box.FillColor==Color.Green)
            {
              maxOut = 1000;
            }
            else if (box.FillColor==Color.Yellow)
            {
              maxIn = 1000;
              maxOut = 1000;
            }

            stencil.anchors.Add(new Anchor((box.BoundingRect.Left + box.BoundingRect.Right) / 2.0F,
                                               (box.BoundingRect.Top + box.BoundingRect.Bottom) / 2.0F,
                                               minIn, maxIn, minOut, maxOut));
          }

          stencil.groupName = groupName;
          
          Stream fileStream = saveDialog.OpenFile();

          SoapFormatter soapFormatter = new SoapFormatter();
          soapFormatter.Serialize(fileStream, stencil);

          fileStream.Close();
        }
        else if (graphicStencilButton.Checked)
        {
          GraphicStencil stencil = new GraphicStencil();

          foreach (Arrow arrow in elementList)
          {
            if (arrow.Style == ArrowStyle.Polyline)
              stencil.elements.Add(new Line(arrow.ControlPoints.GetAt(0).X,
                                            arrow.ControlPoints.GetAt(0).Y,
                                            arrow.ControlPoints.GetAt(1).X,
                                            arrow.ControlPoints.GetAt(1).Y));
            else if (arrow.Style == ArrowStyle.Bezier)
              stencil.elements.Add(new Bezier(arrow.ControlPoints.GetAt(0).X,
                                              arrow.ControlPoints.GetAt(0).Y,
                                              arrow.ControlPoints.GetAt(1).X,
                                              arrow.ControlPoints.GetAt(1).Y,
                                              arrow.ControlPoints.GetAt(2).X,
                                              arrow.ControlPoints.GetAt(2).Y,
                                              arrow.ControlPoints.GetAt(3).X,
                                              arrow.ControlPoints.GetAt(3).Y));
          }

          foreach (Arrow arrow in decorationList)
          {
            if (arrow.Style == ArrowStyle.Polyline)
              stencil.decorations.Add(new Line(arrow.ControlPoints.GetAt(0).X,
                                               arrow.ControlPoints.GetAt(0).Y,
                                               arrow.ControlPoints.GetAt(1).X,
                                               arrow.ControlPoints.GetAt(1).Y));
            else if (arrow.Style == ArrowStyle.Bezier)
              stencil.decorations.Add(new Bezier(arrow.ControlPoints.GetAt(0).X,
                                                 arrow.ControlPoints.GetAt(0).Y,
                                                 arrow.ControlPoints.GetAt(1).X,
                                                 arrow.ControlPoints.GetAt(1).Y,
                                                 arrow.ControlPoints.GetAt(2).X,
                                                 arrow.ControlPoints.GetAt(2).Y,
                                                 arrow.ControlPoints.GetAt(3).X,
                                                 arrow.ControlPoints.GetAt(3).Y));
          }

          foreach (Arrow arrow in textAreaList)
          {
            if (arrow.Style == ArrowStyle.Polyline)
              stencil.textAreas.Add(new Line(arrow.ControlPoints.GetAt(0).X,
                                               arrow.ControlPoints.GetAt(0).Y,
                                               arrow.ControlPoints.GetAt(1).X,
                                               arrow.ControlPoints.GetAt(1).Y));
            else if (arrow.Style == ArrowStyle.Bezier)
              stencil.textAreas.Add(new Bezier(arrow.ControlPoints.GetAt(0).X,
                                                 arrow.ControlPoints.GetAt(0).Y,
                                                 arrow.ControlPoints.GetAt(1).X,
                                                 arrow.ControlPoints.GetAt(1).Y,
                                                 arrow.ControlPoints.GetAt(2).X,
                                                 arrow.ControlPoints.GetAt(2).Y,
                                                 arrow.ControlPoints.GetAt(3).X,
                                                 arrow.ControlPoints.GetAt(3).Y));
          }

          stencil.groupName = groupName;
          stencil.defaultSize = defaultSize;

          Stream fileStream = saveDialog.OpenFile();

          SoapFormatter soapFormatter = new SoapFormatter();
          soapFormatter.Serialize(fileStream, stencil);

          fileStream.Close();
        }
      }
    }

    private void DefaultSizeXTextBox_Validating(object sender, CancelEventArgs e)
    {
      try
      {
        Double.Parse(DefaultSizeXTextBox.Text);
        DefaultSizeXTextBox.BackColor = originalBackColor;
        DefaultSizeXTextBox.ToolTipText = "";
      }
      catch (Exception)
      {
        e.Cancel = true;
        DefaultSizeXTextBox.BackColor = Color.Yellow;
        DefaultSizeXTextBox.ToolTipText = "Please enter numeric value.";
      }
    }

    private void Form1_Resize(object sender, EventArgs e)
    {
      flowChart1.ZoomToRect(new RectangleF(-25.0F, -25.0F, 150.0F, 150.0F));
      flowChart2.ZoomToRect(new RectangleF(-25.0F, -25.0F, 150.0F, 150.0F));
    }

    private void graphicStencilButton_Click(object sender, EventArgs e)
    {
      modelStencilButton.Checked = !graphicStencilButton.Checked;

      SetShape();
    }

    private void modelStencilButton_Click(object sender, EventArgs e)
    {
      graphicStencilButton.Checked = !modelStencilButton.Checked;

      SetShape();
    }
  }
}