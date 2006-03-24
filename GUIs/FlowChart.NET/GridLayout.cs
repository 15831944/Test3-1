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

namespace MindFusion.LayoutSystem
{
	[Access(AccessType.Public)]
	internal class GridLayoutInfo
	{
		public GridLayoutInfo()
		{
			_iterations = 3000;
			_gridSize = 25;
			_xGap = 10;
			_yGap = 10;
		}

		public GridLayoutInfo(int iterations,
			float gridSize, float xGap, float yGap)
		{
			_iterations = iterations;
			_gridSize = gridSize;
			_xGap = xGap;
			_yGap = yGap;
		}


		public int Iterations
		{
			get { return _iterations; }
			set { _iterations = value; }
		}

		public float GridSize
		{
			get { return _gridSize; }
			set { _gridSize = value; }
		}

		public float XGap
		{
			get { return _xGap; }
			set { _xGap = value; }
		}

		public float YGap
		{
			get { return _yGap; }
			set { _yGap = value; }
		}

		public INode StartNode
		{
			get { return _startNode; }
			set { _startNode = value; }
		}

		public INode EndNode
		{
			get { return _endNode; }
			set { _endNode = value; }
		}

		public int RndSeed
		{
			get { return _rndSeed; }
			set { _rndSeed = value; }
		}


		private int _iterations;
		private float _gridSize;
		private float _xGap;
		private float _yGap;
		private INode _startNode;
		private INode _endNode;
		private int _rndSeed = 0;
	}

	[Access(AccessType.Public)]
	internal class GridLayout
	{
		public GridLayout()
		{
			_random = new Random();
		}

		private const int _GridPos = 0;

		public bool Arrange(IGraph graph, GridLayoutInfo info)
		{
			return Arrange(graph, info, null);
		}

		public bool Arrange(IGraph graph,
			GridLayoutInfo info, LayoutProgress progress)
		{
			_graph = new Graph(graph);
			_info = info;

			if (_info.RndSeed != 0)
			{
				// Use the randomization seed supplied by the user
				_random = new Random(_info.RndSeed);
			}

			Path path = null;

			// Find the longest path between the start and the end node
			if (_info.StartNode != null && _info.EndNode != null)
			{
				path = PathFinder.FindLongestPath(graph, _info.StartNode, _info.EndNode);
			}
			else
			{
				path = PathFinder.FindLongestPath(graph);
			}

			if (path == null)
				return false;

			// Build path consisting of the corresponding GraphNode-s
			_backbone = new ArrayList();
			foreach (INode inode in path.Nodes)
			{
				foreach (GraphNode node in _graph.Nodes)
				{
					if (node.Node == inode)
					{
						_backbone.Add(node);
						break;
					}
				}
			}

			GraphNode[,] bestGrid = InitGrid();
			float bestEstimation = float.MaxValue;
			int maxShift = Math.Max(4, _gridWidth / 10); 

			// Update progress
			int current = 0;
			int total = CountIterations(maxShift);
			float ffactor = total / 100;
			int factor = 1;
			if (ffactor > 1)
			{
				factor = (int)Math.Floor((double)ffactor);
				total = total / factor + 1;
			}
			if (progress != null)
				progress(current, total);

			while (maxShift > 0)
			{
				for (int iter = 0;
					iter < (maxShift > 2 ?
						_info.Iterations :
						(_info.Iterations / 5 * maxShift * _graph.Nodes.Count));
					++iter)
				{
					// Update progress
					if (progress != null)
					{
						if (current % factor == 0)
							progress(current / factor, total);
						current++;
					}

					GraphNode[,] grid = ScrambleGrid(bestGrid, maxShift, iter);
					float estimation = EvaluateGrid(grid, maxShift);

					if (estimation < bestEstimation)
					{
						bestEstimation = estimation;
						bestGrid = grid;
					}
					else
					{
						ApplyGrid(bestGrid);
					}
				}

				maxShift--;
			}

			PlaceObjects(bestGrid);

			// Update progress
			if (progress != null)
				progress(total, total);

			return true;
		}

		private int CountIterations(int maxShift)
		{
			int iterations = 0;
			while (maxShift > 0)
			{
				iterations += (maxShift > 2 ?
					_info.Iterations :
					(_info.Iterations / 5 * maxShift * _graph.Nodes.Count));
				maxShift--;
			}

			return iterations;
		}

		private GraphNode[,] InitGrid()
		{
			int nodeCount = _graph.Nodes.Count;

			_gridWidth = _gridHeight = (int)Math.Ceiling(Math.Sqrt(nodeCount)) + 2;

			GraphNode[,] grid = new GraphNode[_gridWidth + 2, _gridHeight + 2];
			GraphNode node0 = _backbone[0] as GraphNode;
			node0.SetData(_GridPos, new Point(0, 0));
			grid[1, 1] = node0;

			int perRow = Math.Max(1, _backbone.Count / _gridHeight + 1);
			int curRow = 0;
			float curCol = 0;
			int c = 0;
			for (int i = 0; i < _backbone.Count - 1; i++)
			{
				int nodeCol = (int)curCol + c;
				int nodeRow = curRow;
				GraphNode node = _backbone[i] as GraphNode;
				node.SetData(_GridPos, new Point(nodeCol, nodeRow));
				grid[1 + nodeCol, 1 + nodeRow] = node;

				c++;
				if (c == perRow)
				{
					c = 0;
					curCol += ((float)_gridWidth - perRow) / _gridHeight;
					curRow++;
				}
			}

			c = 0;
			for (int i = 0; i < nodeCount; i++)
			{
				int nodeCol = i % _gridHeight;
				int nodeRow = i / _gridHeight;

				if (grid[1 + nodeCol, 1 + nodeRow] != null)
					continue;

				while (c < nodeCount && _backbone.Contains(_graph.Nodes[c]))
					c++;

				if (c == nodeCount) break;

				GraphNode node = _graph.Nodes[c] as GraphNode;
				node.SetData(_GridPos, new Point(nodeCol, nodeRow));
				grid[1 + nodeCol, 1 + nodeRow] = node;

				if (c == nodeCount - 1) break;

				c++;
			}

			GraphNode nodeN = _backbone[_backbone.Count - 1] as GraphNode;
			nodeN.SetData(_GridPos, new Point(_gridWidth - 1, _gridHeight - 1));
			grid[1 + _gridWidth - 1, 1 + _gridHeight - 1] = nodeN;

			return grid;
		}

		private GraphNode[,] ScrambleGrid(GraphNode[,] grid, int maxShift, int iter)
		{
			// Da ne e savsem random ami da gleda poveche da murda boxovete s 
			// poveche linkove - i v posoka na linkovete ?
			GraphNode[,] newGrid = (GraphNode[,])grid.Clone();

			if (maxShift > 2)
			{
				for (int i = 0; i < _gridWidth; i++)
					for (int j = 0; j < _gridHeight; ++j)
						ScrambleGridStep(newGrid, maxShift, iter, i, j);
			}
			else
			{
				int c = iter % _graph.Nodes.Count;
				Point pt = (Point)(_graph.Nodes[c] as GraphNode).GetData(_GridPos);
				ScrambleGridStep(newGrid, maxShift, iter, pt.X, pt.Y);
			}

			return newGrid;
		}

		Size CalcLinkDir(GraphNode node)
		{
			int xEval = 0, yEval = 0;
			Point gridPos1 = (Point)node.GetData(_GridPos);

			foreach (GraphLink link in node.OutLinks)
			{
				Point gridPos2 = (Point)link.Destination.GetData(_GridPos);

				xEval += gridPos2.X - gridPos1.X;
				yEval += gridPos2.Y - gridPos1.Y;
			}

			foreach (GraphLink link in node.InLinks)
			{
				Point gridPos2 = (Point)link.Origin.GetData(_GridPos);

				xEval += gridPos2.X - gridPos1.X;
				yEval += gridPos2.Y - gridPos1.Y;
			}

			if (xEval > 0) xEval = 1;
			if (xEval < 0) xEval = -1;
			if (yEval > 0) yEval = 1;
			if (yEval < 0) yEval = -1;

			return new Size(xEval, yEval);
		}

		private void ScrambleGridStep(GraphNode[,] newGrid,
			int maxShift, int iter, int i, int j)
		{
			GraphNode node1 = newGrid[1 + i, 1 + j];
			if (node1 == null) return;

			int col = i;
			int row = j;

			// Choose a new position
			Size dir = CalcLinkDir(node1);

			int newCol = maxShift > 1 ?
				col + dir.Width * _random.Next(0, maxShift + 1) + _random.Next(-1, 1 + 1) :
				col + dir.Width * _random.Next(0, 1 + 1);
			int newRow = maxShift > 1 ?
				row + dir.Height * _random.Next(0, maxShift + 1) + _random.Next(-1, 1 + 1) :
				row + dir.Height * _random.Next(0, 1 + 1);

			if (newCol < 0) newCol = 0;
			if (newCol >= _gridWidth) newCol = _gridWidth - 1;
			if (newRow < 0) newRow = 0;
			if (newRow >= _gridHeight) newRow = _gridHeight - 1;

			// Swap node1 with what's at the new pos
			GraphNode node2 = newGrid[1 + newCol, 1 + newRow];

			// Start & end nodes are static
			INode startNode = _info.StartNode;
			INode endNode = _info.EndNode;

			if (startNode != null)
			{
				if (node1.Node == startNode ||
					(node2 != null && node2.Node == startNode))
					return;
			}

			if (endNode != null)
			{
				if (node1.Node == endNode ||
					(node2 != null && node2.Node == endNode))
					return;
			}

			newGrid[1 + newCol, 1 + newRow] = node1;
			newGrid[1 + i, 1 + j] = node2;

			node1.SetData(_GridPos, new Point(newCol, newRow));
			if (node2 != null)
				node2.SetData(_GridPos, new Point(i, j));
		}

		private float EvaluateGrid(GraphNode[,] grid, int maxShift)
		{
			ApplyGrid(grid);
			float eval = 0;

			// Total arrow length; shorter arrow mean nodes are closer
			// so we would like that sum to be minimal
			foreach (GraphLink link in _graph.Links)
			{
				Point gridPos1 = (Point)link.Origin.GetData(_GridPos);
				Point gridPos2 = (Point)link.Destination.GetData(_GridPos);

				int dx = Math.Abs(gridPos1.X - gridPos2.X);
				int dy = Math.Abs(gridPos1.Y - gridPos2.Y);
				if (dx + dy > 1)
					eval += (dx * dx + dy * dy);
			}

			// Nodeta koito ne sa svurzani da im davame poveche tochki ako
			// sa blizki (znachi ne ni kefjat; zelta e da sa po-malko tochkite)
			foreach (GraphNode node in _graph.Nodes)
			{
				Point gridPos = (Point)node.GetData(_GridPos);
				int col = gridPos.X;
				int row = gridPos.Y;
				float toadd = 0;

				for (int k = -1; k <= +1; k++)
				{
					for (int l = -1; l <= +1; l++)
					{
						if (k == 0 && l == 0) continue;
						if ((k + l) % 2 == 0) continue;
						int neighborCol = col + k;
						int neighborRow = row + l;
						GraphNode neighbor = grid[1 + neighborCol, 1 + neighborRow];
						if (neighbor == null) continue;
						if (!NodesLinked(node, neighbor))
							toadd += 1.0f;
					}
				}

				eval += toadd;
			}

			return eval;
		}

		private bool NodesLinked(GraphNode n1, GraphNode n2)
		{
			foreach (GraphLink link in n1.OutLinks)
				if (link.Destination == n2)
					return true;

			return false;
		}

		private void ApplyGrid(GraphNode[,] grid)
		{
			for (int i = 0; i < _gridWidth; ++i)
			{
				for (int j = 0; j < _gridHeight; ++j)
				{
					GraphNode node = grid[1 + i, 1 + j];
					if (node != null)
						node.SetData(_GridPos, new Point(i, j));
				}
			}
		}

		private void PlaceObjects(GraphNode[,] grid)
		{
			for (int i = 0; i < _gridWidth; ++i)
			{
				for (int j = 0; j < _gridHeight; ++j)
				{
					GraphNode node = grid[1 + i, 1 + j];
					if (node != null)
					{
						node.Node.Bounds = 
							new RectangleF(_info.XGap + i * _info.GridSize + _info.GridSize / 2 - node.Node.Bounds.Width / 2,
								_info.YGap + j * _info.GridSize + _info.GridSize / 2 - node.Node.Bounds.Height / 2,
								node.Node.Bounds.Width, node.Node.Bounds.Height);
					}
				}
			}
		}


		private Random _random;
		private Graph _graph;
		private GridLayoutInfo _info;
		private ArrayList _backbone;

		private int _gridWidth;
		private int _gridHeight;
	}
}
