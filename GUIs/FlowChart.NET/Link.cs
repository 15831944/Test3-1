// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.IO;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// This class represent a connection between an edge and a node in a graph. It decouples
	/// the Arrow class from the Node class, allowing edges to connect different types of nodes
	/// without knowing anything of their implementation details. Each Arrow objects contains
	/// two Link instances describing the connections at both ends of the arrow.
	/// </summary>
	internal abstract class Link : IPersists
	{
		#region initialization and clean up

		/// <summary>
		/// Initializes a new instance of the Link class.
		/// </summary>
		/// <param name="arrow">The arrow whose connection to a node is managed by this Link.</param>
		/// <param name="incoming">Specifies which end of the arrow is represented by this Link.</param>
		public Link(Arrow arrow, bool incoming)
		{
			this.arrow = arrow;
			this.incoming = incoming;
			this.relativePosition = new PointF(0, 0);
		}

		/// <summary>
		/// Used internally by the control to implement serialization.
		/// </summary>
		internal Link()
		{
		}

		/// <summary>
		/// Called when the Arrow instance is completely removed from the diagram and
		/// there aren't any commands in the undo/redo history referring to that arrow.
		/// </summary>
		internal virtual void freeResources()
		{
			arrow = null;
		}

		#endregion

		internal PointF RelativePosition
		{
			get { return relativePosition; }
			set { relativePosition = value; }
		}

		internal abstract bool sameNode(Link otherLink);
		internal abstract bool linkChanges(ChartObject obj, PointF pt);
		internal abstract Node getNode();

		internal bool objsIntersect(Link link)
		{
			return Node.nodesIntersect(this.getNode(), link.getNode());
		}

		internal abstract PointF getIntersection(PointF pt1, PointF pt2);
		internal abstract RectangleF getNodeRect(bool rotated);

		internal abstract void addArrowToObj();
		internal abstract void removeArrowFromObj();
		internal abstract void saveEndRelative();
		internal abstract PointF getEndPoint();

		internal abstract PointF getInitialPt();
		internal abstract bool canMoveLink(PointF pt);

		/// <summary>
		/// determines whether the end of an arrow should be aligned to the
		/// point where the node contour is intersected by the line
		/// connecting the centers of the connected nodes
		/// </summary>
		/// <returns>true to align the arrow end; otherwise false</returns>
		internal virtual bool calcIntrs() { return true; }

		public abstract int getClassId();

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			ctx.saveReference(this, arrow, 1);
			writer.Write(incoming);

			// in format revision 22 these become float:
			writer.Write(relativePosition.X);
			writer.Write(relativePosition.Y);
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			ctx.loadReference(this);

			if (ctx.FileVersion <= 21)
			{
				// not used any more
				float x = (float)reader.ReadDouble();
				float y = (float)reader.ReadDouble();
				int rx = reader.ReadInt32();
				int ry = reader.ReadInt32();
				relativePosition = new PointF(rx, ry);
			}

			incoming = reader.ReadBoolean();

			if (ctx.FileVersion > 21)
			{
				// in format revision 22 these become float:
				float rx = reader.ReadSingle();
				float ry = reader.ReadSingle();
				relativePosition = new PointF(rx, ry);
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			if (refId == 1)
				arrow = (Arrow)obj;
		}

		internal abstract PointF getAnchoredPt(ArrowAnchor anchor);

		internal abstract PointF getAnchoredPt(ArrowAnchor anchor, bool useRotation);

		internal virtual PointF getAnchor(PointF pt,
			Arrow arrow, bool incm, ref int idx)
		{
			return getNode().getAnchor(pt, arrow, incm, ref idx);
		}

		internal virtual PointF getAnchorPos(int anchorIdx)
		{
			RectangleF nodeRect = getNodeRect(false);
			Node node = getNode();
			if (node.AnchorPattern != null)
			{
				AnchorPoint ap = node.AnchorPattern.Points[anchorIdx];
				PointF point = ap.getPos(nodeRect);
				if (node.rotation() != 0)
					point = Utilities.rotatePointAt(point, node.getCenter(), node.rotation());
				return point;
			}
			else
			{
				return getInitialPt();
			}
		}

		internal virtual bool getAnchorPos(int anchorIdx, ref PointF point)
		{
			RectangleF nodeRect = getNodeRect(false);
			Node node = getNode();

			if (node.AnchorPattern != null && anchorIdx >= 0 &&
				anchorIdx < node.anchorPattern.Points.Count)
			{
				AnchorPoint ap = node.AnchorPattern.Points[anchorIdx];
				point = ap.getPos(nodeRect);
				if (node.rotation() != 0)
					point = Utilities.rotatePointAt(point, node.getCenter(), node.rotation());
				return true;
			}

			return false;
		}

		protected Arrow arrow;
		protected bool incoming;
		protected PointF relativePosition;
	}
}