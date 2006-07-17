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
			_lam1 = 10000;
			_lam2 = 80000;
			_lam3 = 0.02f;
			_lam4 = 2000;
			_lam5 = 150;

			_temperature = 40f;
			_temperatureScale = 0.75f;
			_iterationsPerStage = 30;
			_stages = 10;
			_fineTuningMoveValue = 0.4f;
		}


		public float Lam1
		{
			get { return _lam1; }
			set { _lam1 = value; }
		}

		public float Lam2
		{
			get { return _lam2; }
			set { _lam2 = value; }
		}

		public float Lam3
		{
			get { return _lam3; }
			set { _lam3 = value; }
		}

		public float Lam4
		{
			get { return _lam4; }
			set { _lam4 = value; }
		}

		public float Lam5
		{
			get { return _lam5; }
			set { _lam5 = value; }
		}

		public float Temperature
		{
			get { return _temperature; }
			set { _temperature = value; }
		}

		public float TemperatureScale
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

		public float FineTuningMoveValue
		{
			get { return _fineTuningMoveValue; }
			set { _fineTuningMoveValue = value; }
		}


		private float _lam1;
		private float _lam2;
		private float _lam3;
		private float _lam4;
		private float _lam5;

		private float _temperature;
		private float _temperatureScale;
		private float _fineTuningMoveValue;
		int _iterationsPerStage;
		int _stages;
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
			_drawingArea = graph.DocRect;

//			// Arrange the nodes in the graph in a virtual grid
//			int gridSize = (int)Math.Ceiling(Math.Sqrt(_tempGraph.Nodes.Count));
//
//			float offx = float.MaxValue;
//			float offy = float.MaxValue;
//
//			foreach (GraphNode node in _tempGraph.Nodes)
//			{
//				offx = Math.Min(node.Bounds.X, offx);
//				offy = Math.Min(node.Bounds.Y, offy);
//			}
//
//			for (int i = 0; i < _tempGraph.Nodes.Count; i++)
//			{
//				int x = i / gridSize;
//				int y = i % gridSize;
//
//				RectangleF bounds = (_tempGraph.Nodes[i] as GraphNode).Bounds;
//
//				bounds.X = offx + x * 0;//_drawingArea.Width / gridSize;
//				bounds.Y = offy + y * 0;//_drawingArea.Height / gridSize;
//
//				(_tempGraph.Nodes[i] as GraphNode).Bounds = bounds;
//			}

			if (progress != null)
				progress(0, _info.Stages + 2);

			SetGraphElements();
			SetInitStep();
			CalculateInitialState();

			_temperature = _info.Temperature;
				
			float beforeIterCost;
			float stopIterCond;
			float newCostValue;
			float random;
			float currentCostValue = CostFunction();
			Random rand = new Random();

			float realIter = 0;

			for (int stage = 0; stage < _info.Stages; stage++)
			{	
				for (int iter = 0; iter < _info.IterationsPerStage * _nodes.Length; iter++)
				{
					beforeIterCost = currentCostValue;

					for (int n = 0; n < _tempGraph.Nodes.Count; n++ )
					{
						foreach (int i in Enum.GetValues(typeof(ShiftTo)))
						{
							ShiftNode(_nodes[n], (ShiftTo)i, false);

							newCostValue = CostFunction(_nodes[n]);

							realIter++;
							random = (float)rand.NextDouble();

							if ((!_quit) && (newCostValue < currentCostValue || (Math.Pow(Math.E, (currentCostValue - newCostValue) / _temperature) > random)))
							{
								currentCostValue = newCostValue;
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

					stopIterCond = beforeIterCost / currentCostValue;
					beforeIterCost = currentCostValue;

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

			_moveValue = (maxLength / 2f > 5.5f) ? maxLength / 2f : 5.5f;
			_moveScale = 0.1f * _moveValue;
		}

		private void SetGraphElements()
		{
			_nodes  = new GraphNode[_tempGraph.Nodes.Count];
			_links  = new GraphLink[_tempGraph.Links.Count];
			
			for (int i = 0; i < _tempGraph.Nodes.Count; i++)
				_nodes[i] = _tempGraph.Nodes[i] as GraphNode;
			for (int i = 0; i < _tempGraph.Links.Count; i++)
				_links[i] = _tempGraph.Links[i] as GraphLink;

			for (int i = 0; i < _links.Length; i++)
			{
				ArrayList points = new ArrayList();

				points.Add(_links[i].Origin.Center);
				points.Add(_links[i].Destination.Center);

				_links[i].Link.SetPoints(points);
			}

			_sharedNode = new bool[_links.Length, _links.Length];
			for (int i = 0; i < _links.Length; i++)
			{
				for (int j = 0; j < _links.Length; j++)
				{
					_sharedNode[i, j] = (i == j) ||
						_links[i].Origin.Center == _links[j].Origin.Center ||
						_links[i].Origin.Center == _links[j].Destination.Center ||
						_links[i].Destination.Center == _links[j].Destination.Center ||
						_links[i].Destination.Center == _links[j].Origin.Center;
					
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
			float tmp = 0.0f;
			for (int i = 0; i < _nodes.Length; i++)
			{
				tmp = CalcNodeBordLines(_nodes[i]);
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

		private float DistanceBetweenNodes(GraphNode node1, GraphNode node2)
		{
			return DistanceBetweenPoints(node1.Center, node2.Center);
		}

		private float DistanceBetweenPoints(PointF p1, PointF p2)
		{
			return (float)Math.Sqrt((p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) * (p1.Y - p2.Y));
		}

		private float LinkLength(GraphLink link)
		{
			return DistanceBetweenPoints(link.Origin.Center, link.Destination.Center);
		}

		private float CostFunction(GraphNode node)
		{
			CalcNewGlobalCost(node);

			return NodeDistribution() + _glBordLinesCost + EdgeLengths() + EdgeCrossings();
		}

		private float CostFunction()
		{
			return NodeDistribution() + BorderLines() + EdgeLengths() + EdgeCrossings(); 
		}

		private float RichCostFunction(GraphNode node)
		{
			return CostFunction(node) + NodeEdgeDistance2();
		}

		private float RichCostFunction()
		{
			return CostFunction() + NodeEdgeDistance2();
		}

		private float NodeEdgeDistance2()
		{
			float sum = 0.0f;

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

			return _info.Lam5 * sum;
		}

		private float NodeDistribution()
		{
			float sum = 0.0f;
			float distance;
	
			for (int i = 0; i < _nodes.Length; i++)
			{
				for (int j = i; j < _nodes.Length; j++)
				{
					distance = (float)Math.Pow(DistanceBetweenNodes(_nodes[i], _nodes[j]), 2);
					if (distance != 0)
						sum += _info.Lam1 / distance;
				}
			}

			return sum;
		}

		private float BorderLines()
		{
			float sum = 0.0f, r, l, t, b;
			for (int i = 0; i < _nodes.Length; i++)
			{
				r = _drawingArea.Width - _nodes[i].Bounds.Right;
				l = _nodes[i].Bounds.X;
				t = _nodes[i].Bounds.Y;
				b = _drawingArea.Height - _nodes[i].Bounds.Bottom;

				if (r < 0 || t < 0 || l < 0 || b < 0)
					_quit = true;

				sum += 1 / (r * r) + 1 / (l * l) + 1 / (t * t) + 1 / (b * b);
			}

			return _info.Lam2 * sum; 
		}

		private float EdgeLengths()
		{
			float sum = 0.0f;
			for (int i = 0; i < _links.Length; i++)
				sum += (float)Math.Pow(LinkLength(_links[i]), 2);

			return _info.Lam3 * sum;
		}

		private float EdgeCrossings()
		{
			return _info.Lam4 * EdgeCrossingCount2();
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

				for (int j = i; j < _tempGraph.Links.Count; j++)
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

		private float CalcNodeBordLines(GraphNode node)
		{
			float r, l, t, b;

			r = _drawingArea.Width - node.Bounds.Right;
			l = node.Bounds.X;
			t = node.Bounds.Y;
			b = _drawingArea.Height - node.Bounds.Bottom;

			if (r < 0 || t < 0 || l < 0 || b < 0)
				_quit = true;
			else
				_quit = false;

			return _info.Lam2 * (1 / (r * r) + 1 / (l * l) + 1 / (t * t) + 1 / (b * b)); 
		}

		private void CalcNewGlobalCost(GraphNode node)
		{
			_bordNewLocalValue = CalcNodeBordLines(node);
			_glBordLinesCost = _glBordLinesCost - (float)node.GetData(BordLines) + _bordNewLocalValue;
		}

		private void CommitMove(GraphNode node)
		{
			node.SetData(BordLines, _bordNewLocalValue);
		}

		private void RollbackMove(GraphNode node)
		{
			_glBordLinesCost = _glBordLinesCost + (float)node.GetData(BordLines) - _bordNewLocalValue;
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
			float currentCostValue = RichCostFunction();
			float newCostValue;
		
			_moveValue = _info.FineTuningMoveValue;

			for (int j = 0; j < 3; j++)
			{
				foreach (GraphNode node in _tempGraph.Nodes)
				{
					foreach (int i in Enum.GetValues(typeof(ShiftTo)))
					{
						ShiftNode(node, (ShiftTo)i, false);

						newCostValue = RichCostFunction(node);

						if (newCostValue < currentCostValue)
						{
							currentCostValue = newCostValue;
							break;
						}
						else
						{
							ShiftNode(node, (ShiftTo)i, true);
						}
					}
				}
			}
		}


		private Graph _tempGraph;
		private RectangleF _drawingArea;
		private AnnealLayoutInfo _info;

		private float _bordNewLocalValue = 0;
		private float _moveValue;
		private float _temperature;
		private float _glBordLinesCost;
		private float _moveScale;

		private bool _quit;
		private bool[,] _sharedNode;
		private bool[,] _nodeLink;

		private GraphNode[] _nodes;
		private GraphLink[] _links;

		private const int BordLines = 1;
	}
}
