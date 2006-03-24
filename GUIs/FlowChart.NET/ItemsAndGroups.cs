using System;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Used internally in the implementation of copy/paste functions.
	/// </summary>
	internal class ItemsAndGroups
	{
		public ItemsAndGroups(ChartObjectCollection items, GroupCollection groups)
		{
			this.items = items;
			this.groups = groups;
		}

		internal ChartObjectCollection items;
		internal GroupCollection groups;
	}
}
