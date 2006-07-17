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
using MindFusion.FlowChartX.Visitors;
using System.Windows.Forms;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Abstract class representing diagram nodes. Implemented by Box and Table classes.
	/// </summary>
	public class DummyNode : Node
	{
		public DummyNode(FlowChart parent) : base(parent)
		{
		}

		public override void Draw(System.Drawing.Graphics g, bool shadow)
		{
		}

		internal override void drawSelHandles(Graphics g, Color color)
		{
		}

		internal override bool containsPoint(System.Drawing.PointF pt)
		{
			return false;
		}

		public override bool HitTestHandle(System.Drawing.PointF pt, ref int handle)
		{
			return false;
		}

		internal override Link createLink(Arrow arrow, PointF pt, bool incoming)
		{
			return new DummyLink(arrow, incoming, pt);
		}

		// In-place editing
		internal override string getTextToEdit()
		{
			return "";
		}

		internal override void setEditedText(string newText)
		{
		}

		internal override RectangleF getEditRect()
		{
			return getBoundingRect();
		}

		protected override void onRectUpdate()
		{
		}

		internal override PointF getIntersection(PointF ptOrg, PointF ptEnd)
		{
			return ptOrg;
		}

		internal override Cursor getCannotDropCursor()
		{
			return null;
		}

		internal override Cursor getCanDropCursor()
		{
			return null;
		}

		public override ItemType getType()
		{
			return ItemType.Dummy;
		}

	}	// class DummyNode

}	// namespace MindFusion.FlowChartX