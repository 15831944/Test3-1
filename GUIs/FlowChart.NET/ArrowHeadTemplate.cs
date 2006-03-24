// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;


namespace MindFusion.FlowChartX
{
	internal enum EArrowHeadElement
	{
		heLine,
		heShape,
		heCircle
	};

	/// <summary>
	/// Objects of this class are used to specify arrowhead shapes
	/// </summary>
	internal sealed class ArrowHeadTemplate
	{
		internal ArrowHeadTemplate()
		{
			elements = new ArrayList();
			coords = new ArrayList();
		}

		internal void addLine(float x1, float y1, float x2, float y2)
		{
			elements.Add(EArrowHeadElement.heLine);
			coords.Add(x1);
			coords.Add(y1);
			coords.Add(x2);
			coords.Add(y2);
		}


		const int MAX_AHSHAPE_PTS = 20;

		internal void addShape(float[] vertices)
		{
			elements.Add(EArrowHeadElement.heShape);
			elements.Add(vertices.GetLength(0)/2);
			for (int i = 0; i < vertices.GetLength(0); ++i)
				coords.Add(vertices[i]);
		}


		internal void addCircle(float cx, float cy, float r)
		{
			elements.Add(EArrowHeadElement.heCircle);
			coords.Add(cx);
			coords.Add(cy);
			coords.Add(r);
		}


		internal void initArrowHead(ArrowHeadShape ah)
		{
			ah.elements = new int[elements.Count];
			ah.coords = new float[coords.Count];

			for (int i = 0; i < elements.Count; ++i)
				ah.elements[i] = (int)elements[i];
		}


		internal void recalcArrowHead(ArrowHeadShape ah, PointF pt1, PointF pt2)
		{
			float dx = pt2.X - pt1.X;
			float dy = pt2.Y - pt1.Y;
			float l = (float)Math.Sqrt(dx*dx + dy*dy);
			if (l == 0)
			{
				ah.defined = false;
				return;
			}
			ah.defined = true;
			float sine = +dx/l;
			float cosine = -dy/l;
			int pts, c = 0, ch = 0;
			float x, y, r;

			for (int i = 0; i < elements.Count; ++i)
			{
				EArrowHeadElement el = (EArrowHeadElement)elements[i];
				switch (el)
				{
				case EArrowHeadElement.heLine:
					for (int j = 0; j < 2; ++j)
					{
						x = (float)coords[c++] * ah.size;
						y = (float)coords[c++] * ah.size;
						ah.coords[ch++] = x * cosine - y * sine + pt2.X;
						ah.coords[ch++] = x * sine + y * cosine + pt2.Y;
					}
					break;
				case EArrowHeadElement.heShape:
					pts = (int)elements[++i];
					for (int j = 0; j < pts; ++j)
					{
						x = (float)coords[c++] * ah.size;
						y = (float)coords[c++] * ah.size;
						ah.coords[ch++] = x * cosine - y * sine + pt2.X;
						ah.coords[ch++] = x * sine + y * cosine + pt2.Y;
					}
					break;
				case EArrowHeadElement.heCircle:
					x = (float)coords[c++] * ah.size;
					y = (float)coords[c++] * ah.size;
					r = (float)coords[c++] * ah.size;
					ah.coords[ch++] = x * cosine - y * sine + pt2.X;
					ah.coords[ch++] = x * sine + y * cosine + pt2.Y;
					ah.coords[ch++] = r;
					break;

				}	// switch (el)

			}	// for (
		}

		private ArrayList elements;
		private ArrayList coords;
	}
}
