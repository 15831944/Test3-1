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
	/// Arrowhead shape
	/// </summary>
	public sealed class ArrowHeadShape
	{
		internal ArrowHeadShape(float size)
		{
			setSize(size);
			defined = false;
		}

		internal void setSize(float newSize)
		{
			size = newSize;
		}

		internal float getSize()
		{
			return size;
		}

		public int[] Elements
		{
			get { return elements; }
		}

		public float[] Coordinates
		{
			get { return coords; }
		}

		const int MAX_AHSHAPE_PTS = 20;
		
		internal void draw(Graphics g, System.Drawing.Pen pen,
			System.Drawing.Brush brush, float xoff, float yoff)
		{
			if (!defined) return;

			int c = 0;
			for (int i = 0; i < elements.GetLength(0); ++i)
			{
				EArrowHeadElement el = (EArrowHeadElement)elements[i];
				switch (el)
				{
				case EArrowHeadElement.heLine:
					{
						float x1 = coords[c++] + xoff;
						float y1 = coords[c++] + yoff;
						float x2 = coords[c++] + xoff;
						float y2 = coords[c++] + yoff;
						g.DrawLine(pen, new PointF(x1, y1), new PointF(x2, y2));
					}
					break;
				case EArrowHeadElement.heShape:
					{
						int pts = elements[++i];
						PointF[] shape = new PointF[pts];				
						for (int j = 0; j < pts; ++j)
						{
							shape[j].X = coords[c++] + xoff;
							shape[j].Y = coords[c++] + yoff;
						}
						g.FillPolygon(brush, shape);
						g.DrawPolygon(pen, shape);
					}
					break;
				case EArrowHeadElement.heCircle:
					{
						float cx = coords[c++] + xoff;
						float cy = coords[c++] + yoff;
						float r = coords[c++];
						g.FillEllipse(brush, cx-r, cy-r, 2*r, 2*r);
						g.DrawEllipse(pen, cx-r, cy-r, 2*r, 2*r);
					}			
					break;

				}	// switch (el)
			}
		}

		internal int[] elements;
		internal float[] coords;
		internal float size;
		internal bool defined;
	}
}
