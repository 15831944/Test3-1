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
	/// Stores all Arrow properties for undo/redo
	/// </summary>
	internal class ArrowProperties : ItemProperties
	{
		public ArrowProperties()
		{
		}

		internal MindFusion.FlowChartX.Pen headPen;

		internal bool allowMoveStart;
		internal bool allowMoveEnd;
		internal ArrowHead arrowHead;
		internal ArrowHead arrowBase;
		internal ArrowHead arrowInterm;
		internal bool autoRoute;
		internal bool dynamic;
		internal float headSize;
		internal float baseSize;
		internal float intermSize;
		internal Color penColor;
		internal Orientation cascadeOrientation;
		internal bool retainForm;
		internal string text;
		internal Color textColor;
		internal ArrowTextStyle textStyle;
		internal CustomDraw customDraw;
		internal bool snapToNodeBorder;
		internal HandlesStyle selStyle;
	}
}