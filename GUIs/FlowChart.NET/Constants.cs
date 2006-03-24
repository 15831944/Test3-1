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
	/// 
	/// </summary>
	internal sealed class Constants
	{
		internal static float getFitTextStep(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
				case GraphicsUnit.Millimeter:
					return 0.4f;
				case GraphicsUnit.Inch:
					return 0.4f/6;
				case GraphicsUnit.Point:
					return 72.0f/15;
				case GraphicsUnit.Pixel:
					return 2;
				case GraphicsUnit.Document:
					return 300.0f/15;
				case GraphicsUnit.Display:
					return 75.0f/15;
			}

			return 0.4f;
		}

		internal static float getInflation(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
				case GraphicsUnit.Millimeter:
					return 2;
				case GraphicsUnit.Inch:
					return 1.0f/6;
				case GraphicsUnit.Point:
					return 72.0f/6;
				case GraphicsUnit.Pixel:
					return 8;
				case GraphicsUnit.Document:
					return 300.0f/6;
				case GraphicsUnit.Display:
					return 75.0f/6;
			}

			return 2;
		}

		internal static float getTableHeaderOffset(GraphicsUnit unit)
		{
			switch (unit)
			{

				case GraphicsUnit.Millimeter:
					return 3.2f;

				case GraphicsUnit.Inch:
					return 1.0f / 6;

				case GraphicsUnit.Point:
					return 72.0f / 6;

				case GraphicsUnit.Pixel:
					return 12;

				case GraphicsUnit.Document:
					return 300.0f / 6;

				case GraphicsUnit.Display:
					return 75.0f / 6;

			}

			return 2;
		}

		internal static float getRouteGridSize(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
				case GraphicsUnit.Millimeter:
					return 4;
				case GraphicsUnit.Inch:
					return 0.5f;
				case GraphicsUnit.Point:
					return 72.0f/6;
				case GraphicsUnit.Pixel:
					return 16;
				case GraphicsUnit.Document:
					return 300.0f/4;
				case GraphicsUnit.Display:
					return 75.0f/5;
			}

			return 2;
		}

		internal static float getScrollerWdt(GraphicsUnit currUnit)
		{
			return 15 * getPixel(currUnit);
		}

		internal static float getMinObjSize(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 2;
			case GraphicsUnit.Inch:
				return 1.0f/6;
			case GraphicsUnit.Point:
				return 72.0f/6;
			case GraphicsUnit.Pixel:
				return 6;
			case GraphicsUnit.Document:
				return 300.0f/6;
			case GraphicsUnit.Display:
				return 75.0f/6;
			}

			return 2;
		}

		internal static float getRoundRectArc(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 7;
			case GraphicsUnit.Inch:
				return 3.0f/8;
			case GraphicsUnit.Point:
				return 220.0f/8;
			case GraphicsUnit.Pixel:
				return 20;
			case GraphicsUnit.Document:
				return 900.0f/8;
			case GraphicsUnit.Display:
				return 230.0f/8;
			}

			return 7;
		}

		internal static float getMillimeter(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
				case GraphicsUnit.Millimeter:
					return 1;
				case GraphicsUnit.Inch:
					return 1.0f/12;
				case GraphicsUnit.Point:
					return 72.0f/12;
				case GraphicsUnit.Pixel:
					return 3;
				case GraphicsUnit.Document:
					return 300.0f/12;
				case GraphicsUnit.Display:
					return 75.0f/12;
			}

			return 1;
		}

		internal static float getExpHitTestL(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 1;
			case GraphicsUnit.Inch:
				return 1.0f/12;
			case GraphicsUnit.Point:
				return 72.0f/12;
			case GraphicsUnit.Pixel:
				return 3;
			case GraphicsUnit.Document:
				return 300.0f/12;
			case GraphicsUnit.Display:
				return 75.0f/12;
			}

			return 1;
		}

		internal static float getExpandIconSize(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
				case GraphicsUnit.Millimeter:
					return 2.2f;
				case GraphicsUnit.Inch:
					return 1.0f/6;
				case GraphicsUnit.Point:
					return 72.0f/6;
				case GraphicsUnit.Pixel:
					return 8;
				case GraphicsUnit.Document:
					return 300.0f/6;
				case GraphicsUnit.Display:
					return 75.0f/6;
			}

			return 2;
		}

		internal static float getExpHitTestT(GraphicsUnit currUnit)
		{
			return 2 * getExpHitTestL(currUnit);
		}

		internal static float getExpHitTestR(GraphicsUnit currUnit)
		{
			return 5 * getExpHitTestL(currUnit);
		}

		internal static float getExpHitTestB(GraphicsUnit currUnit)
		{
			return 4 * getExpHitTestL(currUnit);
		}

		internal static float getScrollerWth(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 4;
			case GraphicsUnit.Inch:
				return 1.0f/3;
			case GraphicsUnit.Point:
				return 72.0f/3;
			case GraphicsUnit.Pixel:
				return 12;
			case GraphicsUnit.Document:
				return 300.0f/3;
			case GraphicsUnit.Display:
				return 75.0f/3;
			}

			return 2;
		}

		internal static float getMinArrowheadSize(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 1;
			case GraphicsUnit.Inch:
				return 1.0f/12;
			case GraphicsUnit.Point:
				return 72.0f/12;
			case GraphicsUnit.Pixel:
				return 4;
			case GraphicsUnit.Document:
				return 300.0f/12;
			case GraphicsUnit.Display:
				return 75.0f/12;
			}

			return 1;
		}

		internal static float getAutoScroll(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 4;
			case GraphicsUnit.Inch:
				return 1.0f/5;
			case GraphicsUnit.Point:
				return 72.0f/5;
			case GraphicsUnit.Pixel:
				return 12;
			case GraphicsUnit.Document:
				return 300.0f/5;
			case GraphicsUnit.Display:
				return 75.0f/5;
			}

			return 4;
		}

		internal static float getReflWth2(GraphicsUnit currUnit)
		{
			return getArrowMinDist(currUnit) + getMinArrowheadSize(currUnit);
		}

		internal static float getReflHth(GraphicsUnit currUnit)
		{
			return getArrowMinDist(currUnit) + getMinArrowheadSize(currUnit) * 3;
		}

		internal static float getArrowMinDist(GraphicsUnit currUnit)
		{
			return getHalfInch(currUnit);
		}

		internal static float getInch(GraphicsUnit currUnit)
		{
			return getHalfInch(currUnit) * 2;
		}

		internal static float getHalfInch(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 12;
			case GraphicsUnit.Inch:
				return 1.0f/2;
			case GraphicsUnit.Point:
				return 72.0f/2;
			case GraphicsUnit.Pixel:
				return 40;
			case GraphicsUnit.Document:
				return 300.0f/2;
			case GraphicsUnit.Display:
				return 75.0f/2;
			}

			return 4;
		}

		internal static float getLineHitTest(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 5;
			case GraphicsUnit.Inch:
				return 1.25f/5;
			case GraphicsUnit.Point:
				return 90.0f/5;
			case GraphicsUnit.Pixel:
				return 15;
			case GraphicsUnit.Document:
				return 375.0f/5;
			case GraphicsUnit.Display:
				return 92.0f/5;
			}

			return 4;
		}

		internal static float getSelPtTest(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
			case GraphicsUnit.Millimeter:
				return 0.2f;
			case GraphicsUnit.Inch:
				return 0.2f/6;
			case GraphicsUnit.Point:
				return 72.0f/30;
			case GraphicsUnit.Pixel:
				return 1;
			case GraphicsUnit.Document:
				return 300.0f/30;
			case GraphicsUnit.Display:
				return 75.0f/30;
			}

			return 1;
		}

		internal static float getPixel(GraphicsUnit currUnit)
		{
			switch (currUnit)
			{
				case GraphicsUnit.Millimeter:
					return 25.4f/96;
				case GraphicsUnit.Inch:
					return 1f/96;
				case GraphicsUnit.Point:
					return 72f/96;
				case GraphicsUnit.Pixel:
					return 1;
				case GraphicsUnit.Document:
					return 300f/96;
				case GraphicsUnit.Display:
					return 75f/96;
			}

			return 1;
		}

		internal static int NoAnchorPattern = -12345;
		internal static float getMarkSize(GraphicsUnit currUnit)
		{
			return getMinObjSize(currUnit);
		}

		internal const int ARROW_CROSSINGS = 3;
		internal const int KEY_WRAPPER = 1234;
	}
}
