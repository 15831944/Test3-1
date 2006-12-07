using System;
using System.Windows.Forms;
using System.Collections;
using System.Drawing;
using System.IO;


namespace SysCAD.ThingEditor
{
  /// <summary>
  /// 
  /// </summary>
  public abstract class Outline
  {
    internal int PointsX;
    internal int PointsY;
    internal int PointsWidth;
    internal int PointsHeight;

    internal float width;
    internal float height;

    public Outline()
    {
      PointsX = PointsY = 3;
      PointsWidth = PointsHeight = 6;

      width = 100.0F;
      height = 50.0F;

      // 
      // TODO: Add constructor logic here
      //
    }

    internal abstract void addMenuItems(ContextMenu cntMenu, PointF p);
    internal abstract bool checkOverPoint(PointF p);
    internal abstract void Draw(Graphics g, Pen p);
    internal abstract bool hitTest(PointF p);
    internal abstract int getSegmentIndex(PointF p);
    internal abstract void updatePoint(PointF p);
    internal abstract void drawPoints(Graphics g, Brush br);
    internal abstract string saveNameAsText();
    internal abstract void Load(BinaryReader br);
    internal abstract void Save(BinaryWriter br);
    internal abstract float getPercent(float Min, float Max, float Coord);
    internal abstract RectangleF getBoundingRect();
    internal abstract MindFusion.FlowChartX.ElementTemplate[] getElements(RectangleF rect);
    internal abstract void getElements(RectangleF rect, ArrayList list);
  }
}
