using System;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Holds information that define the state of arrow segments.
	/// </summary>
	internal class ArrowSegmentsState
	{
		internal ArrowSegmentsState()
		{
		}

		internal PointCollection controlPoints;
		internal short segmentCount;
		internal bool cascadeStartHorizontal;
	}
}
