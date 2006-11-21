using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.IO;
using System.Collections.Specialized;
using MindFusion.FlowChartX;
using SysCAD.Interface;



namespace SysCAD.ThingEditor.ThingEditorCtrl
{
  /// <summary>
  /// Summary description for ThingEditorCtrl.
  /// </summary>
  /// 
  public delegate void ShapeChanged(object sender, EventArgs e);

  public class ThingEditor : System.Windows.Forms.Control
  {
    #region Declarations

    public GraphicThing graphicThing;

    public event ShapeChanged ShapeChanged;

    public static float MinX;
    public static float MinY;
    public static float MaxX;
    public static float MaxY;

    private ArrayList outLines;

    private Outline currOutline;
    private Outline pointedOutline;
    private SegmentedOutline saveOutline;
    private EGridStyle gridStyle;
    private VScrollBar vScrollBar;
    private HScrollBar hScrollBar;


    private bool dragging;
    private bool flagShowPoints;
    private bool flagShowGrid;
    private bool flagDefineTextArea;
    private int saveIndex;
    private float saveNewPosX;
    private float saveNewPosY;
    private float SaveCurrX;
    private float SaveCurrY;
    private int gridSize;
    private int saveGridSize;


    private int distanceToLine;
    private int distanceToTextArea;
    private int decorationWidth;

    private float scrollX;
    private float scrollY;

    private Color colorNormalSegment;
    private Color backgrndColor;
    private Color gridColor;
    private Color textAreaContourColor;
    private Color mainContourColor;

    #region Context Menu

    private ContextMenu CntMenuSegment;
    private ContextMenu CntMenuDecoration;
    private ContextMenu CntMenuCombine;
    private MenuItem menuItemCombine;
    private MenuItem menuItemInsertDecorLine;
    private MenuItem menuItemInsertDecorBezier;
    private MenuItem menuItemInsertDecorEllipse;
    private MenuItem menuItemSeparator1;
    private MenuItem menuItemDeleteDecoration;
    private MenuItem menuItemDeleteAllDecorations;
    private MenuItem menuItemSeparator4;
    private MenuItem menuItemSeparator5;
    private MenuItem menuItemShowPoints;
    private MenuItem menuItemHidePoints;
    private MenuItem menuItemShowGrid;
    private MenuItem menuItemHideGrid;
    private MenuItem menuItemDefineTextArea;
    private MenuItem menuItemClearTextArea;
    #endregion

    #endregion

    #region Constructors

    public ThingEditor(GraphicThing graphicThing)
    {
      this.graphicThing = graphicThing;


      InitContextMenu();

      colorNormalSegment = Color.FromArgb(220, 220, 255);
      backgrndColor = Color.FromArgb(170, 170, 200);
      gridColor = Color.Black;
      textAreaContourColor = Color.FromArgb(100, 180, 180, 100);
      mainContourColor = Color.FromArgb(100, 220, 220, 255);
      dragging = false;
      flagShowPoints = true;
      flagShowGrid = true;
      flagDefineTextArea = false;
      distanceToLine = 10;
      distanceToTextArea = 10;
      decorationWidth = 100;
      gridSize = 16;
      saveGridSize = gridSize;
      gridStyle = EGridStyle.grdLine;

      vScrollBar = null;
      hScrollBar = null;

      if (graphicThing != null)
      {
        outLines = new ArrayList();

        if (graphicThing.elements.Count > 0)
        {
          Contour elements = new Contour(graphicThing.FrameColor, graphicThing.elements);
          outLines.Add(elements);
          elements.Changed += new OutlineChanged(onContourChanged);
        }
        else
        {
          outLines = new ArrayList();
          Contour contour = new Contour(mainContourColor, gridSize);
          outLines.Add(contour);
          contour.Changed += new OutlineChanged(onContourChanged);
        }

        if (graphicThing.decorations.Count > 0)
        {
          PolyLine polyLine = null;
          foreach (Element element in graphicThing.decorations)
          {
            if (element is Line)
            {
              if (polyLine == null)
              {
                polyLine = new PolyLine(element as Line);
              }
              else
              {
                if (!polyLine.Add(element as Line)) // couldn't add because ends don't connect, start a new one.
                {
                  outLines.Add(polyLine);
                  polyLine.Changed += new OutlineChanged(onContourChanged);
                  polyLine = new PolyLine(element as Line);
                }
              }
            }
          }
          if (polyLine != null)
          {
            outLines.Add(polyLine);
            polyLine.Changed += new OutlineChanged(onContourChanged);
          }
        }

        if (graphicThing.textArea.Count > 0)
        {
          Contour textArea = new Contour(graphicThing.FrameColor, graphicThing.textArea);
          outLines.Add(textArea);
          textArea.Changed += new OutlineChanged(onContourChanged);
        }
      }

      RectangleF tempRect = getBoundingRect();
      MinX = tempRect.Left;
      MinY = tempRect.Top;
      MaxX = tempRect.Right;
      MaxY = tempRect.Bottom;

      onShapeChanged(this, null);
    }

    #endregion

    #region InitContextMenu

    private void InitContextMenu()
    {
      //--------------------------------------------------------------------------------            
      //creating main context menu
      CntMenuSegment = new ContextMenu();
      //end
      //--------------------------------------------------------------------------------            
      //creating context menu for combine two neighbour segments
      CntMenuCombine = new ContextMenu();
      menuItemCombine = new MenuItem("&Combine");
      CntMenuCombine.MenuItems.Add(menuItemCombine);
      menuItemCombine.Click += new EventHandler(menuItemCombineOnClick);
      //end
      //--------------------------------------------------------------------------------            
      //creating context menu for decorations
      CntMenuDecoration = new ContextMenu();
      menuItemInsertDecorLine = new MenuItem("Insert Decoration Line");
      menuItemInsertDecorBezier = new MenuItem("Insert Decoration Bezier");
      menuItemInsertDecorEllipse = new MenuItem("Insert Decoration Ellipse");
      menuItemSeparator1 = new MenuItem("-");
      menuItemDefineTextArea = new MenuItem("Define Text Area");
      menuItemClearTextArea = new MenuItem("Clear Text Area");
      menuItemSeparator5 = new MenuItem("-");
      menuItemDeleteDecoration = new MenuItem("Delete");
      menuItemDeleteAllDecorations = new MenuItem("Delete All");
      menuItemSeparator4 = new MenuItem("-");
      menuItemShowPoints = new MenuItem("Show Points");
      menuItemHidePoints = new MenuItem("Hide Points");
      menuItemShowGrid = new MenuItem("Show Grid");
      menuItemHideGrid = new MenuItem("Hide Grid");

      CntMenuDecoration.MenuItems.Add(menuItemInsertDecorLine);
      CntMenuDecoration.MenuItems.Add(menuItemInsertDecorBezier);
      CntMenuDecoration.MenuItems.Add(menuItemInsertDecorEllipse);
      CntMenuDecoration.MenuItems.Add(menuItemSeparator1);
      CntMenuDecoration.MenuItems.Add(menuItemDefineTextArea);
      CntMenuDecoration.MenuItems.Add(menuItemClearTextArea);
      CntMenuDecoration.MenuItems.Add(menuItemSeparator5);
      CntMenuDecoration.MenuItems.Add(menuItemDeleteDecoration);
      CntMenuDecoration.MenuItems.Add(menuItemDeleteAllDecorations);
      CntMenuDecoration.MenuItems.Add(menuItemSeparator4);
      CntMenuDecoration.MenuItems.Add(menuItemShowPoints);
      CntMenuDecoration.MenuItems.Add(menuItemHidePoints);
      CntMenuDecoration.MenuItems.Add(menuItemShowGrid);
      CntMenuDecoration.MenuItems.Add(menuItemHideGrid);


      menuItemShowPoints.Visible = false;
      menuItemShowGrid.Visible = false;
      menuItemClearTextArea.Visible = false;
      menuItemInsertDecorLine.Click += new EventHandler(menuItemInsertDecorLineOnClick);
      menuItemInsertDecorBezier.Click += new EventHandler(menuItemInsertDecorBezierOnClick);
      menuItemInsertDecorEllipse.Click += new EventHandler(menuItemInsertDecorEllipseOnClick);
      menuItemDeleteDecoration.Click += new EventHandler(menuItemDeleteDecorationOnClick);
      menuItemDeleteAllDecorations.Click += new EventHandler(menuItemDeleteAllDecorationsOnClick);
      menuItemShowPoints.Click += new EventHandler(menuItemShowPointsOnClick);
      menuItemHidePoints.Click += new EventHandler(menuItemHidePointsOnClick);
      menuItemShowGrid.Click += new EventHandler(menuItemShowGridOnClick);
      menuItemHideGrid.Click += new EventHandler(menuItemHideGridOnClick);
      menuItemDefineTextArea.Click += new EventHandler(menuItemDefineTextAreaOnClick);
      menuItemClearTextArea.Click += new EventHandler(menuItemClearTextAreaOnClick);
      // end
    }
    #endregion

    #region Enumerations

    public enum EGridStyle
    {
      grdLine,
      grdDot,
    }

    #endregion

    #region Properties

    public EGridStyle GridStyle
    {
      get
      {
        return gridStyle;
      }
      set
      {
        if (gridStyle != value)
        {
          gridStyle = value;
          Invalidate();
        }
      }
    }

    public override Color BackColor
    {
      get
      {
        return backgrndColor;
      }
      set
      {
        backgrndColor = value;
        Invalidate();
      }
    }

    public Color GridColor
    {
      get
      {
        return gridColor;
      }
      set
      {
        gridColor = value;
        Invalidate();
      }
    }

    public int GridSize
    {
      get
      {
        return gridSize;
      }
      set
      {
        gridSize = value;
        Invalidate();
      }
    }

    public Color PenColor
    {
      get
      {
        return colorNormalSegment;
      }
      set
      {
        colorNormalSegment = value;
        Invalidate();
      }
    }

    public VScrollBar VScrollBar
    {
      get
      {
        return vScrollBar;
      }
    }

    public HScrollBar HScrollBar
    {
      get
      {
        return hScrollBar;
      }
    }

    [Category("Appearance")]
    [Description("Specifies whether scrollbars are visible.")]
    public bool ShowScrollbars
    {
      get
      {
        return (hScrollBar != null && vScrollBar != null);
      }
      set
      {
        if (value)
        {
          vScrollBar = new VScrollBar();
          vScrollBar.Dock = DockStyle.Right;
          vScrollBar.Scroll += new ScrollEventHandler(ScrollEvent);
          Controls.Add(vScrollBar);

          hScrollBar = new HScrollBar();
          hScrollBar.Dock = DockStyle.Bottom;
          hScrollBar.Scroll += new ScrollEventHandler(ScrollEvent);
          Controls.Add(hScrollBar);
        }
        else
        {
          Controls.Remove(hScrollBar);
          hScrollBar = null;
          Controls.Remove(vScrollBar);
          vScrollBar = null;
        }
        Invalidate();
      }

    }

    #endregion

    #region Events

    public void ScrollEvent(object sender, ScrollEventArgs e)
    {
      if (hScrollBar != null)
        if (sender == hScrollBar)
        {
          hScrollBar.Value = e.NewValue;
          setScrollX((float)e.NewValue);
        }
      if (vScrollBar != null)
        if (sender == vScrollBar)
        {
          vScrollBar.Value = e.NewValue;
          setScrollY((float)e.NewValue);
        }
    }

    #endregion

    void setScrollX(float scr)
    {
      scrollX = scr;
      Invalidate();
    }

    void setScrollY(float scr)
    {
      scrollY = scr;
      Invalidate();
    }

    private void onContourChanged(object sender, EventArgs e)
    {
      if (sender is PolyLine)
        if (!(sender as PolyLine).hasSegments())
          outLines.Remove(sender as PolyLine);

      Invalidate();
      onShapeChanged(this, null);
    }

    private RectangleF getBoundingRect()
    {
      Outline outLine;
      outLine = (Outline)outLines[0];
      RectangleF shapeRect = outLine.getBoundingRect();
      for (int i = 1; i < outLines.Count; ++i)
      {
        outLine = (Outline)outLines[i];
        shapeRect = RectangleF.Union(shapeRect, outLine.getBoundingRect());
      }
      return shapeRect;
    }

    private void onShapeChanged(object sender, EventArgs e)
    {
      RectangleF tempRect = getBoundingRect();
      MinX = tempRect.Left;
      MinY = tempRect.Top;
      MaxX = tempRect.Right;
      MaxY = tempRect.Bottom;

      if (ShapeChanged != null)
        ShapeChanged(sender, e);
      Invalidate();
    }

    private void menuItemInsertDecorLineOnClick(object Sender, System.EventArgs e)
    {
      LineSegment newDecorationLine = new LineSegment(new PointF(saveNewPosX, saveNewPosY), new PointF(saveNewPosX + decorationWidth, saveNewPosY));
        newDecorationLine.isDecorationSegment = true;
      PolyLine newPolyLine = new PolyLine(newDecorationLine);
      outLines.Add(newPolyLine);
      newPolyLine.Changed += new OutlineChanged(onContourChanged);
      onContourChanged(Sender, e);
    }

    private void menuItemInsertDecorBezierOnClick(object Sender, System.EventArgs e)
    {
      PointF pt0 = new PointF(saveNewPosX, saveNewPosY);
      PointF pt3 = new PointF(saveNewPosX + decorationWidth, saveNewPosY);

      BezierSegment newBezierSeg = new BezierSegment();
      newBezierSeg.isDecorationSegment = true;
      newBezierSeg.points[0] = pt0;
      newBezierSeg.points[1] = new PointF((pt0.X + pt3.X) / 3, (pt0.Y + pt3.Y) / 3);
      newBezierSeg.points[2] = new PointF((pt0.X + pt3.X) * 2 / 3, (pt0.Y + pt3.Y) * 2 / 3);
      newBezierSeg.points[3] = pt3;

      PolyLine newPolyline = new PolyLine(newBezierSeg);
      outLines.Add(newPolyline);
      newPolyline.Changed += new OutlineChanged(onContourChanged);
      onContourChanged(Sender, e);
    }

    private void menuItemInsertDecorEllipseOnClick(object Sender, System.EventArgs e)
    {
      Ellipse newEllipse = new Ellipse(saveNewPosX, saveNewPosY);
      outLines.Add(newEllipse);
      onContourChanged(Sender, e);
    }

    private void menuItemDeleteDecorationOnClick(object Sender, System.EventArgs e)
    {
      if (currOutline is Ellipse)
      {
        for (int i = 0; i < outLines.Count; ++i)
        {
          Outline outLine = (Outline)outLines[i];
          if (outLine is Ellipse && outLine == currOutline)
            outLines.Remove(currOutline);
        }
        onContourChanged(Sender, e);
      }
    }

    public void menuItemDeleteAllDecorationsOnClick(object Sender, System.EventArgs e)
    {
      InitContextMenu();
      if (HScrollBar != null)
        HScrollBar.Value = 0;
      if (VScrollBar != null)
        VScrollBar.Value = 0;
      scrollX = 0;
      scrollY = 0;
      outLines.Clear();

      Contour contour = new Contour(mainContourColor, GridSize);
      outLines.Add(contour);
      contour.Changed += new OutlineChanged(onContourChanged);
      onContourChanged(Sender, e);
    }

    private void showPoints()
    {
      flagShowPoints = !flagShowPoints;
      menuItemHidePoints.Visible = true;
      menuItemShowPoints.Visible = false;
    }

    private void hidePoints()
    {
      flagShowPoints = !flagShowPoints;
      menuItemHidePoints.Visible = false;
      menuItemShowPoints.Visible = true;
    }

    private void showGrid()
    {
      flagShowGrid = !flagShowGrid;
      menuItemHideGrid.Visible = true;
      menuItemShowGrid.Visible = false;
    }

    private void hideGrid()
    {
      flagShowGrid = !flagShowGrid;
      menuItemHideGrid.Visible = false;
      menuItemShowGrid.Visible = true;
    }

    private void DefineTextArea()
    {
      flagDefineTextArea = !flagDefineTextArea;
      menuItemDefineTextArea.Visible = false;
      menuItemClearTextArea.Visible = true;

      ArrayList arr = new ArrayList();

      arr = ((Outline)outLines[0] as Contour).getSegments();

      RectangleF rect = ((Outline)outLines[0] as Contour).getBoundingRect();
      float middleCenterRectX = (Math.Abs(rect.Right - rect.Left)) / 2.0F + rect.Left;
      float middleCenterRectY = (Math.Abs(rect.Bottom - rect.Top)) / 2.0F + rect.Top;

      PointF[] pStart = new PointF[arr.Count];
      PointF[] pEnd = new PointF[arr.Count];
      PointF[] point1 = new PointF[arr.Count];
      PointF[] point2 = new PointF[arr.Count];
      for (int i = 0; i < arr.Count; ++i)
      {
        Segment s = (Segment)arr[i];

        if (s is BezierSegment)
        {
          if ((s as BezierSegment).points[1].X < middleCenterRectX)
            point1[i].X = (s as BezierSegment).points[1].X + distanceToTextArea;
          if ((s as BezierSegment).points[1].X >= middleCenterRectX)
            point1[i].X = (s as BezierSegment).points[1].X - distanceToTextArea;

          if ((s as BezierSegment).points[1].Y < middleCenterRectY)
            point1[i].Y = (s as BezierSegment).points[1].Y + distanceToTextArea;
          if ((s as BezierSegment).points[1].Y >= middleCenterRectY)
            point1[i].Y = (s as BezierSegment).points[1].Y - distanceToTextArea;

          if ((s as BezierSegment).points[2].X < middleCenterRectX)
            point2[i].X = (s as BezierSegment).points[2].X + distanceToTextArea;
          if ((s as BezierSegment).points[2].X >= middleCenterRectX)
            point2[i].X = (s as BezierSegment).points[2].X - distanceToTextArea;

          if ((s as BezierSegment).points[2].Y < middleCenterRectY)
            point2[i].Y = (s as BezierSegment).points[2].Y + distanceToTextArea;
          if ((s as BezierSegment).points[2].Y >= middleCenterRectY)
            point2[i].Y = (s as BezierSegment).points[2].Y - distanceToTextArea;
        }

        PointF tempStart = s.GetStartPoint();
        PointF tempEnd = s.GetEndPoint();

        if (tempStart.X < middleCenterRectX)
          pStart[i].X = tempStart.X + distanceToTextArea;

        if (tempStart.X >= middleCenterRectX)
          pStart[i].X = tempStart.X - distanceToTextArea;

        if (tempEnd.X < middleCenterRectX)
          pEnd[i].X = tempEnd.X + distanceToTextArea;

        if (tempEnd.X >= middleCenterRectX)
          pEnd[i].X = tempEnd.X - distanceToTextArea;

        if (tempStart.Y < middleCenterRectY)
          pStart[i].Y = tempStart.Y + distanceToTextArea;

        if (tempStart.Y >= middleCenterRectY)
          pStart[i].Y = tempStart.Y - distanceToTextArea;

        if (tempEnd.Y < middleCenterRectY)
          pEnd[i].Y = tempEnd.Y + distanceToTextArea;

        if (tempEnd.Y >= middleCenterRectY)
          pEnd[i].Y = tempEnd.Y - distanceToTextArea;
      }

      Contour contour = new Contour(textAreaContourColor, arr, pStart, point1, point2, pEnd);
      outLines.Add(contour);
      contour.Changed += new OutlineChanged(onContourChanged);
    }

    private void ClearTextArea()
    {
      flagDefineTextArea = !flagDefineTextArea;
      menuItemDefineTextArea.Visible = true;
      menuItemClearTextArea.Visible = false;

      for (int i = 1; i < outLines.Count; ++i)
      {
        Outline outLine = (Outline)outLines[i];
        if (outLine is Contour)
        {
          outLines.Remove(outLine);
          break;
        }
      }
    }

    private void menuItemShowPointsOnClick(object Sender, System.EventArgs e)
    {
      showPoints();
      Invalidate();
    }

    private void menuItemHidePointsOnClick(object Sender, System.EventArgs e)
    {
      hidePoints();
      Invalidate();
    }

    private void menuItemShowGridOnClick(object Sender, System.EventArgs e)
    {
      showGrid();
      Invalidate();
    }

    private void menuItemHideGridOnClick(object Sender, System.EventArgs e)
    {
      hideGrid();
      Invalidate();
    }

    private void menuItemDefineTextAreaOnClick(object Sender, System.EventArgs e)
    {
      DefineTextArea();
      onContourChanged(Sender, e);
    }

    private void menuItemClearTextAreaOnClick(object Sender, System.EventArgs e)
    {
      ClearTextArea();
      onContourChanged(Sender, e);
    }


    private void setShapeTemplate(ArrayList arr, int srcCode)
    {
      if (srcCode == 0)
        arr.Add("   new ShapeTemplate(");
      if (srcCode == 1)
        arr.Add("   new ShapeTemplate( _");
    }

    private void setElementTemplate(ArrayList arr, int srcCode)
    {
      if (srcCode == 0)
      {
        arr.Add("       new ElementTemplate[]");
        arr.Add("       {");
      }
      if (srcCode == 1)
      {
        arr.Add("       new ElementTemplate() _");
        arr.Add("       { _");
      }
    }

    private void setEndElementTemplate(ArrayList arr, int srcCode)
    {
      if (srcCode == 0)
        arr.Add("       },");
      if (srcCode == 1)
        arr.Add("       }, _ ");
    }

    private void getEllipse(int outLineCnt, Outline outLine, ArrayList arr, int srcCode)
    {
      if (srcCode == 0)
        if (outLineCnt != outLines.Count)
          arr.Add((outLine as Ellipse).saveNameAsText() + ",");
        else
          arr.Add((outLine as Ellipse).saveNameAsText());

      if (srcCode == 1)
        if (outLineCnt != outLines.Count)
          arr.Add((outLine as Ellipse).saveNameAsText() + ", _");
        else
          arr.Add((outLine as Ellipse).saveNameAsText() + " _");
    }

    private void getContour(Outline outLine, ArrayList arr, int srcCode)
    {
      setElementTemplate(arr, srcCode);

      string[] getNames = (outLine as Contour).getNamesAsText();

      for (int i = 0; i < getNames.Length; ++i)
      {
        if (srcCode == 0)
          if (i != getNames.Length - 1)
            arr.Add(getNames[i] + ",");
          else
            arr.Add(getNames[i]);

        if (srcCode == 1)
          if (i != getNames.Length - 1)
            arr.Add(getNames[i] + ", _ ");
          else
            arr.Add(getNames[i] + " _ ");
      }
      setEndElementTemplate(arr, srcCode);
    }

    private void getPolyLine(int outLineCnt, Outline outLine, ArrayList arr, int srcCode)
    {
      string[] getNames = (outLine as PolyLine).getNamesAsText();

      for (int i = 0; i < getNames.Length; ++i)
      {
        if (srcCode == 0)
          if (outLineCnt != outLines.Count)
            arr.Add(getNames[i] + ",");
          else
            if (i != getNames.Length - 1)
              arr.Add(getNames[i] + ",");
            else
              arr.Add(getNames[i]);

        if (srcCode == 1)
          if (outLineCnt != outLines.Count)
            arr.Add(getNames[i] + ", _ ");
          else
            if (i != getNames.Length - 1)
              arr.Add(getNames[i] + ", _ ");
            else
              arr.Add(getNames[i] + " _ ");
      }
    }

    private void setEndShapeTemplate(bool defineTextArea, ArrayList arr, int srcCode)
    {
      if (srcCode == 0)
        if (defineTextArea)
          arr.Add(" FillMode.Winding," + "   " + "\"" + "test" + "\"" + " );");
        else
          arr.Add(" null, FillMode.Winding," + "   " + "\"" + "test" + "\"" + " );");

      if (srcCode == 1)
        if (defineTextArea)
          arr.Add("  FillMode.Winding," + "   " + "\"" + "test" + "\"" + " ) ");
        else
          arr.Add(" nothing, FillMode.Winding," + "   " + "\"" + "test" + "\"" + " ) ");
    }


    public string[] exportTemplateCode(int srcCode)
    {
      Outline outLine = null;
      bool isExistElementTemplate = false;
      int i;
      int contourCnt = 1;

      ArrayList arr = new ArrayList();

      setShapeTemplate(arr, srcCode);

      outLine = (Outline)outLines[0];
      getContour(outLine, arr, srcCode);

      for (i = 0; i < outLines.Count; ++i)
      {
        outLine = (Outline)outLines[i];

        //if (outLine is Contour)
        //	continue;
        if (!isExistElementTemplate)
        {
          setElementTemplate(arr, srcCode);
          isExistElementTemplate = true;
        }

        if (outLine is Ellipse)
          getEllipse(i + contourCnt, outLine, arr, srcCode);
        else
          if (outLine is PolyLine)
            getPolyLine(i + contourCnt, outLine, arr, srcCode);
      }

      if (isExistElementTemplate)
        setEndElementTemplate(arr, srcCode);

      for (i = 1; i < outLines.Count; ++i)
      {
        outLine = (Outline)outLines[i];
        if (outLine is Contour)
        {
          getContour(outLine, arr, srcCode);
          contourCnt += 1;
        }
      }


      setEndShapeTemplate(flagDefineTextArea, arr, srcCode);

      string[] str = new string[arr.Count];
      for (i = 0; i < arr.Count; ++i)
        str[i] = arr[i].ToString();

      return str;
    }





    private void menuItemCombineOnClick(object Sender, System.EventArgs e)
    {
      saveOutline.combineSegments(SaveCurrX, SaveCurrY);
      onContourChanged(Sender, e);
    }

    private void drawGrid(Graphics gr)
    {
      switch (GridStyle)
      {
        case EGridStyle.grdLine:
          System.Drawing.Pen drawPen = new System.Drawing.Pen(GridColor, 1);
          for (int i = this.ClientRectangle.Left; i < this.ClientRectangle.Right + scrollX; i += gridSize)
            gr.DrawLine(drawPen, i - scrollX, this.ClientRectangle.Top, i - scrollX, this.ClientRectangle.Bottom);
          for (int i = this.ClientRectangle.Top; i < this.ClientRectangle.Bottom + scrollY; i += gridSize)
            gr.DrawLine(drawPen, this.ClientRectangle.Left, i - scrollY, this.ClientRectangle.Right, i - scrollY);
          drawPen.Dispose();
          break;

        case EGridStyle.grdDot:
          System.Drawing.SolidBrush solidBrush = new System.Drawing.SolidBrush(GridColor);
          for (int i = this.ClientRectangle.Left; i < this.ClientRectangle.Right + scrollX; i += gridSize)
            for (int j = this.ClientRectangle.Top; j < this.ClientRectangle.Bottom + scrollY; j += gridSize)
              gr.FillEllipse(solidBrush, i - scrollX - 1, j - scrollY - 1, 2, 2);
          solidBrush.Dispose();
          break;
      }
    }

    private void setScrollBarsRange(Graphics g)
    {
      g.TranslateTransform(-scrollX, -scrollY);

      if (vScrollBar != null)
      {
        vScrollBar.Minimum = this.ClientRectangle.Top;
        vScrollBar.Maximum = this.ClientRectangle.Height;
      }
      if (hScrollBar != null)
      {
        hScrollBar.Minimum = this.ClientRectangle.Left;
        hScrollBar.Maximum = this.ClientRectangle.Width;
      }
    }

    protected override void OnPaint(PaintEventArgs pe)
    {
      // TODO: Add custom paint code here
      System.Drawing.Pen penNormalSegment = new System.Drawing.Pen(colorNormalSegment, 1);
      System.Drawing.Bitmap bm = new System.Drawing.Bitmap(this.Width, this.Height);
      System.Drawing.Graphics gr = Graphics.FromImage(bm);
      gr.Clear(backgrndColor);

      System.Drawing.SolidBrush brush = new System.Drawing.SolidBrush(Color.Black);

      if (flagShowGrid)
        drawGrid(gr);

      if (flagShowPoints)
        brush.Color = Color.Black;
      else
        brush.Color = Color.Transparent;

      setScrollBarsRange(gr);

      foreach (Outline outLine in outLines)
      {
        outLine.Draw(gr, penNormalSegment);
        outLine.drawPoints(gr, brush);
      }
      pe.Graphics.DrawImage(bm, this.ClientRectangle);

      penNormalSegment.Dispose();
      brush.Dispose();
      gr.Dispose();
      bm.Dispose();
    }

    protected override void OnMouseDown(System.Windows.Forms.MouseEventArgs e)
    {
      dragging = false;
      PointF p = new PointF();

      p.X = e.X + scrollX;
      p.Y = e.Y + scrollY;

      if (e.Button == MouseButtons.Right)
      {
        saveNewPosX = p.X;
        saveNewPosY = p.Y;

        this.ContextMenu = CntMenuDecoration;
        for (int i = 0; i < outLines.Count; ++i)
        {
          Outline outLine = (Outline)outLines[i];

          if (outLine is Ellipse && (outLine as Ellipse).getSegmentIndex(p) < distanceToLine)
          {
            currOutline = outLine;
            return;
          }

          if (!(outLine is Ellipse) && outLine.getSegmentIndex(p) > -1)
          {
            saveIndex = i;
            saveOutline = (SegmentedOutline)outLine;
            outLine.addMenuItems(CntMenuSegment, p);
            this.ContextMenu = CntMenuSegment;
            break;
          }
        }

        if (saveOutline != null)
          if (saveOutline.checkOverPoint(p))
          {
            if (saveOutline.isAllowCombineCntMenu(p))
              this.ContextMenu = CntMenuCombine;
          }
      }
      if (e.Button == MouseButtons.Left)
      {
        for (int i = 0; i < outLines.Count; ++i)
        {
          Outline s = (Outline)outLines[i];
          if (s.hitTest(p))
          {
            dragging = true;
            currOutline = s;
            break;
          }
        }
      }
    }

    protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
    {
      PointF p = new PointF();
      p.X = e.X + (int)scrollX;
      p.Y = e.Y + (int)scrollY;
      SaveCurrX = p.X;
      SaveCurrY = p.Y;

      base.Cursor = Cursors.Default;

      if (flagShowPoints)
      {
        for (int i = 0; i < outLines.Count; ++i)
        {
          Outline outline = (Outline)outLines[i];
          if (outline.checkOverPoint(p))
          {
            base.Cursor = Cursors.SizeAll;
            pointedOutline = outline;
            break;
          }
        }

        if (dragging)
        {
          currOutline.updatePoint(p);
          Invalidate();
        }

        if (pointedOutline != null)
        {
          if (pointedOutline is SegmentedOutline)
            if ((pointedOutline as SegmentedOutline).PointedBezierSegment(p))
              Invalidate();

          if (pointedOutline is Ellipse)
            if ((pointedOutline as Ellipse).isPointedEllipse(p))
              Invalidate();
        }
      }
    }

    protected override void OnMouseUp(System.Windows.Forms.MouseEventArgs e)
    {
      if (e.Button == MouseButtons.Left)
      {
        if (flagShowGrid)
          setToGrigSelectedPoint();
        onShapeChanged(this, null);
      }
      dragging = false;
    }

    private void setToGrigSelectedPoint()
    {
      for (int i = this.ClientRectangle.Left; i < this.ClientRectangle.Right + scrollX; i += gridSize)
        for (int j = this.ClientRectangle.Top; j < this.ClientRectangle.Bottom + scrollY; j += gridSize)
          if (dragging && Math.Abs(SaveCurrX - i) < gridSize / 2 + 1 && Math.Abs(SaveCurrY - j) < gridSize / 2 + 1)
          {
            currOutline.updatePoint(new PointF(i, j));
            Invalidate();
            break;
          }
    }

    protected override void OnCreateControl()
    {
      if (ShapeChanged != null)
        ShapeChanged(this, null);
    }

    protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs pevent)
    {
      return;
    }

    public ShapeTemplate createShapeTemplate()
    {
      int count = 0;
      ArrayList alDecorations = new ArrayList();
      RectangleF rect = getBoundingRect();
      ElementTemplate[] etContour = null;
      ElementTemplate[] etTextArea = null;
      ElementTemplate[] etDecorations = null;

      etContour = ((Outline)outLines[0]).getElements(rect);
      for (int i = 1; i < outLines.Count; ++i)
      {
        Outline outLine = (Outline)outLines[i];
        if (outLine is Contour)
          etTextArea = outLine.getElements(rect);
        else
        {
          etDecorations = outLine.getElements(rect);
          alDecorations.Add(etDecorations);
          count += etDecorations.Length;
        }
      }

      ElementTemplate[] allDecorations = new ElementTemplate[count];

      count = 0;
      foreach (ElementTemplate[] eta in alDecorations)
      {
        eta.CopyTo(allDecorations, count);
        count += eta.Length;
      }

      return new ShapeTemplate(etContour, allDecorations, etTextArea, System.Drawing.Drawing2D.FillMode.Winding, "test");
    }

    public void setGraphicThing()
    {
      ArrayList alDecorations = new ArrayList();
      RectangleF rect = getBoundingRect();

      graphicThing.elements = new ArrayList();
      ((Outline)outLines[0]).getElements(rect, graphicThing.elements);

      graphicThing.decorations = new ArrayList();
      for (int i = 1; i < outLines.Count; ++i)
      {
        Outline outLine = (Outline)outLines[i];
        if (outLine is Contour)
        {
          graphicThing.textArea = new ArrayList();
          outLine.getElements(rect, graphicThing.textArea);
        }
        else
        {
          outLine.getElements(rect, graphicThing.decorations);
        }
      }
    }
  }
}
