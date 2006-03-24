using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Xml;
using System.Xml.XPath;
using System.Diagnostics;
using MindFusion.FlowChartX;
using System.Runtime.InteropServices;


namespace MindFusion.Diagramming.Export
{
	/// <summary>
	/// SVGMANAGER is helper class joining the functionality for SVG creation
	/// </summary>
	internal class SvgManager
	{
		private MindFusion.FlowChartX.FlowChart pChart;
		private int iGradientCount = 0;
		private XmlNamespaceManager m_nsManager = null;
		private XmlDocument m_InnerDoc = null;
		private XmlNode m_InnerRoot = null,m_InnerDefs = null, m_InnerNode = null;
		bool m_ObjectValid = false;
		string sXMLDefault = "<?xml version=\"1.0\"?>\n<svg xmlns=\"{0}\" version=\"1.1\" width=\"{1}\" height=\"{2}\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n<defs>\n</defs>\n</svg>";
		string sXML = "", sTextLine="";
		string sNsDefault = "http://www.w3.org/2000/svg";
		long iPixPerInch = 0;
		float m_lastY = 0;
		float m_lastX = 0;
		GraphicsUnit iUnitDef = GraphicsUnit.Pixel;
		bool IsTable = false;
		bool IsTransparent = false;
		public bool m_GlueLines = true;

		internal enum EArrowHeadElement
		{
			heLine,
			heShape,
			heCircle
		};

		[DllImport("gdi32.dll", EntryPoint="GetDeviceCaps", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)] 
		public static extern int GetDeviceCaps(IntPtr hdc, int nIndex); 


		[DllImport("User32.DLL", EntryPoint="GetActiveWindow", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)]
		public  static extern IntPtr GetActiveWindow ( ); 
		
		[DllImport("User32.DLL",EntryPoint="GetDC", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)]
		public extern static System.IntPtr GetDC(System.IntPtr hWnd); 

	
	
		/// <summary>
		/// Converts System.Drawing.Color into #RGB string color
		/// </summary>
		/// <param name="cr">System.Drawing.Color value to be converted</param>
		/// <returns>#RGB string for passed System.Drawing.Color value</returns>
		public static string Color2Str(Color cr )
		{
			string sResult = "#000000";

			try
			{
				if ( cr == Color.Transparent )
					sResult = "none";
				else
					sResult = String.Format("#{0:X2}{1:X2}{2:X2}", cr.R, cr.G, cr.B);
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.Color2Str",ex.Message));
				
			}

			if (( sResult == "#FFFFFF" ) && ( cr.A == 0))
				sResult = "none";
			return sResult;
		}

		/// <summary>
		/// Converts Flowchart.NET alignment constants into SVG "text-anchor" attribute
		/// </summary>
		/// <param name="sa">StringAlignment enumeration</param>
		/// <returns>String corresponded value for "text-anchor" attribute</returns>
		public static string Align2Str( StringAlignment sa)
		{
			string sAllign = "";
			switch (sa)
			{
				case StringAlignment.Center:
					sAllign = "text-anchor:middle";
					break;
				case StringAlignment.Far:
					sAllign = "text-anchor:end";
					break;
				case StringAlignment.Near:
					sAllign = "text-anchor:start";
					break;
			};

			return sAllign;
		}


		/// <summary>
		/// Converts .NET Rectabgle into SVG string path data
		/// </summary>
		/// <param name="rect">RectangleF type value</param>
		/// <param name="gr">GraphicsPath reference to be added by Rectangle definition</param>
		/// <returns>Returns SVG string path data</returns>
		public string Rect2Path ( RectangleF rect , ref GraphicsPath gr )
		{
			return Rect2Path (rect ,  ref gr , true );
		}
					
		/// <summary>
		/// Converts .NET Rectabgle into SVG string path data
		/// </summary>
		/// <param name="rect">RectangleF type value</param>
		/// <param name="gr">GraphicsPath reference to be added by Rectangle definition</param>
		///<param name="IsNew">If TRUE that GraphicsPath is being added by Rect otherwise NOT</param>
		/// <returns>Returns SVG string path data</returns>										 
		public string Rect2Path ( RectangleF rect , ref GraphicsPath gr , bool IsNew )
		{
			
			string sResult = "";
			if (IsNew)
				gr.AddRectangle(rect);
			sResult = String.Format("M {0},{1} L{2},{3} L{4},{5} L{6},{7} z",
				Unit2Pix(rect.Left) ,Unit2Pix(rect.Top),
				Unit2Pix(rect.Right), Unit2Pix(rect.Top),
				Unit2Pix(rect.Right),  Unit2Pix(rect.Bottom),
				Unit2Pix(rect.Left), Unit2Pix(rect.Bottom));

			return sResult;

		}
		
		/// <summary>
		/// Produces SVG string path and .NET GraphicsPath using Flowchart.NET box reference
		/// </summary>
		/// <param name="newBox">Box reference</param>
		/// <returns>Returns SVG string path data</returns>
		
		public string Shape2Path ( Box newBox )
		{
			GraphicsPath gr = null;
			string sPath = "";
			gr = new GraphicsPath(FillMode.Winding);

			if ( gr!=null )
			{
				if ( newBox.Shape == null )
					sPath = Shape2Path(newBox,ref gr, null);
				else
					sPath = Complex2Path(newBox.BoundingRect, newBox.Shape.Outline, ref gr);

				gr.Dispose();
			}

			return sPath;
		}

		/// <summary>
		/// Produces SVG string path and .NET GraphicsPath using Flowchart.NET box reference
		/// </summary>
		/// <param name="newBox">Box reference</param>
		/// <param name="gr">GraphicsPath reference</param>
		/// <param name="eta">Flowchart.NET ElementTemplate[] for complex shapes</param>
		/// <returns>Returns SVG string path data</returns>
		public string Shape2Path( Box newBox, ref GraphicsPath gr ,  ElementTemplate[] eta )
		{
			string sResult = "";
			PointF pt0 = new PointF(0,0), pt1 = new PointF(0,0), pt2 = new PointF(0,0), pt3 = new PointF(0,0);
		
			switch (newBox.Style)
			{
				case BoxStyle.Rectangle:
				
					Rect2Path(newBox.BoundingRect , ref gr);
					break;
				case BoxStyle.Ellipse:
					gr.AddEllipse(newBox.BoundingRect.X,
						newBox.BoundingRect.Y,
						newBox.BoundingRect.Width,
						newBox.BoundingRect.Height);

					
					sResult = Rect2Path(newBox.BoundingRect, ref gr ,false);

					break;
				case BoxStyle.RoundedRectangle:
					gr = Utilities.getRoundRect(newBox.BoundingRect.X,
						newBox.BoundingRect.Y,
						newBox.BoundingRect.Width,
						newBox.BoundingRect.Height,
						10);
				

					sResult = Rect2Path(newBox.BoundingRect, ref gr, false);
					break;
				case BoxStyle.Delay:
					
					sResult = Rect2Path(newBox.BoundingRect, ref gr);
				
					break;
				case BoxStyle.Rhombus:
				
					
					pt0 = new PointF(
						(newBox.BoundingRect.Left + newBox.BoundingRect.Right) / 2, newBox.BoundingRect.Top);
					pt1 = new PointF(
						Math.Max(newBox.BoundingRect.Right, newBox.BoundingRect.Left), (newBox.BoundingRect.Top + newBox.BoundingRect.Bottom) / 2);
					pt2 = new PointF(
						pt0.X, Math.Max(newBox.BoundingRect.Bottom, newBox.BoundingRect.Top));
					pt3 = new PointF(
						newBox.BoundingRect.Left, pt1.Y);
					
					pt1.X+=5;
					pt3.X-=5;
					pt2.Y+=5;
					pt0.Y-=5;
					PointF[] pts = { pt0, pt1, pt2, pt3 };

					gr.AddPolygon(pts);
									
					sResult = String.Format("M {0},{1} L{2},{3} L{4},{5} L{6},{7} z",
						Unit2Pix((newBox.BoundingRect.Left + newBox.BoundingRect.Right) / 2) ,Unit2Pix(newBox.BoundingRect.Top),
						Unit2Pix(Math.Max(newBox.BoundingRect.Right, newBox.BoundingRect.Left)), Unit2Pix((newBox.BoundingRect.Top + newBox.BoundingRect.Bottom)/2),
						Unit2Pix(pt0.X) , Unit2Pix(Math.Max(newBox.BoundingRect.Bottom, newBox.BoundingRect.Top)),
						Unit2Pix(newBox.BoundingRect.Left), Unit2Pix(pt1.Y));
					
				
					break;
				case BoxStyle.Shape:
					if ( eta != null )
					{
						sResult = Complex2Path(newBox.BoundingRect, eta , ref gr);
					}
					break;

			}

			return sResult;
		}

		/// <summary>
		/// Produces SVG string path and .NET GraphicsPath using Flowchart.NET complex shape data
		/// </summary>
		/// <param name="rect">Shapes Bounding Rect</param>
		/// <param name="eta">Flowchart.NET ElementTemplate[] for complex shapes</param>
		/// <param name="gr">GraphicsPath reference</param>
		/// <returns>Returns SVG string path data</returns>
		
		public string Complex2Path ( RectangleF rect , ElementTemplate[] eta , ref GraphicsPath gr )
		{
			string sPath = "", sPartPath = "";
			float X = 0, Y = 0, X1 = 0, Y1 = 0, X2 = 0 , Y2 = 0, X3 = 0, Y3 = 0;
			bool FirstPass = true;

	
			foreach (ElementTemplate et in eta )
			{
				sPartPath = "";
				switch ( et.getClassId())
				{
					case 28: // arc
						
						ArcTemplate at = ( ArcTemplate ) et;
						
						double rx = 0, ry = 0 , cx = 0, cy = 0;
						double angle1 = 0, angle2 = 0;


						rx = rect.Width * (at.Bounds.Width/200);
						ry = rect.Height * (at.Bounds.Height/200);

						cx = rect.X + rect.Width*(at.Bounds.X/100) + rx;
						cy = rect.Y + rect.Height*(at.Bounds.Y/100) + ry;

						angle1 = at.StartAngle>0 ? (360 - at.StartAngle) : Math.Abs(at.StartAngle);
						angle2 = (360 - (( at.StartAngle + at.SweepAngle) % 360));
						X2 = (float) (cx+rx*Math.Cos(angle1*Math.PI/180));
						Y2 = (float) (cy+ry*Math.Sin(angle1*Math.PI/180));

						X1 = (float) (cx+rx*Math.Cos(angle2*Math.PI/180));
						Y1 = (float) (cy+ry*Math.Sin(angle2*Math.PI/180));

						if ( FirstPass )
						{
							FirstPass = false;
							sPartPath = String.Format("M{0},{1} A{2},{3} {4} {5},{6} {7},{8} " ,
								Unit2Pix(X1),
								Unit2Pix(Y1),
								Unit2Pix(ry),
								Unit2Pix(rx),
								angle2,
								( angle2 > 180 ) ? 1 : 0,
								( at.SweepAngle > 0 ) ? 1 : 0,
								Unit2Pix(X2),
								Unit2Pix(Y2));
						}
						else
						{
							sPartPath = String.Format("A{0},{1} {2} {3},{4} {5},{6} " ,
								Unit2Pix(ry),
								Unit2Pix(rx),
								angle2,
								( angle2 > 180 ) ? 1 : 0,
								( at.SweepAngle > 0 ) ? 1 : 0,
								Unit2Pix(X2),
								Unit2Pix(Y2));
						}

						gr.AddArc(rect.X, rect.Y, rect.Width , rect.Height , at.StartAngle , at.SweepAngle );
						break;
					case 29: // bezier

						BezierTemplate  bt = ( BezierTemplate  ) et;

						X = rect.X  + rect.Width * (bt.Coordinates[0]/100);
						Y = rect.Y  + rect.Height* (bt.Coordinates[1]/100);

						X1 = rect.X  + rect.Width * (bt.Coordinates[2]/100);
						Y1 = rect.Y  + rect.Height* (bt.Coordinates[3]/100);

						X2 = rect.X  + rect.Width * (bt.Coordinates[4]/100);
						Y2 = rect.Y  + rect.Height* (bt.Coordinates[5]/100);

						X3 = rect.X  + rect.Width * (bt.Coordinates[6]/100);
						Y3 = rect.Y  + rect.Height* (bt.Coordinates[7]/100);



						if ( FirstPass )
						{
							FirstPass = false;
							sPartPath = String.Format("M{0},{1} C{2},{3} {4},{5} {6},{7} " ,
								Unit2Pix(X),
								Unit2Pix(Y),
								Unit2Pix(X1),
								Unit2Pix(Y1),
								Unit2Pix(X2),
								Unit2Pix(Y2),
								Unit2Pix(X3),
								Unit2Pix(Y3));
						}
						else
						{
							sPartPath = String.Format("C{0},{1} {2},{3} {4},{5} " ,
								Unit2Pix(X1),
								Unit2Pix(Y1),
								Unit2Pix(X2),
								Unit2Pix(Y2),
								Unit2Pix(X3),
								Unit2Pix(Y3));
						}

						gr.AddBezier(X,Y,X1,Y1,X2,Y2,X3,Y3);

						break;
					case 30: // line
						LineTemplate lt = (LineTemplate) et;
						
						X = rect.X  + rect.Width * (lt.Coordinates[0]/100);
						Y = rect.Y  + rect.Height* (lt.Coordinates[1]/100);

						sPartPath = String.Format( FirstPass ? "M{0},{1} " : "L{0},{1} ", Unit2Pix(X), Unit2Pix(Y) );
						if ( FirstPass )
							FirstPass = false;
						
						X1 = X;
						Y1 = Y;
						for ( int i=2; i<lt.Coordinates.GetLength(0);i = i+2)
						{
							X = rect.X + rect.Width * (lt.Coordinates[i]/100);
							Y = rect.Y + rect.Height* (lt.Coordinates[i+1]/100);
							X2 = X;
							Y2 = Y;
							sPartPath+=String.Format("L{0},{1} ", Unit2Pix(X), Unit2Pix(Y));
							
						}

					
						gr.AddLine(X1,Y1,X2,Y2);
						break;
				}
							
				sPath+=sPartPath;
			}

			return sPath;
		}
	
	

		/// <summary>
		/// Transforms image into Base64 encoded string
		/// </summary>
		/// <param name="img">Inage object reference</param>
		/// <param name="Color2Repl"></param>
		/// <returns>Base64 encoded JPEG image</returns>
		public static string Image2String ( System.Drawing.Image img, Color Color2Repl)
		{
			string sImg = null, sTempFile = "$$$temp$$$.bmp";
			int BytesRead = 0;
			byte[] baImage = null;
			if ( img == null )
				return null;

			try
			{
				
				if ( Color2Repl != Color.Empty )
				{
					Color pixColor;
					Bitmap bmp = img as Bitmap;
					for (int Xcount = 0; Xcount < img.Width; Xcount++)
					{
						for (int Ycount = 0; Ycount < bmp.Height; Ycount++)
						{
							pixColor = bmp.GetPixel(Xcount, Ycount);
							if ( pixColor == Color.FromArgb(0,0,0,0))
							{
								bmp.SetPixel(Xcount, Ycount, Color2Repl);
							}

					
						}
					}
				}
				img.Save(sTempFile, System.Drawing.Imaging.ImageFormat.Jpeg);

				System.IO.FileStream fs = null;
				fs = new System.IO.FileStream(sTempFile, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
				if ( fs != null )
				{
					baImage = new byte[fs.Length];
					BytesRead = fs.Read(baImage, 0, (int) fs.Length);
					if ( BytesRead>0 )
						sImg = System.Convert.ToBase64String(baImage);
				}
				
				fs.Close();
				System.IO.File.Delete(sTempFile);
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","Image2String",ex.Message));
				sImg = null;
			}
			
			return sImg;
		}

		/// <summary>
		/// SvgManager constructor
		/// </summary>
		/// <param name="DimX">Chart X dimension</param>
		/// <param name="DimY">Chart Y demension</param>
		/// <param name="newPixPerInch">PixelsPerInch rate</param>
		/// <param name="UnitDefault">Default measure unit</param>
		public SvgManager( string DimX, string DimY , long newPixPerInch , GraphicsUnit UnitDefault , bool p_GlueLines , MindFusion.FlowChartX.FlowChart ChartRef)
		{
			try
			{

				iPixPerInch = newPixPerInch;
				iUnitDef = UnitDefault;
				m_GlueLines = p_GlueLines;
				sXML = String.Format(sXMLDefault, sNsDefault, DimX, DimY);
				m_InnerDoc = new XmlDocument();
				pChart = ChartRef;

				m_InnerDoc.LoadXml(sXML);
			
				
				m_nsManager = new XmlNamespaceManager(m_InnerDoc.NameTable);
				m_nsManager.AddNamespace("svg", sNsDefault);
				m_nsManager.AddNamespace("xlink", "http://www.w3.org/1999/xlink");
				m_InnerRoot = m_InnerDoc.SelectSingleNode("svg:svg",m_nsManager);
				m_InnerDefs = m_InnerDoc.SelectSingleNode("svg:svg//svg:defs",m_nsManager);
				m_ObjectValid = true;
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.SvgManager()",ex.Message));
				m_ObjectValid = false;
			}

		}

		/// <summary>
		/// Converts measure value in some units into pixels
		/// </summary>
		/// <param name="dValue">Measure unit</param>
		/// <returns>Returns pixels string value like {xxpx} </returns>
		public string Unit2PixStr(double dValue)
		{
			return String.Format("{0}px", Unit2Pix( dValue,  iUnitDef,  iUnitDef , iPixPerInch ));
		}
	
		/// <summary>
		/// Converts measure value in some units into pixels
		/// </summary>
		/// <param name="dValue">Measure unit</param>
		/// <returns>Returns pixels as long</returns>
		public long Unit2Pix(double dValue)
		{
			return Unit2Pix( dValue,  iUnitDef,  iUnitDef , iPixPerInch );
		}
	
		/// <summary>
		/// Converts measure value in some units into pixels
		/// </summary>
		/// <param name="dValue">Measure unit</param>
		/// <param name="ConversionUnit">Unit of the value</param>
		/// <returns>Returns pixels as long</returns>
		public long Unit2Pix(double dValue, GraphicsUnit ConversionUnit)
		{
			return Unit2Pix( dValue,  ConversionUnit,  iUnitDef , iPixPerInch );
		}

		/// <summary>
		/// Converts measure value in some units into pixels
		/// </summary>
		/// <param name="dValue">Measure unit</param>
		/// <param name="ConversionUnit">Unit of the value</param>
		/// <param name="DefaultUnit">Default measure unti</param>
		/// <param name="PixPerInch">PixelsPerInch rate</param>
		/// <returns>Returns pixels as long</returns>
		public static long Unit2Pix(double dValue, GraphicsUnit ConversionUnit,  GraphicsUnit DefaultUnit , long  PixPerInch)
		{
			double dResult = 0;
			try
			{
				if ( ConversionUnit == GraphicsUnit.World )
					ConversionUnit = DefaultUnit;

				switch(ConversionUnit)
				{
					case GraphicsUnit.Display:
						dResult = ( dValue/75 ) * PixPerInch;
						break;
					case GraphicsUnit.Document:
						dResult = ( dValue/300) * PixPerInch;
						break;
					case GraphicsUnit.Inch:
						dResult = dValue * PixPerInch;
						break;
					case GraphicsUnit.Millimeter:
						dResult = ( dValue/25.1 ) * PixPerInch;
						break;
					case GraphicsUnit.Pixel:
						dResult = dValue;
						break;
					case GraphicsUnit.Point:
						dResult = ( dValue/72 ) *  PixPerInch;
						break;
					
				};
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","Unit2Inch",ex.Message));
				dResult = 0;
			}

			return (long) dResult;
		}

		/// <summary>
		/// Adds text to speciafied Flowchart.NET box
		/// </summary>
		/// <param name="newBox">Box reference</param>
		/// <returns>Returns recently added to SVG XML "text" node</returns>
		public XmlNode AddText(MindFusion.FlowChartX.Box newBox)
		{
			RectangleF cloneRect = new RectangleF(newBox.BoundingRect.X, newBox.BoundingRect.Y,
				newBox.BoundingRect.Width  ,newBox.BoundingRect.Height);

			XmlNode nd = AddText(newBox, newBox.Text,newBox.Font,cloneRect,newBox.TextColor, newBox.TextFormat, newBox.EnableStyledText);
			AddRotation(nd,newBox);
			return nd;
		
		
		}

		/// <summary>
		/// Adds text to speciafied Flowchart.NET node
		/// </summary>
		/// <param name="ActiveNode">Node reference</param>
		/// <param name="Text2Render">String text to be added</param>
		/// <param name="Font2Render">Font to be used for text drawing</param>
		/// <param name="Rect2Render">Rectangle of the text area</param>
		/// <param name="Color2Render">Color for text rendering</param>
		/// <param name="TextFormat2Render">Text format for text renedering</param>
		/// <param name="IsStyled">If TRUE the text is styled</param>
		/// <returns>Returns recently added to SVG XML "text" node</returns>
		public XmlNode AddText( Node ActiveNode, string Text2Render, Font Font2Render, RectangleF Rect2Render, Color Color2Render, StringFormat  TextFormat2Render , bool IsStyled )
		{
			return AddText(  ActiveNode,  Text2Render,  Font2Render,  Rect2Render,  Color2Render,   TextFormat2Render ,  IsStyled , 0 );
		}
		/// <summary>
		/// Adds text to speciafied Flowchart.NET node
		/// </summary>
		/// <param name="ActiveNode">Node reference</param>
		/// <param name="Text2Render">String text to be added</param>
		/// <param name="Font2Render">Font to be used for text drawing</param>
		/// <param name="Rect2Render">Rectangle of the text area</param>
		/// <param name="Color2Render">Color for text rendering</param>
		/// <param name="TextFormat2Render">Text format for text renedering</param>
		/// <param name="IsStyled">If TRUE the text is styled</param>
		/// <returns>Returns recently added to SVG XML "text" node</returns>
		public XmlNode AddText( Node ActiveNode, string Text2Render, Font Font2Render, RectangleF Rect2Render, Color Color2Render, StringFormat  TextFormat2Render , bool IsStyled , float angle)
		{
		
		
			Layout tl = null;
			LayoutOptions lo;
			DrawTextHint dhint = null;
			StyledText stext = null;
			PlainText  text = null;
			System.Drawing.SolidBrush br = null;
			GraphicsPath path = null;
			PointF[] pF = null;
			string sPath = "";
			double OffsetY = 0;
			double OffsetX = 0;
			int tlen = 0;
			try
			{
			
				if (( Text2Render == null ) || ( Text2Render==""))
					return null;

				m_InnerNode = m_InnerRoot.InsertAfter(m_InnerDoc.CreateNode(XmlNodeType.Element,"text", sNsDefault), m_InnerRoot.LastChild);
				if ( m_InnerNode ==null )
					return m_InnerNode;
					
				if ( TextFormat2Render.Alignment == StringAlignment.Near )
					Rect2Render.Width+=15;
	
				path = new GraphicsPath(FillMode.Winding);
					
		
				IsTransparent = false;
				IsTable = false;
				if ( ActiveNode == null )
				{
					sPath = Rect2Path(Rect2Render, ref path);
				} 
				else if ( ActiveNode is Box )
				{
					IsTransparent = ((Box) ActiveNode).Transparent;
					if ( ((Box) ActiveNode).Shape == null )
					{
						sPath = Shape2Path((Box) ActiveNode, ref path , null);
					}
					else
						sPath = Complex2Path(Rect2Render, ( ((Box) ActiveNode).Shape.TextArea == null )? ((Box) ActiveNode).Shape.Outline : ((Box) ActiveNode).Shape.TextArea, ref path );
										
				} 
				else if ( ActiveNode is Table )
				{
					sPath = Rect2Path(Rect2Render, ref path);
					IsTable = true;
					OffsetX = -Rect2Render.Width/2;
				}
				else
				{
					return null;
				}
			
				
		
				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("x"));
				m_InnerNode.Attributes["x"].Value = String.Format("{0}px",Unit2Pix(Rect2Render.X+Rect2Render.Width/2));

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("y"));
				m_InnerNode.Attributes["y"].Value = String.Format("{0}px",Unit2Pix(Rect2Render.Y + Rect2Render.Height/2));

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("dx"));
				m_InnerNode.Attributes["dx"].Value = String.Format("{0}px",Unit2Pix(OffsetX));

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("dy"));
				m_InnerNode.Attributes["dy"].Value = String.Format("{0}px",Unit2Pix(OffsetY));

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("font-family"));
				m_InnerNode.Attributes["font-family"].Value = Font2Render.FontFamily.Name.ToString();

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("font-size"));
				m_InnerNode.Attributes["font-size"].Value = Unit2Pix(Font2Render.Size, Font2Render.Unit).ToString();

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("fill"));
				m_InnerNode.Attributes["fill"].Value = SvgManager.Color2Str(Color2Render);

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("style"));
				m_InnerNode.Attributes["style"].Value = SvgManager.Align2Str(TextFormat2Render.Alignment);
				
			
			
				IntPtr hWnd = GetActiveWindow();

				if ( IsStyled )
				{
					stext = new StyledText();
					stext.Setup(Text2Render, System.Drawing.Graphics.FromHwnd(hWnd) ,Font2Render);
					tlen = stext.PlainText.Length;
				}
				else
				{
					text = new PlainText();
					text.Setup(Text2Render, System.Drawing.Graphics.FromHwnd(hWnd) ,Font2Render);
					tlen = text.PlainText.Length;
				}


			
			
				path.Flatten();
				pF = (PointF[])path.PathPoints.Clone();
				path.Dispose();
					

				tl = new Layout();
				br = new System.Drawing.SolidBrush(Color2Render);
				lo = new LayoutOptions();
				dhint = new DrawTextHint(System.Drawing.Graphics.FromHwnd(hWnd),
					Font2Render, br ,TextFormat2Render, false , m_InnerNode, Rect2Render );

				m_lastY = 0;
				m_lastX = 0;
				sTextLine = "";
				
				lo.Alignment = TextFormat2Render.Alignment;
				lo.LineAlignment = TextFormat2Render.LineAlignment;
				tl.LayoutInPolygon((IsStyled ? (Text) stext : (Text) text), docToLocal(pF, Rect2Render),lo);
				tl.Draw(0,0,new RenderTextCallback(___TextCallback),dhint);
				AddRotation(m_InnerNode, Rect2Render , angle );

			
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.AddText)",ex.Message));
		
			}
			return m_InnerNode;
		}

		public bool AddRotation(XmlNode ShapeNode, RectangleF rect, float ra)
		{
			bool bOk = false;
			long lAngle = 0;
			string sTrans = "";
			try
			{
			
				if ( ShapeNode == null )
					return true;

				if ( ra == 0 )
					return true;

				if ( (ra)> 180 )
					lAngle = (long) ((-1) *(360 - (ra)));
				else
					lAngle = (long) (ra);

				
				XmlAttribute attr = ShapeNode.Attributes.Append(m_InnerDoc.CreateAttribute("transform"));
				sTrans = String.Format("rotate({0} {1}px {2}px)", lAngle,
					Unit2Pix(rect.X + rect.Width/2),Unit2Pix(rect.Y + rect.Height/2));
				ShapeNode.Attributes["transform"].Value = sTrans;
			
				bOk = true;

			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.AddRotation)",ex.Message));
				bOk = false;
		
			}
			return bOk;

		}

		/// <summary>
		/// Modifies SVG 'transform' attribute to implement a rotation
		/// </summary>
		/// <param name="ShapeNode">SVG XCML node refence</param>
		/// <param name="newBox">Box reference</param>
		/// <returns>TRUE is successfull otherwise FALSE</returns>
		public bool AddRotation(XmlNode ShapeNode, Box newBox)
		{
			bool bOk = false;
			long lAngle = 0;
			try
			{
				if ( newBox == null )
					return true;

				if ( ShapeNode == null )
					return true;

				if ( ( newBox.RotationAngle + newBox.ShapeOrientation )== 0 )
					return true;

				if ( (newBox.RotationAngle + newBox.ShapeOrientation)> 180 )
					lAngle = (long) ((-1) *(360 - (newBox.RotationAngle+newBox.ShapeOrientation)));
				else
					lAngle = (long) (newBox.RotationAngle + newBox.ShapeOrientation);

				
				ShapeNode.Attributes.Append(m_InnerDoc.CreateAttribute("transform"));
				ShapeNode.Attributes["transform"].Value = String.Format("rotate({0} {1}px {2}px)", lAngle,
					Unit2Pix(newBox.BoundingRect.X + newBox.BoundingRect.Width/2),Unit2Pix(newBox.BoundingRect.Y + newBox.BoundingRect.Height/2));
			
				bOk = true;

			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.AddRotation)",ex.Message));
				bOk = false;
		
			}
			return bOk;

		}

		/// <summary>
		/// Modifies SVG node fill color by adding gradient fill
		/// </summary>
		/// <param name="ShapeNode">XML node of the SVG that reuires to change its filling</param>
		/// <param name="ShapeBrush">Flowchart.NET shape brush</param>
		/// <returns>TRUE is successfull otherwise FALSE</returns>
		public bool AddGradient(XmlNode ShapeNode, MindFusion.FlowChartX.Brush ShapeBrush)
		{

			bool bOk = false;
			XmlNode m_InnerGradient = null , m_XmlTemp = null;
			string sType = "" , sGradientName = "" , sFore = "", sBack = "";

			try
			{
				if (!IsValid())
					return true;

				if ( ShapeNode == null )
					return true;

				sType = ShapeBrush.GetType().ToString();
				if (  sType.IndexOf("LinearGradientBrush")<0)
					return true;
				
				MindFusion.FlowChartX.LinearGradientBrush grBrush = null;
				grBrush = (MindFusion.FlowChartX.LinearGradientBrush) ShapeBrush;
				if ( grBrush == null )
					return true;

				m_InnerGradient = m_InnerDefs.InsertAfter(m_InnerDoc.CreateNode(XmlNodeType.Element,"linearGradient", sNsDefault), m_InnerDefs.LastChild);
				if ( m_InnerGradient == null )
					return false;

				sFore = SvgManager.Color2Str(grBrush.LinearColors[0]);
				sBack = SvgManager.Color2Str(grBrush.LinearColors[1]);

				if ( sFore == "none" )
					return  true;


				if ( sBack == "none" )
					return  true;

				sGradientName = String.Format("Gradient{0}", iGradientCount);
			
				m_InnerGradient.Attributes.Append(m_InnerDoc.CreateAttribute("id"));
				m_InnerGradient.Attributes["id"].Value = sGradientName;
				iGradientCount++;

				m_XmlTemp = m_InnerGradient.InsertAfter(m_InnerDoc.CreateNode(XmlNodeType.Element,"stop", sNsDefault), m_InnerGradient.LastChild);
				if ( m_XmlTemp == null )
					return false;

				m_XmlTemp.Attributes.Append(m_InnerDoc.CreateAttribute("offset"));
				m_XmlTemp.Attributes["offset"].Value = "5%";

				m_XmlTemp.Attributes.Append(m_InnerDoc.CreateAttribute("stop-color"));
				m_XmlTemp.Attributes["stop-color"].Value = sBack;


				m_XmlTemp = m_InnerGradient.InsertAfter(m_InnerDoc.CreateNode(XmlNodeType.Element,"stop", sNsDefault), m_InnerGradient.LastChild);
				if ( m_XmlTemp == null )
					return false;

				m_XmlTemp.Attributes.Append(m_InnerDoc.CreateAttribute("offset"));
				m_XmlTemp.Attributes["offset"].Value = "95%";

				m_XmlTemp.Attributes.Append(m_InnerDoc.CreateAttribute("stop-color"));
				m_XmlTemp.Attributes["stop-color"].Value = sFore;

				
			


				if ( ShapeNode.Attributes["fill"] == null )
					return false;

				ShapeNode.Attributes["fill"].Value = String.Format("url(#{0})", sGradientName);
			
					
				bOk = true;
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.AddGradient",ex.Message));
				bOk = false;
			}

			return bOk;
			
		}

		/// <summary>
		/// Saves inner XML into SVG file
		/// </summary>
		/// <param name="FileName">File name to save</param>
		public void  Save(string FileName)
		{
			if (IsValid())
			{
				m_InnerDoc.Save(FileName);
			}
            
		}

		

		/// <summary>
		/// Adds  new "path" nod to SVG file
		/// </summary>
		/// <returns>Returns recently added to SVG XML "path" node</returns>
		public XmlNode AddPath ( string PathData )
		{
			return  AddPath (  PathData ,  null , Color.Transparent, Color.Transparent );

		}

		/// <summary>
		/// Adds  new "path" nod to SVG file
		/// </summary>
		/// <param name="PathData">String path data in SVG format</param>
		/// <param name="sStrokeWidth">Frame with</param>
		/// <returns>Returns recently added to SVG XML "path" node</returns>
		public XmlNode AddPath ( string PathData , string sStrokeWidth)
		{
			return  AddPath (  PathData ,  sStrokeWidth, Color.Transparent, Color.Transparent );

		}

		/// <summary>
		/// Adds  new "path" nod to SVG file
		/// </summary>
		/// <param name="PathData">String path data in SVG format</param>
		/// <param name="sStrokeWidth">Frame with</param>
		/// <param name="colStroke">Frame color</param>
		/// <returns>Returns recently added to SVG XML "path" node</returns>
		public XmlNode AddPath ( string PathData , string sStrokeWidth, Color colStroke)
		{
			return  AddPath (  PathData ,  sStrokeWidth,colStroke, Color.Transparent );

		}

		/// <summary>
		/// Adds  new "path" nod to SVG file
		/// </summary>
		/// <param name="PathData">String path data in SVG format</param>
		/// <param name="sStrokeWidth">Frame with</param>
		/// <param name="colStroke">Frame color</param>
		/// <param name="colFill">Fill color</param>
		/// <returns>Returns recently added to SVG XML "path" node</returns>
		public XmlNode AddPath ( string PathData , string sStrokeWidth, Color colStroke , Color colFill)
		{
			if (!IsValid())
				return null;
			
			if ( PathData == "" )
				return null;

			m_InnerNode = m_InnerRoot.InsertAfter(m_InnerDoc.CreateNode(XmlNodeType.Element,"path", sNsDefault), m_InnerRoot.LastChild);
			if ( m_InnerNode!=null )
			{
				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("d"));
				m_InnerNode.Attributes["d"].Value = PathData;

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("stroke"));
				m_InnerNode.Attributes["stroke"].Value = SvgManager.Color2Str(colStroke);
				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("fill"));
				m_InnerNode.Attributes["fill"].Value = SvgManager.Color2Str(colFill);
			
				if ( sStrokeWidth!= null )
				{
					m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("stroke-width"));
					m_InnerNode.Attributes["stroke-width"].Value = sStrokeWidth;
				}

			}

			return m_InnerNode;

		}

		/// <summary>
		/// Adds new "rect" node to SVG
		/// </summary>
		/// <param name="sX">X - coord of the node</param>
		/// <param name="sY">Y - coord of the node</param>
		/// <param name="sWidth">Node width</param>
		/// <param name="sHeight">Node heaignt</param>
		/// <param name="sStrokeWidth">Frame width</param>
		/// <param name="colFill">Fill color</param>
		/// <param name="colStroke">Frame color</param>
		/// <returns>Returns recently added to SVG XML "rect" node</returns>
		public XmlNode AddRect(string sX, 
			string sY, 
			string sWidth, 
			string sHeight, 
			string sStrokeWidth,
			Color colFill, 
			Color colStroke)

		{
			return AddRect(sX,sY,sWidth, sHeight, sStrokeWidth, colFill, colStroke , null, null , null );
		}

		/// <summary>
		/// Adds new "rect" node to SVG
		/// </summary>
		/// <param name="sX">X - coord of the node</param>
		/// <param name="sY">Y - coord of the node</param>
		/// <param name="sWidth">Node width</param>
		/// <param name="sHeight">Node heaignt</param>
		/// <param name="sStrokeWidth">Frame width</param>
		/// <param name="colFill">Fill color</param>
		/// <param name="colStroke">Frame color</param>
		/// <param name="sRX">Bounds Z-size ( for bounding rects )</param>
		/// <param name="sRY">Bounds Y-size ( for bounding rects )</param>
		/// <returns>Returns recently added to SVG XML "rect" node</returns>
		public XmlNode AddRect(string sX, 
			string sY, 
			string sWidth, 
			string sHeight, 
			string sStrokeWidth,
			Color colFill, 
			Color colStroke,
			string sRX,
			string sRY)
		{
			return AddRect(sX,sY,sWidth, sHeight, sStrokeWidth, colFill, colStroke , sRX, sRY, null, Color.Empty, 0, RectangleF.Empty );
		}

		/// <summary>
		/// Adds new "rect" node to SVG
		/// </summary>
		/// <param name="sX">X - coord of the node</param>
		/// <param name="sY">Y - coord of the node</param>
		/// <param name="sWidth">Node width</param>
		/// <param name="sHeight">Node heaignt</param>
		/// <param name="sStrokeWidth">Frame width</param>
		/// <param name="colFill">Fill color</param>
		/// <param name="colStroke">Frame color</param>
		/// <param name="sRX">Bounds Z-size ( for bounding rects )</param>
		/// <param name="sRY">Bounds Y-size ( for bounding rects )</param>
		/// <param name="newImg">Node image(if any exists NULL otherwise)</param>
		/// <returns>Returns recently added to SVG XML "rect" node</returns>
		public XmlNode AddRect(string sX, 
			string sY, 
			string sWidth, 
			string sHeight, 
			string sStrokeWidth,
			Color colFill, 
			Color colStroke,
			string sRX,
			string sRY,
			System.Drawing.Image newImg	)
		{
			return AddRect(sX,sY,sWidth,sHeight,sStrokeWidth,colFill,colStroke, sRX,sRY, newImg, Color.Empty, 0 , RectangleF.Empty);
		}

		/// <summary>
		/// Adds new "rect" node to SVG
		/// </summary>
		/// <param name="sX">X - coord of the node</param>
		/// <param name="sY">Y - coord of the node</param>
		/// <param name="sWidth">Node width</param>
		/// <param name="sHeight">Node heaignt</param>
		/// <param name="sStrokeWidth">Frame width</param>
		/// <param name="colFill">Fill color</param>
		/// <param name="colStroke">Frame color</param>
		/// <param name="sRX">Bounds Z-size ( for bounding rects )</param>
		/// <param name="sRY">Bounds Y-size ( for bounding rects )</param>
		/// <param name="newImg">Node image(if any exists NULL otherwise)</param>
		/// <param name="Color2Repl">Color to replace transparent one in the image</param>
		/// <returns>Returns recently added to SVG XML "rect" node</returns>
		public XmlNode AddRect(string sX, 
			string sY, 
			string sWidth, 
			string sHeight, 
			string sStrokeWidth,
			Color colFill, 
			Color colStroke,
			string sRX,
			string sRY,
			System.Drawing.Image newImg,
			Color Color2Repl)
		{
			return AddRect(sX,sY,sWidth,sHeight,sStrokeWidth,colFill,colStroke, sRX,sRY, newImg, Color2Repl, 0  , RectangleF.Empty);
		}

		/// <summary>
		/// Adds new "rect" node to SVG
		/// </summary>
		/// <param name="sX">X - coord of the node</param>
		/// <param name="sY">Y - coord of the node</param>
		/// <param name="sWidth">Node width</param>
		/// <param name="sHeight">Node heaignt</param>
		/// <param name="sStrokeWidth">Frame width</param>
		/// <param name="colFill">Fill color</param>
		/// <param name="colStroke">Frame color</param>
		/// <param name="sRX">Bounds Z-size ( for bounding rects )</param>
		/// <param name="sRY">Bounds Y-size ( for bounding rects )</param>
		/// <param name="newImg">Node image(if any exists NULL otherwise)</param>
		/// <param name="Color2Repl">Color to replace transparent one in the image</param>
		/// <returns>Returns recently added to SVG XML "rect" node</returns>
		public XmlNode AddRect(string sX, 
			string sY, 
			string sWidth, 
			string sHeight, 
			string sStrokeWidth,
			Color colFill, 
			Color colStroke,
			string sRX,
			string sRY,
			System.Drawing.Image newImg,
			Color Color2Repl,
			ImageAlign ia,
			RectangleF rect)
		{

			bool IsImage = ( newImg != null );
			if (!IsValid())
				return null;
			
			m_InnerNode = m_InnerRoot.InsertAfter(m_InnerDoc.CreateNode(XmlNodeType.Element, IsImage ? "image":"rect" , sNsDefault), m_InnerRoot.LastChild);
			if ( m_InnerNode!=null )
			{
				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("x"));
				m_InnerNode.Attributes["x"].Value = sX;

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("y"));
				m_InnerNode.Attributes["y"].Value = sY;

				if ( sRX!=null )
				{
					m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("rx"));
					m_InnerNode.Attributes["rx"].Value = sRX;
				}
				
				if (sRY!=null)
				{
					m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("ry"));
					m_InnerNode.Attributes["ry"].Value = sRY;
				}

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("width"));
				m_InnerNode.Attributes["width"].Value = sWidth;

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("height"));
				m_InnerNode.Attributes["height"].Value = sHeight;

				if (!IsImage)
				{
					m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("stroke"));
					m_InnerNode.Attributes["stroke"].Value = SvgManager.Color2Str(colStroke);
				
			
					m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("fill"));
					m_InnerNode.Attributes["fill"].Value = SvgManager.Color2Str(colFill);
				
					m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("stroke-width"));
					m_InnerNode.Attributes["stroke-width"].Value = sStrokeWidth;
				}
				else
				{
					Image cloneImg = newImg.Clone() as System.Drawing.Image;
					m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("href", "http://www.w3.org/1999/xlink"));
					string sImg = String.Format("data:image/jpg;base64,{0}", SvgManager.Image2String(cloneImg, Color2Repl));
					m_InnerNode.Attributes["href"].Value = sImg;

					float fW = 0, fH = 0, fDX = 0, fDY = 0;

					if ( GetImageDim(cloneImg, rect, ia , ref fW, ref fH , ref fDX, ref fDY))
					{
						m_InnerNode.Attributes["width"].Value = Unit2PixStr(fW);
						m_InnerNode.Attributes["height"].Value = Unit2PixStr(fH);

						if (fDX!=0)
						{						
							m_InnerNode.Attributes["x"].Value = Unit2PixStr(fDX);
						}

						if (fDY!=0)
						{
							m_InnerNode.Attributes["y"].Value = Unit2PixStr(fDY);
						}
					}

				}
			
			}
			return m_InnerNode;
		}

		/// <summary>
		/// Adds new "ellipse" node to SVG
		/// </summary>
		/// <param name="sCX">X - coord of the ellispe center</param>
		/// <param name="sCY">Y - coord of the ellipse center</param>
		/// <param name="sRX">X - radius of the ellispe</param>
		/// <param name="sRY">Y - radius of the ellipse</param>
		/// <param name="sStrokeWidth">Frame width</param>
		/// <returns>Returns recently added to SVG XML "ellipse" node</returns>
		public XmlNode AddEllipse(string sCX, 
			string sCY, 
			string sRX,
			string sRY,
			string sStrokeWidth)
		{
			return AddEllipse(sCX,sCY, sRX,sRY,sStrokeWidth,Color.Transparent,Color.Transparent );
		}

		/// <summary>
		/// Adds new "ellipse" node to SVG
		/// </summary>
		/// <param name="sCX">X - coord of the ellispe center</param>
		/// <param name="sCY">Y - coord of the ellipse center</param>
		/// <param name="sRX">X - radius of the ellispe</param>
		/// <param name="sRY">Y - radius of the ellipse</param>
		/// <param name="sStrokeWidth">Frame width</param>
		/// <param name="colFill">Fill color</param>
		/// <param name="colStroke">Frame color</param>
		/// <returns>Returns recently added to SVG XML "ellipse" node</returns>
		public XmlNode AddEllipse(string sCX, 
			string sCY, 
			string sRX,
			string sRY,
			string sStrokeWidth,
			Color colFill, 
			Color colStroke
			)
		{
			if (!IsValid())
				return null;
			
			m_InnerNode = m_InnerRoot.InsertAfter(m_InnerDoc.CreateNode(XmlNodeType.Element,"ellipse", sNsDefault), m_InnerRoot.LastChild);
			if ( m_InnerNode!=null )
			{
				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("cx"));
				m_InnerNode.Attributes["cx"].Value = sCX;

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("cy"));
				m_InnerNode.Attributes["cy"].Value = sCY;

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("rx"));
				m_InnerNode.Attributes["rx"].Value = sRX;
		
				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("ry"));
				m_InnerNode.Attributes["ry"].Value = sRY;
				

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("stroke-width"));
				m_InnerNode.Attributes["stroke-width"].Value = sStrokeWidth;

				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("stroke"));
				m_InnerNode.Attributes["stroke"].Value = SvgManager.Color2Str(colStroke);
				
				m_InnerNode.Attributes.Append(m_InnerDoc.CreateAttribute("fill"));
				m_InnerNode.Attributes["fill"].Value = SvgManager.Color2Str(colFill);
						
			
			
			}
			return m_InnerNode;
		}
		bool IsValid()
		{
			return m_ObjectValid;
		}

		~SvgManager()
		{
			m_nsManager = null;
			m_InnerDoc = null;
			m_InnerNode = null;
		}

		private PointF[] docToLocal(PointF[] points, RectangleF rect)
		{
			PointF[] result = (PointF[])points.Clone();
			
			for (int i = 0; i < result.Length; i++)
			{
				result[i].X -= rect.X;
				result[i].Y -= rect.Y;
			}

			return result;
		}

		/// <summary>
		/// Callback internal routine called when text is being rended
		/// </summary>
		/// <param name="text">Rendered text</param>
		/// <param name="dest">Destination text area</param>
		/// <param name="hint">Hint object</param>
		internal void ___TextCallback(string text, RectangleF dest, DrawTextHint hint)
		{

		
			float fDX = 0, fDY = 0;

			sTextLine+=text;

			XmlNode new_tspan = null;
		
			if ( hint == null )
				return;

			if ( hint.rect == RectangleF.Empty )
				return;

			if ( hint.m_TextNode == null )
				return;

			if ( m_lastY==0 )
			{
				m_lastX = dest.X;
				m_lastY = dest.Y;
			}
			
			StyledText.StyledWord sword = null;
			if ( hint.CurrentWord is StyledText.StyledWord )
			{
				sword = hint.CurrentWord as StyledText.StyledWord;
			}
			

			new_tspan = hint.m_TextNode.InsertAfter(m_InnerDoc.CreateNode(XmlNodeType.Element,"tspan", sNsDefault), hint.m_TextNode.LastChild);
			if ( new_tspan!=null)
			{
				new_tspan.InnerText = text;

				if ( IsTable)
				{
					if (m_lastY != dest.Y)
					{
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("x"));
						new_tspan.Attributes["x"].Value = Unit2PixStr(hint.rect.X + hint.rect.Width/2);
				
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("dy"));
						new_tspan.Attributes["dy"].Value = Unit2PixStr(dest.Y -   m_lastY );
						m_lastY = dest.Y;
					}
				
				}
				else
				{
					if (!IsTransparent)
					{
						fDX = (dest.Width - 10)/2;
						fDY = (dest.Height -10)/2;
					}
					if (sword != null)
					{
						if ( sword.YOffset!=0)
						{
							fDY-=sword.YOffset;
							Trace.WriteLine(String.Format("SWORD OFFSET: {0}",sword.YOffset));
						}
					}
					
					if (!m_GlueLines )
					{
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("x"));
						new_tspan.Attributes["x"].Value = Unit2PixStr(hint.rect.X + dest.X + fDX);

						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("y"));
						new_tspan.Attributes["y"].Value = Unit2PixStr(hint.rect.Y +  dest.Y + fDY );
					}
					else
					{
						if (m_lastY != dest.Y)
						{
							new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("x"));
							new_tspan.Attributes["x"].Value = Unit2PixStr(hint.rect.X + hint.rect.Width/2);
							m_lastY = dest.Y;
						}			
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("y"));
						new_tspan.Attributes["y"].Value = Unit2PixStr(hint.rect.Y +  dest.Y + fDY );
						
						
					}
					
				}
		
				if ( hint.CurrentWord is StyledText.StyledWord )
				{
					
					new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("fill"));
					new_tspan.Attributes["fill"].Value = SvgManager.Color2Str(sword.Color);

					if (( sword.Format & Styles.Bold) >0 )
					{
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("font-weight"));
						new_tspan.Attributes["font-weight"].Value = "bold";
					}

			
					if (( sword.Format & Styles.Italic) >0 )
					{
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("font-style"));
						new_tspan.Attributes["font-style"].Value = "italic";
					}

					if (( sword.Format & Styles.Underline) >0 )
					{
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("text-decoration"));
						new_tspan.Attributes["text-decoration"].Value = "underline";
					}

					if (( sword.Format & Styles.Sup) >0 )
					{
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("baseline-shift"));
						new_tspan.Attributes["baseline-shift"].Value = "super";
					}

					if (( sword.Format & Styles.Sub) >0 )
					{
						new_tspan.Attributes.Append(m_InnerDoc.CreateAttribute("baseline-shift"));
						new_tspan.Attributes["baseline-shift"].Value = "sub";
					}
			
					
				}
				
			}
			
			;
		}

		/// <summary>
		/// Helper function to remove XML node
		/// </summary>
		/// <param name="Node">XML node</param>
		/// <param name="ChildName">Name of the child to be removed</param>
		/// <param name="IsNode">TRUE if the child is node FALSE if the attribute</param>
		/// <param name="ns">Namespace manager referense</param>
		/// <returns>TRUE if successfull otherwise FALSE</returns>
		public static bool RemoveNode(XmlNode Node, string ChildName , bool IsNode , XmlNamespaceManager ns)
		{
			bool bOk = false;
			try
			{
				if ( Node == null )
					return false;

				if ( ChildName == null )
					return false;

				
				if (IsNode)
				{
					XmlNode node_temp = null;

					node_temp = Node.SelectSingleNode(ChildName, ns);

					if ( node_temp == null )
						return false;

					Node.RemoveChild(node_temp);
				}
				else
				{
					Node.Attributes.RemoveNamedItem(ChildName);
				}

				bOk = true;

			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","RemoveNode",ex.Message));
				bOk = false;
			}

			return bOk;

		}

		
		/// <summary>
		/// Generate GUID to be used in
		/// </summary>
		/// <returns>String GUID</returns>
		public static string GetGUID()
		{
			string sGUID = null;
			try
			{
				System.Guid newGUID = Guid.NewGuid();
				sGUID = newGUID.ToString();
				sGUID = sGUID.ToUpper();
				sGUID = "{"+ sGUID + "}";
					
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","GetGUID",ex.Message));
				sGUID = null;
			}
			return sGUID;

		}

		
		/// <summary>
		/// Converts image to base64 encoded string
		/// </summary>
		/// <param name="img">[Image] object reference</param>
		/// <returns>Base64 encoded string BMP content of the image</returns>
		public static string Image2String ( System.Drawing.Image img)
		{
			string sImg = null, sTempFile = "$$$temp$$$.bmp";
			int BytesRead = 0;
			byte[] baImage = null;
			if ( img == null )
				return null;

			try
			{
				img.Save(sTempFile, System.Drawing.Imaging.ImageFormat.Jpeg);
				System.IO.FileStream fs = null;
				fs = new System.IO.FileStream(sTempFile, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
				if ( fs != null )
				{
					baImage = new byte[fs.Length];
					BytesRead = fs.Read(baImage, 0, (int) fs.Length);
					if ( BytesRead>0 )
						sImg = System.Convert.ToBase64String(baImage);
				}
				
				fs.Close();
				System.IO.File.Delete(sTempFile);
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","Image2String",ex.Message));
				sImg = null;
			}
			
			return sImg;
		}


		public bool GetImageDim(Image pict, RectangleF rect, ImageAlign ppos, ref float  picw, ref float pich , ref float xoff , ref float yoff)
		{
			bool bOk = false;
			int xc = 0, yc = 0;

			try
			{
			
				if ( rect.Equals(RectangleF.Empty))
					return false;

				picw = 0;
				pich = 0;
				xoff =0;
				yoff =0;

				Graphics g = System.Drawing.Graphics.FromHwnd(GetActiveWindow());
				setTransforms(g);

				// get image logical size in document coordinates
				RectangleF sizeDev = RectangleF.FromLTRB(0, 0,
					(float)pict.Size.Width / pict.HorizontalResolution * g.DpiX * g.PageScale,
					(float)pict.Size.Height / pict.VerticalResolution * g.DpiY * g.PageScale);
				RectangleF sizeDoc = deviceToDoc(g, sizeDev);
				picw = sizeDoc.Width;
				pich = sizeDoc.Height;

				
				switch (ppos)
				{
					case ImageAlign.TopLeft:
						xoff = rect.Left;
						yoff = rect.Top;
						break;
					case ImageAlign.BottomLeft:
						xoff = rect.Left;
						yoff = rect.Bottom - pich;
						break;
					case ImageAlign.TopRight:
						xoff = rect.Right - picw;
						yoff = rect.Top;
						break;
					case ImageAlign.BottomRight:
						xoff = rect.Right - picw;
						yoff = rect.Bottom - pich;
						break;
					case ImageAlign.Center:
						xoff = (rect.Right + rect.Left - picw) / 2;
						yoff = (rect.Bottom + rect.Top - pich) / 2;
						break;
					case ImageAlign.TopCenter:
						xoff = rect.X + rect.Width / 2 - picw / 2;
						yoff = rect.Y;
						break;
					case ImageAlign.BottomCenter:
						xoff = rect.X + rect.Width / 2 - picw / 2;
						yoff = rect.Bottom - pich;
						break;
					case ImageAlign.MiddleLeft:
						xoff = rect.X;
						yoff = rect.Y + rect.Height / 2 - pich / 2;
						break;
					case ImageAlign.MiddleRight:
						xoff = rect.Right - picw;
						yoff = rect.Y + rect.Height / 2 - pich / 2;
						break;
					case ImageAlign.Fit:
					{
						float h = rect.Bottom - rect.Top;
						float w = rect.Right - rect.Left;
						if (h == 0) break;

						float ratioCtrl = w / h;
						float ratioPic = picw / pich;

						if (ratioCtrl > ratioPic)
						{
							//stretch vertically
							pich = (int)h;
							picw = (int)(ratioPic * pich);
							yoff = rect.Top;
							xoff = (rect.Right + rect.Left - picw) / 2;
						}
						else
						{
							//stretch horizontally
							picw = (int)w;
							if (ratioPic == 0) break;
							pich = (int)(picw / ratioPic);
							xoff = rect.Left;
							yoff = (rect.Bottom + rect.Top - pich) / 2;
						}
					}
						break;
					case ImageAlign.Stretch:
					{
						picw = rect.Right - rect.Left;
						pich = rect.Bottom - rect.Top;
						xoff = rect.Left; yoff = rect.Top;
					}
						break;
					case ImageAlign.Tile:
					{
						xoff = rect.Left; yoff = rect.Top;
						xc = (int)((rect.Right - rect.Left) / picw) + 1;
						yc = (int)((rect.Bottom - rect.Top) / pich) + 1;
					}
						break;

					default:
						return false;
				}

				PointF center = new PointF(
					rect.X + rect.Width / 2,
					rect.Y + rect.Height / 2);
				bOk = true;
			}
		
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.GetImageDim",ex.Message));
				bOk = false;
			}
			
			return bOk;
		}

		public void setTransforms(Graphics g)
		{
			g.ResetTransform();
			g.TranslateTransform(-pChart.ScrollX,-pChart.ScrollY);
			g.PageUnit = pChart.MeasureUnit;
			g.PageScale = pChart.ZoomFactor / 100F;
		}

		public static RectangleF deviceToDoc(Graphics g, RectangleF r)
		{
			PointF[] pts = new PointF[4];

			// get the corner points
			pts[0].X = r.Left;
			pts[0].Y = r.Top;
			pts[1].X = r.Right;
			pts[1].Y = r.Top;
			pts[2].X = r.Right;
			pts[2].Y = r.Bottom;
			pts[3].X = r.Left;
			pts[3].Y = r.Bottom;

			// get the world coordinates
			g.TransformPoints(CoordinateSpace.World,
				CoordinateSpace.Device, pts);

			// return rectangle in world coordinates
			return RectangleF.FromLTRB(
				pts[0].X, pts[0].Y, pts[2].X, pts[2].Y);
		}

		/// <summary>
		/// Returns SVG path string for Arrow's head/base
		/// </summary>
		/// <param name="ahs">Arrow's head or base</param>
		/// <returns>string SVG path</returns>
		public string GetArrowHead (ArrowHeadShape ahs)
		{
			string sResult = "";
			int c = 0;
			float xoff = 0, yoff =0;

			try
			{

				for (int i = 0; i < ahs.Elements.GetLength(0); ++i)
				{
					EArrowHeadElement el = (EArrowHeadElement)ahs.Elements[i];
					switch (el)
					{
						case EArrowHeadElement.heLine:
						{
							float x1 = ahs.Coordinates[c++] + xoff;
							float y1 = ahs.Coordinates[c++] + yoff;
							float x2 = ahs.Coordinates[c++] + xoff;
							float y2 = ahs.Coordinates[c++] + yoff;

							sResult+=String.Format("M{0},{1} L{2},{3} ", Unit2Pix(x1), Unit2Pix(y1),Unit2Pix(x2), Unit2Pix(y2));
						
						}
							break;
						case EArrowHeadElement.heShape:
						{
							int pts = ahs.Elements[++i];
							PointF[] shape = new PointF[pts];	
						
							for (int j = 0; j < pts; ++j)
							{
								shape[j].X = ahs.Coordinates[c++] + xoff;
								shape[j].Y = ahs.Coordinates[c++] + yoff;
							}
							
							for (int j = 0; j < pts; ++j)
							{
								sResult+=String.Format("{0}{1},{2} ", j == 0 ? "M" : "L" , Unit2Pix(shape[j].X), Unit2Pix(shape[j].Y));
								
							}

						
						}
							break;
						case EArrowHeadElement.heCircle:
						{
							float cx = ahs.Coordinates[c++] + xoff;
							float cy = ahs.Coordinates[c++] + yoff;
							float r = ahs.Coordinates[c++];

							sResult+= String.Format("M{0},{1} A{2},{3} {4} {5},{6} {7},{8} " ,
								Unit2Pix(cx),
								Unit2Pix(cy),
								Unit2Pix(r), 
								Unit2Pix(r),
								0,
								1,
								1,
								Unit2Pix(cx),
								Unit2Pix(cy));
						
						}			
							break;

					}	
				}

				if ( sResult!="" )
					sResult+="z";
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.GetArrowHead",ex.Message));
				sResult = null;
			}
			
			return sResult;

		}
	}
	
}
