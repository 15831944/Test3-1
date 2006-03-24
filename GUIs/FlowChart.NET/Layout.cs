// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Collections;

namespace MindFusion.LayoutSystem
{
	// Helper classes

	#region class Utilities
	[Access(AccessType.Internal)]
	internal class Utilities
	{
		public static RectangleF UnionRects(RectangleF rc1, RectangleF rc2)
		{
			RectangleF union = new RectangleF(0, 0, 0, 0);
			union.X = Math.Min(rc1.Left, rc2.Left);
			union.Y = Math.Min(rc1.Top, rc2.Top);
			union.Width = Math.Max(rc1.Right, rc2.Right) - union.X;
			union.Height = Math.Max(rc1.Bottom, rc2.Bottom) - union.Y;

			return union;
		}
	}
	#endregion


	// Adoption classes - these are implemented
	// by the client in order to adopt its data
	// structures (graphs, trees, etc.) to the
	// one expected by the layouting system

	#region interface INode
	[Access(AccessType.Public)]
	internal interface INode
	{
		/// <summary>
		/// Retrieves the bounding rectangle of the node.
		/// </summary>
		/// <returns></returns>
		RectangleF Bounds { get; set; }

		/// <summary>
		/// Retrieves all outgoing links.
		/// </summary>
		/// <returns>A collection of ILink objects</returns>
		LinkCollection OutLinks { get; }

		/// <summary>
		/// Retrieves all incoming links.
		/// </summary>
		/// <returns>A collection of ILink objects</returns>
		LinkCollection InLinks { get; }

		/// <summary>
		/// Retrieves the weight of the node. Rarely used
		/// by some layouting procedures and path-finding.
		/// </summary>
		float Weight { get; }

		/// <summary>
		/// Temporary data associated with a node during algorithms runtime.
		/// </summary>
		object Data { get; set; }
	}
	#endregion

	#region interface ILink
	[Access(AccessType.Public)]
	internal interface ILink
	{
		/// <summary>
		/// Retrieves the origin INode for this link.
		/// </summary>
		INode Origin { get; }

		/// <summary>
		/// Retrieves the destination INode object for this link.
		/// </summary>
		INode Destination { get; }

		/// <summary>
		/// Retrieves the weight of the link. Rarely used
		/// by some layouting procedures and path-finding.
		/// </summary>
		float Weight { get; }

		/// <summary>
		/// Invoked by some layouting algorithms to update the sequence
		/// of points which define the link.
		/// </summary>
		void SetPoints(ArrayList points);
	}
	#endregion

	#region interface IGraph
	[Access(AccessType.Public)]
	internal interface IGraph
	{
		/// <summary>
		/// Retrieves a collection with all nodes of the graph.
		/// </summary>
		/// <returns>INode collection</returns>
		NodeCollection Nodes { get; }

		/// <summary>
		/// Retrieves a collection with all links in the graph.
		/// </summary>
		/// <returns>ILink collection.</returns>
		LinkCollection Links { get; }

		/// <summary>
		/// Retrieves the bounding rectangle
		/// of the document, containing the graph.
		/// </summary>
		RectangleF DocRect { get; }
	}
	#endregion

	#region class NodeCollection
	/// <summary>
	/// Collection of nodes.
	/// </summary>
	[Access(AccessType.Public)]
	internal class NodeCollection : CollectionBase, ICloneable
	{
		public NodeCollection()
		{
		}

		public virtual object Clone()
		{
			NodeCollection result = new NodeCollection();

			foreach (INode node in List)
				result.Add(node);

			return result;
		}

		public void Add(INode node)
		{
			List.Add(node);
		}

		public void AddRange(ICollection nodes)
		{
			InnerList.AddRange(nodes);
		}

		public void Insert(int index, INode node)
		{
			List.Insert(index, node);
		}

		public INode GetAt(int index)
		{
			if(index >= Count)
				return null;

			return (INode)List[index];
		}

		public void Remove(INode node)
		{
			List.Remove(node);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public bool Contains(INode node)
		{
			return List.Contains(node);
		}

		public INode this[int index]
		{
			get { return GetAt(index); }
			set { List[index] = value; }
		}
	}
	#endregion

	#region class LinkCollection
	/// <summary>
	/// Collection of nodes.
	/// </summary>
	[Access(AccessType.Public)]
	internal class LinkCollection : CollectionBase, ICloneable
	{
		public LinkCollection()
		{
		}

		public virtual object Clone()
		{
			LinkCollection result = new LinkCollection();

			foreach (ILink link in List)
				result.Add(link);

			return result;
		}

		public void Add(ILink link)
		{
			List.Add(link);
		}

		public void AddRange(ICollection links)
		{
			InnerList.AddRange(links);
		}

		public void Insert(int index, ILink link)
		{
			List.Insert(index, link);
		}

		public ILink GetAt(int index)
		{
			if(index >= Count)
				return null;

			return (ILink)List[index];
		}

		public void Remove(ILink link)
		{
			List.Remove(link);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public bool Contains(ILink link)
		{
			return List.Contains(link);
		}

		public ILink this[int index]
		{
			get { return GetAt(index); }
			set { List[index] = value; }
		}
	}
	#endregion


	// Internal representation base classes.
	// All data structures are internally
	// represented by the layouting algorithms.
	// These classes provide a base for
	// this internal representation.

	#region class GraphNode
	[Access(AccessType.Internal)]
	internal class GraphNode
	{
		public GraphNode(INode node)
		{
			_data = new SortedList();
			_inLinks = new ArrayList();
			_outLinks = new ArrayList();
			_node = node;

			// Make a copy of the node's bounding rect
			_rect = node.Bounds;
		}

		public GraphNode(RectangleF bounds)
		{
			_data = new SortedList();
			_inLinks = new ArrayList();
			_outLinks = new ArrayList();
			_node = null;

			_rect = bounds;
		}

		public object GetData(int key)
		{
			return _data[key];
		}

		public void SetData(int key, object data)
		{
			_data[key] = data;
		}

		public INode Node
		{
			get { return _node; }
		}

		public virtual ArrayList InLinks
		{
			get { return _inLinks; }
		}

		public virtual ArrayList OutLinks
		{
			get { return _outLinks; }
		}

		public virtual int LinkCount
		{
			get { return _inLinks.Count + _outLinks.Count; }
		}

		public virtual PointF Center
		{
			get
			{
				return new PointF(
					_rect.Left + _rect.Width / 2,
					_rect.Top + _rect.Height / 2);
			}
			set
			{
				PointF center = Center;
				_rect.Offset(
					value.X - center.X,
					value.Y - center.Y);
			}
		}

		public RectangleF Bounds
		{
			get { return _rect; }
			set { _rect = value; }
		}

		private INode _node;
		private RectangleF _rect;
		private ArrayList _inLinks;
		private ArrayList _outLinks;
		private SortedList _data;
	}
	#endregion

	#region class GraphLink
	[Access(AccessType.Internal)]
	internal class GraphLink
	{
		public GraphLink(ILink link,
			GraphNode origin, GraphNode destination)
		{
			_data = new SortedList();
			_link = link;
			_origin = origin;
			_destination = destination;

			_origin.OutLinks.Add(this);
			_destination.InLinks.Add(this);
		}

		public GraphLink(GraphNode origin, GraphNode destination)
		{
			_data = new SortedList();
			_link = null;
			_origin = origin;
			_destination = destination;

			_origin.OutLinks.Add(this);
			_destination.InLinks.Add(this);
		}

		public object GetData(int key)
		{
			return _data[key];
		}

		public void SetData(int key, object data)
		{
			_data[key] = data;
		}

		public ILink Link
		{
			get { return _link; }
		}

		public GraphNode Origin
		{
			get { return _origin; }
			set
			{
				// First remove the link from the out links
				// of the current origin
				_origin.OutLinks.Remove(this);

				// Assign the new origin and add the link
				// to its out links
				_origin = value;
				_origin.OutLinks.Add(this);
			}
		}

		public GraphNode Destination
		{
			get { return _destination; }
			set
			{
				// First remove the link from the in links
				// of the current destination
				_destination.InLinks.Remove(this);

				// Assign the new destination and add the link
				// to its in links
				_destination = value;
				_destination.InLinks.Add(this);
			}
		}


		private ILink _link;
		private GraphNode _origin;
		private GraphNode _destination;
		private SortedList _data;
	}
	#endregion

	#region class Graph
	[Access(AccessType.Internal)]
	internal class Graph
	{
		public Graph(IGraph graph)
		{
			_data = new SortedList();
			_nodes = new ArrayList();
			_links = new ArrayList();

			foreach (INode node in graph.Nodes)
				_nodes.Add(CreateNode(node));
			foreach (ILink link in graph.Links)
				_links.Add(CreateLink(link, _nodes));
		}

		virtual protected GraphNode CreateNode(INode n)
		{
			return new GraphNode(n);
		}

		virtual protected GraphLink CreateLink(ILink l, ArrayList nodes)
		{
			GraphNode origin = null, destination = null;
			
			foreach(GraphNode g in nodes)
				if(g.Node.Equals(l.Origin))
				{
					origin = g;
					break;
				}
			foreach(GraphNode g in nodes)
				if(g.Node.Equals(l.Destination))
				{
					destination = g;
					break;
				}

			return new GraphLink(l, origin, destination);
		}

		public object GetData(int key)
		{
			return _data[key];
		}

		public void SetData(int key, object data)
		{
			_data[key] = data;
		}

		public ArrayList Nodes
		{
			get { return _nodes; }
		}

		public ArrayList Links
		{
			get { return _links; }
		}

		private ArrayList _nodes;
		private ArrayList _links;
		private SortedList _data;
	}
	#endregion


	#region tree visitors
	[Access(AccessType.Internal)]
	internal interface ITreeNodeVisitor
	{
		void Visit(TreeNode node);
	}

	[Access(AccessType.Internal)]
	internal delegate void VisitOperation(TreeNode node, MethodCallVisitor visitor);

	[Access(AccessType.Internal)]
	internal class MethodCallVisitor : ITreeNodeVisitor
	{
		public MethodCallVisitor(VisitOperation operation)
		{
			_operation = operation;
			_data = new SortedList();
		}

		public object GetData(int key)
		{
			return _data[key];
		}

		public void SetData(int key, object data)
		{
			_data[key] = data;
		}

		public virtual void Visit(TreeNode node)
		{
			_operation(node, this);
		}

		private VisitOperation _operation;
		private SortedList _data;
	}

	#endregion

	#region class TreeNode
	/// <summary>
	/// Internal class for the TreeLayout system,
	/// used during the rearrangement.
	/// </summary>
	[Access(AccessType.Internal)]
	internal class TreeNode : GraphNode
	{
		/// <summary>
		/// Create a tree node with a given parent and an adapter,
		/// containing interface to access the client's tree node.
		/// </summary>
		/// <param name="parent">The parent node of this node.</param>
		/// <param name="node">The adapter object.</param>
		public TreeNode(TreeNode parent, INode node) : base(node)
		{
			// Offset the bounding rectangle to (0, 0)
			RectangleF bounds = Bounds;
			bounds.Offset(-bounds.Left, -bounds.Top);
			Bounds = bounds;

			_parent = parent;
			if(_parent == null)
				_level = 0;
			else
				_level = _parent.Level + 1;

			_children = new ArrayList();
		}

		/// <summary>
		/// Access the depth within the tree of this node.
		/// </summary>
		public int Level
		{
			get { return _level; }
		}

		/// <summary>
		/// Access this node's parent.
		/// </summary>
		public TreeNode Parent
		{
			get { return _parent; }
		}

		/// <summary>
		/// Get the child at the specified position.
		/// </summary>
		public ArrayList Children
		{
			get { return _children; }
		}

		public override ArrayList InLinks
		{
			get { return null; }
		}

		public override ArrayList OutLinks
		{
			get { return null; }
		}

		public override int LinkCount
		{
			get { return 0; }
		}

		public override PointF Center
		{
			get
			{
				return new PointF(Bounds.Left + Bounds.Width / 2,
					Bounds.Top + Bounds.Height / 2);
			}
			set
			{
				PointF center = Center;
				RectangleF rc = Bounds;
				rc.Offset(value.X - center.X,
					value.Y - center.Y);
				Bounds = rc;
			}
		}

		private int _level;
		private TreeNode _parent;
		private ArrayList _children;
	}
	#endregion

	#region class Tree
	[Access(AccessType.Internal)]
	internal class Tree
	{
		public Tree()
		{
			_root = null;
			_treeLevels = null;
			_visited = null;
		}

		/// <summary>
		/// Perform a operation over all tree nodes in the
		/// specified order. The operation is specified
		/// through a visitor object.
		/// </summary>
		public void WalkTree(TreeNode nodeFrom, ITreeNodeVisitor visitor)
		{
			visitor.Visit(nodeFrom);

			foreach(TreeNode child in nodeFrom.Children)
				WalkTree(child, visitor);
		}

		/// <summary>
		/// Build the tree.
		/// </summary>
		public bool Build(INode rootNode)
		{
			_root = new TreeNode(null, rootNode);
			_visited = new ArrayList();
			_visited.Add(rootNode);

			if(!BuildChildren(_root))
				return false;
			BuildLevels();

			return true;
		}

		protected bool IsVisited(object obj)
		{
			return _visited.Contains(obj);
		}

		/// <summary>
		/// Recursive child-building function.
		/// </summary>
		protected bool BuildChildren(TreeNode parent)
		{
			TreeNode tn;
			INode node = parent.Node;
			foreach(object link in node.OutLinks)
			{
				INode child = ((ILink)link).Destination;
				// Check if this child is already visited to prevent loops.
				if (!IsVisited(child))
				{
					parent.Children.Add(tn = new TreeNode(parent, child));
					_visited.Add(child);
					BuildChildren(tn);
				}

				// In case of ignore arrow direction we have to inspect the
				// arrow's origin as well. If ignore arrow direction is off
				// then in OutLinks there are only outgoing links,
				// and their origin is the current node, which is already visited and
				// nothing bad will happen.
				child = ((ILink)link).Origin;
				if (!IsVisited(child))
				{
					parent.Children.Add(tn = new TreeNode(parent, child));
					_visited.Add(child);
					BuildChildren(tn);
				}
			}

			return true;
		}

		/// <summary>
		/// Build the levels of the tree. Each level contain all the nodes from that level.
		/// </summary>
		protected void BuildLevels()
		{
			int levels;
			MethodCallVisitor visitor;
			// Find the number of levels in the tree, i.e. tree depth.
			visitor = new MethodCallVisitor(new VisitOperation(this.FindMaxLevel));
			visitor.SetData(_MaxLevelData, 0);
			WalkTree(_root, visitor);
			levels = (int)visitor.GetData(_MaxLevelData) + 1;

			_treeLevels = new ArrayList();
			for(int i = 0; i < levels; i++)
				_treeLevels.Add(new ArrayList());

			// Assign each node to its appropriate level.
			visitor = new MethodCallVisitor(new VisitOperation(this.AssignLevel));
			visitor.SetData(_LevelAssignData, _treeLevels);
			WalkTree(_root, visitor);
		}

		private const int _MaxLevelData = 0;
		private const int _LevelAssignData = 1;

		private void FindMaxLevel(TreeNode node, MethodCallVisitor visitor)
		{
			visitor.SetData(_MaxLevelData,
				Math.Max(node.Level, (int)visitor.GetData(_MaxLevelData)));
		}
		private void AssignLevel(TreeNode node, MethodCallVisitor visitor)
		{
			ArrayList levels = (ArrayList)visitor.GetData(_LevelAssignData);
			((ArrayList)levels[node.Level]).Add(node);
		}

		public TreeNode Root
		{
			get
			{
				return _root;
			}
		}

		public ArrayList TreeLevels
		{
			get
			{
				return _treeLevels;
			}
		}

		/// <summary>
		/// The root tree node.
		/// </summary>
		private TreeNode _root;
		/// <summary>
		/// A list with all levels of the tree. A level is
		/// defined as a set of all nodes that are located at
		/// the same distance relative to the root node.
		/// </summary>
		ArrayList _treeLevels;

		/// <summary>
		/// A list with all visited tree nodes.
		/// This will ensure that no loops will occur.
		/// </summary>
		ArrayList _visited;
	}
	#endregion
}
