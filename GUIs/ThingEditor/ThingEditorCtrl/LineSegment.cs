using System;
using System.Drawing;
using System.IO;
using System.Collections;
using SysCAD.Interface;

namespace SysCAD.ThingEditor.ThingEditorCtrl
{
	/// <summary>
	/// 
	/// </summary>
	public class LineSegment : Segment
	{
		private PointF pt1;
    private PointF pt2;
        private int Index;

        public bool isDecorationSegment;
// Constructor
		public LineSegment(PointF p1, PointF p2): base()
		{
			pt1 = p1;
			pt2 = p2;
            isDecorationSegment = false;
        }

        public LineSegment(BinaryReader br): base()
        {
            isDecorationSegment = false;
            Load(br);
        }

// end Constructor

		public PointF start
		{
			get
			{
				return pt1;
			}
			set
			{
				pt1 = value; 
			}
		}

		public PointF end
		{
			get
			{
				return pt2;
			}
			set
			{
				pt2 = value;
			}
		}

        internal override void Draw(Graphics g, Pen pen)
		{
			g.DrawLine(pen, start, end);
		}

		internal override PointF GetStartPoint()
		{
			return start;
		}

		internal override PointF GetEndPoint()
		{
			return end;
		}

		internal override bool hitTest(PointF p)
		{
            if (Math.Abs(p.X - start.X) < 5 && Math.Abs(p.Y - start.Y) < 5)
            {
                Index = 0;
                return true;
            }
            if (Math.Abs(p.X - end.X) < 5 && Math.Abs(p.Y - end.Y) < 5)
            {
                Index = 1;
                return true;
            }
            return false;
		}

		internal override void updatePoint(PointF p)
		{
            switch (Index)
            {
                case 0:
                    pt1.X = p.X;
                    pt1.Y = p.Y;
                    break;
                case 1:
                    pt2.X = p.X;
                    pt2.Y = p.Y;
                    break;
            }
		}

		internal override bool checkCommonPointEnd()
		{
			if (Index == 1)
				return true;
			else
				return false;
		}

		internal override bool checkCommonPointStart()
		{
			if (Index == 0)
				return true;
			else 
				return false;
		}

		internal override void drawPoints(Graphics g, Brush br, PointF p)
		{
			g.FillEllipse(br, p.X - PointsX, 
				p.Y - PointsY, 
				PointsWidth, 
				PointsHeight);

            g.FillEllipse(br, start.X - PointsX, 
                start.Y - PointsY, 
                PointsWidth, 
                PointsHeight);
        }

        internal override bool checkOverPoint(PointF p)
        {
            if (Math.Abs(p.X - start.X) < 5 && Math.Abs(p.Y - start.Y) < 5)
                return true;
            if (Math.Abs(p.X - end.X) < 5 && Math.Abs(p.Y - end.Y) < 5)
                return true;
            return false; 
        }

        internal override int distanceToPoint(PointF p)
        {
            double nDist, nMinDist = 1000000;

            double x1 = start.X;
            double y1 = start.Y;
            double x2 = end.X;
            double y2 = end.Y;

            double A = y1 - y2;
            double B = x2 - x1;
            double C = x1*y2 - y1*x2;

            if (Math.Abs(A) < 0.00001 && Math.Abs(B) < 0.00001)
            {
                nDist = Math.Sqrt((x1-p.X)*(x1-p.X) + (y1-p.Y)*(y1-p.Y));
            }
            else
                if (Math.Abs(A) < 0.00001)
                {
                    if ((p.X > x1 && p.X > x2) || (p.X < x1 && p.X < x2))
                    {
                        nDist = Math.Min(
                            Math.Sqrt((x1-p.X)*(x1-p.X) + (y1-p.Y)*(y1-p.Y)),
                            Math.Sqrt((x2-p.X)*(x2-p.X) + (y2-p.Y)*(y2-p.Y)));
                    }
                    else
                    {
                        nDist = Math.Abs(y1 - p.Y);
                    }
                }
                else
                {
                    double xo, yo;

                    yo = (A*A*p.Y - A*B*p.X - B*C) / (A*A + B*B);
                    xo = (-C - B*yo) / A;

                    if ( (xo - x1)*(xo - x2) <= 0 && (yo - y1)*(yo - y2) <= 0)
                    {
                        nDist = Math.Sqrt((p.X - xo)*(p.X - xo) + (p.Y - yo)*(p.Y - yo));
                    }
                    else
                    {
                        nDist = Math.Min(
                            Math.Sqrt((x1-p.X)*(x1-p.X) + (y1-p.Y)*(y1-p.Y)),
                            Math.Sqrt((x2-p.X)*(x2-p.X) + (y2-p.Y)*(y2-p.Y)));
                    }
                }

                if (nMinDist > nDist)
                    nMinDist = nDist;

            return (int)nMinDist;
        }

        internal override void setEndPoint(PointF p)
        {
            end = p;
        }

        internal override void setStartPoint(PointF p)
        {
            start = p;
        }

        internal override string saveNameAsText()
        {
            int startX = (int)getPercent(ThingEditor.MinX, ThingEditor.MaxX, start.X);
            int startY = (int)getPercent(ThingEditor.MinY, ThingEditor.MaxY, start.Y);
            int endX   = (int)getPercent(ThingEditor.MinX, ThingEditor.MaxX, end.X);
            int endY   = (int)getPercent(ThingEditor.MinY, ThingEditor.MaxY, end.Y);

            return "            new LineTemplate(" + (startX).ToString() + ", " + 
                                                     (startY).ToString() + ", " + 
                                                     (endX).ToString()   + ", " +
                                                     (endY).ToString()   + ")";
        }

        internal override void Load(System.IO.BinaryReader br)
        {
            pt1.X = br.ReadInt32();
            pt1.Y = br.ReadInt32();
            pt2.X   = br.ReadInt32();
            pt2.Y   = br.ReadInt32();
        }

        internal override void Save(System.IO.BinaryWriter bw)
        {
            bw.Write(start.X);
            bw.Write(start.Y);
            bw.Write(end.X);
            bw.Write(end.Y);
        }

        internal override float getPercent(float Min, float Max, float Coord)
        {
            return (Coord - Min)/(Max - Min)*100.0F;
        }

        internal override void setPoint(System.Drawing.PointF p)
        {
        
        }

        internal override RectangleF getBoundingRect()
        {
            return new RectangleF(
                Math.Min(start.X, end.X), 
                Math.Min(start.Y, end.Y),
                Math.Abs(start.X - end.X),
                Math.Abs(start.Y - end.Y)
                                );
        }

        internal override MindFusion.FlowChartX.ElementTemplate CreateElement(RectangleF rect)
        {
          float x1 = getPercent(rect.Left, rect.Right, pt1.X);
          float y1 = getPercent(rect.Top, rect.Bottom, pt1.Y);
          float x2 = getPercent(rect.Left, rect.Right, pt2.X);
          float y2 = getPercent(rect.Top, rect.Bottom, pt2.Y);

          return new MindFusion.FlowChartX.LineTemplate(x1, y1, x2, y2);
        }

        internal override void CreateElement(RectangleF rect, ArrayList list)
        {
          float x1 = getPercent(rect.Left, rect.Right, pt1.X);
          float y1 = getPercent(rect.Top, rect.Bottom, pt1.Y);
          float x2 = getPercent(rect.Left, rect.Right, pt2.X);
          float y2 = getPercent(rect.Top, rect.Bottom, pt2.Y);

          list.Add(new Line(x1, y1, x2, y2));
        }
      }
}
