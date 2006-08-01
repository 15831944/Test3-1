// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Base class for flowchart manipulators, which are user interface elements
	/// attached to chart object. The user can perform various manipulations on
	///  objects through their attached manipulators.
	/// </summary>
	internal abstract class Manipulator
	{
		internal Manipulator(ChartObject item)
		{
			this.item = item;
		}

		internal virtual void onDelete()
		{
			item = null;
		}

		internal abstract void draw(Graphics g);
		internal abstract void addToRepaintRect(ref RectangleF rect);

		internal abstract bool hitTest(PointF point);
		internal abstract bool ptInManipulator(PointF pt);

		internal abstract bool supportClipping();

		protected ChartObject item;

	}
}