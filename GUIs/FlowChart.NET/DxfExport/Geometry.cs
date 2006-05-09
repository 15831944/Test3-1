// Copyright (c) 2003-2005, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;


namespace MindFusion.Geometry.Geometry2D
{
	/// <summary>
	/// Provides various conversions.
	/// </summary>
	//[Access(AccessType.Public)]
	internal class Convert
	{
		/// <summary>
		/// Converts polar coordinates to the corresponding
		/// dekart coordinates, using the specified point as
		/// a center of the coordinate system.
		/// </summary>
		public static void PolarToDekart(PointF coordCenter,
			float a, float r, ref PointF dekart)
		{
			if(r == 0)
			{
				dekart = coordCenter;
				return;
			}

			dekart.X = (float)(coordCenter.X + Math.Cos(a * Math.PI / 180) * r);
			dekart.Y = (float)(coordCenter.Y - Math.Sin(a * Math.PI / 180) * r);
		}

		/// <summary>
		/// Converts dekart coordinates to the corresponding
		/// polar coordinates, using the specified point as
		/// a center of the coordinate system.
		/// </summary>
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
	}

	/// <summary>
	/// Represents collection of points.
	/// </summary>
	//[Access(AccessType.Public)]
	internal class PointList : CollectionBase
	{
		public PointList() {}

		public PointList(ICollection points)
		{
			InnerList.AddRange(points);
		}

		public void Add(PointF p)
		{
			InnerList.Add(p);
		}

		public void AddRange(ICollection c)
		{
			InnerList.AddRange(c);
		}

		public void Insert(int index, PointF p)
		{
			InnerList.Insert(index, p);
		}

		public void Remove(PointF p)
		{
			InnerList.Remove(p);
		}

		public void Sort(IComparer comparer)
		{
			InnerList.Sort(comparer);
		}

		public PointF this[int index]
		{
			get { return (PointF)List[index]; }
			set { List[index] = value; }
		}

		public PointF[] ToArray()
		{
			PointF[] points = new PointF[this.Count];
			for (int i = 0; i < this.Count; i++)
				points[i] = this[i];
			return points;
		}
	}

	//[Access(AccessType.Public)]
	internal class ReadOnlyPointList : ReadOnlyCollectionBase
	{
		public ReadOnlyPointList(ICollection points)
		{
			InnerList.AddRange(points);
		}

		public PointF this[int index]
		{
			get { return (PointF)InnerList[index]; }
		}
	}


	/// <summary>
	/// Represents line in the plane.
	/// </summary>
	//[Access(AccessType.Public)]
	internal class Line
	{
		public Line(PointF a, PointF b)
		{
			_a = a;
			_b = b;
		}

		public Line(Line line)
		{
			_a = line._a;
			_b = line._b;
		}


		/// <summary>
		/// Calculates the intersection point with the line
		/// defined by the given points. If there is no intersection
		/// the return value is an infinite point.
		/// </summary>
		public PointF IntersectLine(PointF a, PointF b)
		{
			PointF pt = new PointF(float.PositiveInfinity, float.PositiveInfinity);

			if (_a.X == _b.X && a.X == b.X)
				return pt;

			if (_a.X == _b.X)
			{
				double a2 = (double)(a.Y - b.Y) / (a.X - b.X);
				double b2 = (double)(a.X * b.Y - b.X * a.Y) / (a.X - b.X);

				pt.X = _a.X;
				pt.Y = (float)(a2 * pt.X + b2);

				return pt;
			}

			if (a.X == b.X)
			{
				double a2 = (double)(_a.Y - _b.Y) / (_a.X - _b.X);
				double b2 = (double)(_a.X * _b.Y - _b.X * _a.Y) / (_a.X - _b.X);

				pt.X = a.X;
				pt.Y = (float)(a2 * pt.X + b2);

				return pt;
			}

			double a3 = (double)(_a.Y - _b.Y) / (_a.X - _b.X);
			double b3 = (double)(_a.X * _b.Y - _b.X * _a.Y) / (_a.X - _b.X);

			double a4 = (double)(a.Y - b.Y) / (a.X - b.X);
			double b4 = (double)(a.X * b.Y - b.X * a.Y) / (a.X - b.X);

			if (a3 != a4)
			{
				pt.X = (float)((b4 - b3) / (a3 - a4));
				pt.Y = (float)(a3 * (b4 - b3) / (a3 - a4) + b3);
			}

			return pt;
		}

		/// <summary>
		/// Calculates the intersection point with the specified line.
		/// If there is no intersection the return value is an infinite point.
		/// </summary>
		public PointF IntersectLine(Line line)
		{
			return IntersectLine(line.A, line.B);
		}

		/// <summary>
		/// Calculates the intersection point with the 
		/// line segment defined by the given points.
		/// If there is no intersection the return
		/// value is an infinite point.
		/// </summary>
		public PointF IntersectSegment(PointF a, PointF b)
		{
			PointF pt = IntersectLine(a, b);

			// Check if there is actual segment intersection
			if (Between(pt.X, _a.X, _b.X, true) &&
				Between(pt.Y, _a.Y, _b.Y, false) &&
				Between(pt.X, a.X, b.X, true) &&
				Between(pt.Y, a.Y, b.Y, false))
				return pt;

			pt.X = float.PositiveInfinity;
			pt.Y = float.PositiveInfinity;

			return pt;
		}

		/// <summary>
		/// Calculates the intersection point with the given line segment.
		/// If there is no intersection the return value is an infinite point.
		/// </summary>
		public PointF IntersectSegment(Line line)
		{
			return IntersectSegment(line.A, line.B);
		}

		/// <summary>
		/// Checks whether the given point lies on the line segment.
		/// </summary>
		public bool ContainsInSegment(PointF point)
		{
			if (point.Equals(_a) || point.Equals(_b))
				return true;

			if (!Bounds.Contains(point))
				return false;

			if (_a.X == _b.X) return point.X == _a.X;
			if (_a.Y == _b.Y) return point.Y == _a.Y;

			return (point.X - _a.X) * (_b.Y - _a.Y) ==
				(point.Y - _a.Y) * (_b.X - _a.X);
		}


		public PointF A
		{
			get { return _a; }
			set { _a = value; }
		}

		public PointF B
		{
			get { return _b; }
			set { _b = value; }
		}

		/// <summary>
		/// Gets the smallest rectangle bounding the segment
		/// which defines the line.
		/// </summary>
		public RectangleF Bounds
		{
			get
			{
				return RectangleF.FromLTRB(
					Math.Min(_a.X, _b.X), Math.Min(_a.Y, _b.Y),
					Math.Max(_a.X, _b.X), Math.Max(_a.Y, _b.Y));
			}
		}


		/// <summary>
		/// Checks whether x lies between y and z.
		/// </summary>
		private bool Between(float x, float y, float z, bool extend)
		{
			float min = Math.Min(y, z);
			float max = Math.Max(y, z);

			if (extend)
			{
				min -= 0.0001f;
				max += 0.0001f;
			}

			return (min <= x && x <= max);
		}


		private PointF _a;
		private PointF _b;
	}


	/// <summary>
	/// Represents immutable closed plane figure with
	/// three or more straight sides.
	/// </summary>
	//[Access(AccessType.Public)]
	internal class Polygon
	{
		/// <summary>
		/// Builds new polygon object from a given points list.
		/// </summary>
		public Polygon(PointList points)
		{
			if (points.Count < 3)
				throw new Exception("Polygons need at least 3 points.");

			_points = new PointList(points);

			Complete();
		}

		/// <summary>
		/// Builds new polygon object from the specified rectangle.
		/// </summary>
		/// <param name="rect"></param>
		public Polygon(RectangleF rect)
		{
			float l = Math.Min(rect.Left, rect.Right);
			float r = Math.Max(rect.Left, rect.Right);
			float t = Math.Min(rect.Top, rect.Bottom);
			float b = Math.Max(rect.Top, rect.Bottom);

			_points = new PointList(
				new PointF[]
				{
					new PointF(l, t),
					new PointF(r, t),
					new PointF(r, b),
					new PointF(l, b)
				});

			Complete();
		}

		/// <summary>
		/// Build new polygon from ellipse. The ellipse is
		/// defined by the specified bounding rectangle. The
		/// second parameter specifies how close the
		/// polygon approximates the ellipse.
		/// Factor of 0 specifies simplest polygon, while
		/// factor 100 specifies the most complex polygon.
		/// </summary>
		/// <param name="bounds"></param>
		/// <param name="factor"></param>
		public Polygon(RectangleF bounds, int factor)
		{
			if (factor < 0)
				factor = 0;
			if (factor > 100)
				factor = 100;

			GraphicsPath path = new GraphicsPath();

			path.AddEllipse(bounds);
			path.Flatten(new Matrix(), (float)factor / 100f);
			_points = new PointList(path.PathPoints);

			path.Dispose();

			Complete();
		}

		/// <summary>
		/// Internal initialization routine.
		/// </summary>
		private void Complete()
		{
			// Close the polygon
			if (_points[0] != _points[_points.Count - 1])
				_points.Add(_points[0]);

			_readOnlyPoints = new ReadOnlyPointList(_points);

			// Calculate polygon bounding rectangle
			float l = float.MaxValue;
			float t = float.MaxValue;
			float r = float.MinValue;
			float b = float.MinValue;
			foreach (PointF point in _points)
			{
				l = Math.Min(l, point.X);
				t = Math.Min(t, point.Y);
				r = Math.Max(r, point.X);
				b = Math.Max(b, point.Y);
			}
			_bounds = new RectangleF(l, t, r - l, b - t);
		}

		/// <summary>
		/// Calculates the intersections between the 
		/// polygon and the line segment defined by the given points.
		/// The result is a list containing all points of intersection.
		/// </summary>
		public PointList IntersectSegment(PointF a, PointF b)
		{
			PointList result = new PointList();
			PointF point;

			for (int i = 0; i < _points.Count - 1; i++)
			{
				point = new Line(_points[i], _points[i + 1]).
					IntersectSegment(a, b);

				if (!float.IsPositiveInfinity(point.X))
					result.Add(point);
			}

			return result;
		}

		/// <summary>
		/// Calculates the intersections between the 
		/// polygon and the given line segment. The result is
		/// a list containing all points of intersection.
		/// </summary>
		public PointList IntersectSegment(Line line)
		{
			return IntersectSegment(line.A, line.B);
		}

		/// <summary>
		/// Calculates the intersections between the 
		/// polygon and the line defined by the given points.
		/// The result is a list containing all points of intersection.
		/// </summary>
		public PointList IntersectLine(PointF a, PointF b)
		{
			PointList result = new PointList();
			PointF point;

			for (int i = 0; i < _points.Count - 1; i++)
			{
				point = new Line(_points[i], _points[i + 1]).
					IntersectLine(a, b);

				if (!float.IsPositiveInfinity(point.X))
					result.Add(point);
			}

			return result;
		}

		/// <summary>
		/// Calculates the intersections between the 
		/// polygon and the given line. The result is
		/// a list containing all points of intersection.
		/// </summary>
		public PointList IntersectLine(Line line)
		{
			return IntersectLine(line.A, line.B);
		}

		/// <summary>
		/// Checks whether the specified point lies
		/// whithin the polygon.
		/// </summary>
		public bool Contains(PointF point)
		{
			if (!_bounds.Contains(point))
				return false;

			int intersections = 0;
			PointF point2;

			point2 = point;
			point2.X = _bounds.Right + 50;

			// Now go through each of the lines in
			// the polygon and see if it intersects
			for (int i = 0; i < _points.Count - 1; i++)
			{
				Line line = new Line(_points[i], _points[i + 1]);

				if (line.ContainsInSegment(point)) return true;
				if (Intersect(line.A, line.B, point, point2))
					intersections++;
			}

			return (intersections % 2 > 0);
		}

		/// <summary>
		/// Given two line segments, determine if they intersect.
		/// </summary>
		private bool Intersect(PointF p1, PointF p2, PointF p3, PointF p4)
		{
			return ((( CCW(p1, p2, p3) * CCW(p1, p2, p4)) <= 0)
				&& (( CCW(p3, p4, p1) * CCW(p3, p4, p2)  <= 0) ));
		}

		/// <summary>
		/// Determines, given three points, if when travelling from the first to
		/// the second to the third, we travel in a counterclockwise direction.
		/// </summary>
		private int CCW(PointF p0, PointF p1, PointF p2)
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

		/// <summary>
		/// Finds a point inside the polygon.
		/// </summary>
		public PointF GetInternalPoint()
		{
			int cvi = FindConvexVertex();
			int prev = cvi > 0 ? cvi - 1 : _points.Count - 1;
			int next = (cvi + 1) % _points.Count;

			PointF v = _points[cvi];
			PointF a = _points[prev];
			PointF b = _points[next];

			PointList pl = new PointList();
			pl.Add(a); pl.Add(v); pl.Add(b);

			Polygon avb = new Polygon(pl);

			float minDist = float.MaxValue;
			PointF intPt = v;

			for (int i = 0; i < _points.Count; ++i)
			{
				if (i == cvi) continue;
				if (i == prev) continue;
				if (i == next) continue;

				PointF q = _points[i];
				if (avb.Contains(q))
				{
					float dist = (float)Math.Sqrt((q.X-v.X)*(q.X-v.X) + (q.Y-v.Y)*(q.Y-v.Y));
					if (dist < minDist)
					{
						minDist = dist;
						intPt = q;
					}
				}
			}

			if (intPt == v)
				return new PointF((a.X + b.X) / 2, (a.Y + b.Y) / 2);
			else
				return new PointF((v.X + intPt.X)/ 2, (v.Y + intPt.Y) / 2);
		}

		/// <summary>
		/// Finds a convex vertex of the polygon.
		/// </summary>
		public int FindConvexVertex()
		{
			float bottomMost = float.MinValue;
			float rightMost = float.MinValue;
			int cvi = 0;

			for (int i = 0; i < _points.Count; ++i)
			{
				PointF point = _points[i];
				if (point.Y > bottomMost)
				{
					bottomMost = point.Y;
					rightMost = point.X;
					cvi = i;
				}
				else if (point.Y == bottomMost)
				{
					if (point.X > rightMost)
					{
						rightMost = point.X;
						cvi = i;
					}
				}
			}

			return cvi;
		}

		/// <summary>
		/// Gets a collection of the polygon points.
		/// </summary>
		public ReadOnlyPointList Points
		{
			get { return _readOnlyPoints; }
		}

		/// <summary>
		/// Gets the smallest rectangle bounding the polygon.
		/// </summary>
		public RectangleF Bounds
		{
			get { return _bounds; }
		}


		private PointList _points;
		private ReadOnlyPointList _readOnlyPoints;
		private RectangleF _bounds;
	}
}
