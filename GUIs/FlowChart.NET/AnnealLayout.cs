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
	[Access(AccessType.Private)]
	internal class AnnealLayoutInfo
	{
		public AnnealLayoutInfo()
		{
			distributionFactor = 40000;
			boundaryFactor = 3000;
			edgeLengthFactor = 0.25;
			crossingEdgesCost = 100000;
			nodeEdgeDistFactor = 20000;

			_temperature = 40f;
			_temperatureScale = 0.75f;
			_iterationsPerStage = 50;
			_stages = 15;

			layoutArea = RectangleF.Empty;
			widthHeightRatio = 1;
			randomize = true;
		}


		public double DistributionFactor
		{
			get { return distributionFactor; }
			set { distributionFactor = value; }
		}

		public double BoundaryFactor
		{
			get { return boundaryFactor; }
			set { boundaryFactor = value; }
		}

		public double EdgeLengthFactor
		{
			get { return edgeLengthFactor; }
			set { edgeLengthFactor = value; }
		}

		public double CrossingEdgesCost
		{
			get { return crossingEdgesCost; }
			set { crossingEdgesCost = value; }
		}

		public double NodeEdgeDistFactor
		{
			get { return nodeEdgeDistFactor; }
			set { nodeEdgeDistFactor = value; }
		}

		public double Temperature
		{
			get { return _temperature; }
			set { _temperature = value; }
		}

		public double TemperatureScale
		{
			get { return _temperatureScale; }
			set { _temperatureScale = value; }
		}

		public int IterationsPerStage
		{
			get { return _iterationsPerStage; }
			set { _iterationsPerStage = value; }
		}

		public int Stages
		{
			get { return _stages; }
			set { _stages = value; }
		}

		public RectangleF LayoutArea
		{
			get { return layoutArea; }
			set { layoutArea = value; }
		}

		public float WidthHeightRatio
		{
			get { return widthHeightRatio; }
			set { widthHeightRatio = value; }
		}

		public bool Randomize
		{
			get { return randomize; }
			set { randomize = value; }
		}

		private double distributionFactor;
		private double boundaryFactor;
		private double edgeLengthFactor;
		private double crossingEdgesCost;
		private double nodeEdgeDistFactor;

		private double _temperature;
		private double _temperatureScale;
		int _iterationsPerStage;
		int _stages;

		private RectangleF layoutArea;
		private float widthHeightRatio;
		private bool randomize;
	}


	[Access(AccessType.Private)]
	internal class AnnealLayout
	{
		private enum ShiftTo
		{
			Up,
			Down,
			Left,
			Right,
			UpLeft,
			DownRight,
			UpRight,
			DownLeft  
		}


		public AnnealLayout()
		{
		}

		public bool Arrange(IGraph graph, AnnealLayoutInfo info)
		{
			return Arrange(graph, info, null);
		}

		public bool Arrange(IGraph graph, AnnealLayoutInfo info, LayoutProgress progress)
		{
			_tempGraph = new Graph(graph);
			_info = info;

			float nsize = 0;
			foreach (INode node in graph.Nodes)
				nsize += node.Bounds.Width;
			averageNodeSize = nsize / graph.Nodes.Count;

			// determine what the graph boundaries should be
			if (info.LayoutArea != RectangleF.Empty)
			{
				_drawingArea = info.LayoutArea;
			}
			else
			{
				double squareSize = averageNodeSize * Math.Ceiling(Math.Sqrt(graph.Nodes.Count));
				double width = squareSize * info.WidthHeightRatio;
				double height = squareSize / info.WidthHeightRatio;
				_drawingArea = new RectangleF(0, 0, (float)width * 4.5f, (float)height * 4.5f);
			}

			// start with random positions
			if (info.Randomize)
			{
				Random rnd = new Random();
				foreach (GraphNode node in _tempGraph.Nodes)
				{
					node.Center = new PointF(
						_drawingArea.Left + (float)rnd.NextDouble() * _drawingArea.Width,
						_drawingArea.Top + (float)rnd.NextDouble() * _drawingArea.Height);
				}
			}

			if (progress != null)
				progress(0, _info.Stages + 2);

			SetGraphElements();
			SetInitStep();
			CalculateInitialState();

			_temperature = _info.Temperature;

			double totalCost = CostFunction();
			double previousCost;
			double stopIterCond;
			Random rand = new Random();

			// cool down for the specified number of annealing stages
			for (int stage = 0; stage < _info.Stages; stage++)
			{
				for (int iter = 0; iter < _info.IterationsPerStage; iter++)
				{
					previousCost = totalCost;

					for (int n = 0; n < _tempGraph.Nodes.Count; n++)
					{
						double oldNodeCost = evalCost(_nodes[n]);
						foreach (int i in Enum.GetValues(typeof(ShiftTo)))
						{
							ShiftNode(_nodes[n], (ShiftTo)i, false);
							if (!nodeInBounds(_nodes[n]))
							{
								ShiftNode(_nodes[n], (ShiftTo)i, true);
								continue;
							}

							double newNodeCost = evalCost(_nodes[n]);
							double newTotalCost = totalCost - oldNodeCost + newNodeCost;

							if (newNodeCost < oldNodeCost ||
								Math.Pow(Math.E, (totalCost - newTotalCost) / _temperature) > rand.NextDouble())
							{
								totalCost = newTotalCost;
								CommitMove(_nodes[n]);
								break;
							}
							else
							{
								ShiftNode(_nodes[n], (ShiftTo)i, true);
								RollbackMove(_nodes[n]);
							}
						}
					} 

					stopIterCond = previousCost / totalCost;
					if (stopIterCond > .98 && stopIterCond <= 1)
						break;
				}

				SetTemperature();
				DecreaseMoveValue();

				if (progress != null)
					progress(stage + 1, _info.Stages + 2);
			}

			FineTuning();

			if (progress != null)
				progress(_info.Stages + 2, _info.Stages + 2);

			foreach (GraphNode node in _tempGraph.Nodes)
				node.Node.Bounds = node.Bounds;

			return true;
		}

		private float StepWhenNoLinks()
		{
			if (_nodes.Length == 0)
				return 0;

			float minX = _nodes[0].Bounds.X;
			float minY = _nodes[0].Bounds.Y;
			float maxX = _nodes[0].Bounds.Right;
			float maxY = _nodes[0].Bounds.Bottom;

			for (int i = 1; i < _nodes.Length; i++)
			{
				if (minX > _nodes[i].Bounds.X)
					minX = _nodes[i].Bounds.X;
				if (minY > _nodes[i].Bounds.Y)
					minY = _nodes[i].Bounds.Y;
				if (maxX < _nodes[i].Bounds.Right)
					maxX = _nodes[i].Bounds.Right;
				if (maxY < _nodes[i].Bounds.Bottom)
					maxY = _nodes[i].Bounds.Bottom;	
			}

			return DistanceBetweenPoints(new PointF(maxX, maxY), new PointF(minX, minY));
		}

		private void SetInitStep()
		{
			float maxLength = 0.0f;

			if (_links.Length == 0)
			{
				maxLength = StepWhenNoLinks();
			}
			else
			{
				for (int i = 0; i < _links.Length; i++)
				{
					float linkLength = LinkLength(_links[i]);
					if (maxLength < linkLength)
						maxLength = linkLength;
				}
			}

			_moveValue = Math.Max(averageNodeSize * 6,
				(maxLength / 2f > 5.5f) ? maxLength / 2f : 5.5f);
			_moveScale = _moveValue / _info.Stages;
		}

		private void SetGraphElements()
		{
			_nodes  = new GraphNode[_tempGraph.Nodes.Count];
			_links  = new GraphLink[_tempGraph.Links.Count];
			
			for (int i = 0; i < _tempGraph.Nodes.Count; i++)
				_nodes[i] = _tempGraph.Nodes[i] as GraphNode;
			for (int i = 0; i < _tempGraph.Links.Count; i++)
				_links[i] = _tempGraph.Links[i] as GraphLink;

			_sharedNode = new bool[_links.Length, _links.Length];
			for (int i = 0; i < _links.Length; i++)
			{
				for (int j = 0; j < _links.Length; j++)
				{
					_sharedNode[i, j] = (i == j) ||
						_links[i].Origin == _links[j].Origin ||
						_links[i].Origin == _links[j].Destination ||
						_links[i].Destination == _links[j].Destination ||
						_links[i].Destination == _links[j].Origin;
					
				}
			}

			_nodeLink = new bool[_nodes.Length, _links.Length];
			for (int i = 0; i < _nodes.Length; i++)
			{
				for (int j = 0; j < _links.Length; j++)
				{
					_nodeLink[i,j] =
						_nodes[i].InLinks.Contains(_links[j]) ||
						_nodes[i].OutLinks.Contains(_links[j]);
				}
			}
		}

		private void CalculateInitialState()
		{
			double tmp = 0.0f;
			for (int i = 0; i < _nodes.Length; i++)
			{
				tmp = evalBoundaryCost(_nodes[i]);
				_nodes[i].SetData(BordLines, tmp) ;
				_glBordLinesCost += tmp;			
			}
		}

		private void ShiftNode(GraphNode node, ShiftTo shiftType, bool opposite)
		{
			float sign = 1.0f; 
			if (opposite)
				sign = -1.0f;
	
			switch (shiftType)
			{

				case ShiftTo.Up:
					node.Bounds = new RectangleF(
						node.Bounds.X,
						node.Bounds.Y - sign * _moveValue,
						node.Bounds.Width,
						node.Bounds.Height);
					break;

				case ShiftTo.Down:
					node.Bounds = new RectangleF(
						node.Bounds.X,
						node.Bounds.Y + sign * _moveValue,
						node.Bounds.Width,
						node.Bounds.Height);
					break;

				case ShiftTo.Left:
					node.Bounds = new RectangleF(
						node.Bounds.X - sign * _moveValue,
						node.Bounds.Y,
						node.Bounds.Width,
						node.Bounds.Height);
					break;

				case ShiftTo.Right:
					node.Bounds = new RectangleF(
						node.Bounds.X + sign * _moveValue,
						node.Bounds.Y,
						node.Bounds.Width,
						node.Bounds.Height);
					break;

				case ShiftTo.UpLeft:
					node.Bounds = new RectangleF(
						node.Bounds.X - sign * _moveValue,
						node.Bounds.Y - sign * _moveValue,
						node.Bounds.Width,
						node.Bounds.Height);
					break;

				case ShiftTo.UpRight:
					node.Bounds = new RectangleF(
						node.Bounds.X + sign * _moveValue,
						node.Bounds.Y - sign * _moveValue,
						node.Bounds.Width,
						node.Bounds.Height);
					break;

				case ShiftTo.DownLeft:
					node.Bounds = new RectangleF(
						node.Bounds.X - sign * _moveValue,
						node.Bounds.Y + sign * _moveValue,
						node.Bounds.Width,
						node.Bounds.Height);
					break;

				case ShiftTo.DownRight:
					node.Bounds = new RectangleF(
						node.Bounds.X + sign * _moveValue,
						node.Bounds.Y + sign * _moveValue,
						node.Bounds.Width,
						node.Bounds.Height);
					break;

			}
		}

		private double squareDistance(GraphNode node1, GraphNode node2)
		{
			PointF p1 = node1.Center;
			PointF p2 = node2.Center;
			return Math.Max(0, (p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) * (p1.Y - p2.Y)
				- Math.Pow(lengthFix(node1, node2), 2));
		}

		private float DistanceBetweenPoints(PointF p1, PointF p2)
		{
			return (float)Math.Sqrt((p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) * (p1.Y - p2.Y));
		}

		private float LinkLength(GraphLink link)
		{
			return DistanceBetweenPoints(link.Origin.Center, link.Destination.Center);
		}

		private bool nodeInBounds(GraphNode node)
		{
			double l, t, r, b;

			l = node.Center.X - _drawingArea.Left;
			t = node.Center.Y - _drawingArea.Top;
			r = _drawingArea.Right - node.Center.X;
			b = _drawingArea.Bottom - node.Center.Y;

			return !(r < 0 || t < 0 || l < 0 || b < 0);
		}

		private double evalDistributionCost(GraphNode node)
		{
			double sum = 0.0f;
	
			for (int i = 0; i < _nodes.Length; i++)
			{
				if (node == _nodes[i])
					continue;

				double distance = squareDistance(_nodes[i], node);
				if (distance == 0)
					distance = 0.00001f;
				sum += _info.DistributionFactor / distance;
			}

			return sum;
		}

		private double evalBoundaryCost(GraphNode node)
		{
			double l, t, r, b;

			l = node.Center.X - _drawingArea.Left;
			t = node.Center.Y - _drawingArea.Top;
			r = _drawingArea.Right - node.Center.X;
			b = _drawingArea.Bottom - node.Center.Y;

			return _info.BoundaryFactor * (1 / (r * r) + 1 / (l * l) + 1 / (t * t) + 1 / (b * b)); 
		}

		private double evalEdgeLengthsCost(GraphNode node)
		{
			double sum = 0.0f;

			foreach (GraphLink link in node.InLinks)
				if (link.Origin != link.Destination)
					sum += squareDistance(link.Origin, link.Destination);

			foreach (GraphLink link in node.OutLinks)
				if (link.Origin != link.Destination)
					sum += squareDistance(link.Origin, link.Destination);

			return _info.EdgeLengthFactor * sum;
		}

		private double evalEdgeCrossingsCost(GraphNode node)
		{
			int crossingCount = 0;

			foreach (GraphLink link in node.InLinks)
				if (link.Origin != link.Destination)
					crossingCount += countCrossings(link);

			foreach (GraphLink link in node.OutLinks)
				if (link.Origin != link.Destination)
					crossingCount += countCrossings(link);
			
			return _info.CrossingEdgesCost * crossingCount;
		}

		private int countCrossings(GraphLink link)
		{
			float k1, k2, cpx, cpy;
			PointF crossingPoint;
			int crossingCount = 0;

			PointF o1 = link.Origin.Center;
			PointF d1 = link.Destination.Center;

			for (int i = 0; i < _links.GetLength(0); i++)
			{
				if (link.Origin == _links[i].Origin) continue;
				if (link.Origin == _links[i].Destination) continue;
				if (link.Destination == _links[i].Origin) continue;
				if (link.Destination == _links[i].Destination) continue;

				PointF o2 = _links[i].Origin.Center;
				PointF d2 = _links[i].Destination.Center;

				if ((o1.X == d1.X) && (o2.X == d2.X))
					continue;

				if ((o1.Y == d1.Y) && (o2.Y == d2.Y))
					continue;

				if (o1.X == d1.X)
				{
					cpx = d1.X;
					k2 = (o2.Y - d2.Y) / (o2.X - d2.X);
					cpy = o2.Y - k2 * (o2.X - cpx);
				}
				else if (o2.X == d2.X)
				{
					cpx = o2.X;
					k1 = (o1.Y - d1.Y) / (o1.X - d1.X);
					cpy = o1.Y - k1 * (o1.X - cpx);
				}
				else
				{
					k1 = (o1.Y - d1.Y) / (o1.X - d1.X);
					k2 = (o2.Y - d2.Y) / (o2.X - d2.X);
					if (k1 == k2) continue;
					
					cpx = (o2.Y + k1 * o1.X - o1.Y - k2 * o2.X) / (k1 - k2);
					cpy = -k2 * (o2.X - cpx) + o2.Y;
				}
				
				crossingPoint = new PointF(cpx, cpy);

				if (Math.Abs(LinkLength(link) - (DistanceBetweenPoints(crossingPoint, o1) +  DistanceBetweenPoints(crossingPoint, d1))) <= .00001 && 
					Math.Abs(LinkLength(_links[i]) - (DistanceBetweenPoints(crossingPoint, o2) +  DistanceBetweenPoints(crossingPoint, d2))) <= .00001)
				{
					crossingCount++;
				}
			}

			return crossingCount;
		}

		private double evalCost(GraphNode node)
		{
			return
				evalDistributionCost(node) +
				evalBoundaryCost(node) +
				evalEdgeLengthsCost(node) +
				evalEdgeCrossingsCost(node);
		}

		private double evalTuningCost(GraphNode node)
		{
			return evalCost(node) + nodeEdgeDistance();
		}

		private double CostFunction(GraphNode node)
		{
			CalcNewGlobalCost(node);

			return NodeDistribution() + _glBordLinesCost + EdgeLengths() + EdgeCrossings();
		}

		private double CostFunction()
		{
			return NodeDistribution() + BorderLines() + EdgeLengths() + EdgeCrossings(); 
		}

		private double RichCostFunction(GraphNode node)
		{
			return CostFunction(node) + NodeEdgeDistance2();
		}

		private double RichCostFunction()
		{
			return CostFunction() + NodeEdgeDistance2();
		}

		private double NodeEdgeDistance2()
		{
			double sum = 0.0f;

			float k1, cpx, cpy, distance, tmp;

			for (int i = 0; i < _nodes.Length; i++)
			{
				for (int k = 0; k < _links.Length; k++)
				{
					PointF lp1 = _links[k].Origin.Center;
					PointF lp2 = _links[k].Destination.Center;

					if (_nodeLink[i, k] || lp1.X == lp2.X)
					{
						tmp = 1;
					}
					else
					{
						k1 = (lp1.Y - lp2.Y) / (lp1.X - lp2.X);

						cpx = lp2.X + k1 * (lp1.X - lp2.X);
						cpy = lp2.Y + k1 * (lp1.Y - lp2.Y);

						PointF cp = new PointF(cpx, cpy);

						distance = DistanceBetweenPoints(cp, _nodes[i].Center);

						bool cpIsOnLink = 
							Math.Abs(LinkLength(_links[k]) - (DistanceBetweenPoints(cp, lp1) +  DistanceBetweenPoints(cp, lp2))) <= .00001;

						if (cpIsOnLink)
						{
							if (distance != 0)
								tmp = distance;
							else
								tmp = .001f;
						}
						else
						{
							tmp = Math.Min(DistanceBetweenPoints(lp1, cp), DistanceBetweenPoints(lp2, cp));
						}
					}

					sum += 1 / (tmp * tmp);
				}
			}

			return _info.NodeEdgeDistFactor * sum;
		}

		private double nodeEdgeDistance()
		{
			double sum = 0.0;
			float k1, cpx, cpy, distance, tmp;

			for (int i = 0; i < _nodes.Length; i++)
			{
				GraphNode node = _nodes[i];
				for (int k = 0; k < _links.Length; k++)
				{
					if (_links[k].Origin == node || _links[k].Destination == node)
						continue;

					PointF lp1 = _links[k].Origin.Center;
					PointF lp2 = _links[k].Destination.Center;

					float nodeRad = lengthFix(node);
					if (lp1.X == lp2.X)
					{
						tmp = Math.Abs(node.Center.X - lp1.X) - nodeRad;
					}
					else
					{
						k1 = (lp1.Y - lp2.Y) / (lp1.X - lp2.X);

						cpx = lp2.X + k1 * (lp1.X - lp2.X);
						cpy = lp2.Y + k1 * (lp1.Y - lp2.Y);

						PointF cp = new PointF(cpx, cpy);

						distance = DistanceBetweenPoints(cp, node.Center) - nodeRad;

						bool cpIsOnLink = 
							Math.Abs(LinkLength(_links[k]) - (DistanceBetweenPoints(cp, lp1) +  DistanceBetweenPoints(cp, lp2))) <= 0.005;

						if (cpIsOnLink)
						{
							tmp = distance;
						}
						else
						{
							tmp = Math.Min(DistanceBetweenPoints(lp1, cp), DistanceBetweenPoints(lp2, cp)) -
								nodeRad;
						}
					}

					if (tmp <= 0)
						tmp = 0.1f;
					if (tmp < nodeRad * 2)
						sum += 1 / (tmp * tmp);
				}
			}

			return _info.NodeEdgeDistFactor * sum;
		}

		private float lengthFix(GraphLink link)
		{
			return lengthFix(link.Origin, link.Destination);
		}

		private float lengthFix(GraphNode node1, GraphNode node2)
		{
			return
				(node1.Bounds.Width + node1.Bounds.Height) / 4 +
				(node2.Bounds.Width + node2.Bounds.Height) / 4;
		}

		private float lengthFix(GraphNode node)
		{
			return (node.Bounds.Width + node.Bounds.Height) / 4;
		}

		private double NodeDistribution()
		{
			double sum = 0.0f;
			double distance;
	
			for (int i = 0; i < _nodes.Length; i++)
			{
				for (int j = i + 1; j < _nodes.Length; j++)
				{
					distance = squareDistance(_nodes[i], _nodes[j]);
					if (distance == 0)
						distance = 0.00001f;
					sum += _info.DistributionFactor / distance;
				}
			}

			return sum;
		}

		private double BorderLines()
		{
			double sum = 0.0f, r, l, t, b;
			for (int i = 0; i < _nodes.Length; i++)
			{
				l = _nodes[i].Center.X - _drawingArea.Left;
				t = _nodes[i].Center.Y - _drawingArea.Top;
				r = _drawingArea.Right - _nodes[i].Center.X;
				b = _drawingArea.Bottom - _nodes[i].Center.Y;

				sum += 1 / (r * r) + 1 / (l * l) + 1 / (t * t) + 1 / (b * b);
			}

			return _info.BoundaryFactor * sum; 
		}

		private double EdgeLengths()
		{
			double sum = 0.0f;
			for (int i = 0; i < _links.Length; i++)
				sum += squareDistance(_links[i].Origin, _links[i].Destination);

			return _info.EdgeLengthFactor * sum;
		}

		private double EdgeCrossings()
		{
			return _info.CrossingEdgesCost * EdgeCrossingCount2();
		}

		private int EdgeCrossingCount2()
		{
			float k1, k2, cpx, cpy;
			PointF crossingPoint;
			int crossingCount = 0;

			for (int i = 0; i < _tempGraph.Links.Count; i++)
			{
				PointF o1 = _links[i].Origin.Center;
				PointF d1 = _links[i].Destination.Center;

				for (int j = i + 1; j < _tempGraph.Links.Count; j++)
				{
					if (!_sharedNode[i,j])
					{
						PointF o2 = _links[j].Origin.Center;
						PointF d2 = _links[j].Destination.Center;

						if ((o1.X == d1.X) && (o2.X == d2.X))
							continue;

						if ((o1.Y == d1.Y) && (o2.Y == d2.Y))
							continue;

						if (o1.X == d1.X)
						{
							cpx = d1.X;
							k2 = (o2.Y - d2.Y) / (o2.X - d2.X);
							cpy = o2.Y - k2 * (o2.X - cpx);
						}
						else if (o2.X == d2.X)
						{
							cpx = o2.X;
							k1 = (o1.Y - d1.Y) / (o1.X - d1.X);
							cpy = o1.Y - k1 * (o1.X - cpx);
						}
						else
						{
							k1 = (o1.Y - d1.Y) / (o1.X - d1.X);
							k2 = (o2.Y - d2.Y) / (o2.X - d2.X);
							if (k1 == k2) continue;
							
							cpx = (o2.Y + k1 * o1.X - o1.Y - k2 * o2.X) / (k1 - k2);
							cpy = -k2 * (o2.X - cpx) + o2.Y;
						}
						
						crossingPoint = new PointF(cpx, cpy);

						if (Math.Abs(LinkLength(_links[i]) - (DistanceBetweenPoints(crossingPoint, o1) +  DistanceBetweenPoints(crossingPoint, d1))) <= .00001 && 
							Math.Abs(LinkLength(_links[j]) - (DistanceBetweenPoints(crossingPoint, o2) +  DistanceBetweenPoints(crossingPoint, d2))) <= .00001)
						{
							crossingCount++;
						}
					}
				}
			}

			return crossingCount;
		}

		private void CalcNewGlobalCost(GraphNode node)
		{
			_bordNewLocalValue = evalBoundaryCost(node);
			_glBordLinesCost = _glBordLinesCost - (double)node.GetData(BordLines) + _bordNewLocalValue;
		}

		private void CommitMove(GraphNode node)
		{
			node.SetData(BordLines, _bordNewLocalValue);
		}

		private void RollbackMove(GraphNode node)
		{
			_glBordLinesCost = _glBordLinesCost + (double)node.GetData(BordLines) - _bordNewLocalValue;
		}

		private void SetTemperature()
		{
			if (_temperature > 0.5) _temperature *= _info.TemperatureScale;
		}

		private void DecreaseMoveValue()
		{
			if (_moveValue > 0.5) _moveValue -= _moveScale;
		}

		private void FineTuning()
		{
			for (int j = 0; j < _info.IterationsPerStage / 8; j++)
			{
				foreach (GraphNode node in _tempGraph.Nodes)
				{
					_moveValue = node.Bounds.Width * (float)Math.Pow(0.8, j);
					double oldNodeCost = evalTuningCost(node);
					double minCost = oldNodeCost;
					PointF minPos = node.Center;

					foreach (int i in Enum.GetValues(typeof(ShiftTo)))
					{
						ShiftNode(node, (ShiftTo)i, false);
						double newNodeCost = evalTuningCost(node);

						if (newNodeCost < minCost)
						{
							minCost = newNodeCost;
							minPos = node.Center;
						}

						ShiftNode(node, (ShiftTo)i, true);
					}

					if (minCost < oldNodeCost)
						node.Center = minPos;
				}
			}
		}


		private Graph _tempGraph;
		private RectangleF _drawingArea;
		private AnnealLayoutInfo _info;

		private double _bordNewLocalValue = 0;
		private float _moveValue;
		private double _temperature;
		private double _glBordLinesCost;
		private float _moveScale;

		private bool[,] _sharedNode;
		private bool[,] _nodeLink;

		private GraphNode[] _nodes;
		private GraphLink[] _links;

		private const int BordLines = 1;
		private float averageNodeSize;
	}
}