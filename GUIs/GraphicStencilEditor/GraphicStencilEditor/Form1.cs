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
  public partial class Form1 : Form
  {
    LinkedList<Arrow> elementList;
    LinkedList<Arrow> decorationList;
    LinkedList<Arrow> textAreaList;

    LinkedList<Box> anchorPoints;

    PointF tagPosition;
    SizeF defaultSize;
    String groupName;

    float gridSize = 2.0F;

    Box canvas1, canvas2;
    Box tagPositionBox;
    Box example;

    private ErrorProvider ep = new ErrorProvider();
    private Color originalBackColor;

    public Form1()
    {
      InitializeComponent();

      reset();
    }

    private void reset()
    {
      originalBackColor = DefaultSizeXTextBox.BackColor;

      elementList = new LinkedList<Arrow>();
      decorationList = new LinkedList<Arrow>();
      textAreaList = new LinkedList<Arrow>();
      anchorPoints = new LinkedList<Box>();
      tagPosition = new PointF(50.0F, 50.0F);
      defaultSize = new SizeF(10.0F, 10.0F);
      groupName = "";

      flowChart1.ClearAll();
      flowChart2.ClearAll();

      flowChart1.DocExtents = new RectangleF(-25.0F, -25.0F, 150.0F, 150.0F);
      flowChart2.DocExtents = new RectangleF(-25.0F, -25.0F, 150.0F, 150.0F);

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
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 50.0F, tagPosition.Y - 2.0F), new PointF(tagPosition.X + 50.0F, tagPosition.Y - 2.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 50.0F, tagPosition.Y - 2.0F), new PointF(tagPosition.X + 50.0F, tagPosition.Y + 2.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 50.0F, tagPosition.Y + 2.0F), new PointF(tagPosition.X - 50.0F, tagPosition.Y + 2.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 50.0F, tagPosition.Y + 2.0F), new PointF(tagPosition.X - 50.0F, tagPosition.Y - 2.0F)));

      example = flowChart2.CreateBox(0.0F, 0.0F, 100.0F, 100.0F);
      example.CustomDraw = CustomDraw.ShadowOnly;
      //example.FillColor = Color.FromArgb(100, Color.Blue);
      //example.FrameColor = Color.FromArgb(150, example.FrameColor);
      example.Style = BoxStyle.Shape;
      example.Text = "Sample Tag";
      example.Locked = true;

      Box tl = flowChart1.CreateBox(0.0F + (gridSize / 2.0F), 0.0F + (gridSize / 2.0F), 0.000001F, 0.000001F);
      tl.Locked = true;
      Box tr = flowChart1.CreateBox(100.0F + (gridSize / 2.0F), 0.0F + (gridSize / 2.0F), 0.000001F, 0.000001F);
      tr.Locked = true;
      Box br = flowChart1.CreateBox(100.0F + (gridSize / 2.0F), 100.0F + (gridSize / 2.0F), 0.000001F, 0.000001F);
      br.Locked = true;
      Box bl = flowChart1.CreateBox(0.0F + (gridSize / 2.0F), 100.0F + (gridSize / 2.0F), 0.000001F, 0.000001F);
      bl.Locked = true;

      flowChart1.ArrowColor = Color.FromKnownColor(KnownColor.Red);

      elementList.AddLast(flowChart1.CreateArrow(tl, tr));
      elementList.AddLast(flowChart1.CreateArrow(tr, br));
      elementList.AddLast(flowChart1.CreateArrow(br, bl));
      elementList.AddLast(flowChart1.CreateArrow(bl, tl));

      DefaultSizeXTextBox.Text = "10.0";
      DefaultSizeYTextBox.Text = "10.0";

      SetShape();

      flowChart1.GridSizeX = gridSize;
      flowChart1.GridSizeY = gridSize;
      flowChart1.GridStyle = MindFusion.FlowChartX.GridStyle.Lines;
      flowChart1.ShowGrid = true;

      flowChart1.ZoomToRect(new RectangleF(-25.0F, -25.0F, 150.0F, 150.0F));
      flowChart2.ZoomToRect(new RectangleF(-25.0F, -25.0F, 150.0F, 150.0F));
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

      ShapeTemplate shape = new ShapeTemplate(elements, decorations, textAreas, FillMode.Winding);

      example.Shape = shape;
      flowChart2.Refresh();
      flowChart1.Refresh();
    }

    Arrow contextArrow;

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
      newArrow.FrameColor = Color.FromKnownColor(KnownColor.Red);
      
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
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 50.0F, tagPosition.Y - 2.0F), new PointF(tagPosition.X + 50.0F, tagPosition.Y - 2.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 50.0F, tagPosition.Y - 2.0F), new PointF(tagPosition.X + 50.0F, tagPosition.Y + 2.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X + 50.0F, tagPosition.Y + 2.0F), new PointF(tagPosition.X - 50.0F, tagPosition.Y + 2.0F)));
      textAreaList.AddLast(new Arrow(flowChart1, new PointF(tagPosition.X - 50.0F, tagPosition.Y + 2.0F), new PointF(tagPosition.X - 50.0F, tagPosition.Y - 2.0F)));
      
      SetShape();
    }

    private void loadStencilButton_Click(object sender, EventArgs e)
    {
      OpenFileDialog openDialog = new OpenFileDialog();
      openDialog.CheckPathExists = true;
      openDialog.CheckFileExists = true;
      openDialog.DereferenceLinks = true;
      openDialog.Filter = "Graphic Stencil Files|*.graphicstencil";

      if (openDialog.ShowDialog() == DialogResult.OK)
      {
        XmlSerializer ser = new XmlSerializer(typeof(GraphicStencil));
        GraphicStencil stencil = (GraphicStencil)ser.Deserialize(openDialog.OpenFile());
      }
    }

    private void saveStencilButton_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveDialog = new SaveFileDialog();
      saveDialog.AddExtension = true;
      saveDialog.DefaultExt = "graphicstencil";
      saveDialog.CheckPathExists = true;
      saveDialog.CreatePrompt = true;
      saveDialog.Filter = "Graphic Stencil Files|*.graphicstencil";

      if (saveDialog.ShowDialog() == DialogResult.OK)
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
            stencil.textArea.Add(new Line(arrow.ControlPoints.GetAt(0).X,
                                             arrow.ControlPoints.GetAt(0).Y,
                                             arrow.ControlPoints.GetAt(1).X,
                                             arrow.ControlPoints.GetAt(1).Y));
          else if (arrow.Style == ArrowStyle.Bezier)
            stencil.textArea.Add(new Bezier(arrow.ControlPoints.GetAt(0).X,
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
  }
}