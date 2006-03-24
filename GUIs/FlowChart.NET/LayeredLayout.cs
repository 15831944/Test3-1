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
	// Helper classes

	#region comparers
	/// <summary>
	/// Compares nodes by priority.
	/// </summary>
	[Access(AccessType.Internal)]
	internal class NodePriorityComparer : IComparer
	{
		public NodePriorityComparer()
		{
		}

		public int Compare(object x, object y)
		{
			GraphNode n1 = x as GraphNode;
			GraphNode n2 = y as GraphNode;

			double n1Priority = ((double)n1.GetData(LayeredLayout._UPriority) + 
				(double)n1.GetData(LayeredLayout._DPriority)) / 2;
			double n2Priority = ((double)n2.GetData(LayeredLayout._UPriority) + 
				(double)n2.GetData(LayeredLayout._DPriority)) / 2;

			if (Math.Abs(n1Priority - n2Priority) < double.Epsilon)
				return 0;
			if (n1Priority < n2Priority)
				return 1;
			return -1;
		}
	}

	/// <summary>
	/// Compares nodes by barycenters.
	/// </summary>
	[Access(AccessType.Internal)]
	internal class NodeComparer : IComparer
	{
		public NodeComparer(ArrayList layer)
		{
			_layer = layer;
		}

		public int Compare(object x, object y)
		{
			GraphNode n1 = x as GraphNode;
			GraphNode n2 = y as GraphNode;

			double n1BaryCenter = LayeredLayout.CalcBaryCenter(n1);
			double n2BaryCenter = LayeredLayout.CalcBaryCenter(n2);

			if (Math.Abs(n1BaryCenter - n2BaryCenter) < double.Epsilon)
			{
				// In case of coinciding barycenters compare by
				// the count of in/out links
				if (n1.LinkCount == n2.LinkCount)
					return 0;
				else if (n1.LinkCount < n2.LinkCount)
					return 1;
				return -1;
			}

			double compareValue =
				(n2BaryCenter - n1BaryCenter) * 1000;

			if (compareValue > 0)
				return -1;
			else if (compareValue < 0)
				return 1;
			return 0;
		}


		private ArrayList _layer;
	}
	#endregion


	#region enumerations
	[Access(AccessType.Public)]
	internal enum Orientation
	{
		Vertical,
		Horizontal
	}

	[Access(AccessType.Public)]
	internal enum Direction
	{
		Straight,
		Reversed
	}
	#endregion

	#region class LayeredLayoutInfo
	[Access(AccessType.Public)]
	internal class LayeredLayoutInfo
	{
		public LayeredLayoutInfo()
		{
			_orientation = Orientation.Vertical;
			_direction = Direction.Straight;
			_layerDistance = 25;
			_nodeDistance = 20;
			_xGap = 5;
			_yGap = 5;
			_splitLayers = false;
			_compactFactor = 1;
			_timeLimit = 10000;
		}

		public LayeredLayoutInfo(Orientation orientation,
			float layerDistance, float nodeDistance,
			float xGap, float yGap)
		{
			_orientation = orientation;
			_direction = Direction.Straight;
			_layerDistance = layerDistance;
			_nodeDistance = nodeDistance;
			_xGap = xGap;
			_yGap = yGap;
			_splitLayers = false;
			_compactFactor = 1;
			_timeLimit = 10000;
		}


		public Orientation Orientation
		{
			get { return _orientation; }
			set { _orientation = value; }
		}

		public Direction Direction
		{
			get { return _direction; }
			set { _direction = value; }
		}

		public float LayerDistance
		{
			get { return _layerDistance; }
			set { _layerDistance = value; }
		}

		public float NodeDistance
		{
			get { return _nodeDistance; }
			set { _nodeDistance = value; }
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

		public bool SplitLayers
		{
			get { return _splitLayers; }
			set { _splitLayers = value; }
		}

		public float ArrowsCompactFactor
		{
			get { return _compactFactor; }
			set { _compactFactor = value; }
		}

		public long TimeLimit
		{
			get { return _timeLimit; }
			set { _timeLimit = value; }
		}


		private Orientation _orientation;
		private Direction _direction;
		private float _layerDistance;
		private float _nodeDistance;
		private float _xGap;
		private float _yGap;
		private bool _splitLayers;
		private float _compactFactor;
		private long _timeLimit;
	}
	#endregion

	#region class LayeredLayout
	/// <summary>
	/// Performs layered layout on a graph.
	/// </summary>
	[Access(AccessType.Public)]
	internal class LayeredLayout
	{
		public LayeredLayout()
		{
			_layers = null;
		}


		// Keys to associate node-specific data
		internal const int _Layer = 1;
		internal const int _UBaryCenter = 2;
		internal const int _DBaryCenter = 3;
		internal const int _UPriority = 4;
		internal const int _DPriority = 5;
		internal const int _ULinkCount = 6;
		internal const int _DLinkCount = 7;
		internal const int _GridPosition = 8;
		internal const int _Dummy = 9;

		// Keys to associate link-specific data
		internal const int _DummificationLevel = 1;
		internal const int _LinkPoints = 2;


		/// <summary>
		/// Calculates the barycenter of a node.
		/// </summary>
		internal static double CalcBaryCenter(GraphNode node)
		{
			int uLinkCount = (int)node.GetData(_ULinkCount);
			int dLinkCount = (int)node.GetData(_DLinkCount);
			double uBaryCenter = (double)node.GetData(_UBaryCenter);
			double dBaryCenter = (double)node.GetData(_DBaryCenter);

			if (uLinkCount > 0 && dLinkCount > 0)
				return (uBaryCenter + dBaryCenter) / 2;
			if (uLinkCount > 0)
				return uBaryCenter;
			if (dLinkCount > 0)
				return dBaryCenter;

			return 0;
		}


		public bool Arrange(IGraph graph, LayeredLayoutInfo info)
		{
			return Arrange(graph, info, null);
		}

		/// <summary>
		/// Performs the arrangement of the diagram.
		/// </summary>
		public bool Arrange(IGraph graph,
			LayeredLayoutInfo info, LayoutProgress progress)
		{
			_graph = new Graph(graph);
			_info = info;
			_progress = progress;
			_current = 0;
			_total = 
				1 /*path finding*/ + 
				1 /*layers splitting*/ +
				1 /*dummify*/ + 
				6 /*minimize crossings*/ +
				4 /*swap pairs*/ +
				1 /*layout*/ +
				1 /*apply*/ +
				1 /*compact*/ + 
				1 /*dedumify*/;

			// Update progress
			if (_progress != null)
				_progress(_current, _total);

			// Initialize nodes
			foreach (GraphNode node in _graph.Nodes)
			{
				node.SetData(_Layer, -1);
				node.SetData(_UBaryCenter, 0.0);
				node.SetData(_DBaryCenter, 0.0);
				node.SetData(_ULinkCount, 0);
				node.SetData(_DLinkCount, 0);
				node.SetData(_UPriority, 0);
				node.SetData(_DPriority, 0);
				node.SetData(_GridPosition, 0);
				node.SetData(_Dummy, false);
			}

			// Initialize links
			foreach (GraphLink link in _graph.Links)
			{
				link.SetData(_DummificationLevel, 0);
			}

			// Determine the layer depth
			Path longestPath = PathFinder.FindLongestPath(graph, info.TimeLimit);
			if (longestPath == null)
				return true; // No objects in the graph

			// Update progress
			if (_progress != null)
				_progress(_current++, _total);

			_layers = new ArrayList();
			for (int i = 0; i < longestPath.Nodes.Count; i++)
				_layers.Add(new ArrayList());

			// Distribute nodes to their appropriate layers,
			// starting with the nodes from the longest path
			for (int i = 0; i < longestPath.Nodes.Count; i++)
			{
				// Find the corresponding node in the internal graph
				GraphNode node = null;
				foreach (GraphNode n in _graph.Nodes)
				{
					if (n.Node == longestPath.Nodes[i])
					{
						node = n;
						break;
					}
				}

				node.SetData(_Layer, i);
				(_layers[i] as ArrayList).Add(node);
			}

			foreach (INode node in longestPath.Nodes)
			{
				// Find the corresponding node in the internal graph
				GraphNode gnode = null;
				foreach (GraphNode n in _graph.Nodes)
				{
					if (n.Node == node)
					{
						gnode = n;
						break;
					}
				}

				AddChildren(gnode);
			}

			// For all layers whose nodes are > 1 / 20 of the total ->
			// drop nodes. This will ensure that there are no
			// too wide layers, thus improving visibility of the graph.
			int total = 0;
			bool found;
			foreach (ArrayList layer in _layers)
				total += layer.Count;

			int threshold = total / 10;
			if (total > 50 && _info.SplitLayers)
			{
				found = true;
				while (found)
				{
					found = false;

					foreach (ArrayList layer in _layers)
					{
						if (layer.Count > threshold)
						{
							// Find candidates for dropping.
							// Good candidate is a node which
							// when moved downwards will be closer
							// to its children
							SortedList candidates = new SortedList();
							foreach (GraphNode node in layer)
							{
								// Calculate total child depth
								int factor = 0;
								int nodeLayer = (int)node.GetData(_Layer);
								foreach (GraphLink link in node.OutLinks)
								{
									int childLayer = (int)link.Destination.GetData(_Layer);
									if (childLayer <= nodeLayer)
										factor += 1;
									else
										factor -= 1;
								}
								foreach (GraphLink link in node.InLinks)
								{
									int childLayer = (int)link.Origin.GetData(_Layer);
									if (childLayer <= nodeLayer)
										factor += 1;
									else
										factor -= 1;
								}

								if (factor > 0)
									candidates[factor] = node;
							}

							if (candidates.Keys.Count == 0)
								continue;

							found = true;

							ArrayList nextLayer = null;
							int ilayer = _layers.IndexOf(layer);
							if (ilayer == _layers.Count - 1)
							{
								nextLayer = new ArrayList();
								_layers.Add(nextLayer);
							}
							else
							{
								nextLayer = _layers[ilayer + 1] as ArrayList;
							}

							while (layer.Count > threshold)
							{
								if (candidates.Keys.Count == 0)
									break;

								GraphNode node = candidates.GetByIndex(candidates.Count - 1) as GraphNode;
								candidates.RemoveAt(candidates.Count - 1);

								nextLayer.Add(node);
								layer.Remove(node);
								node.SetData(_Layer, (int)node.GetData(_Layer) + 1);
							}
						}

						if (found)
							break;
					}
				}
			}

			// Check if there are directly related nodes on the same layer
			found = true;
			while (found)
			{
				found = false;

				int ilayer = 0;
				ArrayList nodesToDrop = new ArrayList();
				foreach (ArrayList layer in _layers)
				{
					nodesToDrop.Clear();
					foreach (GraphNode node in layer)
					{
						foreach (GraphLink link in node.OutLinks)
						{
							if ((int)link.Destination.GetData(_Layer) == ilayer)
							{
								// The node's child is on the same layer.
								// Mark it for dropping
								if (!nodesToDrop.Contains(link.Destination))
									nodesToDrop.Add(link.Destination);
								found = true;
							}
						}
					}

					// Drop nodes downwards
					if (found)
					{
						ArrayList curLayer = _layers[ilayer] as ArrayList;
						ArrayList nextLayer = null;
						if (ilayer == _layers.Count - 1)
						{
							nextLayer = new ArrayList();
							_layers.Add(nextLayer);
						}
						else
						{
							nextLayer = _layers[ilayer + 1] as ArrayList;
						}

						foreach (GraphNode node in nodesToDrop)
						{
							if (curLayer.Count > 1)
							{
								nextLayer.Add(node);
								curLayer.Remove(node);
								node.SetData(_Layer, (int)node.GetData(_Layer) + 1);
							}
						}

						break;
					}

					ilayer++;
				}
			}


			// Calculate initial grid positions
			foreach (ArrayList layer in _layers)
				for (int i = 0; i < layer.Count; i++)
					(layer[i] as GraphNode).SetData(_GridPosition, (double)i);

			// Update progress
			if (_progress != null)
				_progress(_current++, _total);

			// Add dummy nodes for all links which cross one or more
			// layers. Add one dummy node for each crossed layer
			Dummify();

			// Reduce crossings
			MinimizeCrossings();

			// Further reduce crossings through pair swap
			SwapPairs();

			// Arrange nodes
			Layout();

			// Update progress
			if (_progress != null)
				_progress(_current++, _total);

			// Compact levels
			if (_info.ArrowsCompactFactor != 1)
				Compact();

			// Update progress
			if (_progress != null)
				_progress(_current++, _total);

			Apply();

			// Update progress
			if (_progress != null)
				_progress(_current++, _total);

			// Reverse dummification
			Dedummify();

			// Update progress
			if (_progress != null)
				_progress(_total, _total);

			// Update nodes positions
			foreach (GraphNode node in _graph.Nodes)
				if (node.Node != null)
					node.Node.Bounds = node.Bounds;

			// Update arrow points
			foreach (GraphLink link in _graph.Links)
			{
				if (link.Link != null)
				{
					object points = link.GetData(_LinkPoints);
					if (points != null)
						link.Link.SetPoints(points as ArrayList);
				}
			}

			return true;
		}


		/// <summary>
		/// Assigns nodes positions using the precalculated
		/// grid positions of the nodes within all layers.
		/// </summary>
		private void Apply()
		{
			float depth;
			if (_info.Orientation == Orientation.Vertical)
				depth = _info.YGap;
			else
				depth = _info.XGap;

			for (int i = (_info.Direction == Direction.Straight ? 0 : _layers.Count - 1);
				_info.Direction == Direction.Straight ? i < _layers.Count : i >= 0;
				i += (_info.Direction == Direction.Straight ? 1 : -1))
			{
				ArrayList layer = _layers[i] as ArrayList;

				// Calculate layer height
				float height = float.MaxValue;
				foreach (GraphNode node in layer)
				{
					if (_info.Orientation == Orientation.Vertical)
						height = Math.Min(height, node.Bounds.Height);
					else
						height = Math.Min(height, node.Bounds.Width);
				}

				double prevGridPosition = 0;
				float offset;
				if (_info.Orientation == Orientation.Vertical)
					offset = _info.XGap;
				else
					offset = _info.YGap;

				foreach (GraphNode node in layer)
				{
					RectangleF bounds = node.Bounds;
					double gridPosition = (double)node.GetData(_GridPosition);

					if (_info.Orientation == Orientation.Vertical)
					{
						offset += //node.Bounds.Width +
							(float)(gridPosition - prevGridPosition) * _info.NodeDistance;
						bounds.Offset(
							-bounds.Left + offset,
							-bounds.Top + depth + (height / 2 - node.Bounds.Height / 2));
					}
					else
					{
						offset += //node.Bounds.Height +
							(float)(gridPosition - prevGridPosition) * _info.NodeDistance;
						bounds.Offset(
							-bounds.Left + depth + (height / 2 - node.Bounds.Height / 2),
							-bounds.Top + offset);
					}

					node.Bounds = bounds;
					prevGridPosition = gridPosition;
				}

				depth += _info.LayerDistance + height;
			}
		}

		/// <summary>
		/// Move nodes to their barycenters, according to their
		/// priority but doing so without any box swapping
		/// (i.e. the boxes' order remains the same).
		/// </summary>
		private void Layout()
		{
			// Initialize node's base priority
			foreach (ArrayList layer in _layers)
			{
				foreach (GraphNode node in layer)
				{
					node.SetData(_UPriority, (double)(int)node.GetData(_ULinkCount));
					node.SetData(_DPriority, (double)(int)node.GetData(_DLinkCount));
				}
			}

			// Layout is invoked after MinimizeCrossings
			// so we may assume node's barycenters are initially correct

			const int maxLayoutIterations = 2;
			for (int it = 0; it < maxLayoutIterations; it++)
			{
				for (int i = _layers.Count - 1; i >= 1; i--)
					LayoutLayer(false, i);

				for (int i = 0; i < _layers.Count - 1; i++)
					LayoutLayer(true, i);
			}


			// Offset the whole structure so that there are no gridPositions < 0
			double gridPos = double.MaxValue;
			foreach (ArrayList layer in _layers)
				foreach (GraphNode node in layer)
					gridPos = Math.Min(gridPos, (double)node.GetData(_GridPosition));

			if (gridPos < 0)
			{
				foreach (ArrayList layer in _layers)
					foreach (GraphNode node in layer)
						node.SetData(_GridPosition,
							(double)node.GetData(_GridPosition) - gridPos);
			}
		}

		private void LayoutLayer(bool down, int layer)
		{
			int iconsidered;
			ArrayList considered;

			if (down)
				considered = _layers[iconsidered = layer + 1] as ArrayList;
			else
				considered = _layers[iconsidered = layer - 1] as ArrayList;

			// List containing the nodes in the considered layer
			// sorted by priority
			ArrayList sorted = considered.Clone() as ArrayList;

			sorted.Sort(new NodePriorityComparer());

			// Each node strives for its barycenter.
			// High priority nodes start first
			foreach (GraphNode node in sorted)
			{
				double nodeGridPos = (double)node.GetData(_GridPosition);
				double nodeBaryCenter = CalcBaryCenter(node);
				double nodePriority = ((double)node.GetData(_UPriority) +
					(double)node.GetData(_DPriority)) / 2;

				if (Math.Abs(nodeGridPos - nodeBaryCenter) < double.Epsilon)
				{
					// This node is exactly at its barycenter -> perfect
					continue;
				}

				if (Math.Abs(nodeGridPos - nodeBaryCenter) < 0.25 + double.Epsilon)
				{
					// This node is close enough to the barycenter -> should work
					continue;
				}

				if (nodeGridPos < nodeBaryCenter)
				{
					// Try to move the node to the right in an
					// attempt to reach its barycenter
					while (nodeGridPos < nodeBaryCenter)
					{
						if (!MoveRight(node, considered, nodePriority))
							break;

						nodeGridPos = (double)node.GetData(_GridPosition);
					}
				}
				else
				{
					// Try to move the node to the left in an
					// attempt to reach its barycenter
					while (nodeGridPos > nodeBaryCenter)
					{
						if (!MoveLeft(node, considered, nodePriority))
							break;

						nodeGridPos = (double)node.GetData(_GridPosition);
					}
				}
			}

			// After the layer has been rearranged we need
			// to recalculate the barycenters
			// of the nodes in the surrounding layers
			if (iconsidered > 0)
				CalcDownData(iconsidered - 1);
			if (iconsidered < _layers.Count - 1)
				CalcUpData(iconsidered + 1);
		}

		private bool MoveRight(GraphNode node, ArrayList layer, double priority)
		{
			int index = layer.IndexOf(node);
			if (index == layer.Count - 1)
			{
				// This is the last node in the layer - 
				// so we can move to the right without troubles
				node.SetData(_GridPosition, (double)node.GetData(_GridPosition) + 0.5);
				return true;
			}

			GraphNode rightNode = (GraphNode)layer[index + 1];

			double rightNodePriority = ((double)rightNode.GetData(_UPriority) +
				(double)rightNode.GetData(_DPriority)) / 2;

			// Check if there is space
			// between the right and the current node
			if ((double)rightNode.GetData(_GridPosition) >
				(double)node.GetData(_GridPosition) + 1)
			{
				node.SetData(_GridPosition, (double)node.GetData(_GridPosition) + 0.5);
				return true;
			}

			// We have reached a node with higher priority - 
			// no movement is allowed
			if (rightNodePriority > priority ||
				Math.Abs(rightNodePriority - priority) < double.Epsilon)
				return false;

			// The right node has lower priority - try to move it
			if (MoveRight(rightNode, layer, priority))
			{
				node.SetData(_GridPosition, (double)node.GetData(_GridPosition) + 0.5);
				return true;
			}

			return false;
		}

		private bool MoveLeft(GraphNode node, ArrayList layer, double priority)
		{
			int index = layer.IndexOf(node);
			if (index == 0)
			{
				// This is the last node in the layer - 
				// so we can move to the left without troubles
				node.SetData(_GridPosition, (double)node.GetData(_GridPosition) - 0.5);
				return true;
			}

			GraphNode leftNode = layer[index - 1] as GraphNode;

			double leftNodePriority = ((double)leftNode.GetData(_UPriority) +
				(double)leftNode.GetData(_DPriority)) / 2;

			// Check if there is space
			// between the left and the current node
			if ((double)leftNode.GetData(_GridPosition) <
				(double)node.GetData(_GridPosition) - 1)
			{
				node.SetData(_GridPosition, (double)node.GetData(_GridPosition) - 0.5);
				return true;
			}

			// We have reached a node with higher priority - 
			// no movement is allowed
			if (leftNodePriority > priority ||
				Math.Abs(leftNodePriority - priority) < double.Epsilon)
				return false;

			// The left node has lower priority - try to move it
			if (MoveLeft(leftNode, layer, priority))
			{
				node.SetData(_GridPosition, (double)node.GetData(_GridPosition) - 0.5);
				return true;
			}

			return false;
		}

		private void AddChildren(GraphNode node)
		{
			ArrayList next = new ArrayList();
			foreach (GraphLink link in node.OutLinks)
			{
				GraphNode child = link.Destination;
				int nodeLayer = (int)node.GetData(_Layer);
				int childLayer = (int)child.GetData(_Layer);
				if (childLayer == -1 && nodeLayer < _layers.Count - 1)
				{
					child.SetData(_Layer, nodeLayer + 1);
					(_layers[nodeLayer + 1] as ArrayList).Add(child);
					next.Add(child);
				}
				else if (childLayer == -1)
				{
					_layers.Add(new ArrayList());
					child.SetData(_Layer, nodeLayer + 1);
					(_layers[nodeLayer] as ArrayList).Add(child);
					next.Add(child);
				}
			}
			foreach (GraphLink link in node.InLinks)
			{
				GraphNode child = link.Origin;
				int nodeLayer = (int)node.GetData(_Layer);
				int childLayer = (int)child.GetData(_Layer);
				if (childLayer == -1 && nodeLayer > 0)
				{
					child.SetData(_Layer, nodeLayer - 1);
					(_layers[nodeLayer - 1] as ArrayList).Add(child);
					next.Add(child);
				}
				else if (childLayer == -1)
				{
					child.SetData(_Layer, 0);
					(_layers[0] as ArrayList).Add(child);
					next.Add(child);
				}
			}

			foreach (GraphNode child in next)
				AddChildren(child);
		}

		private void Dummify()
		{
			ArrayList links = new ArrayList(_graph.Links);
			foreach (GraphLink link in links)
			{
				GraphNode o = link.Origin;
				GraphNode d = link.Destination;

				int oLayer = (int)o.GetData(_Layer);
				int dLayer = (int)d.GetData(_Layer);
				double oPos = (double)o.GetData(_GridPosition);
				double dPos = (double)d.GetData(_GridPosition);

				double step = (dPos - oPos) / Math.Abs(dLayer - oLayer);

				GraphNode p = o;
				if (oLayer - dLayer > 1)
				{
					for (int i = oLayer - 1; i > dLayer; i--)
					{
						GraphNode newNode = new GraphNode(o.Bounds);
						ArrayList layer = _layers[i] as ArrayList;

						double pos = (i - dLayer) * step + oPos;

						if (pos > layer.Count)
							pos = layer.Count;

						// Check if origin and dest are both last
						if (oPos >= (_layers[oLayer] as ArrayList).Count - 1 &&
							dPos >= (_layers[dLayer] as ArrayList).Count - 1)
							pos = layer.Count;

						// Check if origin and destination are both first
						else if (oPos == 0 && dPos == 0)
							pos = 0;

						newNode.SetData(_Layer, i);
						newNode.SetData(_UBaryCenter, 0.0);
						newNode.SetData(_DBaryCenter, 0.0);
						newNode.SetData(_ULinkCount, 0);
						newNode.SetData(_DLinkCount, 0);
						newNode.SetData(_GridPosition, pos);
						newNode.SetData(_Dummy, true);
						layer.Insert((int)pos, newNode);

						// Translate rightwards nodes' positions
						for (int r = (int)pos + 1; r < layer.Count; r++)
						{
							GraphNode node = layer[r] as GraphNode;
							node.SetData(_GridPosition,
								(double)node.GetData(_GridPosition) + 1);
						}

						GraphLink newLink = new GraphLink(p, newNode);
						newLink.SetData(_DummificationLevel, 0);
						p = newNode;

						// Add the new node and the new link to the graph
						_graph.Nodes.Add(newNode);
						_graph.Links.Add(newLink);
					}

					// Set the origin of the real arrow to the last dummy
					link.Origin = p;
					link.SetData(_DummificationLevel, oLayer - dLayer - 1);
				}

				if (oLayer - dLayer < -1)
				{
					for (int i = oLayer + 1; i < dLayer; i++)
					{
						GraphNode newNode = new GraphNode(o.Bounds);
						ArrayList layer = _layers[i] as ArrayList;

						double pos = (i - oLayer) * step + oPos;

						if (pos > layer.Count)
							pos = layer.Count;

						// Check if origin and dest are both last
						if (oPos >= (_layers[oLayer] as ArrayList).Count - 1 &&
							dPos >= (_layers[dLayer] as ArrayList).Count - 1)
							pos = layer.Count;

						// Check if origin and destination are both first
						else if (oPos == 0 && dPos == 0)
							pos = 0;

						newNode.SetData(_Layer, i);
						newNode.SetData(_UBaryCenter, 0.0);
						newNode.SetData(_DBaryCenter, 0.0);
						newNode.SetData(_ULinkCount, 0);
						newNode.SetData(_DLinkCount, 0);
						newNode.SetData(_GridPosition, pos);
						newNode.SetData(_Dummy, true);
						layer.Insert((int)pos, newNode);

						// Translate rightwards nodes' positions
						for (int r = (int)pos + 1; r < layer.Count; r++)
						{
							GraphNode node = layer[r] as GraphNode;
							node.SetData(_GridPosition,
								(double)node.GetData(_GridPosition) + 1);
						}

						GraphLink newLink = new GraphLink(p, newNode);
						newLink.SetData(_DummificationLevel, 0);
						p = newNode;

						// Add the new node and the new link to the graph
						_graph.Nodes.Add(newNode);
						_graph.Links.Add(newLink);
					}

					// Set the origin of the real arrow to the last dummy
					link.Origin = p;
					link.SetData(_DummificationLevel, dLayer - oLayer - 1);
				}
			}
		}

		/// <summary>
		/// Makes the dummy nodes at the beginning and
		/// at the end of each level closer to each other.
		/// </summary>
//		private void Compact()
//		{
//			foreach (ArrayList layer in _layers)
//			{
//				int i = -1;
//				while (true)
//				{
//					i++;
//					if (!(bool)((layer[i] as GraphNode).GetData(_Dummy)))
//						break;
//				}
//
//				double prevGridPos = 0;
//				if (i - 1 >= 0)
//					prevGridPos = (double)(layer[i - 1] as GraphNode).GetData(_GridPosition);
//				i--;
//
//				for (int j = i - 1; j >= 0; j--)
//				{
//					GraphNode node = layer[j] as GraphNode;
//					double gridPos = (double)node.GetData(_GridPosition);
//					gridPos += (1 - _info.ArrowsCompactFactor) * (prevGridPos - gridPos);
//					node.SetData(_GridPosition, gridPos);
//					prevGridPos = gridPos;
//				}
//
//				i = layer.Count;
//				while (true)
//				{
//					i--;
//					if (!(bool)((layer[i] as GraphNode).GetData(_Dummy)))
//						break;
//				}
//
//				prevGridPos = 0;
//				if (i + 1 < layer.Count)
//					prevGridPos = (double)(layer[i + 1] as GraphNode).GetData(_GridPosition);
//				i++;
//
//				for (int j = i + 1; j < layer.Count; j++)
//				{
//					GraphNode node = layer[j] as GraphNode;
//					double gridPos = (double)node.GetData(_GridPosition);
//					gridPos -= (1 - _info.ArrowsCompactFactor) * (gridPos - prevGridPos);
//					node.SetData(_GridPosition, gridPos);
//					prevGridPos = gridPos;
//				}
//			}
//		}
		private void Compact()
		{
			foreach (ArrayList layer in _layers)
			{
				int i = -1;
				while (true)
				{
					i++;
					if (!(bool)((layer[i] as GraphNode).GetData(_Dummy)))
						break;
				}

				double prevGridPos = 0;
				if (i - 1 >= 0)
					prevGridPos = (double)(layer[i - 1] as GraphNode).GetData(_GridPosition);
				i--;

				for (int j = i - 1; j >= 0; j--)
				{
					GraphNode node = layer[j] as GraphNode;
					double gridPos = (double)node.GetData(_GridPosition);
					double save = gridPos;
					gridPos = (double)(layer[j + 1] as GraphNode).GetData(_GridPosition) -
						_info.ArrowsCompactFactor * (prevGridPos - gridPos);
					node.SetData(_GridPosition, gridPos);
					prevGridPos = save;
				}

				i = layer.Count;
				while (true)
				{
					i--;
					if (!(bool)((layer[i] as GraphNode).GetData(_Dummy)))
						break;
				}

				prevGridPos = 0;
				if (i + 1 < layer.Count)
					prevGridPos = (double)(layer[i + 1] as GraphNode).GetData(_GridPosition);
				i++;

				for (int j = i + 1; j < layer.Count; j++)
				{
					GraphNode node = layer[j] as GraphNode;
					double gridPos = (double)node.GetData(_GridPosition);
					double save = gridPos;
					gridPos = (double)(layer[j - 1] as GraphNode).GetData(_GridPosition) +
						_info.ArrowsCompactFactor * (gridPos - prevGridPos);
					node.SetData(_GridPosition, gridPos);
					prevGridPos = save;
				}
			}
		}

		private void Dedummify()
		{
			bool dedum = true;
			while (dedum)
			{
				dedum = false;

				foreach (GraphLink link in _graph.Links)
				{
					if ((int)link.GetData(_DummificationLevel) == 0)
						continue;

					ArrayList points = new ArrayList();

					// Add points in reverse order
					points.Insert(0, link.Destination.Center);
					points.Insert(0, link.Origin.Center);

					// Dedummify the arrow
					GraphLink temp = link;
					int dummificationLevel = (int)link.GetData(_DummificationLevel);
					for (int d = 0; d < dummificationLevel; d++)
					{
						GraphNode node = temp.Origin;
						GraphLink prevLink = node.InLinks[0] as GraphLink;

						points.Insert(0, prevLink.Origin.Center);

						temp = prevLink;
					}

					// Restore the original arrow origin
					link.Origin = temp.Origin;

					// Reset dummification flag
					link.SetData(_DummificationLevel, (int)0);

					// Set link points
					link.SetData(_LinkPoints, points);

					// We are not going to delete the dummy elements.
					// The won't be needed anymore anyway.

					dedum = true;
					break;
				}
			}
		}

		/// <summary>
		/// Tries to minimize crossings across all levels.
		/// </summary>
		private void MinimizeCrossings()
		{
			int moves = -1;
			int maxIterations = 3;
			int iter = 0;

			while (moves != 0)
			{
				if (iter++ > maxIterations)
					break;

				moves = 0;

				// Update progress
				if (_progress != null)
					_progress(_current++, _total);

				for (int i = _layers.Count - 1; i >= 1; i--)
					moves += MinimizeCrossings(false, i);

				// Update progress
				if (_progress != null)
					_progress(_current++, _total);

				for (int i = 0; i < _layers.Count - 1; i++)
					moves += MinimizeCrossings(true, i);
			}
		}

		/// <summary>
		/// Calculates up barycenters and linkCount for the nodes
		/// in the layer whose index is specified in the parameter.
		/// </summary>
		private void CalcUpData(int layer)
		{
			if (layer == 0)
				return;

			ArrayList considered = _layers[layer] as ArrayList;
			Hashtable upLayer = new Hashtable();
			foreach (object obj in _layers[layer - 1] as ArrayList)
				upLayer[obj] = null;

			for (int i = 0; i < considered.Count; i++)
			{
				GraphNode node = considered[i] as GraphNode;

				// Calculate barycenter
				double sum = 0;
				int total = 0;

				foreach (GraphLink link in node.InLinks)
				{
					if (upLayer.Contains(link.Origin))
					{
						total++;
						sum += (double)link.Origin.GetData(_GridPosition);
					}
				}

				foreach (GraphLink link in node.OutLinks)
				{
					if (upLayer.Contains(link.Destination))
					{
						total++;
						sum += (double)link.Destination.GetData(_GridPosition);
					}
				}

				if (total > 0)
				{
					node.SetData(_UBaryCenter, (double)sum / total);
					node.SetData(_ULinkCount, total);
				}
				else
				{
					node.SetData(_UBaryCenter, (double)i);
					node.SetData(_ULinkCount, 0);
				}
			}
		}

		/// <summary>
		/// Calculates down barycenters and linkCount for the nodes
		/// in the layer whose index is specified in the parameter.
		/// </summary>
		private void CalcDownData(int layer)
		{
			if (layer == _layers.Count - 1)
				return;

			ArrayList considered = _layers[layer] as ArrayList;
			Hashtable downLayer = new Hashtable();
			foreach (object obj in _layers[layer + 1] as ArrayList)
				downLayer[obj] = null;

			for (int i = 0; i < considered.Count; i++)
			{
				GraphNode node = considered[i] as GraphNode;

				// Calculate barycenter
				double sum = 0;
				int total = 0;

				foreach (GraphLink link in node.InLinks)
				{
					if (downLayer.Contains(link.Origin))
					{
						total++;
						sum += (double)link.Origin.GetData(_GridPosition);
					}
				}

				foreach (GraphLink link in node.OutLinks)
				{
					if (downLayer.Contains(link.Destination))
					{
						total++;
						sum += (double)link.Destination.GetData(_GridPosition);
					}
				}

				if (total > 0)
				{
					node.SetData(_DBaryCenter, sum / total);
					node.SetData(_DLinkCount, total);
				}
				else
				{
					node.SetData(_DBaryCenter, (double)i);
					node.SetData(_DLinkCount, 0);
				}
			}
		}

		/// <summary>
		/// Tries to minimize crossings between layers 'layer' and
		/// 'layer' + 1 if down is true and between layers 'layer' - 1
		/// and 'layer' if down is false.
		/// </summary>
		private int MinimizeCrossings(bool down, int layer)
		{
			int iconsidered;
			ArrayList considered;

			if (down)
				considered = _layers[iconsidered = layer + 1] as ArrayList;
			else
				considered = _layers[iconsidered = layer - 1] as ArrayList;

			// Remember what it was
			ArrayList presorted = considered.Clone() as ArrayList;

			// Calculate barycenters for all nodes in the considered layer
			if (down)
				CalcUpData(iconsidered);
			else
				CalcDownData(iconsidered);

			// Now that the barycenters are calculated -> sort
			// the nodes within this layer according to the barycenters.
			considered.Sort(new NodeComparer(considered));

			// Count dislocations
			int moves = 0;
			for (int i = 0; i < considered.Count; i++)
				if (considered[i] != presorted[i])
					moves++;

			if (moves > 0)
			{
				// Now that the boxes have been arranged,
				// update their grid positions
				int inode = 0;
				foreach (GraphNode node in considered)
					node.SetData(_GridPosition, (double)inode++);
			}

			return moves;
		}

		/// <summary>
		/// This method performs a single pass over the graph
		/// in an attempt to reduce link crossings through
		/// neighbours swapping.
		/// </summary>
		private void SwapPairs()
		{
			int maxIterations = 4;
			int iter = 0;

			while (true)
			{
				if (iter++ > maxIterations)
					break;

				// Update progress
				if (_progress != null)
					_progress(_current++, _total);

				bool downwards = (iter % 4 <= 1);

				for (int l = (downwards ? 1 : _layers.Count - 2);
					downwards ? l <= _layers.Count - 2 : l >= 1; l += (downwards ? 1 : -1))
				{
					ArrayList layer = _layers[l] as ArrayList;

					bool hasSwapped = false;
					for (int n = 0; n < layer.Count - 1; n++)
					{
						// Count crossings
						int up = CountCrossings(l - 1, l);
						int down = CountCrossings(l, l + 1);
						if (downwards) up *= 2; else down *= 2;
						int crossBefore = up + down;

						if (crossBefore == 0)
							continue;

						// Swap nodes
						GraphNode node1 = layer[n] as GraphNode;
						GraphNode node2 = layer[n + 1] as GraphNode;
						double node1GridPos = (double)node1.GetData(_GridPosition);
						double node2GridPos = (double)node2.GetData(_GridPosition);
						layer[n] = node2;
						layer[n + 1] = node1;
						node1.SetData(_GridPosition, node2GridPos);
						node2.SetData(_GridPosition, node1GridPos);

						// Count crossings again and if worse
						// than before -> restore swapping
						up = CountCrossings(l - 1, l);
						down = CountCrossings(l, l + 1);
						if (downwards) up *= 2; else down *= 2;
						int crossAfter = up + down;

						if (crossAfter > crossBefore)
						{
							node1 = layer[n] as GraphNode;
							node2 = layer[n + 1] as GraphNode;
							node1GridPos = (double)node1.GetData(_GridPosition);
							node2GridPos = (double)node2.GetData(_GridPosition);
							layer[n] = node2;
							layer[n + 1] = node1;
							node1.SetData(_GridPosition, node2GridPos);
							node2.SetData(_GridPosition, node1GridPos);
						}
						else
						{
							hasSwapped = true;
						}
					}

					if (hasSwapped)
					{
						CalcUpData(l + 1);
						CalcDownData(l - 1);
					}
				}
			}
		}

		/// <summary>
		/// Returns the total number of crossings
		/// for the links inbetween the specified layers.
		/// </summary>
		private int CountCrossings(int ulayer, int dlayer)
		{
			int crossings = 0;

			Hashtable uLayer = new Hashtable();
			Hashtable dLayer = new Hashtable();
			foreach (object obj in _layers[ulayer] as ArrayList)
				uLayer[obj] = null;
			foreach (object obj in _layers[dlayer] as ArrayList)
				dLayer[obj] = null;

			// Collect the links located between the layers
			Hashtable dlinks = new Hashtable();
			ArrayList links = new ArrayList();
			ArrayList temp = new ArrayList();

			foreach (GraphNode node in uLayer.Keys)
			{
				temp.AddRange(node.InLinks);
				temp.AddRange(node.OutLinks);
			}

			foreach (GraphLink link in temp)
			{
				if (uLayer.Contains(link.Origin) && dLayer.Contains(link.Destination))
				{
					dlinks[link] = null;
					links.Add(link);
				}
				else if (dLayer.Contains(link.Origin) && uLayer.Contains(link.Destination))
				{
					links.Add(link);
				}
			}

			// Do da deed
			foreach (GraphLink link1 in links)
			{
				foreach (GraphLink link2 in links)
				{
					if (link1 == link2)
						continue;

					GraphNode n11, n12;
					GraphNode n21, n22;

					if (dlinks.Contains(link1))
					{
						n11 = link1.Origin;
						n12 = link1.Destination;
					}
					else
					{
						n11 = link1.Destination;
						n12 = link1.Origin;
					}

					if (dlinks.Contains(link2))
					{
						n21 = link2.Origin;
						n22 = link2.Destination;
					}
					else
					{
						n21 = link2.Destination;
						n22 = link2.Origin;
					}

					double n11gp = (double)n11.GetData(_GridPosition);
					double n12gp = (double)n12.GetData(_GridPosition);
					double n21gp = (double)n21.GetData(_GridPosition);
					double n22gp = (double)n22.GetData(_GridPosition);

					if ((n11gp - n21gp) * (n12gp - n22gp) < 0)
						crossings++;
				}
			}

			return crossings / 2;
		}


		private Graph _graph;
		private LayeredLayoutInfo _info;
		private ArrayList _layers;

		private LayoutProgress _progress;
		private int _current;
		private int _total;
	}
	#endregion
}
