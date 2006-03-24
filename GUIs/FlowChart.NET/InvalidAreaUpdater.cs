// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;

namespace MindFusion.FlowChartX.Visitors
{
	/// <summary>
	/// 
	/// </summary>
	internal class InvalidAreaUpdater : CollectionVisitor
	{
		internal InvalidAreaUpdater(RectangleF rc, bool cn)
		{
			invalidArea = rc;
			connections = cn;
		}

		internal override void accept(ChartObject obj)
		{
			RectangleF rcObj = obj.getRepaintRect(connections);
			invalidArea = Utilities.unionRects(invalidArea, rcObj);
		}

		internal RectangleF getInvalidRect() { return invalidArea; }

		private RectangleF invalidArea;
		private bool connections;
	}


	internal class GroupBoundsVisitor : CollectionVisitor
	{
		internal GroupBoundsVisitor(RectangleF rc, RectangleF rrc)
		{
			_bounds = rc;
			_rotatedBounds = rrc;
		}

		internal override void accept(ChartObject obj)
		{
			if (obj is Node)
			{
				_bounds = Utilities.unionRects(
					_bounds, obj.getBoundingRect());
				_rotatedBounds = Utilities.unionRects(
					_rotatedBounds, obj.getRotatedBounds());
			}
		}


		internal RectangleF Bounds
		{
			get { return _bounds; }
		}

		internal RectangleF RotatedBounds
		{
			get { return _rotatedBounds; }
		}


		private RectangleF _bounds;
		private RectangleF _rotatedBounds;
	}
}
