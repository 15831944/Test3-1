using System;
using System.Drawing;
using System.IO;
using System.Collections;
using SysCAD.Interface;

namespace SysCAD.ThingEditor
{
	/// <summary>
	/// 
	/// </summary>
    public class BezierSegment : Segment
    {
        internal PointF[] points;

        private int Index;
        private bool pointedBezier;

        private bool isDecorationSegment;
        
// Constructor
        public BezierSegment(): base()
        {
            pointedBezier       = false;
            points              = new PointF[4];
            isDecorationSegment = false;
        }

        public BezierSegment(BinaryReader br): base()
        {
            pointedBezier       = false;
            points              = new PointF[4];
            isDecorationSegment = false;
            Load(br);
        }

// end Constructor
      public bool IsDecorationSegment
        {
          get
          {
            return isDecorationSegment;
          }
          set
          {
            isDecorationSegment = value;
          }
        }

        public PointF Start
        {
          get
          {
            return points[0];
          }
          set
          {
            points[0] = value;
          }
        }

        public PointF End
        {
            get
            {
                return points[3];
            }
            set
            {
                points[3] = value;
            }
        }

        internal override void Draw(Graphics g, Pen pen)
        {
            if (pointedBezier)
            {
                pen = new Pen(Color.Yellow, 1);
                g.DrawBeziers(pen, points);
                pen.Dispose();
            }
            else
                g.DrawBeziers(pen, points);
        }

        internal override PointF GetStartPoint()
        {
            return Start;
        }
        
        internal override PointF GetEndPoint()
        {
            return End;
        }

        internal override bool hitTest(PointF p)
        {
            for (int i = 0; i < points.Length; ++i)
                if (Math.Abs(p.X - points[i].X) < 5 && Math.Abs(p.Y - points[i].Y) < 5)
                {
                    Index = i;
                    return true;
                }
            return false;
        }

        internal override void updatePoint(System.Drawing.PointF p)
        {
            points[Index] = p;
        }

        internal override bool checkCommonPointEnd()
        {
            if (Index == points.Length -1)
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
            g.FillEllipse(br, points[0].X - PointsX, 
                points[0].Y - PointsY, 
                PointsWidth, 
                PointsHeight);

            g.FillEllipse(br, points[1].X - PointsX, 
                points[1].Y - PointsY, 
                PointsWidth, 
                PointsHeight);

            g.FillEllipse(br, points[2].X - PointsX, 
                points[2].Y - PointsY, 
                PointsWidth, 
                PointsHeight);

            g.FillEllipse(br, points[3].X - PointsX,
                points[3].Y - PointsY, 
                PointsWidth, 
                PointsHeight);
        }

        internal override bool checkOverPoint(PointF p)
        {
            for (int i = 0; i < points.Length; ++i)
                if (Math.Abs(p.X - points[i].X) < 5 && Math.Abs(p.Y - points[i].Y) < 5)
                    return true;
            return false;
        }

        internal bool isPointedBezierSegment(PointF p)
        {
            if (Math.Abs(p.X - points[1].X) < 5 && Math.Abs(p.Y - points[1].Y) < 5)
            {
                pointedBezier = true;
                return true;
            }
            if (Math.Abs(p.X - points[2].X) < 5 && Math.Abs(p.Y - points[2].Y) < 5)
            {
                pointedBezier = true;
                return true;
            }

            pointedBezier = false;
            return false;
        }

        private void calcBezierCoef(ref double a0,  ref double a1,  ref double a2,  ref double a3,
                                    ref double b0,  ref double b1,  ref double b2,  ref double b3,
                                    ref double u ,  ref double s,   ref double z,   ref double x4,  ref double y4)
        {
            double x = a0 + u*(a1 + u*(a2 + u*a3));
            double y = b0 + u*(b1 + u*(b2 + u*b3));
            double dx4 = x - x4;
            double dy4 = y - y4;
            double dx = a1 + u*(2*a2 + u*3*a3);
            double dy = b1 + u*(2*b2 + u*3*b3);
            z = dx*dx4 + dy*dy4;
            s = dx4*dx4 + dy4*dy4;
        }

        internal override int distanceToPoint(PointF p)
        {
            float nRes = 1000000;

            double x0 = points[0].X;
            double y0 = points[0].Y;
            double x1 = points[1].X;
            double y1 = points[1].Y;
            double x2 = points[2].X;
            double y2 = points[2].Y;
            double x3 = points[3].X;
            double y3 = points[3].Y;

            double a3 = (x3 - x0 + 3*(x1-x2)) / 8;
            double b3 = (y3 - y0 + 3*(y1-y2)) / 8;
            double a2 = (x3 + x0 - x1 - x2) * 3 / 8;
            double b2 = (y3 + y0 - y1 - y2) * 3 / 8;
            double a1 = (x3 - x0) / 2 - a3;
            double b1 = (y3 - y0) / 2 - b3;
            double a0 = (x3 + x0) / 2 - a2;
            double b0 = (y3 + y0) / 2 - b2;

            double x4 = p.X;
            double y4 = p.Y;

            double s1, s=0, u1, u2, z=0, z1, z2;
            s1 = 0;
            u2 = u1 = -1.0;
            z1 = z2 = 0;

            double stepsize = 2.0 / 9;
            double u = 0;
            for (u = -1.0; u < 1.0; u += stepsize)
            {
                calcBezierCoef(ref a0, ref a1, ref a2, ref a3, ref b0, ref b1, ref b2, ref b3, ref u, ref s, ref z, ref x4, ref y4);
                if (Math.Abs(s) < 0.00001)
                {
                    u1 = u; 
                    z1 = z; 
                    s1 = s; 
                    break;
                }
                if (Math.Abs(u + 1.0) < 0.00001)
                {
                    u1 = u; 
                    z1 = z; 
                    s1 = s;
                }
                if (s < s1)
                {
                    u1 = u; 
                    z1 = z; 
                    s1 = s;
                }
            }

            if (Math.Abs(s1) > 0.00001)
            {
                u = u1 + stepsize;
                if (u > 1.0) u = 1.0 - stepsize;
                for(int cnt = 0 ; cnt < 20; cnt++)
                {
                    calcBezierCoef(ref a0, ref a1, ref a2, ref a3, ref b0, ref b1, ref b2, ref b3, ref u, ref s, ref z, ref x4, ref y4);
                    if (Math.Abs(s) < 0.00001) 
                        break;
                    if (Math.Abs(z) < 0.00001) 
                        break;
                    u2 = u;
                    z2 = z;
                    double temp = z2 - z1;
                    if (Math.Abs(temp) > 0.00001)
                    {
                        u = (z2*u1 - z1*u2) / temp;
                    }
                    else
                    {
                        u = (u1 + u2) / 2;
                    }
                    if (u > 1.0)
                    {
                        u = 1.0;
                    }
                    else if (u < -1.0)
                    {
                        u = -1.0;
                    }
                    if (Math.Abs(u - u2) < 0.0001) 
                        break;
                    u1 = u2;
                    z1 = z2;
                }
            }

            if (nRes > Math.Sqrt(s)) nRes = (float)Math.Sqrt(s);
            if (nRes == 0) 
                return 0;
        return (int)nRes;
        }

        internal override void setEndPoint(PointF p)
        {
            End = p;
        }

        internal override void setStartPoint(PointF p)
        {
            Start = p;
        }

        internal override string saveNameAsText()
        {
            int points0X = (int)getPercent(ThingEditorCtrl.MinX, ThingEditorCtrl.MaxX, points[0].X);
            int points0Y = (int)getPercent(ThingEditorCtrl.MinY, ThingEditorCtrl.MaxY, points[0].Y);
            int points1X = (int)getPercent(ThingEditorCtrl.MinX, ThingEditorCtrl.MaxX, points[1].X);
            int points1Y = (int)getPercent(ThingEditorCtrl.MinY, ThingEditorCtrl.MaxY, points[1].Y);
            int points2X = (int)getPercent(ThingEditorCtrl.MinX, ThingEditorCtrl.MaxX, points[2].X);
            int points2Y = (int)getPercent(ThingEditorCtrl.MinY, ThingEditorCtrl.MaxY, points[2].Y);
            int points3X = (int)getPercent(ThingEditorCtrl.MinX, ThingEditorCtrl.MaxX, points[3].X);
            int points3Y = (int)getPercent(ThingEditorCtrl.MinY, ThingEditorCtrl.MaxY, points[3].Y);

            return "            new BezierTemplate(" + (points0X).ToString() + ", " + 
                                                       (points0Y).ToString() + ", " +
                                                       (points1X).ToString() + ", " +
                                                       (points1Y).ToString() + ", " +
                                                       (points2X).ToString() + ", " +
                                                       (points2Y).ToString() + ", " +
                                                       (points3X).ToString() + ", " +
                                                       (points3Y).ToString() + ")";
        }

        internal override void Load(System.IO.BinaryReader br)
        {
            points[0].X   = br.ReadInt32();
            points[0].Y   = br.ReadInt32();
            points[1].X   = br.ReadInt32();
            points[1].Y   = br.ReadInt32();
            points[2].X   = br.ReadInt32();
            points[2].Y   = br.ReadInt32();
            points[3].X   = br.ReadInt32();
            points[3].Y   = br.ReadInt32();
        }

        internal override void Save(System.IO.BinaryWriter bw)
        {
            bw.Write(Start.X);
            bw.Write(Start.Y);
            bw.Write(points[1].X);
            bw.Write(points[1].Y);
            bw.Write(points[2].X);
            bw.Write(points[2].Y);
            bw.Write(End.X);
            bw.Write(End.Y);
        }

      internal override float getPercent(float Min, float Max, float Coord)
        {
            return (Coord - Min)/(Max - Min)*100.0F;
        }

        internal override void setPoint(System.Drawing.PointF p)
        {
        
        }


        internal override MindFusion.FlowChartX.ElementTemplate CreateElement(RectangleF rect)
        {
          float x1 = getPercent(rect.Left, rect.Right, points[0].X);
          float y1 = getPercent(rect.Top, rect.Bottom, points[0].Y);
          float x2 = getPercent(rect.Left, rect.Right, points[1].X);
          float y2 = getPercent(rect.Top, rect.Bottom, points[1].Y);
          float x3 = getPercent(rect.Left, rect.Right, points[2].X);
          float y3 = getPercent(rect.Top, rect.Bottom, points[2].Y);
          float x4 = getPercent(rect.Left, rect.Right, points[3].X);
          float y4 = getPercent(rect.Top, rect.Bottom, points[3].Y);

          return new MindFusion.FlowChartX.BezierTemplate(x1, y1, x2, y2, x3, y3, x4, y4);
        }

        internal override void CreateElement(RectangleF rect, ArrayList list)
        {
          float x1 = getPercent(rect.Left, rect.Right, points[0].X);
          float y1 = getPercent(rect.Top, rect.Bottom, points[0].Y);
          float x2 = getPercent(rect.Left, rect.Right, points[1].X);
          float y2 = getPercent(rect.Top, rect.Bottom, points[1].Y);
          float x3 = getPercent(rect.Left, rect.Right, points[2].X);
          float y3 = getPercent(rect.Top, rect.Bottom, points[2].Y);
          float x4 = getPercent(rect.Left, rect.Right, points[3].X);
          float y4 = getPercent(rect.Top, rect.Bottom, points[3].Y);

          list.Add(new Bezier(x1, y1, x2, y2, x3, y3, x4, y4));
        }

        internal override RectangleF getBoundingRect()
        {
            float epsilon = 0.05F;

            double MaxX = points[0].X;
            double MaxY = points[0].Y;
            double MinX = points[0].X;
            double MinY = points[0].Y;

            double X;
            double Y;
                
            for (float t = 0; t <= 1; t += epsilon)
            {
                X = Math.Pow((1 - t), 3) * points[0].X + 
                    3 * t * Math.Pow((1 - t), 2) * points[1].X + 
                    3 * t * t * (1 -t) * points[2].X + 
                    Math.Pow(t, 3) * points[3].X;

                Y = Math.Pow((1 - t), 3) * points[0].Y + 
                    3 * t * Math.Pow((1 - t), 2) * points[1].Y + 
                    3 * t * t * (1 -t) * points[2].Y + 
                    Math.Pow(t, 3) * points[3].Y;
                
                if (MaxX < X)
                    MaxX = X;

                if (MinX > X)
                    MinX = X;

                if (MaxY < Y)
                    MaxY = Y;

                if (MinY > Y)
                    MinY = Y;
            }

            return new RectangleF(
                (int)MinX,
                (int)MinY,
                Math.Abs((int)(MaxX - MinX)),
                Math.Abs((int)(MaxY - MinY))                                );
        }
	}
}
