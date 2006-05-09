using System;
using System.IO;
using System.Xml;
using System.Reflection;
using System.Diagnostics;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using MindFusion.FlowChartX;
using MindFusion.FlowChartX.Text;
using System.Globalization;
using System.Collections;
using System.Runtime.InteropServices;


namespace MindFusion.Diagramming.Export
{
	///	<summary>
	///	Helper class intended to perform all operation 
	///	of DXF file elements creation
	///	</summary>
	///	
	internal class DxfHelper
	{
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
		/// Helper class intended for Flowchart boxes comparison
		/// when boxes are being sorted
		/// </summary>
		public class BoxComparer : IComparer
		{
			private bool asc = true;
	
			/// <summary>
			/// Comparer class constructor
			/// </summary>
			/// <param name="SortAsc">If true sorting ascending otherwise descending</param>
			public BoxComparer ( bool SortAsc )
			{
				asc = SortAsc;
			}
		
			/// <summary>
			/// Compares two elements of Flowchart.Box type
			/// </summary>
			/// <param name="x">First element of Flowchart.Box type</param>
			/// <param name="y">Second element of Flowchart.Box type</param>
			/// <returns>Comparision result</returns>
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
		/// Helper class intended for formatting strings for DXF groups
		/// </summary>
		public class UnitFormatter : ICustomFormatter, IFormatProvider
		{
			protected float UnitScale = 1f/12;
			NumberFormatInfo fi = null;
			protected long InnerID = 0x100;
			public ArrayList ar = null;
			public bool IsTrace = false;

			/// <summary>
			/// Call the method to start collecting handle IDs
			/// </summary>
			/// <param name="Start">if true start collecting otherwise false</param>
			public void ControlTracking( bool Start)
			{
				if ( ar == null )
                	ar = new ArrayList();
				
				if ( Start )
					ar.Clear();
					
				
				IsTrace = Start;
         	}


			/// <summary>
			/// UnitFormatter constructor
			/// </summary>
			/// <param name="scale">Scale factor for unit conversion ( default 1)</param>
			/// <param name="separator">Decimal separator</param>
			public UnitFormatter(float scale, string separator )
			{

				UnitScale = scale;
				fi = new NumberFormatInfo();
				fi.NumberDecimalSeparator = separator;
		
				
			}

			/// <summary>
			/// This method returns an object that implements ICustomFormatter to do the formatting
			/// </summary>
			/// <param name="argType">Type of the argument to be formatted</param>
			/// <returns>Formatter object for the type passed</returns>
			public object GetFormat( Type argType )
			{
				// Here, the same object (this) is returned, but it would 
				// be possible to return an object of a different type.
				if ( argType == typeof( ICustomFormatter ) )
					return this;
				else
					return  CultureInfo.CurrentCulture.GetFormat(argType);
			} 
        
			/// <summary>
			/// Returns next unique long utilized by 'handle'
			/// </summary>
			/// <returns>Unique long value</returns>
			public long NextID()
			{

				InnerID++;
				if (IsTrace)
				{
					if ( ar == null )
						ar = new ArrayList();
					
					ar.Add(InnerID);
						
				}
				
				return InnerID;
			}

			/// <summary>
			/// Check whether type passed is numeric one
			/// </summary>
			/// <param name="type">Type of the argument to be checked</param>
			/// <returns>Returns true if type passed is numeric one false otherwise</returns>
			private bool IsNumericType(Type type)
			{
				if (type == typeof(int)
					|| type == typeof(uint)
					|| type == typeof(long)
					|| type == typeof(ulong)
					|| type == typeof(short)
					|| type == typeof(ushort)
					|| type == typeof(byte)
					|| type == typeof(sbyte)
					|| type == typeof(char)
					|| type == typeof(float)
					|| type == typeof(double))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			
			/// <summary>
			/// This method does the custom formatting if it recognizes the 
			/// format specification
			/// </summary>
			/// <param name="formatString">Format string</param>
			/// <param name="argToBeFormatted">Argument to be formatted value</param>
			/// <param name="provider">Format provider object reference</param>
			/// <returns>Formatted string got from argument passed</returns>
			public string Format( string formatString, 
				object argToBeFormatted, IFormatProvider provider )
			{

				// HAN - Formatting argument as handle ( argument is suppressed by unique long value )
				if (formatString!=null ) 
				{
					if (formatString.Trim( )== "HAN")
						return String.Format("  5\n{0:X}\n", NextID());
				}

				// ID - Formatting argument as ID ( argument is formatted as Hexadecimal {0:X3})
				if (formatString!=null ) 
				{
					if (formatString.Trim( )== "ID")
					{
						if (IsNumericType(argToBeFormatted.GetType()))
							return String.Format("{0:X3}", argToBeFormatted);
						else
							return argToBeFormatted.ToString();
					}
				}

				// LIST - Formatting argument as list of handles 
				//( argument is suppressed by list of handles since last ControlTracking() call )
				if (formatString!=null ) 
				{
					if (formatString.Trim( )== "LIST")
					{
						if ( ar == null )
							return "";

						if ( ar.Count == 0 )
							return "";

						string sResult = "";

						Trace.WriteLine(String.Format("List consists of {0}", ar.Count));
						for ( int i = 0; i<ar.Count; i ++ )
						{

							sResult+=String.Format("  330\n{0:X3}\n", (long) ar[i]);
						}
						
						return String.Format("  97\n{0}\n{1}", ar.Count, sResult);
					}
				}

				// Checking whether argument passed has 'numeric' type
				double fValue = 0;
				if (IsNumericType(argToBeFormatted.GetType()))
				{

					try
					{
						if ( argToBeFormatted is Single )
							fValue = (Single) argToBeFormatted;
						else if (argToBeFormatted is Double)
							fValue = (Double) argToBeFormatted;
						else if (argToBeFormatted is Int64)
							fValue = (Int64) argToBeFormatted;
						else
							fValue = (Int32) argToBeFormatted;
						//fValue = ( argToBeFormatted is Single) ? ( Single ) argToBeFormatted : (Double) argToBeFormatted;
					}
					catch (Exception)
					{
						Trace.WriteLine("Can't format numeric\n");
					}
					
					// U - Formatting argument as measure unit ( argument is suppressed by unit value * UnitScale )
					if (formatString!=null ) 
					{
					
						if ( formatString.Trim( ).StartsWith( "U" ))
                            fValue*=UnitScale;
						
					}
					return String.Format(fi,"{0}", fValue);

				}
				

				// ACI - Formatting argument as color ( argument is suppressed by ACI - Autocad Color Index )
				if (argToBeFormatted is Color)
				{
					if (formatString!=null ) 
					{
						if ( formatString.Trim( )== "ACI")
							return String.Format("{0}", Color2Aci((Color) argToBeFormatted ));
					}
				}	
			
				// ATEXT - Formatting argument as DXF text  ( RESERVED FOR FUTURE USE )
				if (argToBeFormatted is Font)
				{
					if (formatString!=null ) 
					{
						if ( formatString.Trim( )== "ATEXT")
						{
							return "";
							
						}
					}
				}	

				// DASH_STYLE - Formatting argument as DXF LTYPE ( argument is suppressed by DXF line type value got from DashStyle )
				if (argToBeFormatted is DashStyle)
				{
					if (formatString!=null ) 
					{
						if ( formatString.Trim( )== "DASH_STYLE")
						{
							if (( (DashStyle) argToBeFormatted  == DashStyle.Solid ) ||
								( (DashStyle) argToBeFormatted  == DashStyle.Custom ))
								return  "Continuous";
							else
								return argToBeFormatted.ToString().ToUpper();
						}
					}
				}

				// SAL - Formatting argument as DXF text align ( argument is  DXF align group codes got from LineAlignment ans Alignment )
				if (argToBeFormatted is StringFormat)
				{
					if (formatString!=null ) 
					{
						if ( formatString.Trim( )== "SAL")
						{
							
							if ( (argToBeFormatted as StringFormat).FormatFlags == StringFormatFlags.FitBlackBox)
								return "8";

							StringAlignment sa = (argToBeFormatted as StringFormat).Alignment;
							StringAlignment la = (argToBeFormatted as StringFormat).LineAlignment;
							
							long RetAlign = 0;

							
							switch (la)
							{
								case StringAlignment.Near:
									RetAlign  = 1;
									break;
								case StringAlignment.Center:
									RetAlign = 4;
									break;
								case StringAlignment.Far:
									//RetAlign = 1;
									RetAlign = 7;
									break;
							}
							

							switch (sa)
							{
								case StringAlignment.Near:
									RetAlign+=0;
									break;
								case StringAlignment.Center:
									RetAlign+=1;
									break;
								case StringAlignment.Far:
									RetAlign+=0;
									//RetAlign+=2;
									break;
								
							}
							
							return RetAlign.ToString();
						}
					}
				}	

				// Formatting common arguments
				if( argToBeFormatted is IFormattable )
					return ( (IFormattable)argToBeFormatted ).
						ToString( formatString, provider );
				else
					return argToBeFormatted.ToString( );
			}
				 
		}

		/// <summary>
		/// Enum consist of possible line types ( used by Ps2String())
		/// </summary>
		public enum DxLineType
		{
			ltSingle,
			ltClosedSingle,
			ltPoly,
			ltArrow,
			ltBezier,
			ltHatch,
			ltVertex
		};

		/// <summary>
		/// Enum consists of possible arrow head element types
		/// </summary>
		internal enum EArrowHeadElement
		{
			heLine,
			heShape,
			heCircle
		};

		/// <summary>
		/// Helper class constructor
		/// </summary>
		/// <param name="filePath">Path of the DXF file</param>
		/// <param name="fChart">Flowchart object reference</param>
		/// <param name="ExportTextAsMultiline">ExportTextAsMultiline property value</param>
		/// <param name="ExportExternalImages">ExportExternalImages property value</param>
		/// <param name="NamePattern">Image files name pattern</param>
		/// <param name="ImagePath">Path for image files saving</param>
		public DxfHelper( string filePath, FlowChart fChart, bool ExportTextAsMultiline, bool ExportExternalImages, 
			              string NamePattern, string ImagePath)
		{
			try
			{
			
				// Preparing for writing DXF file
				m_filePath = filePath;
				m_StreamWriter = new StreamWriter(filePath,	false ,	Encoding.ASCII);
				if ( m_StreamWriter	== null)
					throw new Exception("Can't open	DXF	file");

				m_ExportTextAsMultiline = ExportTextAsMultiline;
				m_ExportExternalImages = ExportExternalImages;
				provider = new UnitFormatter( m_UnitScale, ".");
				m_FlowChart = fChart;
				FileNamePattern = NamePattern;
				BasedPath = ImagePath;
				doc = new XmlDocument();

				if ( doc == null )
					throw new Exception("Can't create XML repository");

				// Getting temporarily XML storage from resource
				try
				{
					StreamReader sr = new StreamReader(Assembly.GetExecutingAssembly().
						GetManifestResourceStream("MindFusion.Diagramming.Export.DxfExporter.xml"),Encoding.ASCII);
					if ( sr == null )
						throw new Exception("Can't open stream");

					string XmlContent = sr.ReadToEnd();
					if (XmlContent == "")
						throw new Exception("Can't read content");

					doc.LoadXml(XmlContent);
			
				}
				catch (Exception)				
				{
					throw new Exception("Error loading XML template from resource");
				}
				
				
				// Hashing some temporarily XML nodes for future use
				nodeRoot = doc.DocumentElement;
				
				if ( nodeRoot == null )
					throw new Exception("Can't create nodeRoot");

				HashNode("header", ref nodeHeader);
				HashNode("tables", ref nodeTables);
				HashNode("tables/ltypes", ref nodeLtypes);
				HashNode("tables/style", ref nodeStyle);
				HashNode("tables/layer", ref nodeLayers);
				HashNode("blocks", ref nodeBlocks);
				HashNode("entities", ref nodeEntities);
				HashNode("objects", ref nodeObjects);
				
				// Setting background group				
				XmlNode node = nodeEntities.AppendChild(doc.CreateNode(XmlNodeType.Element,BACKGROUND_LAYER, ""));
				if ( node != null )
					node.InnerText = String.Format(provider, "  0\nSOLID\n  100\nAcDbEntity\n    5\n{10:X3}\n  8\n{9}\n  62\n{8:ACI}\n  100\nAcDbTrace\n  10\n{0:U}\n  20\n{1:U}\n  11\n{2:U}\n  21\n{3:U}\n  12\n{4:U}\n  22\n{5:U}\n  13\n{6:U}\n  23\n{7:U}\n",
						0, 0,
						m_FlowChart.DocExtents.Width, 0,
						0, m_FlowChart.DocExtents.Height,
						m_FlowChart.DocExtents.Width ,m_FlowChart.DocExtents.Height,
						m_FlowChart.BackColor,
						BACKGROUND_LAYER,
						99);
			
				
				// Indicates that object was initialized successfully
				m_valid	= true;
			}
				
			catch (	Exception ex )
			{
				m_valid	= false;
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","DxfHelper.Construct",ex.Message));
			}

		

		}

		/// <summary>
		/// Static method converts Color into ACI
		/// </summary>
		/// <param name="cr">Color value</param>
		/// <returns>Corresponded autocad color index value</returns>
		public static int Color2Aci(Color cr)
		{
			int iColor = 0, iDiff=0, iMinDiff=0;
		

			for ( int i=0;i<256;i++ )
			{
            
				iDiff = Math.Abs(cr.R - ACIs[i].R) + Math.Abs(cr.G - ACIs[i].G) + Math.Abs(cr.B - ACIs[i].B);
				if ( i==0 )
					iMinDiff = iDiff;

				if ( iDiff<=iMinDiff )
				{
					iMinDiff = iDiff;
					iColor = i;
				}
			}
			return iColor;
		}

		/// <summary>
		/// Saves content of the temporarily XML into output DXF file
		/// </summary>
		/// <returns>true if succeded otherwise false</returns>
		public bool Save()
		{
			// Updating last available handle value for using in AUTOCAD
			SetParam(nodeHeader, "HANDSEED", String.Format(provider, "  9\n$HANDSEED\n{0:HAN}",0));

			// Recursive processing all nodes of  temporarily XML
			ProcessNode(nodeRoot);

			// Writing 'End-Of-File' mark into DXF
			if ( m_StreamWriter	!= null)
			{
				m_StreamWriter.Write("  0\nEOF\n");
				m_StreamWriter.Flush();
				m_StreamWriter.Close();
				m_StreamWriter = null;
			}


			return true;
		}

		/// <summary>
		/// Helper class destructor
		/// </summary>
		~DxfHelper()
		{
			try
			{
		
				m_filePath = "";
				

			}
			catch (	Exception ex )
			{
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","DxfHelper.Destruct",ex.Message));
			}
		}

		/// <summary>
		/// Check whether class instance if valid
		/// </summary>
		/// <returns>true if valid otherwise false</returns>
		public bool	IsValid()
		{
			return m_valid;
		}

		/// <summary>
		/// Returns class instance status
		/// </summary>
		/// <returns>Status value</returns>
		public string GetStatus()
		{
			return m_status;
		}

	
	
		/// <summary>
		/// Produces DXF output from various type of lines
		/// </summary>
		/// <param name="pt">Point array of the line points</param>
		/// <param name="crLine">Line color</param>
		/// <param name="LayerName">Name of the layer to place the line</param>
		/// <param name="dlt">Line type according to DxLineType enum</param>
		/// <param name="dash">Line dash style</param>
		/// <param name="LineWidth">Used for bezier curves ( if 8 - points, 16 - controls points )</param>
		/// <returns>DXF output string</returns>
		public string Pt2String(PointF[] pt, Color crLine , string LayerName, DxLineType dlt , DashStyle dash , Single LineWidth )
		{

			string sResult = "";
			bool IsClosedLine = false;
			long SegCount = 0;
			bool FirstOrLast = false;

			try
			{
				// Validating input parameters
				if ( pt == null )
					throw new Exception("Null points array passed");

				if ( pt.Length == 0 )
					throw new Exception("Empty points array passed");

				IsClosedLine = (pt[0] == pt[pt.Length-1]);
				if (( dlt == DxLineType.ltSingle) || (dlt == DxLineType.ltClosedSingle))
					SegCount = pt.Length - 1;
				else
					SegCount = pt.Length;

				// Processing all line points according to its type
				for ( int i=0; i< SegCount ; i++)
				{
					FirstOrLast =((i ==0) || (i == SegCount-1));

					switch ( dlt)
					{

						case DxLineType.ltSingle:
						case DxLineType.ltClosedSingle:
							sResult+=String.Format(provider, "0\nLINE\n  100\nAcDbEntity\n{0:HAN}  8\n{1}\n  6\n{6:DASH_STYLE}\n62\n{0:ACI}\n  100\nAcDbLine\n  10\n{2:U}\n  20\n{3:U}\n  11\n{4:U}\n  21\n{5:U}\n",
								crLine, LayerName, pt[i].X, pt[i].Y, pt[i+1].X, pt[i+1].Y, dash);
						
							break;
						case DxLineType.ltBezier:
						sResult+=String.Format(provider, "  0\nVERTEX\n  100\nAcDbEntity\n{0:HAN}  8\n{2}\n  62\n{4:ACI}\n100\nAcDbVertex\n  100\nAcDb2dVertex\n  10\n{0:U}\n  20\n{1:U}\n  70\n{3}\n", pt[i].X, pt[i].Y, LayerName, 16 , crLine, dash);
							break;

						case DxLineType.ltVertex:
							sResult+=String.Format(provider, "  0\nVERTEX\n  100\nAcDbEntity\n{0:HAN}  8\n{2}\n  62\n{4:ACI}\n100\nAcDbVertex\n  100\nAcDb2dVertex\n  10\n{0:U}\n  20\n{1:U}\n  70\n{3}\n", pt[i].X, pt[i].Y, LayerName, LineWidth, crLine);
							break;
						case DxLineType.ltPoly:
							sResult+=String.Format(provider, "  10\n{0:U}\n  20\n{1:U}\n", pt[i].X, pt[i].Y);
							break;
						case DxLineType.ltHatch:
							sResult+=String.Format(provider, "  10\n{0:U}\n  20\n{1:U}\n", pt[i].X, pt[i].Y);
							break;
						default:
							throw new Exception("Line type is not supported");
					}

					
				}
		
				// Post - processing some types of line
				if  (dlt == DxLineType.ltClosedSingle)
					sResult+=String.Format(provider, "0\nLINE\n  100\nAcDbEntity\n{0:HAN}  8\n{1}\n  6\n{6:DASH_STYLE}\n62\n{0:ACI}\n  100\nAcDbLine\n  10\n{2:U}\n  20\n{3:U}\n  11\n{4:U}\n  21\n{5:U}\n",
						crLine, LayerName, pt[pt.Length - 1].X, pt[pt.Length - 1].Y, pt[0].X, pt[0].Y, dash);
				
				
				if ( dlt == DxLineType.ltPoly )
					sResult = String.Format(provider, "0\nLWPOLYLINE\n  100\nAcDbEntity\n{0:HAN}  8\n{1}\n  6\n{5:DASH_STYLE}\n62\n{0:ACI}\n  100\nAcDbPolyline\n  90\n{2}\n  70\n{4}\n{3}", 
						crLine, LayerName, pt.Length , sResult, IsClosedLine ? "1" : "0", dash);

				
				if ( dlt == DxLineType.ltBezier )
				{
					GraphicsPath gr_temp = new GraphicsPath(FillMode.Winding);
					gr_temp.AddBeziers(pt);

					gr_temp.Flatten();
					PointF[] pts2 = gr_temp.PathData.Points as PointF[];
									
					sResult = String.Format(provider, "0\nPOLYLINE\n{0:HAN}   100\nAcDbEntity\n 8\n{0}\n  6\n{4:DASH_STYLE}\n  62\n{1:ACI}\n  100\nAcDb2dPolyline\n  66\n1\n  70\n4\n  75\n6\n{2}{3}  0\nSEQEND\n  100\nAcDbEntity\n{0:HAN}", 
						LayerName,
						crLine, 
						sResult,
						Pt2String(pts2,crLine,LayerName,DxLineType.ltVertex, dash, 8), dash);

					gr_temp.Dispose();
				}

			}
			catch ( Exception ex)
			{
				sResult = "";
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","DxfHelper.Pt2String",ex.Message));
			}
			
			return sResult;								
		}

		/// <summary>
		/// Converts Rectangle into point array
		/// </summary>
		/// <param name="rect">Rectangle to be converted</param>
		/// <returns>Corresponded point array</returns>
		PointF[] Rect2Pts(RectangleF rect)
		{

			PointF[] pts = null;
			try
			{
			
				pts = new PointF[4]{new PointF(rect.Left, m_FlowChart.DocExtents.Height - rect.Top),
									   new PointF(rect.Right, m_FlowChart.DocExtents.Height -rect.Top),
									   new PointF(rect.Right, m_FlowChart.DocExtents.Height -rect.Bottom),
									   new PointF(rect.Left, m_FlowChart.DocExtents.Height - rect.Bottom)};
									   //new PointF(rect.Left, m_FlowChart.DocExtents.Height - rect.Top)};
		
			}
			catch ( Exception ex)
			{
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","DxfHelper.Rect2Pts",ex.Message));
			
			}
			return pts;
		}

		/// <summary>
		/// Converts PointCollection object into point array
		/// </summary>
		/// <param name="ptc">PointCollection to be converted</param>
		/// <returns>Corresponded point array</returns>
		PointF[] PtColl2Pts(PointCollection ptc)
		{
			PointF[] pts = null;
			try
			{
				
				pts = new PointF[ptc.Count];
				for (int i=0;i<ptc.Count;i++)
				{
					pts[i]=ptc[i];
					pts[i].Y = m_FlowChart.DocExtents.Height - ptc[i].Y;
				}
			}
			catch ( Exception ex)
			{
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","DxfHelper.PtColl2Pts",ex.Message));
			
			}

			Trace.WriteLine(String.Format("PtColl2Pts {0} -> {1}", ptc.Count, pts.Length));
			return pts;
		}

		/// <summary>
		/// Check whether temporarily XML node is pre-defined sub-root
		/// </summary>
		/// <param name="node">Node name</param>
		/// <returns>true if node is sub-root</returns>
		public static bool IsPredefined(XmlNode node)
		{
			bool bOk = false;
			try
			{
				if ( node.Attributes!=null )
				{
					if ( node.Attributes["predefined"]!= null )
						bOk = true;
				}

			}
			catch (Exception)
			{
				bOk = false;
			}

			return bOk;
		}
		
		/// <summary>
		/// Common method converts any Flowchart object into corresponded
		/// DXF string
		/// </summary>
		/// <param name="co">Flowchart object</param>
		/// <returns>Corresponded DXF string</returns>
		public string ChartObject2String(ChartObject co)
		{
			
			string sResult = null;
			RectangleF rect = RectangleF.Empty;
			PointF pt0 = PointF.Empty, 
				pt1 = PointF.Empty,
				pt2 = PointF.Empty,
				pt3 = PointF.Empty;

			Color crFill = Color.Transparent , crLine = Color.Transparent;
			string sFill = "";
			PointF[] pts = null;
			GraphicsPath gr = null;
			StringFormat centerFormat = null;
			RectangleF captionRect = RectangleF.Empty;
			try
			{


				centerFormat = new StringFormat();

				if ( centerFormat!=null )
				{
					centerFormat.Alignment = StringAlignment.Center;
					centerFormat.LineAlignment = StringAlignment.Center;

				}
				
				// If porcessed object is Box
				if ( co	is Box)
				{
					Box oBox = co as Box;
					rect = oBox.BoundingRect;
				
				
						gr = new GraphicsPath(FillMode.Winding);
						sResult = "";
						provider.ControlTracking(true);
						Box2Str(oBox,0,ref gr,ref sResult);
						provider.ControlTracking(false);
				
						// Getting fill color
						crFill = ExtractColor(oBox.Brush, oBox.FillColor);

					    // Checking does the box has embedded image
						if ( oBox.Image == null )
						{
							if (!IsTransparent(crFill, oBox))
							{
								// Processing point array to eliminate duplicated points
								// ( required by DXF POLYLINE )
								pts = MakePtsUnique(gr.PathPoints as PointF[]);
										
								sFill = String.Format(provider, "0\nHATCH\n  100\nAcDbEntity\n{0:HAN}  8\n{0}\n  62\n{10:ACI}\n  100\nAcDbHatch\n  10\n0\n  20\n0\n 30\n0\n  210\n0\n  220\n0\n  230\n1\n  2\n{1}\n  70\n{2}\n  71\n{3}\n  91\n{4}\n  92\n{5}\n  72\n{6}\n  73\n{7}\n  93\n{8}\n{9}{0:LIST}  75\n0\n  76\n1\n  47\n1\n 98\n0\n",
									HATCH_LAYER,"SOLID", "1", "1", "1","7", "0", "1", pts.Length  , 
									Pt2String(pts,crFill, HATCH_LAYER, DxLineType.ltHatch, DashStyle.Solid, 1), 
									crFill);
							
						
								AddEntity(String.Format("FILL{0:X3}",oBox.ZIndex),sFill);
					
							}
						}
						else
						{
							AddImage(oBox.BoundingRect, oBox.Image.Clone() as Image, NextID(), oBox.ImageAlign,m_FlowChart.BackColor);
						}
					
					if ( !oBox.Transparent )
						AddEntity(String.Format("BOX{0:X3}", oBox.ZIndex),sResult);
					
					AddText(oBox);

					if ( gr!=null )
						gr.Dispose();

				}
				else if	( co is	Arrow )  // if object is Arrow
				{

					Arrow oArrow = co as Arrow;
					
					sResult = Pt2String(PtColl2Pts(oArrow.ControlPoints),oArrow.Pen.Color, ARROW_LAYER,
						(oArrow.Style == ArrowStyle.Bezier) ? DxLineType.ltBezier : DxLineType.ltSingle, oArrow.Pen.DashStyle, oArrow.Pen.Width);

					AddEntity(String.Format("ARROW{0:X3}", oArrow.ZIndex),sResult);
					
				
					AddText(oArrow);
					
					sFill = "";
					crFill = ExtractColor(oArrow.Brush, oArrow.Pen.Color);
					sResult = ArrowHead2Str(oArrow.BaseShape, oArrow.Pen.Color, oArrow.Pen.DashStyle ,  crFill, ref sFill);
					AddEntity(String.Format("ABASE{0:X3}", oArrow.ZIndex),sResult);

					if ( sFill!="" )
						AddEntity(String.Format("FILLBASE{0:X3}",oArrow.ZIndex),sFill);
                    

					sFill = "";
					sResult = ArrowHead2Str(oArrow.HeadShape, oArrow.Pen.Color, oArrow.Pen.DashStyle , crFill, ref sFill);
					AddEntity(String.Format("AHEAD{0:X3}", oArrow.ZIndex),sResult);
					if ( sFill!="" )
						AddEntity(String.Format("FILLHEAD{0:X3}",oArrow.ZIndex),sFill);
				}
				else if ( co is Table)  // If object is table
				{
					Table oTable = co as Table;
					rect = oTable.BoundingRect;

					float fX = 0,  fY = 0, fW = 0, fH = 0;
					int RowCount = oTable.CurrScrollRow , ColCount = 0, AutoColls = 0;
					float fCellW = 0, fCellH = 0, fCellX = 0, fCellY = 0 , rh =0, fStepW = 0, fSumW =0;
					bool CellIsSpanned = false;
					string sTemp = "";

					fX = rect.X;
					fY = rect.Y;
					fW = rect.Width;
					fH = rect.Height;

					// Processing table's frame
				
					crFill =  ExtractColor(oTable.Brush, oTable.FillColor);
					AddRect(rect, oTable.ZIndex, Color.Transparent, crFill, oTable.TextColor,oTable.Pen.DashStyle,
						"", null, null);

					// Processing table caption's frame
					if ( oTable.CaptionHeight!=0 )
					{
						AddText(oTable);
						captionRect = new RectangleF(fX, fY, fW, oTable.CaptionHeight);
						
					}
				
					// Pre-process table cells
					sTemp = "";
					fW = 0;
					fH = 0;
					fCellX = fX;
					fCellY = fY + oTable.CaptionHeight;

					AutoColls = 0;
					fSumW =0;
					foreach ( Table.Column col in oTable.Columns)
					{
						if (col.ColumnStyle == ColumnStyle.AutoWidth )
							AutoColls++;
						else
							fSumW+=col.Width;
					}


					// Process table cells considering 'spaning cells'
					while (RowCount<oTable.Rows.Count)
					{

						if ( RowCount!=oTable.CurrScrollRow  )
							rh =  oTable.Rows[RowCount-1].Height;
						else
							rh = 0;

						fCellY+=rh;
						if ( fCellY > oTable.BoundingRect.Bottom - rh)
							break;

						while (ColCount<oTable.Columns.Count )
						{
							if ( ColCount!=0)
								fCellX+= oTable.Columns[ColCount-1].Width;

							Table.Cell cell = oTable[ColCount, RowCount];

							
							if ( cell.Tag is string)
								sTemp = cell.Tag.ToString();
							else
								sTemp = "";

							if (sTemp!="span" )
								{
							
						
									fCellH = 0;
									for (int i = RowCount; i <RowCount + cell.RowSpan; i++)
									{

										fCellH+= oTable.Rows[i].Height;
										fCellW = 0;
							
										bool bPass = true;
										for (int j = ColCount; j < ColCount + cell.ColumnSpan; j++)
										{
											if (( oTable.Columns[ColCount].ColumnStyle == ColumnStyle.AutoWidth ) && bPass)
											{
												fStepW = ( oTable.BoundingRect.Width - fSumW ) / AutoColls;
												bPass = false;
											}
											else
												fStepW = oTable.Columns[j].Width;
										
											fCellW+=fStepW;
											oTable[j,i].Tag = "span";
											
										}
									}

		
									if ( AutoColls == 0 )
									{
										if ( ColCount == ( oTable.Columns.Count -1) )
											fCellW += ( oTable.BoundingRect.Width - fSumW);
										
									}

								    // Calculating cell's rectangle dimensions
									RectangleF cellRect = new RectangleF(fCellX,
										fCellY,
										fCellW,
										fCellH); 
						

						
									CellIsSpanned = ((cell.ColumnSpan>1) || ( cell.RowSpan>1));
									
									if ( oTable.CellFrameStyle == CellFrameStyle.None)
										crLine = Color.Transparent;
									else
										crLine = oTable.FrameColor;

								    // Adding cell's frame and fill
									AddRect(cellRect, oTable.ZIndex, crLine , crFill, oTable.TextColor, oTable.Pen.DashStyle,
										"", /*cell.Text*/ ( oTable.Font == null ) ? m_FlowChart.Font :  oTable.Font , CellIsSpanned ? centerFormat : cell.TextFormat);
							
								    
								    // Adding cell's text
									AddText(cell, oTable, cellRect );

								    // Adding cell's picture if any exists
									if (cell.Image != null)
										AddImage(cellRect, cell.Image.Clone() as Image, NextID(), cell.ImageAlign, oTable.FillColor);

								}
							ColCount++;
					
						
						}

						fCellX = fX;
						ColCount = 0;
						RowCount++;
				
					
					}

				

					// Adding caption's fill
					if ( oTable.CaptionHeight!=0 )
					{
						AddRect(captionRect, oTable.ZIndex, oTable.FrameColor, Color.Transparent, oTable.CaptionColor, oTable.Pen.DashStyle,
							oTable.CaptionPlainText, ( oTable.Font == null ) ? m_FlowChart.Font :  oTable.Font , oTable.CaptionFormat);

						
					}

					// Adding table body fill
					AddRect(rect, oTable.ZIndex, oTable.Pen.Color, Color.Transparent, oTable.TextColor,oTable.Pen.DashStyle,
						"", null, null);
					
				
				

				}
				else
				{
					throw new Exception("Unknown chart object type detected");
				}
			}
		
			catch (	Exception ex )
			{
				sResult = null;
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","ChartObject2String",ex.Message));
			}
		
			return sResult;

		}
		
		/// <summary>
		/// Method inserts DXF string definition for the entity into temporarily XML
		/// </summary>
		/// <param name="Name">Definition name</param>
		/// <param name="Value">Definition value</param>
		/// <returns>true if successfull otherwise false</returns>
		public bool AddEntity(string Name, string Value)
		{

			return AddNode(Name,Value,nodeEntities);
		}

		/// <summary>
		/// Method inserts DXF string definition for the object into temporarily XML
		/// </summary>
		/// <param name="Name">Definition name</param>
		/// <param name="Value">Definition value</param>
		/// <returns>true if successfull otherwise false</returns>
		public bool AddObject(string Name, string Value)
		{

			return AddNode(Name,Value,nodeObjects);
		}

		/// <summary>
		/// Method inserts DXF string definition into temporarily XML
		/// </summary>
		/// <param name="Name">Definition name</param>
		/// <param name="Value">Definition value</param>
		/// <param name="ParentNode">Parent XML node name</param>
		/// <returns>true if successfull otherwise false</returns>
		public bool AddNode(string Name, string Value, XmlNode ParentNode)
		{
			bool bOk = false;
			try
			{
				if ( Name == null )
					throw new Exception("Empty entity name is passed");

				if ( Name == "" )
					throw new Exception("Empty entity name is passed");

				if ( Value == null )
					throw new Exception("Empty entity value is passed");

				if ( Value == "" )
					throw new Exception("Empty entity value is passed");

				XmlNode newNode = null;
				newNode = ParentNode.AppendChild(doc.CreateNode(XmlNodeType.Element,Name, ""));
				if ( newNode != null )
				{
					newNode.InnerText = Value;
					bOk = true;
				}
			}
			catch (	Exception ex )
			{
				bOk= false;
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0}:{2} error {1}\n","AddEntity",ex.Message, Name.ToString()));
			}

			return bOk;
		}

		/// <summary>
		/// Get attributes' value of XML node
		/// </summary>
		/// <param name="node">Node name</param>
		/// <param name="AttrName">Attribute name</param>
		/// <returns>Attribute value as string</returns>
		protected string Attr2String(XmlNode node, string AttrName)
		{
			string sResult = null;
			try
			{
				if (node.Name=="#document" )
					return null;
			
				if (node.Name=="root" )
					return null;

				if (node.Name=="xml" )
					return null;

				if (node.Attributes != null)
					sResult = node.Attributes[AttrName].Value.ToString();
			}
			catch (	Exception ex )
			{
				sResult = "";
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","Attr2String",ex.Message));
			}

			return sResult;
		}

		/// <summary>
		/// Counts child node of the XML node to eliminate 'empty' nodes
		/// </summary>
		/// <param name="node">Node name</param>
		/// <returns>Childs count</returns>
		protected long CountChilds(XmlNode node)
		{
			long lCount = 0;

			try
			{
				if ( node == null )
					return lCount;

				if ( node.ChildNodes.Count == 0 )
				{
					if (node.InnerText!="")
						lCount++;
					return lCount;
				}
				foreach ( XmlNode next in node.ChildNodes)
				{
					lCount+=CountChilds(next);
				}
			}
			catch (	Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","CountChilds",ex.Message));
			}
			
			return lCount;

		}

		/// <summary>
		/// Process node of the temporarily XML saving it into  DXF file
		/// </summary>
		/// <param name="node">Node name</param>
		/// <returns>true if successful otherwise false</returns>
		protected bool ProcessNode(XmlNode node)
		{
			
			string sValue = "";
			bool IsAlone = false, IsPredef = false;
			bool bOk = true;

			if ( node == null )
				return false;


			try
			{
				

				Trace.WriteLine(String.Format("Processing {0} node : {1}\n", IsPredefined(node)?"predefined": "",  node.Name));
				IsAlone  = false;
				IsPredef = IsPredefined(node);

				if ( node.ChildNodes.Count == 0 )
					IsAlone = true;
				else
					if ( node.FirstChild.NodeType == XmlNodeType.Text )
					IsAlone = true;

				if (( IsPredef) && ( CountChilds(node)!=0))
				{
					sValue = Attr2String(node,"before");
					if ( sValue!=null )
					{
					
						sValue = String.Format("  0\n{0}\n  2\n{1}\n", ( sValue == "" ) ? "SECTION" : sValue,
							node.Name.ToString().ToUpper());

						if ( CountChilds(node)!=0)
							m_StreamWriter.Write(sValue);

				
					}
				}
				
				if ( IsAlone )
				{
					
					

					if (IsPredef)
					{
						if ( sValue!=null )
						{
							if (node.Value != null)
							{
								sValue = node.Value.ToString();
								m_StreamWriter.Write(sValue);
							}
						}
					
					}
					else
					{
						if ( node.InnerText!="")
							m_StreamWriter.Write(node.InnerText);
						
					}

				
				}
				else
				{
				
					

					if (IsPredef)
					{
						foreach ( XmlNode cnode in node.ChildNodes)
						{
							ProcessNode(cnode);
						}
					}
					
					
					
				}

				if (( IsPredef) && ( CountChilds(node)!=0))
				{
						sValue = Attr2String(node,"after");
						if ( sValue!=null )
						{
							sValue = String.Format("  0\n{0}\n", ( sValue == "" ) ? "ENDSEC" : sValue );
							m_StreamWriter.Write(sValue);
						}	
					
				}
				
			}
			catch (	Exception ex )
			{
				bOk = false;
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","ProcessNode",ex.Message));
			}
		
			return bOk;
		}

		/// <summary>
		/// Set XML node param's value
		/// </summary>
		/// <param name="node">Node name</param>
		/// <param name="ParamName">Param name</param>
		/// <param name="ParamValue"></param>
		/// <returns>true if successful otherwise false</returns>
		public bool SetParam(XmlNode node, string ParamName, string ParamValue)
		{
			XmlNode newNode = node.AppendChild(doc.CreateNode(XmlNodeType.Element,ParamName, ""));
			if ( newNode != null )
				newNode.InnerText = ParamValue;
			else 
				throw new Exception(String.Format("Error setting {0} header parameter", ParamName));
			
			return true;
		}
		
		/// <summary>
		/// Methods produces DXF string for text of the Flowchart object
		/// </summary>
		/// <param name="co">Flowchart object to extract text from</param>
		/// <returns>true if successful otherwise false</returns>
		public bool AddText(object co )
		{

			return AddText(co, null, RectangleF.Empty);
		}

		/// <summary>
		/// Methods produces DXF string for text of the Flowchart object
		/// </summary>
		/// <param name="co">Flowchart object to extract text from</param>
		/// <param name="po">Parent object if any exist</param>
		/// <param name="rect">Object's rectangle if applicable</param>
		/// <returns>true if successful otherwise false</returns>
		public bool AddText(object co, object po, RectangleF rect)
		{

			PointF[] pF = null;
			MindFusion.FlowChartX.Text.Layout tl = null;
			MindFusion.FlowChartX.Text.LayoutOptions lo;
			MindFusion.FlowChartX.Text.DrawTextHint dhint = null;
			MindFusion.FlowChartX.Text.PlainText  text = null;
			System.Drawing.SolidBrush br = null;
			int tlen = 0;
			GraphicsPath gr = null;
			Color crText = Color.Transparent;
			string strText = "";
			long TextID = 0;
			RectangleF boundrect = RectangleF.Empty;
			Font oFont = null;
			StringFormat sformat = null;
			float Rotation = 0;
			bool bOk = false;

			try
			{
				gr = new GraphicsPath(FillMode.Winding);

				if ( co is Box )  // if chart object is Box
				{

					Box oBox = co as Box;
					if ( oBox == null )
						return false;

				
					strText = oBox.PlainText;
					crText = oBox.TextColor;
					TextID = oBox.ZIndex;
					boundrect = oBox.BoundingRect;
					oFont = oBox.Font;
					sformat = oBox.TextFormat;
					Rotation = oBox.RotationAngle;

					if (!m_ExportTextAsMultiline)
					{
						TextBox2Path(oBox,0,ref gr);
						pF = (PointF[])gr.PathPoints.Clone();
					}
				
			
				}
				else  if ( co is Table.Cell ) // if chart object is cell in the table
				{
					Table.Cell oCell = co as Table.Cell;

					if ( oCell == null )
						return false;

					Table oTable = po as Table;

					if ( oTable == null )
						return false;


					if ( rect == RectangleF.Empty )
						return false;

					strText = oCell.PlainText;
					crText = oCell.TextColor;
					TextID = NextID();
					oFont = oTable.Font;
					sformat = oCell.TextFormat;
					Rotation = 0;
					if (!m_ExportTextAsMultiline)
					{
						TextRect2Path(rect,ref gr);
						pF = (PointF[])gr.PathPoints.Clone();
					}

					boundrect = rect;
				}
				else  if ( co is Arrow ) // If chart object is arrow
				{

					Arrow oArrow = co as Arrow;
					if ( oArrow == null )
						return false;

					strText = oArrow.Text;
					crText = oArrow.TextColor;
					TextID = oArrow.ZIndex;
					oFont = oArrow.Font;
					sformat = m_FlowChart.TextFormat;

					rect = getTextRect(System.Drawing.Graphics.FromHwnd(GetActiveWindow()), oArrow.Style , oArrow.TextStyle,
						oArrow.ControlPoints, oArrow.TextColor,oArrow.SegmentCount, oArrow.Text, oArrow.Font,
						RectangleF.Empty, ref Rotation);

					
					
					

					boundrect = rect;
					if (!m_ExportTextAsMultiline)
					{
						rect.Width+=1;
						rect.Height+=1;
						boundrect = rect;
						if (Rotation!=0)
							Rotation = 360 -Rotation;
						TextRect2Path(rect,ref gr);
						pF = (PointF[])gr.PathPoints.Clone();
					}
					

				}
				else  if ( co is Table )  // If chart object is table itself
				{
					Table oTable = co as Table;
					if ( oTable == null )
						return false;


					strText = oTable.CaptionPlainText;
					crText = oTable.CaptionColor;
					TextID = NextID();

					rect = new RectangleF(oTable.BoundingRect.X, oTable.BoundingRect.Y,
										  oTable.BoundingRect.Width, oTable.CaptionHeight);
					boundrect = rect;
					oFont = oTable.Font;
					sformat = oTable.CaptionFormat;
					Rotation = 0;
					if (!m_ExportTextAsMultiline)
					{
						TextRect2Path(rect,ref gr);
						pF = (PointF[])gr.PathPoints.Clone();
					}

				}
				else
					return false;
				
				// Preparing for text processing
				if ( strText == "" )
					return true;

				if ( oFont == null )
					oFont = m_FlowChart.Font;

				// If 'Multiline' text mode enabled calling method directly
				if ( m_ExportTextAsMultiline )
					return AddText(TextID,boundrect, strText, crText, oFont , 
						sformat, true, Rotation, false);

				
				if ( gr!=null )
					gr.Dispose();
			
				// If not 'Multiline' isn't allowed processing text word-by word

				tl = new MindFusion.FlowChartX.Text.Layout();
				br = new System.Drawing.SolidBrush(crText);
				lo = new MindFusion.FlowChartX.Text.LayoutOptions();
	
				dhint = new MindFusion.FlowChartX.Text.DrawTextHint(System.Drawing.Graphics.FromHwnd(GetActiveWindow()),
					oFont, br ,sformat, false , null, boundrect, crText, TextID*100, TextID, Rotation );

				
			
				lo.Alignment = sformat.Alignment;
				lo.LineAlignment = sformat.LineAlignment;
				text = new MindFusion.FlowChartX.Text.PlainText();
				text.Setup(strText, System.Drawing.Graphics.FromHwnd(GetActiveWindow()) , oFont);
				tlen = text.PlainText.Length;
				tl.LayoutInPolygon( text, docToLocal(pF, boundrect),lo);
				tl.Draw(0,0,new MindFusion.FlowChartX.Text.RenderTextCallback(___TextCallback),dhint);
			
			}
			catch (	Exception ex )
			{
				bOk = false;
				m_status = ex.Message;
				Trace.WriteLine(String.Format("{0} error {1}\n","AddText",ex.Message));
			}

			return bOk;

		}

		/// <summary>
		/// Callback routine for word-by-word text processing
		/// </summary>
		/// <param name="text">Text word as 'plain-text'</param>
		/// <param name="dest">Text word's rectangle</param>
		/// <param name="hint">DrawTextHint object</param>
		internal void ___TextCallback(string text, RectangleF dest, DrawTextHint hint)
		{

			if ( hint == null )
				return;

			dest.Offset(hint.rect.Location);
			AddText(hint.TextID++, dest, text, hint.crText, hint.Font, hint.Format, false, hint.RotationAngle, false);

		}

		/// <summary>
		/// Methods directly produces DXF string for text of the Flowchart object
		/// </summary>
		/// <param name="TextID">Id of the text tag</param>
		/// <param name="rect">Text's rectangle</param>
		/// <param name="sText">Text string</param>
		/// <param name="crText">Text color</param>
		/// <param name="fnText">Text font</param>
		/// <param name="sft">Text format</param>
		/// <param name="Multiline">true if text is multi-lined</param>
		/// <param name="RA">Rotation angle of the text</param>
		/// <param name="IsArrowText">true if text is on arrow ( not used )</param>
		/// <returns>true if succesfull otherwise false</returns>
		public bool AddText(long TextID, RectangleF rect, string sText, Color crText, Font fnText, StringFormat sft, bool Multiline, float RA, bool IsArrowText)
		{
			string sResult = "";
		
			if ( sText == "" )
				return false;

			if ( sText == null )
				return false;


			if ( fnText == null )
				fnText = m_FlowChart.Font;

			try
			{
				if (IsArrowText)
					sft.FormatFlags = StringFormatFlags.FitBlackBox;

				if ( Multiline ) // If the is 'Multylined' producing MTEXT DXF output
				{
					sText = sText.Replace("\n"," ");
					sText = sText.Replace("\r"," ");

					/* USE IF YOU WANT TO SEE TEXT AREA AS RED FRAME
					AddRect(rect, TextID, Color.Red, Color.Transparent, Color.Transparent,DashStyle.Solid,
						"", null, null);
						*/
					sResult = String.Format(provider, "0\nMTEXT\n  100\nAcDbEntity\n{0:HAN} 8\n{15}\n  62\n{0:ACI}\n  100\nAcDbMText\n  10\n{1:U}\n  20\n{2:U}\n  40\n{10:U}\n  41\n{3:U}\n  71\n{4:SAL}\n  72\n{5}\n  1\n{6}{7:ATEXT}{8}{9}\n  7\n{14}\n  42\n{11:U}\n  43\n{12:U}\n  50\n{13}\n",
						crText, rect.Left + ((sft.Alignment==StringAlignment.Center) ? rect.Width/2 : 0f) , m_FlowChart.DocExtents.Height - ( rect.Top + rect.Height/2) , rect.Width , sft, "1" , "" ,  fnText , sText, "" , fnText.Size*0.6, rect.Width/2 , rect.Height/2, (-1)*RA , "Standard", TEXT_LAYER);
				}
				else // If the is not 'Multylined' producing TEXT DXF output
					sResult = String.Format(provider, "0\nTEXT\n  100\nAcDbEntity\n{0:HAN} 8\n{15}\n  62\n{0:ACI}\n  100\nAcDbText\n  10\n{1:U}\n  20\n{2:U}\n  40\n{10:U}\n  71\n{4}\n  1\n{6}{7:ATEXT}{8}{9}\n  41\n0.6\n 7\n{14}\n  50\n{13}\n  72\n{5}\n  11\n{1:U}\n  21\n{2:U}\n  100\nAcDbText\n  73\n{18}\n",
						crText, 
						(RA!=0) ? (rect.Left + rect.Width/4) : (rect.Left),
						m_FlowChart.DocExtents.Height - rect.Top, 
						rect.Width, 
						"0", 
						(RA!=0) ? "1" : "0",
						"" ,  
						fnText , 
						sText, 
						"" , fnText.Size, rect.Width, rect.Height, RA , "Standard", TEXT_LAYER, rect.Left + rect.Width,
						m_FlowChart.DocExtents.Height - ( rect.Top + rect.Height),
						(RA!=0) ? "1" : "2");
				
				Trace.WriteLine(String.Format("{0}:{1}", sText, rect.ToString()));
				AddEntity(String.Format("TEXT{0:X5}", TextID),sResult);
			}
			catch ( Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","AddText",ex.Message));
				return false;

			}

			return true;
			
		}
	
		/// <summary>
		/// Check whether color passed is transparent
		/// </summary>
		/// <param name="cr">Color value</param>
		/// <param name="co">Object of the color</param>
		/// <returns>true if color is transparent false otherwise</returns>
		public static bool IsTransparent(Color cr, ChartObject co)
		{
			if (cr.A==0	)
				return true;

			if ( co	is Box )
			{
				if ((co	as Box).Transparent)
					return true;
			}

			return false;

		}

		/// <summary>
		/// Transforms points' coordinates by Rectangle's dimensions
		/// </summary>
		/// <param name="points">Point array</param>
		/// <param name="rect">Rectangle for transformation</param>
		/// <returns>Modified point array</returns>
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
		/// Approximate two color of gradient to get corresponded ACI
		/// </summary>
		/// <param name="cr1">Color 1 of gradient</param>
		/// <param name="cr2">Color 2 of gradient</param>
		/// <returns></returns>
		public static Color ApproxColors ( Color cr1, Color cr2)
		{
			return Color.FromArgb( (int) Math.Round((cr1.R + cr2.R)/2f), 
				(int) Math.Round((cr1.G + cr2.G)/2f),
				(int) Math.Round((cr1.B + cr2.B)/2f));
		}

		/// <summary>
		/// Determine color of object. If color couldn't be detected uses crDefault instead of it
		/// </summary>
		/// <param name="obj">Flowchart object</param>
		/// <param name="crDefault">Default color</param>
		/// <returns>Color of the object</returns>
		public static Color ExtractColor ( object obj, Color crDefault)
		{
			Color crRet = crDefault;
			try
			{
				if ( obj is MindFusion.FlowChartX.Pen )
				{

					crRet = (obj as MindFusion.FlowChartX.Pen).Color;
				}
				else if ( obj is MindFusion.FlowChartX.Brush )
				{
					if ( obj is MindFusion.FlowChartX.LinearGradientBrush )
						crRet = ApproxColors((obj as  MindFusion.FlowChartX.LinearGradientBrush).LinearColors[0],
							(obj as  MindFusion.FlowChartX.LinearGradientBrush).LinearColors[1]);
										
					else if ( obj is MindFusion.FlowChartX.HatchBrush)
						crRet = ( obj as MindFusion.FlowChartX.HatchBrush).ForegroundColor;
					else if ( obj is MindFusion.FlowChartX.TextureBrush)
						crRet = crDefault;
					else if ( obj is MindFusion.FlowChartX.SolidBrush)
						crRet = ( obj as MindFusion.FlowChartX.SolidBrush).Color;
					else
						throw new Exception("Bad brush object passed");
				

				}
				else
					throw new Exception("Bad color object passed");
			}
			catch ( Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","ExtractColor",ex.Message));

			}
			
			return crRet;
		}

		/// <summary>
		/// Gets rectangle of the arrow text
		/// </summary>
		/// <param name="g">GraphicsPath</param>
		/// <param name="style">Style of the arrow</param>
		/// <param name="textStyle">Style of the arrow text</param>
		/// <param name="points">Arrow's point collection</param>
		/// <param name="textColor">Text color</param>
		/// <param name="segmentCount">Arrow segments count</param>
		/// <param name="text">Arrow's text itself</param>
		/// <param name="textFont">Arrow's text font</param>
		/// <param name="rect">Bounding rect</param>
		/// <param name="a">Text's rotation angle to be returned</param>
		/// <returns>Arrow text's rectangle</returns>
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
							MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
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
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
					else
					{
						float r1 = 0, a1 = 0;
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							points[pt - 1], points[pt], ref a, ref r);
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
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
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
					else
					{
						PointF pt1 = ArrowUtils.GetBezierPt(points, pt, 0.95f);
						PointF pt2 = ArrowUtils.GetBezierPt(points, pt + 1, 0.05f);
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
				}

				a = 180 - a;
				if (a > 90 && a < 270)
					a -= 180;
					
			
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
				StringFormat cf = m_FlowChart.TextFormat;
				SizeF size = g.MeasureString(text, textFont);
				size.Width += ArrowUtils.getMillimeter(m_FlowChart.MeasureUnit);
				size.Height += ArrowUtils.getMillimeter(m_FlowChart.MeasureUnit);

				
			
			}

			brText.Dispose();

			RectangleF rc = new RectangleF(x - textSize.Width/2 ,y - textSize.Height/2,textSize.Width, textSize.Height);
			return rc;

		}
		
		/// <summary>
		/// Gets GraphicsPath of the Box's TextArea
		/// </summary>
		/// <param name="oBox">Flowchart box</param>
		/// <param name="Offset">Box offset if necessary</param>
		/// <param name="gr">Graphics path to be assigned</param>
		/// <returns>true if successfull otherwise false</returns>
		public bool TextBox2Path( Box oBox, float Offset, ref GraphicsPath gr )
		{
			string result = null;
			ElementTemplate[] et= null;

			if ( oBox.Shape != null )
			{
				if ( oBox.Shape.TextArea!=null)
					et = oBox.Shape.TextArea;
				else
					et = oBox.Shape.Outline;
			}

			return Shape2Str(oBox.BoundingRect, oBox.Style, et ,
				oBox.Pen.Color, ( - oBox.RotationAngle + oBox.ShapeOrientation) , oBox.Pen.DashStyle, oBox.Pen.Width,
				Offset, false, ref gr, ref result);
		}
		

		/// <summary>
		/// Gets GraphicsPath from Rectangle
		/// </summary>
		/// <param name="rect">Rectangle</param>
		/// <param name="gr">Graphics path to be assigned</param>
		/// <returns>true if successfull otherwise false</returns>
		public bool TextRect2Path( RectangleF rect , ref GraphicsPath gr )
		{
			string result = null;
			return Shape2Str(rect ,BoxStyle.Rectangle, null ,
				Color.Transparent, 0 , DashStyle.Solid, 1,
				0, false, ref gr, ref result);
		}

		/// <summary>
		/// Produces DXF output for Flowchart Box
		/// </summary>
		/// <param name="oBox">Flowchart box</param>
		/// <param name="Offset">Offset if necessary</param>
		/// <param name="gr">GraphicsPath to be assigned</param>
		/// <param name="result">DXF output string</param>
		/// <returns>true if successfull otherwise false</returns>
		public bool Box2Str( Box oBox, float Offset, ref GraphicsPath gr, ref string result )
		{
			return Shape2Str(oBox.BoundingRect, oBox.Style, ( oBox.Shape == null ) ? null : oBox.Shape.Outline,
				oBox.Pen.Color,  ( - oBox.RotationAngle + oBox.ShapeOrientation) , oBox.Pen.DashStyle, oBox.Pen.Width,
				Offset, true, ref gr, ref result);
		}

		/// <summary>
		/// Produces DXF output for Flowchart shape
		/// </summary>
		/// <param name="rect">Bounding Rectangle</param>
		/// <param name="ShapeStyle">Flowchart shape style</param>
		/// <param name="st">ElementTemplate reference if shape is complex null otherwise</param>
		/// <param name="crLine">Line color</param>
		/// <param name="RA">Rotation angle</param>
		/// <param name="dash">DashStyle</param>
		/// <param name="LineWidth">Line width ( not used)</param>
		/// <param name="Offset">Offset if it's necessary</param>
		/// <param name="WCS2UCS">if true conversion for world-coordinate to user-coordinate is required</param>
		/// <param name="gr">GraphicsPath to be assigned</param>
		/// <param name="result">>DXF output string</param>
		/// <returns>true if successfull otherwise false</returns>
		public bool Shape2Str( RectangleF rect, BoxStyle ShapeStyle, ElementTemplate[] st, 
			Color crLine, float RA,  DashStyle dash, Single LineWidth, 
			float Offset, bool WCS2UCS, ref GraphicsPath gr, ref string result )
		{
			
			float X	= 0, Y = 0,	X1 = 0,	Y1 = 0,
				X2 = 0,	Y2 = 0,	X3 = 0,	Y3 = 0;
			bool DisableStringOutput = false;
			PointF[] pts = null;
			bool bOk = false;
			string PathPart = "";
			GraphicsPath gr_temp = new GraphicsPath(FillMode.Winding);

			try
			{
		
				if ( gr == null )
					throw new Exception("Empty Graphics Path reference passed");
						

				DisableStringOutput = ( result == null );
			
				// Detecting box's style
				switch (ShapeStyle)
				{
		
					case BoxStyle.Rectangle:
						gr.AddRectangle(rect);
						break;
					case BoxStyle.Ellipse:
						gr.AddEllipse(rect.X,rect.Y,rect.Width,	rect.Height);
						break;
					case BoxStyle.RoundedRectangle:
						gr = Utilities.getRoundRect(rect.X,	rect.Y,	rect.Width,	rect.Height,10);
						break;
					case BoxStyle.Delay:
						gr.AddRectangle(rect);
						break;
					case BoxStyle.Rhombus:
		
						pts = new PointF[4] {new PointF((rect.Left	+ rect.Right) /	2, rect.Top - Offset),
												new PointF(Math.Max(rect.Right, rect.Left) + Offset, (rect.Top	+ rect.Bottom) / 2),
												new PointF((rect.Left	+ rect.Right) /	2,	Math.Max(rect.Bottom, rect.Top) + Offset),
												new PointF(rect.Left - Offset,(rect.Top	+ rect.Bottom) / 2)};
		
									
						gr.AddPolygon(pts);
									
					
						break;
					case BoxStyle.Shape:  // if shape is complex then processing all its elements
						

						if (st  ==	null)
							throw new Exception("Empty shape reference in the complex shape");

						

						foreach	(ElementTemplate et	in st )
						{
							switch ( et.getClassId())
							{
								case 28: // If shape element is arc
						
									ArcTemplate	at =  et as	ArcTemplate;
									double rx = 0, ry = 0 , cx = 0, cy = 0;
					
									rx = rect.Width * (at.Bounds.Width/200);
									ry = rect.Height * (at.Bounds.Height/200);

									cx = rect.X + rect.Width*(at.Bounds.X/100) + rx;
									cy = rect.Y + rect.Height*(at.Bounds.Y/100) + ry;

									gr.AddArc((float) (cx - rx), (float) ( cy - ry) , (float) rx*2, (float) ry*2, at.StartAngle ,at.SweepAngle);

									float StartAngle =  180 + at.StartAngle + (360 - at.SweepAngle) - RA, EndAngle =  StartAngle + at.SweepAngle;

									if ( rx != ry )
									{
										

										float iAngleC = 0;
										double majorx = 0, majory = 0;
									
										
										if (at.StartAngle<0)
											iAngleC = 360 - Math.Abs(at.StartAngle);
										else
											iAngleC = at.StartAngle;


										majorx = (rx < ry) ? 0 : rx;
										majory = (rx < ry) ? ry : 0; 
										
									
										if (( iAngleC >=0 ) && (iAngleC<90))
										{
										
											majorx*=-1;
											majory*=-1;
										}
										else if (( iAngleC >=90 ) && (iAngleC<180))
										{
										
								
										}
										else if (( iAngleC >=180 ) && (iAngleC<270))
										{
								
										
											majorx*=-1;
											majory*=-1;
											
										}
										else if (( iAngleC >=270 ) && (iAngleC<=360))
										{
								
										}
										
									

										
										StartAngle =  at.StartAngle + RA;
										EndAngle =  StartAngle + at.SweepAngle;
										
										if ((at.SweepAngle>=270) && (at.SweepAngle<360))
										{
											if ( ry > rx )
										{
												StartAngle = at.StartAngle + RA + 90 + ( 360 - at.SweepAngle);
												EndAngle = at.StartAngle + RA + 90;
											}
											
										} 
										else if (at.SweepAngle>=180)
										{
											if (ry>rx)
											{
												StartAngle-=90;
												EndAngle-=90;
											}
										
											
										}
										
										
	
										
										
										PathPart = String.Format(provider, "  0\nELLIPSE\n  100\nAcDbEntity\n{0:HAN}  8\n{5}\n  62\n{6:ACI}\n 100\nAcDbEllipse\n  10\n{0:U}\n  20\n{1:U}\n  11\n{7:U}\n  21\n{8:U}\n  40\n{9:U}\n  41\n{3:U}\n  42\n{4:U}\n",
											cx, m_FlowChart.DocExtents.Height - cy,0, (StartAngle*Math.PI)/180 , (EndAngle*Math.PI)/180, SHAPE_LAYER, crLine, majorx , majory , (rx < ry) ? rx/ry : ry/rx);
									}
									else
									{
										
										PathPart = String.Format(provider, "  0\nARC\n  100\nAcDbEntity\n{0:HAN}  8\n{5}\n  62\n{6:ACI}\n 100\nAcDbCircle\n  10\n{0:U}\n  20\n{1:U}\n  40\n{2:U}\n  100\nAcDbArc\n  50\n{3}\n  51\n{4}\n",
											cx, m_FlowChart.DocExtents.Height - cy,rx, StartAngle , EndAngle, SHAPE_LAYER, crLine);
									}

									break;
								case 29: // If shape element is bezier curve

									BezierTemplate	bt =  et as	BezierTemplate;

									X =	rect.X	+ rect.Width * (bt.Coordinates[0]/100);
									Y =	rect.Y	+  rect.Height*	(bt.Coordinates[1]/100);

									X1 = rect.X	 + rect.Width *	(bt.Coordinates[2]/100);
									Y1 = rect.Y	 +	rect.Height* (bt.Coordinates[3]/100);

									X2 = rect.X	 + rect.Width *	(bt.Coordinates[4]/100);
									Y2 = rect.Y	 + rect.Height*	(bt.Coordinates[5]/100);

									X3 = rect.X	 + rect.Width *	(bt.Coordinates[6]/100);
									Y3 = rect.Y	 + rect.Height*	(bt.Coordinates[7]/100);

									
									gr_temp.Reset();
									gr_temp.AddBezier(X,Y,X1,Y1,X2,Y2,X3,Y3);
									gr.AddBezier(X,Y,X1,Y1,X2,Y2,X3,Y3);
									
									// Applying rotation if it's necessary
									pts = RotatePoints(RA , new PointF(rect.X + rect.Width/2,rect.Y + rect.Height/2), gr_temp);

									gr_temp.Flatten();
									PointF[] pts2 = gr_temp.PathData.Points.Clone() as PointF[];
									
									PathPart = String.Format(provider, "0\nPOLYLINE\n{0:HAN}   100\nAcDbEntity\n8\n{0}\n  62\n{1:ACI}\n  100\nAcDb2dPolyline\n  66\n1\n  70\n4\n  75\n6\n{2}{3}0\nSEQEND\n  100\nAcDbEntity\n{0:HAN}", 
										SHAPE_LAYER,
										crLine, 
										Pt2String(pts,crLine,SHAPE_LAYER,DxLineType.ltVertex, dash, 16),
										Pt2String(pts2,crLine,SHAPE_LAYER,DxLineType.ltVertex, dash, 8));

									break;
								case 30:  // If shape element is line


									LineTemplate lt	= et as	LineTemplate;
					
									X1 = rect.X	+ rect.Width * (lt.Coordinates[0]/100);
									Y1 = rect.Y	+ rect.Height* (lt.Coordinates[1]/100);

									X2 = rect.X	+ rect.Width * (lt.Coordinates[2]/100);
									Y2 = rect.Y	+ rect.Height* (lt.Coordinates[3]/100);
									
									gr_temp.Reset();
									gr_temp.AddLine(X1,Y1,X2,Y2);
									gr.AddLine(X1,Y1,X2,Y2);
									
									// Applying rotation if it's necessary
									pts = RotatePoints(RA , new PointF(rect.X + rect.Width/2,rect.Y + rect.Height/2), gr_temp);
									PathPart = Pt2String(pts, crLine,SHAPE_LAYER,DxLineType.ltSingle,DashStyle.Solid, 1);
									break;
							}

							result+=PathPart;
						}
						break;
					default:
						gr.AddRectangle(rect);
						break;

				}


			
				// Converting shapes coordinates from UCS to WSC
				TranslateCoords(RA , new PointF(rect.X + rect.Width/2,rect.Y + rect.Height/2), WCS2UCS, ref gr);
				
				gr.Flatten();

				// If string output is required producing DXF string
				if (!DisableStringOutput)
				{
					if ( result=="" )
					{
						pts = gr.PathPoints.Clone() as PointF[];
						result = Pt2String(pts, crLine, SHAPE_LAYER, DxLineType.ltClosedSingle, dash, LineWidth);
					}
					
				}
				
				bOk = true;
			}
			catch (	Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","Box2Str",ex.Message));
				bOk = false;
			}

			return bOk;
		}

		/// <summary>
		/// Translates coordinates of the Graphics path applying rotation if it's necessary
		/// </summary>
		/// <param name="Rotation">Rotation angle</param>
		/// <param name="RotationPoint">Point of the rotation ( used if RA is not 0)</param>
		/// <param name="WCS2UCS">if true then translate from UCS to WCS </param>
		/// <param name="gr">GraphicsPath to be processed</param>
		/// <returns>true if succesfull otherwise false</returns>
		public bool TranslateCoords(float Rotation, PointF RotationPoint, bool WCS2UCS, ref GraphicsPath gr)
		{
			bool bOk = false;
			try
			{
				Matrix mtxTrans = new Matrix();

				if ( WCS2UCS )
				{
					mtxTrans.Translate(0, m_FlowChart.DocExtents.Height);
					mtxTrans.Scale(1,-1);
				}
				
								
				if ( Rotation!= 0 )
				{					
					mtxTrans.RotateAt((-1)*Rotation, RotationPoint );
				}

				gr.Transform(mtxTrans);
				//gr.Flatten();
				mtxTrans.Dispose();
				bOk = true;
			
			}
			catch (	Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","TranslateCoords",ex.Message));
				bOk = false;
			}

			return bOk;
		}

		/// <summary>
		/// Rotates GraphicsPath using specified rotation angle and rotation point
		/// </summary>
		/// <param name="Rotation">Rotation angle</param>
		/// <param name="RotationPoint">Point of the rotation</param>
		/// <param name="gr">GraphicsPath to be processed</param>
		/// <returns>Array of the rotated points</returns>
		public PointF[] RotatePoints(float Rotation, PointF RotationPoint, GraphicsPath gr )
		{

			PointF[] pts = null;

			try
			{

				TranslateCoords(Rotation, RotationPoint, true, ref gr);
				pts = new PointF[gr.PointCount];

				for ( int i=0; i<gr.PointCount;i++ )
				{
					pts[i] = gr.PathPoints[i];
				}
			
			
			}
			catch (	Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","RotatePoints",ex.Message));
				pts = null;
			}

			return pts;
		}

	
		/// <summary>
		/// Adds embedded image to DXF output
		/// </summary>
		/// <param name="rect1">Image bounding rect</param>
		/// <param name="oImage">Image itself</param>
		/// <param name="ImageID">Uniques ID of the image</param>
		/// <returns>true if succesfull otherwsie false</returns>
		public bool AddImage(RectangleF rect1, Image oImage, long ImageID)
		{
			return AddImage( rect1,  oImage, ImageID, ImageAlign.Center, Color.Empty );
		}

		/// <summary>
		/// Adds embedded image to DXF output
		/// </summary>
		/// <param name="rect1">Image bounding rect</param>
		/// <param name="oImage">Image itself</param>
		/// <param name="ImageID">Uniques ID of the image</param>
		/// <param name="ia">Image alignment</param>
		/// <param name="Color2Repl">Color that to be replace transparent one in the image</param>
		/// <returns>true if succesfull otherwsie false</returns>
		public bool AddImage(RectangleF rect1, Image oImage, long ImageID, ImageAlign ia, Color Color2Repl)
		{
			string FileName = "", sText = "";
			try
			{

				if (!m_ExportExternalImages)
					return true;

				float fW = 0, fH = 0, fDX = 0, fDY = 0;
				RectangleF rect = RectangleF.Empty;

				// Validating input parameters
				if ( oImage == null )
					return true;


				if ( FileName == null )
					return true;


				// Correcting bounding rectangle according to image alignment
				rect = new RectangleF(rect1.X, rect1.Y,rect1.Width, rect1.Height);
				if ( GetImageDim(oImage, rect, ia , ref fW, ref fH , ref fDX, ref fDY))
				{
					rect.Width = fW;
					rect.Height = fH;
					rect.X = fDX;
					rect.Y = fDY;

				}
				
				// Replicating color if it's necessary
				if ( Color2Repl != Color.Empty )
				{
					Color pixColor;
					Bitmap bmp = oImage as Bitmap;
					for (int Xcount = 0; Xcount < oImage.Width; Xcount++)
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

				// Preparing & inserting IMAGEDEF
				FileName = String.Format("{2}\\{0}_{1}.jpg", FileNamePattern, ImageID, BasedPath);
				oImage.Save(FileName, System.Drawing.Imaging.ImageFormat.Jpeg);

				sText = String.Format(provider , "  0\nIMAGEDEF\n  5\n{0:ID}\n  100\nAcDbRasterImageDef\n  90\n0\n  1\n{1}\n   10\n{2:U}\n  20\n{3:U}\n  11\n0.01\n  21\n0.01\n  280\n1\n  281\n0\n",
					ImageID, 
					FileName,
					rect.Width,
					rect.Height);

				// Preparing & inserting IMAGE
                if (AddObject(String.Format("OBJ{0:X3}", ImageID), sText))
				{

					sText = String.Format(provider, "  0\nIMAGE\n100\nAcDbEntity\n{0:HAN}  8\n{3}\n100\nAcDbRasterImage\n  10\n{0:U}\n  20\n{1:U}\n  11\n1\n  21\n0\n  31\n0\n  12\n0\n  22\n1\n  32\n0\n  13\n{4:U}\n  23\n{5:U}\n  340\n{2:ID}\n  70\n9\n",
						rect.X,
						m_FlowChart.DocExtents.Height - rect.Bottom,
						ImageID,
						IMAGE_LAYER,
						rect.Width,
						rect.Height);

					AddEntity(String.Format("IMG{0:X3}", ImageID), sText);
				}


			}
			catch ( Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","AddImage",ex.Message));
				return false;

			}

			return true;
		}

		/// <summary>
		/// Intended for hashing some XML nodes
		/// </summary>
		/// <param name="NodeName">Node name</param>
		/// <param name="nodeOut">Variable for saving node</param>
		/// <returns>true if succesfull otherwise false</returns>
		public bool HashNode(string NodeName, ref XmlNode nodeOut)
		{
			
			nodeOut = nodeRoot.SelectSingleNode(NodeName);

			if ( nodeOut == null )
				throw new Exception(String.Format("Can't hash node: {0}", NodeName));

			return true;

		}
		
		/// <summary>
		/// Get next unique ID
		/// </summary>
		/// <returns>Value of the unique ID</returns>
		public long NextID()
		{
			return  provider.NextID();
		}

		/// <summary>
		/// Get DXF string output for arrow's head
		/// </summary>
		/// <param name="ahs">Arrow head type</param>
		/// <param name="crLine">Arrow head line color</param>
		/// <param name="dst">Arrow line dash style</param>
		/// <param name="crFill">Arrow head fill color</param>
		/// <param name="sFill">DXF output for arrow head's fill</param>
		/// <returns>DXF string for arrow head frame</returns>
		public string ArrowHead2Str (ArrowHeadShape ahs, Color crLine, DashStyle dst, Color crFill, ref string sFill)
		{
			string sResult = "";
			int c = 0;
			float xoff = 0, yoff =0;
			PointF[] shape = null;

			try
			{

				sFill = "";
				for (int i = 0; i < ahs.Elements.GetLength(0); ++i)
				{
					EArrowHeadElement el = (EArrowHeadElement)ahs.Elements[i];
					switch (el)
					{
						case EArrowHeadElement.heLine:
						{
							float x1 = ahs.Coordinates[c++] + xoff;
							float y1 = m_FlowChart.DocExtents.Height -  ahs.Coordinates[c++] + yoff;
							float x2 = ahs.Coordinates[c++] + xoff;
							float y2 = m_FlowChart.DocExtents.Height -  ahs.Coordinates[c++] + yoff;
							shape = new PointF[2] {new PointF(x1,y1), new PointF(x2,y2)};

							sResult+=Pt2String(shape, crLine, ARROW_LAYER, DxLineType.ltSingle, dst, 1);
							
						
						}
							break;
						case EArrowHeadElement.heShape:
						{
							int pts = ahs.Elements[++i];
							shape = new PointF[pts];	
						
							for (int j = 0; j < pts; ++j)
							{
								shape[j].X = ahs.Coordinates[c++] + xoff;
								shape[j].Y = m_FlowChart.DocExtents.Height - ahs.Coordinates[c++] + yoff;
							}
							
							
							
							sResult+=Pt2String(shape, crLine, ARROW_LAYER, DxLineType.ltClosedSingle, dst, 1);
							if (!IsTransparent(crFill, null))
							{
								shape = MakePtsUnique(shape);
								sFill+= String.Format(provider, "0\nHATCH\n  100\nAcDbEntity\n{0:HAN}  8\n{0}\n  62\n{10:ACI}\n  100\nAcDbHatch\n  10\n0\n  20\n0\n  30\n0\n    210\n0\n  220\n0\n  230\n1\n  2\n{1}\n  70\n{2}\n  71\n{3}\n  91\n{4}\n  92\n{5}\n  72\n{6}\n  73\n{7}\n  93\n{8}\n{9}  97\n0\n  75\n0\n  76\n1\n  47\n1\n 98\n0\n",
									HATCH_LAYER,"SOLID", "1", "1", "1","7", "0", "1", shape.Length , Pt2String(shape,crFill, HATCH_LAYER, DxLineType.ltHatch, DashStyle.Solid, 1), crFill);
							}
							

						
						}
							break;
						case EArrowHeadElement.heCircle:
						{
							float cx = ahs.Coordinates[c++] + xoff;
							float cy = ahs.Coordinates[c++] + yoff;
							float r = ahs.Coordinates[c++];

							sResult+= String.Format(provider, "  0\nARC\n  100\nAcDbEntity\n{0:HAN}  8\n{5}\n  62\n{6:ACI}\n 100\nAcDbCircle\n  10\n{0:U}\n  20\n{1:U}\n  40\n{2:U}\n  100\nAcDbArc\n  50\n{3}\n  51\n{4}\n",
										cx, m_FlowChart.DocExtents.Height - cy, r, 0 , 360, ARROW_LAYER, crLine);
						
							if (!IsTransparent(crFill, null))
								sFill+= String.Format(provider, "0\nHATCH\n  100\nAcDbEntity\n{0:HAN}  8\n{0}\n  62\n{7:ACI}\n  100\nAcDbHatch\n  10\n0\n  20\n0\n  30\n0\n  210\n0\n  220\n0\n  230\n1\n  2\n{1}\n  70\n{2}\n  71\n{3}\n  91\n1\n  92\n1\n  93\n1\n  72\n{6}\n  10\n{8}\n  20\n{9}\n  40\n{10}\n  50\n{11}\n  51\n{12}\n   73\n0\n  97\n0\n    75\n0\n  76\n1\n  98\n0\n",
									HATCH_LAYER,"SOLID", "1", "1", "1","5", "2",  crFill,cx, m_FlowChart.DocExtents.Height - cy, r, 0, 360);

							
						}			
							break;

					}	
				}

			
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","ArrowHead2Str",ex.Message));
				sResult = null;
			}
			
			return sResult;

		}

		/// <summary>
		/// Produces DXF output for Rectangle with its fill
		/// </summary>
		/// <param name="rect">Rectangle to be processed</param>
		/// <param name="RectID">Unique id of the rectangle</param>
		/// <param name="crLine">Line color</param>
		/// <param name="crFill">Fill color</param>
		/// <param name="crText">RESERVED</param>
		/// <param name="dst">Line style</param>
		/// <param name="str_text">RESERVED</param>
		/// <param name="fnText">RESERVED</param>
		/// <param name="sft">RESERVED</param>
		/// <returns>Produced DXf string</returns>
		protected bool AddRect(RectangleF rect, long RectID, Color crLine, Color crFill, Color crText, DashStyle dst, string str_text , Font fnText, StringFormat sft )
		{
			bool bOk = false;
			string sResult = "", sFill = "";
			GraphicsPath gr = null;
			PointF[] pts = null;
			
			try
			{
				gr = new GraphicsPath(FillMode.Winding);
				sResult = "";

		
				if (Shape2Str(rect,BoxStyle.Rectangle, null, crLine, 0, dst,
					1, 0, true, ref gr ,  ref sResult))
				{

					

					if (!IsTransparent(crFill, null))
					{
						pts = MakePtsUnique(gr.PathPoints as PointF[]);
						sFill = String.Format(provider, "0\nHATCH\n  100\nAcDbEntity\n{0:HAN}  8\n{0}\n  62\n{10:ACI}\n  100\nAcDbHatch\n  10\n0\n  20\n0\n  30\n0\n    210\n0\n  220\n0\n  230\n1\n  2\n{1}\n  70\n{2}\n  71\n{3}\n  91\n{4}\n  92\n{5}\n  72\n{6}\n  73\n{7}\n  93\n{8}\n{9}  97\n0\n  75\n0\n  76\n1\n  47\n1\n 98\n0\n",
							HATCH_LAYER,"SOLID", "1", "1", "1","7", "0", "1", pts.Length, Pt2String(pts,crFill, HATCH_LAYER, DxLineType.ltHatch, DashStyle.Solid, 1), crFill);
							
						
						AddEntity(String.Format("FILL{0:X3}",RectID),sFill);
					
					}

					if (!IsTransparent(crLine, null))
						AddEntity(String.Format("TABLE{0:X3}", RectID),sResult);
				
				
					
				
					
				}

				
				bOk = true;
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","AddRect",ex.Message));
				bOk = false;
				
			}

			if ( gr!=null )
				gr.Dispose();

			return bOk;
		}

		/// <summary>
		/// Returns true if the line is closed ( start = end points are equal )
		/// </summary>
		/// <param name="pts">Line points</param>
		/// <returns>Returns true if the line is closed ( start = end points are equal )</returns>
		public static bool IsLineClosed(PointF[] pts)
		{
			return ((pts[0].X == pts[pts.Length - 1].X) && (pts[0].Y == pts[pts.Length - 1].Y));
		}

		public static PointF[] MakePtsUnique( PointF[] iPoints)
		{
			PointF [] result = null;
			PointF last_pt = PointF.Empty;
			ArrayList ar = null;
			bool MoveElement = false;

			ar  = new ArrayList();
			foreach ( PointF pt in iPoints)
			{
				MoveElement = true;
			
				if ( pt == last_pt )
					MoveElement = false;

				if ( MoveElement )
					ar.Add(pt);

				last_pt = pt;

			}

			if ( (PointF) ar[0] == (PointF) ar[ar.Count-1] )
				ar.RemoveAt(ar.Count-1);

			result = (PointF[]) (ar.Clone() as ArrayList).ToArray(typeof(PointF));
			
			return result;	
						
		}

		/// <summary>
		/// Returns new dimensions of the image's bounding rect according to its parameters
		/// </summary>
		/// <param name="pict">Image reference</param>
		/// <param name="rect">Image bounding rect</param>
		/// <param name="ppos">Image alignment</param>
		/// <param name="picw">New image's width value</param>
		/// <param name="pich">New image's height value</param>
		/// <param name="xoff">Image's X offset</param>
		/// <param name="yoff">Image's Y offset</param>
		/// <returns>true if successfull otherwise false</returns>
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


		/// <summary>
		/// Inherited from Flowchart source
		/// </summary>
		/// <param name="g"></param>
		public void setTransforms(Graphics g)
		{
			g.ResetTransform();
			g.TranslateTransform(-m_FlowChart.ScrollX,-m_FlowChart.ScrollY);
			g.PageUnit = m_FlowChart.MeasureUnit;
			g.PageScale = m_FlowChart.ZoomFactor / 100F;
		}

		/// <summary>
		/// Inherited from Flowchart source
		/// </summary>
		/// <param name="g"></param>
		/// <param name="r"></param>
		/// <returns></returns>
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

		// Member variables definition
		protected string m_status = "";
		protected string m_filePath	= "";
		protected bool m_valid = false;
		protected StreamWriter m_StreamWriter =	null;
		UnitFormatter provider = null;
		protected XmlDocument doc = null;
		protected XmlNode nodeRoot = null;
		protected XmlNode nodeHeader = null;
		protected XmlNode nodeTables = null;
		protected XmlNode nodeLtypes = null;
		protected XmlNode nodeStyle = null;
		protected XmlNode nodeEntities = null;
		protected XmlNode nodeBlocks = null;
		protected XmlNode nodeLayers = null;
		protected XmlNode nodeObjects = null;
		protected FlowChart m_FlowChart = null;
		protected XmlNode node = null;
		protected float m_UnitScale = 1f;//1f/25.2f;
		protected float m_OffY = 0;
		protected float m_OffX = 0;
		protected GraphicsUnit m_DxfUnit = GraphicsUnit.Millimeter;
		
		protected static string IMAGE_LAYER = "05-IMAGES";
		protected static string TEXT_LAYER	= "04-TEXT";
		protected static string SHAPE_LAYER = "03-SHAPES";
		protected static string ARROW_LAYER = "02-ARROWS";
		protected static string HATCH_LAYER = "01-HATCHES";
		protected static string BACKGROUND_LAYER = "00-BACKGROUNDS";
		protected bool m_ExportTextAsMultiline = false;
		protected bool m_ExportExternalImages = true;
		protected string FileNamePattern = "";
		protected string BasedPath = "";
	
		// static array used for translation RGB color to ACI
		static Color[] ACIs = {Color.FromArgb(255,255,255),
								  Color.FromArgb(255,  0,  0), Color.FromArgb(255,255,  0), Color.FromArgb(  0,255,  0), Color.FromArgb(  0,255,255), Color.FromArgb(  0,  0,255),	// 5
								  Color.FromArgb(255,  0,255), Color.FromArgb(255,255,255), Color.FromArgb(128,128,128), Color.FromArgb(192,192,192), Color.FromArgb(255,  0,  0),	// 10
								  Color.FromArgb(255,127,127), Color.FromArgb(204,  0,  0), Color.FromArgb(204,102,102), Color.FromArgb(153,  0,  0), Color.FromArgb(153, 76, 76),	// 15
								  Color.FromArgb(127,  0,  0), Color.FromArgb(127, 63, 63), Color.FromArgb( 76,  0,  0), Color.FromArgb( 76, 38, 38), Color.FromArgb(255, 63,  0),	// 20
								  Color.FromArgb(255,159,127), Color.FromArgb(204, 51,  0), Color.FromArgb(204,127,102), Color.FromArgb(153, 38,  0), Color.FromArgb(153, 95, 76),	// 25
								  Color.FromArgb(127, 31,  0), Color.FromArgb(127, 79, 63), Color.FromArgb( 76, 19,  0), Color.FromArgb( 76, 47, 38), Color.FromArgb(255,127,  0),	// 30
								  Color.FromArgb(255,191,127), Color.FromArgb(204,102,  0), Color.FromArgb(204,153,102), Color.FromArgb(153, 76,  0), Color.FromArgb(153,114, 76),	// 35
								  Color.FromArgb(127, 63,  0), Color.FromArgb(127, 95, 63), Color.FromArgb( 76, 38,  0), Color.FromArgb( 76, 57, 38), Color.FromArgb(255,191,  0),	// 40
								  Color.FromArgb(255,223,127), Color.FromArgb(204,153,  0), Color.FromArgb(204,178,102), Color.FromArgb(153,114,  0), Color.FromArgb(153,133, 76),	// 45
								  Color.FromArgb(127, 95,  0), Color.FromArgb(127,111, 63), Color.FromArgb( 76, 57,  0), Color.FromArgb( 76, 66, 38), Color.FromArgb(255,255,  0),	// 50
								  Color.FromArgb(255,255,127), Color.FromArgb(204,204,  0), Color.FromArgb(204,204,102), Color.FromArgb(153,153,  0), Color.FromArgb(153,153, 76),	// 55
								  Color.FromArgb(127,127,  0), Color.FromArgb(127,127, 63), Color.FromArgb( 76, 76,  0), Color.FromArgb( 76, 76, 38), Color.FromArgb(191,255,  0),	// 60
								  Color.FromArgb(223,255,127), Color.FromArgb(153,204,  0), Color.FromArgb(178,204,102), Color.FromArgb(114,153,  0), Color.FromArgb(133,153, 76),	// 65
								  Color.FromArgb( 95,127,  0), Color.FromArgb(111,127, 63), Color.FromArgb( 57, 76,  0), Color.FromArgb( 66, 76, 38), Color.FromArgb(127,255,  0),	// 70
								  Color.FromArgb(191,255,127), Color.FromArgb(102,204,  0), Color.FromArgb(153,204,102), Color.FromArgb( 76,153,  0), Color.FromArgb(114,153, 76),	// 75
								  Color.FromArgb( 63,127,  0), Color.FromArgb( 95,127, 63), Color.FromArgb( 38, 76,  0), Color.FromArgb( 57, 76, 38), Color.FromArgb( 63,255,  0),	// 80
								  Color.FromArgb(159,255,127), Color.FromArgb( 51,204,  0), Color.FromArgb(127,204,102), Color.FromArgb( 38,153,  0), Color.FromArgb( 95,153, 76),	// 85
								  Color.FromArgb( 31,127,  0), Color.FromArgb( 79,127, 63), Color.FromArgb( 19, 76,  0), Color.FromArgb( 47, 76, 38), Color.FromArgb(  0,255,  0),	// 90
								  Color.FromArgb(127,255,127), Color.FromArgb(  0,204,  0), Color.FromArgb(102,204,102), Color.FromArgb(  0,153,  0), Color.FromArgb( 76,153, 76),	// 95
								  Color.FromArgb(  0,127,  0), Color.FromArgb( 63,127, 63), Color.FromArgb(  0, 76,  0), Color.FromArgb( 38, 76, 38), Color.FromArgb(  0,255, 63),	// 100
								  Color.FromArgb(127,255,129), Color.FromArgb(  0,204, 51), Color.FromArgb(102,204,127), Color.FromArgb(  0,153, 38), Color.FromArgb( 76,153, 95),	// 105
								  Color.FromArgb(  0,127, 31), Color.FromArgb( 63,127, 79), Color.FromArgb(  0, 76, 19), Color.FromArgb( 38, 76, 47), Color.FromArgb(  0,255,127),	// 110
								  Color.FromArgb(127,255,191), Color.FromArgb(  0,204,102), Color.FromArgb(102,204,153), Color.FromArgb(  0,153, 76), Color.FromArgb( 76,153,114),	// 115
								  Color.FromArgb(  0,127, 63), Color.FromArgb( 63,127, 95), Color.FromArgb(  0, 76, 38), Color.FromArgb( 38, 76, 57), Color.FromArgb(  0,255,191),	// 120
								  Color.FromArgb(127,255,223), Color.FromArgb(  0,204,153), Color.FromArgb(102,204,178), Color.FromArgb(  0,153,114), Color.FromArgb( 76,153,133),	// 125
								  Color.FromArgb(  0,127, 95), Color.FromArgb( 63,127,111), Color.FromArgb(  0, 76, 57), Color.FromArgb( 38, 76, 66), Color.FromArgb(  0,255,255),	// 130
								  Color.FromArgb(127,255,255), Color.FromArgb(  0,204,204), Color.FromArgb(102,204,204), Color.FromArgb(  0,153,153), Color.FromArgb( 76,153,153),	// 135
								  Color.FromArgb(  0,127,127), Color.FromArgb( 63,127,127), Color.FromArgb(  0, 76, 76), Color.FromArgb( 38, 76, 76), Color.FromArgb(  0,191,255),	// 140
								  Color.FromArgb(127,223,255), Color.FromArgb(  0,153,204), Color.FromArgb(102,178,204), Color.FromArgb(  0,114,153), Color.FromArgb( 76,133,153),	// 145
								  Color.FromArgb(  0, 95,127), Color.FromArgb( 63,111,127), Color.FromArgb(  0, 57, 76), Color.FromArgb( 38, 66, 76), Color.FromArgb(  0,127,255),	// 150
								  Color.FromArgb(127,191,255), Color.FromArgb(  0,102,204), Color.FromArgb(102,153,204), Color.FromArgb(  0, 76,153), Color.FromArgb( 76,114,153),	// 155
								  Color.FromArgb(  0, 63,127), Color.FromArgb( 63, 95,127), Color.FromArgb(  0, 38, 76), Color.FromArgb( 38, 57, 76), Color.FromArgb(  0, 63,255),	// 160
								  Color.FromArgb(127,159,255), Color.FromArgb(  0, 51,204), Color.FromArgb(102,127,204), Color.FromArgb(  0, 38,153), Color.FromArgb( 76, 95,153),	// 165
								  Color.FromArgb(  0, 31,127), Color.FromArgb( 63, 79,127), Color.FromArgb(  0, 19, 76), Color.FromArgb( 38, 47, 76), Color.FromArgb(  0,  0,255),	// 170
								  Color.FromArgb(127,127,255), Color.FromArgb(  0,  0,204), Color.FromArgb(102,102,204), Color.FromArgb(  0,  0,153), Color.FromArgb( 76, 76,153),	// 175
								  Color.FromArgb(  0,  0,127), Color.FromArgb( 63, 63,127), Color.FromArgb(  0,  0, 76), Color.FromArgb( 38, 38, 76), Color.FromArgb( 63,  0,255),	// 180
								  Color.FromArgb(159,127,255), Color.FromArgb( 51,  0,204), Color.FromArgb(127,102,204), Color.FromArgb( 38,  0,153), Color.FromArgb( 95, 76,153),	// 185
								  Color.FromArgb( 31,  0,127), Color.FromArgb( 79, 63,127), Color.FromArgb( 19,  0, 76), Color.FromArgb( 47, 38, 76), Color.FromArgb(127,  0,255),	// 190
								  Color.FromArgb(191,127,255), Color.FromArgb(102,  0,204), Color.FromArgb(153,102,204), Color.FromArgb( 76,  0,153), Color.FromArgb(114, 76,153),	// 195
								  Color.FromArgb( 63,  0,127), Color.FromArgb( 95, 63,127), Color.FromArgb( 38,  0, 76), Color.FromArgb( 57, 38, 76), Color.FromArgb(191,  0,255),	// 200
								  Color.FromArgb(223,127,255), Color.FromArgb(153,  0,204), Color.FromArgb(178,102,204), Color.FromArgb(114,  0,153), Color.FromArgb(133, 76,153),	// 205
								  Color.FromArgb( 95,  0,127), Color.FromArgb(111, 63,127), Color.FromArgb( 57,  0, 76), Color.FromArgb( 66, 38, 76), Color.FromArgb(255,  0,255),	// 210
								  Color.FromArgb(255,127,255), Color.FromArgb(204,  0,204), Color.FromArgb(204,102,204), Color.FromArgb(153,  0,153), Color.FromArgb(153, 76,153),	// 215
								  Color.FromArgb(127,  0,127), Color.FromArgb(127, 63,127), Color.FromArgb( 76,  0, 76), Color.FromArgb( 76, 38, 76), Color.FromArgb(255,  0,191),	// 220
								  Color.FromArgb(255,127,223), Color.FromArgb(204,  0,153), Color.FromArgb(204,102,178), Color.FromArgb(153,  0,114), Color.FromArgb(153, 76,133),	// 225
								  Color.FromArgb(127,  0, 95), Color.FromArgb(127, 63,111), Color.FromArgb( 76,  0, 57), Color.FromArgb( 76, 38, 66), Color.FromArgb(255,  0,127),	// 230
								  Color.FromArgb(255,127,191), Color.FromArgb(204,  0,102), Color.FromArgb(204,102,153), Color.FromArgb(153,  0, 76), Color.FromArgb(153, 76,114),	// 235
								  Color.FromArgb(127,  0, 63), Color.FromArgb(127, 63, 95), Color.FromArgb( 76,  0, 38), Color.FromArgb( 76, 38, 57), Color.FromArgb(255,  0, 63),	// 240
								  Color.FromArgb(255,127,159), Color.FromArgb(204,  0, 51), Color.FromArgb(204,102,127), Color.FromArgb(153,  0, 38), Color.FromArgb(153, 76, 95),	// 245
								  Color.FromArgb(127,  0, 31), Color.FromArgb(127, 63, 79), Color.FromArgb( 76,  0, 19), Color.FromArgb( 76, 38, 47), Color.FromArgb( 51, 51, 51),	// 250
								  Color.FromArgb( 91, 91, 91), Color.FromArgb(132,132,132), Color.FromArgb(173,173,173),Color.FromArgb(214,214,214),Color.FromArgb(255,255,255)};

	}
}

