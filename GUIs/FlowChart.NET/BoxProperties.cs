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
	/// Summary description for BoxProperties.
	/// </summary>
	internal class BoxProperties : NodeProperties
	{
		internal override void freeResources()
		{
			base.freeResources();

			if (picture != null)
			{
				picture = null;
				System.GC.Collect();
			}
		}

		internal CustomDraw customDraw;
		internal Image picture;
		internal ImageAlign picturePos;
		internal HandlesStyle selStyle;
		internal float shapeRotation;
		internal ShapeTemplate shapeTemplate;
		internal BoxStyle style;
		internal string text;
		internal Color textColor;
		internal StringFormat textFormat;
		internal bool transparent;
		internal bool usePolyTextLt;
		internal bool useStyledText;
	}
}
