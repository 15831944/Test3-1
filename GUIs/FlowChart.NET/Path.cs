// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;

namespace MindFusion.LayoutSystem
{
	[Access(AccessType.Public)]
	internal class Path
	{
		public Path()
		{
			_nodes = new NodeCollection();
			_links = new LinkCollection();
			_items = new ArrayList();
		}

		public Path(Path p)
		{
			_nodes = p._nodes.Clone() as NodeCollection;
			_links = p._links.Clone() as LinkCollection;
			_items = p._items.Clone() as ArrayList;
		}

		/// <summary>
		/// Checks whether the specified node is a participant in the path.
		/// </summary>
		public bool Contains(INode node)
		{
			foreach (INode n in _nodes)
				if (n.Equals(node))
					return true;

			return false;
		}

		/// <summary>
		/// Checks whether the specified link is a participant in the path.
		/// </summary>
		public bool Contains(ILink link)
		{
			foreach (ILink l in _links)
				if (l.Equals(link))
					return true;

			return false;
		}

		/// <summary>
		/// Determines whether the current path traverses the same
		/// nodes as the specified path in the same order, disregard
		/// which one is the starting node.
		/// </summary>
		public bool SameCycle(Path path)
		{
			ArrayList items = path._items.Clone() as ArrayList;

			if (items.Count != _items.Count)
				return false;

			int iterations = items.Count;
			for (int i = 0; i < iterations; i++)
			{
				if (SameCycle(items))
					return true;

				object item = items[0];
				items.RemoveAt(0);
				items.Add(item);
			}

			return false;
		}

		/// <summary>
		/// The same as the above but the starting nodes need to be the same.
		/// </summary>
		private bool SameCycle(ArrayList items)
		{
			for (int i = 0; i < items.Count; i++)
			{
				if (items[i].GetType() != _items[i].GetType())
					return false;
				if (!items[i].Equals(_items[i]))
					return false;
			}

			return true;
		}

	
		/// <summary>
		/// Gets the list of all nodes connected by the path.
		/// </summary>
		public NodeCollection Nodes
		{
			get { return _nodes; }
		}

		/// <summary>
		/// Gets a list of all links composing the path.
		/// </summary>
		public LinkCollection Links
		{
			get { return _links; }
		}

		/// <summary>
		/// Gets a list with all items (nodes and links) composing the path.
		/// </summary>
		public ArrayList Items
		{
			get { return _items; }
		}

		/// <summary>
		/// Gets the length of the path.
		/// </summary>
		public int Length
		{
			get { return _links.Count; }
		}

		/// <summary>
		/// Gets the length of the path expressed
		/// as a sum of the weights of all nodes and
		/// links, composing the path.
		/// </summary>
		public float GetWeight(bool incNodes, bool incLinks)
		{
			float weight = 0;

			if (incNodes)
				foreach (INode node in _nodes)
					weight += node.Weight;

			if (incLinks)
				foreach (ILink link in _links)
					weight += link.Weight;

			return weight;
		}


		/// <summary>
		/// Append the node and the link to the path.
		/// </summary>
		internal void Add(ILink link, INode node)
		{
			_links.Add(link);
			_items.Add(link);
			_nodes.Add(node);
			_items.Add(node);
		}

		/// <summary>
		/// Append the node to the path.
		/// </summary>
		internal void Add(INode node)
		{
			_nodes.Add(node);
			_items.Add(node);
		}


		private NodeCollection _nodes;
		private LinkCollection _links;
		private ArrayList _items;
	}

	#region class PathList
	[Access(AccessType.Public)]
	internal class PathList : CollectionBase
	{
		public PathList()
		{
		}

		public void Add(Path p)
		{
			List.Add(p);
		}

		public void AddRange(ICollection c)
		{
			InnerList.AddRange(c);
		}

		public void Insert(int i, Path p)
		{
			List.Insert(i, p);
		}

		public Path GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (Path)List[i];
		}

		public void Remove(Path p)
		{
			List.Remove(p);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public Path this[int index]
		{
			get
			{
				return (Path)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public bool Contains(Path p)
		{
			return List.Contains(p);
		}
	}
	#endregion

	[Access(AccessType.Internal)]
	internal class PathStats
	{
		internal PathStats()
		{
			currentSet = new Hashtable();
			newSet = new Hashtable();
		}

		internal Hashtable currentSet;
		internal Hashtable newSet;
	}

	[Access(AccessType.Public)]
	internal class PathFinder
	{
		/// <summary>
		/// Finds and returns all paths starting from node 'from' and
		/// ending at node 'to'. Returns empty collection if no
		/// path exists.
		/// </summary>
		public static PathList FindAllPaths(IGraph graph, INode from, INode to)
		{
			return FindPaths(graph, from, to, false);
		}

		/// <summary>
		/// Finds and returns the shortest path between from and to.
		/// Returns null if no path exists.
		/// </summary>
		public static Path FindShortestPath(IGraph graph, INode from, INode to)
		{
			PathList paths = FindPaths(graph, from, to, true);

			if (paths.Count > 0)
				return paths[0];
			else
				return null;
		}

		/// <summary>
		/// Finds and returns the shortest path between from and to,
		/// considering the weight of the nodes, the weight of the 
		/// links or both.
		/// </summary>
		public static Path FindShortestPath(IGraph graph, INode from, INode to,
			bool useNodeWeights, bool useLinkWeights)
		{
			if (!useNodeWeights && !useLinkWeights)
				return FindShortestPath(graph, from, to);

			// Find all paths between from and to and
			// pick the less heavy one
			PathList paths = FindPaths(graph, from, to, false);

			if (paths.Count == 0)
				return null;

			SortedList table = new SortedList();
			foreach (Path path in paths)
				table[path.GetWeight(useNodeWeights, useLinkWeights)] = path;

			// Return the first path
			return table[table.GetKey(0)] as Path;
		}

		/// <summary>
		/// Finds the longest path between two nodes.
		/// Returns null if no path exists.
		/// </summary>
		public static Path FindLongestPath(IGraph graph, INode from, INode to)
		{
			PathList paths = FindPaths(graph, from, to, false);

			if (paths.Count > 0)
				return paths[paths.Count - 1];
			else
				return null;
		}

		/// <summary>
		/// Finds the longest path in a graph.
		/// Returns null if path is not found.
		/// </summary>
		public static Path FindLongestPath(IGraph graph)
		{
			return FindLongestPath(graph, 0);
		}

		/// <summary>
		/// Finds the longest path in the graph. The time limit
		/// specifies the maximum duration of the search process
		/// in milliseconds.
		/// Returns null if no path exists.
		/// </summary>
		public static Path FindLongestPath(IGraph graph, long timeLimit)
		{
			PathList paths = new PathList();

			// create initial set of paths of length zero
			foreach (INode node in graph.Nodes)
			{
				// create a path consisting only of this node
				Path path = new Path();
				path.Add(node);
				paths.Add(path);

				// paths in which this node participates
				PathStats pathStats = new PathStats();
				pathStats.currentSet.Add(path, path);
				node.Data = pathStats;
			}

			long startMillis = DateTime.Now.Ticks / 10000;

			// find set of paths that are one node longer than in previous set
			bool morePathsAdded = false;
			do
			{
				morePathsAdded = false;
				PathList newPaths = new PathList();

				// try to extend each path in the previous set
				foreach (Path path in paths)
				{
					INode lastNode = path.Nodes[path.Nodes.Count - 1];
					foreach (ILink link in lastNode.OutLinks)
					{
						INode nodeToAdd = link.Destination;

						// test for cycles
						PathStats testPaths = nodeToAdd.Data as PathStats;
						if (testPaths.currentSet.Contains(path)) continue;

						// extend the path with this node
						Path newPath = new Path(path);
						newPath.Add(link, nodeToAdd);

						// update all nodes in the new path
						foreach (INode node in newPath.Nodes)
						{
							PathStats nodePaths = node.Data as PathStats;
							nodePaths.newSet.Add(newPath, newPath);
						}

						// add to the new set
						morePathsAdded = true;
						newPaths.Add(newPath);
					}
				}

				if (morePathsAdded)
				{
					paths = newPaths;

					// remove shorter paths from node associated path lists
					// they are not needed in next iterations
					foreach (INode node in graph.Nodes)
					{
						PathStats nodePaths = node.Data as PathStats;
						Hashtable swap = nodePaths.currentSet;
						nodePaths.currentSet = nodePaths.newSet;
						nodePaths.newSet = swap;
						nodePaths.newSet.Clear();
					}
				}

				// Check time limit
				if (timeLimit > 0 &&
					DateTime.Now.Ticks / 10000 - startMillis > timeLimit)
					break;
			}
			while (morePathsAdded);

			// cleanup
			foreach (INode node in graph.Nodes)
				node.Data = null;

			if (paths.Count == 0) return null;

			return paths[0];
		}

		/// <summary>
		/// Detects whether the specified node participates in a cycle.
		/// </summary>
		public static Path FindCycle(IGraph graph, INode participant)
		{
			PathList cycles = FindPaths(graph, participant, participant, false);

			if (cycles.Count > 0)
			{
				// Remove the last node from the path to avoid duplicates
				Path path = cycles[0];
				path.Nodes.RemoveAt(path.Nodes.Count - 1);
				path.Items.RemoveAt(path.Items.Count - 1);

				return path;
			}

			return null;
		}

		/// <summary>
		/// Detects whether there is a cycle in a graph.
		/// </summary>
		public static Path FindCycle(IGraph graph)
		{
			foreach (INode node in graph.Nodes)
			{
				PathList cycles = FindPaths(graph, node, node, false);

				if (cycles.Count > 0)
				{
					// Remove the last node from the path to avoid duplicates
					Path path = cycles[0];
					path.Nodes.RemoveAt(path.Nodes.Count - 1);
					path.Items.RemoveAt(path.Items.Count - 1);

					return path;
				}
			}

			return null;
		}

		public static PathList FindAllCycles(IGraph graph)
		{
			PathList result = new PathList();

			foreach (INode node in graph.Nodes)
			{
				PathList cycles = FindPaths(graph, node, node, false);

				// Remove the last node from the path to avoid duplicates
				foreach (Path path in cycles)
				{
					path.Nodes.RemoveAt(path.Nodes.Count - 1);
					path.Items.RemoveAt(path.Items.Count - 1);
				}

				// Add cycles to result list skipping equal sycles.
				foreach (Path path in cycles)
				{
					bool found = false;
					foreach (Path path2 in result)
					{
						if (path2.SameCycle(path))
						{
							found = true;
							break;
						}
					}

					if (!found)
						result.Add(path);
				}
			}

			return result;
		}


		private static PathList FindPaths(IGraph graph,
			INode from, INode to, bool shortestOnly)
		{
			PathList result = new PathList();
			Path path = null;

			if (from == to && shortestOnly)
			{
				// Return a collection with a single path consisting of a single node.
				path = new Path();
				path.Add(from);
				result.Add(path);

				return result;
			}

			// Perform the search
			PathList tempCol = new PathList();
			Path tempPath;

			// Create the first path, constisting only of the first node
			tempCol.Add(tempPath = new Path());
			tempPath.Add(from);

			bool pathFound = false;
			while (true)
			{
				int size = tempCol.Count;
				if (size == 0)
					break;
				if (pathFound && shortestOnly)
					break;

				// For all paths - get their next nodes
				for (int i = 0; i < size; i++)
				{
					tempPath = tempCol[0];
					tempCol.RemoveAt(0);

					// Get the last node for this path and find its successors
					INode lastNode = tempPath.Nodes[tempPath.Nodes.Count - 1];
					LinkCollection links = lastNode.OutLinks;
					foreach (ILink link in links)
					{
						INode nextNode;

						// Get the next node in the path
						nextNode = link.Destination;

						// Check if the path target is reached
						if (nextNode.Equals(to))
						{
							// We've reached the end
							Path newPath = new Path(tempPath);
							newPath.Add(link, nextNode);
							result.Add(newPath);
							pathFound = true;
							continue;
						}

						// The node does not belong to the path -> add it
						if (!tempPath.Contains(nextNode))
						{
							Path newPath = new Path(tempPath);
							newPath.Add(link, nextNode);
							tempCol.Add(newPath);
						}
					}
				}
			}

			return result;
		}
	}
}
