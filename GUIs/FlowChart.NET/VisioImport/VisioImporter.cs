using System;
using System.Xml;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Reflection;
using System.Xml.XPath;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using MindFusion.FlowChartX;
using System.Globalization;


namespace MindFusion.Diagramming.Import
{
	/// <summary>
	/// Imports Visio 2003 VDX files into FlowChart.NET
	/// </summary>
	public class VisioImporter
	{
		[DllImport("gdi32.dll", EntryPoint="GetDeviceCaps", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)] 
		static extern int GetDeviceCaps(IntPtr hdc, int nIndex); 


		[DllImport("User32.DLL", EntryPoint="GetActiveWindow", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)]
		static extern IntPtr GetActiveWindow ( ); 
		
		[DllImport("User32.DLL",EntryPoint="GetDC", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)]
		extern static System.IntPtr GetDC(System.IntPtr hWnd); 

		[FlagsAttribute]
		enum ShapeTypeEnum: long 
		{
			ST_NONE = 0x00000000,
			ST_SHAPE = 0x00000001,
			ST_GROUP = 0x00000002,
			ST_CONNECTOR = 0x00000004,
			ST_ENTITY = 0x00000008,
			ST_PART_OF_ENTITY = 0x00000010,
			ST_OVAL_PROCESS = 0x00000020,
			ST_RECTANGLE = 0x00000040,
			ST_STORED_DATA = 0x00000080,
			ST_PART_OF_ARROW = 0x00000100,
			ST_HIDDEN = 0x00000200,
			ST_NO_NAMEU = 0x00000400,
			ST_PART_OF_ENTITY2 = 0x00000800,
			ST_ENTITY2 = 0x00001000
		};

		/// <summary>
		/// Initializes a new instance of the VisioImporter class
		/// </summary>
		public VisioImporter()
		{
		}
		
		private MindFusion.FlowChartX.FlowChart pChart;
		private  MindFusion.FlowChartX.Box oBox;
		private  MindFusion.FlowChartX.Arrow oArrow;
		private  MindFusion.FlowChartX.Table oTable;
		private  MindFusion.FlowChartX.Group oGroup;
		private bool bImportEntitiesAsTables = false;
		private long lPageCount = 0,  lBasedX = 0,lBasedY =0 ,lOffsetX = 0,lOffsetXFinal = 0,lOffsetY = 0 ,
			lGroupY = 0 , lGroupX =0, lGroupPinY = 0, lGroupPinX =0 ,lPagesPerRow = 1, lShapeCount = 0 , 
			lShapesCount = 0 ,	PixPerInch = 0, PixPerInchFirst = 0, lPagesCount = 0, StartTick = 0,EndTick = 0;
		private double PageScale = 1;
		private XmlDocument doc;
		private XmlNamespaceManager ns;
		private XmlNodeList all_pages;
		private XmlNode current_page;
		private XmlNodeList all_shapes;
		private XmlNode current_shape;
		private XmlNode root, colorsRoot, stylesRoot, mastersRoot,connectRoot, facesRoot;
		private XmlNode page_props;
		private string sPageID ="", sDL ="",sMasterID="", sPenStyle = "", sLineStyle="", sTextStyle="", sFillStyle = "", sFillPattern= "",
			sLineStyle2="", sTextStyle2="", sFillStyle2 = "",  sSeparator = NumberFormatInfo.CurrentInfo.NumberDecimalSeparator;
		Color crFillColor,crFillColor2, crLineColor, crTextColor;
		Font fnShape = null;

		/// <summary>
		/// Raised when a Visio shape is imported into FlowChart.NET as a box.
		/// </summary>
		public event BoxImportedEventHandler BoxImported;

		/// <summary>
		/// 'BoxImported' Event handler
		/// </summary>
		/// <param name="Box">Flowchart.NET Box object</param>
		/// <param name="VisioShapeName">Source Visio's shape name</param>
		/// <param name="VisioExtraData">Some extra data</param>
		protected virtual void OnBoxImported(MindFusion.FlowChartX.Box Box,
			string VisioShapeName, string VisioExtraData)
		{
			if (BoxImported != null)
				BoxImported(this, new BoxImportedArgs(Box, VisioShapeName));
		}

		/// <summary>
		/// Raised when a Visio connector is imported into FlowChart.NET as an arrow.
		/// </summary>
		public event ArrowImportedEventHandler ArrowImported;

		/// <summary>
		/// 'ArrowImported' Event handler
		/// </summary>
		/// <param name="Arrow">Flowchart.NET Arrow object</param>
		/// <param name="VisioLinkData">Some extra data/param>
		protected virtual void OnArrowImported(MindFusion.FlowChartX.Arrow Arrow, string shapeName)
		{
			if (ArrowImported != null)
				ArrowImported(this, new ArrowImportedArgs(Arrow, shapeName));
		}

		/// <summary>
		/// Raised when a Visio group is imported into FlowChart.NET
		/// </summary>
		public event GroupImportedEventHandler GroupImported;

		/// <summary>
		/// 'GroupImported' Event handler
		/// </summary>
		/// <param name="Group">Flowchart.NET Group object</param>
		/// 
		protected virtual void OnGroupImported(  MindFusion.FlowChartX.Group Group)
		{
			if (GroupImported != null)
				GroupImported(this, new GroupImportedArgs(Group));
		}

		private double Angle2Deg(string AngleUnit, string AngleValue)
		{
			double fResult = 0, fAngle = 0;
			try
			{
				
				if (AngleValue == null )
					return fResult;

				if ( AngleValue == "0")
					return fResult;

				if ( AngleUnit != null )
				{
					AngleUnit = AngleUnit.Trim();
					AngleUnit = AngleUnit.ToLower();
				}
				else
					AngleUnit = "rad";

				AngleValue = AngleValue.Replace(".", sSeparator);

				switch  (AngleUnit)
				{
					case "am":
					case "as":
					case "ad":
					case "da":
					case "rad":
					case "deg":
					default:
						fAngle = (Convert.ToDouble(AngleValue)/Math.PI)*180;
						break;

				}

				if ( fAngle>0 )
					fResult = 360 - Math.Abs(fAngle);
				else
					fResult = Math.Abs(fAngle);

			
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","Angle2Rad",ex.Message));
				fResult = 0;
			}

			return fResult;
		}

		/// <summary>
		/// Imports the specified page from a Visio drawing into FlowChart.NET
		/// </summary>
		/// <param name="vdxPathName">Path to the VDX file</param>
		/// <param name="ChartRef">Reference to the FlowChart instance</param>
		/// <param name="PageIndex">Zero based index of the source page.</param>
		public void ImportPage(string vdxPathName, FlowChart flowChart, int PageIndex)
		{
			string sResult = "", sTemp = "", sPath = "";
			try
			{
				// Initializing cros-call members
				pChart = flowChart;
				doc = null;
				ns = null;
				all_pages = null;
				root = null;
				current_page = null;
				all_shapes = null;
				current_shape = null;
				lPagesCount = 0;
				lShapesCount = 0;
				lPageCount = 0;
				lShapeCount = 0;
				StartTick = 0;
				EndTick = 0;
				lOffsetX = 0;
				lOffsetY = 0;

				// Getting screen resolution 'Pix-Per-Inch' parameter 
				IntPtr hWnd = GetActiveWindow(); 
				System.IntPtr hDC = GetDC(hWnd);
				PixPerInch = GetDeviceCaps(hDC, 88 );
				PixPerInchFirst = PixPerInch;

				// Setting measure unit to 'Pixels' to avoid problems

				pChart.ClearAll();
				pChart.MeasureUnit = System.Drawing.GraphicsUnit.Pixel;
				pChart.AutoSizeDoc = MindFusion.FlowChartX.AutoSize.RightAndDown;

				// Loading XML document using namespace 
				doc = new XmlDocument();
					
				string sXml = null;

				System.IO.File.Copy(vdxPathName, vdxPathName +".copy" , true );
				System.IO.StreamReader sr = new System.IO.StreamReader(vdxPathName +".copy");
				
				if ( sr == null ) 
				{
					doc.Load(vdxPathName);
				}
				else
				{
				
					sXml = sr.ReadToEnd();
					if ( sXml != null )
					{
						doc.LoadXml(sXml);
					}
					sr.Close();
					System.IO.File.Delete(vdxPathName +".copy");
				}

				
				
				ns =new XmlNamespaceManager(doc.NameTable);
				root = doc.DocumentElement;
				ns.AddNamespace("vdx", "http://schemas.microsoft.com/visio/2003/core");
				if ( PageIndex == -1 )
					sPath ="//vdx:Pages/vdx:Page";
				else
					sPath = String.Format("//vdx:Pages/vdx:Page[{0}]", PageIndex + 1);

				all_pages = root.SelectNodes(sPath, ns);
				lPageCount = all_pages.Count;
				
				// Hash some common-used VDX nodes

				colorsRoot = null;
				colorsRoot = root.SelectSingleNode("//vdx:Colors", ns);

				mastersRoot = null;
				mastersRoot = root.SelectSingleNode("//vdx:Masters", ns);
				
				stylesRoot = null;
				stylesRoot = root.SelectSingleNode("//vdx:StyleSheets", ns);
			
				facesRoot = null;
				facesRoot = root.SelectSingleNode("//vdx:FaceNames", ns);

				// Setting start tick
				StartTick = Environment.TickCount;

				// Page processing loop for all pages required
				foreach( XmlNode current_page2 in all_pages)
				{
					// Getting page's parameters & properties

					if ( current_page2 == null )
						continue;

					current_page = current_page2;
					connectRoot = null;
					connectRoot = current_page.SelectSingleNode("vdx:Connects", ns);
					lPagesCount++;
					sPageID = current_page.Attributes["ID"].Value.ToString();
					page_props = current_page.SelectSingleNode("vdx:PageSheet/vdx:PageProps", ns);
					if ( page_props == null)
						continue;
					
					// Getting default page's measure Unit
					sPath = String.Format("//vdx:Pages/vdx:Page[@ID='{0}']/vdx:PageSheet/vdx:PageProps/vdx:PageHeight", sPageID);
					sDL = GetShapeAttr(sPath, "Unit");

					// Getting page's scale
					sPath = String.Format("//vdx:Pages/vdx:Page[@ID='{0}']/vdx:PageSheet/vdx:PageProps/vdx:PageScale", sPageID);
					sTemp = GetShapeAttr(sPath, "");

					sTemp = sTemp.Replace(".",sSeparator);
					PageScale = System.Convert.ToDouble(sTemp);
				    
					PixPerInch = (long ) ( PixPerInchFirst / PageScale);

					// Getting page's dimensions
					lBasedX = Measure2Pix(page_props, "PageWidth");
					lBasedY = Measure2Pix(page_props, "PageHeight");
					
					
					

					//sPath = String.Format("//vdx:Pages/vdx:Page[@ID={0}]/vdx:Shapes/vdx:Shape", sPageID);
					//all_shapes = root.SelectNodes(sPath, ns);
				
					// Preparing Xpath expression for non-arrow shape loop
					all_shapes = current_page.SelectNodes("vdx:Shapes/vdx:Shape",ns);
					lShapeCount = all_shapes.Count;
						
					// Shape processing loop for the page selected
					foreach( XmlNode current_shape2 in all_shapes)
					{
						current_shape = current_shape2;
						if ( current_shape2 == null )
							continue;

						XmlNode is_arrow = current_shape.SelectSingleNode("vdx:XForm1D", ns);
						if ( is_arrow == null )
							GetShapeRef(current_shape);
						else
							continue;
					}


					// Preparing Xpath expression for arrow - like shape loop
					all_shapes = current_page.SelectNodes("vdx:Shapes/vdx:Shape",ns);
					lShapeCount = all_shapes.Count;
						
					// Shape processing loop for the page selected
					foreach( XmlNode current_shape2 in all_shapes)
					{
						current_shape = current_shape2;
						if ( current_shape2 == null )
							continue;

						XmlNode is_arrow = current_shape.SelectSingleNode("vdx:XForm1D", ns);
						if ( is_arrow != null )
							GetShapeRef(current_shape);
						else
							continue;
					}

					lOffsetX+=lBasedX;
					if ( lPagesCount % lPagesPerRow == 0)
					{
						lOffsetY+=lBasedY;
						lOffsetXFinal = lOffsetX;
						lOffsetX=0;
					}
					
				};
				
				// Setting page's size got as FlowChartX Document's size
				if (( lOffsetY!=0) && (lOffsetXFinal!=0))
				{
					
					pChart.DocExtents = new System.Drawing.RectangleF(0,0,lOffsetXFinal, lOffsetY );
													
				}

				if ( lShapesCount == 0 )
					lShapesCount = 1;

				// Setting end tick
				EndTick = Environment.TickCount;

				sResult = String.Format("{0} page(s) imported\n-----------------------\nOverall time:\t{1} ms\nShapes & Arrows:\t{2}\nTime per shape:\t{3} ms", 
					lPagesCount, EndTick - StartTick, lShapesCount , (EndTick - StartTick) / lShapesCount);
				doc = null;
				
			}
			catch (Exception ex)
			{
				sResult = ex.Message;
			}
		}

		/// <summary>
		/// Gets the number of pages in a Visio drawing file.
		/// </summary>
		/// <param name="vdxPathName">Path to a VDX file</param>
		/// <returns>The number of pages</returns>
		public long GetPageCount(string vdxPathName)
		{
			long iRet = 0;
			try
			{
				
				// Loading XML document using namespace 
				doc = new XmlDocument();
					
				string sXml = null;

				System.IO.File.Copy(vdxPathName, vdxPathName +".copy" , true );
				System.IO.StreamReader sr = new System.IO.StreamReader(vdxPathName +".copy");
				
				if ( sr == null ) 
				{
					doc.Load(vdxPathName);
				}
				else
				{
				
					sXml = sr.ReadToEnd();
					if ( sXml != null )
					{
						doc.LoadXml(sXml);
					}
					sr.Close();
					System.IO.File.Delete(vdxPathName +".copy");
				}
				
				XmlNamespaceManager ns =new XmlNamespaceManager(doc.NameTable);
				XmlNode root = doc.DocumentElement;
				ns.AddNamespace("vdx", "http://schemas.microsoft.com/visio/2003/core");
				XmlNodeList all_pages;
				all_pages = root.SelectNodes("//vdx:Pages/vdx:Page", ns);
				lPageCount = all_pages.Count;
				iRet = lPageCount;
				doc = null;
				
			}
			catch (Exception ex)
			{
				iRet = -1;
				Trace.WriteLine(String.Format("{0} error {1}\n","GetPageCount",ex.Message));
			}
			
			return iRet;
		}

		/// <summary>
		/// Imports all pages from a Visio drawing file.
		/// </summary>
		/// <param name="vdxPathName">The path and name of the VDX file</param>
		/// <param name="ChartRef">Reference to a FlowChart object</param>
		public void ImportAllPages(string vdxPathName, FlowChart flowChart)
		{
			ImportPage(vdxPathName, flowChart, -1 ) ;
		}

		/// <summary>
		/// Getting VDX node attribute or text
		/// </summary>
		/// <param name="AttrPath">String XPath expression to found child node of the [shape]</param>
		/// <param name="AttrName">String Name of the attribute or "" for [Text]</param>
		/// <param name="shape">Parent node reference or [null] if root node is parent</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		
		private string GetShapeAttr(string AttrPath, string AttrName,  XmlNode shape)
		{
			string sTemp = null;
			try
			{

				XmlNode temp_node = null;
				
				if ( shape == null )
					temp_node = root.SelectSingleNode(AttrPath, ns);
				else
				{
					if ( AttrPath != "" )
						temp_node = shape.SelectSingleNode(AttrPath, ns);
					else
						temp_node = shape;
				}
				if ( temp_node == null )
					return null;

				if ( AttrName == "" )
					sTemp = temp_node.InnerText;
				else
					sTemp = temp_node.Attributes[AttrName].Value;
				
				
			}
			catch (Exception ex)
			{
				sTemp = null;
				Trace.WriteLine(String.Format("{0} error {1}\n","GetShapeAttr",ex.Message));
			}

			return sTemp;
		}

		/// <summary>
		/// Getting VDX node attribute or text
		/// </summary>
		/// <param name="AttrPath">String XPath expression to found child node of the [shape]</param>
		/// <param name="AttrName">String Name of the attribute or "" for [Text]</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private string GetShapeAttr(string AttrPath, string AttrName )
		{
			return  GetShapeAttr( AttrPath,  AttrName, null);
		}

		/// <summary>
		/// Converts string dimension value from unit to pixels
		/// </summary>
		/// <param name="Unit">Name of the unit of measures</param>
		/// <param name="DefaultUnit">>Name of the DEFAULT unit of measures</param>
		/// <param name="Value">String value of the dimensions</param>
		/// <returns>Dimension value in pixels</returns>
		private double Unit2Pix(string Unit, string DefaultUnit, string Value)
		{
			string sUnit = Unit, sDefaultUnit = DefaultUnit;
			double fVal, fRes;

			// Setting decimal separator to ','
			Value = Value.Replace(".",sSeparator);
			fVal = System.Convert.ToDouble(Value);

			sUnit =	sUnit.ToLower();
			sDefaultUnit = sDefaultUnit.ToLower();

			if ( sUnit == "dl" )
				sUnit = sDefaultUnit;

			if ( sUnit == "in" )
			{
				fRes = fVal*PixPerInch;
			}
			else if ( sUnit == "mm" )
			{
				fRes = (double) (fVal/25.1)*PixPerInch;
			}
			else if ( sUnit == "cm" )
			{
				fRes = (double) (fVal/2.51)*PixPerInch;
			}
			else
				fRes = 0;

			if ( sUnit != sDefaultUnit)
				fRes = fRes * PageScale;

			return fRes;
		}

		/// <summary>
		/// Processing Visio shape converting it into Flowchart.NET node
		/// </summary>
		/// <param name="shape">Visio shape's XML node</param>
		/// <param name="ShapeType">String value of the shape type</param>
		/// <param name="group">Reference to parent group ( if any exists) </param>
		/// <returns>Returns [true] if successfull [false] otherwise</returns>
		private bool GetShapeRef(XmlNode shape, string ShapeType,MindFusion.FlowChartX.Group group)
		{
			string sShapeName, sUID = "", sType = "",  sNameU= "", sPath="", sTemp = "",
				sStepX = "", sStepY = "";

			long lShapeType = 0, lX = 0, lY =0, lWidth = 0,
				lHeight = 0, lPinY = 0, lPinX = 0, lLineWith = 0,lCount = 1, lPtX = 0,  lPtY = 0 , lStepX = 0, lStepY = 0, lStartX =0 ,
				lStartY = 0,lEndX = 0, lEndY = 0, lBeginX = 0, lBeginY = 0, lBoxX = 0, lBoxY = 0,
				lTemp = 0, lAdjustX = 0, lAdjustY = 0;

			double fAngle2 = 0;
			XmlNode node_temp = null;
			XmlNode xform1d_node = null;
		
			// Incrementing shape's count
			lShapesCount++;
			sUID = shape.Attributes["ID"].Value.ToString();
			sType = shape.Attributes["Type"].Value.ToString();
			sType = sType.ToLower();

			// Getting 'XForm' node root
			XmlNode xform_node = shape.SelectSingleNode("vdx:XForm", ns);
			
			if ( xform_node == null)
				return false;

			
			
			// Getting Master shape's ID if any exist
			sMasterID = "";
			if ( shape.Attributes["Master"] == null )
			{
				if ( shape.Attributes["MasterShape"] != null )
				{
					sMasterID = shape.Attributes["MasterShape"].Value.ToString();
				}
			}
			else
				sMasterID = shape.Attributes["Master"].Value.ToString();

			// Getting shape's 'NameU' tag if any exist
			sNameU = "";
			if ( shape.Attributes["NameU"] != null )
				sNameU = shape.Attributes["NameU"].Value.ToString();
			sShapeName = sNameU;
			sNameU = sNameU.Trim();
			sNameU = sNameU.ToLower();
			
			if ( sNameU == "" )
			{
				lShapeType |= (long) ShapeTypeEnum.ST_NO_NAMEU;
			}

			if (( sNameU == "") && (sMasterID!=""))
			{
				sPath = String.Format("vdx:Master[@ID='{0}']",sMasterID);
				sNameU = GetShapeAttr(sPath,"NameU", mastersRoot);
				sShapeName = sNameU;
			}

			if ( sNameU == null )
				sNameU = "";
				
			sNameU = sNameU.Trim();
			sNameU = sNameU.ToLower();
			
			// Shape is GROUP
			if ( sType.IndexOf("group")>=0 )
			{
				lShapeType |= (long) ShapeTypeEnum.ST_GROUP;
			}

			// Shape is 'arrow - like'
			if (( sNameU.IndexOf("connector") >= 0 ) || ( sNameU.IndexOf("link") >= 0 ) || ( sNameU.IndexOf("generalization") >= 0 ) ||
				( sNameU.IndexOf("binary association") >= 0 ) || ( sNameU.IndexOf("composition")>=0) ||  ( sNameU == "relationship") ||
				( sNameU.IndexOf("parent to category") >= 0 ) || ( sNameU.IndexOf("categorytochild ") >= 0 ) )
			{
				lShapeType |= (long) ShapeTypeEnum.ST_CONNECTOR;

			}
			else 
			{
				xform1d_node = shape.SelectSingleNode("vdx:XForm1D", ns);
				if ( xform1d_node != null)
				{
					//return true;
				//	??? REMOVED ??? lShapeType |= (long) ShapeTypeEnum.ST_CONNECTOR;
				}
			}

			// Shape is 'entity - like'

			if ( ( sNameU == "entity 1" ) || ( sNameU.IndexOf("entity." )>=0) || ( sNameU == "entity 2" ) || ( sNameU == "entity" ) || ( sNameU =="view"))
			{
				lShapeType |= (long) ShapeTypeEnum.ST_ENTITY;
			}
		

			// Shape  is 'entity2 - like'
			if ( sNameU == "entity 2" )
			{
				lShapeType |= (long) ShapeTypeEnum.ST_ENTITY2;
			}

			// Shape  is part of 'entity - like' group
			if ( ShapeType == "entity" )
			{
				lShapeType |= (long) ShapeTypeEnum.ST_PART_OF_ENTITY;
				lShapeType |= (long) ShapeTypeEnum.ST_SHAPE;
			}

			// Shape  is part of 'entity2 - like' group
			if ( ShapeType == "entity 2" )
			{
				lShapeType |= (long) ShapeTypeEnum.ST_PART_OF_ENTITY;
				lShapeType |= (long) ShapeTypeEnum.ST_PART_OF_ENTITY2;
				lShapeType |= (long) ShapeTypeEnum.ST_SHAPE;
			}

			// Shape  is part of 'arrow - like' group
			if ( ShapeType == "arrow" )
			{
				lShapeType |= (long) ShapeTypeEnum.ST_PART_OF_ARROW;
				lShapeType |= (long) ShapeTypeEnum.ST_SHAPE;
			}
			
			if (((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_PART_OF_ENTITY) == ShapeTypeEnum.ST_PART_OF_ENTITY)
			{
				lShapeType |= (long) ShapeTypeEnum.ST_OVAL_PROCESS;
				lShapeType |= (long) ShapeTypeEnum.ST_SHAPE;
			}

			if ( sNameU == "parameters" )
			{
				lShapeType |= (long) ShapeTypeEnum.ST_HIDDEN;
				lShapeType |= (long) ShapeTypeEnum.ST_SHAPE;
			}

			if (( lShapeType == (long) ShapeTypeEnum.ST_NONE ) || ( lShapeType == (long) ShapeTypeEnum.ST_NO_NAMEU))
				lShapeType |= (long) ShapeTypeEnum.ST_SHAPE;

			// Getting shape's dimensions and converting it to 'Pixels'
			lWidth = Measure2Pix(xform_node, "Width");
			lHeight = Measure2Pix(xform_node, "Height");
			lX = Measure2Pix(xform_node, "PinX");
			lY = Measure2Pix(xform_node, "PinY");
			lPinX = Measure2Pix(xform_node, "LocPinX");
			lPinY = Measure2Pix(xform_node, "LocPinY");
			
			if ((( lWidth == 0 ) || ( lHeight == 0 )) && 
				(((ShapeTypeEnum)lShapeType & ShapeTypeEnum.ST_SHAPE) == ShapeTypeEnum.ST_SHAPE) && 
				(((ShapeTypeEnum)lShapeType & ShapeTypeEnum.ST_CONNECTOR)!=ShapeTypeEnum.ST_CONNECTOR))
			{
				return false;
			}

			if ( sNameU == "separator")
			{
				return false;
			}	

		
			// Getting shape's styles IDs
			sLineStyle = GetStyle(shape, "LineStyle", ref sLineStyle2);
			sTextStyle = GetStyle(shape, "TextStyle", ref sTextStyle2);
			sFillStyle = GetStyle(shape, "FillStyle", ref sFillStyle2);

			// Getting colors
			crLineColor = Color.Empty;
			crLineColor = GetColor(shape,sLineStyle,"vdx:Line/vdx:LineColor",null,null,sLineStyle2);

			crFillColor = Color.Empty;
			crFillColor = GetColor(shape,sFillStyle,"vdx:Fill/vdx:FillForegnd","vdx:Fill/vdx:FillForegndTrans",null,sFillStyle2);

			crFillColor2 = Color.Empty;
			crFillColor2 = GetColor(shape,sFillStyle,"vdx:Fill/vdx:FillBkgnd",null,null,sFillStyle2);

			crTextColor = Color.Empty;
			crTextColor = GetColor(shape,sTextStyle,"vdx:Char/vdx:Color",null,null,sTextStyle2);

			// Getting fill patterns
			sFillPattern = "";
			sFillPattern = GetShapeAttr("vdx:Fill/vdx:FillPattern","", shape);
			if ( sFillPattern == null)
			{
				sFillPattern = GetShapeAttr(String.Format("vdx:StyleSheet[@ID='{0}']/vdx:Fill/vdx:FillPattern", sFillStyle),"",stylesRoot);
				
			}

			sTemp = GetShapeAttr("vdx:Line/vdx:LineWeight","",shape);
			if ( sTemp == null)
			{
				sTemp = GetShapeAttr(String.Format("vdx:StyleSheet[@ID='{0}']/vdx:Line/vdx:LineWeight", sLineStyle),"",stylesRoot);
				
			}

			// Getting line width
			if ( sTemp!=null )
				lLineWith = (long) this.Unit2Pix(sDL,sDL,sTemp);
			else
				lLineWith = 0;
			
			// Getting rotaton angle
			string sAngle = "0", sAngleUnit = "";
			sAngle = GetShapeAttr("vdx:Angle", "", xform_node);
			sAngleUnit = GetShapeAttr("vdx:Angle", "Unit", xform_node);

			fAngle2 = Angle2Deg(sAngleUnit, sAngle);
			
			/*
			if (( sAngle != "0") && ( sAngle!=null))
			{
				sAngle = sAngle.Replace(".",sSeparator);
				fAngle = Convert.ToDouble(sAngle);

				if ( fAngle!=0 )
				{
					fAngle2 = (fAngle/Math.PI)*180;
					if ( fAngle2>0 )
						fAngle2 = 360 - Math.Abs(fAngle2);
					else
						fAngle2 = Math.Abs(fAngle2);

					
				}


			}
			else
			{
				lAngle = 0;
			}

			*/

			sPenStyle = "";
			sPenStyle =  GetShapeAttr("vdx:Line/vdx:LinePattern","", shape);
			if ( sPenStyle == null )
			{
				sPenStyle =  GetShapeAttr(String.Format("vdx:StyleSheet[@ID='{0}']/vdx:Line/vdx:LinePattern", sLineStyle),"", stylesRoot);
			}

			if ((((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_SHAPE ) ==  ShapeTypeEnum.ST_SHAPE)||
				(((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_GROUP ) == ShapeTypeEnum.ST_GROUP)||
				(((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_PART_OF_ARROW ) == ShapeTypeEnum.ST_PART_OF_ARROW))
			{
				if (group != null)			
				{
					// Getting coordinates of the upper left corner of the group
					lBoxX = lOffsetX + lGroupX - lGroupPinX;
					lBoxY = lOffsetY + lBasedY -  ( lGroupY + lGroupPinY );

					// Getting coordinates of the grouped shape
					if (( (ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_ENTITY ) == ShapeTypeEnum.ST_ENTITY )
						lPinY = 0;

					lBoxX = lBoxX + lX - lPinX;

			
					// Aplying coordinates adustment is shape is part of 'entity-like' group
					if ((((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_ENTITY )!=ShapeTypeEnum.ST_ENTITY ) && 
						(((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_PART_OF_ENTITY ) == ShapeTypeEnum.ST_PART_OF_ENTITY) && 
						(((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_NO_NAMEU)== ShapeTypeEnum.ST_NO_NAMEU))
					{

						if (((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_PART_OF_ENTITY2 ) == ShapeTypeEnum.ST_PART_OF_ENTITY2)
							lBoxY = lBoxY + lGroupPinY*2 - lY;
						else
							lBoxY = lBoxY;

					}
					else
					{
						lBoxY = lBoxY + lGroupPinY*2 - lY - lPinY ;
					}
		

				}
				else // If shape is NOT a part of group
				{


					lBoxX = lOffsetX + lX - lPinX;
					lBoxY = lOffsetY + lBasedY -  ( lY + lPinY );

				}
			
				if  (( bImportEntitiesAsTables ) && ( (((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_ENTITY ) == ShapeTypeEnum.ST_ENTITY) ))
				{
					// If 'entity - like' shape and 'ImportEnitiesAsTables' mode ON
					// then creating group & table instead of group's content

					oTable = pChart.CreateTable( lBoxX  , lBoxY ,lWidth,lHeight);
					if ( oTable == null )
						return false;

					int lRowCount = 0, lCol = 0, lRow = 0;
					bool IsCaption = true;

					XmlNodeList entity_items = shape.SelectNodes("vdx:Shapes/vdx:Shape",ns);
					foreach ( XmlNode entity_node in entity_items )
					{
						if ( entity_node == null)
							continue;
							
						if ( IsCaption )
						{
							sTemp = GetShapeAttr("vdx:Text", "", entity_node);

							if ( sTemp!=null )
								oTable.Caption = sTemp;

							sTemp = GetShapeAttr("vdx:Fill/vdx:FillForegnd", "", entity_node);
							crFillColor = GetColor(entity_node,"","vdx:Fill/vdx:FillForegnd", null, sTemp, null);
							IsCaption = false;
						}
						else
						{
							// Processing Flowchart Table object 
							sTemp = GetShapeAttr("vdx:User[@NameU='LineCount']/vdx:Value", "", entity_node );
							if ( sTemp!=null )
							{
								lRowCount = Convert.ToInt16(sTemp);
								oTable.RowCount = lRowCount;

							}

							if ( lRowCount == 0 )
								lRowCount = 1;

							oTable.RowHeight = lHeight /( lRowCount);
							oTable.CaptionHeight = lHeight /( lRowCount );
							oTable.RedimTable(2, lRowCount );
							XmlNodeList table_list = null;
							table_list = entity_node.SelectNodes("vdx:Text/text()", ns);

							foreach ( XmlNode text_node in table_list )
							{
								// Parsing text using '\n' as separator
								sTemp = text_node.InnerText.ToString();
								sTemp = sTemp.Trim();
								string [] split = sTemp.Split(new Char [] {'\n'});
								foreach ( string text_piece in split )
								{
									if (text_piece!=null)
									{
										if ( oTable.RowCount <= lRow )
											oTable.RowCount++;
												
										oTable[lCol,lRow].Text = text_piece;
									}
											
									lRow++;
								}

							
							
								
								lRow--;
								lRow++;
							
								if (( lRow == 1 ) && (lCol == 0))
								{
									lCol = 1;
									lRow = 0;
								}
							}
								

						}
							
					}

					// Applying table parameters
					//oTable.CaptionColor  = crFillColor;
					oTable.Brush =  new MindFusion.FlowChartX.SolidBrush(Color.White);
					oTable.FillColor = Color.White;
					oTable.CellFrameStyle = CellFrameStyle.Simple;
					oTable.Tag =  sPageID + "_" + sUID;
		
					return true;
				}
				else
					oBox = pChart.CreateBox( lBoxX  , lBoxY ,lWidth,lHeight);
			
				if ( oBox.Equals(null) )
					return false;
			
				oBox.Tag = sPageID + "_" + sUID;

				Trace.WriteLine(String.Format("Box: {0}",sUID));

				if ( sNameU == "" )
					sTemp = sType;
				else
				{
					sTemp = sNameU;
				}

				ShapeTemplate templ1 = 	null;

				if ( ((ShapeTypeEnum)lShapeType & ShapeTypeEnum.ST_PART_OF_ENTITY) == ShapeTypeEnum.ST_PART_OF_ENTITY )
					templ1 = VisioShape2PredefinedShape("Process");
				else
					templ1 = VisioShape2PredefinedShape(sTemp);
		
				if ( templ1 == null )
					return false;

				oBox.Style = BoxStyle.Shape;
				oBox.Shape = templ1;
			
				if ( lHeight == 0 )
				{
					// Getting shape's begin & end coordinates
					lBeginX = Measure2Pix(xform1d_node, "BeginX");
					lBeginY = Measure2Pix(xform1d_node, "BeginY");
					lEndX = Measure2Pix(xform1d_node, "EndX");
					lEndY = Measure2Pix(xform1d_node, "EndY");
				
			
					if ( lEndX != lBeginX)
						lHeight = System.Math.Abs(lEndX - lBeginX);
					else if ( lEndY != lBeginY)
						lHeight = System.Math.Abs(lEndY - lBeginY);
					else
						lHeight = 0;


				}

				/*
				 * 
				 * 
				 * ROTATION
				// If shape has some rotattion then correct Width & Heigh
				if ((lAngle == 90) || ( lAngle == 270))
				{
					long lH =lHeight, lW = lWidth, lDx = Math.Abs(lW-lH);
					lHeight = lW;
					lWidth = lH;
					lBoxX+= lDx/2;
					lBoxY-= lDx/2;
				}
				*/

				// Processing controls & bitmaps

				if ( sType.IndexOf("foreign") >= 0 )
					ProcessForeign(shape);
				// Applying shape's colors, pens etc.

				if ( sFillPattern == "" )
					sFillPattern = "0";
			
				
				if ( System.Convert.ToInt16(sFillPattern) > 1 )
					oBox.Brush = new MindFusion.FlowChartX.LinearGradientBrush(crFillColor, crFillColor2, 90);
				else
					oBox.Brush = new MindFusion.FlowChartX.SolidBrush(crFillColor);
				
				oBox.Pen = new MindFusion.FlowChartX.Pen(crLineColor,lLineWith);

				/*
				 * 
				 * ROTATION
				if ( lAngle!=0 )
					oBox.ShapeOrientation = lAngle;

				*/

				if ( fAngle2!=0)
				{
					oBox.RotationAngle = (float) fAngle2;
					oBox.RotateContents = true;
				}

				// set the shape's text
				Font ff = GetFont(shape);
				if ( ff == null )
					oBox.Font = (Font)pChart.Font.Clone();
				else
					oBox.Font = ff;
				sTemp = GetShapeText(shape, oBox);
				if (sTemp != null)
				{
					oBox.PolyTextLayout = true;
					oBox.EnableStyledText = true;
						oBox.Text = sTemp;
					oBox.TextColor = crTextColor;
				}

				if (group!=null)			
				{
					if ( ((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_PART_OF_ARROW ) == ShapeTypeEnum.ST_PART_OF_ARROW)
						//if ( lShapeType & ST_PART_OF_ARROW)
					{
						group.AttachToArrowPoint(oBox,1);
						oBox.Transparent = true;
					}
					else
					{
						group.AttachToCorner(oBox,0);
					}

					oBox.Locked = true;
				}

				if ( sPenStyle!=null )
					oBox.PenDashStyle = String2DashStyle(sPenStyle);

				// Raising 'BoxImported' event
				OnBoxImported(oBox, sShapeName, "");
			}
				// If the shape is connector then creating the arrow
			else if ( ((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_CONNECTOR ) == ShapeTypeEnum.ST_CONNECTOR)
			{

				if ( connectRoot == null )
					return false;

				sPath = String.Format("vdx:Connect[@FromSheet='{0}']", sUID);
				XmlNodeList connected_list = connectRoot.SelectNodes(sPath, ns);
					 
				if  (connected_list == null)
					return false;
					 
					

				lTemp = 0;
				string sFrom = "", sTo = "";
				foreach ( XmlNode EachConnection in connected_list)
				{
					if ( EachConnection == null )
						break;
 					
					if ( EachConnection.Attributes["ToSheet"]!=null)
						if ( lTemp == 0 )
							sFrom = EachConnection.Attributes["ToSheet"].Value.ToString();
						else
							sTo = EachConnection.Attributes["ToSheet"].Value.ToString();

					if ( lTemp>0 )
						break;

					lTemp++;

				}
			
				Trace.WriteLine(String.Format("Arrow: {0} - {1}",sFrom,sTo));
				oArrow = CreateFcxLink(sPageID + "_" + sFrom, sPageID + "_" + sTo);
				if ( oArrow == null )
					return false;

				
				// Setting Arrow's ID
				oArrow.Tag = sPageID + "_" + sUID;

				xform1d_node = shape.SelectSingleNode("vdx:XForm1D", ns);
				if ( xform1d_node == null)
					return false;

				// Getting arrow's begin & end coordinates
				lBeginX = Measure2Pix(xform1d_node, "BeginX");
				lBeginY = Measure2Pix(xform1d_node, "BeginY");
				lEndX = Measure2Pix(xform1d_node, "EndX");
				lEndY = Measure2Pix(xform1d_node, "EndY");
				
				// Detecting arrow style
				ArrowStyle stArrow = ArrowStyle.Cascading;

				sTemp = GetShapeAttr("vdx:Layout/vdx:ConLineRouteExt", "", shape);
				if ( sTemp == "2")
					stArrow = ArrowStyle.Bezier;
				else if ( sTemp == "1")
					stArrow = ArrowStyle.Polyline;

				// If arrow is 'line curved' then set 'asBezier' type
				if ( sNameU == "line-curve connector" )
					stArrow = ArrowStyle.Bezier;


			
				// Counting segments arrow consists in
				XmlNodeList segments_list = shape.SelectNodes("vdx:Geom/vdx:LineTo", ns);
				lTemp = segments_list.Count;
				
				if (( stArrow  == ArrowStyle.Cascading) || ( stArrow  == ArrowStyle.Polyline))
				{
					if (lTemp>1)
						stArrow = ArrowStyle.Cascading;
					else
					{
						stArrow = ArrowStyle.Polyline;
					
						
					}
				}

				//Setting arrow style
				oArrow.Style = stArrow;


				if (stArrow == ArrowStyle.Cascading)
				{
					// If the arrow has more than one segment than
					// setting 'asPerpendicular' style for arrow

					oArrow.SegmentCount = (short) lTemp;

					// Setting coordinates of the arrow's origin
					lStartX = lOffsetX + lBeginX ; 
					lStartY = lOffsetY + lBasedY -  ( lBeginY  );
					oArrow.ControlPoints[0]= new PointF(lStartX, lStartX);
					lPtX = lStartX;
					lPtY = lStartY;
					Trace.WriteLine(String.Format("Arrow {0}\tPt:{1}\tX:{2}\tY:{3}",sUID,0,lPtX, lPtY));
                  

					// TODO: If source shape ( pBoxFrom) is non-symmetric ( triangle, pentagon, etc )
					//       arrow base's coordinates need to be adjusted to be source shape's part
					//		 such as follows:
					//		 pArrow->CtrlPtX[0] = ????;
					//		 pArrow->CtrlPtY[0] = ????;

					// Getting arrow segments 'one-by-one' using 'LineTo' XML nodes
					foreach ( XmlNode segment in segments_list )
					{
						if ( segment == null )
							continue;

						// Getting X-coordinate
						node_temp = segment.SelectSingleNode("vdx:X", ns);

						if ( node_temp == null )
						{
							oArrow.Style = ArrowStyle.Polyline;
							break;
						}

						sStepX = node_temp.InnerText;
						lStepX = (long) Unit2Pix(sDL, sDL, sStepX);
						lPtX = lStartX + lStepX;


						// Getting Y-coordinate
						node_temp = segment.SelectSingleNode("vdx:Y", ns);

						if ( node_temp == null )
						{
							oArrow.Style = ArrowStyle.Polyline;
							break;
						}


						sStepY = node_temp.InnerText;
						lStepY = (long) Unit2Pix(sDL, sDL, sStepY);
						lPtY = lStartY - lStepY;

						// Setting new segment for FloChartX arrow
						if ( lCount <= lTemp)
						{
							oArrow.ControlPoints[(int) lCount]= new PointF(lPtX, lPtY);
							Trace.WriteLine(String.Format("Arrow {0}\tPt:{1}\tX:{2}\tY:{3}",sUID,lCount,lPtX, lPtY));
						}
					
						lCount++;
					}
			
			

				}
				else
				{
					// Setting coordinates of the arrow's origin & destination

						
					lAdjustX = 0;
					lAdjustY = 0;
					if ( stArrow == ArrowStyle.Bezier )
					{
						// If curved shape has control points then reflecting adustments
				
						oArrow.SegmentCount = 1;

						if ( sNameU == "line-curve connector" )
						{
				
							node_temp = shape.SelectSingleNode("vdx:Control/vdx:X", ns);
							if ( node_temp != null )
							{
								sTemp = node_temp.InnerText;
								lAdjustX = (long) Unit2Pix(sDL, sDL,sTemp);

							}

							node_temp = shape.SelectSingleNode("vdx:Control/vdx:Y", ns);
							if ( node_temp != null )
							{
								sTemp = node_temp.InnerText;
								lAdjustY = (long) Unit2Pix(sDL, sDL,sTemp);

							}
					
						}

						PointF cpt = oArrow.ControlPoints[0];
						oArrow.ControlPoints[1]= new PointF(cpt.X + lAdjustX , cpt.Y -lAdjustY/2 );
						oArrow.ControlPoints[2]= new PointF(cpt.X + lAdjustX , cpt.Y -lAdjustY/2 );
						oArrow.ControlPoints[3]= new PointF(lOffsetX + lEndX , lOffsetY + lBasedY -  ( lEndY  ));

					}
					else
					{
						oArrow.SegmentCount = 1;
					}
				
				}
				// Setting Begin/End point of the arrow
				oArrow.ControlPoints[0]= new PointF(lOffsetX + lBeginX, lOffsetY + lBasedY -  ( lBeginY  ));	
				oArrow.ControlPoints[oArrow.SegmentCount]= new PointF(lOffsetX + lEndX, lOffsetY + lBasedY -  ( lEndY  ));

				// Updating arrow
				oArrow.UpdateFromPoints();

				// Settiing ArrowBase property
				sTemp = null;
				
				sTemp = GetShapeAttr("vdx:Line/vdx:BeginArrow","",shape);
				if (( sTemp == null ) && ( sLineStyle!=null))
				{
					sTemp = GetShapeAttr(String.Format("vdx:StyleSheet[@ID='{0}']/vdx:Line/vdx:BeginArrow",sLineStyle),
						"",stylesRoot);
				}

				if ( sTemp!=null )
				{
					oArrow.ArrowBase = String2ArrowType( sTemp, ref crLineColor);
				}


				// Settiing ArrowBaseSize property
				sTemp = null;
				
				sTemp = GetShapeAttr("vdx:Line/vdx:BeginArrowSize","",shape);
				if (( sTemp == null ) && ( sLineStyle!=null))
				{
					sTemp = GetShapeAttr(String.Format("vdx:StyleSheet[@ID='{0}']/vdx:Line/vdx:BeginArrowSize",sLineStyle),
						"",stylesRoot);
				}

				if ( sTemp!=null )
				{
					oArrow.ArrowBaseSize = System.Convert.ToInt16(sTemp) * 5;
				}

				// Setting ArrowHead
				sTemp = GetShapeAttr("vdx:Line/vdx:EndArrow","",shape);
				if (( sTemp == null ) && ( sLineStyle!=null))
				{
					sTemp = GetShapeAttr(String.Format("vdx:StyleSheet[@ID='{0}']/vdx:Line/vdx:EndArrow",sLineStyle),
						"",stylesRoot);
				}

				if ( sTemp!=null )
				{
					oArrow.ArrowHead = String2ArrowType( sTemp, ref crLineColor);
				}


				// Settiing ArrowHeadSize property
				sTemp = null;
				
				sTemp = GetShapeAttr("vdx:Line/vdx:EndArrowSize","",shape);
				if (( sTemp == null ) && ( sLineStyle!=null))
				{
					sTemp = GetShapeAttr(String.Format("vdx:StyleSheet[@ID='{0}']/vdx:Line/vdx:EndArrowSize",sLineStyle),
						"",stylesRoot);
				}

				if ( sTemp!=null )
				{
					oArrow.ArrowHeadSize = System.Convert.ToInt16(sTemp) * 5;
				}


			
				// If relation type is "composition" or "generalization" there is hard-coded custom logic
				if ( sNameU	== "composition")
				{
					oArrow.ArrowBase = ArrowHead.Rhombus;
					oArrow.ArrowHead =	ArrowHead.None;
				}
				else if	( sNameU ==	"generalization")
				{
					oArrow.ArrowBase =	ArrowHead.Triangle;
					oArrow.ArrowHead = ArrowHead.None;
					oArrow.FillColor =  Color.FromArgb(255,255,255);
				}

				// [???] Setting arrow's color
				oArrow.Pen = new MindFusion.FlowChartX.Pen(crLineColor,lLineWith);
				oArrow.HeadPen = oArrow.Pen;
				oArrow.Brush = new MindFusion.FlowChartX.SolidBrush(crFillColor);

				// Setting arrow's text

				sTemp = GetShapeText(shape, oArrow);
				if (sTemp != null)
				{
					Font ff = GetFont(shape);
					if ( ff == null )
						oArrow.Font = pChart.Font;
					else
						oArrow.Font = ff;

					oArrow.Text = sTemp;
					oArrow.TextColor = crTextColor;

				}
				fnShape = null;
				fnShape = GetFont(shape);

				// Raising 'ArrowImported' event
				OnArrowImported(oArrow, sShapeName);
			}
			// if the shape is GROUP
			if (((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_GROUP ) == ShapeTypeEnum.ST_GROUP)
			{
				if ((((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_ENTITY ) != ShapeTypeEnum.ST_ENTITY) &&
					(((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_OVAL_PROCESS ) != ShapeTypeEnum.ST_OVAL_PROCESS))																					 
				{
					oBox.Transparent = true;
					oBox.FillColor = Color.FromArgb(255,255,255);
				}
		
				oGroup = null;

				if ( oArrow != null )
					oGroup = pChart.CreateGroup(oArrow);
				else
					oGroup = pChart.CreateGroup(oBox);
				// Saving group's measures for future use
				lGroupY = lY;
				lGroupX = lX;
				lGroupPinY = lPinY;
				lGroupPinX = lPinX;

				// Scanning for group members and creating it using recursive call
			
				XmlNodeList grshape_list = null;
				grshape_list = shape.SelectNodes("vdx:Shapes/vdx:Shape", ns);
				foreach ( XmlNode grshape in grshape_list)
				{
					sTemp = String.Format("{0}",ShapeType);

					if (((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_ENTITY ) == ShapeTypeEnum.ST_ENTITY)
						//   if ( lShapeType & ST_ENTITY )
						sTemp = "entity";

					if (((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_ENTITY2 ) == ShapeTypeEnum.ST_ENTITY2)
						//if ( lShapeType & ST_ENTITY2 )
						sTemp = "entity 2";

					if (((ShapeTypeEnum) lShapeType & ShapeTypeEnum.ST_CONNECTOR ) == ShapeTypeEnum.ST_CONNECTOR)
						//   if ( lShapeType & ST_CONNECTOR )
						sTemp = "arrow";
					GetShapeRef(grshape,sTemp,oGroup);
				}

				// Raising 'GroupImported' event
				OnGroupImported(oGroup);
			}

			oBox = null;
			oArrow = null;

			return true;
		}

		private bool GetShapeRef(XmlNode shape, string ShapeType)
		{
			return GetShapeRef( shape, ShapeType, null);
		}

		private bool GetShapeRef(XmlNode shape)
		{
			return GetShapeRef( shape, null , null);
		}

		/// <summary>
		/// Convert shape node measures to pixels
		/// </summary>
		/// <param name="xform_shape">XML shape node reference</param>
		/// <param name="MeasureName">String name of the measure to be converted</param>
		/// <returns>Measure value in pixels</returns>
		long Measure2Pix( XmlNode xform_shape, string MeasureName )
		{
			string sTemp = "", sUnit = "";

			try
			{
				MeasureName = "vdx:" + MeasureName;
				XmlNode measure_node = xform_shape.SelectSingleNode(MeasureName, ns);

				if ( measure_node == null )
					return -1;

				if ( measure_node.Attributes["Unit"] == null )
					sUnit = sDL;
				else
					sUnit = measure_node.Attributes["Unit"].Value.ToString();

				sTemp = measure_node.InnerText.ToString();
			
			}
			catch (Exception ex)
			{
				sTemp = ex.Message;
				return -1;
			}
			return (long) Unit2Pix(sUnit,sDL,sTemp);
		}
		
		/// <summary>
		/// Converts Visio shape type into Flowchart.NET predefined shape
		/// </summary>
		/// <param name="sVisioShape">Visio shape type</param>
		/// <returns>Flowchart.NET predefinded shape reference or [null] if some error occured</returns>
		private MindFusion.FlowChartX.ShapeTemplate VisioShape2PredefinedShape( string sVisioShape )
		{
			string Result = "Rectangle";
			MindFusion.FlowChartX.ShapeTemplate templ = null;
			
			try
			{
				if (( sVisioShape == "" ) || ( sVisioShape == null))
					sVisioShape = "Process";

				sVisioShape = sVisioShape.Trim();
				sVisioShape = sVisioShape.ToLower();

				if ( sVisioShape == "process")
					Result = "Rectangle";
				else if  ( sVisioShape.IndexOf("person") >=0 )
					Result = "Actor";
				else if  ( sVisioShape.IndexOf("45 degree single") >=0 )
					Result = "Arrow3";
				else if  ( sVisioShape.IndexOf("foreign") >=0 )
					Result = "Rectangle";
				else if  ( sVisioShape.IndexOf("45 degree tail") >=0 )
					Result = "Arrow3";
				else if  ( sVisioShape.IndexOf("60 degree single") >=0 )
					Result = "Arrow3";
				else if  ( sVisioShape.IndexOf("curved arrow") >=0 )
					Result = "Arrow3";
				else if  ( sVisioShape.IndexOf("60 degree tail") >=0 )
					Result = "Arrow3";
				else if  ( sVisioShape.IndexOf("45 degree double") >=0 )
					Result = "Arrow5";
				else if  ( sVisioShape.IndexOf("60 degree double") >=0 )
					Result = "Arrow5";
				else if  ( sVisioShape.IndexOf("2-d double") >=0 )
					Result = "Arrow5";
				else if  ( sVisioShape.IndexOf("2-d single") >=0 )
					Result = "Arrow3";
				else if  ( sVisioShape.IndexOf("1-d double") >=0 )
					Result = "Arrow5";
				else if  ( sVisioShape.IndexOf("1-d open end") >=0 )
					Result = "Arrow3";
				else if  ( sVisioShape.IndexOf("1-d single") >=0 )
					Result = "Arrow3";
				else if  ( sVisioShape.IndexOf("double flexi-arrow") >=0 )
					Result = "Arrow5";
				else if  ( sVisioShape.IndexOf("flexi-arrow") >=0 )
					Result = "Arrow4";
				else if  ( sVisioShape.IndexOf("fancy arrow") >=0 )
					Result = "Arrow4";
				else if  ( sVisioShape.IndexOf("loop limit") >=0 )
					Result = "BeginLoop";
				else if  ( sVisioShape.IndexOf("object") >=0 )
					Result = "Cloud";
				else if  ( sVisioShape.IndexOf("preparation") >=0 )
					Result = "DataTransmition";
				else if  ( sVisioShape.IndexOf("stored data") >=0 )
					Result = "DDelay";
				else if  ( sVisioShape.IndexOf("diamond") >=0 )
					Result = "Decision";
				else if  ( sVisioShape.IndexOf("decision") >=0 )
					Result = "Decision";
				else if  ( sVisioShape.IndexOf("display") >=0 )
					Result = "Display";
				else if  ( sVisioShape.IndexOf("document") >=0 )
					Result = "Document";
				else if  ( sVisioShape.IndexOf("control transfer") >=0 )
					Result = "Interrupt";
				else if  ( sVisioShape.IndexOf("lined document") >=0 )
					Result = "LinedDocument";
				else if  ( sVisioShape.IndexOf("manual operation") >=0 )
					Result = "ManualOperation";
				else if  ( sVisioShape.IndexOf("extract") >=0 )
					Result = "Alternative";
				else if  ( sVisioShape.IndexOf("file") >=0 )
					Result = "File";
				else if  ( sVisioShape.IndexOf("sequential data") >=0 )
					Result = "Tape";
				else if  ( sVisioShape.IndexOf("terminator") >=0 )
					Result = "Terminator";
				else if  ( sVisioShape == "data" )
					Result = "Save";
				else if  ( sVisioShape.IndexOf("internal storage") >=0 )
					Result = "InternalStorage";
				else if  ( sVisioShape.IndexOf("triangle") >=0 )
				{
					Result = "Alternative";
					//	PinY*=2;
				}
				else if  ( sVisioShape.IndexOf("heptagon") >=0 )
				{
					Result = "Heptagon";
				}
				else if  ( sVisioShape.IndexOf("hexagon") >=0 )
				{
					Result = "Decision2";

				}
				else if  ( sVisioShape.IndexOf("pentagon") >=0 )
				{
					Result = "Pentagon";
					//	PinY*=1.5;
				}
				else if  ( sVisioShape.IndexOf("octagon") >=0 )
				{
					Result = "Octagon";
			
				}
				else if  ( sVisioShape.IndexOf("cross") >=0 )
					Result = "Cross";
				else if  ( sVisioShape == "star")
					Result = "Star";
				else if  ( sVisioShape.IndexOf("circle") >=0 )
					Result = "Ellipse";
				else if  ( sVisioShape.IndexOf("ellipse") >=0 )
					Result = "Ellipse"; 
				else if  ( sVisioShape.IndexOf("rounded rectangle") >=0 )
					Result = "RoundRect"; //
				else if  ( sVisioShape.IndexOf("predefined process") >=0 )
					Result = "Procedure";
				else if  ( sVisioShape.IndexOf("direct data") >=0 )
					Result = "DirectAccessStorage";
				else if  ( sVisioShape.IndexOf("manual input") >=0 )
					Result = "Input";
				else if  ( sVisioShape.IndexOf("card") >=0 )
					Result = "PunchedCard";
				else if  ( sVisioShape.IndexOf("paper tape") >=0 )
					Result = "Microform";
				else if  ( sVisioShape.IndexOf("on-page reference") >=0 )
					Result = "Ellipse";
				else if  ( sVisioShape.IndexOf("off-page reference") >=0 )
					Result = "OffpageReference";
				else if  ( sVisioShape.IndexOf("note") >=0 )
					Result = "File";
				else if  ( sVisioShape.IndexOf("data process") >=0 )
					Result = "Ellipse";
				else if  ( sVisioShape.IndexOf("multiple process") >=0 )
					Result = "Ellipse";
				else if  ( sVisioShape == "state" )
					Result = "Ellipse";
				else if  ( sVisioShape == "start state")
					Result = "Ellipse";
				else if  ( sVisioShape == "stop state 2")
					Result = "Merge";
				else if  ( sVisioShape == "stop state")
					Result = "Ellipse";
				else if  ( sVisioShape.IndexOf("multi state") >=0 )
					Result = "Ellipse";
				else if  ( sVisioShape == "entity")
					Result = "Rectangle";
				else if  ( sVisioShape == "oval process")
					Result = "Ellipse";
				else if  ( sVisioShape == "category")
					Result = "Ellipse";
				else if  ( sVisioShape == "entity relationship")
					Result = "Decision";
				else
					Result = "Rectangle";
		
				templ = ShapeTemplate.FromId(Result);

			}
			catch (Exception ex)
			{
				templ = null;
				Trace.WriteLine(String.Format("{0} error {1}\n","VisioShape2PredefinedShape",ex.Message));
			}
			return templ;
		

		}
		/// <summary>
		/// Creates Flowchart.NET link for the shapes selected
		/// </summary>
		/// <param name="FromID">Source shape ID</param>
		/// <param name="ToID">Destination shape ID</param>
		/// <returns>Flowchart.NET arrow reference or [null] if some error occured</returns>
		private MindFusion.FlowChartX.Arrow CreateFcxLink( string FromID, string ToID)
		{
			MindFusion.FlowChartX.Arrow Arrow = null;
			MindFusion.FlowChartX.Box BoxFrom = null;
			MindFusion.FlowChartX.Box BoxTo = null;
			MindFusion.FlowChartX.Table TableFrom = null;
			MindFusion.FlowChartX.Table TableTo = null;
			bool IsBoxFrom = true , IsBoxTo = true;

			try
			{
				BoxFrom = pChart.FindBox(FromID);
				BoxTo = pChart.FindBox(ToID);

				if ( BoxFrom == null )
				{
				
					TableFrom = pChart.FindTable(FromID);
					if ( TableFrom != null )
						IsBoxFrom = false;
					else
						IsBoxFrom = true;
				}
		

				if ( BoxTo == null )
				{
				
					TableTo = pChart.FindTable(ToID);
					if ( TableTo != null )
						IsBoxTo = false;
					else
						IsBoxTo = true;
				}
		
				
				PointF pt = new PointF(0,0);
				if (IsBoxFrom && IsBoxTo)
				{
					if ( BoxFrom == null )
						Arrow = pChart.CreateArrow(pt, BoxTo);
					else if ( BoxTo == null )
						Arrow = pChart.CreateArrow(BoxFrom, pt);
					else if ( ( BoxFrom == null ) && ( BoxTo == null ))
						Arrow = pChart.CreateArrow(pt, pt);
					else
						Arrow = pChart.CreateArrow(BoxFrom, BoxTo);
				}

				if (IsBoxFrom && !IsBoxTo)
					Arrow = pChart.CreateArrow(TableTo ,-1, BoxFrom);
		
				if (!IsBoxFrom && IsBoxTo)
					Arrow = pChart.CreateArrow(TableFrom ,-1, BoxTo);
		
				if (!IsBoxFrom && !IsBoxTo)
					Arrow = pChart.CreateArrow(TableFrom , -1,  TableTo, -1);
		
				
			}
			catch (Exception ex)
			{
				Arrow = null;
				Trace.WriteLine(String.Format("{0} error {1}\n","CreateFcxLink",ex.Message));
			}
			

			return Arrow;
		}

	
		/// <summary>
		/// Gets Visio style from its name
		/// </summary>
		/// <param name="StyleName">Style name</param>
		/// <returns>String style</returns>
		private string GetStyle( XmlNode shape,  string StyleName, ref string ExtraStyle )
		{

			string Result = "", sTemp = "", sTemp2;
			XmlNode style_node = null;

			try
			{
				if (shape.Attributes[StyleName]!=null)
				{
					Result = shape.Attributes[StyleName].Value.ToString();
				}
				else
				{
					if ( sMasterID == "")
						style_node = current_page.SelectSingleNode("vdx:PageSheet", ns);
					else
						style_node = mastersRoot.SelectSingleNode(String.Format("vdx:Master[@ID='{0}']/vdx:Shapes/vdx:Shape", sMasterID), ns);

					if ( style_node == null )
						return null;

					Result = style_node.Attributes[StyleName].Value.ToString();

				}

				
			
				sTemp = String.Format("vdx:StyleSheet[@ID='{0}']",Result);
				sTemp2 = GetShapeAttr(sTemp, StyleName, stylesRoot);

				if ( sTemp2!= null )
				{
					if (sTemp2!="0")
						ExtraStyle = sTemp2;
				}
				
				
			}
			catch (Exception ex)
			{
				Result = null;
				Trace.WriteLine(String.Format("{0} error {1}\n","GetStyle",ex.Message));
			}
			return Result;
		}

	
		/// <summary>
		/// Converts Visio's arrow type into Flowchart.NET EArrowHead enum
		/// </summary>
		/// <param name="ArrowType">String arrow style</param>
		/// <param name="lpCR">Color object reference to be returned</param>
		/// <returns>EArrowHead enumeration value</returns>
		private ArrowHead  String2ArrowType( string ArrowType, ref Color lpCR )
		{
			long lArrowType = 0;
			ArrowHead ahReturn = ArrowHead.None;

			if (( ArrowType == null ) || ( ArrowType == "") )
				return ahReturn;

			try
			{
				lArrowType = System.Convert.ToInt16(ArrowType);
			
				ahReturn = ArrowHead.Arrow;
				switch ( lArrowType)
				{
					case 0:
						ahReturn = ArrowHead.None;
						break;
					case 2:
					case 4:
					case 5:
					case 6:
					case 8:
					case 13:
					case 14:
					case 15:
					case 16:
					case 17:
					case 18:
					case 19:
					case 39:
					case 40:
					case 44:
						ahReturn = ArrowHead.Triangle;
						break;
					case 20:
					case 29:
					case 30:
					case 31:
					case 32:
					case 33:
					case 34:
					case 41:
					case 42:
						ahReturn = ArrowHead.Circle;
						break;
					case 11:
					case 21:
						ahReturn = ArrowHead.Tetragon;
						break;
					case 22:
					case 38:
						ahReturn = ArrowHead.Rhombus;
						break;
					case 23:
						ahReturn = ArrowHead.BackSlash;
						break;
					case 9:
						ahReturn = ArrowHead.Slash;
						break;
					case 43:
					case 45:
						ahReturn = ArrowHead.DoubleArrow;
						break;
	
				}

				if ((lpCR.Equals(null)) && ( lArrowType>=15))
					lpCR = Color.FromArgb(255,255,255);
			}
			catch (Exception ex)
			{
				ahReturn = ArrowHead.None;
				Trace.WriteLine(String.Format("{0} error {1}\n","String2ArrowType",ex.Message));
			}
			return ahReturn;
		}


		/// <summary>
		/// Creates Font object for selected Visio shape
		/// </summary>
		/// <param name="shape">XML node of the shape</param>
		/// <returns>Font object created or [null] if some error occured</returns>
		private Font GetFont(XmlNode shape)
		{
			string sSize, sFontID, sFontName, sStrikethru,  sStyle;
			double ftSize = 0;
			long lSize = 0, lStyle = 0;
			Font newFont = null;
			FontStyle  newFS = FontStyle.Regular;
					
			try
			{

				sFontID = GetShapeAttr("vdx:Char/vdx:Font","",shape);
				if ( sFontID == null )
					return null;

				sFontName = GetShapeAttr(String.Format("vdx:FaceName[@ID='{0}']",sFontID),"Name",facesRoot);
				if ( sFontName == null )
					return null;

				sSize = GetShapeAttr("vdx:Char/vdx:Size","",shape);
				if ( sSize == null )
					return null;

								
				ftSize = System.Convert.ToDouble(sSize.Replace(".",sSeparator));
				lSize = (long) ftSize*72;


				sStrikethru = GetShapeAttr("vdx:Char/vdx:Strikethru","",shape);
				sStyle = GetShapeAttr("vdx:Char/vdx:Style","",shape);
				lStyle = Convert.ToInt16(sStyle);

				newFS = new FontStyle();
				

				if ( ( lStyle & 1)  == 1 )
					newFS |= FontStyle.Bold;

				if (( lStyle & 2) == 2)
					newFS |= FontStyle.Italic;

				if (( lStyle & 4) == 4 )
					newFS |= FontStyle.Underline;

				if ( sStrikethru == "1" )
					newFS |= FontStyle.Strikeout;
				
				newFont = new Font( sFontName,(float) ftSize*72, newFS);
			}
			catch ( Exception ex )
			{
				newFont = null;
				Trace.WriteLine(String.Format("{0} error {1}\n","GetFont",ex.Message));
			}

			return newFont;
		}

	
		/// <summary>
		/// Get styled text from Visio shape
		/// </summary>
		/// <param name="shape">XML node of the shape</param>
		/// <param name="Arrow">Arrow object to apply text to</param>
		/// <returns>Styled text of the Visio shape</returns>
		private string GetShapeText(XmlNode shape , MindFusion.FlowChartX.Arrow Arrow )
		{
			return GetShapeText( shape ,  Arrow , null);
	
		}

		/// <summary>
		/// Get styled text from Visio shape
		/// </summary>
		/// <param name="shape">XML node of the shape</param>
		/// <param name="Box">Box object to apply text to</param>
		/// <returns>Styled text of the Visio shape</returns>
		private string GetShapeText(XmlNode shape , MindFusion.FlowChartX.Box Box )
		{
			return GetShapeText( shape ,  null , Box);
	
		}

		/// <summary>
		/// Get styled text from Visio shape
		/// </summary>
		/// <param name="shape">XML node of the shape</param>
		/// <param name="Arrow">Arrow object to apply text to</param>
		/// <param name="Box">Box object to apply text to</param>
		/// <returns>Styled text of the Visio shape</returns>
		private string GetShapeText(XmlNode shape , MindFusion.FlowChartX.Arrow Arrow , MindFusion.FlowChartX.Box Box)
		{
			string sText = "", sPrefix = "", sSuffix = "", sTextAttr = "", sWholeText ="", sStyle = "", sPara = "",
				sAlign = "", sTemp = "";
			int lStyle = 0, attr_count = 0, LineCount = 0;
			XmlNodeList attr_list = null;
			XmlNodeList text_list = null;
			XmlNode attr_node = null;
			StringAlignment TextAlignment = StringAlignment.Center;
			bool IsRight = false;
			try
			{
			
				attr_list = shape.SelectNodes("vdx:Text/vdx:cp", ns);
				text_list = shape.SelectNodes("vdx:Text/text()", ns);
			
				sPara = GetShapeAttr("vdx:Text/vdx:pp","IX", shape);

				if ( sPara!=null )
				{
					sTemp = GetShapeAttr(String.Format("vdx:Para[@IX='{0}']/vdx:HorzAlign", sPara),"", shape);
					if ( sTemp!=null )
					{
						sAlign = sTemp;
					}
				}

				LineCount = text_list.Count;

				foreach ( XmlNode text_node in text_list )
				{
					if ( text_node == null )
					{
						attr_count++;
						continue;
					}
					
					sPrefix = "";
					sText = "";
					sSuffix = "";
					sText = text_node.InnerText.ToString();
					sText = sText.Replace("«", "\"");
					sText = sText.Replace("»", "\"");
				
					attr_node = attr_list[attr_count];
					if ( attr_node != null)
					{
						
						sTextAttr = attr_node.Attributes["IX"].Value.ToString();
						sStyle = GetShapeAttr(String.Format("vdx:Char[@IX='{0}']/vdx:Style", sTextAttr),"",shape);

						if ( sStyle == null )
							lStyle = 0;
						else
							lStyle = System.Convert.ToInt16(sStyle);

						if (( lStyle & 0x1 )  == 0x1 )
						{
							sPrefix+=" <b>";
							sSuffix+=" </b> ";
							if (Box != null && Box.Font != null && Box.Font.Bold)
								Box.Font = new Font(Box.Font, Box.Font.Style & ~FontStyle.Bold);
						}

						if (( lStyle & 0x2 ) == 0x2)
						{
							sPrefix+=" <i>";
							sSuffix+=" </i> ";
							if (Box != null && Box.Font != null && Box.Font.Italic)
								Box.Font = new Font(Box.Font, Box.Font.Style & ~FontStyle.Italic);
						}

						if (( lStyle & 0x4 ) == 0x4)
						{
							sPrefix+=" <u>";
							sSuffix+=" </u> ";
							if (Box != null && Box.Font != null && Box.Font.Underline)
								Box.Font = new Font(Box.Font, Box.Font.Style & ~FontStyle.Underline);
						}

					}
					sWholeText+=sPrefix + sText + sSuffix;
					attr_count++;

				}
				
				if ( sAlign	== "0")
					TextAlignment =	StringAlignment.Far;
				else if	( sAlign ==	"1")
					TextAlignment =	StringAlignment.Center;
				else if	( sAlign ==	"2")
				{
					TextAlignment =	StringAlignment.Far;
					IsRight = true;
				}
				else
					TextAlignment =	StringAlignment.Center;

				if	( LineCount	>1 )
					TextAlignment =	StringAlignment.Center;
				
				
				if ( sAlign!=null )
				{
					if ( Box!=null )
					{
						if (IsRight)
						{
							Box.TextFormat.FormatFlags = StringFormatFlags.DirectionRightToLeft;
						}
						Box.TextFormat.Alignment = TextAlignment;
					}		
				}

			}
			catch ( Exception ex)
			{
				sWholeText = null;
				Trace.WriteLine(String.Format("{0} error {1}\n","GetShapeText",ex.Message));

			}

			return sWholeText;
		}

		/// <summary>
		/// Gets color value of the Visio shape selected
		/// </summary>
		/// <param name="shape">XML node of the shape</param>
		/// <param name="StyleID">String ID of the style to be used</param>
		/// <param name="ColorName">String name of the color to be converted</param>
		/// <returns>Color object</returns>
		private Color GetColor(XmlNode shape, string StyleID,  string ColorName)
		{
			return GetColor( shape,  StyleID,   ColorName,  null, null, null);
		}

		/// <summary>
		/// Gets color value of the Visio shape selected
		/// </summary>
		/// <param name="shape">XML node of the shape</param>
		/// <param name="StyleID">String ID of the style to be used</param>
		/// <param name="ColorName">String name of the color to be converted</param>
		/// <param name="TransparencyName">String name of the transparency to be combined with color</param>
		/// <returns>Color object</returns>
		private Color GetColor(XmlNode shape, string StyleID,  string ColorName, string TransparencyName)
		{
			return GetColor( shape,  StyleID,   ColorName,  TransparencyName, null, null);
		}

		/// <summary>
		/// Gets color value of the Visio shape selected
		/// </summary>
		/// <param name="shape">XML node of the shape</param>
		/// <param name="StyleID">String ID of the style to be used</param>
		/// <param name="ColorName">String name of the color to be converted</param>
		/// <param name="TransparencyName">String name of the transparency to be combined with color</param>
		/// <param name="ColorValue">#RGB value of the color</param>
		/// <returns>Color object</returns>
		private Color GetColor(XmlNode shape, string StyleID,  string ColorName, string TransparencyName, string ColorValue , string altStyleID)
		{
			string sColor = "" , sTemp = "", sTransparency = "" ;
			Color crColor = Color.Empty;
			int iR =0, iG = 0, iB = 0, iA = 255;

			try
			{
		
				if ( ColorValue == null )
					sColor = GetShapeAttr(ColorName, "", shape);
				else
					sColor = ColorValue;

				if ( sColor == null)
				{
					sTemp = String.Format("vdx:StyleSheet[@ID='{0}']/{1}",StyleID, ColorName);
					sColor = GetShapeAttr(sTemp, "", stylesRoot);

				}

				if ( sColor == null )
				{
					sTemp = String.Format("vdx:StyleSheet[@ID='{0}']/{1}",altStyleID, ColorName);
					sColor = GetShapeAttr(sTemp, "", stylesRoot);

				}

				if ( sColor == null )
					return crColor;

				if ( sColor[0] != '#')
				{		

					sTemp = String.Format("vdx:ColorEntry[@IX='{0}']",sColor);
					sColor = GetShapeAttr(sTemp, "RGB", colorsRoot);
					
				}


				if ( sColor.Length <=6 )
					return crColor;

				if (  TransparencyName !=null )
				{
					sTransparency = GetShapeAttr(TransparencyName, "", shape);
					if ( sTransparency != null )
					{
						sTransparency = sTransparency.Replace(".",",");
						double dA = System.Convert.ToDouble(sTransparency);
						iA = (int) (( 1 - dA ) * 255);
					}
				
				}
			
			

				iR = System.Convert.ToInt16(sColor.Substring(1,2), 16);
				iG = System.Convert.ToInt16(sColor.Substring(3,2), 16);
				iB = System.Convert.ToInt16(sColor.Substring(5,2), 16);
				crColor = new Color();
				crColor = Color.Empty;
				crColor = Color.FromArgb(iA, iR, iG, iB);
				

			}
			catch ( Exception ex )
			{
				crColor = Color.Empty;
				Trace.WriteLine(String.Format("{0} error {1}\n","GetColor",ex.Message));
			}

			
			return crColor;

		}

		/// <summary>
		/// Process foreign nodes ( ActiveX objects, pictures etc. ) of the Visio shapes
		/// </summary>
		/// <param name="shape">XML node of the shape</param>
		/// <returns>Returns [true] if succesfull [false] otherwise</returns>
		private bool ProcessForeign ( XmlNode shape )
		{

			bool result = false;
			string sFT = "", sB64 = "";
			System.IO.MemoryStream st = null;
			System.Drawing.Image img = null;
			int size = 0;
			byte[] baImage;

			try
			{
				
				sFT = GetShapeAttr("vdx:ForeignData", "ForeignType",shape);
				if ( sFT == null )
					return false;

				sFT = sFT.ToLower();
				if ( sFT == "bitmap")
				{
					sB64 = GetShapeAttr("vdx:ForeignData", "",shape);
					if ( sB64 == null )
						return false;

					sB64 = sB64.Trim();
					baImage = System.Convert.FromBase64String(sB64);
					size = baImage.Length;
					if ( size ==0 )
						return false;
					
					st = new System.IO.MemoryStream(baImage);

					if ( st == null )
						return false;

					img = System.Drawing.Image.FromStream(st);
					if ( img == null )
						return false;

					if ( oBox == null )
						return false;

					oBox.Picture = img;
			

				}
				else if ( sFT == "object")
				{
					// ActiveX & .NET controls are ignored as per specification
				}
				
				result = true;
			}
			catch ( Exception ex )
			{
				result = false;
				Trace.WriteLine(String.Format("{0} error {1}\n","ProcessForeign",ex.Message));
			}
			
			return result;
		}

		/// <summary>
		/// Converts string PenStyle into DashStyle enum
		/// </summary>
		/// <param name="PenStyle">String PenStyle</param>
		/// <returns>DashStyle enum value</returns>
		private DashStyle   String2DashStyle(string PenStyle)
		{
					 
			long lPenStyle = 0;
			DashStyle   psReturn = DashStyle.Solid;

			if (( PenStyle == null ) || (  PenStyle == ""))
				return psReturn;

			try
			{
				lPenStyle = Convert.ToInt16(PenStyle);

				switch (lPenStyle)
				{
					case 1:
						psReturn = DashStyle.Solid;
						break;
					case 2:
					case 7:
					case 8:
					case 9:
					case 16:
						psReturn = DashStyle.Dash;
						break;
					case 3:
					case 10:
					case 17:
					case 23:
						psReturn = DashStyle.Dot;
						break;
					case 4:
					case 6:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 18:
					case 19:
					case 20:
					case 21:
					case 22:
						psReturn = DashStyle.DashDot;
						break;
					case 5:
						psReturn = DashStyle.DashDotDot;
						break;
				}
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","String2DashStyle",ex.Message));
			}
			

			return psReturn;
		}

		/// <summary>
		/// Specifies whether Visio DB and UML entity shapes should be imported
		/// as FlowChart.NET tables or as box groups.
		/// </summary>
		public bool ImportEntitiesAsTables
		{
			get
			{
				return bImportEntitiesAsTables;
			}
			set
			{
				bImportEntitiesAsTables = value;
			}
		}

		/// <summary>
		/// Specifies how many pages to import in a row by offsetting them 
		/// horizontally in FlowChart.NET before starting a new row of pages.
		/// </summary>
		public long PagesPerRow
		{
			get
			{
				return lPagesPerRow;
			}
			set
			{
				lPagesPerRow = value;
			}
		}
	}
}