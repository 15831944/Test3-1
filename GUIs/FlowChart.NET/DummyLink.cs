// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Runtime.Serialization;
using System.IO;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Allows unconnected arrows.
	/// </summary>
	/// 
	[Serializable]
	internal class DummyLink : Link
	{
		public DummyLink(Arrow arrow, bool incoming, PointF point) :
			base(arrow, incoming)
		{
			this.point = point;
		}

		internal override bool sameNode(Link otherLink)
		{
			return false;
		}

		internal override bool linkChanges(ChartObject obj, PointF pt)
		{
			return true;
		}

		internal override Node getNode() { return arrow.fcParent.Dummy; }

		internal override PointF getIntersection(PointF pt1, PointF pt2)
		{
			return pt1;
		}

		internal override RectangleF getNodeRect(bool rotated)
		{
			return new RectangleF(point, new SizeF(0.001f, 0.001f));
		}

		private PointF point;

		internal override void addArrowToObj()
		{
		}

		internal override void removeArrowFromObj()
		{
		}

		internal override void saveEndRelative()
		{
			if (incoming)
			{
				// last point is unconnected
				point = arrow.ControlPoints[arrow.ControlPoints.Count - 1];
			}
			else
			{
				// first point is unconnected
				point = arrow.ControlPoints[0];
			}
		}

		internal override PointF getEndPoint()
		{
			return point;
		}

		internal override PointF getInitialPt()
		{
			return point;
		}

		internal override bool canMoveLink(PointF pt)
		{
			return true;
		}

		/// <summary>
		/// determines whether the end of an arrow should be aligned to the
		/// point where the node contour is intersected by the line
		/// connecting the centers of the connected nodes
		/// </summary>
		/// <returns>true to align the arrow end; otherwise false</returns>
		internal override bool calcIntrs() { return false; }

		public override int getClassId()
		{
			return 42;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);
			writer.Write((double)point.X);
			writer.Write((double)point.Y);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);
			float x = (float)reader.ReadDouble();
			float y = (float)reader.ReadDouble();
			point = new PointF(x, y);
		}

		internal override PointF getAnchoredPt(ArrowAnchor anchor)
		{
			return point;
		}

		internal override PointF getAnchoredPt(ArrowAnchor anchor, bool useRotation)
		{
			return point;
		}
	}
}