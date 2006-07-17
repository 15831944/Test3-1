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
	/// Summary description for TableLink.
	/// </summary>
	internal class TableLink : Link
	{
		public TableLink(Table table, Arrow arrow, bool incoming, int row)
			: base(arrow, incoming)
		{
			this.table = table;
			this.row = row;
		}

		internal TableLink()
		{
		}

		internal override void freeResources()
		{
			base.freeResources();
			table = null;
		}
		
		private Table table;
		int row;

		internal int getRow() { return row; }

		internal override Node getNode()
		{
			return table;
		}

		internal override bool linkChanges(ChartObject obj, PointF pt)
		{
			if (table != obj) return true;
			if (row != table.rowFromPt(pt)) return true;

			return false;
		}

		internal override bool sameNode(Link otherLink)
		{
			ChartObject obj = otherLink.getNode();

			if(obj == null)
				return false;
			else
				return (obj == table);
		}


		internal override PointF getIntersection(PointF pt1, PointF pt2)
		{
			if (row != -1)
				return table.getRowIntersection(row, pt1, pt2);
			else
				return table.getIntersection(pt1, pt2);
		}

		internal override PointF getInitialPt()
		{
			if(row != -1)
				return table.getRowCenter(row);
			else
				return table.getCenter();
		}

		internal override RectangleF getNodeRect(bool rotated)
		{
			return table.getBoundingRect();
		}


		internal override void addArrowToObj()
		{
			if (row == -1)
			{
				if (incoming)
					table.addIncomingArrow(arrow);
				else
					table.addOutgoingArrow(arrow);

				return;
			}

			if (incoming)
				table.addIncomingArrow(row, arrow);
			else
				table.addOutgoingArrow(row, arrow);
		}

		internal override void removeArrowFromObj()
		{
			if (row == -1)
			{
				if (incoming)
				{
					if (table.IncomingArrows.Contains(arrow))
						table.removeIncomingArrow(arrow);
				}
				else
				{
					if (table.OutgoingArrows.Contains(arrow))
						table.removeOutgoingArrow(arrow);
				}

				return;
			}

			if (incoming)
				table.removeIncomingArrow(row, arrow);
			else
				table.removeOutgoingArrow(row, arrow);
		}

		
		internal override void saveEndRelative()
		{
			PointF pt;

			if (incoming)
				pt = arrow.getEndPt();
			else
				pt = arrow.getStartPt();

			if (row != -1)
				relativePosition = Utilities.getRectPtPercent(pt, table.getRowRect(row));
			else
				relativePosition = Utilities.getRectPtPercent(pt, table.getBoundingRect());
		}

		internal override PointF getEndPoint()
		{
			RectangleF rc = table.getRowRect(row);
			PointF ptp = relativePosition;

			if (row != -1)
				rc = table.getRowRect(row);
			else
				rc = table.getBoundingRect();

			// use anchor point position if available
			if (row != -1)
			{
				AnchorPattern rowap = table.getRowAnchorPattern(row);
				if (rowap != null)
				{
					// check DestAnchor for incoming arrow
					if (incoming && arrow.DestAnchor != -1)
					{
						AnchorPoint ap = rowap.Points[arrow.DestAnchor];
						ptp = new PointF(ap.X, ap.Y);
						if (ap.Column != -1)
							rc = table.getSpannedCellRect(row, ap.Column);
					}

					// check OrgnAnchor for outgoing arrow
					if (!incoming && arrow.OrgnAnchor != -1)
					{
						AnchorPoint ap = rowap.Points[arrow.OrgnAnchor];
						ptp = new PointF(ap.X, ap.Y);
						if (ap.Column != -1)
							rc = table.getSpannedCellRect(row, ap.Column);
					}
				}
			}

			// calculate point coordinates
			PointF pt = Utilities.rectPtFromPercent(ptp, rc);

			// stay within table boundaries
			RectangleF rcTable = table.getBoundingRect();
			if (pt.Y > rcTable.Bottom) pt.Y = rcTable.Bottom;
			if (pt.X > rcTable.Right) pt.X = rcTable.Right;

			return pt;
		}

		internal override bool canMoveLink(PointF pt)
		{
			return table.containsPoint(pt);
		}


		internal void updateRow(int row)
		{
			this.row = row;
		}

		public override int getClassId()
		{
			return 7;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);
			ctx.saveReference(this, table, 2);
			writer.Write(row);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);
			ctx.loadReference(this);
			row = reader.ReadInt32();

			if (ctx.FileVersion < 6)
			{
				// fix the mess from old versions
				if (relativePosition.X == -1) relativePosition.X = 0;
				if (relativePosition.X == 1) relativePosition.X = 100;
				relativePosition.Y = 50;
			}
		}

		public override void setReference(int refId, IPersists obj)
		{
			base.setReference(refId,obj);

			if (refId == 2)
				table = (Table)obj;
		}

		internal override PointF getAnchoredPt(ArrowAnchor anchor)
		{
			if (row == -1)
				return table.getAnchor(anchor);

			if (anchor == ArrowAnchor.LeftCenter ||
				anchor == ArrowAnchor.TopCenter)
				relativePosition.X = 0;
			else
				relativePosition.X = 100;
			relativePosition.Y = 50;

			return getEndPoint();
		}

		internal override PointF getAnchoredPt(ArrowAnchor anchor, bool useRotation)
		{
			return table.getAnchor(anchor, useRotation);
		}

		/// <summary>
		/// determines whether the end of an arrow should be aligned to the
		/// point where the node contour is intersected by the line
		/// connecting the centers of the connected nodes
		/// </summary>
		/// <returns>true to align the arrow end; otherwise false</returns>
		internal override bool calcIntrs()
		{
			if (table.RowAnchorPattern != null) return false;
			if (table.AnchorPattern != null) return false;
			foreach (Table.Row row in table.Rows)
				if (row.AnchorPattern != null) return false;

			return true;
		}

		internal override PointF getAnchor(PointF pt,
			Arrow arrow, bool incm, ref int idx)
		{
			return table.getNearestAnchor(row, pt, arrow, incm, ref idx);
		}

		internal override PointF getAnchorPos(int anchorIdx)
		{
			if (row == -1)
			{
				RectangleF nodeRect = getNodeRect(false);
				if (getNode().AnchorPattern != null)
				{
					AnchorPoint ap = getNode().AnchorPattern.Points[anchorIdx];
					return ap.getPos(nodeRect);
				}
				else
				{
					return getInitialPt();
				}
			}

			AnchorPattern rowap = table.getRowAnchorPattern(row);
			RectangleF rowRect = table.getRowRect(row);
			if (rowap != null)
			{
				AnchorPoint ap = rowap.Points[anchorIdx];
				return ap.getPos(rowRect);
			}
			else
			{
				return getInitialPt();
			}
		}

		internal override bool getAnchorPos(int anchorIdx, ref PointF point)
		{
			if (row == -1)
				return base.getAnchorPos(anchorIdx, ref point);

			AnchorPattern rowap = table.getRowAnchorPattern(row);
			if (rowap != null && anchorIdx >= 0 &&
				anchorIdx < rowap.Points.Count)
			{
				AnchorPoint ap = rowap.Points[anchorIdx];

				RectangleF targetRect = (ap.Column == -1) ?
					table.getRowRect(row) : table.getSpannedCellRect(row, ap.Column);
				point = ap.getPos(targetRect);

				return true;
			}

			return false;
		}
	}
}
