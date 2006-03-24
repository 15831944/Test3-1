using System;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Globalization;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Xml;
using System.Xml.XPath;

using MindFusion.FlowChartX;
using MindFusion.FlowChartX.Text;


namespace MindFusion.Diagramming.Export
{
	public class BoxComparer : IComparer
	{
		private bool asc = true;
	
		public BoxComparer ( bool SortAsc )
		{
			asc = SortAsc;
		}
		
		public int Compare(object x, object y)
		{
			int zx = 0, zy = 0, CompareResult = 0;

			zx = (x as Box).ZIndex;
			zy = (y as Box).ZIndex;

			if ( zx!=zy )
			{
				if ( zx > zy )
					CompareResult = asc ? 1 : -1;
				else
					CompareResult = asc ? -1 : 1;

			}
			return CompareResult;
		}
	}

	/// <summary>
	/// FcnImporter class
	/// </summary>
	public class SvgExporter
	{
		private long PixPerInchFirst = 0,  PixPerInch = 0, m_InnerID = 0;
		private double  lDocX = 0, lDocY =0,  DocTop = 0, DocLeft = 0, DocBottom = 0, DocRight = 0;
		private MindFusion.FlowChartX.FlowChart pChart;
		private string  sSeparator = NumberFormatInfo.CurrentInfo.NumberDecimalSeparator;
		private SvgManager sMan = null;
		private bool m_GlueLines = true;
		private bool m_InvisibleItems = false;

		/// </summary>
		/// If TRUE then invisible items will be exported
		/// </summary>
		public bool InvisibleItems 
		{
			get
			{
				return m_InvisibleItems ;
			}
			set
			{
				m_InvisibleItems  = value;
			}
		}

		/// <summary>
		/// If TRUE then text will be glued to lines before output
		/// </summary>
		internal bool GlueLines
		{
			get
			{
				return m_GlueLines;
			}
			set
			{
				m_GlueLines = value;
			}
		}

		public SvgExporter()
		{
		}

		/// <summary>
		/// Intended for getting unique ID
		/// </summary>
		/// <returns></returns>
		private long NextID()
		{
			m_InnerID++;
			return m_InnerID;
		}

		/// <summary>
		///  Intended for reset unique ID
		/// </summary>
		private void ResetID()
		{
			m_InnerID = 0;
						
		}

		/// <summary>
		/// Main method to export Flowchart.NET into SVG
		/// </summary>
		/// <param name="flowChart">Flowchart.NET object reference</param>
		/// <param name="filePath">String path of the target SVG file</param>
		/// <returns>String report or error description</returns>
		public string Export(FlowChart flowChart, string filePath)
		{
			string sResult = "";

			try
			{
				// Initializing variables
				pChart = flowChart;
				
				if ( pChart == null )
					throw new Exception("Empty chart reference was passed");

				if ( filePath == null )
					throw new Exception("Empty VDX path was passed");

				// Getting screen resolution 'Pix-Per-Inch' parameter 
				IntPtr hWnd = SvgManager.GetActiveWindow(); 
				System.IntPtr hDC = SvgManager.GetDC(hWnd);
				PixPerInch = SvgManager.GetDeviceCaps(hDC, 88 );
				PixPerInchFirst = PixPerInch;


				// Getting FlowChartX document's dimensions

				double OffsetX = 0, OffsetY = 0;

				OffsetX = pChart.DocExtents.X;
				OffsetY = pChart.DocExtents.Y;
				DocLeft = pChart.DocExtents.Left;
				DocTop = pChart.DocExtents.Top;
				DocRight = pChart.DocExtents.Right;
				DocBottom = pChart.DocExtents.Bottom;

				lDocX =	DocRight -	DocLeft;
				lDocY =	DocBottom - DocTop;


				lDocX =  pChart.DocExtents.Width;
				lDocY = pChart.DocExtents.Height;

				// Calculating FlowChartX document's dimensions	in INCHES
			
				sMan = new SvgManager(String.Format("{0}px",Unit2Pix(lDocX)),  String.Format("{0}px",Unit2Pix(lDocY)), PixPerInch, pChart.MeasureUnit, m_GlueLines, pChart);
				
				
				// Preparing item's enumeration
				
				ResetID();

				// Processing items [ Tables ]

				foreach ( MindFusion.FlowChartX.Table Table in pChart.Tables)
				{

					if ( Table == null )
						continue;
					
					CreateTable(Table);					

				}

				// Processing items [ Boxes ]

				ArrayList.Adapter(pChart.Boxes).Sort(new BoxComparer(true));
				foreach ( MindFusion.FlowChartX.Box Box in pChart.Boxes)
				{
					if ( Box == null )
						continue;
					
					CreateBox(Box);			
				}

				// Processing items [ Arrows ]

				foreach ( MindFusion.FlowChartX.Arrow Arrow in pChart.Arrows)
				{
					if ( Arrow == null )
						continue;
					
					CreateArrow(Arrow);
				}

			

				sMan.Save(filePath);
				

			}
			catch ( Exception ex )
			{
				sResult = String.Format("Error occured when exporting: {0}",ex.Message);
				Trace.WriteLine(String.Format("{0} error {1}\n","ExportVDX",ex.Message));
			}
			
			return sResult;
		}

		/// <summary>
		/// Converts Flowchart.NET arrow into SVG
		/// </summary>
		/// <param name="newArrow">Arrow reference</param>
		/// <returns>TRUE if successfull otherwise FALSE</returns>
		private bool CreateArrow(MindFusion.FlowChartX.Arrow newArrow)
		{
			bool bOk = false;
			string sPath = "", sPathPart = "";
			int iCount = 0;
		

			try
			{
				if ( newArrow.Origin!=null )
				{
					if  ((!newArrow.Origin.Visible) && (!InvisibleItems))
						return true;
				}

				if ( newArrow.Destination!=null )
				{
					if  ((!newArrow.Destination.Visible) && (!InvisibleItems))
						return true;
				}

				if ( newArrow.Style == ArrowStyle.Bezier )
				{
					sPath = String.Format("M{0},{1} C{2},{3} {4},{5} {6},{7} " ,
						Unit2Pix(newArrow.ControlPoints[0].X),
						Unit2Pix(newArrow.ControlPoints[0].Y),
						Unit2Pix(newArrow.ControlPoints[1].X),
						Unit2Pix(newArrow.ControlPoints[1].Y),
						Unit2Pix(newArrow.ControlPoints[newArrow.ControlPoints.Count -2].X),
						Unit2Pix(newArrow.ControlPoints[newArrow.ControlPoints.Count -2].Y),
						Unit2Pix(newArrow.ControlPoints[newArrow.ControlPoints.Count -1].X),
						Unit2Pix(newArrow.ControlPoints[newArrow.ControlPoints.Count -1].Y));
				}
				else
				{
					sPath = String.Format("M{0},{1} ", Unit2Pix(newArrow.ControlPoints[0].X), Unit2Pix(newArrow.ControlPoints[0].Y) );

					for ( iCount = 1; iCount<newArrow.ControlPoints.Count ; iCount++)
					{
						
						sPathPart = String.Format("L{0},{1} ", Unit2Pix(newArrow.ControlPoints[iCount].X), Unit2Pix(newArrow.ControlPoints[iCount].Y));
						sPath+=sPathPart;
					}
				}
				if ( sPath=="" )
					return false;
				
				sPath = sPath.TrimEnd();
				string sWidth = "1px";
				if ( newArrow.Pen.Width != 0 ) 
						String.Format("{0}px", Unit2Pix(newArrow.Pen.Width));

					sMan.AddPath(sPath , sWidth , newArrow.PenColor,Color.Transparent);
				
			
				sPath = "";
				sPath = sMan.GetArrowHead(newArrow.HeadShape);
				XmlNode last_node = sMan.AddPath(sPath , sWidth , newArrow.PenColor,newArrow.PenColor);

				sPath = "";
				sPath = sMan.GetArrowHead(newArrow.BaseShape);
				last_node = sMan.AddPath(sPath , sWidth , newArrow.PenColor,newArrow.PenColor);

				RectangleF rect = RectangleF.Empty;
				float angle = 0;

				rect = getTextRect(System.Drawing.Graphics.FromHwnd(SvgManager.GetActiveWindow()), newArrow.Style , newArrow.TextStyle,
					newArrow.ControlPoints, newArrow.TextColor,newArrow.SegmentCount, newArrow.Text, newArrow.Font,
					RectangleF.Empty, ref angle);

				if (!rect.Equals(RectangleF.Empty))
				{
					StringFormat sf = new StringFormat();
					sf.Alignment = StringAlignment.Center;

					XmlNode text_added = sMan.AddText( null ,newArrow.Text,
						newArrow.Font, 
						rect,
						newArrow.TextColor , sf  , false, angle);


				}
				bOk = true;
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","CreateArrow",ex.Message));
				bOk = false;
			}

			return bOk;
		}
		
		private RectangleF getTextRect(Graphics g , ArrowStyle style, ArrowTextStyle textStyle, PointCollection points , Color textColor ,int segmentCount , 
			string text , Font textFont , RectangleF rect , ref float a )
		{
			// DrawString crashes with a too small PageScale
			if (g.PageScale < 0.01) return RectangleF.Empty;

			System.Drawing.Brush brText = new System.Drawing.SolidBrush(textColor);
			StringFormat sf = StringFormat.GenericDefault;
			sf.Alignment = StringAlignment.Center;
			sf.LineAlignment = StringAlignment.Far;

			float x =0 , y = 0;

			a  = 0;
			ArrowUtils au = new ArrowUtils(style,points );
			System.Drawing.SizeF textSize = g.MeasureString(text, textFont);

			if (textStyle == ArrowTextStyle.Rotate)
			{
				// Find the segment near which the text will be drawn
				int pt = points.Count / 2 - 1;
				int jump = 1;
				int ptBest = pt;
				float r = 0,  rBest = 0;
				if (style != ArrowStyle.Bezier)
				{
					if (style == ArrowStyle.Cascading ||
						(style == ArrowStyle.Polyline && segmentCount % 2 == 1))
					{
						do
						{
							MindFusion.Diagramming.Export.Convert.DekartToPolar(
								points[pt], points[pt + 1], ref a, ref r);
							if (r > rBest)
							{
								rBest = r;
								ptBest = pt;
							}
							pt = pt + jump;
							jump = -Math.Sign(jump) * (Math.Abs(jump) + 1);
							if(pt < 0 || pt >= points.Count - 1)
								break;
						}
						while(textSize.Width > r);
						pt = ptBest;
						x = (points[pt].X + points[pt + 1].X) / 2;
						y = (points[pt].Y + points[pt + 1].Y) / 2;
					}
					else
					{
						pt = pt + 1;
						x = points[pt].X;
						y = points[pt].Y;
					}
				}
				else
				{
					if(segmentCount % 2 == 1)
					{
						pt = (pt + 1) / 4;
						PointF ptMid = ArrowUtils.GetBezierPt(points, pt, 0.5f);
						x = ptMid.X;
						y = ptMid.Y;
					}
					else
					{
						pt = (pt + 1) / 4;
						PointF ptMid = points[(pt + 1) * 3];
						x = ptMid.X;
						y = ptMid.Y;
					}
				}

						
				// Find out the angle
				if (style != ArrowStyle.Bezier)
				{
					if( style == ArrowStyle.Cascading ||
						(style == ArrowStyle.Polyline && segmentCount % 2 == 1))
					{
						PointF pt1 = points[pt];
						PointF pt2 = points[pt + 1];
						MindFusion.Diagramming.Export.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
					else
					{
						float r1 = 0, a1 = 0;
						MindFusion.Diagramming.Export.Convert.DekartToPolar(
							points[pt - 1], points[pt], ref a, ref r);
						MindFusion.Diagramming.Export.Convert.DekartToPolar(
							points[pt], points[pt + 1], ref a1, ref r1);
						a = (a + a1) / 2;
					}
				}
				else
				{
					if (segmentCount % 2 == 1)
					{
						PointF pt1 = ArrowUtils.GetBezierPt(points, pt, 0.45f);
						PointF pt2 = ArrowUtils.GetBezierPt(points, pt, 0.55f);
						MindFusion.Diagramming.Export.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
					else
					{
						PointF pt1 = ArrowUtils.GetBezierPt(points, pt, 0.95f);
						PointF pt2 = ArrowUtils.GetBezierPt(points, pt + 1, 0.05f);
						MindFusion.Diagramming.Export.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
				}

				a = 180 - a;
				if (a > 90 && a < 270)
					a -= 180;
					
				// Draw the text
				/*
				 * 
				 * 
				GraphicsState state = g.Save();

				g.TranslateTransform(x, y);
				g.RotateTransform(a);
				g.TranslateTransform(-x, -y);
				g.DrawString(text, textFont, brText, new PointF(x, y), sf);

				g.Restore(state);
				*/
			}
			else if (textStyle == ArrowTextStyle.Center)
			{
				if(points.Count % 2 == 1)
				{
					x = points[points.Count / 2].X;
					y = points[points.Count / 2].Y;
				}
				else
				{
					x = points[points.Count / 2 - 1].X + points[points.Count / 2].X;
					y = points[points.Count / 2 - 1].Y + points[points.Count / 2].Y;
					x /= 2;
					y /= 2;
				}

				//g.DrawString(text, textFont, brText, new PointF(x, y), sf);
			}
			else if (textStyle == ArrowTextStyle.OverLongestSegment)
			{
				
				// find the center point of longest segment
				int longest = au.getLongestSegment();
				PointF center = au.getSegmentCenter(longest);

				// see how much space the text requires
				StringFormat cf = new StringFormat();
				cf.Alignment = StringAlignment.Center;
				cf.LineAlignment = StringAlignment.Center;
				SizeF size = g.MeasureString(text, textFont);
				size.Width += ArrowUtils.getMillimeter(pChart.MeasureUnit);
				size.Height += ArrowUtils.getMillimeter(pChart.MeasureUnit);

				// fill using the background brush
				/*
				 * 
				 * 
				System.Drawing.Brush brush = pChart.BackBrush.CreateGDIBrush(rect);
				g.FillRectangle(brush, center.X - size.Width / 2,
					center.Y - size.Height /2, size.Width, size.Height);
				brush.Dispose();

				// draw the text
				g.DrawString(text, textFont, brText, center, cf);
				*/
			
			}

			brText.Dispose();

			RectangleF rc = new RectangleF(x - textSize.Width/2 ,y - textSize.Height/2,textSize.Width, textSize.Height);
			return rc;

		}

		/// <summary>
		/// Gets Flowchart.NET box's style
		/// </summary>
		/// <param name="newBox">Box reference</param>
		/// <returns>Returned BoxStyle</returns>
		private BoxStyle GetBoxStyle (MindFusion.FlowChartX.Box newBox)
		{
			BoxStyle retStyle= BoxStyle.Shape;

			if ( newBox.Shape == null )
				return newBox.Style;

			if (newBox.Style != BoxStyle.Shape)
				return newBox.Style;

			switch ( newBox.Shape.ToString())
			{
				case "Rectangle":
					retStyle = BoxStyle.Rectangle;
					break;
				case "Ellipse":
					retStyle = BoxStyle.Ellipse;
					break;
				case "RoundRect":
					retStyle = BoxStyle.RoundedRectangle;
					break;
				default:
					retStyle = newBox.Style;
					break;
				
			};

			return retStyle;
		}

		/// <summary>
		/// Converts Flowchart.NET table into SVG
		/// </summary>
		/// <param name="newTable">Table reference</param>
		/// <returns>TRUE if successfull otherwise FALSE</returns>
		private bool CreateTable(MindFusion.FlowChartX.Table newTable)
		{
			bool bOk = false;
			float fX = 0,  fY = 0, fW = 0, fH = 0, fLine = 0;
			long lLine = 0;
			XmlNode last_node = null;
			ArrayList spannedCells = new ArrayList();

			try
			{
				fX = newTable.BoundingRect.X;
				fY = newTable.BoundingRect.Y;
				fW = newTable.BoundingRect.Width;
				fH = newTable.BoundingRect.Height;
				fLine =  newTable.Pen.Width;

				lLine = Unit2Pix(fLine);
				if ( lLine == 0 )
					lLine = 1;

		
				last_node = sMan.AddRect(String.Format("{0}px",Unit2Pix(fX)), 
					String.Format("{0}px",Unit2Pix(fY)), 
					String.Format("{0}px",Unit2Pix(fW)), 
					String.Format("{0}px",Unit2Pix(newTable.CaptionHeight)), 
					String.Format("{0}px",lLine), 
					newTable.FillColor, 
					newTable.FrameColor,
					null,
					null,
					newTable.Picture);

				sMan.AddGradient(last_node, newTable.Brush);

                sMan.AddText(null, newTable.Caption,
							 newTable.Font, 
							 new RectangleF(fX, fY,fW,newTable.CaptionHeight),
							 newTable.CaptionColor ,pChart.TextFormat, newTable.EnableStyledText);
				

				last_node = sMan.AddRect(String.Format("{0}px",Unit2Pix(fX)), 
					String.Format("{0}px",Unit2Pix(fY + newTable.CaptionHeight)), 
					String.Format("{0}px",Unit2Pix(fW)), 
					String.Format("{0}px",Unit2Pix(fH - newTable.CaptionHeight)), 
					String.Format("{0}px",lLine), 
					newTable.FillColor, 
					newTable.FrameColor,
					null,
					null,
					newTable.Picture);

				sMan.AddGradient(last_node, newTable.Brush);

				fW = 0;
				fH = 0;
				int RowCount = newTable.CurrScrollRow , ColCount = 0, AutoColls = 0;
				float fCellW = 0, fCellH = 0, fCellX = 0, fCellY = 0 , rh =0, fStepW = 0, fSumW =0;
				bool CellIsSpanned = false;
				fCellX = fX;
				fCellY = fY + newTable.CaptionHeight;

				AutoColls = 0;
				fSumW =0;
				foreach ( Table.Column col in newTable.Columns)
				{
					if (col.ColumnStyle == ColumnStyle.AutoWidth )
						AutoColls++;
					else
						fSumW+=col.Width;
				}

				while (RowCount<newTable.Rows.Count)
				{

					if ( RowCount!=newTable.CurrScrollRow  )
						rh =  newTable.Rows[RowCount-1].Height;
					else
						rh = 0;

					fCellY+=rh;
					if ( fCellY > newTable.BoundingRect.Bottom - rh)
						break;

					while (ColCount<newTable.Columns.Count )
					{
									
						if ( ColCount!=0)
							fCellX+= newTable.Columns[ColCount-1].Width;

						Table.Cell cell = newTable[ColCount, RowCount];

						if (!spannedCells.Contains(cell))
						{
							
						
							fCellH = 0;
							for (int i = RowCount; i <RowCount + cell.RowSpan; i++)
							{

								fCellH+= newTable.Rows[i].Height;
								fCellW = 0;
								bool bPass = true;
								for (int j = ColCount; j < ColCount + cell.ColumnSpan; j++)
								{
									if (( newTable.Columns[ColCount].ColumnStyle == ColumnStyle.AutoWidth ) && bPass)
									{
										fStepW = ( newTable.BoundingRect.Width - fSumW ) / AutoColls;
										bPass = false;
									}
									else
										fStepW = newTable.Columns[j].Width;
										
									fCellW+=fStepW;
									spannedCells.Add(newTable[j,i]);
									//Trace.WriteLine(String.Format("Pos ({0}, {1} ) Spanned ({2},{3})", RowCount, ColCount, i,j));
								}
							}

		
							if ( AutoColls == 0 )
							{
								if ( ColCount == ( newTable.Columns.Count -1) )
									fCellW += ( newTable.BoundingRect.Width - fSumW);
								//fCellW = newTable.BoundingRect.Right - fCellX + ( cell.Text == "" ? 0 : 15) ;
							}

							RectangleF cellRect = new RectangleF(fCellX,
								fCellY,
								fCellW,
								fCellH); 
						

						
							CellIsSpanned = ((cell.ColumnSpan>1) || ( cell.RowSpan>1));
							sMan.AddText( CellIsSpanned ? null : newTable , cell.Text,
								newTable.Font, 
								cellRect,
								newTable.TextColor , cell.TextFormat, newTable.EnableStyledText);

							
							sMan.AddRect(Unit2PixStr(cellRect.X), Unit2PixStr(cellRect.Y), Unit2PixStr(cellRect.Width), Unit2PixStr(cellRect.Height), "1px", Color.Transparent,Color.Transparent,null,null,cell.Picture, newTable.FillColor, cell.PicturePos, cellRect);
							if ( newTable.CellFrameStyle!= CellFrameStyle.None)
								sMan.AddRect(Unit2PixStr(cellRect.X), Unit2PixStr(cellRect.Y), Unit2PixStr(cellRect.Width), Unit2PixStr(cellRect.Height), "1px", Color.Transparent,newTable.FrameColor);
						
						}
						ColCount++;
					

					}

					fCellX = fX;
					ColCount = 0;
					RowCount++;
				
				}

				
				bOk = true;
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","CreateTable",ex.Message));
				bOk = false;
			}

			return bOk;
		}

		/// <summary>
		/// Converts Flowchart.NET box into SVG
		/// </summary>
		/// <param name="newArrow">Box reference</param>
		/// <returns>TRUE if successfull otherwise FALSE</returns>
		private bool CreateBox(MindFusion.FlowChartX.Box newBox)
		{
			bool bOk = false;
			float fX = 0,  fY = 0, fW = 0, fH = 0, fLine = 0;
			long lLine = 0;
	
			XmlNode last_node = null;
			try
			{


				if ( newBox == null )
					throw new Exception("Empty box reference passed");

				if  (( !newBox.Visible ) && ( !InvisibleItems))
					return true;

				if (( newBox.Transparent ) && (newBox.Picture == null))
				{
					sMan.AddText(newBox);
					return true;
				}
				

				fX = newBox.BoundingRect.X;
				fY = newBox.BoundingRect.Y;
				fW = newBox.BoundingRect.Width;
				fH = newBox.BoundingRect.Height;
				fLine =  newBox.Pen.Width;

				if (( newBox.ShapeOrientation == 90 )|| (newBox.ShapeOrientation == 270 ))
				{
					float lTemp = fW;
					fW = fH;
					fH = lTemp;
				}

				lLine = Unit2Pix(fLine);
				if ( lLine == 0 )
					lLine = 1;

				//Color boxFillColor = newBox.Locked ? Color.Transparent : newBox.FillColor;
				Color boxFillColor = newBox.FillColor;
				

				switch ( GetBoxStyle(newBox))
				{
					
					case BoxStyle.Rhombus:
						last_node = sMan.AddPath(sMan.Shape2Path(newBox), 
							String.Format("{0}px",lLine), 
							newBox.Pen.Color,
							boxFillColor);
						break;

					case BoxStyle.Ellipse:

										
						last_node = sMan.AddEllipse(String.Format("{0}px",Unit2Pix(fX + fW/2)), 
							String.Format("{0}px",Unit2Pix(fY + fH/2)), 
							String.Format("{0}px",Unit2Pix(fW/2)),
							String.Format("{0}px",Unit2Pix(fH/2)), String.Format("{0}px", lLine),
							boxFillColor,
							newBox.Pen.Color);

						
						break;
					case BoxStyle.Rectangle:
						last_node = sMan.AddRect(String.Format("{0}px",Unit2Pix(fX)), 
							String.Format("{0}px",Unit2Pix(fY)), 
							String.Format("{0}px",Unit2Pix(fW)), 
							String.Format("{0}px",Unit2Pix(fH)), 
							String.Format("{0}px",lLine), 
							boxFillColor, 
							newBox.Pen.Color,
							null,
							null,
							newBox.Picture,
							newBox.Transparent ? pChart.BackColor : Color.Empty);

						break;
					case BoxStyle.RoundedRectangle:
						
						last_node = sMan.AddRect(String.Format("{0}px",Unit2Pix(fX)), 
							String.Format("{0}px",Unit2Pix(fY)), 
							String.Format("{0}px",Unit2Pix(fW)), 
							String.Format("{0}px",Unit2Pix(fH)), 
							String.Format("{0}px",lLine), 
							boxFillColor, 
							newBox.Pen.Color,
							"10px",
							"10px",
							newBox.Picture,
							newBox.Transparent ? pChart.BackColor : Color.Empty);
						break;

					case BoxStyle.Shape:
					case BoxStyle.Delay:
					default:
						
						last_node = sMan.AddPath(sMan.Shape2Path(newBox), 
							String.Format("{0}px",lLine), 
							newBox.Pen.Color,
							boxFillColor);
					
						break;

				}
			
				sMan.AddGradient(last_node,newBox.Brush);
				sMan.AddRotation(last_node,newBox);
				sMan.AddText(newBox);
				bOk  = true;
			}

			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","CreateBox",ex.Message));
				bOk = false;
			}

			return bOk;
		}

		
		/// <summary>
		/// Converts measure value in some units into pixels
		/// </summary>
		/// <param name="dValue">Measure unit</param>
		/// <returns>Returns pixels string value like {xxpx} </returns>
		public string Unit2PixStr(double dValue)
		{
			return String.Format("{0}px", Unit2Pix( dValue,  pChart.MeasureUnit ));
		}

		/// <summary>
		/// Converts measure in GraphicsUnit into PIXELS
		/// </summary>
		/// <param name="dValue">Value in GraphicsUnit</param>
		/// <returns>Returns value in PIXELS</returns>
		private long Unit2Pix(double dValue)
		{
			return SvgManager.Unit2Pix(dValue, pChart.MeasureUnit,pChart.MeasureUnit, PixPerInch );
		}

		/// <summary>
		/// Converts measure in GraphicsUnit into PIXELS
		/// </summary>
		/// <param name="dValue">Value in GraphicsUnit</param>
		/// <param name="iUnit">GraphicsUnit</param>
		/// <returns>Returns value in PIXELS</returns>
		private long Unit2Pix(double dValue, GraphicsUnit iUnit)
		{
			return SvgManager.Unit2Pix(dValue, iUnit,pChart.MeasureUnit, PixPerInch );
		}

		/// <summary>
		/// Converts measure in GraphicsUnit into INCHES
		/// </summary>
		/// <param name="iUnit">GraphicsUnit</param>
		/// <param name="dValue">Value in GraphicsUnit</param>
		/// <returns>Returns value in INCHES</returns>
		private double Unit2Inch(GraphicsUnit iUnit, double dValue)
		{
			double dResult = 0;
			try
			{
				switch(iUnit)
				{
					case GraphicsUnit.Display:
						dResult = dValue/75;
						break;
					case GraphicsUnit.Document:
						dResult = dValue/300;
						break;
					case GraphicsUnit.Inch:
						dResult = dValue;
						break;
					case GraphicsUnit.Millimeter:
						dResult = dValue/25.1;
						break;
					case GraphicsUnit.Pixel:
						dResult = dValue/PixPerInch;
						break;
					case GraphicsUnit.Point:
						dResult = dValue/75;
						break;
					
				};
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","Unit2Inch",ex.Message));
				dResult = 0;
			}

			return dResult;
		}

		

		
	}
}
