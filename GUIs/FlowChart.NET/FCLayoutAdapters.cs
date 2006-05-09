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
using MindFusion.LayoutSystem;

namespace MindFusion.FlowChartX.LayoutSystem
{
	#region Factory classes
	internal interface IFactory
	{
		/// <summary>
		/// Creates new node adapter object.
		/// </summary>
		INode CreateNode(Node node, bool keepGroups,
			bool ignoreArrowDirection);

		/// <summary>
		/// Creates new arrow adapter object.
		/// </summary>
		ILink CreateLink(Arrow arrow, bool keepGroups,
			bool ignoreArrowDirection);

		/// <summary>
		/// Collects all nodes.
		/// </summary>
		NodeCollection Nodes { get; }

		/// <summary>
		/// Collects all links.
		/// </summary>
		LinkCollection Links { get; }
	}

	/// <summary>
	/// Factory used to create ordinary adapter objects.
	/// </summary>
	internal class StraightFactory : IFactory
	{
		public StraightFactory()
		{
			_nodes = new Hashtable();
			_links = new Hashtable();
		}

		public virtual INode CreateNode(Node node,
			bool keepGroups, bool ignoreArrowDirection)
		{
			// If groups' layout is preserved, then, the underlying
			// node is always the highest-in-hierarchy group master.
			if (keepGroups)
			{
				while (node.MasterGroup != null)
				{
					if (!(node.MasterGroup.MainObject is Node))
						break;

					node = node.MasterGroup.MainObject as Node;
				}
			}

			if (_nodes.Contains(node))
				return _nodes[node] as INode;

			FCNode result = new FCNode();
			_nodes[node] = result;
			result.Create(node, keepGroups,
				ignoreArrowDirection, this);

			return result;
		}

		public virtual ILink CreateLink(Arrow arrow,
			bool keepGroups, bool ignoreArrowDirection)
		{
			if (_links.Contains(arrow))
				return _links[arrow] as ILink;

			FCLink result = new FCLink();
			_links[arrow] = result;
			result.Create(arrow, keepGroups,
				ignoreArrowDirection, this);

			return result;
		}

		public virtual NodeCollection Nodes
		{
			get
			{
				NodeCollection result = new NodeCollection();
				result.AddRange(_nodes.Values);
				return result;
			}
		}

		public virtual LinkCollection Links
		{
			get
			{
				LinkCollection result = new LinkCollection();
				result.AddRange(_links.Values);
				return result;
			}
		}


		private Hashtable _nodes;
		private Hashtable _links;
	}

	/// <summary>
	/// Factory used to create reversed adapter objects.
	/// </summary>
	internal class ReversedFactory : IFactory
	{
		public ReversedFactory()
		{
			_nodes = new Hashtable();
			_links = new Hashtable();
		}

		public virtual INode CreateNode(Node node,
			bool keepGroups, bool ignoreArrowDirection)
		{
			// If groups' layout is preserved, then, the underlying
			// node is always the highest-in-hierarchy group master.
			if (keepGroups)
			{
				while (node.MasterGroup != null)
				{
					if (!(node.MasterGroup.MainObject is Node))
						break;

					node = node.MasterGroup.MainObject as Node;
				}
			}

			if (_nodes.Contains(node))
				return _nodes[node] as INode;

			ReversedFCNode result = new ReversedFCNode();
			_nodes[node] = result;
			result.Create(node, keepGroups,
				ignoreArrowDirection, this);

			return result;
		}

		public virtual ILink CreateLink(Arrow arrow,
			bool keepGroups, bool ignoreArrowDirection)
		{
			if (_links.Contains(arrow))
				return _links[arrow] as ILink;

			ReversedFCLink result = new ReversedFCLink();
			_links[arrow] = result;
			result.Create(arrow, keepGroups,
				ignoreArrowDirection, this);

			return result;
		}

		public virtual NodeCollection Nodes
		{
			get
			{
				NodeCollection result = new NodeCollection();
				result.AddRange(_nodes.Values);
				return result;
			}
		}

		public virtual LinkCollection Links
		{
			get
			{
				LinkCollection result = new LinkCollection();
				result.AddRange(_links.Values);
				return result;
			}
		}


		private Hashtable _nodes;
		private Hashtable _links;
	}
	#endregion

	#region class FCNode
	internal class FCNode : INode
	{
		public FCNode()
		{
		}
		
		public void Create(Node node, bool keepGroups,
			bool ignoreArrowDirection, IFactory factory)
		{
			_node = node;
			_keepGroups = keepGroups;
			_ignoreArrowDirection = ignoreArrowDirection;

			// If groups' layout is preserved, then, the underlying
			// node is always the highest-in-hierarchy group master.
			if (_keepGroups)
			{
				while (node.MasterGroup != null)
				{
					if (!(node.MasterGroup.MainObject is Node))
						break;

					node = node.MasterGroup.MainObject as Node;
				}

				_node = node;
			}

			// Enumerate incoming and outgoing links
			_inLinks = GetInLinks(factory);
			_outLinks = GetOutLinks(factory);
			_allLinks = new LinkCollection();
			_allLinks.AddRange(_inLinks);
			_allLinks.AddRange(_outLinks);
		}

		/// <summary>
		/// Get the node's bounding rectangle.
		/// </summary>
		public virtual RectangleF Bounds
		{
			get
			{
				RectangleF result = _node.getBoundingRect();
				RectangleF rotated = _node.getRotatedBounds();

				// Calculate the bounding rect for the entire group
				if (_keepGroups)
				{
					MindFusion.FlowChartX.Visitors.GroupBoundsVisitor vis =
						new MindFusion.FlowChartX.Visitors.GroupBoundsVisitor(result, rotated);
					_node.visitAttachedItems(vis);
					result = vis.RotatedBounds;
				}
				else
				{
					result = rotated;
				}

				return result;
			}
			set
			{
				RectangleF bounds = Bounds;
				RectangleF main = _node.getRotatedBounds();
				float dx = main.Left - bounds.Left;
				float dy = main.Top - bounds.Top;

				_node.moveWith(value.Left + dx - bounds.X, value.Top + dy - bounds.Y);
			}
		}

		/// <summary>
		/// Returns a collection of all outgoing links.
		/// </summary>
		private LinkCollection GetOutLinks(IFactory factory)
		{
			LinkCollection links = new LinkCollection();

			ChartObjectCollection nodes = new ChartObjectCollection();
			nodes.Add(_node);

			// Collect all nodes within that group
			if (_keepGroups)
			{
				int i = 0;
				while (i < nodes.Count)
				{
					Node node = (Node)nodes[i];
					i++;

					if (node.SubordinateGroup != null)
					{
						foreach (ChartObject sub in node.SubordinateGroup.AttachedObjects)
						{
							// Check if attached object is node
							if (!(sub is Node))
								continue;

							// Add the node to the nodes of the group
							if (!nodes.Contains(sub))
								nodes.Add(sub);
						}
					}
				}
			}

			// Now get all outgoing links, that do not point to 
			// objects in the group
			foreach (Node node in nodes)
			{
				foreach (Arrow a in node.OutgoingArrows)
				{
					// Ignore non-layoutable objects
					if (a.IgnoreLayout)
						continue;
					if (a.Destination.Frozen)
						continue;
					if (!a.IsConnected)
						continue;

					// Add only if it points outside the group
					if (!nodes.Contains(a.Destination))
					{
						// Use the factory to create the adapter link.
						// That way, if the adapter for the arrow already
						// exists, it will be returned instead of creating a
						// new adapter.
						links.Add(factory.CreateLink(a,
							_keepGroups, _ignoreArrowDirection));
					}
				}

				// Collect per-row arrows for tables
				if (node is Table)
				{
					Table t = node as Table;

					foreach (Table.Row r in t.Rows)
					{
						foreach (Arrow a in r.OutgoingArrows)
						{
							// Ignore non-layoutable objects
							if (a.IgnoreLayout)
								continue;
							if (a.Destination.Frozen)
								continue;
							if (!a.IsConnected)
								continue;

							// Add only if it points outside the group
							if (!nodes.Contains(a.Destination))
							{
								// Use the factory to create the adapter link.
								// That way, if the adapter object for the arrow already
								// exists, it will be returned instead of creating a
								// new adapter.
								links.Add(factory.CreateLink(a,
									_keepGroups, _ignoreArrowDirection));
							}
						}
					}
				}
			}

			return links;
		}

		private LinkCollection GetInLinks(IFactory factory)
		{
			LinkCollection links = new LinkCollection();

			ChartObjectCollection nodes = new ChartObjectCollection();
			nodes.Add(_node);

			// Collect all objects within that group
			if (_keepGroups)
			{
				int i = 0;
				while (i < nodes.Count)
				{
					Node node = nodes[i] as Node;
					i++;

					if (node.SubordinateGroup != null)
					{
						foreach (ChartObject sub in node.SubordinateGroup.AttachedObjects)
						{
							// Check if attached object is a node
							if (!(sub is Node))
								continue;

							// Add the node to the nodes of the group
							if (!nodes.Contains(sub))
								nodes.Add(sub);
						}
					}
				}
			}

			foreach (Node node in nodes)
			{
				foreach (Arrow a in node.IncomingArrows)
				{
					// Ignore non-layoutable objects
					if (a.IgnoreLayout)
						continue;
					if (a.Origin.Frozen)
						continue;
					if (!a.IsConnected)
						continue;

					// Add only if it comes from outside the group
					if (!nodes.Contains(a.Origin))
					{
						// Use the factory to create the adapter link.
						// That way, if the adapter for the arrow already
						// exists, it will be returned instead of creating a
						// new adapter.
						links.Add(factory.CreateLink(a,
							_keepGroups, _ignoreArrowDirection));
					}
				}

				if (node is Table)
				{
					Table t = node as Table;

					foreach (Table.Row r in t.Rows)
					{
						foreach (Arrow a in r.IncomingArrows)
						{
							// Ignore non-layoutable objects
							if (a.IgnoreLayout)
								continue;
							if (a.Origin.Frozen)
								continue;
							if (!a.IsConnected)
								continue;

							// Add only if it comes from outside the group
							if (!nodes.Contains(a.Origin))
							{
								// Use the factory to create the adapter link.
								// That way, if the adapter for the arrow already
								// exists, it will be returned instead of creating a
								// new adapter.
								links.Add(factory.CreateLink(a,
									_keepGroups, _ignoreArrowDirection));
							}
						}
					}
				}
			}

			return links;
		}

		/// <summary>
		/// Get the node's outgoing links. In case of IgnoreArrowDirection - 
		/// returns all links.
		/// </summary>
		public virtual LinkCollection OutLinks
		{
			get
			{
				if (_ignoreArrowDirection)
					return _allLinks;
				else
					return _outLinks;
			}
		}

		/// <summary>
		/// Get the node's incoming arrows.
		/// </summary>
		/// <returns></returns>
		public virtual LinkCollection InLinks
		{
			get
			{
				if (_ignoreArrowDirection)
					return _allLinks;
				else
					return _inLinks;
			}
		}

		/// <summary>
		/// Gets the weight value associated with the node.
		/// </summary>
		public virtual float Weight
		{
			get { return _node.Weight; }
		}

		/// <summary>
		/// Returns the underlying FlowChart object.
		/// </summary>
		public Node Node
		{
			get { return _node; }
		}

		public object Data
		{
			get { return _node.getData(Constants.KEY_WRAPPER); }
			set { _node.setData(Constants.KEY_WRAPPER, value); }
		}


		private Node _node;
		private bool _keepGroups;
		private bool _ignoreArrowDirection;
		private LinkCollection _inLinks;
		private LinkCollection _outLinks;
		private LinkCollection _allLinks;
	}
	#endregion

	#region class FCLink
	internal class FCLink : ILink
	{
		public FCLink()
		{
		}

		public void Create(Arrow arrow, bool keepGroups,
			bool ignoreArrowDirection, IFactory factory)
		{
			_arrow = arrow;
			_origin = factory.CreateNode(_arrow.Origin,
				keepGroups, ignoreArrowDirection);
			_destination = factory.CreateNode(_arrow.Destination,
				keepGroups, ignoreArrowDirection);
		}

		public virtual void SetPoints(ArrayList points)
		{
			if (_arrow.IgnoreLayout)
				return;

			// Set arrow points
			bool route = _arrow.AutoRoute;
			_arrow.SilentAutoRoute = false;
			_arrow.SegmentCount = (short)(points.Count - 1);
			_arrow.Points.Clear();
			_arrow.Points.AddRange(points);
			_arrow.SilentAutoRoute = route;

			_arrow.UpdateFromPoints();
		}

		public virtual INode Origin
		{
			get { return _origin; }
		}

		public virtual INode Destination
		{
			get { return _destination; }
		}

		public virtual float Weight
		{
			get { return _arrow.Weight; }
		}

		public virtual INode GetOppositeNode(INode aNode)
		{
			if (aNode == Destination)
				return Origin;

			return Destination;
		}

		/// <summary>
		/// Gets the underlying FlowChart object.
		/// </summary>
		public Arrow Arrow
		{
			get { return _arrow; }
		}


		private Arrow _arrow;
		private INode _origin;
		private INode _destination;
	}
	#endregion

	#region class FCGraph
	internal class FCGraph : IGraph
	{
		/// <summary>
		/// Constructs an empty graph.
		/// </summary>
		public FCGraph(FlowChart chart)
		{
			_chart = chart;
			directed = true;

			_nodes = new NodeCollection();
			_links = new LinkCollection();
		}

		/// <summary>
		/// Constructs a new graph representing
		/// the specified FlowChart diagram.
		/// </summary>
		public FCGraph(FlowChart chart, bool keepGroups)
		{
			_chart = chart;
			directed = true;

			Build(keepGroups, false);
		}

		/// <summary>
		/// Constructs a new graph representing
		/// the specified FlowChart diagram.
		/// </summary>
		public FCGraph(FlowChart chart, bool keepGroups, bool ignoreArrowDirection)
		{
			_chart = chart;
			directed = !ignoreArrowDirection;

			Build(keepGroups, ignoreArrowDirection);
		}

		private void Build(bool keepGroups, bool ignoreArrowDirection)
		{
			IFactory factory = CreateFactory();

			// Collect nodes
			_nodes = new NodeCollection();

			// Traverse all boxes
			foreach (Box b in _chart.Boxes)
			{
				// Ignore non-layoutable nodes
				if (b.Frozen)
					continue;

				if (keepGroups)
				{
					// Add only masters
					if (b.MasterGroup == null ||
						b.MasterGroup.MainObject is Arrow)
						_nodes.Add(factory.CreateNode(b,
							keepGroups, ignoreArrowDirection));
				}
				else
				{
					_nodes.Add(factory.CreateNode(b,
						keepGroups, ignoreArrowDirection));
				}
			}

			foreach (ControlHost h in _chart.ControlHosts)
			{
				// Ignore non-layoutable nodes
				if (h.Frozen)
					continue;

				if (keepGroups)
				{
					// Add only masters
					if (h.MasterGroup == null ||
						h.MasterGroup.MainObject is Arrow)
						_nodes.Add(factory.CreateNode(h,
							keepGroups, ignoreArrowDirection));
				}
				else
				{
					_nodes.Add(factory.CreateNode(h,
						keepGroups, ignoreArrowDirection));
				}
			}

			// Traverse all tables
			foreach (Table t in _chart.Tables)
			{
				// Ignore non-layoutable nodes
				if (t.Frozen)
					continue;

				if (keepGroups)
				{
					if (t.MasterGroup == null ||
						t.MasterGroup.MainObject is Arrow)
						_nodes.Add(factory.CreateNode(t,
							keepGroups, ignoreArrowDirection));
				}
				else
				{
					_nodes.Add(factory.CreateNode(t,
						keepGroups, ignoreArrowDirection));
				}
			}

			// Collect links
			_links = factory.Links;
		}

		public virtual NodeCollection Nodes
		{
			get { return _nodes; }
		}

		public virtual LinkCollection Links
		{
			get { return _links; }
		}

		public virtual RectangleF DocRect
		{
			get { return _chart.DocExtents; }
		}

		public virtual bool Directed
		{
			get { return directed; }
		}

		/// <summary>
		/// Gets the root node of this graph (assuming that the graph is tree).
		/// If there is no root node, returns null.
		/// </summary>
		public INode Root
		{
			get
			{
				foreach (INode node in _nodes)
					if (node.InLinks.Count == 0)
						return node;

				return null;
			}
		}

		/// <summary>
		/// Gets the smallest rectangle containing all graph nodes
		/// and optionally the graph links.
		/// </summary>
		public RectangleF GetBounds(bool includeLinks)
		{
			RectangleF result = RectangleF.Empty;

			foreach (FCNode node in _nodes)
				result = Utilities.unionNonEmptyRects(result, node.Bounds);

			if (includeLinks)
			{
				foreach (FCLink link in _links)
					foreach (PointF pt in link.Arrow.Points)
						result = Utilities.unionRects(result,
							new RectangleF(pt, SizeF.Empty));
			}

			return result;
		}

		/// <summary>
		/// Creates the required factory for building graph elements.
		/// </summary>
		protected virtual IFactory CreateFactory()
		{
			return new StraightFactory();
		}


		private FlowChart _chart;
		private NodeCollection _nodes;
		private LinkCollection _links;
		private bool directed;
	}
	#endregion

	#region class ReversedFCNode
	internal class ReversedFCNode : FCNode
	{
		public ReversedFCNode()
		{
		}

		public override LinkCollection OutLinks
		{
			get { return base.InLinks; }
		}

		public override LinkCollection InLinks
		{
			get { return base.OutLinks; }
		}
	}
	#endregion

	#region class ReversedFCLink
	internal class ReversedFCLink : FCLink
	{
		public ReversedFCLink()
		{
		}

		public override INode Origin
		{
			get { return base.Destination; }
		}

		public override INode Destination
		{
			get { return base.Origin; }
		}
	}
	#endregion

	#region class ReversedFCGraph
	internal class ReversedFCGraph : FCGraph
	{
		public ReversedFCGraph(FlowChart chart) : base(chart)
		{
		}

		public ReversedFCGraph(FlowChart chart, bool keepGroups,
			bool ignoreArrowDirection) : base(chart, keepGroups, ignoreArrowDirection)
		{
		}

		protected override IFactory CreateFactory()
		{
			return new ReversedFactory();
		}
	}
	#endregion

	#region class FCGraphBuilder
	internal class FCGraphBuilder : MindFusion.LayoutSystem.IGraphBuilder
	{
		public FCGraphBuilder(FlowChart chart, bool reversedArrows)
		{
			_chart = chart;
			_reversedArrows = reversedArrows;
		}

		public MindFusion.LayoutSystem.IGraph Create()
		{
			if (_reversedArrows)
				return new ReversedFCGraph(_chart);
			else
				return new FCGraph(_chart);
		}


		private FlowChart _chart;
		private bool _reversedArrows;
	}
	#endregion
}
