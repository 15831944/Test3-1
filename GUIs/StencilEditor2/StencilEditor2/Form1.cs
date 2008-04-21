using System;
using System.IO;
using System.Windows.Forms;
using System.Runtime.Serialization.Formatters.Soap;
using SysCAD.Protocol;
using MindFusion.FlowChartX;
using System.Drawing;
using System.Collections;

namespace StencilEditor2
{
  public partial class Form1 : Form
  {
    const int previewCount = 8;
    Preview[] previews = new Preview[previewCount];
    ModelStencil modelStencil;
    Box modelBox;
    RectangleF visibleRect = new RectangleF(-10.0F, -10.0F, 120.0F, 120.0F);

    String path = "D:\\SysCAD\\PKH\\BaseFiles\\Stencils\\";

    ChartObject contextObject;
    PointF contextPoint;

    public Form1()
    {
      InitializeComponent();

      this.flowChart1.DocExtents = new RectangleF(-10.0F, -10.0F, 120.0F, 120.0F);
      ZoomToRect(visibleRect);

      int previewY = 0;
      for (int i = 0; i < previewCount; i++)
      {
        int height = (int)(400.0 / Math.Pow(2.0, (i + 1.0)));
        previews[i] = new Preview(this, showPreviewToolStripMenuItem, height, previewY);
        previewY += height + 2;

        previews[i].Owner = this;
      }

      UpdatePreviews();

      ResetFlowChart(true);
    }

    private void ResetFlowChart(bool clearAll)
    {
      if (clearAll)
      {
        flowChart1.ClearAll();
      }
      else
      {
        foreach (ChartObject chartObject in flowChart1.Objects)
        {
          chartObject.Visible = false;
        }
      }

      modelBox = flowChart1.CreateBox(0.0F + flowChart1.GridSizeX / 2.0F, 0.0F + flowChart1.GridSizeY / 2.0F, 100.0F, 100.0F);
      modelBox.Visible = false;
      modelBox.RotationAngle = 0.0F;
      modelBox.Style = BoxStyle.Shape;
      modelBox.FillColor = System.Drawing.Color.FromArgb(55, 222, 184, 136);
      modelBox.FrameColor = System.Drawing.Color.FromArgb(61, 111, 92, 68);
      modelBox.ZBottom();


      modelBox.EnabledHandles = Handles.None;
      modelBox.HandlesStyle = HandlesStyle.Invisible;

      modelBox.Locked = true;

      Box backBox = flowChart1.CreateBox(0.0F + flowChart1.GridSizeX / 2.0F, 0.0F + flowChart1.GridSizeY / 2.0F, 100.0F, 100.0F);
      backBox.Style = BoxStyle.Rectangle;
      backBox.FillColor = System.Drawing.Color.FloralWhite;
      backBox.FrameColor = System.Drawing.Color.LightSlateGray;
      backBox.Locked = true;
      backBox.ZBottom();
    }

    private void Form1_VisibleChanged(object sender, EventArgs e)
    {
      UpdatePreviews();
    }

    private void UpdatePreviews()
    {
      foreach (Preview preview in previews)
        preview.UpdatePreview();
    }

    private void modelStencilToolStripMenuItem_CheckStateChanged(object sender, EventArgs e)
    {
      if (modelStencilToolStripMenuItem.CheckState == CheckState.Checked)
        graphicStencilToolStripMenuItem.CheckState = CheckState.Unchecked;

      else if (modelStencilToolStripMenuItem.CheckState == CheckState.Unchecked)
        graphicStencilToolStripMenuItem.CheckState = CheckState.Checked;

      else if (modelStencilToolStripMenuItem.CheckState == CheckState.Indeterminate)
        graphicStencilToolStripMenuItem.CheckState = CheckState.Indeterminate;
    }

    private void graphicStencilToolStripMenuItem_CheckStateChanged(object sender, EventArgs e)
    {
      if (graphicStencilToolStripMenuItem.CheckState == CheckState.Checked)
        modelStencilToolStripMenuItem.CheckState = CheckState.Unchecked;

      else if (graphicStencilToolStripMenuItem.CheckState == CheckState.Unchecked)
        modelStencilToolStripMenuItem.CheckState = CheckState.Checked;

      else if (graphicStencilToolStripMenuItem.CheckState == CheckState.Indeterminate)
        modelStencilToolStripMenuItem.CheckState = CheckState.Indeterminate;
    }

    private void openToolStripMenuItem_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog = new OpenFileDialog();
      openFileDialog.AddExtension = true;
      openFileDialog.CheckFileExists = true;
      openFileDialog.CheckPathExists = true;
      openFileDialog.DefaultExt = "ModelStencil";
      openFileDialog.Filter = "ModelStencils|*.ModelStencil";
      openFileDialog.Multiselect = false;
      openFileDialog.ShowHelp = false;
      openFileDialog.ShowReadOnly = false;
      openFileDialog.SupportMultiDottedExtensions = true;
      openFileDialog.FileName = System.IO.Path.GetFileName(path);
      openFileDialog.Title = "Open ModelStencil";

      openFileDialog.InitialDirectory = System.IO.Path.GetFullPath(path);

      if (openFileDialog.ShowDialog(this) == DialogResult.OK)
      {
        modelStencil = OpenModelStencil(openFileDialog.FileName);

        editPropertiesToolStripMenuItem.Enabled = true;
        saveToolStripMenuItem.Enabled = true;

        ResetFlowChart(true);
        SetStencil(modelStencil);
        SetControls(modelStencil);
      }
      path = openFileDialog.FileName;
    }

    private void SetControls(ModelStencil modelStencil)
    {
      foreach (Element element in modelStencil.Decorations)
      {
        if (element is Line)
        {
          Line line = element as Line;
          Arrow arrow = flowChart1.CreateArrow(GridFix(line.pointF1), GridFix(line.pointF2));
          arrow.FrameColor = Color.LightSkyBlue;
          arrow.ZTop();
          arrow.Tag = line;
        }

        if (element is Bezier)
        {
          Bezier bezier = element as Bezier;
          Arrow arrow = flowChart1.CreateArrow(bezier.pointF1, bezier.pointF4);
          arrow.FrameColor = Color.LightSkyBlue;
          arrow.SegmentCount = 3;
          arrow.ControlPoints[0] = GridFix(bezier.pointF1);
          arrow.ControlPoints[1] = GridFix(bezier.pointF2);
          arrow.ControlPoints[2] = GridFix(bezier.pointF3);
          arrow.ControlPoints[3] = GridFix(bezier.pointF4);
          arrow.ZTop();
          arrow.Tag = bezier;
        }

        if (element is Arc)
        {
          Arc arc = element as Arc;
          Box box = flowChart1.CreateBox(GridFix(arc.x), GridFix(arc.y), (float)arc.w, (float)arc.h);
          box.Style = BoxStyle.Ellipse;
          box.FillColor = Color.Transparent;
          box.FrameColor = Color.LightSkyBlue;
          box.ZBottom();
          box.Tag = arc;
        }
      }

      foreach (Element element in modelStencil.Elements)
      {
        if (element is Line)
        {
          Line line = element as Line;
          Arrow arrow = flowChart1.CreateArrow(GridFix(line.pointF1), GridFix(line.pointF2));
          arrow.FrameColor = Color.LightGreen;
          arrow.ZTop();
          arrow.Tag = line;
        }

        if (element is Bezier)
        {
          Bezier bezier = element as Bezier;
          Arrow arrow = flowChart1.CreateArrow(bezier.pointF1, bezier.pointF4);
          arrow.FrameColor = Color.LightGreen;
          arrow.SegmentCount = 3;
          arrow.ControlPoints[0] = GridFix(bezier.pointF1);
          arrow.ControlPoints[1] = GridFix(bezier.pointF2);
          arrow.ControlPoints[2] = GridFix(bezier.pointF3);
          arrow.ControlPoints[3] = GridFix(bezier.pointF4);
          arrow.ZTop();
          arrow.Tag = bezier;
        }

        if (element is Arc)
        {
          Arc arc = element as Arc;
          Box box = flowChart1.CreateBox(GridFix(arc.x), GridFix(arc.y), (float)arc.w, (float)arc.h);
          box.Style = BoxStyle.Ellipse;
          box.FillColor = Color.Transparent;
          box.FrameColor = Color.LightGreen;
          box.ZBottom();
          box.Tag = arc;
        }
      }

      foreach (Anchor anchor in modelStencil.Anchors)
      {
        foreach (SysCAD.Protocol.Point point in anchor.Positions)
        {
          Box box = flowChart1.CreateBox(
            GridFix(point.X) - 1.0F, GridFix(point.Y) - 1.0F, 2.0F, 2.0F);
          box.Style = BoxStyle.Rhombus;
          box.FillColor = Color.LightSalmon;
          box.FrameColor = Color.Salmon;
          box.HandlesStyle = HandlesStyle.MoveOnly;
          box.ToolTip = anchor.Tag + " : " + anchor.Look;
          box.ZTop();
          box.Tag = point;
        }
      }
    }

    private float GridFix(double pt)
    {
      return (float)pt + flowChart1.GridSizeX / 2.0F;
    }

    private PointF GridFix(PointF pointF)
    {
      return new PointF(pointF.X + flowChart1.GridSizeX / 2.0F, pointF.Y + flowChart1.GridSizeY / 2.0F);
    }

    private void SetStencil(ModelStencil modelStencil)
    {
      toolStripStatusLabel1.Text = "";

      modelBox.Shape = GetShapeTemplate(modelStencil);
      modelBox.Visible = true;

      foreach (Preview preview in previews)
        preview.SetStencil(GetShapeTemplate(modelStencil));

      flowChart1.Enabled = true;
    }


    private ShapeTemplate GetShapeTemplate(SysCAD.Protocol.ModelStencil stencil)
    {
      int i;

      if (stencil != null)
      {
        ElementTemplate[] elementTemplate = new ElementTemplate[stencil.Elements.Count];
        i = 0;

        foreach (Element element in stencil.Elements)
        {
          elementTemplate[i] = Element(element);
          i++;
        }

        ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.Decorations.Count];
        i = 0;

        foreach (Element decoration in stencil.Decorations)
        {
          decorationTemplate[i] = Element(decoration);
          i++;
        }

        return (new ShapeTemplate(elementTemplate, decorationTemplate, null, System.Drawing.Drawing2D.FillMode.Alternate, stencil.Tag));
      }

      else
        return null;
    }

    private ElementTemplate Element(Element element)
    {
      SysCAD.Protocol.Line line = element as SysCAD.Protocol.Line;

      if (line != null) return new LineTemplate((float)line.x1, (float)line.y1,
            (float)line.x2, (float)line.y2);

      Arc arc = element as Arc;

      if (arc != null) return new ArcTemplate((float)arc.x, (float)arc.y,
        (float)arc.w, (float)arc.h, (float)arc.a, (float)arc.s);

      Bezier bezier = element as Bezier;

      if (bezier != null) return new BezierTemplate((float)bezier.x1, (float)bezier.y1,
            (float)bezier.x2, (float)bezier.y2,
            (float)bezier.x3, (float)bezier.y3,
            (float)bezier.x4, (float)bezier.y4);

      return null;
    }

    private ModelStencil OpenModelStencil(string fullpath)
    {
      SoapFormatter sf = new SoapFormatter();
      StreamReader streamRdr = new StreamReader(fullpath);
      Stream stream = streamRdr.BaseStream;
      ModelStencil modelStencil = (ModelStencil)sf.Deserialize(stream);
      modelStencil.Tag = System.IO.Path.GetFileNameWithoutExtension(fullpath);
      return modelStencil;
    }

    private void SaveModelStencil(string fullpath, ModelStencil modelStencil)
    {
      SoapFormatter sf = new SoapFormatter();
      StreamWriter streamWriter = new StreamWriter(fullpath);
      Stream stream = streamWriter.BaseStream;
      sf.Serialize(stream, modelStencil);
      stream.Close();
    }

    private void Form1_Move(object sender, EventArgs e)
    {
      UpdatePreviews();
    }

    private void Form1_Resize(object sender, EventArgs e)
    {
      UpdatePreviews();
      ZoomToRect(this.flowChart1.DocExtents);
    }

    private void ZoomToRect(RectangleF rectangleF)
    {
      foreach (Arrow arrow in this.flowChart1.Arrows)
      {
        arrow.ArrowHeadSize = rectangleF.Width * 0.01F;
        arrow.ArrowBaseSize = rectangleF.Width * 0.01F;
      }
      this.flowChart1.ArrowHeadSize = rectangleF.Width * 0.01F;
      this.flowChart1.ArrowBaseSize = rectangleF.Width * 0.01F;
      this.flowChart1.SelHandleSize = rectangleF.Width * 0.01F;
      this.flowChart1.ZoomToRect(rectangleF);
    }

    private void Form1_Activated(object sender, EventArgs e)
    {
      UpdatePreviews();
    }

    private void inToolStripMenuItem_Click(object sender, EventArgs e)
    {
      visibleRect = flowChart1.ClientToDoc(flowChart1.ClientRectangle);
      visibleRect.Inflate(-visibleRect.Width * 0.11F, -visibleRect.Height * 0.11F);
      //this.flowChart1.DocExtents = visibleRect;
      ZoomToRect(visibleRect);
    }

    private void outToolStripMenuItem_Click(object sender, EventArgs e)
    {
      visibleRect = flowChart1.ClientToDoc(flowChart1.ClientRectangle);
      visibleRect.Inflate(visibleRect.Width * 0.1F, visibleRect.Height * 0.1F);

      if (visibleRect.Width > flowChart1.DocExtents.Width)
        visibleRect.Width = flowChart1.DocExtents.Width;

      if (visibleRect.Height > flowChart1.DocExtents.Height)
        visibleRect.Height = flowChart1.DocExtents.Height;

      if (visibleRect.Left < flowChart1.DocExtents.Left)
        visibleRect.X = flowChart1.DocExtents.X;

      if (visibleRect.Right > flowChart1.DocExtents.Right)
        visibleRect.X = flowChart1.DocExtents.Right - flowChart1.DocExtents.Width;

      if (visibleRect.Top < flowChart1.DocExtents.Top)
        visibleRect.Y = flowChart1.DocExtents.Y;

      if (visibleRect.Bottom > flowChart1.DocExtents.Bottom)
        visibleRect.Y = flowChart1.DocExtents.Bottom - flowChart1.DocExtents.Height;

      //this.flowChart1.DocExtents = visibleRect;
      ZoomToRect(visibleRect);
    }

    private void resetToolStripMenuItem_Click(object sender, EventArgs e)
    {
      visibleRect = new RectangleF(-10.0F, -10.0F, 120.0F, 120.0F);
      //this.flowChart1.DocExtents = visibleRect;
      ZoomToRect(visibleRect);
    }

    private void flowChart1_BoxModified(object sender, BoxMouseArgs e)
    {
      if (e.Box.Tag is Arc)
      {
        Arc arc = e.Box.Tag as Arc;
        arc.x = GridUnfix(e.Box.BoundingRect.X);
        arc.y = GridUnfix(e.Box.BoundingRect.Y);
        arc.w = e.Box.BoundingRect.Width;
        arc.h = e.Box.BoundingRect.Height;
      }
      else if (e.Box.Tag is SysCAD.Protocol.Point)
      {
        SysCAD.Protocol.Point point = e.Box.Tag as SysCAD.Protocol.Point;
        point.X = GridUnfix(e.Box.BoundingRect.X + 1.0F);
        point.Y = GridUnfix(e.Box.BoundingRect.Y + 1.0F);
      }

      SetStencil(modelStencil);
    }

    private void flowChart1_ArrowModified(object sender, ArrowMouseArgs e)
    {
      if (e.Arrow.Tag is Line)
      {
        Line line = e.Arrow.Tag as Line;

        line.pointF1 = GridUnfix(e.Arrow.ControlPoints[0]);
        line.pointF2 = GridUnfix(e.Arrow.ControlPoints[1]);
      }
      else if (e.Arrow.Tag is Bezier)
      {
        Bezier bezier = e.Arrow.Tag as Bezier;
        bezier.pointF1 = GridUnfix(e.Arrow.ControlPoints[0]);
        bezier.pointF2 = GridUnfix(e.Arrow.ControlPoints[1]);
        bezier.pointF3 = GridUnfix(e.Arrow.ControlPoints[2]);
        bezier.pointF4 = GridUnfix(e.Arrow.ControlPoints[3]);
      }

      SetStencil(modelStencil);
    }

    private PointF GridUnfix(PointF pointF)
    {
      return new PointF(pointF.X - flowChart1.GridSizeX / 2.0F, pointF.Y - flowChart1.GridSizeY / 2.0F);
    }

    private double GridUnfix(float pt)
    {
      return (Double)(pt - flowChart1.GridSizeX / 2.0F);
    }

    private void flowChart1_ArrowModifying(object sender, ArrowConfirmArgs e)
    {
      toolStripStatusLabel1.Text = GridUnfix(e.Point).ToString();
    }

    private void toolStripMenuItem3_Click(object sender, EventArgs e)
    {
      flowChart1.GridSizeX = 1;
      flowChart1.GridSizeY = 1;

      toolStripMenuItem3.Checked = true;
      toolStripMenuItem4.Checked = false;
      toolStripMenuItem5.Checked = false;
      toolStripMenuItem6.Checked = false;

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void toolStripMenuItem4_Click(object sender, EventArgs e)
    {
      flowChart1.GridSizeX = 2;
      flowChart1.GridSizeY = 2;

      toolStripMenuItem3.Checked = false;
      toolStripMenuItem4.Checked = true;
      toolStripMenuItem5.Checked = false;
      toolStripMenuItem6.Checked = false;

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void toolStripMenuItem5_Click(object sender, EventArgs e)
    {
      flowChart1.GridSizeX = 5;
      flowChart1.GridSizeY = 5;

      toolStripMenuItem3.Checked = false;
      toolStripMenuItem4.Checked = false;
      toolStripMenuItem5.Checked = true;
      toolStripMenuItem6.Checked = false;

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void toolStripMenuItem6_Click(object sender, EventArgs e)
    {
      flowChart1.GridSizeX = 10;
      flowChart1.GridSizeY = 10;

      toolStripMenuItem3.Checked = false;
      toolStripMenuItem4.Checked = false;
      toolStripMenuItem5.Checked = false;
      toolStripMenuItem6.Checked = true;

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void flowChart1_MouseClick(object sender, MouseEventArgs e)
    {
      if (e.Button == MouseButtons.Right) // Show context menu.
      {
        ContextMenu contextMenu = new ContextMenu();

        contextPoint = GridUnfix(flowChart1.ClientToDoc(e.Location));

        contextObject = flowChart1.GetArrowAt(contextPoint, 1.0F, true);
        if (!(contextObject is Arrow))
          contextObject = flowChart1.GetObjectAt(contextPoint, true);

        if (contextObject is Arrow)
        {
          contextMenu.MenuItems.Add("Split Line", SplitLine);
          contextMenu.MenuItems.Add("Delete Line", DeleteLine);

          if (contextObject.Tag is Line)
            contextMenu.MenuItems.Add("Convert To Bezier", ConvertToBezier);
          else if (contextObject.Tag is Bezier)
            contextMenu.MenuItems.Add("Convert To Line", ConvertToLine);

        }
        else if (contextObject is Box)
        {
        }
        else
        {
          contextMenu.MenuItems.Add("New Decoration Line", NewDecorationLine);
          contextMenu.MenuItems.Add("New Decoration Bezier", NewDecorationBezier);
          contextMenu.MenuItems.Add("New Decoration Ellipse", NewDecorationEllipse);

          foreach (Anchor anchor in modelStencil.Anchors)
          {
            MenuItem menuItem = contextMenu.MenuItems.Add("New " + anchor.Tag + " Anchor...", NewAnchor);
            menuItem.Tag = anchor;
          }
        }

        contextMenu.Show(this, e.Location);
      }
    }

    private void DeleteLine(object sender, EventArgs e)
    {
      Line line = (contextObject as Arrow).Tag as Line;
      if (modelStencil.Decorations.Contains(line))
      {
        modelStencil.Decorations.Remove(line);
      }
      else if (modelStencil.Elements.Contains(line))
      {
        modelStencil.Elements.Remove(line);
      }

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void SplitLine(object sender, EventArgs e)
    {
      Arrow arrow = contextObject as Arrow;
      Line line = arrow.Tag as Line;

      if (modelStencil.Elements.Contains(line))
      {
        int indexOf = modelStencil.Elements.IndexOf(line);

        Line newLine = new Line();
        newLine.pointF1 = line.pointF1;
        newLine.pointF2 = contextPoint;
        line.pointF1 = contextPoint;

        modelStencil.Elements.Insert(indexOf, newLine);

        ResetFlowChart(true);
        SetStencil(modelStencil);
        SetControls(modelStencil);
      }
      if (modelStencil.Decorations.Contains(line))
      {
        int indexOf = modelStencil.Decorations.IndexOf(line);

        Line newLine = new Line();
        newLine.pointF1 = line.pointF1;
        newLine.pointF2 = contextPoint;
        line.pointF1 = contextPoint;

        modelStencil.Decorations.Insert(indexOf, newLine);

        ResetFlowChart(true);
        SetStencil(modelStencil);
        SetControls(modelStencil);
      }
    }

    private void NewDecorationLine(object sender, EventArgs e)
    {
      modelStencil.Decorations.Add(
        new Line(contextPoint.X - 10.0,
          contextPoint.Y,
          contextPoint.X + 10.0,
          contextPoint.Y));

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void NewDecorationBezier(object sender, EventArgs e)
    {
      modelStencil.Decorations.Add(
        new Bezier(contextPoint.X - 10.0,
          contextPoint.Y,
          contextPoint.X - 3.0,
          contextPoint.Y - 10.0,
          contextPoint.X + 3.0,
          contextPoint.Y + 10.0,
          contextPoint.X + 10.0,
          contextPoint.Y));

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void NewDecorationEllipse(object sender, EventArgs e)
    {
      modelStencil.Decorations.Add(
        new Arc(contextPoint.X - 10.0, contextPoint.Y - 10.0, 20.0, 20.0, 0.0, 360.0));

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void NewAnchor(object sender, EventArgs e)
    {
      Anchor anchor = (sender as MenuItem).Tag as Anchor;

      anchor.Positions.Add(new SysCAD.Protocol.Point(contextPoint));

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void ConvertToBezier(object sender, EventArgs e)
    {
      Arrow arrow = contextObject as Arrow;
      Line line = arrow.Tag as Line;

      if (modelStencil.Elements.Contains(line))
      {
        int indexOf = modelStencil.Elements.IndexOf(line);

        Bezier bezier = new Bezier(
          line.x1,
          line.y1,
          (2.0 * line.x1 + 1.0 * line.x2) / 3.0,
          (2.0 * line.y1 + 1.0 * line.y2) / 3.0,
          (1.0 * line.x1 + 2.0 * line.x2) / 3.0,
          (1.0 * line.y1 + 2.0 * line.y2) / 3.0,
          line.x2,
          line.y2);

        modelStencil.Elements[indexOf] = bezier;
      }

      if (modelStencil.Decorations.Contains(line))
      {
        int indexOf = modelStencil.Decorations.IndexOf(line);

        Bezier bezier = new Bezier(
          line.x1,
          line.y1,
          (2.0 * line.x1 + 1.0 * line.x2) / 3.0,
          (2.0 * line.y1 + 1.0 * line.y2) / 3.0,
          (1.0 * line.x1 + 2.0 * line.x2) / 3.0,
          (1.0 * line.y1 + 2.0 * line.y2) / 3.0,
          line.x2,
          line.y2);

        modelStencil.Decorations[indexOf] = bezier;
      }

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void ConvertToLine(object sender, EventArgs e)
    {
      Arrow arrow = contextObject as Arrow;
      Bezier bezier = arrow.Tag as Bezier;

      if (modelStencil.Elements.Contains(bezier))
      {
        int indexOf = modelStencil.Elements.IndexOf(bezier);

        Line line = new Line(
          bezier.x1,
          bezier.y1,
          bezier.x4,
          bezier.y4);

        modelStencil.Elements[indexOf] = line;
      }

      if (modelStencil.Decorations.Contains(bezier))
      {
        int indexOf = modelStencil.Decorations.IndexOf(bezier);

        Line line = new Line(
          bezier.x1,
          bezier.y1,
          bezier.x4,
          bezier.y4);

        modelStencil.Decorations[indexOf] = line;
      }

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void editPropertiesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      EditProperties editProperties = new EditProperties();
      editProperties.group = modelStencil.Groups[0] as string;
      editProperties.tag = modelStencil.Tag;
      editProperties.anchors = modelStencil.Anchors;

      if (editProperties.ShowDialog() == DialogResult.OK)
      {
        modelStencil.Groups[0] = editProperties.group;
        modelStencil.Tag = editProperties.tag;

        ArrayList newAnchors = new ArrayList();
        foreach (Anchor anchor in modelStencil.Anchors)
        {
          if (editProperties.listView1.Items.ContainsKey(anchor.Tag))
          {
            newAnchors.Add(anchor);
          }
        }
        modelStencil.Anchors = newAnchors;

        foreach (ListViewItem listViewItem in editProperties.listView1.Items)
        {
          bool anchorFound = false;

          foreach (Anchor anchor in modelStencil.Anchors)
          {
            if (anchor.Tag == listViewItem.Text)
            {
              anchorFound = true;
              anchor.Look = (short)editProperties.listView1.Groups.IndexOf(listViewItem.Group);
            }
          }

          if (!anchorFound)
          {
            Anchor newAnchor = new Anchor(listViewItem.Text, AnchorType.Process,
                (short)editProperties.listView1.Groups.IndexOf(listViewItem.Group));
            modelStencil.Anchors.Add(newAnchor);
            newAnchor.Positions = new ArrayList();
          }
        }
      }

      ResetFlowChart(true);
      SetStencil(modelStencil);
      SetControls(modelStencil);
    }

    private void flowChart1_MouseMove(object sender, MouseEventArgs e)
    {
      PointF hoverPoint = GridUnfix(flowChart1.ClientToDoc(e.Location));
      ChartObject hoverObject = flowChart1.GetObjectAt(contextPoint, true);

      // Hover information (e.g. anchor and look...)
    }

    private void showPreviewToolStripMenuItem_CheckStateChanged(object sender, EventArgs e)
    {
      UpdatePreviews();
    }

    private void toggleGridToolStripMenuItem_CheckStateChanged(object sender, EventArgs e)
    {
      flowChart1.AlignToGrid = toggleGridToolStripMenuItem.Checked;
      //flowChart1.ShowGrid = toggleGridToolStripMenuItem.Checked;
    }

    private void saveToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveFileDialog = new SaveFileDialog();
      saveFileDialog.AddExtension = true;
      saveFileDialog.CheckPathExists = true;
      saveFileDialog.CreatePrompt = true;
      saveFileDialog.DefaultExt = "ModelStencil";
      saveFileDialog.Filter = "ModelStencils|*.ModelStencil";
      saveFileDialog.OverwritePrompt = true;
      saveFileDialog.ShowHelp = false;
      saveFileDialog.SupportMultiDottedExtensions = true;
      saveFileDialog.FileName = System.IO.Path.GetFileName(path);
      saveFileDialog.Title = "Save ModelStencil";

      saveFileDialog.InitialDirectory = System.IO.Path.GetFullPath(path);

      if (saveFileDialog.ShowDialog(this) == DialogResult.OK)
      {
        SaveModelStencil(saveFileDialog.FileName, modelStencil);
      }
    }
  }
}
