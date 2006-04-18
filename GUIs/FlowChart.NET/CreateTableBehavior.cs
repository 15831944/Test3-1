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

namespace MindFusion.FlowChartX.Behaviors
{
	/// <summary>
	/// 
	/// </summary>
	internal sealed class CreateTableBehavior : Behavior
	{
		internal CreateTableBehavior(FlowChart fc) : base(fc) {}

		public override InteractionState StartDraw(PointF pt)
		{
			ChartObject ac = fc.ActiveObject;
			int handle = 0;

			if (ac != null && !ac.notInteractive() && ac.HitTestHandle(pt, ref handle))
			{
				return new InteractionState(ac, handle, Action.Modify);
			}
			else
			{
				return new InteractionState(
					new Table(fc), -1, Action.Create);
			}
		}

		public override void SetMouseCursor(PointF pt)
		{
			int handle = 0;
			bool active = false;
			bool autoh = false;

			if (fc.Selection.HitTestHandle(pt, ref handle))
				fc.Cursor = fc.CurModify;
			else
				if ((fc.ActiveObject != null && fc.ActiveObject.HitTestHandle(pt, ref handle) && (active = true)) ||
				(fc.getAutoHObj() != null && fc.getAutoHObj().HitTestHandle(pt, ref handle) && (autoh = true)))
				setModfCursor(pt, handle, active, autoh);
			else
				fc.Cursor = fc.CurPointer;
		}

	}
}
