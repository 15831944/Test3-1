#define _POST_FIX_3 // arrow head styles processing extended

using System;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Xml;
using System.Xml.XPath;
using MindFusion.FlowChartX;


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
	/// Exports FlowChart.NET diagrams as Visio 2003 XML Drawing files.
	/// </summary>
	public class VisioExporter
	{
		[DllImport("gdi32.dll", EntryPoint="GetDeviceCaps", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)] 
		private static extern int GetDeviceCaps(IntPtr hdc, int nIndex); 


		[DllImport("User32.DLL", EntryPoint="GetActiveWindow", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)]
		private  static extern IntPtr GetActiveWindow ( ); 
		
		[DllImport("User32.DLL",EntryPoint="GetDC", 
			 SetLastError=true, 
			 ExactSpelling=true, CallingConvention=CallingConvention.StdCall)]
		private extern static System.IntPtr GetDC(System.IntPtr hWnd); 

		private long PixPerInchFirst = 0,  PixPerInch = 0, m_InnerID = 0;
		private double  lDocX = 0, lDocY =0, fDocX = 0, fDocY = 0, DocTop = 0, DocLeft = 0, DocBottom = 0, DocRight = 0;
		private bool m_CreateVisioGroups = false;
		private bool m_ExportTablesAsGroups = false;
		private MindFusion.FlowChartX.FlowChart pChart;
		private XmlDocument doc;
		private XmlNamespaceManager ns;
		private XmlNode root,shapeLast,groupLast, shapeNew , facesRoot, connectRoot, shapeRoot, shapeTemplate;
		private string  sSeparator = NumberFormatInfo.CurrentInfo.NumberDecimalSeparator;
		public RectangleF basedRect = RectangleF.Empty;

		/// <summary>
		/// Initializes a new instance of the VisioExporter class
		/// </summary>
		public VisioExporter()
		{
		}

		/// <summary>
		/// Specifies whether FlowChart.NET groups should be exported
		/// as Visio groups when possible.
		/// </summary>
		public bool CreateVisioGroups
		{
			get
			{
				return m_CreateVisioGroups;
			}
			set
			{
				m_CreateVisioGroups = value;
			}
		}

		/// <summary>
		/// Specifies whether FlowChart.NET groups should be exported
		/// as Visio groups when possible.
		/// </summary>
		public bool ExportTablesAsGroups
		{
			get
			{
				return m_ExportTablesAsGroups;
			}
			set
			{
				m_ExportTablesAsGroups = value;
			}
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
			m_InnerID = pChart.Objects.Count;
		}

		private int getID(ChartObject item)
		{
			return item.ZIndex + 1;
		}

		/// <summary>
		/// Removing XML child node from VDX structure
		/// </summary>
		/// <param name="Node">XML node reference</param>
		/// <param name="ChildName">Name of the child to be removed</param>
		/// <param name="IsNode">Bool flag: If [true] child is a node , if [false] the child is an attribute</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private bool RemoveNode(XmlNode Node, string ChildName , bool IsNode)
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
		/// Returns Visio VDX arrow type derived from FlowChart.NET EArrowHead enum
		/// </summary>
		/// <param name="eaType">EArrowHead  enum value</param>
		/// <returns>String Visio arrow type</returns>
		private string  ArrowType2String(ArrowHead eaType)
		{
			string sTemp = "0";

			try
			{
				switch ( eaType)
				{
#if _POST_FIX_3
					case ArrowHead.Arrow:
						sTemp = "1";
						break;
					case ArrowHead.BowArrow:
						sTemp = "6";
						break;
					case ArrowHead.PointerArrow:
						sTemp = "6";
						break;
					case ArrowHead.DoubleArrow:
						sTemp = "39";
						break;
					case ArrowHead.Pentagon:
						sTemp = "4";
						break;
					case ArrowHead.Reversed:
						sTemp = "27";
						break;
					case ArrowHead.RevWithLine:
						sTemp = "28";
						break;
					case ArrowHead.RevWithCirc:
						sTemp = "29";
						break;
					case ArrowHead.RevTriangle:
						sTemp = "27";
						break;
					case  ArrowHead.Circle:
						sTemp="42";
						break;
					case ArrowHead.BackSlash:
						sTemp = "9";
						break;
					case ArrowHead.Slash:
						sTemp = "23";
						break;
					case ArrowHead.Quill:
						sTemp = "2";
						break;
#else
					case ArrowHead.BackSlash:
						sTemp = "23";
						break;
					case  ArrowHead.ahCircle:
						sTemp="20";
						break;
					case ArrowHead.hDoubleArrow:
						sTemp = "43";
						break;
					case ArrowHead.Slash:
						sTemp = "9";
						break;
			
#endif
					case ArrowHead.None:
						sTemp = "0";
						break;
					case ArrowHead.Triangle:
						sTemp="2";
						break;
					case ArrowHead.Tetragon:
						sTemp = "11";
						break;
					case ArrowHead.Rhombus:
						sTemp = "22";
						break;
			
					default:
						sTemp = "0";
						break;

				}
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","ArrowType2String",ex.Message));
				sTemp = "0";
			}

			return sTemp;
		}

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
					sResult = "#FFFFFF";
				else
					sResult = String.Format("#{0:X2}{1:X2}{2:X2}", cr.R, cr.G, cr.B);
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SvgManager.Color2Str",ex.Message));
				
			}

			if (( sResult == "#FFFFFF" ) && ( cr.A == 0))
				sResult = "#FFFFFF";
			return sResult;
		}

		private bool SetText( XmlNode Node, string Text , bool IsStyled , Font NodeFont)
		{
			return SetText(  Node,  Text ,  IsStyled ,  NodeFont, StringAlignment.Center);
		}

		/// <summary>
		/// Used for setting text from the Flowchart.Net node into VDX [Text] node using font specified
		/// </summary>
		/// <param name="Node">VDX node reference</param>
		/// <param name="Text">string contining text to be set</param>
		/// <param name="IsStyled">If [true] text is Styled otherwise not</param>
		/// <param name="NodeFont">Font object to be used for text settings</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private bool SetText( XmlNode Node, string Text , bool IsStyled , Font NodeFont, StringAlignment sa)
		{
			bool bOk = false;
			XmlNode node_temp = null, node_char = null, node_char_first = null, node_text = null, node_text_first = null,
				node_text_root = null, node_para = null, node_temp2 = null, node_temp1 = null;
			MindFusion.Diagramming.Export.StyledText Parser = null;
			int text_mask = 0, node_count =0;
			string text_word = null, sTemp = null;
			
			try
			{
				if ( Node == null )
					return false;

				if ( Text == null )
					Text = "";

				node_temp =	Node.SelectSingleNode("vdx:Text/text()", ns);

				if ( node_temp == null )
					return false;

				node_temp.InnerText = Text;
				

				// If the text is not styled
				if (!IsStyled)
				{
					bOk = true;	
				}
				else
				{
					// If the text is styled
					IntPtr hWnd = GetActiveWindow();
					Parser = new MindFusion.Diagramming.Export.StyledText();
					Parser.Setup(Text, System.Drawing.Graphics.FromHwnd(hWnd) ,NodeFont);

					// Hashing some VDX node references
					node_char_first = Node.SelectSingleNode("vdx:Char", ns);

					if ( node_char_first == null )
						return false;

					node_text_first = Node.SelectSingleNode("vdx:Text/vdx:cp", ns);

					if ( node_text_first == null )
						return false;

					node_text_root = Node.SelectSingleNode("vdx:Text", ns);

					if ( node_text_root == null )
						return false;

					// (1)  Setting paragraph formatting
					// Parsing styled text using [Parser] object
					node_count = 1;
					foreach (MindFusion.Diagramming.Export.StyledText.StyledWord new_word in Parser.Words )
					{
						if ( new_word == null )
							continue;

						text_word = new_word.Value.ToString();
						
						if (!new_word.IsLineBreak)
							text_word = text_word.Trim();

						if ( text_word == "" )
							continue;

						// Creating text mask according to styled word format
						text_mask = (int) new_word.Format;

						if ( node_char == null )
							node_char  = Node.InsertAfter(node_char_first.Clone(), node_char_first);
						else
							node_char  = Node.InsertAfter(node_char_first.Clone(), node_char);

						SetShapeAttr("","IX",node_char,String.Format("{0}",node_count));
						SetShapeAttr("vdx:FontScale","",node_char,"1");
						node_char.SelectSingleNode("vdx:FontScale", ns).Attributes.RemoveNamedItem("F");
						
						if (( text_mask & 32 ) == 32)
						{
							text_mask-=32;
						}

						if (( text_mask & 16 ) == 16)
						{
							sTemp = "1";
							text_mask-=16;
						}
						else
							if (( text_mask & 8 ) == 8)
						{
							sTemp = "2";
							text_mask-=8;
						}
						else
							sTemp = "0";
						
						// Inserting new text node
						SetShapeAttr("vdx:Pos","",node_char,sTemp);
						node_char.SelectSingleNode("vdx:Pos", ns).Attributes.RemoveNamedItem("F");
						SetShapeAttr("vdx:Style","",node_char,String.Format("{0}",text_mask));
						node_char.SelectSingleNode("vdx:Style", ns).Attributes.RemoveNamedItem("F");
						SetShapeAttr("vdx:Color","",node_char,String.Format("{0}",Color2Str(new_word.Color)));
						node_char.SelectSingleNode("vdx:Color", ns).Attributes.RemoveNamedItem("F");
						//SetShapeColor(cellNode, "vdx:Char/vdx:Color", new_word.Color);
						

						
						if ( node_count == 1 )
						{
							// (1) Setting paragraph formatting
							node_para = Node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Para","http://schemas.microsoft.com/visio/2003/core"), ( node_para == null ) ? node_char: node_para);
						
							if ( node_para!=null )
							{
								SetShapeAttr("","IX",node_para,String.Format("{0}",node_count));
								node_temp2 = node_para.InsertAfter(doc.CreateNode(XmlNodeType.Element,"HorzAlign","http://schemas.microsoft.com/visio/2003/core"), node_para.LastChild);

								if ( node_temp2!=null )
								{
									node_temp2.InnerText = Align2Str(sa);
													
								}
							}
						}
						// (1)  Setting paragraph formatting
						

						if ( node_text == null )
						{
							node_text_root.RemoveChild(node_text_first.NextSibling);
							node_text = node_text_root.InsertAfter(node_text_first.Clone(), node_text_first);
							
						}
						else
						{
							node_text = node_text_root.InsertAfter(node_text_first.Clone(), node_text);
						}
						
						if ( node_count ==1 )
						{
							node_temp1 = node_text_root.InsertAfter(doc.CreateNode(XmlNodeType.Element,"pp","http://schemas.microsoft.com/visio/2003/core"), node_text);
							if ( node_temp1!=null )
								SetShapeAttr("","IX",node_temp1,String.Format("{0}",node_count));
						}

						SetShapeAttr("","IX",node_text,String.Format("{0}",node_count));
						
						node_text = node_text_root.InsertAfter(doc.CreateNode(XmlNodeType.Text,"", "http://schemas.microsoft.com/visio/2003/core"), ( node_count == 1) ? node_temp1: node_text);
						text_word+=" ";
						node_text.InnerText = text_word;

						//Trace.WriteLine(String.Format("{0}:\t{1}\n",node_count ,text_word));
						node_count++;
						
					}

					node_text_root.RemoveChild(node_text_first);
					Node.RemoveChild(node_char_first);
					
															
				}
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SetText",ex.Message));
				bOk = false;
			}

			return bOk;

		}

		/// <summary>
		/// Generate GUID to be used in Visio's [UniqueID] attribute
		/// </summary>
		/// <returns>String GUID</returns>
		private string GetGUID()
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
		/// Converts [DashStyle] to corresponded Visio line style
		/// </summary>
		/// <param name="dStyle">[DashStyle] to be converted</param>
		/// <returns>VDX line style as string</returns>
		string DashStyle2String(DashStyle dStyle)
		{
			string sRes = "0";

			try
			{
				switch (  dStyle	)
				{
					case DashStyle.Solid:
						sRes =	"1";
						break;
					case DashStyle.Dash:
						sRes =	"2";
						break;
					case DashStyle.Dot:
						sRes =	"3";
						break;
					case DashStyle.DashDot:
						sRes =	"4";
						break;
					case DashStyle.DashDotDot:
						sRes =	"5";
						break;
					default:
						sRes = "0";
						break;

				};
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","DashStyle2String",ex.Message));
				sRes = "0";
			}
			return sRes;
		}

		/// <summary>
		/// Methods is intended for setting VDX node font parameters
		/// </summary>
		/// <param name="Node">VDX node reference</param>
		/// <param name="Font">Font object reference</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private bool SetShapeFont(XmlNode Node, System.Drawing.Font Font)
		{
			XmlNode node_temp = null, new_font = null, last_font = null;
			bool bOk = false;
			string sTemp = "", sFontID = "1",sFontName="", sLastID = "1";
			long lCharStyle = 0, lLastID = 0;
			double dSize = 0;

			try
			{

				if ( Node == null )
					return false;

				if ( Font == null)
					return false;


				
				//  Setting font's size
				GraphicsUnit FontUnit = Font.Unit;
				if  ( FontUnit == GraphicsUnit.World )
					FontUnit = pChart.MeasureUnit;

				switch(FontUnit)
				{
					case GraphicsUnit.Display:
						dSize = Font.Size/75;
						break;
					case GraphicsUnit.Document:
						dSize = Font.Size/300;
						break;
					case GraphicsUnit.Inch:
						dSize = Font.Size;
						break;
					case GraphicsUnit.Millimeter:
						dSize = Font.Size/25.1;
						break;
					case GraphicsUnit.Pixel:
						dSize = Font.Size/PixPerInch;
						break;
					case GraphicsUnit.Point:
						dSize = Font.Size/75;
						break;
					
				};

				sTemp = String.Format("{0}", dSize);
				sTemp = sTemp.Replace( sSeparator,".");
				SetShapeAttr("vdx:Char/vdx:Size","", Node, sTemp);
						
				// Looking for the font in the VDX [FaceNames] root
				sFontName = Font.Name;
				sTemp = String.Format("//vdx:FaceNames/vdx:FaceName[@Name='{0}']", sFontName);
				node_temp = Node.SelectSingleNode(sTemp, ns);

				if ( node_temp != null)
				{
					// If font name was found set its ID
					sFontID = node_temp.Attributes["ID"].Value.ToString();
				}
				else
				{
					// If font name wasn't found inserting new FaceName into [FaceNames] root
					foreach ( XmlNode current_font in root.SelectNodes("//vdx:FaceNames/vdx:FaceName", ns))
					{
						if ( current_font == null )
							continue;

						sLastID = current_font.Attributes["ID"].Value.ToString();
						if ( Convert.ToInt16(sLastID)>=lLastID)
						{
							lLastID = Convert.ToInt16(sLastID);
							last_font = current_font;
						}
					}

					new_font = facesRoot.InsertAfter(last_font.Clone(), last_font);
					if ( new_font == null )
					{
						sFontID = "1";
					}
					else
					{
					
						lLastID++;
						sFontID = String.Format("{0}",lLastID);
						new_font.Attributes["ID"].Value = sFontID;
						new_font.Attributes["Name"].Value = sFontName;
						
					}
										
				}

				SetShapeAttr("vdx:Char/vdx:Font", "", Node, sFontID);
									
					
				if ( Font.Bold )
					lCharStyle |= 1;


				if ( Font.Italic )
					lCharStyle |= 2;

				if ( Font.Underline )
					lCharStyle |= 4;

				// Setting font style
				SetShapeAttr("vdx:Char/vdx:Style", "", Node, String.Format("{0}", lCharStyle));
													
				bOk  = true;
			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SetShapeFont",ex.Message));
				bOk = false;
			}

			return bOk;
		}

		/// <summary>
		/// Method is for setting VDX node colors
		/// </summary>
		/// <param name="Node">VDX node reference</param>
		/// <param name="ColorName">String name of the color to set</param>
		/// <param name="crColor">[Color] object reference</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private bool SetShapeColor(XmlNode Node, string ColorName, Color crColor)
		{
			XmlNode	node_temp = null, node_temp2 = null;
			string	sTemp = "", sTemp2 = "";
			bool bOk = false;

			try
			{

				if ( Node == null )
					return false;

				node_temp =	Node.SelectSingleNode(ColorName, ns);

				if ( node_temp == null)
					return false;

				// Converting [Color] value to string #RGB
				sTemp = String.Format("{0:X2}{1:X2}{2:X2}", crColor.R, crColor.G, crColor.B);
				sTemp = "#" + sTemp;

				// Looking for desired color in [Colors] root
				sTemp2 = String.Format("//vdx:Colors/vdx:ColorEntry[@RGB='{0}']",sTemp);
				node_temp2 = root.SelectSingleNode(sTemp2, ns);

				// If color is found using its ID
				if ( node_temp2!= null)
					sTemp = node_temp2.Attributes["IX"].Value.ToString();

				// Otherwise using #RGB value itself
				node_temp.InnerText = sTemp;
				node_temp.Attributes.RemoveNamedItem("F");
				bOk  = true;

			}
			catch (Exception ex)
			{

				Trace.WriteLine(String.Format("{0} error {1}\n","SetShapeColor",ex.Message));
				bOk = false;
			}

			return bOk;
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

		/// Setting VDX node attribute or text
		/// </summary>
		/// <param name="AttrPath">String XPath expression to found child node of the [shape]</param>
		/// <param name="AttrName">String Name of the attribute or "" for [Text]</param>
		/// <param name="shape">Parent node reference or [null] if root node is parent</param>
		/// <param name="newValue">String value to be set</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		
		private bool SetShapeAttr(string AttrPath, string AttrName,  XmlNode shape , string newValue)
		{
			bool bOk = false;

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
					return bOk;

				if ( AttrName == "" )
					temp_node.InnerText = newValue;
				else
				{
					if ( temp_node.Attributes[AttrName] == null )
					{
						temp_node.Attributes.Append(doc.CreateAttribute(AttrName));
					}
					
					temp_node.Attributes[AttrName].Value = newValue;
					bOk = true;
					
				}
				

			}
			catch (Exception ex)
			{
				bOk = false;
				Trace.WriteLine(String.Format("{0} error {1}\n","GetShapeAttr",ex.Message));
			}

			return bOk;
		}

		/// <summary>
		/// Exports the FlowChart.NET diagram to a Visio VDX file.
		/// </summary>
		/// <param name="ChartRef">A FlowChart instance to be exported</param>
		/// <param name="vdxPathName">The path and name of the target VDX file</param>
		public void Export(FlowChart flowChart, string vdxPathName)
		{
			string sResult = "", sTemp = "";

			try
			{
				// Initializing variables
				pChart = flowChart;
				
				if ( pChart == null )
					throw new Exception("Empty chart reference was passed");

				if ( vdxPathName == null )
					throw new Exception("Empty VDX path was passed");

				// Getting screen resolution 'Pix-Per-Inch' parameter 
				IntPtr hWnd = GetActiveWindow(); 
				System.IntPtr hDC = GetDC(hWnd);
				PixPerInch = GetDeviceCaps(hDC, 88 );
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

				fDocX =	lDocX/PixPerInch;
				fDocY =	lDocY/PixPerInch;

				
				// Loading XML template using namespace 
				doc = new XmlDocument();
				string sXml = null, sTempPath = "VisioExport.vxt";

				// search for the template file in the exe or dll folder
				string asmLoc = typeof(VisioExporter).Assembly.Location;
				string prcLoc = Assembly.GetEntryAssembly().Location;
				string[] searchPath = new string[] { 
													   asmLoc.Substring(0, asmLoc.LastIndexOf('\\') + 1),
													   prcLoc.Substring(0, prcLoc.LastIndexOf('\\') + 1),
													   Directory.GetCurrentDirectory()
												   };
				for (int i = 0; i < 3; ++i)
				{
					if (searchPath[i][searchPath[i].Length - 1] != '\\')
						searchPath[i] += "\\";
					if (File.Exists(searchPath[i] + sTempPath))
					{
						sTempPath = searchPath[i] + sTempPath;
						break;
					}
				}

				// load the template XML document
				System.IO.StreamReader sr = new System.IO.StreamReader(sTempPath);
				if ( sr == null ) 
				{
					doc.Load(sTempPath);
				}
				else
				{
				
					sXml = sr.ReadToEnd();
					if ( sXml != null )
					{
						doc.LoadXml(sXml);
					}
					sr.Close();
				
				}

				
				
				ns =new XmlNamespaceManager(doc.NameTable);
				root = doc.DocumentElement;
				ns.AddNamespace("vdx", "http://schemas.microsoft.com/visio/2003/core");

				// Preparing shape's template
				connectRoot = root.SelectSingleNode("//vdx:Pages/vdx:Page[@ID='0']/vdx:Connects", ns);

				if ( connectRoot == null )
					throw new Exception("Can't load connect root");

				// Hashing some VDX roots
				shapeRoot = root.SelectSingleNode("//vdx:Pages/vdx:Page[@ID='0']/vdx:Shapes", ns);

				if ( shapeRoot == null )
					throw new Exception("Can't load shape root");

			
				shapeTemplate = root.SelectSingleNode("//vdx:Pages/vdx:Page[@ID='0']/vdx:Shapes/vdx:Shape[@ID='0']", ns);

				if ( shapeTemplate == null )
					throw new Exception("Can't load shape template");

				facesRoot = root.SelectSingleNode("//vdx:FaceNames", ns);

				if ( facesRoot == null )
					throw new Exception("Can't load font's root");
				
				shapeLast = shapeTemplate;

				
				// Seting Visio	page dimensions	in INCHES
				SetShapeDim(root, "//vdx:Pages/vdx:Page[@ID='0']/vdx:PageSheet/vdx:PageProps/vdx:PageWidth", lDocX,  true);
				SetShapeDim(root, "//vdx:Pages/vdx:Page[@ID='0']/vdx:PageSheet/vdx:PageProps/vdx:PageHeight", lDocY,  true);
			
				// Setting date & timestamps
				DateTime dt = DateTime.Now;
				sTemp = dt.ToString("yyyy-MM-ddThh:mm:ss");
				
				SetShapeAttr("//vdx:DocumentProperties/vdx:TimeCreated","", root,sTemp);
				SetShapeAttr("//vdx:DocumentProperties/vdx:TimeSaved","", root,sTemp);
				SetShapeAttr("//vdx:DocumentProperties/vdx:TimeEdited","", root,sTemp);
				SetShapeAttr("//vdx:DocumentProperties/vdx:TimePrinted","", root,sTemp);
				
				// Preparing item's enumeration
				
				ResetID();

				// process tables
				foreach (MindFusion.FlowChartX.Table table in pChart.Tables)
				{

					if (table == null)
						continue;

					AddTable(table);
				}

				// process boxes
				foreach (MindFusion.FlowChartX.Box box in pChart.Boxes)
				{
					if (box == null)
						continue;

					if ((box.MasterGroup != null) && (m_CreateVisioGroups))
						continue;

					if (box.SubordinateGroup == null)
						AddBox(box, null, false);
				}

				// Processing items [ Groups ]

				foreach ( MindFusion.FlowChartX.Group Group in pChart.Groups)
				{

					if ( Group == null )
						continue;

					if (!m_CreateVisioGroups )
					{
						if ( Group.MainObject.SubordinateGroup!=null )
						{
							if ( Group.MainObject is Box)
								AddBox(Group.MainObject as Box, null, false);
						}
						continue;
					}
					
					AddBox((MindFusion.FlowChartX.Box) Group.MainObject, null, true);
					foreach ( MindFusion.FlowChartX.Box BoxOfGroup in Group.AttachedObjects )
					{
						if ( BoxOfGroup == null )
							continue;

						AddBox(BoxOfGroup, Group, false);
					}
				}

				// Processing items [ Arrows ]

				foreach (MindFusion.FlowChartX.Arrow arrow in pChart.Arrows)
				{
					if (arrow == null)
						continue;
					
					AddArrow(arrow);
				}

				// Removing temporary shape

				CleanUp();
							
				// Saving generated VDX content into file
				doc.Save(vdxPathName);

				

			}
			catch ( Exception ex )
			{
				sResult = String.Format("Error occured when exporting: {0}",ex.Message);
				Trace.WriteLine(String.Format("{0} error {1}\n","ExportVDX",ex.Message));
			}
		}

		/// <summary>
		/// Method is intended for setting shape's dimensions
		/// </summary>
		/// <param name="Node">VDX node reference</param>
		/// <param name="DimName">Dimension name</param>
		/// <param name="DimValue">Dimension value</param>
		/// <param name="SupressFormula">Bool flag: if [true] [Formula] attribute of the [Node] will be overlapped with 'No formula'</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private bool SetShapeDim( XmlNode Node, string DimName,	double DimValue, bool SupressFormula)
		{
			XmlNode node_temp;
			string	sTemp = null;
			double fIN =	0;
			bool bOk = false;
			try
			{

				if ( Node == null )
					return false;

				node_temp = Node.SelectSingleNode(DimName, ns);

				if (node_temp == null)
					return false;


				switch(pChart.MeasureUnit)
				{
					case GraphicsUnit.Display:
						fIN = DimValue/75;
						break;
					case GraphicsUnit.Document:
						fIN = DimValue/300;
						break;
					case GraphicsUnit.Inch:
						fIN = DimValue;
						break;
					case GraphicsUnit.Millimeter:
						fIN = DimValue/25.1;
						break;
					case GraphicsUnit.Pixel:
						fIN = DimValue/PixPerInch;
						break;
					case GraphicsUnit.Point:
						fIN = DimValue/75;
						break;
					
				};

			
				sTemp = String.Format("{0}", fIN);
				sTemp = sTemp.Replace(sSeparator , ".");
				node_temp.InnerText = sTemp;
				SetShapeAttr("","Unit",node_temp, "IN");
			
		
				SupressFormula = true;
				if ( (node_temp.Attributes["F"]!=null) && (SupressFormula))
				{
					node_temp.Attributes["F"].Value =  "No Formula";
										
				}

				bOk = true;
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","SetShapeDim",ex.Message));
				bOk = false;
			}
			return bOk;
		}

		private string SetMasterID(XmlNode shape, MindFusion.FlowChartX.Box newBox)
		{
			return SetMasterID(shape,newBox.Shape,newBox.Style , getID(newBox));
		}

		/// <summary>
		/// Intended to set 'Master' attribute of the [Shape]
		/// </summary>
		/// <param name="shape">VDX node name</param>
		/// <param name="newBox">FlowChart/NET box reference</param>
		/// <returns>String FLowchart.NET predefinde shape's name</returns>
		private string SetMasterID(XmlNode shape, 	MindFusion.FlowChartX.ShapeTemplate templ,
			BoxStyle StyleOfShape, long IdOfShape)
		{

			string sFXShape = null, sTemp = null, sVisioShape = null, sMasterID = null;
			XmlNode node_temp = null;
			
			try
			{
				if ( shape == null )
					return null;

				if ( templ == null )
					sFXShape = "Rectangle";
				else
					sFXShape = templ.ToString();

				switch ( StyleOfShape )
				{
					case BoxStyle.Rectangle:
						sFXShape = "Rectangle";
						break;
					case BoxStyle.Ellipse:
						sFXShape = "Ellipse";
						break;
					case BoxStyle.RoundedRectangle:
						sFXShape = "RoundRect";
						break;
					case BoxStyle.Rhombus:
						sFXShape = "Decision";
						break;
					case BoxStyle.Delay:
						sFXShape = "Delay";
						break;
				};
		
				sTemp = String.Format("//vdx:Masters/vdx:Master[@Name='{0}']", sFXShape);
				node_temp =	root.SelectSingleNode(sTemp, ns);

				if (node_temp != null)
				{
					sVisioShape	 = node_temp.Attributes["NameU"].Value.ToString();
					sMasterID  = node_temp.Attributes["ID"].Value.ToString();
				}
				else
				{
					sVisioShape	= "Process";
					sMasterID =	"0";
				}
			
				SetShapeAttr("","Master",shape,sMasterID);
				//??? shape.Attributes["Master"].Value =  sMasterID;
				sTemp = String.Format("{0}.{1}", sVisioShape, IdOfShape);
				SetShapeAttr("","NameU",shape,sMasterID);
				SetShapeAttr("","Name",shape,sMasterID);
				//shape.Attributes["NameU"].Value = sTemp;
				//shape.Attributes["Name"].Value = sTemp;
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","SetMasterID",ex.Message));
				sVisioShape = null;
			}
			
			return sVisioShape;
		}

		/// <summary>
		/// Converts image to base64 encoded string
		/// </summary>
		/// <param name="img">[Image] object reference</param>
		/// <returns>Base64 encoded string BMP content of the image</returns>
		private string Image2String ( System.Drawing.Image img)
		{
			string sImg = null, sTempFile = "$$$temp$$$.bmp";
			int BytesRead = 0;
			byte[] baImage = null;
			if ( img == null )
				return null;

			try
			{
				img.Save(sTempFile, System.Drawing.Imaging.ImageFormat.Bmp);
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
		/// Add Flowchart.Net arrow as a XML node into VDX
		/// </summary>
		/// <param name="newArrow">Flowchart.Net arrow</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private bool AddArrow(MindFusion.FlowChartX.Arrow newArrow)
		{
	
			bool bOk = false;
			XmlNode	xform_node, geom_node, xform1d_node, node_temp, node_temp2;
			string	sTemp = "" , sNURBS = "" , sNURBS1 = "";
			long  GeomCount = 0, PointCount = 0 , EveryCount = 0;
			double fTemp = 0, fTemp2 = 0, lW = 0, lW2 =0, lH = 0, lH2 = 0 ,  lBX = 0, lBY = 0 , lX1 = 0, lX2 = 0, lY1 = 0 , lY2 = 0,
				lStepX = 0, lStepY = 0;

			try
			{
				if ( newArrow == null )
					return false;
		
				shapeNew = shapeRoot.InsertAfter( shapeTemplate.Clone(), shapeRoot.LastChild) ;//shapeLast);

				if ( shapeNew == null )
					return false;

		
				// Setting arrow's ID & unique GUID
				shapeLast = shapeNew;
				shapeNew.Attributes["Master"].Value = "2";
				shapeNew.Attributes["ID"].Value = getID(newArrow).ToString();
				shapeNew.Attributes["Name"].Value = String.Format("Dynamic connector.{0}", getID(newArrow).ToString());
				shapeNew.Attributes["NameU"].Value = String.Format("Dynamic connector.{0}", getID(newArrow).ToString());

				sTemp = GetGUID();
				if ( sTemp != null )
				{
					shapeNew.Attributes["UniqueID"].Value = sTemp;
				}	

				// Getting arrow start & end point coordinates
		
				lX1 = newArrow.ControlPoints[0].X;
				lY1 = newArrow.ControlPoints[0].Y;
				lX2 = newArrow.ControlPoints[newArrow.ControlPoints.Count - 1].X;
				lY2 = newArrow.ControlPoints[newArrow.ControlPoints.Count - 1].Y;
				lW2 = lX2 - lX1;
				lW = ( lW2 == 0 ) ? ( 0.25 * PixPerInch ): lW2;
				lH2 = lY1 - lY2;
				lH = ( lH2 == 0 ) ? ( 0.25 * PixPerInch ): lH2;
				lBX = lX1 + ( ( lW2 == 0 ) ? 0 : lW/2 );
				lBY = lDocY - ( lY1 - (( lH2 == 0 ) ? 0 : lH/2)) ;
		
				// Setting arrow's dimensions for Visio	XML	nodes
				
				SetShapeDim(shapeNew, "vdx:XForm/vdx:PinX", lBX,  false);
				SetShapeDim(shapeNew, "vdx:XForm/vdx:PinY",lBY,  false);
				SetShapeDim(shapeNew, "vdx:XForm/vdx:Width", lW ,  true);
				SetShapeDim(shapeNew, "vdx:XForm/vdx:Height", lH,  true) ; 
				SetShapeDim(shapeNew, "vdx:XForm/vdx:LocPinX", lW/2 ,  true );
				SetShapeDim(shapeNew, "vdx:XForm/vdx:LocPinY", lH/2 ,  true);
				RemoveNode(shapeNew, "vdx:Group", true);
		

				// Reflecting arrow's geometry
				xform_node = shapeNew.SelectSingleNode("vdx:XForm", ns);

				if ( xform_node == null )
					return false;

				xform1d_node = shapeNew.InsertAfter(doc.CreateNode(XmlNodeType.Element,"XForm1D","http://schemas.microsoft.com/visio/2003/core"), xform_node);

				if ( xform1d_node == null )
					return false;

				node_temp = xform1d_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"BeginX","http://schemas.microsoft.com/visio/2003/core"), xform1d_node.LastChild);
				if ( node_temp == null )
					return false;

				node_temp = xform1d_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"BeginY","http://schemas.microsoft.com/visio/2003/core"), xform1d_node.LastChild);
				if ( node_temp == null )
					return false;

				node_temp = xform1d_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"EndX","http://schemas.microsoft.com/visio/2003/core"), xform1d_node.LastChild);
				if ( node_temp == null )
					return false;
		
				node_temp = xform1d_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"EndY","http://schemas.microsoft.com/visio/2003/core"), xform1d_node.LastChild);
				if ( node_temp == null )
					return false;


				SetShapeDim(shapeNew, "vdx:XForm1D/vdx:BeginX", lX1 ,  true);
				SetShapeDim(shapeNew, "vdx:XForm1D/vdx:BeginY", lDocY - lY1,  true) ; 
				SetShapeDim(shapeNew, "vdx:XForm1D/vdx:EndX", lX2 ,  true );
				SetShapeDim(shapeNew, "vdx:XForm1D/vdx:EndY",  lDocY - lY2 ,  true);

				geom_node =  shapeNew.InsertBefore(doc.CreateNode(XmlNodeType.Element,"Geom","http://schemas.microsoft.com/visio/2003/core"), shapeNew.SelectSingleNode("vdx:Text",ns));

				if ( geom_node == null )
					return false;

				SetShapeAttr("","IX",geom_node,"0");

				node_temp = geom_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"NoFill","http://schemas.microsoft.com/visio/2003/core"), geom_node.LastChild);
				if ( node_temp == null )
					return false;

				SetShapeAttr("","",node_temp,"1");

				node_temp = geom_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"NoLine","http://schemas.microsoft.com/visio/2003/core"), geom_node.LastChild);
				if ( node_temp == null )
					return false;
		
				SetShapeAttr("","",node_temp,"0");

				node_temp = geom_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"NoShow","http://schemas.microsoft.com/visio/2003/core"), geom_node.LastChild);
				if ( node_temp == null )
					return false;

				SetShapeAttr("","",node_temp,"0");
				node_temp = geom_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"NoSnap","http://schemas.microsoft.com/visio/2003/core"), geom_node.LastChild);
				if ( node_temp == null )
					return false;
		
				SetShapeAttr("","",node_temp,"0");

				node_temp = geom_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"MoveTo","http://schemas.microsoft.com/visio/2003/core"), geom_node.LastChild);
				if ( node_temp == null )
					return false;

				GeomCount++;

				SetShapeAttr("","IX",node_temp,String.Format("{0}",GeomCount));

		
				node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"X","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
				if ( node_temp2 == null )
					return false;

				SetShapeDim(node_temp,"vdx:X", 0, false);

				node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Y","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
				if ( node_temp2 == null )
					return false;

				SetShapeDim(node_temp,"vdx:Y", 0, false);

				SetShapeAttr("vdx:Misc/vdx:GlueType","",shapeNew, "2");
				SetShapeAttr("vdx:Misc/vdx:NoAlignBox","",shapeNew, "1");
				SetShapeAttr("vdx:Misc/vdx:ObjType","",shapeNew, "2");
				SetShapeAttr("vdx:Misc/vdx:BegTrigger","",shapeNew, "2");
				SetShapeAttr("vdx:Misc/vdx:EndTrigger","",shapeNew, "2");
		
				GeomCount++;
				PointCount = newArrow.ControlPoints.Count;
				if ( lW2 == 0 )
					sNURBS =  "NURBS(0.55, 3, 1, 1";
				else
					sNURBS =  "NURBS(0.75, 3, 1, 1";

				foreach ( PointF EveryPoint in newArrow.ControlPoints )
				{
					if ( EveryPoint.Equals( null ) )
						continue;

					if ( EveryCount == 0 )
					{
						EveryCount++;
						continue;
					}

					lStepX = EveryPoint.X - newArrow.ControlPoints[0].X;
					lStepY = newArrow.ControlPoints[0].Y - EveryPoint.Y;

					if ((newArrow.Style == ArrowStyle.Bezier) && (EveryCount!=1))
					{
						fTemp = lStepX/PixPerInch;
						fTemp2 = lStepY/PixPerInch;
						sNURBS1 = String.Format(",{0},{1}, 0,1", fTemp, fTemp2);
						sNURBS+=sNURBS1;

					}
					else
					{

						if ( newArrow.Style == ArrowStyle.Bezier)
						{
							sTemp = "NURBSTo";
							lStepX = (lW2 == 0) ? lW/2 : lW ;
							lStepY = (lH2 == 0) ? lH/2 : lH; 
						}
						else
							sTemp = "LineTo";
					
						node_temp = geom_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,sTemp ,"http://schemas.microsoft.com/visio/2003/core"), geom_node.LastChild);
					
						if ( node_temp != null )
						{
							SetShapeAttr("","IX",node_temp,String.Format("{0}",GeomCount));

		
							node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"X","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
							if ( node_temp2 != null )
								SetShapeDim(node_temp,"vdx:X", lStepX, false);

							node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Y","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
							if ( node_temp2 != null )
								SetShapeDim(node_temp,"vdx:Y", lStepY, false);

						}
				
						GeomCount++;
						EveryCount++;
					}
				}
				if (newArrow.Style == ArrowStyle.Bezier)
				{
					

					
					node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"A","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
					if ( node_temp2 != null )
						//SetShapeDim(node_temp,"vdx:A", 0*PixPerInch, false);
						SetShapeAttr("vdx:A","",node_temp, "0");

					node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"B","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
					if ( node_temp2 != null )
						SetShapeAttr("vdx:B","",node_temp, "1");
					//SetShapeDim(node_temp,"vdx:B", 1 *PixPerInch, false);

					node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"C","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
					if ( node_temp2 != null )
						//SetShapeDim(node_temp,"vdx:C", 0*PixPerInch, false);
						SetShapeAttr("vdx:C","",node_temp, "0");

					node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"D","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
					if ( node_temp2 != null )
						//SetShapeDim(node_temp,"vdx:D", 1*PixPerInch, false);
						SetShapeAttr("vdx:D","",node_temp, "1");

					node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"E","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
					if ( node_temp2 != null )
					{
						sNURBS+=")";
						SetShapeAttr("","Unit", node_temp2,"NURBS");

						double fX1 =0, fX2 = 0, fY1 =0, fY2 =0;
						fTemp = newArrow.ControlPoints[1].X - newArrow.ControlPoints[0].X;
						//fX1 = fTemp/PixPerInch;
						fX1 = Unit2Inch(pChart.MeasureUnit, fTemp);
						fTemp = newArrow.ControlPoints[0].Y - newArrow.ControlPoints[1].Y;
						//fY1 = fTemp/PixPerInch;
						fY1 = Unit2Inch(pChart.MeasureUnit, fTemp);
						fTemp = newArrow.ControlPoints[2].X - newArrow.ControlPoints[0].X;
						//fX2 = fTemp/PixPerInch;
						fX2 = Unit2Inch(pChart.MeasureUnit, fTemp);
						fTemp = newArrow.ControlPoints[0].Y - newArrow.ControlPoints[2].Y;
						//fY2 = fTemp/PixPerInch;
						fY2 = Unit2Inch(pChart.MeasureUnit, fTemp);
						sNURBS = String.Format("NURBS({0}, 3, 1, 1, {1}, {2} ,0,1, {3}, {4} ,0,1)", "0.75", 
							fX1.ToString().Replace(sSeparator, "."), 
							fY1.ToString().Replace(sSeparator, "."),
							fX2.ToString().Replace(sSeparator, "."), 
							fY2.ToString().Replace(sSeparator, "."));
						SetShapeAttr("","F", node_temp2,sNURBS);
						SetShapeAttr("","", node_temp2,sNURBS);
								
					}
								
				

					

		
					if (( GeomCount == 3	) && (newArrow.Style == ArrowStyle.Cascading))
					{
						node_temp2 = geom_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"LineTo","http://schemas.microsoft.com/visio/2003/core"), geom_node.LastChild);
						if ( node_temp2!=null)
						{
							SetShapeAttr("","IX",node_temp2,String.Format("{0}", GeomCount ));
							SetShapeAttr("","Del",node_temp2,"1");
						}
						
					}

				}
		
				// Getting font's parameters

				Font ShapeFont = null;
				ShapeFont =	newArrow.Font;
		
				if (ShapeFont == null)
					ShapeFont = pChart.Font;

				SetShapeFont(shapeNew, ShapeFont);

				// Setting shape elements colors
				
				SetShapeColor(shapeNew, "vdx:Fill/vdx:FillForegnd", newArrow.FillColor);
				SetShapeColor(shapeNew, "vdx:Char/vdx:Color", newArrow.TextColor);
				SetShapeColor(shapeNew, "vdx:Line/vdx:LineColor",newArrow.FrameColor);

				
		
				// Setting shape's text
				SetText(shapeNew, newArrow.Text,false , ShapeFont);

				// Getting line	width & pattern
		
				fTemp =  newArrow.Pen.Width / PixPerInch;
				sTemp = String.Format("{0}", fTemp);
				sTemp = sTemp.Replace(sSeparator, ".");
				SetShapeAttr("vdx:Line/vdx:LineWeight","",shapeNew, sTemp);
				SetShapeAttr("vdx:Line/vdx:LinePattern","",shapeNew, DashStyle2String(newArrow.Pen.DashStyle));

				SetShapeAttr("vdx:Line/vdx:EndArrow","",shapeNew, ArrowType2String(newArrow.ArrowHead));
				SetShapeAttr("vdx:Line/vdx:BeginArrow","",shapeNew, ArrowType2String(newArrow.ArrowBase));
		
				if (newArrow.Origin is Box || newArrow.Origin is Table)
				{
					node_temp = connectRoot.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Connect","http://schemas.microsoft.com/visio/2003/core"), connectRoot.LastChild);
					if ( node_temp!=null)
					{
						SetShapeAttr("","FromSheet",node_temp, getID(newArrow).ToString());
						SetShapeAttr("","FromCell",node_temp, "BeginX");
						SetShapeAttr("","FromPart",node_temp, "9");
						SetShapeAttr("","ToSheet",node_temp, getID(newArrow.Origin).ToString());
						SetShapeAttr("","ToCell",node_temp, "PinX");
						SetShapeAttr("","ToPart",node_temp, "3");
						
					}
				}

				if (newArrow.Destination is Box || newArrow.Destination is Table)
				{
					node_temp = connectRoot.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Connect","http://schemas.microsoft.com/visio/2003/core"), connectRoot.LastChild);
					if ( node_temp!=null)
					{
						SetShapeAttr("","FromSheet",node_temp, getID(newArrow).ToString());
						SetShapeAttr("","FromCell",node_temp, "EndX");
						SetShapeAttr("","FromPart",node_temp, "12");
						SetShapeAttr("","ToSheet",node_temp, getID(newArrow.Destination).ToString());
						SetShapeAttr("","ToCell",node_temp, "PinX");
						SetShapeAttr("","ToPart",node_temp, "3");
						
					}
				}

				// Setting Visio connector style
				switch (newArrow.Style)
				{
					case ArrowStyle.Polyline:
						sTemp = "1";
						break;
					case ArrowStyle.Bezier:
						sTemp = "2";
						break;
					default:
						sTemp = "";
						break;
				}


				if ( sTemp!="" )
				{
					node_temp = shapeNew.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Layout","http://schemas.microsoft.com/visio/2003/core"), geom_node);
					if ( node_temp!=null )
					{
						node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ConLineRouteExt","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
						if ( node_temp2!=null )
							node_temp2.InnerText = sTemp;

						if ( newArrow.Style == ArrowStyle.Polyline )
						{
							node_temp2 = node_temp.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ShapeRouteStyle","http://schemas.microsoft.com/visio/2003/core"), node_temp.LastChild);
							if ( node_temp2!=null )
								node_temp2.InnerText = "16";

							
						}


					}

				}
				
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","AddArrow",ex.Message));
				bOk = false;
			}

			return bOk;
		}

		/// <summary>
		/// Add Flowchart.Net box as a XML node into VDX
		/// </summary>
		/// <param name="newBox">Flowchart.NET box node</param>
		/// <param name="mainGroup">Flowchart.NET group if the box is attached to the group, set to [null] otherwise </param>
		/// <param name="IsGroup">Bool flag: set to [true] if the box is main group frame</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private bool AddBox(MindFusion.FlowChartX.Box newBox, MindFusion.FlowChartX.Group mainGroup, bool IsGroup)
		{
			return AddBox(newBox.BoundingRect, getID(newBox), newBox.RotationAngle, newBox.ShapeOrientation, newBox.Picture,
				newBox.FillColor, newBox.FrameColor, newBox.TextColor, newBox.Font, newBox.Transparent, newBox.Pen, newBox.Brush,
				newBox.EnableStyledText,newBox.Text, newBox.Style, newBox.Shape, (mainGroup == null) ? RectangleF.Empty : ( mainGroup.MainObject as Node ).BoundingRect , IsGroup,
				newBox.TextFormat.Alignment);

		
		}

		private bool AddBox(RectangleF rect, long ItemID, float RotationAngle, float ShapeOrientation, Image Picture ,
			Color FillColor, Color FrameColor, Color TextColor, Font ShapeFont, bool Transparent,
			FlowChartX.Pen ItemPen, FlowChartX.Brush ItemBrush, bool IsStyled, string ItemText, 
			BoxStyle ItemStyle, FlowChartX.ShapeTemplate ItemTemplate,
			RectangleF rectGroup, bool IsGroup, StringAlignment sa)
		{
			bool bOk = false, bPicture = false, NoDimChanges = false;
			double lLeft = 0,	lRight = 0,	lTop = 0, lBottom =	0, lW =	0, lH =	0, 
				lLeftG = 0,lRightG = 0,lTopG = 0, lBottomG =0,lWG = 0, lHG =0, fTemp = 0;
			string sTemp = null, sImg = null;
			XmlNode node_temp = null;


			try
			{
				// Inserting new Box
				if ( rectGroup != RectangleF.Empty)
				{
					sTemp = String.Format("vdx:Shape[@ID='{0}']", ItemID );
					shapeNew = shapeRoot.SelectSingleNode(sTemp, ns);
					if ( shapeNew == null )
						shapeNew = groupLast.InsertAfter( shapeTemplate.Clone(),  groupLast.LastChild );
					else
						NoDimChanges = true;
				}
				else
				{
					if (IsGroup) // If the box is main group's frame
					{
						
						shapeNew = shapeRoot.InsertAfter( shapeTemplate.Clone(), shapeRoot.LastChild);
					}
					else
						shapeNew = shapeRoot.InsertAfter( shapeTemplate.Clone(), shapeLast);
				}
				
				if ( shapeNew == null )
					return false;

				// If group is added then creating <Shapes> sub-tree

				
				/*
				 * 
				 * ROTATION
				 * 
				 */
				
				if ( RotationAngle!=0 )
				{
				
					float fAngle = RotationAngle + ShapeOrientation;
					int iSign = ( fAngle<180 ) ? -1 : 1;
					float fAngle2 = ( fAngle<180 ) ? fAngle : 360 - fAngle;

					sTemp =  String.Format("{0}", iSign*Math.PI*(fAngle2/180));
					sTemp = sTemp.Replace(sSeparator, ".");
					SetShapeAttr("vdx:XForm/vdx:Angle","",shapeNew, sTemp);
					RemoveNode(shapeNew.SelectSingleNode("vdx:XForm/vdx:Angle", ns),"F",false);

				}

				if (IsGroup)
				{
					node_temp = shapeNew.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Shapes","http://schemas.microsoft.com/visio/2003/core"), shapeNew.LastChild);
					if ( node_temp == null )
						return false;

					groupLast = node_temp;
				}

				// Setting shape's ID & unique GUID
				if ( rectGroup == RectangleF.Empty)
					shapeLast = shapeNew;

				shapeNew.Attributes["ID"].Value = ItemID.ToString();
			
				sTemp = GetGUID();
				if ( sTemp != null )
				{
					shapeNew.Attributes["UniqueID"].Value = sTemp;
				}	
				lLeft = rect.Left;
				lRight =  rect.Right;
				lTop =  rect.Top;
				lBottom = rect.Bottom;
				lW = rect.Width;
				lH = rect.Height;

				if (( ShapeOrientation == 90 )|| (ShapeOrientation == 270 ))
				{
					double lTemp = lW;
					lW = lH;
					lH = lTemp;
				}

				if ( Picture != null )
				{
					bPicture = true;
					sImg = Image2String(Picture);
					
				}

				if (rectGroup!=RectangleF.Empty )
				{
					lLeftG = rectGroup.Left;
					lRightG = rectGroup.Right;
					lTopG =  rectGroup.Top;
					lBottomG = rectGroup.Bottom;
					lWG = rectGroup.Width;
					lHG = rectGroup.Height;
					
					lLeft = lLeft - ( lLeftG );
					lTop = lTop - lTopG + lDocY  - lHG;
				}

				if (!IsGroup)
				{
					if ( bPicture )
					{
						// If the box is picture container
						// If box is PICTURE or OLE Control

						RemoveNode(shapeNew, "vdx:LayerMem", true);
						RemoveNode(shapeNew, "vdx:Event", true);
						RemoveNode(shapeNew, "vdx:Char", true);
						RemoveNode(shapeNew, "vdx:Text", true);
						RemoveNode(shapeNew, "vdx:Line", true);
						RemoveNode(shapeNew, "vdx:Fill", true);
						RemoveNode(shapeNew, "vdx:Misc", true);
						RemoveNode(shapeNew, "vdx:Group", true);
						RemoveNode(shapeNew, "Master", false);
						
						shapeNew.Attributes["NameU"].Value =  String.Format("Foreign.{0}", ItemID);
						shapeNew.Attributes["Name"].Value =  String.Format("Foreign.{0}", ItemID);
						shapeNew.Attributes["Type"].Value =  "Foreign";
						
						shapeNew.SelectSingleNode("vdx:XForm/vdx:LocPinX", ns).Attributes.RemoveNamedItem("F");
						shapeNew.SelectSingleNode("vdx:XForm/vdx:LocPinY", ns).Attributes.RemoveNamedItem("F");
					
						node_temp = shapeNew.SelectSingleNode("vdx:XForm", ns);

						if ( node_temp!=null)
						{
							XmlNode foregin_node = shapeNew.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Foreign","http://schemas.microsoft.com/visio/2003/core"), node_temp);
							if ( foregin_node!=null)
							{
								XmlNode first_node = null, second_node = null;
								first_node = foregin_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ImgOffsetX","http://schemas.microsoft.com/visio/2003/core"), first_node);
								second_node = foregin_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ImgOffsetY","http://schemas.microsoft.com/visio/2003/core"), first_node);
								second_node = foregin_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ImgWidth","http://schemas.microsoft.com/visio/2003/core"), second_node);
								second_node = foregin_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ImgHeight","http://schemas.microsoft.com/visio/2003/core"), second_node);
								SetShapeDim(shapeNew, "vdx:Foreign/vdx:ImgOffsetX", 0 , false);
								SetShapeDim(shapeNew, "vdx:Foreign/vdx:ImgOffsetY", 0, false);
								SetShapeDim(shapeNew, "vdx:Foreign/vdx:ImgWidth", lW , false);
								SetShapeDim(shapeNew, "vdx:Foreign/vdx:ImgHeight", lH, false);
							}

							XmlNode foregin_data_node = shapeNew.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ForeignData","http://schemas.microsoft.com/visio/2003/core"), foregin_node);
							if ( foregin_data_node!=null)
							{
								SetShapeAttr("", "ForeignType",foregin_data_node, "Bitmap");
								foregin_data_node.InnerText = sImg;
																
							}
						}

					
					}
					else
					{
						// If the box is regular					
						sTemp= SetMasterID(shapeNew,ItemTemplate, ItemStyle, ItemID);
						if (!NoDimChanges)
							RemoveNode(shapeNew, "vdx:Group", true);
					}

				}
				else
				{
					// If the box is a main group object

					SetShapeAttr("vdx:Misc/vdx:ObjType","", shapeNew, "8");
					SetShapeAttr("","Type", shapeNew, "Group");

					RemoveNode(shapeNew, "vdx:LayerMem", true);
					RemoveNode(shapeNew, "vdx:Event", true);
					//RemoveNode(shapeNew, "vdx:Char", true);
					//RemoveNode(shapeNew, "vdx:Text", true);
					//RemoveNode(shapeNew, "vdx:Line", true);
					//RemoveNode(shapeNew, "vdx:Fill", true);
					RemoveNode(shapeNew, "vdx:Foreign", true);
					RemoveNode(shapeNew, "vdx:ForeignData", true);
					RemoveNode(shapeNew, "Master", false);
					RemoveNode(shapeNew, "NameU", false);
					RemoveNode(shapeNew, "Name", false);
					sTemp = SetMasterID(shapeNew,ItemTemplate, ItemStyle, ItemID);
					
				}
				
				// Setting shape's dimensions for Visio	XML	nodes
				if (!NoDimChanges)
				{
					SetShapeDim(shapeNew, "vdx:XForm/vdx:PinX", lLeft + lW/2,  true);
					SetShapeDim(shapeNew, "vdx:XForm/vdx:PinY", (lDocY - ( lTop + lH/2)),  true);
					SetShapeDim(shapeNew, "vdx:XForm/vdx:Width",	 lW ,  true);
					SetShapeDim(shapeNew, "vdx:XForm/vdx:Height", lH,  true) ; 
					SetShapeDim(shapeNew, "vdx:XForm/vdx:LocPinX", lW/2 ,  true );
					SetShapeDim(shapeNew, "vdx:XForm/vdx:LocPinY", lH/2 ,  true);
				}

				if ( bPicture )
					return true;

				// Getting font's parameters

				if (ShapeFont == null)
					ShapeFont = pChart.Font;

				SetShapeFont(shapeNew, ShapeFont);

				// Setting shape elements colors
				
				SetShapeColor(shapeNew, "vdx:Fill/vdx:FillForegnd", FillColor);
				
				
				if ( ItemBrush is MindFusion.FlowChartX.LinearGradientBrush )
				{
					SetShapeColor(shapeNew, "vdx:Fill/vdx:FillBkgnd", 
						(ItemBrush as MindFusion.FlowChartX.LinearGradientBrush).LinearColors[1]);
					SetShapeAttr("vdx:Fill/vdx:FillPattern","", shapeNew, "36");
				}
				

				SetShapeColor(shapeNew, "vdx:Char/vdx:Color", TextColor);
				SetShapeColor(shapeNew, "vdx:Line/vdx:LineColor",FrameColor	);

				// Getting shape's transparency	level
				if (Transparent)
				{
					SetShapeAttr("vdx:Fill/vdx:FillForegndTrans","",shapeNew, "1");
					SetShapeAttr("vdx:Line/vdx:LineColorTrans","",shapeNew, "1");
					SetShapeAttr("vdx:Fill/vdx:FillPattern","", shapeNew, "0");
				}
				else
				{
					if (( FillColor == Color.Transparent ) || ( FillColor.A == 0 ))
					{
						SetShapeAttr("vdx:Fill/vdx:FillForegndTrans","",shapeNew, "1");
						SetShapeAttr("vdx:Fill/vdx:FillPattern","", shapeNew, "0");
					}
					else
					{
						fTemp  = (1 - FillColor.A)/ 255;
						sTemp = String.Format("{0}", fTemp);
						SetShapeAttr("vdx:Fill/vdx:FillForegndTrans","",shapeNew, sTemp);
					}
					
					if ( FrameColor == Color.Transparent )
					{
						SetShapeAttr("vdx:Line/vdx:LineColorTrans","",shapeNew, "1");
					}	
						
					
				}
		
				// Setting shape's text
				SetText(shapeNew, ItemText, IsStyled , ShapeFont, sa);

				// Getting line	width & pattern
		
				fTemp =  ItemPen.Width / PixPerInch;
				sTemp = String.Format("{0}", fTemp);
				sTemp = sTemp.Replace(sSeparator, ".");
				SetShapeAttr("vdx:Line/vdx:LineWeight","",shapeNew, sTemp);
				SetShapeAttr("vdx:Line/vdx:LinePattern","",shapeNew, DashStyle2String(ItemPen.DashStyle));

				bOk = true;
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("Box#{2}({3}) {0} error {1}\n","AddBox",ex.Message, ItemID, ItemText));
				bOk = false;
			}

			return bOk;
		
		}

		private bool AddTable2(MindFusion.FlowChartX.Table newTable)
		{
			bool IsAdded = false;
			if (AddBox(newTable.BoundingRect, getID(newTable), 0, 0 , newTable.Picture,
				Color.Transparent, newTable.FrameColor, newTable.TextColor, newTable.Font, false , newTable.Pen, newTable.Brush,
				newTable.EnableStyledText, "" , BoxStyle.Rectangle, null , RectangleF.Empty, true, StringAlignment.Center))
			{

				AddBox(newTable.BoundingRect, NextID(), 0, 0 , null,
					newTable.FillColor , newTable.FrameColor, newTable.TextColor, newTable.Font, false , newTable.Pen, newTable.Brush,
					newTable.EnableStyledText, "" , BoxStyle.Rectangle, null , newTable.BoundingRect, false, StringAlignment.Center);
		
						

				String sTag = "";
				float fX = newTable.BoundingRect.Left, 
					fY = newTable.BoundingRect.Top + newTable.CaptionHeight, 
					fW = (float) Math.Abs(newTable.BoundingRect.Right - newTable.BoundingRect.Left), 
					fH = (float) Math.Abs(newTable.BoundingRect.Top - newTable.BoundingRect.Bottom), 
					fCellH = 0, fCellX = 0, fCellY = 0 , rh =0, 
					fStepW = 0, fSumW =0, fCellW = 0;
				int ColCount = 0, RowCount = 0, AutoColls = 0;
				bool CellIsSpanned = false;

				RowCount = newTable.CurrScrollRow;
				ColCount = 0;
				AutoColls = 0;
							
				fCellX = fX;
				fCellY = fY;//+ newTable.CaptionHeight;
				
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
				
				
						if ( cell.Tag is string)
							sTag = cell.Tag.ToString();
						else
							sTag = "";
				
						if (sTag != "span")
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
									newTable[j,i].Tag = "span";
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

							IsAdded = AddBox(cellRect, NextID(), 0, 0 , null,
								Color.Transparent , (( newTable.CellFrameStyle == CellFrameStyle.None ) ? Color.Transparent : newTable.FrameColor), cell.TextColor, (newTable.Font == null) ? pChart.Font : newTable.Font , false , newTable.Pen, newTable.Brush,
								newTable.EnableStyledText, cell.Text , BoxStyle.Rectangle, null , newTable.BoundingRect , false, ( cell.TextFormat == null ) ? StringAlignment.Center : cell.TextFormat.Alignment );
										
										
							if ( cell.Picture != null )
							{
								float fiX = 0, fiY = 0, fiW = 0, fiH = 0;
								if ( GetImageDim(cell.Picture, cellRect, cell.PicturePos , ref fiW,
									ref fiH , ref fiX, ref fiY))
								{
									RectangleF cellImageRect = new RectangleF(fiX, fiY, fiW, fiH);
									IsAdded = AddBox(cellImageRect, NextID(), 0, 0 , cell.Picture,
										Color.Transparent, Color.Transparent, Color.Transparent, (newTable.Font == null) ? pChart.Font : newTable.Font , true , newTable.Pen, newTable.Brush,
										false, "" , BoxStyle.Rectangle, null , newTable.BoundingRect , false, StringAlignment.Center);
								}
							}
										
										
										
						}
						ColCount++;
									
										
					}
				
					fCellX = fX;
					ColCount = 0;
					RowCount++;
								
									
				}


				if ( newTable.CaptionHeight!=0 )
				{
					RectangleF captionRect = new RectangleF(newTable.BoundingRect.Left, newTable.BoundingRect.Top,
						newTable.BoundingRect.Width, newTable.CaptionHeight);
					IsAdded = AddBox(captionRect, NextID(), 0, 0 , null,
						newTable.FillColor, newTable.FrameColor, newTable.CaptionColor , (newTable.Font == null) ? pChart.Font : newTable.Font , false , newTable.Pen, newTable.Brush,
						newTable.EnableStyledText, newTable.Caption , BoxStyle.Rectangle, null , newTable.BoundingRect , false, ( newTable.CaptionFormat == null ) ? StringAlignment.Center : newTable.CaptionFormat.Alignment );
									
				}
			}

			return true;

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
		/// Add Flowchart.Net table as a XML node into VDX
		/// </summary>
		/// <param name="newTable">Flowchart.NET table node</param>
		/// <returns>[true] if successfull [false] otherwise</returns>
		private bool AddTable(MindFusion.FlowChartX.Table newTable)
		{
			if (m_ExportTablesAsGroups)
				return AddTable2(newTable);

			bool bOk = false;
			double lLeft = 0,	lTop = 0,  lW =	0, lH =	0, lWG = 0, lHG =0,
				lLeftG = 0,	lRightG = 0,	lTopG = 0, lBottomG =0;
			string sTemp = null, sTemp2 = null;
			XmlNode  table_shapes = null, shapeCaption = null, shapeBody = null;
			int RowCount = 0, ColCount = 0;
			long ID = 0;

			try
			{
				// Inserting new <shape> node
				shapeNew = shapeRoot.InsertAfter( shapeTemplate.Clone(), shapeLast);
				
				if ( shapeNew == null )
					return false;

			
				ID = (long) getID(newTable);
				sTemp = GetGUID();
				
				// Setting entity - like shape parameters as 'group'
				shapeLast = shapeNew;
				shapeNew.Attributes["ID"].Value = ID.ToString();
				//Trace.WriteLine(String.Format("Table {0}\t{1}", ID.ToString(), sTemp));
				shapeNew.Attributes["Name"].Value = String.Format("Entity 2.{0}",ID);
				shapeNew.Attributes["Master"].Value = "3"; //"47";//"3";
				shapeNew.Attributes.RemoveNamedItem("NameU");
				RemoveNode(shapeNew, "vdx:LayerMem", true);
				RemoveNode(shapeNew, "vdx:Event", true);
				RemoveNode(shapeNew, "vdx:Text", true);
				RemoveNode(shapeNew, "vdx:Line", true);
				RemoveNode(shapeNew, "vdx:Fill", true);
				SetShapeAttr("vdx:Misc/vdx:ObjType","", shapeNew, "8");
				SetShapeAttr("","Type", shapeNew, "Group");

				
				if ( sTemp != null )
				{
					shapeNew.Attributes["UniqueID"].Value = sTemp;
				}	

				// Getting table dimensions
				lLeftG = newTable.BoundingRect.Left;
				lRightG =  newTable.BoundingRect.Right;
				lTopG =  newTable.BoundingRect.Top;
				lBottomG = newTable.BoundingRect.Bottom;
				lWG = newTable.BoundingRect.Width;
				lHG = newTable.BoundingRect.Height;

				// Setting entity frame's dimensions
				SetShapeDim(shapeNew, "vdx:XForm/vdx:PinX", lLeftG + lWG/2,  true);
				SetShapeDim(shapeNew, "vdx:XForm/vdx:PinY", lDocY - ( lTopG +	lHG/2),  true);
				SetShapeDim(shapeNew, "vdx:XForm/vdx:Width",	 lWG ,  true);
				SetShapeDim(shapeNew, "vdx:XForm/vdx:Height", lHG,  true) ; 
				SetShapeDim(shapeNew, "vdx:XForm/vdx:LocPinX", lWG/2 ,  true );
				SetShapeDim(shapeNew, "vdx:XForm/vdx:LocPinY", lHG/2 ,  true);
			
				// Getting & setting font's parameters

				Font ShapeFont = null;
				ShapeFont =	newTable.Font;
		
				if (ShapeFont == null)
					ShapeFont = pChart.Font;

				SetShapeFont(shapeNew, ShapeFont);


				// Creating <Shapes> tree
				table_shapes = shapeNew.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Shapes","http://schemas.microsoft.com/visio/2003/core"), shapeNew.LastChild);

				if ( table_shapes == null )
					return false;

				// Inserting <shape> node for table's caption
				shapeCaption = table_shapes.InsertAfter(shapeTemplate.Clone(), table_shapes.LastChild);

				if ( shapeCaption == null )
					return false;

				
				lW = lWG;
				lH = newTable.CaptionHeight;
				lLeft = lWG/2; // 0
				lTop = lHG - newTable.CaptionHeight;//0

				ID = NextID();
				sTemp = GetGUID();

				// Setting enity caption's attributes
				shapeCaption.Attributes["ID"].Value = ID.ToString();
				SetShapeAttr("","MasterShape",shapeCaption,"6");//"9");//"6");
				//Trace.WriteLine(String.Format("Caption {0}\t{1}", ID.ToString(), sTemp));
				RemoveNode(shapeCaption,"Master",false);
				RemoveNode(shapeCaption,"Name",false);
				RemoveNode(shapeCaption,"NameU",false);
				RemoveNode(shapeCaption, "vdx:Group", true);
				
				if ( sTemp != null )
				{
					shapeCaption.Attributes["UniqueID"].Value = sTemp;
				}	

				// Setting enity caption's dimensions
				SetShapeDim(shapeCaption, "vdx:XForm/vdx:PinX",lLeft,  true);
				SetShapeDim(shapeCaption, "vdx:XForm/vdx:PinY", lTop,  true);
				SetShapeDim(shapeCaption, "vdx:XForm/vdx:Width", lW ,  true);
				SetShapeDim(shapeCaption, "vdx:XForm/vdx:Height", lH,  true) ; 
				SetShapeDim(shapeCaption, "vdx:XForm/vdx:LocPinX", lLeft ,  true ); //0
				SetShapeDim(shapeCaption, "vdx:XForm/vdx:LocPinY", 0 ,  true);
			
			
				// Getting & setting  caption's font
				SetShapeFont(shapeCaption, ShapeFont);
				SetText(shapeCaption, newTable.Caption, newTable.EnableStyledText, ShapeFont);

				// Getting & setting  caption's colors
				SetShapeColor(shapeCaption, "vdx:Fill/vdx:FillForegnd", newTable.FillColor);
				SetShapeColor(shapeCaption, "vdx:Char/vdx:Color", newTable.CaptionColor);
				SetShapeColor(shapeCaption, "vdx:Line/vdx:LineColor",newTable.Pen.Color);

				// Inserting <shape> node for table's body
				shapeBody = table_shapes.InsertAfter(shapeTemplate.Clone(), table_shapes.LastChild);

				if ( shapeBody == null )
					return false;

				
				lW = lWG;
				lH = lHG - newTable.CaptionHeight;
				lLeft = lWG/2; // 0
				lTop = lHG - newTable.CaptionHeight;//0


				// Setting enity body's attributes
				ID = NextID();
				sTemp = GetGUID();
				shapeBody.Attributes["ID"].Value = ID.ToString();
				SetShapeAttr("","MasterShape",shapeBody,"7");//"10");//"7");
				//Trace.WriteLine(String.Format("Body {0}\t{1}", ID.ToString(), sTemp));
				RemoveNode(shapeBody,"Master",false);
				RemoveNode(shapeBody,"Name",false);
				RemoveNode(shapeBody,"NameU",false);
				RemoveNode(shapeBody, "vdx:Group", true);
				
			
				if ( sTemp != null )
				{
					shapeBody.Attributes["UniqueID"].Value = sTemp;
				}

				// Setting  enity body's dimensions
				SetShapeDim(shapeBody, "vdx:XForm/vdx:PinX",lLeft,  true);
				SetShapeDim(shapeBody, "vdx:XForm/vdx:PinY", lTop,  true);
				SetShapeDim(shapeBody, "vdx:XForm/vdx:Width", lW ,  true);
				SetShapeDim(shapeBody, "vdx:XForm/vdx:Height", lH,  true) ; 
				SetShapeDim(shapeBody, "vdx:XForm/vdx:LocPinX", lLeft ,  true );
				SetShapeDim(shapeBody, "vdx:XForm/vdx:LocPinY", lTop ,  true);
			
				SetShapeColor(shapeBody, "vdx:Fill/vdx:FillForegnd", newTable.FillColor);
				SetShapeColor(shapeBody, "vdx:Char/vdx:Color", newTable.TextColor);
				SetShapeColor(shapeBody, "vdx:Line/vdx:LineColor",newTable.Pen.Color);

				// Collecting text from all table rows
				sTemp = "";
				for ( RowCount =1; RowCount<=newTable.Rows.Count; RowCount++ )
				{
			
					for ( ColCount =1; ColCount<=newTable.Columns.Count; ColCount++ )
					{
						sTemp2+=newTable[ColCount -1 ,RowCount -1 ].Text + " ";
					}

					sTemp2 = sTemp2.Trim();
					sTemp+=sTemp2 + "\n";
					sTemp2 = "";
				}

				// Setting collected text into tanle's body
				SetShapeFont(shapeBody, ShapeFont);
				SetText(shapeBody, sTemp, newTable.EnableStyledText, ShapeFont);
				bOk = true;
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","AddTable",ex.Message));
				bOk = false;
			}

			return bOk;
		
		}

		public static string Align2Str( StringAlignment sa)
		{
			string sAllign = "";
			switch (sa)
			{
				case StringAlignment.Center:
					sAllign = "1";
					break;
				case StringAlignment.Far:
					sAllign = "2";
					break;
				case StringAlignment.Near:
					sAllign = "0";
					break;
			};

			return sAllign;
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

		bool CleanUp ()
		{
			bool bOk = false;
			string sMaster = "", sPath = "";
			int nCount = 0;
			XmlNode masters = null;
			try
			{
				shapeRoot.RemoveChild(root.SelectSingleNode("//vdx:Pages/vdx:Page[@ID='0']/vdx:Shapes/vdx:Shape[@ID='0']",ns));

				masters = root.SelectSingleNode("//vdx:Masters", ns);

				if ( masters == null )
					return false;

				// Processing <Masters> tree checking whether each <Master> is utilized
				foreach ( XmlNode next_master in root.SelectNodes("//vdx:Masters/vdx:Master", ns))
				{
					if ( next_master == null )
						continue;

					sMaster = next_master.Attributes["ID"].Value.ToString();

					sPath = String.Format("//vdx:Pages/vdx:Page[@ID='0']/vdx:Shapes/vdx:Shape[@Master='{0}']", sMaster);
					nCount = 0;
					foreach ( XmlNode next_shape in root.SelectNodes(sPath, ns))
					{
						if ( next_shape == null )
							continue;
						
						nCount++;
					}
				

					sPath = String.Format("//vdx:Masters/vdx:Master[@ID='{0}']", sMaster);
					
					if ( nCount == 0 )
						RemoveNode(masters, sPath , true );

					Trace.WriteLine(String.Format("Master {0}\t{1}", sMaster, (nCount==0) ? "DELETED" : ""));
					
				}
				bOk = true;
			}
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","CleanUp",ex.Message));
				bOk = false;
			}

			return bOk;

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

	

		bool AddPicture ( XmlNode shapeNew, Image newImg , RectangleF rect)
		{
			XmlNode node_temp;
			bool bOk = false;
			try
			{
				if ( newImg == null )
					return true;
				RemoveNode(shapeNew, "vdx:LayerMem", true);
				RemoveNode(shapeNew, "vdx:Event", true);
				RemoveNode(shapeNew, "vdx:Char", true);
				RemoveNode(shapeNew, "vdx:Text", true);
				//	RemoveNode(shapeNew, "vdx:Line", true);
				RemoveNode(shapeNew, "vdx:Fill", true);
				RemoveNode(shapeNew, "vdx:Misc", true);
				RemoveNode(shapeNew, "vdx:Group", true);
				RemoveNode(shapeNew, "Master", false);
						
				shapeNew.Attributes["Type"].Value =  "Foreign";
						
				shapeNew.SelectSingleNode("vdx:XForm/vdx:LocPinX", ns).Attributes.RemoveNamedItem("F");
				shapeNew.SelectSingleNode("vdx:XForm/vdx:LocPinY", ns).Attributes.RemoveNamedItem("F");
					
				node_temp = shapeNew.SelectSingleNode("vdx:XForm", ns);

				if ( node_temp!=null)
				{
					XmlNode foregin_node = shapeNew.InsertAfter(doc.CreateNode(XmlNodeType.Element,"Foreign","http://schemas.microsoft.com/visio/2003/core"), node_temp);
					if ( foregin_node!=null)
					{
						XmlNode first_node = null, second_node = null;
						first_node = foregin_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ImgOffsetX","http://schemas.microsoft.com/visio/2003/core"), first_node);
						second_node = foregin_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ImgOffsetY","http://schemas.microsoft.com/visio/2003/core"), first_node);
						second_node = foregin_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ImgWidth","http://schemas.microsoft.com/visio/2003/core"), second_node);
						second_node = foregin_node.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ImgHeight","http://schemas.microsoft.com/visio/2003/core"), second_node);
						SetShapeDim(shapeNew, "vdx:Foreign/vdx:ImgOffsetX", 0 , false);
						SetShapeDim(shapeNew, "vdx:Foreign/vdx:ImgOffsetY", 0, false);
						SetShapeDim(shapeNew, "vdx:Foreign/vdx:ImgWidth", rect.Width , false);
						SetShapeDim(shapeNew, "vdx:Foreign/vdx:ImgHeight", rect.Height, false);
					}

					XmlNode foregin_data_node = shapeNew.InsertAfter(doc.CreateNode(XmlNodeType.Element,"ForeignData","http://schemas.microsoft.com/visio/2003/core"), foregin_node);
					if ( foregin_data_node!=null)
					{
						SetShapeAttr("", "ForeignType",foregin_data_node, "Bitmap");
						foregin_data_node.InnerText = Image2String(newImg);
																
					}

					bOk = true;
				}
			} 
			catch ( Exception ex )
			{
				Trace.WriteLine(String.Format("{0} error {1}\n","AddPicture",ex.Message));
				bOk = false;
			}

			return bOk;

		
		}
	}
}
