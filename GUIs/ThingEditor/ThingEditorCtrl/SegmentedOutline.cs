using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.IO;


namespace SysCAD.ThingEditor
{
  /// <summary>
  /// 
  /// </summary>
  public delegate void OutlineChangeEventHandler(object sender, EventArgs e);

  public class SegmentedOutline : Outline, IDisposable
  {


    protected ArrayList shapeSegments;
    protected Segment currSegment;
    private Segment belongSegment;

    private Segment saveSegment;

    private MenuItem menuItemSplit;
    private MenuItem menuItemToLine;
    private MenuItem menuItemToBezier;
    private MenuItem menuItemConvertTo;
    private MenuItem menuItemSeparator1;
    private MenuItem menuItemSeparator2;
    private MenuItem menuItemDeleteSegment;

    private bool showPoints;
    private int distanceToLine;
    private int MinNumLines;
    private int saveIndex;
    private float saveNewPosX;
    private float saveNewPosY;
    private PointF saveCurrPos;
    private PointF saveStartPoint;
    private PointF saveEndPoint;

    public event OutlineChangeEventHandler Changed;

    protected Segment BelongSegment
    {
      get { return belongSegment; }
      set { belongSegment = value; }
    }


    public bool hasSegments()
    {
      return shapeSegments.Count > 0;
    }

    public SegmentedOutline()
    {
      showPoints = true;
      MinNumLines = 3;
      distanceToLine = 10;
    }

    internal bool PointedBezierSegment(PointF p)
    {
      foreach (Segment s in shapeSegments)
        if (s is BezierSegment)
          if ((s as BezierSegment).isPointedBezierSegment(p))
            return true;
      return false;
    }

    private void splitToLine()
    {
      LineSegment lineSegment = new LineSegment(saveStartPoint, new PointF(saveNewPosX, saveNewPosY));
      LineSegment lineSegmentNext = new LineSegment(new PointF(saveNewPosX, saveNewPosY), saveEndPoint);

      if (saveSegment is LineSegment && (saveSegment as LineSegment).IsDecorationSegment)
      {
        lineSegment.IsDecorationSegment = true;
        lineSegmentNext.IsDecorationSegment = true;
      }


      shapeSegments.Remove(saveSegment);
      shapeSegments.Insert(saveIndex, lineSegment);
      shapeSegments.Insert(saveIndex + 1, lineSegmentNext);
      if (Changed != null)
        Changed(this, new EventArgs());
    }

    internal void splitToBezier()
    {
      PointF pt0, pt3;

      pt0 = new PointF(saveNewPosX, saveNewPosY);
      pt3 = saveEndPoint;

      BezierSegment newBezierSegment = new BezierSegment();
      LineSegment newLineSegment = new LineSegment(saveStartPoint, new PointF(saveNewPosX, saveNewPosY));

      if (saveSegment is LineSegment && (saveSegment as LineSegment).IsDecorationSegment)
      {
        newBezierSegment.IsDecorationSegment = true;
        newLineSegment.IsDecorationSegment = true;
      }

      shapeSegments.Remove(saveSegment);
      shapeSegments.Insert(saveIndex, newLineSegment);


      newBezierSegment.points[0] = pt0;
      newBezierSegment.points[1] = new PointF(((pt0.X) * 2 + pt3.X) / 3, ((pt0.Y) * 2 + pt3.Y) / 3);
      newBezierSegment.points[2] = new PointF(((pt0.X) / 2 + pt3.X) * 2 / 3, ((pt0.Y) / 2 + pt3.Y) * 2 / 3);
      newBezierSegment.points[3] = pt3;

      shapeSegments.Insert(saveIndex + 1, newBezierSegment);
      if (Changed != null)
        Changed(this, new EventArgs());
    }

    private void CovertTo()
    {
      if (saveSegment is LineSegment)
      {
        BezierSegment newBezierSeg = new BezierSegment();

        if ((saveSegment as LineSegment).IsDecorationSegment)
          newBezierSeg.IsDecorationSegment = true;

        shapeSegments.Remove(saveSegment);

        newBezierSeg.points[0] = saveStartPoint;
        newBezierSeg.points[1] = new PointF(((saveStartPoint.X) * 2 + saveEndPoint.X) / 3, ((saveStartPoint.Y) * 2 + saveEndPoint.Y) / 3);
        newBezierSeg.points[2] = new PointF(((saveStartPoint.X) / 2 + saveEndPoint.X) * 2 / 3, ((saveStartPoint.Y) / 2 + saveEndPoint.Y) * 2 / 3);
        newBezierSeg.points[3] = saveEndPoint;
        shapeSegments.Insert(saveIndex, newBezierSeg);
      }
      if (saveSegment is BezierSegment)
      {
        LineSegment newLineSegment = new LineSegment(saveStartPoint, saveEndPoint);

        if ((saveSegment as BezierSegment).IsDecorationSegment)
          newLineSegment.IsDecorationSegment = true;

        shapeSegments.Remove(saveSegment);
        shapeSegments.Insert(saveIndex, newLineSegment);
      }
      if (Changed != null)
        Changed(this, new EventArgs());
    }

    private void deleteSegment(int Index)
    {
      Segment saveNextSegment = (Segment)shapeSegments[(Index + 1) % shapeSegments.Count];
      shapeSegments.Remove(saveSegment);
      saveNextSegment.setStartPoint(saveStartPoint);
    }

    private void DeleteSegment()
    {
      if (saveSegment is LineSegment && (saveSegment as LineSegment).IsDecorationSegment)
      {
        if (shapeSegments.Count > 2)
        {
          if ((saveIndex + 1) == shapeSegments.Count)
            shapeSegments.Remove(saveSegment);
          else
            deleteSegment(saveIndex);
        }
        else
          shapeSegments.Remove(saveSegment);
      }
      else
        if (saveSegment is BezierSegment && (saveSegment as BezierSegment).IsDecorationSegment)
        {
          if (shapeSegments.Count > 2)
          {
            if ((saveIndex + 1) == shapeSegments.Count)
              shapeSegments.Remove(saveSegment);
            else
              deleteSegment(saveIndex);
          }
          else
            shapeSegments.Remove(saveSegment);
        }
        else
          if (shapeSegments.Count > 3)
            deleteSegment(saveIndex);

      if (Changed != null)
        Changed(this, new EventArgs());
    }

    internal void ModifyMenuText(MenuItem menuItemConvertTo)
    {
      if (saveSegment is LineSegment)
        menuItemConvertTo.Text = "Convert To Bezier";
      if (saveSegment is BezierSegment)
        menuItemConvertTo.Text = "Convert To Line";
    }

    internal void combineSegments(float SaveCurrX, float SaveCurrY)
    {
      PointF pStart;
      PointF pEnd;
      if (shapeSegments.Count > MinNumLines)
      {
        for (int i = 0; i < shapeSegments.Count; ++i)
        {
          Segment s1 = (Segment)shapeSegments[i];
          Segment s2 = (Segment)shapeSegments[(i + 1) % shapeSegments.Count];
          if (s1.hitTest(new PointF(SaveCurrX, SaveCurrY)) && s2.hitTest(new PointF(SaveCurrX, SaveCurrY)))
          {
            saveIndex = i;
            pStart = s1.GetStartPoint();
            pEnd = s2.GetEndPoint();
            shapeSegments.Remove(s1);
            shapeSegments.Remove(s2);
            Segment sCombinedLine = new LineSegment(pStart, pEnd);
            shapeSegments.Insert(i, sCombinedLine);
            break;
          }
        }
      }
    }

    internal override void drawPoints(Graphics g, Brush br)
    {
      foreach (Segment s in shapeSegments)
        s.drawPoints(g, br, s.GetEndPoint());
    }

    internal override void Draw(Graphics g, Pen pen)
    {
      foreach (Segment s in shapeSegments)
        s.Draw(g, pen);
    }

    internal void storeCurrMousePos(PointF p)
    {
      saveCurrPos = p;
    }

    internal override bool hitTest(PointF p)
    {
      for (int i = 0; i < shapeSegments.Count; ++i)
      {
        Segment s1 = (Segment)shapeSegments[i];
        Segment s2 = (Segment)shapeSegments[(i + 1) % shapeSegments.Count];
        if (s1.hitTest(p))
        {
          if (!s1.checkCommonPointStart() && !s1.checkCommonPointEnd() && showPoints)
          {
            currSegment = s1;
            belongSegment = null;
            return true;
          }
          else
            if (s1.checkCommonPointEnd() && s2.hitTest(p) && showPoints)
            {
              currSegment = s1;
              belongSegment = s2;
              return true;
            }
        }
      }
      return false;
    }

    internal string[] getNamesAsText()
    {
      string[] Names = new string[shapeSegments.Count];
      for (int i = 0; i < shapeSegments.Count; ++i)
      {
        Segment s = (Segment)shapeSegments[i];
        Names[i] = s.saveNameAsText();
      }
      return Names;
    }

    internal ArrayList getSegments()
    {
      return shapeSegments;
    }

    internal override int getSegmentIndex(PointF p)
    {
      int Value = -1;

      for (int i = 0; i < shapeSegments.Count; ++i)
      {
        Segment s = (Segment)shapeSegments[i];
        if (s.distanceToPoint(p) < distanceToLine)
        {
          saveSegment = s;
          saveStartPoint = s.GetStartPoint();
          saveEndPoint = s.GetEndPoint();
          saveIndex = i;
          Value = i;
          break;
        }
      }
      return Value;
    }


    internal bool isAllowCombineCntMenu(PointF p)
    {
      foreach (Segment s in shapeSegments)
      {
        if (s is LineSegment)
          if ((s as LineSegment).IsDecorationSegment)
            return false;

        if (s is BezierSegment)
          if ((s as BezierSegment).IsDecorationSegment)
            return false;
      }
      return true;
    }

    internal override bool checkOverPoint(PointF p)
    {
      foreach (Segment s in shapeSegments)
        if (s.checkOverPoint(p))
          return true;
      return false;
    }

    internal override void updatePoint(PointF p)
    {
      currSegment.updatePoint(p);
      if (belongSegment != null)
        belongSegment.updatePoint(p);
    }

    internal override void addMenuItems(ContextMenu cntMenu, PointF p)
    {
      saveNewPosX = p.X;
      saveNewPosY = p.Y;
      cntMenu.MenuItems.Clear();
      menuItemSplit = new MenuItem("&Split");
      menuItemToLine = new MenuItem("To Line");
      menuItemToBezier = new MenuItem("To Bezier");

      menuItemSeparator1 = new MenuItem("-");
      menuItemConvertTo = new MenuItem("Convert to Line");

      menuItemSeparator2 = new MenuItem("-");
      menuItemDeleteSegment = new MenuItem("&Delete");

      cntMenu.MenuItems.Add(menuItemSplit);
      menuItemSplit.MenuItems.Add(menuItemToLine);
      menuItemSplit.MenuItems.Add(menuItemToBezier);

      cntMenu.MenuItems.Add(menuItemSeparator1);
      cntMenu.MenuItems.Add(menuItemConvertTo);
      cntMenu.MenuItems.Add(menuItemSeparator2);

      cntMenu.MenuItems.Add(menuItemDeleteSegment);
      ModifyMenuText(menuItemConvertTo);

      menuItemToLine.Click += new EventHandler(menuItemSplitToLineOnClick);
      menuItemToBezier.Click += new EventHandler(menuItemSplitToBezierOnClick);
      menuItemConvertTo.Click += new EventHandler(menuItemConvertToOnClick);
      menuItemDeleteSegment.Click += new EventHandler(menuItemDeleteSegmentOnClick);
    }

    private void menuItemSplitToLineOnClick(object Sender, System.EventArgs e)
    {
      splitToLine();
    }

    private void menuItemSplitToBezierOnClick(object Sender, System.EventArgs e)
    {
      splitToBezier();
    }

    private void menuItemConvertToOnClick(object Sender, System.EventArgs e)
    {
      CovertTo();
    }

    private void menuItemDeleteSegmentOnClick(object Sender, System.EventArgs e)
    {
      DeleteSegment();
    }

    internal override string saveNameAsText()
    {
      return null;
    }

    internal override void Load(BinaryReader br)
    {
      int shapeSegmentsCount = br.ReadInt32();

      for (int i = 0; i < shapeSegmentsCount; ++i)
      {
        string ID = br.ReadString();
        if (ID == "L")
          shapeSegments.Add(new LineSegment(br));
        if (ID == "B")
          shapeSegments.Add(new BezierSegment(br));
      }
    }

    internal override void Save(BinaryWriter bw)
    {
      bw.Write(shapeSegments.Count);
      for (int i = 0; i < shapeSegments.Count; ++i)
      {
        Segment s = (Segment)shapeSegments[i];
        if (s is LineSegment)
          bw.Write("L");
        if (s is BezierSegment)
          bw.Write("B");
        s.Save(bw);
      }
    }

    internal override float getPercent(float Min, float Max, float Coord)
    {
      return 0.0F;
    }

    internal override RectangleF getBoundingRect()
    {
      // return the big rectangle combined from all segments -  beziers  and lines
      RectangleF outlineRect = new RectangleF(0, 0, 0, 0);
      Segment s;
      s = (Segment)shapeSegments[0];
      outlineRect = s.getBoundingRect();
      for (int i = 1; i < shapeSegments.Count; ++i)
      {
        s = (Segment)shapeSegments[i];
        outlineRect = RectangleF.Union(outlineRect, s.getBoundingRect());
      }
      return outlineRect;
    }

    internal override MindFusion.FlowChartX.ElementTemplate[] getElements(RectangleF rect)
    {
      MindFusion.FlowChartX.ElementTemplate[] et = new MindFusion.FlowChartX.ElementTemplate[shapeSegments.Count];

      for (int i = 0; i < shapeSegments.Count; ++i)
        et[i] = ((Segment)shapeSegments[i]).CreateElement(rect);

      return et;
    }

    internal override void getElements(RectangleF rect, ArrayList list)
    {
      for (int i = 0; i < shapeSegments.Count; ++i)
        ((Segment)shapeSegments[i]).CreateElement(rect, list);

      //!!!!!return et;
    }

    #region IDisposable Members

    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    }

    protected virtual void Dispose(bool disposing)
    {
      if (disposing)
      {
        // dispose managed resources
        menuItemSplit.Dispose();
        menuItemToLine.Dispose();
        menuItemToBezier.Dispose();
        menuItemConvertTo.Dispose();
        menuItemSeparator1.Dispose();
        menuItemSeparator2.Dispose();
        menuItemDeleteSegment.Dispose();
      }
      // free native resources
    }

    #endregion
  }
}

