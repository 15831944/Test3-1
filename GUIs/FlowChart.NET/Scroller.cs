// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Resources;
using MindFusion.FlowChartX.Commands;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	internal sealed class Scroller : MindFusion.FlowChartX.Manipulator
	{
		public Scroller(ChartObject obj) : base(obj)
		{
		}

		internal override void onDelete()
		{
			base.onDelete();
		}

		internal override void draw(Graphics g)
		{
			Table table = (Table)obj;

			// do not paint the scroller if the table is too small
			RectangleF tableRect = table.getBoundingRect();
			float minSize = Constants.getMinObjSize(table.fcParent.MeasureUnit);
			if (tableRect.Width < minSize || tableRect.Height < minSize ||
				tableRect.Height < table.CaptionHeight)
				return;

			// determine the scroller rectangle coordinates
			float scrollerWth = Constants.getScrollerWdt(table.fcParent.MeasureUnit);
			RectangleF scrollerRect = tableRect;
			scrollerRect.Height = table.CaptionHeight;
			scrollerRect.X = scrollerRect.Right - scrollerWth;
			scrollerRect.Width = scrollerWth;

			// setup the graphics objects needed to paint the scroller
			System.Drawing.Brush enabledBkgr =
				new System.Drawing.SolidBrush(Color.FromArgb(220, 220, 220));
			System.Drawing.Brush disabledBkgr =
				new System.Drawing.SolidBrush(Color.FromArgb(150, 150, 150));
			System.Drawing.Brush arrowInterior =
				new System.Drawing.SolidBrush(Color.Black);
			System.Drawing.Pen pen = new System.Drawing.Pen(Color.Black, 0);

			// paint scroller background
			RectangleF arrowRect = scrollerRect;
			arrowRect.Height = scrollerRect.Height / 2;
			if (table.canScrollUp())
				g.FillRectangle(enabledBkgr, arrowRect);
			else
				g.FillRectangle(disabledBkgr, arrowRect);
			arrowRect.Y += arrowRect.Height;
			if (table.canScrollDown())
				g.FillRectangle(enabledBkgr, arrowRect);
			else
				g.FillRectangle(disabledBkgr, arrowRect);

			// paint scroller frame
			g.DrawRectangle(pen, scrollerRect.X, scrollerRect.Y,
				scrollerRect.Width, scrollerRect.Height);

			// paint up arrow
			GraphicsPath arrow = new GraphicsPath();
			float pixel = 2 * Constants.getPixel(table.fcParent.MeasureUnit);
			arrow.AddLines(new PointF[] {
				new PointF(scrollerRect.X + scrollerRect.Width / 2,
					scrollerRect.Y + pixel),
				new PointF(scrollerRect.X + scrollerRect.Width - pixel,
					scrollerRect.Y + scrollerRect.Height / 2 - pixel),
				new PointF(scrollerRect.X + pixel,
					scrollerRect.Y + scrollerRect.Height / 2 - pixel)
			});
			arrow.CloseFigure();
			g.FillPath(arrowInterior, arrow);
			arrow.Dispose();

			// paint down arrow
			arrow = new GraphicsPath();
			arrow.AddLines(new PointF[] {
				new PointF(scrollerRect.X + scrollerRect.Width / 2,
					scrollerRect.Y + scrollerRect.Height - pixel),
				new PointF(scrollerRect.X + pixel,
					scrollerRect.Y + scrollerRect.Height / 2 + pixel),
				new PointF(scrollerRect.X + scrollerRect.Width - pixel,
					scrollerRect.Y + scrollerRect.Height / 2 + pixel)
			});
			arrow.CloseFigure();
			g.FillPath(arrowInterior, arrow);
			arrow.Dispose();

			// clean up
			pen.Dispose();
			arrowInterior.Dispose();
			disabledBkgr.Dispose();
			enabledBkgr.Dispose();
		}

		internal override void addToRepaintRect(ref RectangleF rect)
		{
		}

		internal override bool hitTest(PointF point)
		{
			RectangleF rc = obj.getBoundingRect();
			Table tbl = (Table)obj;
			float h = tbl.CaptionHeight;

			// up-arrow
			if (point.Y > rc.Top && point.Y < rc.Top + h/2 && 
				point.X > rc.Right - Constants.getScrollerWdt(
				obj.fcParent.MeasureUnit) && point.X < rc.Right)
			{
				if (tbl.canScrollUp())
				{
					ScrollTableCmd cmd = new ScrollTableCmd(tbl);
					tbl.scrollUp();
					tbl.fcParent.UndoManager.executeCommand(cmd);
				}
				return true;
			}

			// down-arrow
			if (point.Y >= rc.Top + h/2 && point.Y < rc.Top + h && 
				point.X > rc.Right - Constants.getScrollerWdt(
				obj.fcParent.MeasureUnit) && point.X < rc.Right)
			{
				if (tbl.canScrollDown())
				{
					ScrollTableCmd cmd = new ScrollTableCmd(tbl);
					tbl.scrollDown();
					tbl.fcParent.UndoManager.executeCommand(cmd);
				}
				return true;
			}

			return false;
		}

		internal override bool ptInManipulator(PointF pt)
		{
			RectangleF rcTable = obj.getBoundingRect();
			Table tbl = (Table)obj;

			RectangleF rcScr = new RectangleF(
				rcTable.Right - Constants.getScrollerWdt(obj.fcParent.MeasureUnit), rcTable.Top,
				Constants.getScrollerWdt(obj.fcParent.MeasureUnit), tbl.CaptionHeight);

			if (Utilities.pointInRect(pt, rcScr))
				return true;

			return false;
		}

		internal override bool supportClipping()
		{
			return true;
		}
	}
}