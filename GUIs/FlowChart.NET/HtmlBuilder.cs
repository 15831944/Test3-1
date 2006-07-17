// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.IO;
using System.Collections;
using System.Runtime.InteropServices;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Provides methods to create and use HTML client side image maps.
	/// </summary>
	[ComVisible(true)]
	public class HtmlBuilder
	{
		/// <summary>
		/// Initializes a new instance of the HtmlBuilder class.
		/// </summary>
		/// <param name="fc">The FlowChart object whose contents will generate the image map.</param>
		public HtmlBuilder(FlowChart fc)
		{
			flowChart = fc;
			linkTarget = "_self";
			areasForTableCells = false;
			expandBtnHyperLink = "";
		}

		/// <summary>
		/// Specifies the value of the TARGET attribute of AREA tags.
		/// </summary>
		public string LinkTarget
		{
			get { return linkTarget; }
			set { linkTarget = value; }
		}

		/// <summary>
		/// Specifies whether AREA tags are generated for table cells
		/// </summary>
		public bool AreasForTableCells
		{
			get { return areasForTableCells; }
			set { areasForTableCells = value; }
		}

		/// <summary>
		/// Gets or sets the hyperlink assigned to AREA tags generated for
		/// the +/- buttons of expandable nodes. If this value is an empty
		/// string, there aren't AREA tags generated for the +/- buttons.
		/// </summary>
		public string ExpandBtnHyperLink
		{
			get { return expandBtnHyperLink; }
			set { expandBtnHyperLink = value; }
		}

		/// <summary>
		/// Creates a MAP tag HTML source
		/// </summary>
		public string CreateImageMap(string mapName)
		{
			// image map coordinate system starts from 0, 0, so we might have
			// to offset the AREA coordinates to make them match the diagram items
			docOffsetX = flowChart.DocExtents.X;
			docOffsetY = flowChart.DocExtents.Y;

			string html = TAB + "<MAP NAME=\"" + mapName + "\">\r\n";

			// create an AREA tag for each node
			for (int i = flowChart.Objects.Count - 1; i >= 0; i--)
			{
				ChartObject item = flowChart.Objects[i];
				if (!item.Visible)
					continue;

				Node node = item as Node;
				if (item.HyperLink != "")
				{
					html += TAB + TAB;
					string areaTag = "<AREA SHAPE=";

					if (node != null)
					{
						if (node is Box)
						{
							Box b = node as Box;
							switch(b.Style)
							{
								case BoxStyle.Rectangle:
								case BoxStyle.RoundedRectangle:
									// SHAPE="RECT"
									areaTag += createRectArea(b.BoundingRect);
									break;
								case BoxStyle.Shape:
								case BoxStyle.Ellipse:
								case BoxStyle.Delay:
								case BoxStyle.Rhombus:
									// SHAPE="POLY"
									areaTag += createPolyArea(b.getOutlinePoly());
									break;
							}
						}

						if (node is ControlHost)
						{
							// SHAPE="RECT"
							areaTag = areaTag + createRectArea(node.BoundingRect);
						}

						if (node is Table)
						{
							Table t = node as Table;

							// use whole bounding rect if cell AREAs won't be generated
							RectangleF rect = t.BoundingRect;
							if (areasForTableCells)
								rect.Height = t.CaptionHeight;

							areaTag = areaTag + createRectArea(rect);
						}
					}
					else
					{
						areaTag += createPolyArea(item.getOutlinePoly());
					}

					areaTag += "\r\n";

					areaTag += TAB + TAB + TAB + "HREF=\"" + item.HyperLink + "\" ";
					areaTag += "TARGET=\"" + linkTarget + "\" ";
					areaTag += "TITLE=\"" + item.ToolTip + "\">";

					if (CreatingArea != null)
					{
						AreaEventArgs args = new AreaEventArgs(areaTag, item);
						CreatingArea(this, args);
						areaTag = args.AreaTag;
					}

					html += areaTag + "\r\n";
				}

				if (node != null)
				{
					if (areasForTableCells && node is Table)
						html += createCellAreas(node as Table);

					if (node.Expandable && expandBtnHyperLink != "")
						html += createExpBtnArea(node);
				}
			}

			html += TAB + "</MAP>\r\n";

			return html;
		}

		private string createCellAreas(Table table)
		{
			string html = "";
			RectangleF rc = table.BoundingRect;

			bool[,] coveredCells =
				table.getHasSpanningCells() ? table.getCoveredCells() : null;
			RectangleF cellsRect = RectangleF.FromLTRB(rc.Left,
				rc.Top + table.CaptionHeight, rc.Right, rc.Bottom);

			int rowFrom = table.CurrentRow - table.getMaxRowSpan() + 1;
			if (rowFrom < 0)
				rowFrom = 0;

			float h = rc.Top + table.CaptionHeight;
			for (int r = rowFrom; r < table.RowCount; r++)
			{
				RectangleF cellRect = rc;
				cellRect.Y = h;
				if (cellRect.Top >= rc.Bottom - 1) break;

				cellRect.Height = (table.Rows[r]).Height;

				if (cellRect.Bottom >= rc.Bottom) break;

				// If it is a hidden row below a collapsed header row, skip it
				if (table.isRowCollapsed(r))
					continue;

				if (rowFrom >= table.CurrScrollRow)
					h += (table.Rows[r]).Height;

				for (int c = 0; c < table.ColumnCount; ++c)
				{
					cellRect = table.getSpannedCellRect(r, c);

					if (cellRect.Height == 0 ||
						cellRect.Width == 0 ||
						cellRect.Bottom <= cellsRect.Top)
						continue;

					Table.Cell cell = table[c, r];

					// If the cell is covered by a spanned cell
					// and is not a span cell itself, do not process it
					if (!table.getHasSpanningCells() ||
						(coveredCells != null && !coveredCells[c,r]) ||
						(cell.RowSpan != 1 || cell.ColumnSpan != 1))
					{
						if (cell.RowSpan != 1 || cell.ColumnSpan != 1)
							cellRect = RectangleF.Intersect(cellRect, cellsRect);

						html += TAB + TAB;

						// generate rect AREA for the cell
						string areaTag = "<AREA SHAPE=";
						areaTag += createRectArea(cellRect);
						areaTag += "\r\n";
						areaTag += TAB + TAB + TAB + "HREF=\"" + cell.HyperLink + "\" ";
						areaTag += "TARGET=\"" + linkTarget + "\" ";
						areaTag += "TITLE=\"" + cell.ToolTip + "\">";

						if (CreatingArea != null)
						{
							AreaEventArgs args = new AreaEventArgs(areaTag, table, c, r);
							CreatingArea(this, args);
							areaTag = args.AreaTag;
						}

						html += areaTag + "\r\n";
					}

					cellRect.X = cellRect.Right;
					if (cellRect.Left >= rc.Right) break;
				}
			}

			return html;
		}

		private string createExpBtnArea(Node node)
		{
			Expander exp = node.getExpander();
			if (exp == null) return "";

			string link = String.Format(expandBtnHyperLink, node.ZIndex, node.Tag);

			string html = "";
			html += TAB + TAB + "<AREA SHAPE=";
			html += createRectArea(exp.getBounds());
			html += "\r\n";
			html += TAB + TAB + TAB + "HREF=\"" + link + "\" ";
			html += "TARGET=\"" + linkTarget + "\">\r\n";

			return html;
		}

		public string CreateUseMapAttr(string mapID)
		{
			string str = "";
			if (flowChart.Boxes.Count > 0)
			{
				Point docExtents = DocToClient(new PointF(flowChart.DocExtents.Width, flowChart.DocExtents.Height));
				str = "USEMAP=" + "\"" + "#" + mapID + "\"" + "WIDTH=" + "\"" + 
					(docExtents.X).ToString() + "\""+ "HEIGHT=" + "\"" + 
					(docExtents.Y).ToString() + "\"";
			}
			return str;
		}

		public string CreateImageHtml(
			string htmlFileName,
			string htmlTitle,
			string imgLocalPath,
			string imgURL,
			System.Drawing.Imaging.ImageFormat imgFormat)
		{
			string str;

			Bitmap bmp = flowChart.CreateImage();
			bmp.Save(imgLocalPath, imgFormat);

			Point docExtents = DocToClient(new PointF(flowChart.DocExtents.Width, flowChart.DocExtents.Height));
            
			str =
				"<html>\r\n" + TAB +
				"<head>\r\n" + TAB + TAB +
				"<title>"  + htmlTitle + 
				
				"</title>\r\n" + TAB + 
				"</head>\r\n" + 
				"<body>\r\n" + TAB +
				"<IMG SRC=" + "\"" + imgURL + "\"" + "ALT=" + "\"" + flowChart.ToolTip + "\" BORDER=" + "\"" + "0" + "\"\r\n" + TAB +

				"USEMAP=" + "\"#fcImgMap\" WIDTH=\"" + (docExtents.X).ToString() + "\" HEIGHT=\"" + (docExtents.Y).ToString() + "\">\r\n";

			str = str + CreateImageMap("fcImgMap");

			str = str + "\r\n</body>\r\n</html>";

			FileStream fs = new FileStream(htmlFileName, FileMode.Create);
			TextWriter tw = new StreamWriter(fs);
			tw.Write(str);
			tw.Close();
			fs.Close();
			return str;
		}

		private void setTransforms(Graphics g)
		{
			g.PageUnit = flowChart.MeasureUnit;
			g.PageScale = flowChart.ZoomFactor / 100F;
		}

		private Point DocToClient(PointF docPt)
		{
			Graphics g = flowChart.CreateGraphics();
			setTransforms(g);
			Point clientPt = Utilities.docToDevice(g, docPt);
			g.Dispose();
			return clientPt;
		}

		private string createPolyArea(PointCollection points)
		{
			string strCoords = "\"";
			Point pt = new Point(0,0);
            
			for (int j = 0; j < points.Count; ++j)
			{
				PointF docPt = points[j];
				docPt.X -= docOffsetX;
				docPt.Y -= docOffsetY;

				pt = DocToClient(docPt);
				strCoords += pt.X.ToString() + "," + pt.Y.ToString();
				if (j < points.Count - 1) strCoords += ",";
			}
            
			strCoords = strCoords + "\"";

			return "\"POLY\" COORDS=" + strCoords;
		}

		private string createRectArea(RectangleF rect)
		{
			string strCoords = "";

			rect.Offset(-docOffsetX, -docOffsetY);
			float left   = rect.Left;
			float top    = rect.Top;
			float right  = rect.Right;
			float bottom = rect.Bottom;

			Point clientLT = DocToClient(new PointF(left, top));
			Point clientRB = DocToClient(new PointF(right, bottom));

			strCoords = "\"" + 
				clientLT.X.ToString() + "," + 
				clientLT.Y.ToString() + "," +
				clientRB.X.ToString() + "," +
				clientRB.Y.ToString() + "\"";

			return "\"RECT\" COORDS=" + strCoords;
		}

		public event AreaEventHandler CreatingArea;

		private FlowChart flowChart;
		private string linkTarget;
		private bool areasForTableCells;
		private float docOffsetX;
		private float docOffsetY;
		private string expandBtnHyperLink;
		private const string TAB = "\t";
	}

	public class AreaEventArgs : EventArgs
	{
		public AreaEventArgs(string areaTag, ChartObject item, int column, int row)
		{
			this.areaTag = areaTag;
			this.item = item;
			this.column = column;
			this.row = row;
		}

		public AreaEventArgs(string areaTag, ChartObject item)
		{
			this.areaTag = areaTag;
			this.item = item;
			this.column = -1;
			this.row = -1;
		}

		public string AreaTag
		{
			get { return areaTag; }
			set { areaTag = value; }
		}

		public ChartObject Item
		{
			get { return item; }
		}

		public int Column
		{
			get { return column; }
		}

		public int Row
		{
			get { return row; }
		}

		private string areaTag;
		private ChartObject item;
		private int column;
		private int row;
	}

	public delegate void AreaEventHandler(object sender, AreaEventArgs e);
}