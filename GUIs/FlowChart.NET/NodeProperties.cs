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


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Summary description for NodeProperties.
	/// </summary>
	internal class NodeProperties : ItemProperties
	{
		public NodeProperties() : base()
		{
			anchorPattern = null;
			expandable = false;
			expanded = true;
			obstacle = true;
			contraints = null;
			boolValues = (ushort)0xFFFF;
		}

		internal AnchorPattern anchorPattern;
		internal bool expandable;
		internal bool expanded;
		internal bool obstacle;
		internal NodeConstraints contraints;
		internal Handles enabledHandles;
		internal ushort boolValues;
	}
}