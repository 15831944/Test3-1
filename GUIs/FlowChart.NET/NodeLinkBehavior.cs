// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Windows.Forms;

namespace MindFusion.FlowChartX.Behaviors
{
	/// <summary>
	/// Declares a base class for all node-link behaviors,
	/// such as Flowcharting, Entity-Relationship, etc.
	/// </summary>
	internal abstract class NodeLinkBehavior : Behavior
	{
		internal NodeLinkBehavior(FlowChart fc) : base(fc) {}

		internal protected abstract Node CreateNode();

		public override InteractionState StartDraw(PointF pt)
		{
			ChartObject ac = fc.ActiveObject;
			Node node = fc.GetNodeAt(pt);
			int handle = 0;

			if (ac != null && (fc.ModifierKeyActions.GetKeys(ModifierKeyAction.OverrideBehavior) & Control.ModifierKeys) == 0 &&
				!ac.notInteractive() && ac.HitTestHandle(pt, ref handle))
			{
				return new InteractionState(ac, handle, Action.Modify);
			}
			else if (node != null && node.canHaveArrows(true))
			{
				return new InteractionState(
					new Arrow(fc, node), -1, Action.Create);
			}
			else if (node != null)
			{
				return new InteractionState(fc.Selection, -1, Action.Create);
			}
			else
			{
				return new InteractionState(CreateNode(), -1, Action.Create);
			}
		}

		public override void SetMouseCursor(PointF pt)
		{
			int handle = 0;
			bool active = false;
			bool autoh = false;

			Node node = fc.GetNodeAt(pt);

			if (fc.Selection.HitTestHandle(pt, ref handle))
				fc.Cursor = fc.CurModify;
			else if ((fc.ActiveObject != null && fc.ActiveObject.HitTestHandle(pt, ref handle) && (active = true)) ||
				(fc.getAutoHObj() != null && fc.getAutoHObj().HitTestHandle(pt, ref handle) && (autoh = true)))
				setModfCursor(pt, handle, active, autoh);
			else if (node != null && node.canHaveArrows(true))
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
