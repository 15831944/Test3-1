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
	internal sealed class CreateArrowBehavior : Behavior
	{
		internal CreateArrowBehavior(FlowChart fc) : base(fc) {}

		public override InteractionState StartDraw(PointF pt)
		{
			ChartObject ac = fc.ActiveObject;
			Node obj = fc.GetNodeAt(pt);
			int handle = 0;

			if (ac != null && !ac.notInteractive() && ac.pointInHandle(pt, ref handle))
			{
				return new InteractionState(ac, handle, Action.Modify);
			}
			else
			if (obj != null && obj.canHaveArrows(true))
			{
				return new InteractionState(
					new Arrow(fc, obj), -1, Action.Create);
			}
			else
			if (obj == null && fc.AllowUnconnectedArrows)
			{
				return new InteractionState(
					new Arrow(fc, fc.Dummy), -1, Action.Create);
			}
			else
			{
				return new InteractionState(
					fc.Selection, -1, Action.Create);
			}
		}

		public override void SetMouseCursor(PointF pt)
		{
			int handle = 0;
			Node obj = null;
			bool active = false;
			bool autoh = false;

			if (fc.Selection.pointInHandle(pt, ref handle))
				fc.Cursor = fc.CurModify;
			else
				if ((fc.ActiveObject != null && fc.ActiveObject.pointInHandle(pt, ref handle) && (active = true)) ||
				(fc.getAutoHObj() != null && fc.getAutoHObj().pointInHandle(pt, ref handle) && (autoh = true)))
				setModfCursor(pt, handle, active, autoh);
			else
			if ((obj = fc.GetNodeAt(pt)) != null && obj.canHaveArrows(true))
				fc.Cursor = fc.CurArrowStart;
			else
				fc.Cursor = fc.CurPointer;
		}

		public override Node GetTargetNode(PointF pt)
		{
			return fc.GetNodeAt(pt);
		}
	}
}
