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
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace MindFusion.FlowChartX.LayoutSystem
{
	using Layout = MindFusion.LayoutSystem;

	/// <summary>
	/// Common interface for all FlowChart.NET layouters.
	/// </summary>
	public interface ILayout
	{
		bool Arrange(FlowChart chart);

		LayoutNode LayoutNode { get; set; }
		LayoutLink LayoutLink { get; set; }
	}


	public delegate void LayoutProgress(int current, int total);
	public delegate void LayoutNode(Node node, RectangleF oldBounds);
	public delegate void LayoutLink(Arrow link);


	#region Tree Layout adoption classes
	public enum TreeLayoutArrowType
	{
		Straight,
		Cascading2,
		Cascading3,
		Rounded
	}

	public enum TreeLayoutType
	{
		Cascading,
		Centered,
		Radial
	}

	public enum TreeLayoutDirection
	{
		TopToBottom = Layout.TreeLayoutDirection.TopToBottom,
		BottomToTop = Layout.TreeLayoutDirection.BottomToTop,
		LeftToRight = Layout.TreeLayoutDirection.LeftToRight,
		RightToLeft = Layout.TreeLayoutDirection.RightToLeft
	}


	[ComVisible(true)]
	public class TreeLayout : Component, ILayout
	{
		public TreeLayout() {}

		public TreeLayout(Node root)
		{
			_root = root;
			_keepRootPosition = true;
		}

		public TreeLayout(TreeLayoutType type)
		{
			_type = type;
		}

		public TreeLayout(Node root, TreeLayoutType type)
		{
			_root = root;
			_type = type;
			_keepRootPosition = true;
		}

		public TreeLayout(
			Node root, TreeLayoutType type,
			bool reversedArrows,
			TreeLayoutArrowType arrowStyle,
			TreeLayoutDirection direction,
			float levelDistance, float nodeDistance,
			bool keepRootPosition, float xGap, float yGap)
		{
			_root = root;
			_type = type;
			_reversedArrows = reversedArrows;
			_arrowStyle = arrowStyle;
			_direction = direction;
			_levelDistance = levelDistance;
			_nodeDistance = nodeDistance;
			_keepRootPosition = keepRootPosition;
			_xGap = xGap;
			_yGap = yGap;
		}

		public TreeLayout(
			Node root, TreeLayoutType type,
			bool reversedArrows,
			TreeLayoutArrowType arrowStyle,
			TreeLayoutDirection direction,
			float levelDistance, float nodeDistance,
			bool keepRootPosition, float xGap, float yGap, bool keepGroupLayout)
		{
			_root = root;
			_type = type;
			_reversedArrows = reversedArrows;
			_arrowStyle = arrowStyle;
			_direction = direction;
			_levelDistance = levelDistance;
			_nodeDistance = nodeDistance;
			_keepRootPosition = keepRootPosition;
			_xGap = xGap;
			_yGap = yGap;
			_keepGroupLayout = keepGroupLayout;
		}

		/// <summary>
		/// Performs the arrangement.
		/// </summary>
		public virtual bool Arrange(FlowChart chart)
		{
			if (_ignoreArrowDirection && _root == null)
				return false;

			chart.UndoManager.onStartLayout("Tree layout");

			// Build the graph
			FCGraph graph = null;
			if (_reversedArrows)
			{
				graph = new ReversedFCGraph(chart,
					_keepGroupLayout, _ignoreArrowDirection);
			}
			else
			{
				graph = new FCGraph(chart,
					_keepGroupLayout, _ignoreArrowDirection);
			}

			// Get the root node within the graph
			Layout.INode rootNode = null;
			if (_root != null)
			{
				foreach (FCNode node in graph.Nodes)
				{
					if (node.Node == _root)
					{
						rootNode = node;
						break;
					}
				}
			}

			CalculateAnchors();

			// Split graph to subgraphs
			Layout.IGraph[] subgraphs =
				Layout.GraphSplitter.Split(graph,
				new FCGraphBuilder(chart, _reversedArrows));

			float xOffset = this.XGap;
			foreach (FCGraph subgraph in subgraphs)
			{
				Layout.INode theRoot = null;
				if (subgraph.Nodes.Contains(rootNode))
				{
					// The root node of the user is within this
					// subgraph - respect the user's wishes
					theRoot = rootNode;
				}
				else
				{
					// If the user specified a root node,
					// arrange only the graph, containing
					// that node
					if (rootNode != null)
						continue;

					// If the subgraph has root node, use it,
					// otherwise select the first node as root
					Layout.INode subRoot = subgraph.Root;

					if (subRoot == null)
						theRoot = subgraph.Nodes[0];
					else
						theRoot = subRoot;
				}

				// Check if the root is layoutable
				if ((theRoot as FCNode).Node.Frozen)
					continue;

				Layout.LayoutProgress progress = null;
				if (_progress != null)
					progress = new Layout.LayoutProgress(
						this.OnLayoutProgress);

				Layout.TreeLayoutInfo info = 
					new Layout.TreeLayoutInfo();
				info.Direction = (Layout.TreeLayoutDirection)this.Direction;
				info.KeepRootPosition = this.KeepRootPosition;
				info.LevelDistance = this.LevelDistance;
				info.NodeDistance = this.NodeDistance;
				info.StretchFactor = this.StretchFactor;
				info.XGap = this.XGap;
				info.YGap = this.YGap;

				if (_layoutNode != null)
				{
					// remember the old positions
					foreach (FCNode node in subgraph.Nodes)
						node.Node.setData(Constants.OLD_BOUNDS, node.Node.BoundingRect);
				}

				// Arrange the subgraph
				switch (_type)
				{

					case TreeLayoutType.Cascading:
						new Layout.BorderTreeLayout().Arrange(
							theRoot, info, progress);
						break;

					case TreeLayoutType.Centered:
						new Layout.CenterTreeLayout().Arrange(
							theRoot, info, progress);
						break;

					case TreeLayoutType.Radial:
						new Layout.RadialTreeLayout().Arrange(
							theRoot, info, progress);
						break;

				}

				// If the root is not at fixed position, translate the whole tree
				if (!this.KeepRootPosition)
				{
					RectangleF graphBounds = subgraph.GetBounds(false);
					float xToMove = xOffset - graphBounds.X;
					float yToMove = this.YGap - graphBounds.Y;

					foreach (FCNode node in subgraph.Nodes)
					{
						RectangleF nodeBounds = node.Bounds;
						nodeBounds.X += xToMove;
						nodeBounds.Y += yToMove;
						node.Bounds = nodeBounds;
					}

					xOffset += graphBounds.Width + this.XGap;
				}

				if (_layoutNode != null)
				{
					// raise the LayoutNode event for each node
					foreach (FCNode node in subgraph.Nodes)
						_layoutNode(node.Node, (RectangleF)node.Node.getData(Constants.OLD_BOUNDS));
					chart.clearRuntimeData(Constants.OLD_BOUNDS);
				}

				// Update the arrows in this particular subgraph
				ArrayList visitedLinks = new ArrayList();
				ArrayList nodes = new ArrayList();

				nodes.Add(theRoot);

				while (nodes.Count > 0)
				{
					FCNode node = nodes[0] as FCNode;
					nodes.RemoveAt(0);

					if (node.Node.Frozen)
						continue;

					foreach (FCLink link in node.OutLinks)
					{
						if (!visitedLinks.Contains(link))
						{
							visitedLinks.Add(link);

							if (!link.Arrow.IgnoreLayout)
							{
								if (link.Destination == node)
								{
									UpdateArrow(link.Arrow, true);
									nodes.Add(link.Origin);
								}
								else
								{
									UpdateArrow(link.Arrow, false);
									nodes.Add(link.Destination);
								}

								if (_layoutLink != null)
									_layoutLink(link.Arrow);
							}
						}
					}
				}
			}

			chart.Invalidate();
			chart.UndoManager.onEndLayout();

			return true;
		}

		private void OnLayoutProgress(int current, int total)
		{
			_progress(current, total);
		}

		public void UpdateArrow(Arrow a, bool ignoredDirection)
		{
			if (_inc == ArrowAnchor.NoAnchor)
				CalculateAnchors();

			// Temporarily disable routing
			bool route = a.AutoRoute;
			a.SilentAutoRoute = false;

			// Preserve arrow anchoring
			int orgnAnchor = a.OrgnAnchor;
			int destAnchor = a.DestAnchor;

			switch(Type)
			{
				case TreeLayoutType.Cascading:
				{
					switch(ArrowStyle)
					{

						case TreeLayoutArrowType.Cascading2:
						{
							a.Style = MindFusion.FlowChartX.ArrowStyle.Cascading;
							bool v = false;
							if( Direction == TreeLayoutDirection.LeftToRight ||
								Direction == TreeLayoutDirection.RightToLeft)
							{
								if(!(ReversedArrows ^ ignoredDirection))
									v = true;
							}
							else
							{
								if(ReversedArrows ^ ignoredDirection)
									v = true;
							}
							if (v)
								a.CascadeOrientation = MindFusion.FlowChartX.Orientation.Vertical;
							else
								a.CascadeOrientation = MindFusion.FlowChartX.Orientation.Horizontal;

							a.SegmentCount = 2;

							// Adjust the arrow which connects the boxes.
							if (ignoredDirection)
								a.adjustEnds(_inc, _outg);
							else
								a.adjustEnds(_outg, _inc);

							SetAnchors(a, orgnAnchor, destAnchor);

							PointF ptMiddle = a.Points[1];
							PointF p0 = (!(ReversedArrows ^ ignoredDirection) ? a.Points[0] : a.Points[2]);
							PointF p2 = (!(ReversedArrows ^ ignoredDirection) ? a.Points[2] : a.Points[0]);
							if( Direction == TreeLayoutDirection.LeftToRight ||
								Direction == TreeLayoutDirection.RightToLeft)
							{
								ptMiddle.X = p0.X;
								ptMiddle.Y = p2.Y;
							}
							else
							{
								ptMiddle.X = p2.X;
								ptMiddle.Y = p0.Y;
							}
							a.Points[1] = ptMiddle;
							a.updateArrowHeads();
						}
							break;

						case TreeLayoutArrowType.Cascading3:
						{
							a.Style = MindFusion.FlowChartX.ArrowStyle.Cascading;
							a.SegmentCount = 3;

							if( Direction == TreeLayoutDirection.LeftToRight ||
								Direction == TreeLayoutDirection.RightToLeft)
								a.CascadeOrientation = MindFusion.FlowChartX.Orientation.Horizontal;
							else
								a.CascadeOrientation = MindFusion.FlowChartX.Orientation.Vertical;

							// Adjust the arrow which connects the boxes.
							if (ignoredDirection)
								a.adjustEnds(_inc, _outg);
							else
								a.adjustEnds(_outg, _inc);

							SetAnchors(a, orgnAnchor, destAnchor);

							PointF ptMiddle1 = a.Points[1];
							PointF ptMiddle2 = a.Points[2];
							if( Direction == TreeLayoutDirection.LeftToRight ||
								Direction == TreeLayoutDirection.RightToLeft)
							{
								ptMiddle1.X = ptMiddle2.X =
									(a.Points[0].X + a.Points[3].X)/2;
								ptMiddle1.Y = a.Points[0].Y;
								ptMiddle2.Y = a.Points[3].Y;
							}
							else
							{
								ptMiddle1.Y = ptMiddle2.Y =
									(a.Points[0].Y + a.Points[3].Y)/2;
								ptMiddle1.X = a.Points[0].X;
								ptMiddle2.X = a.Points[3].X;
							}

							a.Points[1] = ptMiddle1;
							a.Points[2] = ptMiddle2;
							a.updateArrowHeads();
						}
							break;

						case TreeLayoutArrowType.Rounded:
						{
							a.Style = MindFusion.FlowChartX.ArrowStyle.Bezier;
							a.SegmentCount = 1;

							// Adjust the arrow which connects the boxes.
							if (ignoredDirection)
								a.adjustEnds(_inc, _outg);
							else
								a.adjustEnds(_outg, _inc);

							SetAnchors(a, orgnAnchor, destAnchor);

							PointF ptMiddle = a.Points[1];
							PointF p0 = (!(ReversedArrows ^ ignoredDirection) ? a.Points[0] : a.Points[3]);
							PointF p3 = (!(ReversedArrows ^ ignoredDirection) ? a.Points[3] : a.Points[0]);
							if( Direction == TreeLayoutDirection.LeftToRight ||
								Direction == TreeLayoutDirection.RightToLeft)
							{
								ptMiddle.X = p0.X;
								ptMiddle.Y = p3.Y;
							}
							else
							{
								ptMiddle.X = p3.X;
								ptMiddle.Y = p0.Y;
							}

							a.Points[1] = ptMiddle;
							a.Points[2] = ptMiddle;
							a.updateArrowHeads();
						}
							break;

						case TreeLayoutArrowType.Straight:
						{
							a.Style = MindFusion.FlowChartX.ArrowStyle.Polyline;
							a.SegmentCount = 1;

							// Adjust the arrow which connects the boxes.
							if (ignoredDirection)
								a.adjustEnds(_inc, _outg);
							else
								a.adjustEnds(_outg, _inc);

							SetAnchors(a, orgnAnchor, destAnchor);
						}
							break;

					}
				}
					break;

				case TreeLayoutType.Centered:
				{
					switch(ArrowStyle)
					{

						case TreeLayoutArrowType.Cascading2:
							// Centered tree does not support
							// 2-segmented perpendicular arrows.
							break;

						case TreeLayoutArrowType.Cascading3:
						{
							a.Style = MindFusion.FlowChartX.ArrowStyle.Cascading;
							a.SegmentCount = 3;

							if( Direction == TreeLayoutDirection.LeftToRight ||
								Direction == TreeLayoutDirection.RightToLeft)
								a.CascadeOrientation = MindFusion.FlowChartX.Orientation.Horizontal;
							else
								a.CascadeOrientation = MindFusion.FlowChartX.Orientation.Vertical;

							// Adjust the arrow which connects the boxes.
							if (ignoredDirection)
								a.adjustEnds(_inc, _outg);
							else
								a.adjustEnds(_outg, _inc);

							SetAnchors(a, orgnAnchor, destAnchor);

							PointF ptMiddle1 = a.Points[1];
							PointF ptMiddle2 = a.Points[2];
							if( Direction == TreeLayoutDirection.LeftToRight ||
								Direction == TreeLayoutDirection.RightToLeft)
							{
								ptMiddle1.X = ptMiddle2.X =
									(a.Points[0].X + a.Points[3].X)/2;
								ptMiddle1.Y = a.Points[0].Y;
								ptMiddle2.Y = a.Points[3].Y;
							}
							else
							{
								ptMiddle1.Y = ptMiddle2.Y =
									(a.Points[0].Y + a.Points[3].Y)/2;
								ptMiddle1.X = a.Points[0].X;
								ptMiddle2.X = a.Points[3].X;
							}

							a.Points[1] = ptMiddle1;
							a.Points[2] = ptMiddle2;
							a.updateArrowHeads();
						}
							break;

						case TreeLayoutArrowType.Rounded:
						{
							a.Style = MindFusion.FlowChartX.ArrowStyle.Bezier;
							a.SegmentCount = 1;

							// Adjust the arrow which connects the boxes.
							if (ignoredDirection)
								a.adjustEnds(_inc, _outg);
							else
								a.adjustEnds(_outg, _inc);

							SetAnchors(a, orgnAnchor, destAnchor);

							PointF ptMiddle1 = a.Points[1];
							PointF ptMiddle2 = a.Points[2];
							if( Direction == TreeLayoutDirection.LeftToRight ||
								Direction == TreeLayoutDirection.RightToLeft)
							{
								ptMiddle1.X = a.Points[3].X;
								ptMiddle2.X = a.Points[0].X;
								ptMiddle1.Y = a.Points[0].Y;
								ptMiddle2.Y = a.Points[3].Y;
							}
							else
							{
								ptMiddle1.X = a.Points[0].X;
								ptMiddle2.X = a.Points[3].X;
								ptMiddle1.Y = a.Points[3].Y;
								ptMiddle2.Y = a.Points[0].Y;
							}

							a.Points[1] = ptMiddle1;
							a.Points[2] = ptMiddle2;
							a.updateArrowHeads();
						}
							break;

						case TreeLayoutArrowType.Straight:
						{
							a.Style = MindFusion.FlowChartX.ArrowStyle.Polyline;
							a.SegmentCount = 1;

							// Adjust the arrow which connects the boxes.
							if (ignoredDirection)
								a.adjustEnds(_inc, _outg);
							else
								a.adjustEnds(_outg, _inc);

							SetAnchors(a, orgnAnchor, destAnchor);
						}
							break;

					}
				}
					break;

				case TreeLayoutType.Radial:
				{
					// Ignore the arrow style
					a.arrangePoints(_anchoring);
				}
					break;

			}

			a.SilentAutoRoute = route;
		}

		private void SetAnchors(Arrow arrow, int orgnAnchor, int destAnchor)
		{
			switch (_anchoring)
			{

				case Anchoring.Ignore:
					// Do nothing
					break;

				case Anchoring.Keep:
					// Restore anchors
					if (orgnAnchor >= 0)
					{
						arrow.OrgnAnchor = -1;
						arrow.OrgnAnchor = orgnAnchor;
					}
					if (destAnchor >= 0)
					{
						arrow.DestAnchor = -1;
						arrow.DestAnchor = destAnchor;
					}
					break;

				case Anchoring.Reassign:
				{
					int anchorIndex = 0;

					arrow.Origin.getNearestAnchor(
						arrow.Points[0], arrow, false, ref anchorIndex);
					if (anchorIndex >= 0)
					{
						arrow.OrgnAnchor = -1;
						arrow.OrgnAnchor = anchorIndex;
					}

					arrow.Destination.getNearestAnchor(
						arrow.Points[arrow.Points.Count - 1], arrow, true, ref anchorIndex);
					if (anchorIndex >= 0)
					{
						arrow.DestAnchor = -1;
						arrow.DestAnchor = anchorIndex;
					}
				}
					break;

			}
		}

		private void CalculateAnchors()
		{
			ArrowAnchor inc = ArrowAnchor.BottomCenter;
			ArrowAnchor outg = ArrowAnchor.BottomCenter;

			switch(_type)
			{
				case TreeLayoutType.Cascading:
				{
					switch(Direction)
					{
						case TreeLayoutDirection.TopToBottom:
							inc = ArrowAnchor.TopCenter;
							if(_arrowStyle == TreeLayoutArrowType.Cascading3)
								outg = ArrowAnchor.BottomCenter;
							else
								outg = ArrowAnchor.RightCenter;
							break;
						case TreeLayoutDirection.BottomToTop:
							inc = ArrowAnchor.BottomCenter;
							if(_arrowStyle == TreeLayoutArrowType.Cascading3)
								outg = ArrowAnchor.TopCenter;
							else
								outg = ArrowAnchor.RightCenter;
							break;
						case TreeLayoutDirection.LeftToRight:
							inc = ArrowAnchor.LeftCenter;
							if(_arrowStyle == TreeLayoutArrowType.Cascading3)
								outg = ArrowAnchor.RightCenter;
							else
								outg = ArrowAnchor.BottomCenter;
							break;
						case TreeLayoutDirection.RightToLeft:
							inc = ArrowAnchor.RightCenter;
							if(_arrowStyle == TreeLayoutArrowType.Cascading3)
								outg = ArrowAnchor.LeftCenter;
							else
								outg = ArrowAnchor.BottomCenter;
							break;
					}
				}
					break;
				case TreeLayoutType.Centered:
				{
					switch(Direction)
					{
						case TreeLayoutDirection.TopToBottom:
							inc = ArrowAnchor.TopCenter;
							outg = ArrowAnchor.BottomCenter;
							break;
						case TreeLayoutDirection.BottomToTop:
							inc = ArrowAnchor.BottomCenter;
							outg = ArrowAnchor.TopCenter;
							break;
						case TreeLayoutDirection.LeftToRight:
							inc = ArrowAnchor.LeftCenter;
							outg = ArrowAnchor.RightCenter;
							break;
						case TreeLayoutDirection.RightToLeft:
							inc = ArrowAnchor.RightCenter;
							outg = ArrowAnchor.LeftCenter;
							break;
					}
				}
					break;
			}

			if (_reversedArrows)
			{
				ArrowAnchor temp = outg;
				outg = inc;
				inc = temp;
			}

			// Store the anchors
			_inc = inc;
			_outg = outg;
		}


		/// <summary>
		/// Get or set the node, which will be used
		/// as root for the layouting.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(null)]
		public Node Root
		{
			get { return _root; }
			set { _root = value; }
		}

		/// <summary>
		/// Get or set the type of the tree layout.
		/// </summary>
		[Category("Settings")]
		[Description("The type of the layout.")]
		[DefaultValue(typeof(TreeLayoutType), "Centered")]
		public TreeLayoutType Type
		{
			get { return _type; }
			set { _type = value; }
		}

		/// <summary>
		/// Get or set the type of arrows.
		/// </summary>
		[Category("Settings")]
		[Description("The look of the arrows in the arranged tree.")]
		[DefaultValue(typeof(TreeLayoutArrowType), "Straight")]
		public TreeLayoutArrowType ArrowStyle
		{
			get { return _arrowStyle; }
			set { _arrowStyle = value; }
		}

		[Category("Settings")]
		[Description("The space to leave between adjacent tree levels.")]
		[DefaultValue(10f)]
		public float LevelDistance
		{
			get { return _levelDistance; }
			set { _levelDistance = value; }
		}

		[Category("Settings")]
		[Description("The space to leave between nodes on the same level.")]
		[DefaultValue(10f)]
		public float NodeDistance
		{
			get { return _nodeDistance; }
			set { _nodeDistance = value; }
		}

		[Category("Settings")]
		[Description("A flag specifying whether to keep " +
			 "the root at its original position.")]
		[DefaultValue(false)]
		public bool KeepRootPosition
		{
			get { return _keepRootPosition; }
			set { _keepRootPosition = value; }
		}

		[Category("Settings")]
		[Description("The distance between the leftmost node " +
			 "in the arranged diagram and the left border of the document.")]
		[DefaultValue(5f)]
		public float XGap
		{
			get { return _xGap; }
			set { _xGap = value; }
		}

		[Category("Settings")]
		[Description("The distance between the topmost node " +
			 "in the arranged diagram and the top border of the document.")]
		[DefaultValue(5f)]
		public float YGap
		{
			get { return _yGap; }
			set { _yGap = value; }
		}

		[Category("Settings")]
		[Description("Allows using elliptic outlines for radial layout.")]
		[DefaultValue(1f)]
		public float StretchFactor
		{
			get { return _stretch; }
			set { _stretch = value; }
		}

		[Category("Settings")]
		[Description("The layout direction.")]
		[DefaultValue(typeof(TreeLayoutDirection), "TopToBottom")]
		public TreeLayoutDirection Direction
		{
			get { return _direction; }
			set { _direction = value;}
		}

		[Category("Settings")]
		[Description("Specifies the arrow direction that relates parent to children.")]
		[DefaultValue(false)]
		public bool ReversedArrows
		{
			get { return _reversedArrows; }
			set { _reversedArrows = value; }
		}

		[Category("Settings")]
		[Description("A flag specifying whether the node disposition " +
			 "within groups is kept intact.")]
		[DefaultValue(false)]
		public bool KeepGroupLayout
		{
			get { return _keepGroupLayout; }
			set { _keepGroupLayout = value; }
		}

		/// <summary>
		/// This property is kept hidden because it requires
		/// the root to be set explicitly, which cannot be done
		/// through design editors.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(false)]
		public bool IgnoreArrowDirection
		{
			get { return _ignoreArrowDirection; }
			set { _ignoreArrowDirection = value; }
		}

		[Category("Settings")]
		[Description("Specifies how to align arrows to the anchor points of tree nodes.")]
		[DefaultValue(typeof(Anchoring), "Ignore")]
		public Anchoring Anchoring
		{
			get { return _anchoring; }
			set { _anchoring = value; }
		}

		/// <summary>
		/// This delegate cannot be set via the VS property editor.
		/// TODO: make this an event fired by the layout component 
		/// instead of a simple delegate.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutProgress Progress
		{
			get { return _progress; }
			set { _progress = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutNode LayoutNode
		{
			get { return _layoutNode; }
			set { _layoutNode = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutLink LayoutLink
		{
			get { return _layoutLink; }
			set { _layoutLink = value; }
		}

		private LayoutNode _layoutNode = null;
		private LayoutLink _layoutLink = null;
	
		private ArrowAnchor _inc = ArrowAnchor.NoAnchor;
		private ArrowAnchor _outg = ArrowAnchor.NoAnchor;

		private Node _root = null;
		private bool _keepRootPosition = false;
		private TreeLayoutType _type = TreeLayoutType.Centered;
		private TreeLayoutArrowType _arrowStyle = TreeLayoutArrowType.Straight;
		private TreeLayoutDirection _direction = TreeLayoutDirection.TopToBottom;
		private float _levelDistance = 10;
		private float _nodeDistance = 10;
		private bool _reversedArrows = false;
		private bool _keepGroupLayout = false;
		private bool _ignoreArrowDirection = false;
		private Anchoring _anchoring = Anchoring.Ignore;
		private float _xGap = 5;
		private float _yGap = 5;
		private float _stretch = 1;

		private LayoutProgress _progress = null;
	}
	#endregion

	#region Anneal Layout adoption classes
	[ComVisible(true)]
	public class AnnealLayout : Component, ILayout
	{
		public AnnealLayout()
		{
			distributionFactor = 40000;
			boundaryFactor = 3000;
			arrowLengthFactor = 0.25;
			crossingArrowsCost = 100000;
			nodeArrowDistFactor = 20000;

			_temperature = 40f;
			_temperatureScale = 0.75f;
			_iterationsPerStage = 50;
			_stages = 15;

			_anchoring = Anchoring.Ignore;
			_keepGroupLayout = false;

			layoutArea = RectangleF.Empty;
			widthHeightRatio = 1;
			randomize = true;

			_splitGraph = false;
		}


		public virtual bool Arrange(FlowChart chart)
		{
			chart.UndoManager.onStartLayout("Anneal layout");

			// Build the graph
			FCGraph graph = new FCGraph(chart, _keepGroupLayout);

			// Find the root adapter
			Layout.INode rootNode = null;
			if (_root != null)
			{
				foreach (FCNode node in graph.Nodes)
				{
					if (node.Node == _root)
					{
						rootNode = node;
						break;
					}
				}
			}

			// Split graph to subgraphs
			Layout.IGraph[] subgraphs = null;
			if (_splitGraph)
			{
				subgraphs = Layout.GraphSplitter.Split(
					graph, new FCGraphBuilder(chart, false));
			}
			else
			{
				subgraphs = new Layout.IGraph[] { graph };
			}
			
			// Create the layouter
			Layout.AnnealLayout layout = new Layout.AnnealLayout();

			Layout.LayoutProgress progress = null;
			if (_progress != null)
				progress = new Layout.LayoutProgress(this.OnLayoutProgress);

			Layout.AnnealLayoutInfo info = new Layout.AnnealLayoutInfo();

			info.DistributionFactor = this.DistributionFactor;
			info.BoundaryFactor = this.BoundaryFactor;
			info.EdgeLengthFactor = this.ArrowLengthFactor;
			info.CrossingEdgesCost = this.CrossingArrowsCost;
			info.NodeEdgeDistFactor = this.NodeArrowDistFactor;

			info.IterationsPerStage = this.IterationsPerStage;
			info.Stages = this.Stages;
			info.Temperature = this.InitialTemperature;
			info.TemperatureScale = this.TemperatureScale;

			info.LayoutArea = this.LayoutArea;
			info.WidthHeightRatio = this.WidthHeightRatio;
			info.Randomize = this.Randomize;

			float xOffset = 0;
			foreach (FCGraph subgraph in subgraphs)
			{
				// If a root node is specified and the subgraph
				// does not contain that node, do not arrange
				// the subgraph
				if (rootNode != null)
				{
					if (!subgraph.Nodes.Contains(rootNode))
						continue;
				}

				layout.Arrange(subgraph, info, progress);

				// Translate the whole subgraph
				RectangleF graphBounds = subgraph.GetBounds(false);
				float xToMove = xOffset - graphBounds.X;
				float yToMove = -graphBounds.Y;

				foreach (FCNode node in subgraph.Nodes)
				{
					RectangleF nodeBounds = node.Bounds;
					RectangleF oldBounds = node.Node.BoundingRect;
					nodeBounds.X += xToMove;
					nodeBounds.Y += yToMove;
					node.Bounds = nodeBounds;
					if (_layoutNode != null)
						_layoutNode(node.Node, oldBounds);
				}

				xOffset += graphBounds.Width;

				// Update arrows
				foreach (FCLink link in subgraph.Links)
				{
					Arrow arrow = link.Arrow;

					if (arrow.IgnoreLayout)
						continue;

					// If the arrow being arranged is dynamic,
					// ignore the anchoring flag?
					if (arrow.Dynamic)
						arrow.updatePosFromOrgAndDest(false);
					arrow.arrangePoints(_anchoring);

					if (_layoutLink != null)
						_layoutLink(arrow);
				}
			}

			chart.Invalidate();
			chart.UndoManager.onEndLayout();

			return true;
		}

		private void OnLayoutProgress(int current, int total)
		{
			_progress(current, total);
		}


		/// <summary>
		/// Get or set the object, which will be used
		/// as root for the layouting.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(null)]
		public Node Root
		{
			get { return _root; }
			set { _root = value; }
		}

		[Category("Settings")]
		[Description("A flag specifying whether the node disposition " +
			 "within groups is kept intact.")]
		[DefaultValue(false)]
		public bool KeepGroupLayout
		{
			get { return _keepGroupLayout; }
			set { _keepGroupLayout = value; }
		}

		[Category("Settings")]
		[Description("Specifies how to align arrows to the anchor points of nodes.")]
		[DefaultValue(typeof(Anchoring), "Ignore")]
		public Anchoring Anchoring
		{
			get { return _anchoring; }
			set { _anchoring = value; }
		}

		[Category("Settings")]
		[DefaultValue(40000)]
		public double DistributionFactor
		{
			get { return distributionFactor; }
			set { distributionFactor = value; }
		}

		[Category("Settings")]
		[DefaultValue(3000)]
		public double BoundaryFactor
		{
			get { return boundaryFactor; }
			set { boundaryFactor = value; }
		}

		[Category("Settings")]
		[DefaultValue(0.25)]
		public double ArrowLengthFactor
		{
			get { return arrowLengthFactor; }
			set { arrowLengthFactor = value; }
		}

		[Category("Settings")]
		[DefaultValue(100000)]
		public double CrossingArrowsCost
		{
			get { return crossingArrowsCost; }
			set { crossingArrowsCost = value; }
		}

		[Category("Settings")]
		[DefaultValue(20000)]
		public double NodeArrowDistFactor
		{
			get { return nodeArrowDistFactor; }
			set { nodeArrowDistFactor = value; }
		}

		[Category("Settings")]
		[DefaultValue(40f)]
		public double InitialTemperature
		{
			get { return _temperature; }
			set { _temperature = value; }
		}

		[Category("Settings")]
		[DefaultValue(0.75f)]
		public double TemperatureScale
		{
			get { return _temperatureScale; }
			set { _temperatureScale = value; }
		}

		[Category("Settings")]
		[DefaultValue(50)]
		public int IterationsPerStage
		{
			get { return _iterationsPerStage; }
			set { _iterationsPerStage = value; }
		}

		[Category("Settings")]
		[DefaultValue(15)]
		public int Stages
		{
			get { return _stages; }
			set { _stages = value; }
		}

		/// <summary>
		/// Progress callback delegate.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutProgress Progress
		{
			get { return _progress; }
			set { _progress = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutNode LayoutNode
		{
			get { return _layoutNode; }
			set { _layoutNode = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutLink LayoutLink
		{
			get { return _layoutLink; }
			set { _layoutLink = value; }
		}

		[Browsable(false)]
		public RectangleF LayoutArea
		{
			get { return layoutArea; }
			set { layoutArea = value; }
		}

		[Category("Settings")]
		[DefaultValue(1f)]
		public float WidthHeightRatio
		{
			get { return widthHeightRatio; }
			set { widthHeightRatio = value; }
		}

		[Category("Settings")]
		[DefaultValue(true)]
		public bool Randomize
		{
			get { return randomize; }
			set { randomize = value; }
		}

		[Category("Settings")]
		[Description("Specifies whether to split the graph to its interconnected " +
			 "subgraphs and arrange the subgraphs independently.")]
		[DefaultValue(false)]
		public bool SplitGraph
		{
			get { return _splitGraph; }
			set { _splitGraph = value; }
		}

		private LayoutNode _layoutNode = null;
		private LayoutLink _layoutLink = null;

		private Node _root = null;
		//private bool _keepRootPosition = false;
		private bool _keepGroupLayout;
		private Anchoring _anchoring;

		private double distributionFactor;
		private double boundaryFactor;
		private double arrowLengthFactor;
		private double crossingArrowsCost;
		private double nodeArrowDistFactor;

		private double _temperature;
		private double _temperatureScale;
		private int _iterationsPerStage;
		private int _stages;

		private RectangleF layoutArea;
		private float widthHeightRatio;
		private bool randomize;

		private bool _splitGraph;
	
		private LayoutProgress _progress = null;
	}
	#endregion

	#region Spring Layout adoption classes
	[ComVisible(true)]
	public class SpringLayout : Component, ILayout
	{
		public SpringLayout()
		{
			_keepGroupLayout = false;
			_anchoring = Anchoring.Ignore;
			_iterationCount = 100;
			_nodeDistance = 30;
			_minimizeCrossings = false;
			_enableClusters = false;
		}

		public SpringLayout(int iterationCount,
			float nodeDistance, bool minimizeCrossings)
		{
			_keepGroupLayout = false;
			_anchoring = Anchoring.Ignore;
			_iterationCount = iterationCount;
			_nodeDistance = nodeDistance;
			_minimizeCrossings = minimizeCrossings;
			_enableClusters = false;
		}

		public SpringLayout(
			int iterationCount, float nodeDistance,
			bool minimizeCrossings, bool keepGroupLayout)
		{
			_keepGroupLayout = keepGroupLayout;
			_anchoring = Anchoring.Ignore;
			_iterationCount = iterationCount;
			_nodeDistance = nodeDistance;
			_minimizeCrossings = minimizeCrossings;
			_enableClusters = false;
		}


		public virtual bool Arrange(FlowChart chart)
		{
			chart.UndoManager.onStartLayout("Spring layout");

			// Build the graph
			FCGraph graph = new FCGraph(chart, _keepGroupLayout);

			// Find the root adapter
			Layout.INode rootNode = null;
			if (_root != null)
			{
				foreach (FCNode node in graph.Nodes)
				{
					if (node.Node == _root)
					{
						rootNode = node;
						break;
					}
				}
			}

			// Split graph to subgraphs
			Layout.IGraph[] subgraphs = Layout.GraphSplitter.Split(
				graph, new FCGraphBuilder(chart, false));

			// Create the layouter
			Layout.SpringLayout layout = new Layout.SpringLayout();
			layout.Stretch = _stretch;

			Layout.LayoutProgress progress = null;
			if (_progress != null)
				progress = new Layout.LayoutProgress(this.OnLayoutProgress);

			Layout.SpringLayoutInfo info = new Layout.SpringLayoutInfo();
			info.IterationCount = this.IterationCount;
			info.MinimizeCrossings = this.MinimizeCrossings;
			info.NodeDistance = this.NodeDistance;
			info.EnableClusters = this.EnableClusters;
			info.RepulsionFactor = this.RepulsionFactor;
			info.RndSeed = this.RndSeed;

			float xOffset = 0;
			foreach (FCGraph subgraph in subgraphs)
			{
				// If a root node is specified and the subgraph
				// does not contain that node, do not arrange
				// the subgraph
				if (rootNode != null)
				{
					if (!subgraph.Nodes.Contains(rootNode))
						continue;
				}

				layout.Arrange(subgraph, info, progress);

				// Translate the whole subgraph
				RectangleF graphBounds = subgraph.GetBounds(false);
				float xToMove = xOffset - graphBounds.X;
				float yToMove = -graphBounds.Y;

				foreach (FCNode node in subgraph.Nodes)
				{
					RectangleF nodeBounds = node.Bounds;
					RectangleF oldBounds = node.Node.BoundingRect;
					nodeBounds.X += xToMove;
					nodeBounds.Y += yToMove;
					node.Bounds = nodeBounds;
					if (_layoutNode != null)
						_layoutNode(node.Node, oldBounds);
				}

				xOffset += graphBounds.Width + this.NodeDistance;

				// Update arrows
				foreach (FCLink link in subgraph.Links)
				{
					Arrow arrow = link.Arrow;

					if (arrow.IgnoreLayout)
						continue;

					// If the arrow being arranged is dynamic,
					// ignore the anchoring flag?
					if (arrow.Dynamic)
						arrow.updatePosFromOrgAndDest(false);
					arrow.arrangePoints(_anchoring);

					if (_layoutLink != null)
						_layoutLink(arrow);
				}
			}

			chart.Invalidate();
			chart.UndoManager.onEndLayout();

			return true;
		}

		private void OnLayoutProgress(int current, int total)
		{
			_progress(current, total);
		}


		/// <summary>
		/// Too specific to be displayed in an editor.
		/// </summary>
		[Browsable(false)]
		[DefaultValue((double)85)]
		public double Stretch
		{
			get { return _stretch; }
			set { _stretch = value; }
		}

		[Category("Settings")]
		[Description("A flag specifying whether the node disposition " +
			 "within groups is kept intact.")]
		[DefaultValue(false)]
		public bool KeepGroupLayout
		{
			get { return _keepGroupLayout; }
			set { _keepGroupLayout = value; }
		}

		[Category("Settings")]
		[Description("Specifies how to align arrows to the anchor points of nodes.")]
		[DefaultValue(typeof(Anchoring), "Ignore")]
		public Anchoring Anchoring
		{
			get { return _anchoring; }
			set { _anchoring = value; }
		}

		[Category("Settings")]
		[Description("The number iterations that should be performed.")]
		[DefaultValue(100)]
		public int IterationCount
		{
			get { return _iterationCount; }
			set { _iterationCount = value; }
		}

		[Category("Settings")]
		[Description("The relative distance between adjacent nodes.")]
		[DefaultValue(30f)]
		public float NodeDistance
		{
			get { return _nodeDistance; }
			set { _nodeDistance = value; }
		}

		[Category("Settings")]
		[Description("Enables / disables minimization of arrow crossings.")]
		[DefaultValue(false)]
		public bool MinimizeCrossings
		{
			get { return _minimizeCrossings; }
			set { _minimizeCrossings = value; }
		}

		[Category("Settings")]
		[Description("Enables / disables allocating nodes to distinct clusters.")]
		[DefaultValue(false)]
		public bool EnableClusters
		{
			get { return _enableClusters; }
			set { _enableClusters = value; }
		}

		/// <summary>
		/// Too specific to be displayed in an editor.
		/// </summary>
		[Browsable(false)]
		[DefaultValue((double)2.5)]
		public double RepulsionFactor
		{
			get { return _repulsionFactor; }
			set { _repulsionFactor = value; }
		}

		/// <summary>
		/// Too specific to be displayed in an editor.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(0)]
		public int RndSeed
		{
			get { return _rndSeed; }
			set { _rndSeed = value; }
		}

		/// <summary>
		/// Progress callback delegate.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutProgress Progress
		{
			get { return _progress; }
			set { _progress = value; }
		}

		/// <summary>
		/// Get or set the object, which will be used
		/// as root for the layouting.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(null)]
		public Node Root
		{
			get { return _root; }
			set { _root = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutNode LayoutNode
		{
			get { return _layoutNode; }
			set { _layoutNode = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutLink LayoutLink
		{
			get { return _layoutLink; }
			set { _layoutLink = value; }
		}

		private LayoutNode _layoutNode = null;
		private LayoutLink _layoutLink = null;

		private double _stretch = 85;
		private bool _keepGroupLayout;
		private Anchoring _anchoring;
		private int _iterationCount = 100;
		private float _nodeDistance = 30;
		private bool _minimizeCrossings = false;
		private bool _enableClusters = false;
		private double _repulsionFactor = 2.5;
		private int _rndSeed = 0;
		private Node _root = null;

		private LayoutProgress _progress = null;
	}
	#endregion

	#region Layered Layout adoption classes
	public enum Orientation
	{
		Vertical = Layout.Orientation.Vertical,
		Horizontal = Layout.Orientation.Horizontal
	}

	public enum Direction
	{
		Straight = Layout.Direction.Straight,
		Reversed = Layout.Direction.Reversed
	}

	[ComVisible(true)]
	public class LayeredLayout : Component, ILayout
	{
		public LayeredLayout()
		{
			_keepGroupLayout = false;
			_anchoring = Anchoring.Ignore;
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

		public LayeredLayout(
			Orientation orientation, float layerDistance,
			float nodeDistance, float xGap, float yGap)
		{
			_keepGroupLayout = false;
			_anchoring = Anchoring.Ignore;
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

		public virtual bool Arrange(FlowChart chart)
		{
			chart.UndoManager.onStartLayout("Layered layout");

			// Disable undo to prevent unneeded actions.
			// For example: RouteAllArrows is invoked each time
			// when a new dummy box is created, which is a very
			// expensive operation even with no AutoRoute arrows present.
			bool undo = chart.UndoManager.UndoEnabled;
			chart.UndoManager.enable(false);

			// Build the graph
			FCGraph graph = new FCGraph(chart, _keepGroupLayout);

			// Find the root adapter
			Layout.INode rootNode = null;
			if (_root != null)
			{
				foreach (FCNode node in graph.Nodes)
				{
					if (node.Node == _root)
					{
						rootNode = node;
						break;
					}
				}
			}

			// Split graph to subgraphs
			Layout.IGraph[] subgraphs = Layout.GraphSplitter.Split(
				graph, new FCGraphBuilder(chart, false));

			// Make sure all arrows are polylines with 1 segment.
			// Arrows' AutoRoute flag is also undesired.
			// Note: Process only the arrows belonging to
			// the subgraph, which contains the specified root (if any)
			if (rootNode == null)
			{
				foreach (Arrow arrow in chart.Arrows)
				{
					if (arrow.isReflexive())
						continue;

					arrow.AutoRoute = false;
					arrow.Style = MindFusion.FlowChartX.ArrowStyle.Polyline;
					arrow.SegmentCount = 1;
				}
			}
			else
			{
				foreach (FCGraph subgraph in subgraphs)
				{
					if (subgraph.Nodes.Contains(rootNode))
					{
						foreach (FCLink link in subgraph.Links)
						{
							Arrow arrow = link.Arrow;

							if (arrow.isReflexive())
								continue;

							arrow.AutoRoute = false;
							arrow.Style = MindFusion.FlowChartX.ArrowStyle.Polyline;
							arrow.SegmentCount = 1;
						}

						break;
					}
				}
			}

			// Create the layouter
			Layout.LayeredLayout layout = new Layout.LayeredLayout();

			Layout.LayoutProgress progress = null;
			if (_progress != null)
				progress = new Layout.LayoutProgress(this.OnLayoutProgress);

			Layout.LayeredLayoutInfo info = new Layout.LayeredLayoutInfo();
			info.ArrowsCompactFactor = this.ArrowsCompactFactor;
			info.Direction = (Layout.Direction)this.Direction;
			info.LayerDistance = this.LayerDistance;
			info.NodeDistance = this.NodeDistance;
			info.Orientation = (Layout.Orientation)this.Orientation;
			info.SplitLayers = this.SplitLayers;
			info.XGap = this.XGap;
			info.YGap = this.YGap;
			info.TimeLimit = this.TimeLimit;

			float xOffset = this.XGap;
			foreach (FCGraph subgraph in subgraphs)
			{
				// If a root node is specified and the subgraph
				// does not contain that node, do not arrange
				// the subgraph
				if (rootNode != null)
				{
					if (!subgraph.Nodes.Contains(rootNode))
						continue;
				}

				layout.Arrange(subgraph, info, progress);

				// Translate the whole subgraph
				RectangleF graphBounds = subgraph.GetBounds(true);
				float xToMove = xOffset - graphBounds.X;
				float yToMove = this.YGap - graphBounds.Y;

				foreach (FCNode node in subgraph.Nodes)
				{
					RectangleF nodeBounds = node.Bounds;
					RectangleF oldBounds = node.Node.BoundingRect;
					nodeBounds.X += xToMove;
					nodeBounds.Y += yToMove;
					node.Bounds = nodeBounds;
					if (_layoutNode != null)
						_layoutNode(node.Node, oldBounds);
				}

				// Update arrows' inner points; the end points
				// must have already been offset by the loop above
				foreach (FCLink link in subgraph.Links)
				{
					// Arrows that retain form need not be updated
					if (link.Arrow.RetainForm)
						continue;

					for (int i = 1; i < link.Arrow.Points.Count - 1; i++)
					{
						PointF pt = link.Arrow.Points[i];
						pt.X += xToMove;
						pt.Y += yToMove;
						link.Arrow.Points[i] = pt;
					}
				}

				xOffset += graphBounds.Width + this.XGap;

				// Update arrows
				foreach (FCLink link in subgraph.Links)
				{
					Arrow arrow = link.Arrow;

					if (arrow.IgnoreLayout)
						continue;

					// Update end points
					if (arrow.SegmentCount == 1)
					{
						arrow.arrangePoints(_anchoring);
					}
					else
					{
						int orgnAnchor = arrow.OrgnAnchor;
						int destAnchor = arrow.DestAnchor;

						arrow.Points[0] = arrow.getOrgnLink().getIntersection(
							arrow.Origin.getCenter(), arrow.Points[1]);
						arrow.Points[arrow.Points.Count - 1] = arrow.getDestLink().getIntersection(
							arrow.Points[arrow.Points.Count - 2], arrow.Destination.getCenter());

						switch (_anchoring)
						{

							case Anchoring.Ignore:
								// Do nothing
								break;

							case Anchoring.Keep:
								if (orgnAnchor >= 0)
								{
									arrow.OrgnAnchor = -1;
									arrow.OrgnAnchor = orgnAnchor;
								}
								if (destAnchor >= 0)
								{
									arrow.DestAnchor = -1;
									arrow.DestAnchor = destAnchor;
								}
								break;

							case Anchoring.Reassign:
								arrow.Points[0] =
									arrow.Origin.getNearestAnchor(
									arrow.Points[0], arrow, false, ref orgnAnchor);
								arrow.Points[arrow.Points.Count - 1] =
									arrow.Destination.getNearestAnchor(
									arrow.Points[arrow.Points.Count - 1], arrow, true, ref destAnchor);
								arrow.setOrgnAnchor(orgnAnchor);
								arrow.setDestAnchor(destAnchor);
								break;

						}
					}

					arrow.UpdateFromPoints();

					if (_layoutLink != null)
						_layoutLink(arrow);
				}
			}

			chart.Invalidate();
			chart.UndoManager.enable(undo);
			chart.UndoManager.onEndLayout();

			return true;
		}

		private void OnLayoutProgress(int current, int total)
		{
			_progress(current, total);
		}


		[Category("Settings")]
		[Description("A flag specifying whether the node disposition " +
			 "within groups is kept intact.")]
		[DefaultValue(false)]
		public bool KeepGroupLayout
		{
			get { return _keepGroupLayout; }
			set { _keepGroupLayout = value; }
		}

		[Category("Settings")]
		[Description("Specifies how to align arrows to the anchor points of nodes.")]
		[DefaultValue(typeof(Anchoring), "Ignore")]
		public Anchoring Anchoring
		{
			get { return _anchoring; }
			set { _anchoring = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutProgress Progress
		{
			get { return _progress; }
			set { _progress = value; }
		}

		[Category("Settings")]
		[Description("The orientation of the layout.")]
		[DefaultValue(typeof(Orientation), "Vertical")]
		public Orientation Orientation
		{
			get { return _orientation; }
			set { _orientation = value; }
		}

		[Category("Settings")]
		[Description("The direction of the layout.")]
		[DefaultValue(typeof(Direction), "Straight")]
		public Direction Direction
		{
			get { return _direction; }
			set { _direction = value; }
		}

		[Category("Settings")]
		[Description("The distance between adjacent layers.")]
		[DefaultValue(25f)]
		public float LayerDistance
		{
			get { return _layerDistance; }
			set { _layerDistance = value; }
		}

		[Category("Settings")]
		[Description("The minimal distance between adjacent nodes on the same layer.")]
		[DefaultValue(20f)]
		public float NodeDistance
		{
			get { return _nodeDistance; }
			set { _nodeDistance = value; }
		}

		[Category("Settings")]
		[Description("The distance between the leftmost node " +
			 "in the arranged diagram and the left border of the document.")]
		[DefaultValue(5f)]
		public float XGap
		{
			get { return _xGap; }
			set { _xGap = value; }
		}

		[Category("Settings")]
		[Description("The distance between the topmost node " +
			 "in the arranged diagram and the top border of the document.")]
		[DefaultValue(5f)]
		public float YGap
		{
			get { return _yGap; }
			set { _yGap = value; }
		}

		[Browsable(false)]
		[DefaultValue(false)]
		public bool SplitLayers
		{
			get { return _splitLayers; }
			set { _splitLayers = value; }
		}

		[Category("Settings")]
		[Description("A value indicating how much to compact the outermost arrows.")]
		[DefaultValue(1f)]
		public float ArrowsCompactFactor
		{
			get { return _compactFactor; }
			set { _compactFactor = value; }
		}

		[Category("Settings")]
		[Description("A time-out value for the path-finding part " +
			 "of the layered layout algorithm.")]
		[DefaultValue((long)10000)]
		public long TimeLimit
		{
			get { return _timeLimit; }
			set { _timeLimit = value; }
		}

		/// <summary>
		/// Get or set the object, which will be used
		/// as root for the layouting.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(null)]
		public Node Root
		{
			get { return _root; }
			set { _root = value; }
	
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutNode LayoutNode
		{
			get { return _layoutNode; }
			set { _layoutNode = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutLink LayoutLink
		{
			get { return _layoutLink; }
			set { _layoutLink = value; }
		}


		private LayoutNode _layoutNode = null;
		private LayoutLink _layoutLink = null;

		private bool _keepGroupLayout;
		private Anchoring _anchoring;
		private Orientation _orientation;
		private Direction _direction;
		private float _layerDistance;
		private float _nodeDistance;
		private float _xGap;
		private float _yGap;
		private bool _splitLayers;
		private float _compactFactor;
		private long _timeLimit;
		private Node _root;

		private LayoutProgress _progress = null;
	}
	#endregion

	#region GridLayout adoption classes
	[ComVisible(true)]
	public class GridLayout : Component, ILayout
	{
		public GridLayout()
		{
			_startNode = null;
			_endNode = null;
			_anchoring = Anchoring.Ignore;
			_iterations = 3000;
			_gridSize = 25;
			_xGap = 10;
			_yGap = 10;
			_keepGroupLayout = false;
			_splitGraph = true;
		}

		public virtual bool Arrange(FlowChart chart)
		{
			chart.UndoManager.onStartLayout("Grid layout");

			// Build the graph
			FCGraph graph = new FCGraph(chart, _keepGroupLayout);

			// Find the root adapter
			Layout.INode rootNode = null;
			if (_root != null)
			{
				foreach (FCNode node in graph.Nodes)
				{
					if (node.Node == _root)
					{
						rootNode = node;
						break;
					}
				}
			}

			// Split graph to subgraphs
			Layout.IGraph[] subgraphs = null;

			if (_splitGraph)
			{
				subgraphs = Layout.GraphSplitter.Split(
					graph, new FCGraphBuilder(chart, false));
			}
			else
			{
				subgraphs = new Layout.IGraph[] { graph };
			}

			// Create the layouter
			Layout.GridLayout layout = new Layout.GridLayout();

			Layout.LayoutProgress progress = null;
			if (_progress != null)
				progress = new Layout.LayoutProgress(this.OnLayoutProgress);

			Layout.GridLayoutInfo info = new Layout.GridLayoutInfo();
			info.GridSize = this.GridSize;
			info.Iterations = this.Iterations;
			info.XGap = this.XGap;
			info.YGap = this.YGap;
			info.RndSeed = this.RndSeed;

			float xOffset = XGap;
			foreach (FCGraph subgraph in subgraphs)
			{
				// If a root node is specified and the subgraph
				// does not contain that node, do not arrange
				// the subgraph
				if (rootNode != null)
				{
					if (!subgraph.Nodes.Contains(rootNode))
						continue;
				}

				// Set the start and end nodes
				info.StartNode = null;
				info.EndNode = null;
				foreach (FCNode node in subgraph.Nodes)
				{
					if (node.Node == _startNode)
						info.StartNode = node;
					if (node.Node == _endNode)
						info.EndNode = node;
				}

				// Ensure both start and end nodes are set
				if (info.StartNode == null || info.EndNode == null)
				{
					info.StartNode = null;
					info.EndNode = null;
				}

				layout.Arrange(subgraph, info, progress);

				// Translate the whole subgraph
				RectangleF graphBounds = subgraph.GetBounds(false);
				float xToMove = xOffset - graphBounds.X;
				float yToMove = info.YGap - graphBounds.Y;

				foreach (FCNode node in subgraph.Nodes)
				{
					RectangleF nodeBounds = node.Bounds;
					RectangleF oldBounds = node.Node.BoundingRect;
					nodeBounds.X += xToMove;
					nodeBounds.Y += yToMove;
					node.Bounds = nodeBounds;
					if (_layoutNode != null)
						_layoutNode(node.Node, oldBounds);
				}

				xOffset += graphBounds.Width + info.GridSize;

				// Update arrows
				foreach (FCLink link in subgraph.Links)
				{
					if (link.Arrow.IgnoreLayout)
						continue;

					link.Arrow.arrangePoints(_anchoring);
					if (_layoutLink != null)
						_layoutLink(link.Arrow);
				}
			}

			chart.RouteAllArrows();
			chart.Invalidate();
			chart.UndoManager.onEndLayout();

			return true;
		}

		private void OnLayoutProgress(int current, int total)
		{
			_progress(current, total);
		}


		[Browsable(false)]
		[DefaultValue(null)]
		public Node StartNode
		{
			get { return _startNode; }
			set { _startNode = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public Node EndNode
		{
			get { return _endNode; }
			set { _endNode = value; }
		}

		[Category("Settings")]
		[Description("Specifies how to align arrows to the anchor points of nodes.")]
		[DefaultValue(typeof(Anchoring), "Ignore")]
		public Anchoring Anchoring
		{
			get { return _anchoring; }
			set { _anchoring = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutProgress Progress
		{
			get { return _progress; }
			set { _progress = value; }
		}

		[Category("Settings")]
		[Description("The number iterations that should be performed.")]
		[DefaultValue(3000)]
		public int Iterations
		{
			get { return _iterations; }
			set { _iterations = value; }
		}

		[Category("Settings")]
		[Description("The size of the virtual grid where nodes are placed.")]
		[DefaultValue(25f)]
		public float GridSize
		{
			get { return _gridSize; }
			set { _gridSize = value; }
		}

		[Category("Settings")]
		[Description("The distance between the leftmost node " +
			 "in the arranged diagram and the left border of the document.")]
		[DefaultValue(10f)]
		public float XGap
		{
			get { return _xGap; }
			set { _xGap = value; }
		}

		[Category("Settings")]
		[Description("The distance between the topmost node " +
			 "in the arranged diagram and the top border of the document.")]
		[DefaultValue(10f)]
		public float YGap
		{
			get { return _yGap; }
			set { _yGap = value; }
		}

		[Browsable(false)]
		[DefaultValue(0)]
		public int RndSeed
		{
			get { return _rndSeed; }
			set { _rndSeed = value; }
		}

		[Category("Settings")]
		[Description("A flag specifying whether the node disposition " +
			 "within groups is kept intact.")]
		[DefaultValue(false)]
		public bool KeepGroupLayout
		{
			get { return _keepGroupLayout; }
			set { _keepGroupLayout = value; }
		}

		[Category("Settings")]
		[Description("Specifies whether to split the graph to its interconnected " +
			 "subgraphs and arrange the subgraphs independently.")]
		[DefaultValue(false)]
		public bool SplitGraph
		{
			get { return _splitGraph; }
			set { _splitGraph = value; }
		}

		/// <summary>
		/// Get or set the object, which will be used
		/// as root for the layouting.
		/// </summary>
		[Browsable(false)]
		[DefaultValue(null)]
		public Node Root
		{
			get { return _root; }
			set { _root = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutNode LayoutNode
		{
			get { return _layoutNode; }
			set { _layoutNode = value; }
		}

		[Browsable(false)]
		[DefaultValue(null)]
		public LayoutLink LayoutLink
		{
			get { return _layoutLink; }
			set { _layoutLink = value; }
		}


		private LayoutNode _layoutNode = null;
		private LayoutLink _layoutLink = null;

		private bool _keepGroupLayout;
		private Node _startNode;
		private Node _endNode;
		private Anchoring _anchoring;
		private int _iterations;
		private float _gridSize;
		private float _xGap;
		private float _yGap;
		private int _rndSeed = 0;
		private Node _root;
		private bool _splitGraph;

		private LayoutProgress _progress = null;
	}
	#endregion
}
