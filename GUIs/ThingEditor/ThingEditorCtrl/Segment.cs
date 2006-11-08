using System;
using System.Drawing;
using System.Collections;
using System.IO;

namespace SysCAD.ThingEditor.ThingEditorCtrl
{
	/// <summary>
	/// Summary description for Segment.
	/// </summary>
	
	public abstract class Segment
	{
        internal int PointsX;
        internal int PointsY;
        internal int PointsWidth;
        internal int PointsHeight;

        public Segment()
		{
            PointsX = PointsY = 3;
            PointsWidth = PointsHeight = 6;
			//
			// TODO: Add constructor logic here
			//
		}
		internal abstract void Draw(Graphics g, Pen p);
		internal abstract PointF GetEndPoint();
		internal abstract PointF GetStartPoint();
		internal abstract bool hitTest(PointF p);
		internal abstract bool checkCommonPointStart();
		internal abstract bool checkCommonPointEnd();
        internal abstract bool checkOverPoint(PointF p);
		internal abstract void updatePoint(PointF p);
		internal abstract void drawPoints(Graphics g, Brush br, PointF pt);
        internal abstract int  distanceToPoint(PointF p);
        internal abstract void setStartPoint(PointF p);
        internal abstract void setEndPoint(PointF p);
        internal abstract void setPoint(PointF p);
        internal abstract string saveNameAsText();
        internal abstract void Load(BinaryReader br);
        internal abstract void Save(BinaryWriter bw);
        internal abstract float getPercent(float Min, float Max, float Coord);
        internal abstract RectangleF getBoundingRect();
        internal abstract MindFusion.FlowChartX.ElementTemplate CreateElement(RectangleF rect);
        internal abstract void CreateElement(RectangleF rect, ArrayList list);
      }
}
