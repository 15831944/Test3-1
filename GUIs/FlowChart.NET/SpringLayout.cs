// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using MindFusion.Geometry.Geometry2D;

namespace MindFusion.LayoutSystem
{
	// Spring layout classes

	#region Spring Layout data structures
	[Access(AccessType.Public)]
	internal class SpringLayoutInfo
	{
		public SpringLayoutInfo() {}
		public SpringLayoutInfo(
			int iterationCount, float nodeDistance,
			bool minimizeCrossings, bool enableClusters)
		{
			_iterationCount = iterationCount;
			_nodeDistance = nodeDistance;
			_minimizeCrossings = minimizeCrossings;
			_enableClusters = enableClusters;
		}

		public int IterationCount
		{
			get { return _iterationCount; }
			set { _iterationCount = value; }
		}

		public float NodeDistance
		{
			get { return _nodeDistance; }
			set { _nodeDistance = value; }
		}

		public bool MinimizeCrossings
		{
			get { return _minimizeCrossings; }
			set { _minimizeCrossings = value; }
		}

		public bool EnableClusters
		{
			get { return _enableClusters; }
			set { _enableClusters = value; }
		}

		public double RepulsionFactor
		{
			get { return _repulsionFactor; }
			set { _repulsionFactor = value; }
		}

		public int RndSeed
		{
			get { return _rndSeed; }
			set { _rndSeed = value; }
		}


		private int _iterationCount = 100;
		private float _nodeDistance = 30;
		private bool _minimizeCrossings = false;
		private bool _enableClusters = false;
		private double _repulsionFactor = 2.5;
		private int _rndSeed = 0;
	}

	[Access(AccessType.Internal)]
	internal class SpringEdgeData
	{
		public double Length
		{
			get { return _length; }
			set { _length = value; }
		}
		public int Crossings
		{
			get { return _crossings; }
			set { _crossings = value; }
		}

		private double _length = 60;
		private int _crossings = 0;
	}

	[Access(AccessType.Internal)]
	internal class SpringVertexData
	{
		public double EdgeDX
		{
			get { return _edgedx; }
			set { _edgedx = value; }
		}
		public double EdgeDY
		{
			get { return _edgedy; }
			set { _edgedy = value; }
		}
		public double RepulsionDX
		{
			get { return _repulsiondx; }
			set { _repulsiondx = value; }
		}
		public double RepulsionDY
		{
			get { return _repulsiondy; }
			set { _repulsiondy = value; }
		}
		public double DX
		{
			get { return _dx; }
			set { _dx = value; }
		}
		public double DY
		{
			get { return _dy; }
			set { _dy = value; }
		}
		public int Crossings
		{
			get { return _crossings; }
			set { _crossings = value; }
		}


		double _edgedx = 0;
		double _edgedy = 0;
		double _repulsiondx = 0;
		double _repulsiondy = 0;

		// movement speed, x
		double _dx = 0;
		// movement speed, y
		double _dy = 0;

		int _crossings = 0;
	}
	#endregion

	#region class SpringLayout
	[Access(AccessType.Public)]
	internal class SpringLayout
	{
		// Runtime data indices
		private const int _SpringEdge = 1;
		private const int _SpringVertex = 2;
		private const int _VertexRadius = 3;

		private const double _ForceConstant = 1.0 / 3.0;
		private double _stretch = 70;
		private const double _Range = 100;


		public SpringLayout()
		{
			_graph = null;
			_info = null;
			_r = new Random(System.DateTime.Now.Second);
		}

		public double Stretch
		{
			get { return _stretch; }
			set { _stretch = value; }
		}

		private RectangleF CalcContentRect(Graph graph)
		{
			RectangleF res = RectangleF.Empty;

			foreach(GraphNode node in graph.Nodes)
			{
				if(res == RectangleF.Empty)
					res = node.Bounds;
				else
					res = Utilities.UnionRects(res, node.Bounds);
			}

			return res;
		}

		private int CountCrossings()
		{
			// Initialize
			foreach(GraphNode node in _graph.Nodes)
			{
				SpringVertexData data = (SpringVertexData)node.GetData(_SpringVertex);
				data.Crossings = 0;
			}
			foreach(GraphLink link in _graph.Links)
			{
				SpringEdgeData data = (SpringEdgeData)link.GetData(_SpringEdge);
				data.Crossings = 0;
			}

			foreach(GraphLink link in _graph.Links)
				CountEdgeCrossings(link);

			int crossings = 0;
			foreach(GraphNode node in _graph.Nodes)
			{
				SpringVertexData data = (SpringVertexData)node.GetData(_SpringVertex);
				crossings += data.Crossings;
			}

			return crossings;
		}

		private void CountEdgeCrossings(GraphLink edge)
		{
			int crossings = 0;

			GraphNode v1 = edge.Origin;
			GraphNode v2 = edge.Destination;

			SpringVertexData v1D = (SpringVertexData)v1.GetData(_SpringVertex);
			SpringVertexData v2D = (SpringVertexData)v2.GetData(_SpringVertex);

			PointF pt = new PointF(0, 0), pt11, pt12, pt21, pt22;
			pt11 = v1.Center;
			pt12 = v2.Center;

			foreach(GraphLink test in _graph.Links)
			{
				if(edge == test)
					continue;

				pt21 = test.Origin.Center;
				pt22 = test.Destination.Center;
				if(pt11 != pt21 && pt11 != pt22 && pt12 != pt21 && pt12 != pt22)
				{
					if (new Line(pt11, pt12).IntersectSegment(pt21, pt22).X !=
						float.PositiveInfinity)
					{
						crossings++;
						v1D.Crossings++;
						v2D.Crossings++;
					}
				}
			}

			SpringEdgeData data = (SpringEdgeData)edge.GetData(_SpringEdge);
			data.Crossings = crossings;
		}

		private GraphNode NodeWithMostCrossings()
		{
			GraphNode node = null;
			int max = 0;
			int numLinks = int.MaxValue;

			foreach (GraphNode n in _graph.Nodes)
			{
				SpringVertexData data = (SpringVertexData)n.GetData(_SpringVertex);
				if (data.Crossings > max ||
					(data.Crossings == max && n.LinkCount < numLinks))
				{
					node = n;
					max = data.Crossings;
					numLinks = n.LinkCount;
				}
			}

			return node;
		}

		private void TryDecreaseCrossings(RectangleF doc, float ddist, int crossings)
		{
			GraphNode node = NodeWithMostCrossings();
			if(node == null)
				return;

			SpringVertexData data = (SpringVertexData)node.GetData(_SpringVertex);
			PointF pos = node.Center;
			PointF minPos = pos;

			for(int c = 3; c >= 1; c--)
			{
				for(int h = -1; h <= 1; h++)
				{
					for (int v = -1; v <= 1; v++)
					{
						if (h == 0 && v == 0) continue;

						PointF newPos = new PointF(0, 0);
						newPos.X = pos.X + c * h * ddist;
						newPos.Y = pos.Y + c * v * ddist;
				
						if(newPos.X < doc.Left) continue;
						if(newPos.X > doc.Right) continue;
						if(newPos.Y < doc.Top)continue;
						if(newPos.Y > doc.Bottom) continue;

						node.Center = newPos;

						int newCrossings = CountCrossings();
						if(newCrossings < crossings)
							minPos = newPos;
					}
				}
			}

			if(minPos != pos)
				node.Center = minPos;
		}

		public bool Arrange(IGraph graph, SpringLayoutInfo layoutInfo)
		{
			return Arrange(graph, layoutInfo, null);
		}

		public bool Arrange(IGraph graph,
			SpringLayoutInfo layoutInfo, LayoutProgress progress)
		{
			_info = layoutInfo;

			if (_info.RndSeed != 0)
			{
				// Use the randomization seed supplied by the user
				_r = new Random(_info.RndSeed);
			}

			// Build the internal graph
			_graph = new Graph(graph);

			RectangleF rcDoc = CalcContentRect(_graph);

			// Initialize
			foreach (GraphNode node in _graph.Nodes)
				node.SetData(_SpringVertex, new SpringVertexData());
			foreach (GraphLink link in _graph.Links)
				link.SetData(_SpringEdge, new SpringEdgeData());

			// Calculate vertex radiuses
			foreach (GraphNode node in _graph.Nodes)
			{
				RectangleF rc = node.Bounds;
				node.SetData(_VertexRadius,
					(double)(rc.Width + rc.Height) / 4);
			}

			// Update progress
			int total = layoutInfo.IterationCount;
			float ffactor = total / 100;
			int factor = 1;
			if (ffactor > 1)
			{
				factor = (int)Math.Floor((double)ffactor);
				total = total / factor + 1;
			}
			if (progress != null)
				progress(0, total);

			// An iterative layout algorithm
			for(int step = 1; step <= layoutInfo.IterationCount; step++)
			{
				// Update progress
				if (progress != null)
				{
					if (step % factor == 0)
						progress(step / factor, total);
				}

				foreach(GraphNode node in _graph.Nodes)
				{
					SpringVertexData data = (SpringVertexData)
						node.GetData(_SpringVertex);
					data.DX /= 4;
					data.DY /= 4;
					data.EdgeDX = data.EdgeDY = 0;
					data.RepulsionDX = data.RepulsionDY = 0;
				}

				// here the "springs" confine nodes to the desired node distance
				foreach (GraphLink link in _graph.Links)
				{
					// get incident nodes
					GraphNode v1 = link.Origin;
					GraphNode v2 = link.Destination;

					// compute current distance between nodes
					PointF v1Pos = v1.Center;
					PointF v2Pos = v2.Center;
					double vx = v1Pos.X - v2Pos.X;
					double vy = v1Pos.Y - v2Pos.Y;
					double dist = Math.Sqrt(vx * vx + vy * vy);
					dist = (dist == 0) ? 0.0001 : dist;

					// get desired distance accomodated for node extents
					double desiredDist = _info.NodeDistance;
					desiredDist += ((double)v1.GetData(_VertexRadius) +
						(double)v2.GetData(_VertexRadius)) / 2;

					// now handling cluster center nodes ?
					bool masterNodes = _info.EnableClusters &&
						(v1.LinkCount > 4 && v2.LinkCount > 4);
					desiredDist *= masterNodes ? 4 : link.Link.Weight;

					// force factor: optimal length minus actual length,
					// is made smaller as the current actual length gets larger.
					double f = _ForceConstant * (desiredDist - dist) / dist;

					// nodes with few links are moved easier than ones with more
					double f1 = masterNodes ? f :
						(f * Math.Pow(_stretch / 100.0, v1.LinkCount - 1));
					double f2 = masterNodes ? f :
						(f * Math.Pow(_stretch / 100.0, v2.LinkCount - 1));

					// move first node towards the second one
					SpringVertexData v1D = (SpringVertexData)v1.GetData(_SpringVertex);
					v1D.EdgeDX += f1 * vx;
					v1D.EdgeDY += f1 * vy;

					// move second node towards the first one
					SpringVertexData v2D = (SpringVertexData)v2.GetData(_SpringVertex);
					v2D.EdgeDX += -f2 * vx;
					v2D.EdgeDY += -f2 * vy;
				}

				// here nodes tend to run away one from another
				foreach (GraphNode node in _graph.Nodes)
				{
					SpringVertexData svd = (SpringVertexData)node.GetData(_SpringVertex);
					double dx = 0;
					double dy = 0;

					foreach (GraphNode node2 in _graph.Nodes)
					{
						if (node2 == node)
							continue;

						// now handling cluster center nodes ?
						bool masterNodes = _info.EnableClusters &&
							(node.LinkCount > 4 && node2.LinkCount > 4);

						// if distance is longer than this, ignore repulsion
						double intrZone = _info.NodeDistance *
							(masterNodes ? 4.5 : _info.RepulsionFactor);

						// if distance is shorter than this, randomize node positions
						double tooClose = _info.NodeDistance / (masterNodes ? 1 : 5);

						// distance to move when using random position
						double moveRange = _info.NodeDistance * (masterNodes ? 5 : 1);

						// compute current distance between nodes
						double vx = node.Center.X - node2.Center.X;
						double vy = node.Center.Y - node2.Center.Y;
						double distance = vx * vx + vy * vy;

						// kind of simmulated annealing, use random jumps for nodes
						// that are close one to another; longer jumps for nodes that
						// overlap completely and shorter for nodes that don't overlap
						if (distance == 0)
						{
							dx += moveRange/2 + _r.Next() % ((int)(moveRange * 3));
							dy += moveRange/2 + _r.Next() % ((int)(moveRange * 3));
							if (_r.Next() % 2 == 1) dx = -dx;
							if (_r.Next() % 2 == 1) dy = -dy;
						}
						else if (distance < tooClose * tooClose)
						{
							dx += moveRange/2 + _r.Next((int)moveRange);
							dy += moveRange/2 + _r.Next((int)moveRange);
							if (_r.Next() % 2 == 1) dx = -dx;
							if (_r.Next() % 2 == 1) dy = -dy;
						}
						// if nodes are not that close, calculate normal repulsion
						else if (distance < intrZone * intrZone)
						{
							float mf = masterNodes ?
								(node.LinkCount + node2.LinkCount) : 1;
							dx += mf * vx / (distance * distance);
							dy += mf * vy / (distance * distance);
						}
					}

					// apply the summary repulsion of this node with all other nodes
					double dlen = dx * dx + dy * dy;
					if (dlen > 0)
					{
						dlen = Math.Sqrt(dlen) / 2;
						svd.RepulsionDX += dx / dlen;
						svd.RepulsionDY += dy / dlen;
					}
				}

				// sum repulsion and "spring" forces and move nodes
				foreach(GraphNode node in _graph.Nodes)
				{
					// update node movement speed
					SpringVertexData vd =
						(SpringVertexData)node.GetData(_SpringVertex);
					vd.DX += vd.RepulsionDX + vd.EdgeDX;
					vd.DY += vd.RepulsionDY + vd.EdgeDY;

					// move node
					PointF xyd = node.Center;
					xyd.X += (float)vd.DX;
					xyd.Y += (float)vd.DY;
					node.Center = xyd;
				}

				// try to decrease crossings if that option is enabled
				if (_info.MinimizeCrossings &&
					(step % 10 == 0) &&
					(step < _info.IterationCount * 2 / 3))
				{
					int currentCrossings = CountCrossings();
					if (currentCrossings > 0)
						TryDecreaseCrossings(graph.DocRect, _info.NodeDistance, currentCrossings);
				}
			}

			// offset the graph to its original location
			RectangleF rcDoc2 = CalcContentRect(_graph);
			float pdx = rcDoc2.Left - rcDoc.Left;
			float pdy = rcDoc2.Top - rcDoc.Top;
			foreach(GraphNode node in _graph.Nodes)
			{
				PointF xyd = node.Center;
				xyd.X -= pdx;
				xyd.Y -= pdy;
				node.Center = xyd;
			}

			// update flowchart nodes positions
			foreach(GraphNode node in _graph.Nodes)
				node.Node.Bounds = node.Bounds;

			// call progress delegate
			if (progress != null)
				progress(total, total);

			return true;
		}

		private Graph _graph;
		private SpringLayoutInfo _info;
		private Random _r;
	}
	#endregion
}
