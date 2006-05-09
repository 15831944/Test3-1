// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Reflection;
using System.Windows.Forms;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	public sealed class Utilities
	{
		internal static RectangleF transformRect(Graphics g,
			Rectangle r, CoordinateSpace destSpace, CoordinateSpace srcSpace)
		{
			PointF[] pts = new PointF[4];

			// get the corner points
			pts[0].X = r.Left;
			pts[0].Y = r.Top;
			pts[1].X = r.Right;
			pts[1].Y = r.Top;
			pts[2].X = r.Right;
			pts[2].Y = r.Bottom;
			pts[3].X = r.Left;
			pts[3].Y = r.Bottom;

			// get the device coordinates
			g.TransformPoints(destSpace, srcSpace, pts);

			// return rectangle in world coordinates
			return RectangleF.FromLTRB(pts[0].X, pts[0].Y, pts[2].X, pts[2].Y);
		}

		internal static RectangleF transformRect(Graphics g,
			RectangleF r, CoordinateSpace destSpace, CoordinateSpace srcSpace)
		{
			PointF[] pts = new PointF[4];

			// get the corner points
			pts[0].X = r.Left;
			pts[0].Y = r.Top;
			pts[1].X = r.Right;
			pts[1].Y = r.Top;
			pts[2].X = r.Right;
			pts[2].Y = r.Bottom;
			pts[3].X = r.Left;
			pts[3].Y = r.Bottom;

			// get the device coordinates
			g.TransformPoints(destSpace, srcSpace, pts);

			// return rectangle in world coordinates
			return RectangleF.FromLTRB(pts[0].X, pts[0].Y, pts[2].X, pts[2].Y);
		}

		internal static Point docToDevice(Graphics g, PointF docPt)
		{
			PointF[] pts = new PointF[1] { new PointF(docPt.X, docPt.Y) };
			g.TransformPoints(CoordinateSpace.Device,
				CoordinateSpace.World, pts);

			return new Point((int)Math.Round(pts[0].X), (int)Math.Round(pts[0].Y));
		}

		internal static PointF docToDeviceF(Graphics g, PointF docPt)
		{
			PointF[] pts = new PointF[1] { docPt };
			g.TransformPoints(CoordinateSpace.Device,
				CoordinateSpace.World, pts);

			return pts[0];
		}

		internal static SizeF docToDeviceF(Graphics g, SizeF size)
		{
			PointF[] pts = new PointF[2] {
				new PointF(0, 0), new PointF(size.Width, size.Height) };
			g.TransformPoints(CoordinateSpace.Device,
				CoordinateSpace.World, pts);

			return new SizeF(pts[1].X - pts[0].X, pts[1].Y - pts[0].Y);
		}

		internal static void docToDevice(Graphics g,
			float docX, float docY, ref int devX, ref int devY)
		{
			PointF[] pts = new PointF[1] { new PointF(docX, docY) };
			g.TransformPoints(CoordinateSpace.Device,
				CoordinateSpace.World, pts);

			devX = (int)(pts[0].X);
			devY = (int)(pts[0].Y);
		}

		internal static Rectangle docToDevice(Graphics g, RectangleF r)
		{
			PointF[] pts = new PointF[4];

			// get the corner points
			pts[0].X = r.Left;
			pts[0].Y = r.Top;
			pts[1].X = r.Right;
			pts[1].Y = r.Top;
			pts[2].X = r.Right;
			pts[2].Y = r.Bottom;
			pts[3].X = r.Left;
			pts[3].Y = r.Bottom;

			// get the device coordinates
			g.TransformPoints(CoordinateSpace.Device,
				CoordinateSpace.World, pts);

			// return rectangle in world coordinates
			return Rectangle.FromLTRB(
				(int)pts[0].X, (int)pts[0].Y, (int)pts[2].X, (int)pts[2].Y);
		}

		internal static Rectangle docToDevice(Graphics g, Rectangle r)
		{
			Point[] pts = new Point[4];

			// get the corner points
			pts[0].X = r.Left;
			pts[0].Y = r.Top;
			pts[1].X = r.Right;
			pts[1].Y = r.Top;
			pts[2].X = r.Right;
			pts[2].Y = r.Bottom;
			pts[3].X = r.Left;
			pts[3].Y = r.Bottom;

			// get the device coordinates
			g.TransformPoints(CoordinateSpace.Device,
				CoordinateSpace.World, pts);

			// return rectangle in world coordinates
			return Rectangle.FromLTRB(
				pts[0].X, pts[0].Y, pts[2].X, pts[2].Y);
		}

		
		internal static RectangleF deviceToDoc(Graphics g, Rectangle r)
		{
			PointF[] pts = new PointF[4];

			// get the corner points
			pts[0].X = r.Left;
			pts[0].Y = r.Top;
			pts[1].X = r.Right;
			pts[1].Y = r.Top;
			pts[2].X = r.Right;
			pts[2].Y = r.Bottom;
			pts[3].X = r.Left;
			pts[3].Y = r.Bottom;

			// get the world coordinates
			g.TransformPoints(CoordinateSpace.World,
				CoordinateSpace.Device, pts);

			// return rectangle in world coordinates
			return RectangleF.FromLTRB(
				pts[0].X, pts[0].Y, pts[2].X, pts[2].Y);
		}

		internal static RectangleF deviceToDoc(Graphics g, RectangleF r)
		{
			PointF[] pts = new PointF[4];

			// get the corner points
			pts[0].X = r.Left;
			pts[0].Y = r.Top;
			pts[1].X = r.Right;
			pts[1].Y = r.Top;
			pts[2].X = r.Right;
			pts[2].Y = r.Bottom;
			pts[3].X = r.Left;
			pts[3].Y = r.Bottom;

			// get the world coordinates
			g.TransformPoints(CoordinateSpace.World,
				CoordinateSpace.Device, pts);

			// return rectangle in world coordinates
			return RectangleF.FromLTRB(
				pts[0].X, pts[0].Y, pts[2].X, pts[2].Y);
		}

		internal static PointF deviceToDoc(Graphics g, float devX, float devY)
		{
			PointF[] pts = new PointF[1] { new PointF(devX, devY) };
			g.TransformPoints(CoordinateSpace.World,
				CoordinateSpace.Device, pts);

			return pts[0];
		}

		internal static PointF deviceToDoc(Graphics g, Point devPoint)
		{
			PointF[] pts = new PointF[1] { new PointF(devPoint.X, devPoint.Y) };
			g.TransformPoints(CoordinateSpace.World,
				CoordinateSpace.Device, pts);

			return pts[0];
		}

		public static Rectangle normalizeRect(Rectangle rc)
		{
			Rectangle rcTmp = new Rectangle(0, 0, 0, 0);
			rcTmp.X = Math.Min(rc.Left, rc.Right);
			rcTmp.Width = Math.Abs(rc.Width);
			rcTmp.Y = Math.Min(rc.Top, rc.Bottom);
			rcTmp.Height = Math.Abs(rc.Height);
			return rcTmp;
		}

		public static RectangleF normalizeRect(RectangleF rc)
		{
			RectangleF rcTmp = new RectangleF(0, 0, 0, 0);
			rcTmp.X = Math.Min(rc.Left, rc.Right);
			rcTmp.Width = Math.Abs(rc.Width);
			rcTmp.Y = Math.Min(rc.Top, rc.Bottom);
			rcTmp.Height = Math.Abs(rc.Height);
			return rcTmp;
		}

		public static GraphicsPath getRoundRect(
			float x, float y, float width, float height, float r)
		{
			float rx = r;
			float ry = r;
			if (2 * rx > width) rx = width / 2;
			if (2 * ry > height) ry = height / 2;

			GraphicsPath gp = new GraphicsPath();
			gp.AddLine(x + rx, y, x + width - rx, y);
            if (rx>0 && ry>0) gp.AddArc(x + width - rx, y, rx, ry, 270, 90);
			gp.AddLine(x + width, y + ry, x + width, y + height - ry);
			if (rx>0 && ry>0) gp.AddArc(x + width - rx, y + height - ry, rx, ry, 0, 90);
			gp.AddLine(x + width - rx, y + height, x + rx, y + height);
			if (rx>0 && ry>0) gp.AddArc(x, y + height - ry, rx, ry, 90, 90);
			gp.AddLine(x, y + height - ry, x, y + ry);
			if (rx>0 && ry>0) gp.AddArc(x, y, rx, ry, 180, 90);
			gp.CloseFigure();

			return gp;
		}

		public static void drawRect(Graphics g, System.Drawing.Pen p,
			System.Drawing.Brush b, float x, float y, float width, float height)
		{
			g.FillRectangle(b, x, y, width, height);
			g.DrawRectangle(p, x, y, width, height);
		}

		public static RectangleF unionRects(RectangleF rc1, RectangleF rc2)
		{
			RectangleF union = new RectangleF(0, 0, 0, 0);
			union.X = Math.Min(rc1.Left, rc2.Left);
			union.Y = Math.Min(rc1.Top, rc2.Top);
			union.Width = Math.Max(rc1.Right, rc2.Right) - union.X;
			union.Height = Math.Max(rc1.Bottom, rc2.Bottom) - union.Y;

			return union;
		}

		public static RectangleF unionNonEmptyRects(RectangleF rc1, RectangleF rc2)
		{
			if (rc1.Width == 0 || rc1.Height == 0) return rc2;

			RectangleF union = new RectangleF(0, 0, 0, 0);
			union.X = Math.Min(rc1.Left, rc2.Left);
			union.Y = Math.Min(rc1.Top, rc2.Top);
			union.Width = Math.Max(rc1.Right, rc2.Right) - union.X;
			union.Height = Math.Max(rc1.Bottom, rc2.Bottom) - union.Y;

			return union;
		}

		public static bool pointInRect(PointF pt, RectangleF rc)
		{
			RectangleF rct = normalizeRect(rc);

			if (pt.X >= rct.Left && pt.X <= rct.Right &&
				pt.Y >= rct.Top && pt.Y <= rct.Bottom) return true;

			return false;
		}

		public static bool pointInRect(Point pt, Rectangle rc)
		{
			Rectangle rct = normalizeRect(rc);

			if (pt.X >= rct.Left && pt.X <= rct.Right &&
				pt.Y >= rct.Top && pt.Y <= rct.Bottom) return true;

			return false;
		}

		public static bool pointInSegment(PointF pt, PointF sgm1, PointF sgm2)
		{
			RectangleF sgmBounds = RectangleF.FromLTRB(
				Math.Min(sgm1.X, sgm2.X), Math.Min(sgm1.Y, sgm2.Y),
				Math.Max(sgm1.X, sgm2.X), Math.Max(sgm1.Y, sgm2.Y));

			if (pt.X >= sgmBounds.Left && pt.X <= sgmBounds.Right &&
				pt.Y >= sgmBounds.Top && pt.Y <= sgmBounds.Bottom)
			{
				if (sgm1.X == sgm2.X) return pt.X == sgm1.X;
				if (sgm1.Y == sgm2.Y) return pt.Y == sgm1.Y;
				if (pt.Equals(sgm1)) return true;
				if (pt.Equals(sgm2)) return true;

				return (pt.X - sgm1.X)*(sgm2.Y - sgm1.Y) ==
					(pt.Y - sgm1.Y)*(sgm2.X - sgm1.X);
			}

			return false;
		}

		public static bool pointInEllipse(PointF pt, RectangleF rc)
		{
			RectangleF rct = normalizeRect(rc);

			// determine radii
			float a = (rct.Right - rct.Left) / 2;
			float b = (rct.Bottom - rct.Top) / 2;
    
			// determine x, y
			float x = pt.X - (rct.Left + rct.Right) / 2;
			float y = pt.Y - (rct.Top + rct.Bottom) / 2;
    
			// apply ellipse formula
			return ((x * x) / (a * a) + (y * y) / (b * b) <= 1);
		}

		
		/*************************************************************************

		  * FUNCTION:   G_PtInPolygon
		  *
		  * PURPOSE
		  * This routine determines if the point passed is in the polygon. It uses

		  * the classical polygon hit-testing algorithm: a horizontal ray starting

		  * at the point is extended infinitely rightwards and the number of
		 * polygon edges that intersect the ray are counted. If the number is odd,

		  * the point is inside the polygon.
		  *
		  * RETURN VALUE
		  * (BOOL) TRUE if the point is inside the polygon, FALSE if not.
		*************************************************************************/ 

		public static bool pointInPolygon(PointF[] poly, PointF ptTest)
		{
			RectangleF r = new RectangleF(0, 0, 0, 0);
			int i;
			PointF pt2;
			int numIntsct = 0;

			if (!pointInPolyRect(poly, ptTest, ref r))
				return false;

			pt2 = ptTest;
			pt2.X = r.Right + 50;

			// Now go through each of the lines in the polygon and see if it
			// intersects
			for (i = 0; i < poly.Length - 1 ; i++)
			{
				if (pointInSegment(ptTest, poly[i], poly[i+1])) return true;
				if (intersect(ptTest, pt2, poly[i], poly[i+1]))
					numIntsct++ ;
			}

			// And the last line
			if (pointInSegment(ptTest, poly[poly.Length-1], poly[0])) return true;
			if (intersect(ptTest, pt2, poly[poly.Length-1], poly[0]))
				numIntsct++ ;

			return (numIntsct % 2 > 0);

		}

		/*************************************************************************

		   * FUNCTION:   G_PtInPolyRect
		   *
		   * PURPOSE
		   * This routine determines if a point is within the smallest rectangle
		   * that encloses a polygon.
		   *
		   * RETURN VALUE
		   * (BOOL) TRUE or FALSE depending on whether the point is in the rect or

		   * not.
		 *************************************************************************/ 

		public static bool pointInPolyRect(PointF[] poly, PointF ptTest,
			ref RectangleF rBound)
		{
			float xmin, xmax, ymin, ymax ;
			int i ;

			xmin = ymin = int.MaxValue;
			xmax = ymax = int.MinValue;

			for (i=0; i < poly.Length; i++)
			{
				PointF ppt = poly[i];
				if (ppt.X < xmin)
					xmin = ppt.X;
				if (ppt.X > xmax)
					xmax = ppt.X;
				if (ppt.Y < ymin)
					ymin = ppt.Y;
				if (ppt.Y > ymax)
					ymax = ppt.Y;
			}

			rBound.X = xmin;
			rBound.Y = ymin;
			rBound.Width = xmax - xmin;
			rBound.Height = ymax - ymin;

			return (pointInRect(ptTest, rBound));

		}

		/*************************************************************************
		   * FUNCTION:   Intersect
		   *
		   * PURPOSE
		   * Given two line segments, determine if they intersect.
		   *
		   * RETURN VALUE
		   * TRUE if they intersect, FALSE if not.
		 *************************************************************************/ 

		public static bool intersect(PointF p1, PointF p2, PointF p3, PointF p4)
		{
			return ((( CCW(p1, p2, p3) * CCW(p1, p2, p4)) <= 0)
				&& (( CCW(p3, p4, p1) * CCW(p3, p4, p2)  <= 0) ));
		}

		/*************************************************************************

		   * FUNCTION:   CCW (CounterClockWise)
		   *
		   * PURPOSE
		   * Determines, given three points, if when travelling from the first to
		   * the second to the third, we travel in a counterclockwise direction.
		   *
		   * RETURN VALUE
		   * (int) 1 if the movement is in a counterclockwise direction, -1 if
		   * not.
		 *************************************************************************/ 

		public static int CCW(PointF p0, PointF p1, PointF p2)
		{
			float dx1, dx2;
			float dy1, dy2;

			dx1 = p1.X - p0.X ; dx2 = p2.X - p0.X ;
			dy1 = p1.Y - p0.Y ; dy2 = p2.Y - p0.Y ;

			/* This is basically a slope comparison: we don't do divisions because

			 * of divide by zero possibilities with pure horizontal and pure
			 * vertical lines.
			 */ 

			return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1);

		}

		public static PointF lineIntersect(PointF pt1_l1, PointF pt2_l1, PointF pt1_l2, PointF pt2_l2)
		{
			PointF pt = new PointF(float.MinValue, float.MinValue);

			if (pt1_l1.X == pt2_l1.X && pt1_l2.X == pt2_l2.X)
			{
				return pt;
			}

			if (pt1_l1.X == pt2_l1.X)
			{
				float a = (pt1_l2.Y - pt2_l2.Y) / (pt1_l2.X - pt2_l2.X);
				float b = (pt1_l2.X*pt2_l2.Y - pt2_l2.X*pt1_l2.Y) /
									(pt1_l2.X - pt2_l2.X);

				pt.X = pt1_l1.X;
				pt.Y = a * pt.X + b;

				return pt;
			}

			if (pt1_l2.X == pt2_l2.X)
			{
				float a = (pt1_l1.Y - pt2_l1.Y) / (pt1_l1.X - pt2_l1.X);
				float b = (pt1_l1.X*pt2_l1.Y - pt2_l1.X*pt1_l1.Y) /
									(pt1_l1.X - pt2_l1.X);

				pt.X = pt1_l2.X;
				pt.Y = a * pt.X + b;

				return pt;
			}

			float a1 = (pt1_l1.Y - pt2_l1.Y) / (pt1_l1.X - pt2_l1.X);
			float b1 = (pt1_l1.X*pt2_l1.Y - pt2_l1.X*pt1_l1.Y) /
								(pt1_l1.X - pt2_l1.X);

			float a2 = (pt1_l2.Y - pt2_l2.Y) / (pt1_l2.X - pt2_l2.X);
			float b2 = (pt1_l2.X*pt2_l2.Y - pt2_l2.X*pt1_l2.Y) /
								(pt1_l2.X - pt2_l2.X);

			if (a1 != a2)
			{
				pt.X = (b2 - b1) / (a1 - a2);
				pt.Y = a1 * (b2 - b1) / (a1 - a2) + b1;
			}

			return pt;
		}

		public static bool segmentIntersect(PointF pt1_s, PointF pt2_s, PointF pt1_l, PointF pt2_l, ref PointF pt)
		{
			pt = lineIntersect(pt1_s, pt2_s, pt1_l, pt2_l);

			// do not compare to 0 but to a small float number, because what's 0 in
			// debug builds seems to result in 2.XXXXXE-5 in release builds ???

			float p1 = (pt.X - pt1_s.X)*(pt.X - pt2_s.X);
			float p2 = (pt.Y - pt1_s.Y)*(pt.Y - pt2_s.Y);
			if (p1 > 0.0001 || p2 > 0.0001)
				return false;

			float pl1 = (pt.X - pt1_l.X)*(pt.X - pt2_l.X);
			float pl2 = (pt.Y - pt1_l.Y)*(pt.Y - pt2_l.Y);
			if (pl1 > 0.0001 || pl2 > 0.0001)
				return false;

			return true;
		}

		public static Region createEllipticRgn(RectangleF rc)
		{
			GraphicsPath gp = new GraphicsPath();
			gp.AddEllipse(rc);

			Region res = new Region(gp);
			gp.Dispose();

			return res;
		}

		public static Region createRoundRectRgn(RectangleF rc, float r)
		{
			float width = rc.Width;
			float height = rc.Height;
			float x = rc.X;
			float y = rc.Y;
			float rx = r;
			float ry = r;
			if (2 * rx > rc.Width) rx = rc.Width / 2;
			if (2 * ry > rc.Height) ry = rc.Height / 2;

			GraphicsPath gp = new GraphicsPath();
			gp.AddLine(x + rx, y, x + width - rx, y);
            if (rx>0 && ry>0) gp.AddArc(x + width - rx, y, rx, ry, 270, 90);
			gp.AddLine(x + width, y + ry, x + width, y + height - ry);
			if (rx>0 && ry>0) gp.AddArc(x + width - rx, y + height - ry, rx, ry, 0, 90);
			gp.AddLine(x + width - rx, y + height, x + rx, y + height);
			if (rx>0 && ry>0) gp.AddArc(x, y + height - ry, rx, ry, 90, 90);
			gp.AddLine(x, y + height - ry, x, y + ry);
			if (rx>0 && ry>0) gp.AddArc(x, y, rx, ry, 180, 90);
			gp.CloseFigure();

			Region res = new Region(gp);
			gp.Dispose();

			return res;
		}

		public static Region createPolygonRgn(PointF[] poly)
		{
			GraphicsPath gp = new GraphicsPath();
			for (int i = 0; i < poly.Length - 1; ++i)
				gp.AddLine(poly[i], poly[i+1]);
			gp.AddLine(poly[poly.Length-1], poly[0]);
			gp.CloseFigure();

			Region res = new Region(gp);
			gp.Dispose();

			return res;
		}

		public static PointF midPoint(PointF pt1, PointF pt2)
		{
			PointF pt = new PointF( (pt1.X + pt2.X) / 2, (pt1.Y + pt2.Y) / 2 );

			return pt;
		}

		public static void addToRect(ref RectangleF rect, float w, float h)
		{
			if (w > 0)
				rect.Width += w;
			else
			{
				rect.X += w;
				rect.Width -= w;
			}

			if (h > 0)
				rect.Height += h;
			else
			{
				rect.Y += h;
				rect.Height -= h;
			}
		}

		internal static GraphicsPath rotateRectAt(RectangleF rc, PointF pivot, float angle)
		{
			GraphicsPath path = new GraphicsPath();
			path.AddRectangle(rc);
			Matrix rotation = new Matrix();
			rotation.RotateAt(angle, pivot);
			path.Transform(rotation);
			rotation.Dispose();
			return path;
		}

		internal static PointF rotatePointAt(PointF point, PointF pivot, float angle)
		{
			PointF[] points = new PointF[] { point };
			Matrix rotation = new Matrix();
			rotation.RotateAt(angle, pivot);
			rotation.TransformPoints(points);
			rotation.Dispose();
			return points[0];
		}

		internal static void rotatePointsAt(PointF[] points, PointF pivot, float angle)
		{
			Matrix rotation = new Matrix();
			rotation.RotateAt(angle, pivot);
			rotation.TransformPoints(points);
			rotation.Dispose();
		}

		internal static void drawSquareHandles(Graphics g,
			Handles mask, Color rgb, Color rgbDis, bool showDsbl,
			RectangleF rc, float rotationAngle, HandlesStyle st, float size)
		{
			PointF center = getCenter(rc);

			// select pen and brush for drawing the selection handles
			System.Drawing.Pen pen = new System.Drawing.Pen(Color.Black, 0);
			System.Drawing.Brush enbBrush = new System.Drawing.SolidBrush(rgb);
			System.Drawing.Brush dsbBrush = new System.Drawing.SolidBrush(rgbDis);
			System.Drawing.Brush brush;

			// calculate selection handles coordinates
			PointF[] handlePoints = new PointF[10];
			handlePoints[0] = new PointF(rc.Left, rc.Top);
			handlePoints[1] = new PointF(rc.Right, rc.Top);
			handlePoints[2] = new PointF(rc.Right, rc.Bottom);
			handlePoints[3] = new PointF(rc.Left, rc.Bottom);
			handlePoints[4] = new PointF(center.X, rc.Top);
			handlePoints[5] = new PointF(rc.Right, center.Y);
			handlePoints[6] = new PointF(center.X, rc.Bottom);
			handlePoints[7] = new PointF(rc.Left, center.Y);
			handlePoints[8] = center;
			handlePoints[9] = new PointF(
				center.X, rc.Top - 6 * Constants.getMillimeter(g.PageUnit));

			// apply rotation
			if (rotationAngle != 0)
			{
				Matrix rotation = new Matrix();
				rotation.RotateAt(rotationAngle, center);
				rotation.TransformPoints(handlePoints);
				rotation.Dispose();
			}

			// draw the handles
			float hsize = size / 2;
			for (int h = 0; h <= 9; h++)
			{
				bool draw = true;

				// determine whether this handle should be drawn
				if (((long)mask & (1 << h)) != 0)
				{
					brush = enbBrush;
				}
				else
				{
					brush = dsbBrush;
					draw = showDsbl && h != 9;
				}

				if (draw) 
				{
					PointF point = handlePoints[h];

					// the side and corner handles
					if (h < 8)
					{
						g.FillRectangle(brush,
							point.X - hsize, point.Y - hsize, size, size);
						g.DrawRectangle(pen,
							point.X - hsize, point.Y - hsize, size, size);
					}
					// the center handle
					else if (h == 8)
					{
						if (st == HandlesStyle.EasyMove)
						{
							g.FillEllipse(brush,
								point.X - size, point.Y - size, 2*size, 2*size);
							g.DrawEllipse(pen,
								point.X - size, point.Y - size, 2*size, 2*size);
						}
						if (st == HandlesStyle.SquareHandles)
						{
							g.FillRectangle(brush,
								point.X - hsize, point.Y - hsize, size, size);
							g.DrawRectangle(pen,
								point.X - hsize, point.Y - hsize, size, size);
						}
					}
					// the rotation handle
					else if (h == 9)
					{
						g.FillEllipse(brush,
							point.X - 2.8F*size/4, point.Y - 2.8F*size/4, 2.8F*size/2, 2.8F*size/2);
						g.DrawEllipse(pen,
							point.X - 2.8F*size/4, point.Y - 2.8F*size/4, 2.8F*size/2, 2.8F*size/2);

						System.Drawing.Pen rotMarker =
							new System.Drawing.Pen(brush, 0);
						rotMarker.DashStyle = DashStyle.Dot;
						g.DrawLine(rotMarker, point, handlePoints[4]);
						rotMarker.Dispose();
					}
				}
			}

			//cleanup
			pen.Dispose();
			enbBrush.Dispose();
			dsbBrush.Dispose();
		}

		internal static void drawRotationHandle(
			Graphics g, Color rgb, Color rgbDis,
			RectangleF rc, float rotationAngle,
			Handles mask, bool showDsbl, float size)
		{
			bool draw = true;
			System.Drawing.Pen pen = new System.Drawing.Pen(Color.Black, 0);
			System.Drawing.Brush enbBrush = new System.Drawing.SolidBrush(rgb);
			System.Drawing.Brush dsbBrush = new System.Drawing.SolidBrush(rgbDis);
			System.Drawing.Brush brush;

			// determine whether this handle should be drawn
			if ((mask & Handles.Rotate) != 0)
			{
				brush = enbBrush;
			}
			else
			{
				brush = dsbBrush;
				draw = false;
			}
			if (draw)
			{
				PointF center = getCenter(rc);
				PointF[] rotPoints = new PointF[] {
					new PointF(center.X, rc.Top - 6 * Constants.getMillimeter(g.PageUnit)),
					new PointF(center.X, rc.Top) };

				// apply rotation
				if (rotationAngle != 0)
				{
					Matrix rotation = new Matrix();
					rotation.RotateAt(rotationAngle, center);
					rotation.TransformPoints(rotPoints);
					rotation.Dispose();
				}

				PointF point = rotPoints[0];
				g.FillEllipse(brush,
					point.X - 2.8F*size/4, point.Y - 2.8F*size/4, 2.8F*size/2, 2.8F*size/2);
				g.DrawEllipse(pen,
					point.X - 2.8F*size/4, point.Y - 2.8F*size/4, 2.8F*size/2, 2.8F*size/2);

				System.Drawing.Pen rotMarker =
					new System.Drawing.Pen(brush, 0);
				rotMarker.DashStyle = DashStyle.Dot;
				g.DrawLine(rotMarker, point, rotPoints[1]);
				rotMarker.Dispose();
			}

			pen.Dispose();
			enbBrush.Dispose();
			dsbBrush.Dispose();
		}

		internal static void drawSelHandles(
			Graphics g, Color rgb, Color rgbDis,
			RectangleF rc, float rotationAngle,
			Handles mask, bool showDsbl, HandlesStyle st, float size)
		{
			// these styles must not be painted
			if (st == HandlesStyle.Invisible ||
				st == HandlesStyle.InvisibleMove)
				return;

			// draw square selection handles for these styles
			if (st == HandlesStyle.SquareHandles ||
				st == HandlesStyle.SquareHandles2 ||
				st == HandlesStyle.EasyMove)
			{
				drawSquareHandles(g, mask, rgb, rgbDis, showDsbl,
					rc, rotationAngle, st, size);

				return;
			}

			PointF center = getCenter(rc);

			if (st == HandlesStyle.DashFrame)
			{
				try
				{
					// drawing with a dashed pen sometimes throws a generic exception
					// in GDIPLUS.DLL, so catch everything or everyone will blame us;
					System.Drawing.Pen pen = new System.Drawing.Pen(rgb, 0);
					pen.DashStyle = DashStyle.Dash;

					if (rotationAngle == 0)
					{
						g.DrawRectangle(pen, rc.X, rc.Y, rc.Width, rc.Height);
					}
					else
					{
						GraphicsPath path = rotateRectAt(rc, center, rotationAngle);
						g.DrawPath(pen, path);
						path.Dispose();
					}

					pen.Dispose();

					drawRotationHandle(g, rgb, rgbDis,
						rc, rotationAngle, mask, showDsbl, size);
				}
				catch (Exception)
				{
					// sometimes the exception says 'out of memory'
					GC.Collect();
				}

				return;
			}

			if (st == HandlesStyle.HatchFrame)
			{
				rc.Inflate(size / 2, size / 2);

				System.Drawing.Drawing2D.HatchBrush hatchBrush =
					new System.Drawing.Drawing2D.HatchBrush(
					HatchStyle.BackwardDiagonal, Color.Black, Color.White);

				RectangleF[] rects = new RectangleF[] {
					new RectangleF(rc.Left + size, rc.Top, rc.Width - 2 * size, size),
					new RectangleF(rc.Left + size, rc.Bottom - size, rc.Width - 2 * size, size),
					new RectangleF(rc.Left, rc.Top, size, rc.Height),
					new RectangleF(rc.Right - size, rc.Top, size, rc.Height) };

				if (rotationAngle == 0)
				{
					for (int i = 0; i < 4; ++i)
						g.FillRectangle(hatchBrush, rects[i]);
				}
				else
				{
					for (int i = 0; i < 4; ++i)
					{
						GraphicsPath path = rotateRectAt(
							rects[i], center, rotationAngle);
						g.FillPath(hatchBrush, path);
						path.Dispose();
					}
				}

				hatchBrush.Dispose();

				drawRotationHandle(g, rgb, rgbDis,
					rc, rotationAngle, mask, showDsbl, size);

				return;
			}

			if (st == HandlesStyle.HatchHandles || st == HandlesStyle.HatchHandles2 ||
				st == HandlesStyle.HatchHandles3 || st == HandlesStyle.MoveOnly)
			{
				RectangleF rch = rc;
				rch.Inflate(size / 4, size / 4);

				System.Drawing.Drawing2D.HatchBrush hatchBrush = null;

				if (st == HandlesStyle.HatchHandles3 || st == HandlesStyle.MoveOnly)
					hatchBrush = new System.Drawing.Drawing2D.HatchBrush(
						HatchStyle.Percent50, Color.Black, Color.FromArgb(50, Color.Black));
				else
					hatchBrush = new System.Drawing.Drawing2D.HatchBrush(
						HatchStyle.BackwardDiagonal, Color.Black, Color.White);

				RectangleF[] rects = new RectangleF[] {
					new RectangleF(rch.Left + size*2/4, rch.Top, rch.Width - size, size*2/4),
					new RectangleF(rch.Left + size*2/4, rch.Bottom - size*2/4, rch.Width - size, size*2/4),
					new RectangleF(rch.Left, rch.Top, size*2/4, rch.Height),
					new RectangleF(rch.Right - size*2/4, rch.Top, size*2/4, rch.Height) };

				if (rotationAngle == 0)
				{
					for (int i = 0; i < 4; ++i)
						g.FillRectangle(hatchBrush, rects[i]);
				}
				else
				{
					for (int i = 0; i < 4; ++i)
					{
						GraphicsPath path = rotateRectAt(
							rects[i], center, rotationAngle);
						g.FillPath(hatchBrush, path);
						path.Dispose();
					}
				}

				hatchBrush.Dispose();

				if (st == HandlesStyle.MoveOnly)
					return;

				drawSquareHandles(g, mask, rgb, rgbDis, showDsbl,
					rc, rotationAngle, st, size);

				return;
			}
		}

		internal static void calcBezierCoef(ref double a0,  ref double a1,  ref double a2,  ref double a3,
			 ref double b0,  ref double b1,  ref double b2,  ref double b3,
			 ref double u , ref double s,  ref double z,  ref double x4,  ref double y4)
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

		internal static float distToBezier(PointF pt, PointCollection bez_pts, int nPoints)
		{
			int nSplineNum = nPoints / 3;
			float nRes = 1000000;

			for (int nSpline = 0; nSpline < nSplineNum; ++nSpline)
			{
				double x0 = bez_pts[nSpline*3+0].X;
				double y0 = bez_pts[nSpline*3+0].Y;
				double x1 = bez_pts[nSpline*3+1].X;
				double y1 = bez_pts[nSpline*3+1].Y;
				double x2 = bez_pts[nSpline*3+2].X;
				double y2 = bez_pts[nSpline*3+2].Y;
				double x3 = bez_pts[nSpline*3+3].X;
				double y3 = bez_pts[nSpline*3+3].Y;

				double a3 = (x3 - x0 + 3*(x1-x2)) / 8;
				double b3 = (y3 - y0 + 3*(y1-y2)) / 8;
				double a2 = (x3 + x0 - x1 - x2) * 3 / 8;
				double b2 = (y3 + y0 - y1 - y2) * 3 / 8;
				double a1 = (x3 - x0) / 2 - a3;
				double b1 = (y3 - y0) / 2 - b3;
				double a0 = (x3 + x0) / 2 - a2;
				double b0 = (y3 + y0) / 2 - b2;

				double x4 = pt.X;
				double y4 = pt.Y;

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
						u1 = u; z1 = z; s1 = s; break;
					}
					if (Math.Abs(u + 1.0) < 0.00001)
					{
						u1 = u; z1 = z; s1 = s;
					}
					if (s < s1)
					{
						u1 = u; z1 = z; s1 = s;
					}
				}

				if (Math.Abs(s1) > 0.00001)
				{
					u = u1 + stepsize;
					if (u > 1.0) u = 1.0 - stepsize;
					for(int cnt = 0 ; cnt < 20; cnt++)
					{
						calcBezierCoef(ref a0, ref a1, ref a2, ref a3, ref b0, ref b1, ref b2, ref b3, ref u, ref s, ref z, ref x4, ref y4);
						if (Math.Abs(s) < 0.00001) break;
						if (Math.Abs(z) < 0.00001) break;
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
						if (Math.Abs(u - u2) < 0.0001) break;
						u1 = u2;
						z1 = z2;
					}
				}

				if (nRes > Math.Sqrt(s)) nRes = (float)Math.Sqrt(s);
				if (nRes == 0) return 0;
			}

			return nRes;
		}

		internal static float distToPolyline(PointF pt, PointCollection line_pts, int nPoints)
		{
			int segmNum = 0;
			return distToPolyline(pt, line_pts, nPoints, ref segmNum);
		}

		internal static float distToPolyline(PointF pt, PointCollection line_pts, int nPoints, ref int segmNum)
		{
			float nDist, nMinDist = 1000000;
			segmNum = 0;

			for (int l = 0; l < nPoints - 1; ++l)
			{
				double x1 = line_pts[l].X;
				double y1 = line_pts[l].Y;
				double x2 = line_pts[l+1].X;
				double y2 = line_pts[l+1].Y;

				double A = y1 - y2;
				double B = x2 - x1;
				double C = x1*y2 - y1*x2;

				if (Math.Abs(A) < 0.00001 && Math.Abs(B) < 0.00001)
				{
					nDist = (float)Math.Sqrt((x1-pt.X)*(x1-pt.X) + (y1-pt.Y)*(y1-pt.Y));
				}
				else
				if (Math.Abs(A) < 0.00001)
				{
					if ((pt.X > x1 && pt.X > x2) || (pt.X < x1 && pt.X < x2))
					{
						nDist = (float)Math.Min(
										Math.Sqrt((x1-pt.X)*(x1-pt.X) + (y1-pt.Y)*(y1-pt.Y)),
										Math.Sqrt((x2-pt.X)*(x2-pt.X) + (y2-pt.Y)*(y2-pt.Y)));
					}
					else
					{
						nDist = (float)Math.Abs(y1 - pt.Y);
					}
				}
				else
				{
					double xo, yo;

					yo = (A*A*pt.Y - A*B*pt.X - B*C) / (A*A + B*B);
					xo = (-C - B*yo) / A;

					if ( (xo - x1)*(xo - x2) <= 0 && (yo - y1)*(yo - y2) <= 0)
					{
						nDist = (float)Math.Sqrt((pt.X - xo)*(pt.X - xo) + (pt.Y - yo)*(pt.Y - yo));
					}
					else
					{
						nDist = (float)Math.Min(
										Math.Sqrt((x1-pt.X)*(x1-pt.X) + (y1-pt.Y)*(y1-pt.Y)),
										Math.Sqrt((x2-pt.X)*(x2-pt.X) + (y2-pt.Y)*(y2-pt.Y)));
					}
				}

				if (nMinDist > nDist)
				{
					nMinDist = nDist;
					segmNum = l;
				}
			}

			return nMinDist;
		}

		internal static PointF getNearestPt(PointF pt, PointF segPt1, PointF segPt2)
		{
			double x1 = segPt1.X;
			double y1 = segPt1.Y;
			double x2 = segPt2.X;
			double y2 = segPt2.Y;

			double A = y1 - y2;
			double B = x2 - x1;
			double C = x1*y2 - y1*x2;

			PointF op = new PointF(0, 0);

			if (Math.Abs(A) < 0.00001 && Math.Abs(B) < 0.00001)
			{
				op = segPt1;
			}
			else
			if (Math.Abs(A) < 0.00001)
			{
				if ((pt.X > x1 && pt.X > x2) || (pt.X < x1 && pt.X < x2))
				{
					if (Math.Sqrt((x1-pt.X)*(x1-pt.X) + (y1-pt.Y)*(y1-pt.Y)) <
						Math.Sqrt((x2-pt.X)*(x2-pt.X) + (y2-pt.Y)*(y2-pt.Y)))
						op = segPt1;
					else
						op = segPt2;
				}
				else
				{
					op = segPt1;
					op.X = pt.X;
				}
			}
			else
			{
				double xo, yo;

				yo = (A*A*pt.Y - A*B*pt.X - B*C) / (A*A + B*B);
				xo = (-C - B*yo) / A;
				op.X = (float)xo;
				op.Y = (float)yo;
			}

			return op;
		}

		internal static float Distance(PointF pt1, PointF pt2)
		{
			return (float)Math.Sqrt(
				Math.Pow(pt1.X - pt2.X, 2) + Math.Pow(pt1.Y - pt2.Y, 2));
		}

		internal static GraphicsPath getDelaySymbol(RectangleF rc)
		{
			float w = rc.Right - rc.Left;

			GraphicsPath path = new GraphicsPath();
			path.AddLine(rc.X + w/2, rc.Bottom, rc.X, rc.Bottom);
			path.AddLine(rc.X, rc.Bottom, rc.X, rc.Top);
			path.AddLine(rc.X, rc.Top, rc.X + w/2, rc.Top);
			path.AddArc(rc.Left, rc.Top, w, rc.Height, -90, 180);
			path.CloseFigure();

			return path;
		}

		internal static void getRectIntr(RectangleF rcBox, PointF pt1, PointF pt2, ref PointF pt)
		{
			RectangleF rc = RectangleF.FromLTRB(pt1.X, pt1.Y, pt2.X, pt2.Y);
			rc = normalizeRect(rc);

			float x1 = pt1.X;
			float y1 = pt1.Y;
			float x2 = pt2.X;
			float y2 = pt2.Y;

			if (x1 == x2)
			{
				pt.X = x1;

				// try with the top line
				pt.Y = rcBox.Top;
				if (pt.X >= rcBox.Left && pt.X <= rcBox.Right &&
					pt.Y >= rc.Top && pt.Y <= rc.Bottom) return;

				// try with the bottom line
				pt.Y = rcBox.Bottom;
				if (pt.X >= rcBox.Left && pt.X <= rcBox.Right &&
					pt.Y >= rc.Top && pt.Y <= rc.Bottom) return;
			}
			else
			if (y1 == y2)
			{
				pt.Y = y1;

				// try with the left line segment
				pt.X = rcBox.Left;
				if (pt.Y >= rcBox.Top && pt.Y <= rcBox.Bottom &&
					pt.X >= rc.Left && pt.X <= rc.Right) return;

				// try with the right line segment
				pt.X = rcBox.Right;
				if (pt.Y >= rcBox.Top && pt.Y <= rcBox.Bottom &&
					pt.X >= rc.Left && pt.X <= rc.Right) return;
			}
			else
			{
				float a = (y1 - y2) / (x1 - x2);
				float b = (x1*y2 - x2*y1) / (x1 - x2);

				// try with the top line
				pt.Y = rcBox.Top;
				pt.X = (pt.Y - b) / a;
				if (pt.X >= rcBox.Left && pt.X <= rcBox.Right &&
					pt.Y <= rcBox.Bottom &&
					pt.Y >= rc.Top && pt.Y <= rc.Bottom) return;

				// try with the bottom line
				pt.Y = rcBox.Bottom;
				pt.X = (pt.Y - b) / a;
				if (pt.X >= rcBox.Left && pt.X <= rcBox.Right &&
					pt.Y >= rcBox.Top &&
					pt.Y >= rc.Top && pt.Y <= rc.Bottom) return;

				// try with the left line
				pt.X = rcBox.Left;
				pt.Y = a*pt.X + b;
				if (pt.Y >= rcBox.Top && pt.Y <= rcBox.Bottom &&
					pt.X <= rcBox.Right &&
					pt.X >= rc.Left && pt.X <= rc.Right) return;

				// try with the right line
				pt.X = rcBox.Right;
				pt.Y = a*pt.X + b;
				if (pt.Y >= rcBox.Top && pt.Y <= rcBox.Bottom &&
					pt.X >= rcBox.Left &&
					pt.X >= rc.Left && pt.X <= rc.Right) return;
			}
		}

		internal static void getEllipseIntr(RectangleF rcBox, PointF pt1, PointF pt2, ref PointF pt)
		{
			RectangleF rc = RectangleF.FromLTRB(pt1.X, pt1.Y, pt2.X, pt2.Y);
			rc = normalizeRect(rc);

			float x1 = pt1.X;
			float y1 = pt1.Y;
			float x2 = pt2.X;
			float y2 = pt2.Y;

			if (Math.Abs(x1 - x2) > 0.0001)
			{
				float cx = (rcBox.Left + rcBox.Right) / 2;
				float cy = (rcBox.Top + rcBox.Bottom) / 2;
				float ea = (rcBox.Right - rcBox.Left) / 2;
				float eb = (rcBox.Bottom - rcBox.Top) / 2;
				float a = (y1 - y2) / (x1 - x2);
				float b = (x1*y2 - x2*y1) / (x1 - x2);

				double A = eb*eb + a*a*ea*ea;
				double B = 2*a*(b-cy)*ea*ea - 2*cx*eb*eb;
				double C = eb*eb*cx*cx + ea*ea*(b-cy)*(b-cy) - ea*ea*eb*eb;

				float X1, X2, Y1, Y2;

				X1 = (float)((-B + Math.Sqrt(B*B - 4*A*C))/(2*A));
				X2 = (float)((-B - Math.Sqrt(B*B - 4*A*C))/(2*A));
				Y1 = a*X1 + b;
				Y2 = a*X2 + b;

				pt.X = X1; pt.Y = Y1;
				if (pt.X >= rc.Left && pt.X <= rc.Right &&
					pt.Y >= rc.Top && pt.Y <= rc.Bottom) return;

				pt.X = X2; pt.Y = Y2;
				if (pt.X >= rc.Left && pt.X <= rc.Right &&
					pt.Y >= rc.Top && pt.Y <= rc.Bottom) return;

			}
			else
			{
				float cx = (rcBox.Left + rcBox.Right) / 2;
				float cy = (rcBox.Top + rcBox.Bottom) / 2;
				float ea = (rcBox.Right - rcBox.Left) / 2;
				float eb = (rcBox.Bottom - rcBox.Top) / 2;

				float X = x1;
				float Y1 = cy - (float)Math.Sqrt((1 - (X-cx)*(X-cx)/(ea*ea)) * eb*eb);
				float Y2 = cy + (float)Math.Sqrt((1 - (X-cx)*(X-cx)/(ea*ea)) * eb*eb);

				pt.X = X; pt.Y = Y1;
				if (pt.X >= rc.Left && pt.X <= rc.Right &&
					pt.Y >= rc.Top && pt.Y <= rc.Bottom) return;

				pt.X = X; pt.Y = Y2;
				if (pt.X >= rc.Left && pt.X <= rc.Right &&
					pt.Y >= rc.Top && pt.Y <= rc.Bottom) return;
			}
		}

		internal static bool pointInHandle(PointF pt, ref int handle,
			RectangleF rc, float rotationAngle, Handles mask,
			float ht, HandlesStyle st, float handleSize, GraphicsUnit measureUnit)
		{
			if (rotationAngle != 0)
				pt = rotatePointAt(pt, getCenter(rc), -rotationAngle);

			if (st == HandlesStyle.MoveOnly || st == HandlesStyle.InvisibleMove)
			{
				handle = 8;
				return rc.Contains(pt) && (mask & Handles.Move) != 0;
			}

			if (st == HandlesStyle.SquareHandles || st == HandlesStyle.HatchHandles ||
				st == HandlesStyle.EasyMove)
			{
				PointF[] pts = new PointF[10];

				//the corner points
				pts[0].X = rc.X;
				pts[0].Y = rc.Y;
				pts[1].X = rc.Right;
				pts[1].Y = rc.Y;
				pts[2].X = rc.Right;
				pts[2].Y = rc.Bottom;
				pts[3].X = rc.X;
				pts[3].Y = rc.Bottom;

				//the side points
				pts[4].X = (pts[0].X + pts[1].X) / 2;
				pts[4].Y = pts[0].Y;
				pts[5].X = pts[1].X;
				pts[5].Y = (pts[1].Y + pts[2].Y) / 2;
				pts[6].X = pts[4].X;
				pts[6].Y = pts[2].Y;
				pts[7].X = pts[0].X;
				pts[7].Y = pts[5].Y;

				//the center point
				pts[8].X = pts[4].X;
				pts[8].Y = pts[5].Y;

				// the rotation handle
				pts[9].X = pts[4].X;
				pts[9].Y = pts[4].Y - 6*Constants.getMillimeter(measureUnit);

				if (st == HandlesStyle.EasyMove && (Distance(pts[8], pt) < 1.5 * handleSize))
					return false;

				for (int i = 0; i < 10; ++i)
				{
					if (Math.Abs(pts[i].X - pt.X) <= (handleSize/2) &&
						Math.Abs(pts[i].Y - pt.Y) <= (handleSize/2) &&
						((long)mask & (1 << i)) != 0)
					{
						handle = i;
						return true;
					}
				}

				if (st == HandlesStyle.EasyMove && rc.Contains(pt) && (mask & Handles.Move) != 0)
				{
					handle = 8;
					return true;
				}

				rc.Inflate(handleSize / 2, handleSize / 2);
				if (st == HandlesStyle.HatchHandles && rc.Contains(pt) &&
					(mask & Handles.Move) != 0)
				{
					rc.Inflate(- handleSize, - handleSize);
					handle = 8;
					return !rc.Contains(pt);
				}
			}
			else
			{
				// check the corners
				PointF[] ptCorner = new PointF[4];
				ptCorner[0].X = rc.Left;
				ptCorner[0].Y = rc.Top;
				ptCorner[1].X = rc.Right;
				ptCorner[1].Y = rc.Top;
				ptCorner[2].X = rc.Right;
				ptCorner[2].Y = rc.Bottom;
				ptCorner[3].X = rc.Left;
				ptCorner[3].Y = rc.Bottom;
				for (int i = 0; i < 4; ++i)
				{
					if (Math.Abs(ptCorner[i].X - pt.X) <= (handleSize/2) &&
						Math.Abs(ptCorner[i].Y - pt.Y) <= (handleSize/2) &&
						((long)mask & (1 << i)) != 0)
					{
						handle = i;
						return true;
					}
				}

				//the side points
				rc.Inflate(Constants.getSelPtTest(measureUnit)*2, Constants.getSelPtTest(measureUnit)*2);
				if (Utilities.pointInRect(pt, rc))
				{
					float inflate = -Constants.getSelPtTest(measureUnit)*2;
					rc.Inflate(inflate, inflate);
					if (Math.Abs(rc.Top - pt.Y) <= (handleSize/2) &&
						(mask & Handles.ResizeTopCenter) != 0)
					{
						handle = 4;
						return true;
					}
					if (Math.Abs(rc.Right - pt.X) <= (handleSize/2) &&
						(mask & Handles.ResizeMiddleRight) != 0)
					{
						handle = 5;
						return true;
					}
					if (Math.Abs(rc.Bottom - pt.Y) <= (handleSize/2) &&
						(mask & Handles.ResizeBottomCenter) != 0)
					{
						handle = 6;
						return true;
					}
					if (Math.Abs(rc.Left - pt.X) <= (handleSize/2) &&
						(mask & Handles.ResizeMiddleLeft) != 0)
					{
						handle = 7;
						return true;
					}
				}

				// the rotation handle
				PointF rotHandle = new PointF(
					(ptCorner[0].X + ptCorner[1].X) / 2,
					ptCorner[0].Y - 6*Constants.getMillimeter(measureUnit));
				if (Math.Abs(rotHandle.X - pt.X) <= (handleSize/2) &&
					Math.Abs(rotHandle.Y - pt.Y) <= (handleSize/2) &&
					(mask & Handles.Rotate) != 0)
				{
					handle = 9;
					return true;
				}

				// check if the point is in the title area
				RectangleF rcCpt = rc;
				if (ht != 0)
					rcCpt.Height = ht;
				else
					rcCpt.Inflate(- handleSize * 2, - handleSize * 2);
				if (Utilities.pointInRect(pt, rcCpt) && (mask & Handles.Move) != 0)
				{
					handle = 8;
					return true;
				}
			}

			return false;
		}

		internal static void drawImage(Graphics g, Image pict,
			RectangleF rect, ImageAlign ppos)
		{
			drawImage(g, pict, rect, ppos, 0, PointF.Empty);
		}

		internal static void drawImage(Graphics g, Image pict,
			RectangleF rect, ImageAlign ppos, float rotationAngle, PointF pivot)
		{
			float xoff = 0, yoff = 0;
			int xc = 0, yc = 0;

			// get image logical size in document coordinates
			RectangleF sizeDev = RectangleF.FromLTRB(0, 0,
				(float)pict.Size.Width / pict.HorizontalResolution * g.DpiX * g.PageScale,
				(float)pict.Size.Height / pict.VerticalResolution * g.DpiY * g.PageScale);
			RectangleF sizeDoc = deviceToDoc(g, sizeDev);
			float picw = sizeDoc.Width;
			float pich = sizeDoc.Height;

			switch (ppos)
			{
			case ImageAlign.TopLeft:
				xoff = rect.Left;
				yoff = rect.Top;
				break;
			case ImageAlign.BottomLeft:
				xoff = rect.Left;
				yoff = rect.Bottom - pich;
				break;
			case ImageAlign.TopRight:
				xoff = rect.Right - picw;
				yoff = rect.Top;
				break;
			case ImageAlign.BottomRight:
				xoff = rect.Right - picw;
				yoff = rect.Bottom - pich;
				break;
			case ImageAlign.Center:
				xoff = (rect.Right + rect.Left - picw) / 2;
				yoff = (rect.Bottom + rect.Top - pich) / 2;
				break;
			case ImageAlign.TopCenter:
				xoff = rect.X + rect.Width / 2 - picw / 2;
				yoff = rect.Y;
				break;
			case ImageAlign.BottomCenter:
				xoff = rect.X + rect.Width / 2 - picw / 2;
				yoff = rect.Bottom - pich;
				break;
			case ImageAlign.MiddleLeft:
				xoff = rect.X;
				yoff = rect.Y + rect.Height / 2 - pich / 2;
				break;
			case ImageAlign.MiddleRight:
				xoff = rect.Right - picw;
				yoff = rect.Y + rect.Height / 2 - pich / 2;
				break;
			case ImageAlign.Fit:
				{
					float h = rect.Bottom - rect.Top;
					float w = rect.Right - rect.Left;
					if (h == 0) break;

					float ratioCtrl = w / h;
					float ratioPic = picw / pich;

					if (ratioCtrl > ratioPic)
					{
						//stretch vertically
						pich = (int)h;
						picw = (int)(ratioPic * pich);
						yoff = rect.Top;
						xoff = (rect.Right + rect.Left - picw) / 2;
					}
					else
					{
						//stretch horizontally
						picw = (int)w;
						if (ratioPic == 0) break;
						pich = (int)(picw / ratioPic);
						xoff = rect.Left;
						yoff = (rect.Bottom + rect.Top - pich) / 2;
					}
				}
				break;
			case ImageAlign.Stretch:
				{
					picw = rect.Right - rect.Left;
					pich = rect.Bottom - rect.Top;
					xoff = rect.Left; yoff = rect.Top;
				}
				break;
			case ImageAlign.Tile:
				{
					xoff = rect.Left; yoff = rect.Top;
					xc = (int)((rect.Right - rect.Left) / picw) + 1;
					yc = (int)((rect.Bottom - rect.Top) / pich) + 1;
				}
				break;
			}

			if (ppos != ImageAlign.Tile)
			{
				Point alignedDevPos = docToDevice(g, new PointF(xoff, yoff));
				PointF alignedDocPos = deviceToDoc(g, alignedDevPos);

				if (rotationAngle != 0)
				{
					// Apply rotation transformation
					g.TranslateTransform(pivot.X, pivot.Y);
					g.RotateTransform(rotationAngle);
					g.TranslateTransform(-pivot.X, -pivot.Y);
				}

				g.DrawImage(pict, alignedDocPos.X, alignedDocPos.Y, picw, pich);

				if (rotationAngle != 0)
				{
					// Remove rotation
					g.TranslateTransform(pivot.X, pivot.Y);
					g.RotateTransform(-rotationAngle);
					g.TranslateTransform(-pivot.X, -pivot.Y);
				}
			}
			else
			{
				RectangleF sizeDev2 = RectangleF.FromLTRB(0, 0,
					1+(float)Math.Ceiling((double)pict.Size.Width / pict.HorizontalResolution * g.DpiX * g.PageScale),
					1+(float)Math.Ceiling((double)pict.Size.Height / pict.VerticalResolution * g.DpiY * g.PageScale));
				RectangleF sizeDoc2 = deviceToDoc(g, sizeDev2);
				float picw2 = sizeDoc2.Width;
				float pich2 = sizeDoc2.Height;

				if (rotationAngle != 0)
				{
					// Apply rotation transformation
					g.TranslateTransform(pivot.X, pivot.Y);
					g.RotateTransform(rotationAngle);
					g.TranslateTransform(-pivot.X, -pivot.Y);
				}

				for (int x = 0; x < xc; x++)
					for (int y = 0; y < yc; y++)
						g.DrawImage(pict, xoff+x*picw, yoff+y*pich, picw2, pich2);

				if (rotationAngle != 0)
				{
					// Remove rotation
					g.TranslateTransform(pivot.X, pivot.Y);
					g.RotateTransform(-rotationAngle);
					g.TranslateTransform(-pivot.X, -pivot.Y);
				}
			}
		}

		internal static PointF symmetricPt(PointF pt, PointF ptCenter)
		{
			PointF ptRes = new PointF(0, 0);

			float dx = pt.X - ptCenter.X;
			float dy = pt.Y - ptCenter.Y;

			ptRes.X = ptCenter.X - dx;
			ptRes.Y = ptCenter.Y - dy;

			return ptRes;
		}

		internal static PointF getRectPtPercent(PointF pt, RectangleF rc)
		{
			PointF ptPer = new PointF(50f, 50f);

			float w = rc.Right - rc.Left;
			float h = rc.Bottom - rc.Top; 

			if (w != 0 && h != 0)
			{
				ptPer.X = (pt.X - rc.Left) * 100 / w;
				ptPer.Y = (pt.Y - rc.Top) * 100 / h;
			}

			return ptPer;
		}

		internal static PointF rectPtFromPercent(PointF ptPer, RectangleF rc)
		{
			PointF pt = new PointF(0, 0);

			float w = rc.Right - rc.Left;
			float h = rc.Bottom - rc.Top; 

			pt.X = rc.Left + ptPer.X * w / 100f;
			pt.Y = rc.Top + ptPer.Y * h / 100f;

			return pt;
		}

		internal static PointF getRotatedPt(PointF pt, float rotation)
		{
			PointF res = pt;

			if (rotation == 0)
				return res;

			if (rotation == 90)
			{
				res.X = 100 - pt.Y;
				res.Y = pt.X;
			}
			else if (rotation == 180)
			{
				res.X = 100 - pt.X;
				res.Y = 100 - pt.Y;
			}
			else
			{
				res.X = pt.Y;
				res.Y = 100 - pt.X;
			}

			return res;
		}

		internal static PointF GetBezierPt(PointCollection points, int segment, float t)
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

		public static PointCollection approxEllipse(RectangleF rect, int quality)
		{
			PointCollection approximation = new PointCollection(0);
			float epsilon = 1.0f / quality;

			// get the points defining the curve
			float a = rect.Width / 2;
			float b = rect.Height / 2;
			float cx = rect.X + a;
			float cy = rect.Y + b;

			for (float t = 0; t <= 3.1415f * 2; t += epsilon)
			{
				float x = (float)(cx + a * Math.Cos(t));
				float y = (float)(cy + b * Math.Sin(t));

				// draw straight line between last two calculated points
				approximation.Add(new PointF(x, y));
			}

			return approximation;
		}

		internal static Type getLoadedType(string typeName, string asmName)
		{
			AppDomain domain = AppDomain.CurrentDomain;
			Assembly[] loaded = domain.GetAssemblies();
			foreach (Assembly asm in loaded)
			{
				if (asm.GetName().Name == asmName)
					return asm.GetType(typeName, false);
			}

			return null;
		}

		internal static Assembly getLoadedAssembly(string asmName)
		{
			AppDomain domain = AppDomain.CurrentDomain;
			Assembly[] loaded = domain.GetAssemblies();
			foreach (Assembly asm in loaded)
			{
				//if (asm.GetName().FullName == asmName)
				if (asm.FullName == asmName)
					return asm;
			}

			return null;
		}

		internal static void getProjections(PointF pt, RectangleF rc, PointF[] proj)
		{
			proj[0] = proj[1] = proj[2] = proj[3] = pt;
			proj[0].Y = rc.Top;
			proj[1].Y = rc.Bottom;
			proj[2].X = rc.Right;
			proj[3].X = rc.Left;
		}

		internal static bool getTextEllipsis(StringFormat textFormat)
		{
			if (textFormat == null)
				return false;

			return
				textFormat.Trimming == StringTrimming.EllipsisCharacter ||
				textFormat.Trimming == StringTrimming.EllipsisPath ||
				textFormat.Trimming == StringTrimming.EllipsisWord;
		}

		internal static void getPathIntersection(GraphicsPath path,
			PointF org, PointF end, ref PointF result)
		{
			ArrayList pts = new ArrayList();
			PointF[] ppoints = path.PathPoints;
			for (int i = 0; i < ppoints.GetLength(0); ++i)
			{
				PointF p = new PointF(0, 0);
				if (Utilities.segmentIntersect(ppoints[i],
					ppoints[(i+1)%ppoints.GetLength(0)], org, end, ref p))
				{
					pts.Add(p);
				}
			}
			float distance = float.MaxValue;
			while (pts.Count > 0)
			{
				PointF p = (PointF)pts[0];
				float dist2 = (float)(Math.Sqrt((p.X-end.X)*(p.X-end.X) + (p.Y-end.Y)*(p.Y-end.Y)));
				if (dist2 < distance)
				{
					distance = dist2;
					result = p;
				}
				pts.RemoveAt(0);
			}
		}

		internal static PointF getCenter(RectangleF rect)
		{
			return new PointF(rect.X + rect.Width / 2, rect.Y + rect.Height / 2);
		}

	}

}
