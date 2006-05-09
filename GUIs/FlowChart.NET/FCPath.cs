// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using MindFusion.FlowChartX.LayoutSystem;

namespace MindFusion.FlowChartX
{
	#region Path-Finding adoptions classes
	/// <summary>
	/// Nothing more but a pure wrapper of MindFusion.LayoutSystem.Path
	/// for the sake of full name conversion.
	/// </summary>
	public sealed class Path
	{
		internal Path(MindFusion.LayoutSystem.Path path)
		{
			_nodes = new ChartObjectCollection();
			_links = new ArrowCollection();
			_items = new ChartObjectCollection();

			foreach (FCNode node in path.Nodes)
				_nodes.Add(node.Node);
			foreach (FCLink link in path.Links)
				_links.Add(link.Arrow);
			foreach (object item in path.Items)
			{
				if (item is FCLink)
					_items.Add((item as FCLink).Arrow);
				else
					_items.Add((item as FCNode).Node);
			}
		}


		public ChartObjectCollection Nodes
		{
			get { return _nodes; }
		}

		public ArrowCollection Links
		{
			get { return _links; }
		}

		public ChartObjectCollection Items
		{
			get { return _items; }
		}

		public int Length
		{
			get { return _links.Count; }
		}

		public float GetWeight(bool incNodes, bool incLinks)
		{
			float weight = 0;

			if (incNodes)
				foreach (Node node in _nodes)
					weight += node.Weight;

			if (incLinks)
				foreach (Arrow link in _links)
					weight += link.Weight;

			return weight;
		}

		private ChartObjectCollection _nodes;
		private ArrowCollection _links;
		private ChartObjectCollection _items;
	}

	public sealed class PathCollection : CollectionBase
	{
		public PathCollection()
		{
		}

		internal PathCollection(MindFusion.LayoutSystem.PathList paths)
		{
			foreach (MindFusion.LayoutSystem.Path path in paths)
				Add(new Path(path));
		}

		public void Add(MindFusion.FlowChartX.Path p)
		{
			List.Add(p);
		}

		public void Insert(int i, MindFusion.FlowChartX.Path p)
		{
			List.Insert(i, p);
		}

		public MindFusion.FlowChartX.Path GetAt(int i)
		{
			if(i >= Count)
				return null;

			return List[i] as MindFusion.FlowChartX.Path;
		}

		public void Remove(MindFusion.FlowChartX.Path p)
		{
			List.Remove(p);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public MindFusion.FlowChartX.Path this[int index]
		{
			get
			{
				return List[index] as MindFusion.FlowChartX.Path;
			}
			set
			{
				List[index] = value;
			}
		}
	}

	public sealed class PathFinder
	{
		/// <summary>
		/// Initializes a new instance of the PathFinder class.
		/// Path-finding will depend on the direction of arrows.
		/// </summary>
		/// <param name="diagram">A FlowChart instance in which to look for paths or cycles.</param>
		public PathFinder(FlowChart diagram)
		{
			graph = new FCGraph(diagram, false);
			ignoreDirection = false;
		}

		/// <summary>
		/// Initializes a new instance of the PathFinder class.
		/// </summary>
		/// <param name="diagram">A FlowChart instance in which to look for paths or cycles.</param>
		/// <param name="ignoreDirection">Specifies whether the direction of arrows
		/// should be considered when looking for paths or cycles.</param>
		public PathFinder(FlowChart diagram, bool ignoreDirection)
		{
			graph = new FCGraph(diagram, false, ignoreDirection);
			this.ignoreDirection = ignoreDirection;
		}

		/// <summary>
		/// Finds and returns all paths starting from node 'from' and
		/// ending at node 'to'. Returns empty collection if no
		/// path exists.
		/// </summary>
		public PathCollection FindAllPaths(Node from, Node to)
		{
			FCNode fromNode = null;
			FCNode toNode = null;

			foreach (FCNode node in graph.Nodes)
			{
				if (node.Node == from)
					fromNode = node;
				else if (node.Node == to)
					toNode = node;
			}

			MindFusion.LayoutSystem.PathList paths =
				MindFusion.LayoutSystem.PathFinder.FindAllPaths(graph, fromNode, toNode);

			return new PathCollection(paths);
		}

		/// <summary>
		/// Finds and returns the shortest path between from and to.
		/// Returns null if no path exists.
		/// </summary>
		public Path FindShortestPath(Node from, Node to)
		{
			FCNode fromNode = null;
			FCNode toNode = null;

			foreach (FCNode node in graph.Nodes)
			{
				if (node.Node == from)
					fromNode = node;
				else if (node.Node == to)
					toNode = node;
			}

			MindFusion.LayoutSystem.Path path =
				MindFusion.LayoutSystem.PathFinder.FindShortestPath(graph, fromNode, toNode);

			if (path != null)
				return new Path(path);
			else
				return null;
		}

		/// <summary>
		/// Finds and returns the shortest path between from and to,
		/// considering the weight of the nodes, the weight of the 
		/// links or both.
		/// </summary>
		public Path FindShortestPath(Node from, Node to, bool useNodeWeights, bool useLinkWeights)
		{
			FCNode fromNode = null;
			FCNode toNode = null;

			foreach (FCNode node in graph.Nodes)
			{
				if (node.Node == from)
					fromNode = node;
				else if (node.Node == to)
					toNode = node;
			}

			MindFusion.LayoutSystem.Path path =
				MindFusion.LayoutSystem.PathFinder.FindShortestPath(
				graph, fromNode, toNode, useNodeWeights, useLinkWeights);

			if (path != null)
				return new Path(path);
			else
				return null;
		}

		/// <summary>
		/// Finds the longest path between two nodes.
		/// Returns null if no path exists.
		/// </summary>
		public Path FindLongestPath(Node from, Node to)
		{
			FCNode fromNode = null;
			FCNode toNode = null;

			foreach (FCNode node in graph.Nodes)
			{
				if (node.Node == from)
					fromNode = node;
				else if (node.Node == to)
					toNode = node;
			}

			MindFusion.LayoutSystem.Path path =
				MindFusion.LayoutSystem.PathFinder.FindLongestPath(graph, fromNode, toNode);

			if (path != null)
				return new Path(path);
			else
				return null;
		}

		/// <summary>
		/// Finds the longest path in the graph.
		/// Returns null if no path exists.
		/// </summary>
		public Path FindLongestPath()
		{
			MindFusion.LayoutSystem.Path path =
				MindFusion.LayoutSystem.PathFinder.FindLongestPath(graph);

			if (path != null)
				return new Path(path);
			else
				return null;
		}

		/// <summary>
		/// Finds the longest path in the diagram. The time limit
		/// specifies the maximum duration of the search process in
		/// milliseconds.
		/// Returns null if no path is found.
		/// </summary>
		public Path FindLongestPath(long timeLimit)
		{
			MindFusion.LayoutSystem.Path path =
				MindFusion.LayoutSystem.PathFinder.FindLongestPath(graph, timeLimit);

			if (path != null)
				return new Path(path);
			else
				return null;
		}

		/// <summary>
		/// Detects whether the specified node participates in a cycle.
		/// </summary>
		public Path FindCycle(Node participant)
		{
			StraightFactory factory = new StraightFactory();
			FCNode participantNode = factory.CreateNode(participant, false, false) as FCNode;

			foreach (FCNode node in graph.Nodes)
			{
				if (node == participantNode)
				{
					participantNode = node;
					break;
				}
			}

			MindFusion.LayoutSystem.Path path =
				MindFusion.LayoutSystem.PathFinder.FindCycle(graph, participantNode);

			if (path != null)
				return new Path(path);
			else
				return null;
		}

		/// <summary>
		/// Detects whether there is a cycle in a graph.
		/// </summary>
		public Path FindCycle()
		{
			MindFusion.LayoutSystem.Path path =
				MindFusion.LayoutSystem.PathFinder.FindCycle(graph);

			if (path != null)
				return new Path(path);
			else
				return null;
		}

		public PathCollection FindAllCycles()
		{
			MindFusion.LayoutSystem.PathList paths =
				MindFusion.LayoutSystem.PathFinder.FindAllCycles(graph);

			return new PathCollection(paths);
		}

		FCGraph graph;
		bool ignoreDirection;
	}
	#endregion
}
