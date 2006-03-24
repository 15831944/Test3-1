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
	// Tree layout classes

	#region enumerations
	[Access(AccessType.Public)]
	internal enum TreeLayoutDirection
	{
		TopToBottom,
		BottomToTop,
		LeftToRight,
		RightToLeft
	}
	#endregion

	#region class TreeLayoutInfo
	[Access(AccessType.Public)]
	internal class TreeLayoutInfo
	{
		public TreeLayoutInfo() {}

		public TreeLayoutInfo(
			TreeLayoutDirection direction,
			float levelDistance, float nodeDistance,
			bool keepRootPosition, float xGap, float yGap)
		{
			_direction = direction;
			_levelDistance = levelDistance;
			_nodeDistance = nodeDistance;
			_keepRootPosition = keepRootPosition;
			_xGap = xGap;
			_yGap = yGap;
		}


		/// <summary>
		/// Get or set the layout direction.
		/// </summary>
		public TreeLayoutDirection Direction
		{
			get { return _direction; }
			set { _direction = value; }
		}

		/// <summary>
		/// Get or set the minimal distance between two adjacent tree levels.
		/// </summary>
		public float LevelDistance
		{
			get { return _levelDistance; }
			set { _levelDistance = value; }
		}

		/// <summary>
		/// Get or set the minimal distance between to adjacent nodes in the same level.
		/// </summary>
		public float NodeDistance
		{
			get { return _nodeDistance; }
			set { _nodeDistance = value; }
		}

		/// <summary>
		/// Get or set a flag, indicating whether the layout manager should
		/// preserve the position of the root node.
		/// </summary>
		public bool KeepRootPosition
		{
			get { return _keepRootPosition; }
			set { _keepRootPosition = value; }
		}

		/// <summary>
		/// Get or set the gap between the leftmost box
		/// and the left bound of the document.
		/// </summary>
		public float XGap
		{
			get { return _xGap; }
			set { _xGap = value; }
		}

		/// <summary>
		/// Get or set the gap between the topmost box
		/// and the top bound of the document.
		/// </summary>
		public float YGap
		{
			get { return _yGap; }
			set { _yGap = value; }
		}

		/// <summary>
		/// Gets or sets a value indicating the
		/// stretch factor in radial layouts.
		/// </summary>
		public float StretchFactor
		{
			get { return _stretch; }
			set { _stretch = value; }
		}


		private TreeLayoutDirection _direction = TreeLayoutDirection.TopToBottom;

		private float _levelDistance = 10;
		private float _nodeDistance = 10;

		private bool _keepRootPosition = false;

		private float _xGap = 5;
		private float _yGap = 5;

		private float _stretch = 1;
	}
	#endregion

	#region class BorderTreeLayout
	/// <summary>
	/// A tree layout that perform bordered arrangement.
	/// </summary>
	[Access(AccessType.Public)]
	internal class BorderTreeLayout
	{
		public bool Arrange(INode rootNode, TreeLayoutInfo info)
		{
			return Arrange(rootNode, info, null);
		}

		public bool Arrange(INode rootNode,
			TreeLayoutInfo info, LayoutProgress progress)
		{
			// Preserve the root position.
			RectangleF rcRoot = rootNode.Bounds;

			_tree = new Tree();
			if(!_tree.Build(rootNode))
				return false;

			if(info.KeepRootPosition)
			{
				_x = rcRoot.Left;
				_y = rcRoot.Top;
			}
			else
			{
				_x = info.XGap;
				_y = info.YGap;
			}

			_total = 0;
			_current = 0;
			_progress = progress;

			// Update progress
			MethodCallVisitor counter = new MethodCallVisitor(
				new VisitOperation(this.Counter));
			_tree.WalkTree(_tree.Root, counter);

			// Make sure total is within reasonable bounds
			float factor = _total / 100;
			if (factor > 1)
			{
				_factor = (int)Math.Floor((double)factor);
				_total = _total / _factor + 1;
			}

			if (progress != null)
				progress(_current++, _total);

			// Arrange the tree.
			RArrange(_tree.Root, info);

			// Offset levels...

			// Calculate the size of each level.
			int i, j;
			float sz;
			ArrayList levelSizes = new ArrayList();
			for(i = 0; i < _tree.TreeLevels.Count; i++)
			{
				ArrayList treeLevel = (ArrayList)_tree.TreeLevels[i];

				sz = 0;
				for(j = 0; j < treeLevel.Count; j++)
				{
					if( info.Direction == TreeLayoutDirection.LeftToRight ||
						info.Direction == TreeLayoutDirection.RightToLeft)
						sz = Math.Max(sz, ((TreeNode)treeLevel[j]).Bounds.Width);
					else
						sz = Math.Max(sz, ((TreeNode)treeLevel[j]).Bounds.Height);
				}

				levelSizes.Add(sz);
			}

			// Perform the offseting.
			float off = 0;
			if(info.KeepRootPosition)
			{
				if(info.Direction == TreeLayoutDirection.BottomToTop)
					off = rcRoot.Top;
				if(info.Direction == TreeLayoutDirection.RightToLeft)
					off = rcRoot.Left;
					
			}
			else
			{
				// Calculate the space needed.
				float size = 0;
				for(int l = 1; l < levelSizes.Count; l++)
				{
					size += (float)levelSizes[l];
					size += info.LevelDistance;
				}

				if( info.Direction == TreeLayoutDirection.LeftToRight ||
					info.Direction == TreeLayoutDirection.RightToLeft)
					size += info.XGap;
				else
					size += info.YGap;

				if( info.Direction == TreeLayoutDirection.RightToLeft ||
					info.Direction == TreeLayoutDirection.BottomToTop)
					off = size;
			}

			for(i = 0; i < _tree.TreeLevels.Count; i++)
			{
				ArrayList treeLevel = (ArrayList)_tree.TreeLevels[i];
				for(j = 0; j < treeLevel.Count; j++)
				{
					RectangleF rc = ((TreeNode)treeLevel[j]).Bounds;

					if( info.Direction == TreeLayoutDirection.TopToBottom ||
						info.Direction == TreeLayoutDirection.BottomToTop)
						rc.Offset(0, off);
					else
						rc.Offset(off, 0);

					((TreeNode)treeLevel[j]).Bounds = rc;
				}

				switch(info.Direction)
				{
					case TreeLayoutDirection.LeftToRight:
					case TreeLayoutDirection.TopToBottom:
						off += info.LevelDistance + (float)levelSizes[i];
						break;
					case TreeLayoutDirection.BottomToTop:
					case TreeLayoutDirection.RightToLeft:
						off += -(info.LevelDistance + (float)levelSizes[i]);
						break;
				}

			}

			// Apply the arrangement
			MethodCallVisitor visitor = 
				new MethodCallVisitor(new VisitOperation(this.Apply));
			_tree.WalkTree(_tree.Root, visitor);

			// Update progress
			if (progress != null)
				progress(_total, _total);

			return true;
		}

		private void Counter(TreeNode node, MethodCallVisitor visitor)
		{
			_total++;
		}

		private void RArrange(TreeNode node, TreeLayoutInfo info)
		{
			// Update progress
			if (_progress != null)
			{
				if (_current % _factor == 0)
					_progress(_current / _factor, _total);
				_current++;
			}

			// Calculate the new node rectangle.
			RectangleF rc;
			rc = new RectangleF(_x, _y, node.Bounds.Width, node.Bounds.Height);

			if(info.Direction == TreeLayoutDirection.RightToLeft)
			{
				// Make the node right-aligned with the root.
				RectangleF rcRoot = _tree.Root.Bounds;
				float s = rc.Width;
				rc = new RectangleF(rcRoot.Right - s, rc.Top, rc.Width, rc.Height);
			}
			if(info.Direction == TreeLayoutDirection.BottomToTop)
			{
				// Make the node bottom aligned with the root.
				RectangleF rcRoot = _tree.Root.Bounds;
				float s = rc.Height;
				rc = new RectangleF(rc.Left, rcRoot.Bottom - s, rc.Width, rc.Height);
			}

			// Set the new node rectangle.
			node.Bounds = rc;

			switch(info.Direction)
			{
				case TreeLayoutDirection.TopToBottom:
				case TreeLayoutDirection.BottomToTop:
					_x += rc.Width + info.NodeDistance;
					break;
				case TreeLayoutDirection.LeftToRight:
				case TreeLayoutDirection.RightToLeft:
					_y += rc.Height + info.NodeDistance;
					break;
			}

			// Reposition the child nodes.
			foreach(TreeNode child in node.Children)
				RArrange(child, info);
		}

		private void Apply(TreeNode node, MethodCallVisitor visitor)
		{
			node.Node.Bounds = node.Bounds;
		}

		/// <summary>
		/// Temporary variable to store x-values in recursive algorithms.
		/// </summary>
		private float _x;
		/// <summary>
		/// Temporary variable to store y-values in recursive algorithms.
		/// </summary>
		private float _y;

		private Tree _tree = null;

		private LayoutProgress _progress;

		private int _current;
		private int _total;
		private int _factor = 1;
	}
	#endregion

	#region class CenterTreeLayout
	[Access(AccessType.Public)]
	internal class CenterTreeLayout
	{
		public bool Arrange(INode rootNode, TreeLayoutInfo info)
		{
			return Arrange(rootNode, info, null);
		}

		public bool Arrange(INode rootNode,
			TreeLayoutInfo info, LayoutProgress progress)
		{
			// Preserve the root position.
			RectangleF rcRoot = rootNode.Bounds;

			_tree = new Tree();
			if(!_tree.Build(rootNode))
				return false;

			int i, j;
			float sz, s = 0;

			// Calculate the size of each level.
			ArrayList levelSizes = new ArrayList();
			for(i = 0; i < _tree.TreeLevels.Count; i++)
			{
				ArrayList treeLevel = (ArrayList)_tree.TreeLevels[i];

				sz = 0;
				for(j = 0; j < treeLevel.Count; j++)
				{
					if( info.Direction == TreeLayoutDirection.LeftToRight ||
						info.Direction == TreeLayoutDirection.RightToLeft)
						sz = Math.Max(sz, ((TreeNode)treeLevel[j]).Bounds.Width);
					else
						sz = Math.Max(sz, ((TreeNode)treeLevel[j]).Bounds.Height);
				}

				levelSizes.Add(sz);
			}

			// Arrange the bottommost level nodes at equal distance.
			ArrayList lastLevel = (ArrayList)_tree.TreeLevels[_tree.TreeLevels.Count - 1];
			for(i = 0; i < lastLevel.Count; i++)
			{
				TreeNode node = (TreeNode)lastLevel[i];
				RectangleF rc = node.Bounds;

				if( info.Direction == TreeLayoutDirection.LeftToRight ||
					info.Direction == TreeLayoutDirection.RightToLeft)
				{
					rc.Offset(0, s);
					s += rc.Height + info.NodeDistance;
				}
				else
				{
					rc.Offset(s, 0);
					s += rc.Width + info.NodeDistance;
				}

				node.Bounds = rc;
			}

			// Update progress
			int total = _tree.TreeLevels.Count;
			int current = 0;
			if (progress != null)
				progress(current++, total);

			// Skip the bottommost level of nodes during arrangement cycle.
			for(i = _tree.TreeLevels.Count - 2; i >= 0; i--)
			{
				// Update progress
				if (progress != null)
					progress(current++, total);

				ArrayList treeLevel = (ArrayList)_tree.TreeLevels[i];
				for(j = 0; j < treeLevel.Count; j++)
				{
					TreeNode node = (TreeNode)treeLevel[j];

					// Make this node in the center of its children.
					if(node.Children.Count > 0)
					{
						ArrayList children = node.Children;
						RectangleF rcTotal = ((TreeNode)children[0]).Bounds; // First node's rectangle.
						for(int c = 1; c < children.Count; c++)
							rcTotal = Utilities.UnionRects(rcTotal,
								((TreeNode)children[c]).Bounds);

						RectangleF rc = node.Bounds;
						if( info.Direction == TreeLayoutDirection.LeftToRight ||
							info.Direction == TreeLayoutDirection.RightToLeft)
						{
							rc.Offset(0,
								(rcTotal.Top + rcTotal.Bottom)/2 - rc.Height/2);
						}
						else
						{
							rc.Offset(
								(rcTotal.Left + rcTotal.Right)/2 - rc.Width/2, 0);
						}
						node.Bounds = rc;
					}
				}

				// Check if there is a need to offset branches.
				for(j = 0; j < treeLevel.Count - 1; j++)
				{
					for(int k = j; k >= 0; k--)
					{
						float dist = BranchDistance(info, (TreeNode)treeLevel[k], (TreeNode)treeLevel[j + 1]);

						if(dist < info.NodeDistance)
						{
							if( info.Direction == TreeLayoutDirection.LeftToRight ||
								info.Direction == TreeLayoutDirection.RightToLeft)
								OffsetBranch((TreeNode)treeLevel[j + 1], 0, info.NodeDistance - dist);
							else
								OffsetBranch((TreeNode)treeLevel[j + 1], info.NodeDistance - dist, 0);
						}
					}
				}
			}

			// Arrange the levels.
			float mh = 0;
			s = 0;

			int iStart;
			int iEnd;
			int iGrow;

			if( info.Direction == TreeLayoutDirection.LeftToRight ||
				info.Direction == TreeLayoutDirection.TopToBottom)
			{
				iStart = 0;
				iEnd = _tree.TreeLevels.Count;
				iGrow = +1;
			}
			else
			{
				iStart = _tree.TreeLevels.Count - 1;
				iEnd = -1;
				iGrow = -1;
			}

			for(i = iStart;
				(info.Direction == TreeLayoutDirection.LeftToRight ||
				info.Direction == TreeLayoutDirection.TopToBottom) ? i < iEnd : i > iEnd;
				i += iGrow)
			{
				mh = 0;

				ArrayList treeLevel = (ArrayList)_tree.TreeLevels[i];
				for(j = 0; j < treeLevel.Count; j++)
				{
					TreeNode node = (TreeNode)treeLevel[j];
					float o = 0;

					if( info.Direction == TreeLayoutDirection.LeftToRight ||
						info.Direction == TreeLayoutDirection.RightToLeft)
					{
						o = node.Bounds.Width;

						float xoff = 0;
						if(info.Direction == TreeLayoutDirection.RightToLeft)
							xoff = (float)levelSizes[i] - o;

						node.Bounds = new RectangleF(
							s + xoff + info.XGap,
							node.Bounds.Top + info.YGap,
							node.Bounds.Width, node.Bounds.Height);
					}
					else
					{
						o = node.Bounds.Height;

						float yoff = 0;
						if(info.Direction == TreeLayoutDirection.BottomToTop)
							yoff = (float)levelSizes[i] - o;

						node.Bounds = new RectangleF(
							node.Bounds.Left + info.XGap,
							s + yoff + info.YGap,
							node.Bounds.Width, node.Bounds.Height);
					}

					mh = Math.Max(mh, o);
				}

				s += mh + info.LevelDistance;
			}

			// Offset the tree as though the root has persisted its position.
			if(info.KeepRootPosition)
			{
				RectangleF rcNewRoot = _tree.Root.Bounds;
				float xoff = rcRoot.Left - rcNewRoot.Left;
				float yoff = rcRoot.Top - rcNewRoot.Top;

				OffsetBranch(_tree.Root, xoff, yoff);
			}

			// Apply the arrangement
			MethodCallVisitor visitor = 
				new MethodCallVisitor(new VisitOperation(this.Apply));
			_tree.WalkTree(_tree.Root, visitor);

			// Update progress
			if (progress != null)
				progress(total, total);

			return true;
		}

		/// <summary>
		/// Calculate the distance between the branches, whose
		/// root nodes are specified as arguments.
		/// </summary>
		/// <param name="node1">The root node of the first branch.</param>
		/// <param name="node2">The root note of the second branch.</param>
		/// <returns>The distance between the branches.</returns>
		private float BranchDistance(TreeLayoutInfo layoutInfo, TreeNode node1, TreeNode node2)
		{
			float dist = 0;

			// Add the node offset to the distance received.
			if( layoutInfo.Direction == TreeLayoutDirection.LeftToRight ||
				layoutInfo.Direction == TreeLayoutDirection.RightToLeft)
				dist = node2.Bounds.Top - node1.Bounds.Bottom;
			else
				dist = node2.Bounds.Left - node1.Bounds.Right;

			int i;
			ArrayList tmp1 = new ArrayList();
			ArrayList tmp2 = new ArrayList();
			tmp1.Add(node1);
			tmp2.Add(node2);

			while(tmp1.Count > 0 && tmp2.Count > 0)
			{
				// Get the next level for both branches.
				int t1 = tmp1.Count;
				for(i = 0; i < t1; i++)
				{
					foreach(TreeNode child in ((TreeNode)tmp1[i]).Children)
						tmp1.Add(child);
				}
				while(t1-- > 0)
					tmp1.RemoveAt(0);

				int t2 = tmp2.Count;
				for(i = 0; i < t2; i++)
				{
					foreach(TreeNode child in ((TreeNode)tmp2[i]).Children)
						tmp2.Add(child);
				}
				while(t2-- > 0)
					tmp2.RemoveAt(0);

				float r = 0;
				for(i = 0; i < tmp1.Count; i++)
				{
					if( layoutInfo.Direction == TreeLayoutDirection.LeftToRight ||
						layoutInfo.Direction == TreeLayoutDirection.RightToLeft)
						r = Math.Max(r, ((TreeNode)tmp1[i]).Bounds.Bottom);
					else
						r = Math.Max(r, ((TreeNode)tmp1[i]).Bounds.Right);
				}
				float l = float.MaxValue;
				for(i = 0; i < tmp2.Count; i++)
				{
					if( layoutInfo.Direction == TreeLayoutDirection.LeftToRight ||
						layoutInfo.Direction == TreeLayoutDirection.RightToLeft)
						l = Math.Min(l, ((TreeNode)tmp2[i]).Bounds.Top);
					else
						l = Math.Min(l, ((TreeNode)tmp2[i]).Bounds.Left);
				}

				if( (l - r < dist) &&
					(tmp1.Count > 0 && tmp2.Count > 0))
					dist = l - r;
			}

			return dist;
		}

		/// <summary>
		/// Offset a branch of the tree with the specified values.
		/// </summary>
		/// <param name="tiFrom">The root node of the branch to offset.</param>
		/// <param name="xoff">The x-axis offset.</param>
		/// <param name="yoff">The y-axis offset.</param>
		private void OffsetBranch(TreeNode nodeFrom, float xoff, float yoff)
		{
			MethodCallVisitor visitor = 
				new MethodCallVisitor(new VisitOperation(this.Offset));
			visitor.SetData(_XOffsetData, xoff);
			visitor.SetData(_YOffsetData, yoff);
			_tree.WalkTree(nodeFrom, visitor);
		}

		private const int _XOffsetData = 0;
		private const int _YOffsetData = 1;

		private void Apply(TreeNode node, MethodCallVisitor visitor)
		{
			node.Node.Bounds = node.Bounds;
		}

		private void Offset(TreeNode node, MethodCallVisitor visitor)
		{
			float xOff = (float)visitor.GetData(_XOffsetData);
			float yOff = (float)visitor.GetData(_YOffsetData);
			RectangleF rc = node.Bounds;
			rc.Offset(xOff, yOff);
			node.Bounds = rc;
		}

		private Tree _tree = null;
	}
	#endregion

	#region class RadialTreeLayout
	[Access(AccessType.Public)]
	internal class RadialTreeLayout
	{
		public bool Arrange(INode rootNode, TreeLayoutInfo info)
		{
			return Arrange(rootNode, info, null);
		}

		public bool Arrange(INode rootNode,
			TreeLayoutInfo info, LayoutProgress progress)
		{
			// Preserve the root position.
			RectangleF rcRoot = rootNode.Bounds;

			_tree = new Tree();
			if(!_tree.Build(rootNode))
				return false;

			// Place the root at (0, 0)
			TreeNode root = _tree.Root;
			root.Center = new PointF(0, 0);

			// Set root's bisectors
			double deg = 0;
			switch(info.Direction)
			{
				case TreeLayoutDirection.TopToBottom:
					deg = 270;
					break;
				case TreeLayoutDirection.BottomToTop:
					deg = 90;
					break;
				case TreeLayoutDirection.LeftToRight:
					deg = 180;
					break;
				case TreeLayoutDirection.RightToLeft:
					deg = 0;
					break;
			}
			root.SetData(_LeftBisectorData, deg);
			root.SetData(_RightBisectorData, deg + 360);

			// Update progress
			int total = _tree.TreeLevels.Count;
			int current = 0;
			if (progress != null)
				progress(current++, total);

			// Iterate through all levels and arrange them
			for(int l = 0; l < _tree.TreeLevels.Count - 1; l++)
			{
				// Update progress
				if (progress != null)
					progress(current++, total);
				ArrangeLevel(l, info);
			}

			// Offset the whole tree structure
			float xOff = 0;
			float yOff = 0;
			MethodCallVisitor visitor;
			if(info.KeepRootPosition)
			{
				xOff = rcRoot.Left - root.Bounds.Left;
				yOff = rcRoot.Top - root.Bounds.Top;
			}
			else
			{
				// Calculate the bounding rect of the entire tree's
				visitor = new MethodCallVisitor(new VisitOperation(this.CalcBounds));
				_tree.WalkTree(root, visitor);
				RectangleF rc = (RectangleF)visitor.GetData(_CalcBoundsData);

				xOff = -rc.Left + info.XGap;
				yOff = -rc.Top + info.YGap;
			}
			visitor = new MethodCallVisitor(new VisitOperation(this.Offset));
			visitor.SetData(_XOffsetData, xOff);
			visitor.SetData(_YOffsetData, yOff);
			_tree.WalkTree(root, visitor);

			// Apply the stretch factor
			visitor = new MethodCallVisitor(new VisitOperation(this.Stretch));
			visitor.SetData(_StretchFactor, info.StretchFactor);
			if (info.KeepRootPosition)
				visitor.SetData(_StretchCenter, _tree.Root.Center);
			else
				visitor.SetData(_StretchCenter, new PointF(info.XGap, info.YGap));
			_tree.WalkTree(root, visitor);

			// Apply the arrangement
			visitor = new MethodCallVisitor(new VisitOperation(this.Apply));
			_tree.WalkTree(_tree.Root, visitor);

			// Update progress
			if (progress != null)
				progress(total, total);

			return true;
		}

		private void ArrangeLevel(int parentLevel, TreeLayoutInfo info)
		{
			// Calculate radiuses
			double rad = CalcRadius(parentLevel + 1, info);
			double nrad = CalcRadius(parentLevel + 2, info);
			double angle = 90 - Math.Asin(rad / nrad) * 180 / Math.PI;

			ArrayList parents = (ArrayList)_tree.TreeLevels[parentLevel];

			foreach(TreeNode parent in parents)
			{
				ArrayList children = parent.Children;
				if(children.Count == 0)
					continue;

				// Acquire parent's bisectors
				double lbis = (double)parent.GetData(_LeftBisectorData);
				double rbis = (double)parent.GetData(_RightBisectorData);
				double dist;
				double deg;

				// Calculate the angular distance between child nodes
				if(parentLevel != 0)
				{
					if(children.Count == 1)
					{
						dist = 0;
						deg = (double)parent.GetData(_AngleData);
					}
					else
					{
						dist = (rbis - lbis) / children.Count;
						deg = lbis + dist / 2;
					}
				}
				else
				{
					dist = (rbis - lbis) / children.Count;
					deg = lbis;
				}


				// Place children on their appropriate positions
				foreach(TreeNode child in children)
				{
					double x = rad * Math.Cos(Math.PI * deg / 180);
					double y = -rad * Math.Sin(Math.PI * deg / 180);

					child.Center = new PointF((float)x, (float)y);

					// Store the angle in the runtime data
					child.SetData(_AngleData, deg);

					deg += dist;
				}
			}

			if(parentLevel == _tree.TreeLevels.Count - 1)
				return;

			// Calculate the bisector limits
			// for all nodes just arranged
			for(int p = 0; p < parents.Count; p++)
			{
				TreeNode parent = (TreeNode)parents[p];

				for(int i = 0; i < parent.Children.Count; i++)
				{
					TreeNode n1 = null, n3 = null;
					TreeNode n2 = (TreeNode)parent.Children[i];
					ArrayList level = (ArrayList)_tree.TreeLevels[parentLevel + 1];
					int i2 = level.IndexOf(n2);
					int i1 = i2 - 1;
					int i3 = i2 + 1;

					if(n2.Children.Count == 0)
						continue;

					// Search backwards for sibling with children
					bool turned1 = false, turned2 = false;
					while(true)
					{
						if(i1 < 0)
						{
							i1 = level.Count - 1;
							turned1 = true;
						}

						n1 = (TreeNode)level[i1--];
						if(n1.Children.Count == 0)
							continue;

						break;
					}
					// Search forwards for sibling with children
					while(true)
					{
						if(i3 >= level.Count)
						{
							i3 = 0;
							turned2 = true;
						}

						n3 = (TreeNode)level[i3++];
						if(n3.Children.Count == 0)
							continue;

						break;
					}

					double a1, a2, a3;
					double b1, b2;
					double t1, t2;

					// Calculate the bisectors
					a1 = (double)n1.GetData(_AngleData);
					a2 = (double)n2.GetData(_AngleData);
					a3 = (double)n3.GetData(_AngleData);
					if(turned1)
						a1 -= 360;
					if(turned2)
						a3 += 360;

					b1 = (a1 + a2) / 2;
					b2 = (a2 + a3) / 2;

					if(nrad > 0)
					{
						// Calculate tangents
						t1 = a2 - angle;
						t2 = a2 + angle;

						if(parent.Children.Count == 1 && parentLevel == 0)
						{
							b1 = t1;
							b2 = t2;
						}
						else
						{
							b1 = Math.Max(b1, t1);
							b2 = Math.Min(b2, t2);
						}

						if(n1 == n2 && i != 0)
							b1 = t1;
						if(n2 == n3)
							b2 = t2;

						b1 += 0.001;
						b2 -= 0.001;
					}

					n2.SetData(_LeftBisectorData, Math.Min(b1, b2));
					n2.SetData(_RightBisectorData, Math.Max(b1, b2));
				}
			}
		}

		private double CalcRadius(int level, TreeLayoutInfo info)
		{
			if(level >= _tree.TreeLevels.Count)
				return info.LevelDistance;
			if(level - 1 < 0)
				return 0;

			// Calculate the distance between (level - 1)-th and level-th circles
			double maxRad1 = 0, maxRad2 = 0;
			RectangleF rc;

			foreach(TreeNode node in (ArrayList)_tree.TreeLevels[level - 1])
			{
				rc = node.Bounds;
				double maxSide = (double)Math.Max(rc.Width, rc.Height);
				double radius = maxSide / 2;

				maxRad1 = Math.Max(maxRad1, radius);
			}
			foreach(TreeNode node in (ArrayList)_tree.TreeLevels[level])
			{
				rc = node.Bounds;
				double maxSide = (double)Math.Max(rc.Width, rc.Height);
				double radius = maxSide / 2;

				maxRad2 = Math.Max(maxRad2, radius);
			}

			return maxRad1 + maxRad2 + info.LevelDistance + 
				CalcRadius(level - 1, info);
		}

		private void Apply(TreeNode node, MethodCallVisitor visitor)
		{
			node.Node.Bounds = node.Bounds;
		}

		private void CalcBounds(TreeNode node, MethodCallVisitor visitor)
		{
			object data = visitor.GetData(_CalcBoundsData);
			RectangleF rc;
			if(data == null)
				rc = node.Bounds;
			else
				rc = Utilities.UnionRects((RectangleF)data, node.Bounds);
			visitor.SetData(_CalcBoundsData, rc);
		}

		private void Offset(TreeNode node, MethodCallVisitor visitor)
		{
			float xOff = (float)visitor.GetData(_XOffsetData);
			float yOff = (float)visitor.GetData(_YOffsetData);
			RectangleF rc = node.Bounds;
			rc.Offset(xOff, yOff);
			node.Bounds = rc;
		}

		private void Stretch(TreeNode node, MethodCallVisitor visitor)
		{
			float factor = (float)visitor.GetData(_StretchFactor);
			PointF center = (PointF)visitor.GetData(_StretchCenter);
			PointF pos = node.Center;

			// Stretch in X direction
			float xOff = (pos.X - center.X) * factor;

			node.Center = new PointF(center.X + xOff, pos.Y);
		}


		private const int _LeftBisectorData = 0;
		private const int _RightBisectorData = 1;
		private const int _AngleData = 2;
		private const int _CalcBoundsData = 0;
		private const int _XOffsetData = 1;
		private const int _YOffsetData = 2;
		private const int _StretchFactor = 0;
		private const int _StretchCenter = 1;

		private Tree _tree = null;
	}
	#endregion
}
