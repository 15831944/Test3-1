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
	/// 
	/// </summary>
	internal abstract class Link : IPersists
	{
		public Link(Arrow arrow, bool incm)
		{
			this.arrow = arrow;
			this.incoming = incm;
			ptRelative = new PointF(0, 0);
		}

		internal Link()
		{
		}

		internal void setArrow(Arrow arrow)
		{
			this.arrow = arrow;
		}

		internal virtual void onDelete() {}

		internal virtual void freeResources()
		{
			arrow = null;
		}

		internal PointF PtRelative
		{
			get { return ptRelative; }
			set { ptRelative = value; }
		}

		protected Arrow arrow;
		protected PointF ptRelative;
		protected bool incoming;
		
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
			writer.Write(ptRelative.X);
			writer.Write(ptRelative.Y);
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
				ptRelative = new PointF(rx, ry);
			}

			incoming = reader.ReadBoolean();

			if (ctx.FileVersion > 21)
			{
				// in format revision 22 these become float:
				float rx = reader.ReadSingle();
				float ry = reader.ReadSingle();
				ptRelative = new PointF(rx, ry);
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
	}
}