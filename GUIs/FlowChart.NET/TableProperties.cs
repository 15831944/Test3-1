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
	/// Summary description for TableProperties.
	/// </summary>
	internal class TableProperties : NodeProperties
	{
		internal TableProperties()
		{
			captionBackBrush = null;
		}

		internal override void freeResources()
		{
			base.freeResources();

			if (picture != null)
			{
				picture = null;
				System.GC.Collect();
			}

			if (captionBackBrush != null)
			{
				captionBackBrush.Release();
				captionBackBrush = null;
			}
		}

		internal string caption;
		internal Color captionColor;
		internal StringFormat captionFormat;
		internal float captionHeight;
		internal CellFrameStyle cellBorders;
		internal float columnWidth;
		internal int currScrollRow;
		internal TableLinkStyle linkStyle;
		internal Image picture;
		internal ImageAlign picturePos;
		internal float rowHeight;
		internal bool scrollable;
		internal HandlesStyle selStyle;
		internal TableStyle style;
		internal Color textColor;
		internal bool enableStyledText;
		internal CustomDraw customDraw;
		internal CustomDraw cellCustomDraw;
		internal MindFusion.FlowChartX.Brush captionBackBrush;
	}
}