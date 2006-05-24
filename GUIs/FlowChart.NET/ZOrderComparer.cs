using System;
using System.Collections;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Summary description for ZOrderComparer.
	/// </summary>
	internal class ZOrderComparer : IComparer
	{
		public ZOrderComparer() {}

		public int Compare(object x, object y)
		{
			ChartObject item1 = x as ChartObject;
			ChartObject item2 = y as ChartObject;

			if (item1.ZIndex < item2.ZIndex)
				return -1;
			if (item1.ZIndex > item2.ZIndex)
				return 1;

			return 0;
		}
	}
}
