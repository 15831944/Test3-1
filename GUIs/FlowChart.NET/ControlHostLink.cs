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
	internal class ControlHostLink : Link
	{
		public ControlHostLink(ControlHost host, Arrow arrow, bool incoming) :
			base(arrow, incoming)
		{
			this.host = host;
		}

		internal ControlHostLink()
		{
		}

		internal override void freeResources()
		{
			base.freeResources();
			host = null;
		}

		internal override bool sameNode(Link otherLink)
		{
			ChartObject obj = otherLink.getNode();

			return (obj != null && obj == host);
		}

		internal override bool linkChanges(ChartObject obj, PointF pt)
		{
			if (host != obj) return true;

			return false;
		}

		internal override Node getNode()
		{
			return host;
		}

		internal override PointF getIntersection(PointF pt1, PointF pt2)
		{
			return host.getIntersection(pt1, pt2);
		}

		internal override RectangleF getNodeRect(bool rotated)
		{
			return host.getBoundingRect();
		}

		internal override void addArrowToObj()
		{
			if (incoming)
				host.addIncomingArrow(arrow);
			else
				host.addOutgoingArrow(arrow);
		}

		internal override void removeArrowFromObj()
		{
			if (incoming)
				host.removeIncomingArrow(arrow);
			else
				host.removeOutgoingArrow(arrow);
		}

		internal override void saveEndRelative()
		{
			if (incoming)
				relativePosition = host.getHostPtPercent(arrow.getEndPt());
			else
				relativePosition = host.getHostPtPercent(arrow.getStartPt());
		}

		internal override PointF getEndPoint()
		{
			return host.hostPtFromPercent(relativePosition);
		}

		internal override PointF getInitialPt()
		{
			return host.getCenter();
		}

		internal override bool canMoveLink(PointF pt)
		{
			return host.containsPoint(pt);
		}

		/// <summary>
		/// determines whether the end of an arrow should be aligned to the
		/// point where the node contour is intersected by the line
		/// connecting the centers of the connected nodes
		/// </summary>
		/// <returns>true to align the arrow end; otherwise false</returns>
		internal override bool calcIntrs()
		{
			return host.AnchorPattern == null;
		}


		public override int getClassId()
		{
			return 40;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);
			ctx.saveReference(this, host, 2);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);
			ctx.loadReference(this);
		}

		public override void setReference(int refId, IPersists obj)
		{
			base.setReference(refId, obj);

			if (refId == 2)
				host = (ControlHost)obj;
		}

		internal override PointF getAnchoredPt(ArrowAnchor anchor)
		{
			return host.getAnchor(anchor);
		}

		internal override PointF getAnchoredPt(ArrowAnchor anchor, bool useRotation)
		{
			return host.getAnchor(anchor, useRotation);
		}

	
		private ControlHost host;
	}
}
