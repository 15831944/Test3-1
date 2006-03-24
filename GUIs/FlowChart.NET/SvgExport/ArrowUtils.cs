using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Xml;
using System.Xml.XPath;
using System.Diagnostics;
using MindFusion.FlowChartX;
using System.Runtime.InteropServices;
using MindFusion.FlowChartX.Text;

namespace MindFusion.Diagramming.Export
{
	/// <summary>
	/// Summary description for ArrowUtils.
	/// </summary>
	public class ArrowUtils
	{
		ArrowStyle style;
		PointCollection points = null;
		int SegmentCount=0;
		public ArrowUtils( ArrowStyle pstyle,  PointCollection pPoints )
		{
			points = pPoints.Clone();
			style = pstyle;
			//
			// TODO: Add constructor logic here
			//
		}
		
		public static PointF GetBezierPt(PointCollection points, int segment, float t)
		{
			float x0 = points[segment * 3 + 0].X;
			float y0 = points[segment * 3 + 0].Y;
			float x1 = points[segment * 3 + 1].X;
			float y1 = points[segment * 3 + 1].Y;
			float x2 = points[segment * 3 + 2].X;
			float y2 = points[segment * 3 + 2].Y;
			float x3 = points[segment * 3 + 3].X;
			float y3 = points[segment * 3 + 3].Y;

			float tt = t;

			float q0 = (1-tt)*(1-tt)*(1-tt);
			float q1 = 3*tt*(1-tt)*(1-tt);
			float q2 = 3*tt*tt*(1-tt);
			float q3 = tt*tt*tt;
			float xt = q0*x0 + q1*x1 + q2*x2 + q3*x3;
			float yt = q0*y0 + q1*y1 + q2*y2 + q3*y3;

			return new PointF(xt, yt);
		}

		public static void DekartToPolar(PointF coordCenter,
			PointF dekart, ref float a, ref float r)
		{
			if(coordCenter == dekart)
			{
				a = 0;
				r = 0;
				return;
			}

			float dx = dekart.X - coordCenter.X;
			float dy = dekart.Y - coordCenter.Y;
			r = (float)(Math.Sqrt(Math.Pow(dx, 2) + Math.Pow(dy, 2)));

			a = (float)(Math.Atan(-dy / dx) * 180 / Math.PI);
			if(dx < 0) a += 180;
		}
	
		public  float getSegmentLength(int index )
		{
			switch (style)
			{
				case ArrowStyle.Polyline:
				case ArrowStyle.Cascading:
					return Distance(points[index], points[index + 1]);
				case ArrowStyle.Bezier:
					// approximate the bezier segment to polyline
					int counter = index * 3;
					PointF[] segmentPoints = new PointF[4];
					segmentPoints[0] = points[counter++];
					segmentPoints[1] = points[counter++];
					segmentPoints[2] = points[counter++];
					segmentPoints[3] = points[counter++];
					PointCollection bezierApprox = Utilities.approxBezier(
						segmentPoints, 0, 30);

					// sum up the length of straight segment lengths
					double total = 0;
					for (int s = 0; s < bezierApprox.Count - 1; ++s)
					{
						PointF pt1 = bezierApprox[s];
						PointF pt2 = bezierApprox[s + 1];
						float dx = pt1.X - pt2.X;
						float dy = pt1.Y - pt2.Y;
						total += Math.Sqrt(dx*dx + dy*dy);
					}

					return (float)total;
			}

			return 0;
		}

		/// <summary>
		/// returns the index of a segment that has the longest length
		/// </summary>
		public int getLongestSegment()
		{
			int longest = 0;
			float maxLength = 0;
			for (int i = 0; i < SegmentCount; ++i)
			{
				float length = getSegmentLength(i);
				if (length > maxLength)
				{
					maxLength = length;
					longest = i;
				}
			}

			return longest;
		}

		/// <summary>
		/// returns the center point of a segment
		/// </summary>
		public PointF getSegmentCenter(int index)
		{
			switch (style)
			{
				case ArrowStyle.Bezier:
					int startIdx = index * 3;

					// get the points defining the bezier curve
					float x0 = points[0 + startIdx].X;
					float y0 = points[0 + startIdx].Y;
					float x1 = points[1 + startIdx].X;
					float y1 = points[1 + startIdx].Y;
					float x2 = points[2 + startIdx].X;
					float y2 = points[2 + startIdx].Y;
					float x3 = points[3 + startIdx].X;
					float y3 = points[3 + startIdx].Y;

					// x(t) = (1-t)^3 x0 + 3t(1-t)^2 x1 + 3t^2 (1-t) x2 + t^3 x3
					// y(t) = (1-t)^3 y0 + 3t(1-t)^2 y1 + 3t^2 (1-t) y2 + t^3 y3
					double t = 0.5;
					double q0 = (1-t)*(1-t)*(1-t);
					double q1 = 3*t*(1-t)*(1-t);
					double q2 = 3*t*t*(1-t);
					double q3 = t*t*t;
					double xt = q0*x0 + q1*x1 + q2*x2 + q3*x3;
					double yt = q0*y0 + q1*y1 + q2*y2 + q3*y3;

					return new PointF((float)xt, (float)yt);

				default:
					PointF pt1 = points[index];
					PointF pt2 = points[index + 1];
					return new PointF((pt1.X + pt2.X) / 2, (pt1.Y + pt2.Y) / 2);
			}
		}

		public static PointCollection approxBezier(PointF[] points, int startIdx, int quality)
		{
			PointCollection approximation = new PointCollection(0);
			double epsilon = 1.0 / quality;

			// get the points defining the curve
			float x0 = points[0 + startIdx].X;
			float y0 = points[0 + startIdx].Y;
			float x1 = points[1 + startIdx].X;
			float y1 = points[1 + startIdx].Y;
			float x2 = points[2 + startIdx].X;
			float y2 = points[2 + startIdx].Y;
			float x3 = points[3 + startIdx].X;
			float y3 = points[3 + startIdx].Y;

			// x(t) = (1-t)^3 x0 + 3t(1-t)^2 x1 + 3t^2 (1-t) x2 + t^3 x3
			// y(t) = (1-t)^3 y0 + 3t(1-t)^2 y1 + 3t^2 (1-t) y2 + t^3 y3
			for (double t = 0; t <= 1.0; t += epsilon)
			{
				double q0 = (1-t)*(1-t)*(1-t);
				double q1 = 3*t*(1-t)*(1-t);
				double q2 = 3*t*t*(1-t);
				double q3 = t*t*t;
				double xt = q0*x0 + q1*x1 + q2*x2 + q3*x3;
				double yt = q0*y0 + q1*y1 + q2*y2 + q3*y3;

				// draw straight line between last two calculated points
				approximation.Add(new PointF((float)xt, (float)yt));
			}

			return approximation;
		}

		public static float Distance(PointF pt1, PointF pt2)
		{
			return (float)Math.Sqrt(
				Math.Pow(pt1.X - pt2.X, 2) + Math.Pow(pt1.Y - pt2.Y, 2));
		}

		public static float getMillimeter(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
				case GraphicsUnit.Millimeter:
					return 1;
				case GraphicsUnit.Inch:
					return 1.0f/12;
				case GraphicsUnit.Point:
					return 72.0f/12;
				case GraphicsUnit.Pixel:
					return 3;
				case GraphicsUnit.Document:
					return 300.0f/12;
				case GraphicsUnit.Display:
					return 75.0f/12;
			}

			return 1;
		}
	}
}
