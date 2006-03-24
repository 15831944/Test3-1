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
	/// Summary description for ItemProperties.
	/// </summary>
	internal class ItemProperties
	{
		public ItemProperties()
		{
			itemState = null;
			hyperLink = "";
		}

		internal virtual void freeResources()
		{
			brush.Release();
			brush = null;
		}

		internal ItemState itemState;

		internal Color fillColor;
		internal Color frameColor;
		internal bool ignoreLayout;
		internal bool invisible;
		internal bool locked;
		internal DashStyle penDashStyle;
		internal float penWidth;
		internal Color shadowColor;
		internal float shadowOffsetX;
		internal float shadowOffsetY;
		internal object tag;
		internal string toolTip;

		internal MindFusion.FlowChartX.Pen pen;
		internal MindFusion.FlowChartX.Brush brush;

		internal bool printable;
		internal float weight;
		internal string hyperLink;
	}
}
