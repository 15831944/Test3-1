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
using MindFusion.Geometry.Geometry2D;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	/// 
	internal class BoxLink : Link
	{
		public BoxLink(Box box, Arrow arrow, bool incoming) :
			base(arrow, incoming)
		{
			this.box = box;
		}

		internal BoxLink()
		{
		}

		internal override void onDelete() {}

		internal override void freeResources()
		{
			base.freeResources();
			box = null;
		}

		internal override bool sameNode(Link otherLink)
		{
			ChartObject obj = otherLink.getNode();

			return (obj != null && obj == box);
		}

		internal override bool linkChanges(ChartObject obj, PointF pt)
		{
			if (box != obj) return true;

			return false;
		}

		internal override Node getNode()
		{
			return box;
		}

		internal override PointF getIntersection(PointF pt1, PointF pt2)
		{
			return box.getIntersection(pt1, pt2);
		}

		internal override RectangleF getNodeRect(bool rotated)
		{
			if (rotated)
				return box.getRotatedBounds();

			return box.getBoundingRect();
		}

		internal override void addArrowToObj()
		{
			if (incoming)
				box.addIncomingArrow(arrow);
			else
				box.addOutgoingArrow(arrow);
		}

		internal override void removeArrowFromObj()
		{
			if (incoming)
				box.removeIncomingArrow(arrow);
			else
				box.removeOutgoingArrow(arrow);
		}

		internal override void saveEndRelative()
		{
			if (incoming)
				ptRelative = box.getBoxPtPercent(arrow.getEndPt());
			else
				ptRelative = box.getBoxPtPercent(arrow.getStartPt());
		}

		internal override PointF getEndPoint()
		{
			return box.pointFromPercent(ptRelative);
		}

		internal override PointF getInitialPt()
		{
			PointF point = box.getCenter();

			if (box.Style == BoxStyle.Shape && !box.containsPoint(point))
			{
				PointList points = box.getPolygon();
				if (points != null && points.Count > 2)
				{
					Polygon poly = new Polygon(points);
					return poly.GetInternalPoint();
				}
			}

			return point;
		}

		internal override bool canMoveLink(PointF pt)
		{
			return box.containsPoint(pt);
		}

		/// <summary>
		/// determines whether the end of an arrow should be aligned to the
		/// point where the node contour is intersected by the line
		/// connecting the centers of the connected nodes
		/// </summary>
		/// <returns>true to align the arrow end; otherwise false</returns>
		internal override bool calcIntrs()
		{
			return box.AnchorPattern == null;
		}

		private Box box;

		public override int getClassId()
		{
			return 6;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);
			ctx.saveReference(this, box, 2);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);
			ctx.loadReference(this);
		}

		public override void setReference(int refId, IPersists obj)
		{
			base.setReference(refId,obj);

			if (refId == 2)
				box = (Box)obj;
		}

		internal override PointF getAnchoredPt(ArrowAnchor anchor)
		{
			return box.getAnchor(anchor);
		}

		internal override PointF getAnchoredPt(ArrowAnchor anchor, bool useRotation)
		{
			return box.getAnchor(anchor, useRotation);
		}
	}
}