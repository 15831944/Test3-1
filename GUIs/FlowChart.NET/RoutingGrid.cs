// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Manages allocation of routing grid arrays.
	/// </summary>
	internal class RoutingGrid
	{
		/// <summary>
		/// initialize the routing grid
		/// </summary>
		internal RoutingGrid(FlowChart flowChart)
		{
			this.flowChart = flowChart;
			costGrid = null;
			closedGrid = null;
			openGrid = null;
		}

		/// <summary>
		/// allocates larger grid arrays if necessary
		/// </summary>
		internal void allocate(int cols, int rows, RectangleF bounds, Arrow arrow)
		{
			int curCols = 0;
			int curRows = 0;

			if (costGrid != null)
			{
				curCols = costGrid.GetLength(0);
				curRows = costGrid.GetLength(1);
			}

			// allocate new arrays if needed
			if (costGrid == null || curCols < cols || curRows < rows)
			{
				int newCols = Math.Max(curCols, cols);
				int newRows = Math.Max(curRows, rows);

				costGrid = new byte[newCols, newRows];
				closedGrid = new PathNode[newCols, newRows];
				openGrid = new PathNode[newCols, newRows];
			}
			//or reuse the existing ones
			else
			{
				Array.Clear(costGrid, 0, costGrid.Length);
				Array.Clear(closedGrid, 0, closedGrid.Length);
				Array.Clear(openGrid, 0, openGrid.Length);
			}

			// mark border lines as obstacles
			for (int c = 0; c < cols; c++)
			{
				costGrid[c, 0] = 255;
				costGrid[c, rows - 1] = 255;
			}
			for (int r = 1; r < rows - 1; r++)
			{
				costGrid[0, r] = 255;
				costGrid[cols - 1, r] = 255;
			}

			// mark node locations
			markObstacles(bounds, arrow, cols, rows);
		}

		/// <summary>
		/// mark obstacles and costs in the routing grid.
		/// </summary>
		private void markObstacles(RectangleF bounds,
			Arrow arrow, int cols, int rows)
		{
			RoutingOptions rop = flowChart.RoutingOptions;
			float gridSize = rop.GridSize;
			byte ccost = rop.CrossingCost;

			foreach (ChartObject obj in flowChart.Objects)
			{
				// if there is a crossing cost assigned, mark arrows in the route grid
				if (obj is Arrow && ccost > 0)
				{
					if (obj == arrow) continue;
					Arrow link = obj as Arrow;

					// at this time we handle only asPerpendicular links
					if (link.Style == ArrowStyle.Cascading)
					{
						PointF cp1 = link.ControlPoints[0];
						Point gp1 = new Point(
							(int)((cp1.X - bounds.Left) / gridSize),
							(int)((cp1.Y - bounds.Top) / gridSize));

						// iterate over all segments
						for (int i = 0; i < link.ControlPoints.Count - 1; ++i)
						{
							PointF cp2 = link.ControlPoints[i + 1];
							Point gp2 = new Point(
								(int)((cp2.X - bounds.Left) / gridSize),
								(int)((cp2.Y - bounds.Top) / gridSize));

							if (!gp1.Equals(gp2))
							{
								if (gp1.X == gp2.X)
								{
									// vertical segment
									if (gp1.X >= 0 && gp1.X < cols)
									{
										int miny = Math.Min(gp1.Y, gp2.Y);
										miny = Math.Max(0, miny);
										int maxy = Math.Max(gp1.Y, gp2.Y);
										maxy = Math.Min(rows - 1, maxy);
										for (int y = miny; y <= maxy; ++y)
											if (costGrid[gp1.X, y] < ccost) costGrid[gp1.X, y] = ccost;
									}
								}
								else
								{
									// horizontal segment
									if (gp1.Y >= 0 && gp1.Y < rows)
									{
										int minx = Math.Min(gp1.X, gp2.X);
										minx = Math.Max(0, minx);
										int maxx = Math.Max(gp1.X, gp2.X);
										maxx = Math.Min(cols - 1, maxx);
										for (int x = minx; x <= maxx; ++x)
											if (costGrid[x, gp1.Y] < ccost) costGrid[x, gp1.Y] = ccost;
									}
								}
							}

							gp1 = gp2;
						}
					}
					continue;
				}

				if (!(obj is Node))
					continue;

				Node node = obj as Node;
				if (!node.Obstacle)
					continue;
				if (node.MasterGroup != null && node.MasterGroup.MainObject == arrow)
					continue;

				RectangleF nodeRect = node.getRotatedBounds();

				if (bounds.IntersectsWith(nodeRect))
				{
					RectangleF intrRect = bounds;
					intrRect.Intersect(nodeRect);

					Point ptStart = new Point(
						(int)((intrRect.Left - bounds.Left) / gridSize),
						(int)((intrRect.Top - bounds.Top) / gridSize));
					Point ptEnd = new Point(
						(int)((intrRect.Right - bounds.Left) / gridSize),
						(int)((intrRect.Bottom - bounds.Top) / gridSize));

					if (ptStart.X < 0) ptStart.X = 0;
					if (ptStart.Y < 0) ptStart.Y = 0;
					if (ptEnd.X >= cols) ptEnd.X = cols - 1;
					if (ptEnd.Y >= rows) ptEnd.Y = rows - 1;

					// mark node interior as obstacle
					for (int c = ptStart.X; c <= ptEnd.X; ++c)
						for (int r = ptStart.Y; r <= ptEnd.Y; ++r)
							costGrid[c, r] = 255;

					// mark surrounding area with any cost assigned
					// going lineary down to the outside directions
					if (rop.NodeVicinityCost == 0) continue;
					for (int i = 1; i <= rop.NodeVicinitySize / gridSize; ++i)
					{
						byte cost = (byte)(rop.NodeVicinityCost / i);

						int minc = Math.Max(0, ptStart.X - i);
						int maxc = Math.Min(cols - 1, ptEnd.X + i);
						int minr = Math.Max(0, ptStart.Y - i);
						int maxr = Math.Min(rows - 1, ptEnd.Y + i);

						// top side
						int r = ptStart.Y - i;
						if (r >= 0)
						{
							for (int c = minc; c <= maxc; ++c)
								if (costGrid[c, r] < cost) costGrid[c, r] = cost;
						}

						// bottom side
						r = ptEnd.Y + i;
						if (r <= maxr)
						{
							for (int c = minc; c <= maxc; ++c)
								if (costGrid[c, r] < cost) costGrid[c, r] = cost;
						}

						// left side
						int cc = ptStart.X - i;
						if (cc >= 0)
						{
							for (r = minr; r <= maxr; ++r)
								if (costGrid[cc, r] < cost) costGrid[cc, r] = cost;
						}

						// right side
						cc = ptEnd.X + i;
						if (cc <= maxc)
						{
							for (r = minr; r <= maxr; ++r)
								if (costGrid[cc, r] < cost) costGrid[cc, r] = cost;
						}
					}
				}
			}
		}

		/// <summary>
		/// returns the grid of costs assigned to diagram locations
		/// </summary>
		internal byte[,] getCostGrid()
		{
			return costGrid;
		}

		/// <summary>
		/// returns the grid of closed path nodes
		/// </summary>
		internal PathNode[,] getClosedGrid()
		{
			return closedGrid;
		}

		/// <summary>
		/// returns the grid of open path nodes
		/// </summary>
		internal PathNode[,] getOpenGrid()
		{
			return openGrid;
		}

		FlowChart flowChart;
		byte[,] costGrid;
		PathNode[,] closedGrid;
		PathNode[,] openGrid;
	}

	/// <summary>
	/// Represents a path node, used by the link routing algorithm
	/// </summary>
	internal class PathNode : IComparable
	{
		internal PathNode(int x, int y)
		{
			X = x;
			Y = y;
			ChildCount = 0;
			Parent = null;
			for (int i = 0; i < Children.Length; i++)
				Children[i] = null;
		}

		// IComparable implementation
		public int CompareTo(object obj)
		{
			if (obj == null) return 1;
			if (!(obj is PathNode)) throw new ArgumentException();

			PathNode other = obj as PathNode;
			if (this == other) return 0;

			if (this.F > other.F) return 1;
			if (this.F < other.F) return -1;

			if (this.X > other.X) return 1;
			if (this.X < other.X) return -1;

			if (this.Y > other.Y) return 1;
			if (this.Y < other.Y) return -1;

			return 0;
		}

		// node location
		internal int X, Y;

		// node evaluation
		internal int F, G, H;

		// adjacent nodes
		internal PathNode Parent;
		internal int ChildCount;
		internal PathNode[] Children = new PathNode[4];
	};
}