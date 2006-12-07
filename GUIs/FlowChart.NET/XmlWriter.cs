// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Xml;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;
using System.ComponentModel;
using System.Collections;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;

namespace MindFusion.FlowChartX.Xml
{
	using FlowChartX = MindFusion.FlowChartX;

	/// <summary>
	/// Extends the XmlConvert class.
	/// </summary>
	public class XmlConvert
	{
		static XmlConvert()
		{
			_enums = new Hashtable();

			_enums["BorderAligned"] = "Cascading";
			_enums["Centralized"] = "Centered";
			_enums["Rect"] = "Rectangle";
			_enums["Perpendicular"] = "Cascading";
			_enums["WhenIntersectNode"] = "WhenIntersectsNode";
			_enums["Breaking"] = "BreakOff";
			_enums["RoundRect"] = "RoundedRectangle";
			_enums["Complex"] = "Shape";
			_enums["CenterLeft"] = "MiddleLeft";
			_enums["SelHandles"] = "SelectionHandles";
		}

		private XmlConvert()
		{
		}

		private static CultureInfo _english = new CultureInfo("en-US");

		public static bool ToBoolean(string s)
		{
			try
			{
				return bool.Parse(s);
			}
			catch
			{
				return false;
			}
		}

		public static string FromBoolean(bool val)
		{
			return val.ToString();
		}

		public static float ToSingle(string s)
		{
			try
			{
				return float.Parse(s, _english);
			}
			catch
			{
				try
				{
					return float.Parse(s, CultureInfo.CurrentCulture);
				}
				catch
				{
					return 0;
				}
			}
		}

		public static string FromSingle(float val)
		{
			return val.ToString(_english);
		}

		public static double ToDouble(string s)
		{
			try
			{
				return double.Parse(s, _english);
			}
			catch
			{
				try
				{
					return double.Parse(s, CultureInfo.CurrentCulture);
				}
				catch
				{
					return 0;
				}
			}
		}

		public static string FromDouble(double val)
		{
			return val.ToString(_english);
		}

		public static Int16 ToInt16(string s)
		{
			try
			{
				return Int16.Parse(s, _english);
			}
			catch
			{
				return Int16.Parse(s, CultureInfo.CurrentCulture);
			}
		}

		public static string FromInt16(Int16 val)
		{
			return val.ToString(_english);
		}

		public static Int32 ToInt32(string s)
		{
			try
			{
				return Int32.Parse(s, _english);
			}
			catch
			{
				return Int32.Parse(s, CultureInfo.CurrentCulture);
			}
		}

		public static string FromInt32(Int32 val)
		{
			return val.ToString(_english);
		}

		public static Int64 ToInt64(string s)
		{
			try
			{
				return Int64.Parse(s, _english);
			}
			catch
			{
				return Int64.Parse(s, CultureInfo.CurrentCulture);
			}
		}

		public static string FromInt64(Int64 val)
		{
			return val.ToString(_english);
		}

		public static Color ToColor(string s)
		{
			if(s.Length != 9)
				return Color.Black;
			if(s[0] != '#')
				return Color.Black;

			int a, r, g, b;

			try
			{
				a = int.Parse(s.Substring(1, 2), NumberStyles.AllowHexSpecifier);
				r = int.Parse(s.Substring(3, 2), NumberStyles.AllowHexSpecifier);
				g = int.Parse(s.Substring(5, 2), NumberStyles.AllowHexSpecifier);
				b = int.Parse(s.Substring(7, 2), NumberStyles.AllowHexSpecifier);
			}
			catch
			{
				return Color.Black;
			}

			return Color.FromArgb(a, r, g, b);
		}

		public static string FromColor(Color val)
		{
			string s;
			s = "#" + val.A.ToString("X2") +
				val.R.ToString("X2") + val.G.ToString("X2") +
				val.B.ToString("X2");

			return s;
		}

		public static System.Drawing.Image ImageFromString(string imageRepresentation)
		{
			int len = imageRepresentation.Length / 2;
			byte[] buf = new byte[len];

			for (int cc = 0; cc < len; ++cc)
			{
				string hex = imageRepresentation.Substring(cc * 2, 2);
				buf[cc] = Byte.Parse(hex, System.Globalization.NumberStyles.HexNumber);
			}

			return System.Drawing.Image.FromStream(new System.IO.MemoryStream(buf));
		}

		public static Image ToImageV4(string s)
		{
			return Image.FromStream(ToStreamV4(s));
		}

		public static string FromImageV4(Image val)
		{
			MemoryStream stream = new MemoryStream();
			try
			{
				val.Save(stream, val.RawFormat);
			}
			catch (ArgumentNullException)
			{
				val.Save(stream, ImageFormat.Bmp);
			}
			return FromStreamV4(stream);
		}

		public static Image ToImage(string s)
		{
			byte[] b = Convert.FromBase64String(s);
			MemoryStream stream = new MemoryStream(b, false);
			return Image.FromStream(stream);
		}

		public static string FromImage(Image val)
		{
			MemoryStream stream = new MemoryStream();
			try
			{
				val.Save(stream, val.RawFormat);
			}
			catch (ArgumentNullException)
			{
				val.Save(stream, ImageFormat.Bmp);
			}
			return Convert.ToBase64String(stream.GetBuffer());
		}

		public static Stream ToStreamV4(string s)
		{
			int len = s.Length / 2;
			byte[] buf = new byte[len];
			for (int cc = 0; cc < len; ++cc)
			{
				string hex = s.Substring(cc * 2, 2);
				buf[cc] = Byte.Parse(hex, System.Globalization.NumberStyles.HexNumber);
			}

			return new MemoryStream(buf);
		}

		public static string FromStreamV4(MemoryStream val)
		{
			int len = (int)val.Length;
			byte[] buf = val.GetBuffer();
			System.Text.StringBuilder builder = new
				System.Text.StringBuilder(2 * len + 1);
			for (uint cc = 0; cc < len; ++cc)
				builder.AppendFormat("{0:X2}", buf[cc]);

			return builder.ToString();
		}

		public static Stream ToStream(string s)
		{
			byte[] b = Convert.FromBase64String(s);
			MemoryStream stream = new MemoryStream(b, false);
			return stream;
		}

		public static string FromStream(Stream val)
		{
			int len = (int)val.Length;
			byte[] buf = new byte[len];

			val.Seek(0, SeekOrigin.Begin);
			val.Read(buf, 0, len);
			return Convert.ToBase64String(buf);
		}

		public static object ToEnum(Type type, string s)
		{
			try
			{
				return Enum.Parse(type, s, false);
			}
			catch
			{
				int ss = 0;

				try
				{
					ss = ToInt32(s);

					Array values = Enum.GetValues(type);
					foreach (int val in values)
					{
						if (val == ss)
							return Enum.Parse(type, Enum.GetName(type, ss), false);
					}
				}
				catch
				{
				}

				// It is likely a renamed enum

				// Try to remove the lowercase prefix
				string newEnum = "";

				int first = 0;
				for (int i = 0; i < s.Length; i++)
				{
					if (s[i] >= 'A' && s[i] <= 'Z')
					{
						first = i;
						break;
					}
				}

				newEnum = s.Substring(first);

				if (Enum.IsDefined(type, newEnum))
					return Enum.Parse(type, newEnum, false);

				// It is a renamed enum, not only prefix-truncated
				if (_enums.Contains(newEnum))
					newEnum = _enums[newEnum] as string;

				if (Enum.IsDefined(type, newEnum))
					return Enum.Parse(type, newEnum, false);

				return null;
			}
		}


		private static Hashtable _enums;
	}

	public class XmlWriterOptions
	{
		public XmlWriterOptions()
		{
			_writeImages = false;
			_writeXMLDecl = true;
			customTagSerialization = false;
		}

		public bool WriteImages
		{
			get { return _writeImages; }
			set { _writeImages = value; }
		}

		public bool WriteXMLDecl
		{
			get { return _writeXMLDecl; }
			set { _writeXMLDecl = value; }
		}

		public bool CustomTagSerialization
		{
			get { return customTagSerialization; }
			set { customTagSerialization = value; }
		}

		private bool _writeImages;
		private bool _writeXMLDecl;
		private bool customTagSerialization;
	}

	/// <summary>
	/// Writes the FlowChart diagram in a XML file.
	/// </summary>
	[ComVisible(true)]
	public class XmlWriter
	{
		private FlowChart _diagram;
		private XmlWriterOptions _options;

		// Arrays with the properties to be exported
		static private string[] _appearanceProps =
			new string[]
			{
				"BackColor",
				"ActiveMnpColor",
				"AntiAlias",
				"ArrowCrossings",
				"DisabledMnpColor",
				"BackgroundImage",
				"BkgrImagePos",
				"CrossingRadius",
				"ExpandBtnPos",
				"Font",
				"InplaceEditFont",
				"RoundedArrows",
				"RoundedArrowsRadius",
				"RoundRectFactor",
				"SelMnpColor",
				"ShadowColor",
				"ShadowsStyle",
				"ShowAnchors",
				"SelHandleSize"
			};
		static private string[] _behaviourProps =
			new string[]
			{
				"AllowInplaceEdit",
				"AllowLinksRepeat",
				"AllowRefLinks",
				"AllowUnanchoredArrows",
				"ArrowEndsMovable",
				"ArrowsSplittable",
				"AutoScroll",
				"Behavior",
				"ExpandOnIncoming",
				"HitTestPriority",
				"ModificationStart",
				"PrpArrowStartOrnt",
				"RecursiveExpand",
				"RestrObjsToDoc",
				"SelectAfterCreate",
				"SelectionOnTop",
				"SnapToAnchor"
			};
		static private string[] _defaultProps =
			new string[]
			{
				"ArrowBase",
				"ArrowBaseSize",
				"ArrowColor",
				"ArrowCustomDraw",
				"ArrowFillColor",
				"ArrowHead",
				"ArrowHeadSize",
				"ArrowIntermSize",
				"ArrowsRetainForm",
				"ArrowSegments",
				"ArrowSelStyle",
				"ArrowsSnapToBorders",
				"ArrowStyle",
				"ArrowText",
				"ArrowTextStyle",
				"BoxCustomDraw",
				"BoxesExpandable",
				"BoxFillColor",
				"BoxFrameColor",
				"BoxSelStyle",
				"BoxStyle",
				"BoxText",
				"DefaultShape",
				"DynamicArrows",
				"EnableStyledText",
				"IntermArrowHead",
				"PenDashStyle",
				"PenWidth",
				"PolyTextLayout",
				"RouteArrows",
				"ShadowOffsetX",
				"ShadowOffsetY",
				"ShapeOrientation",
				"TableCaption",
				"TableCaptionHeight",
				"TableCellBorders",
				"TableColumnsCount",
				"TableColWidth",
				"TableFillColor",
				"TableFrameColor",
				"TableLinkStyle",
				"TableRowHeight",
				"TableRowsCount",
				"TableSelStyle",
				"TablesScrollable",
				"TextColor",
				"TextFormat"
			};
		static private string[] _gridProps =
			new string[]
			{
				"AlignToGrid",
				"GridColor",
				"GridSizeX",
				"GridSizeY",
				"GridStyle",
				"ShowGrid"
			};
		static private string[] _layoutProps =
			new string[]
			{
				"DocExtents",
				"ScrollX",
				"ScrollY",
				"ZoomFactor"
			};
		static private string[] _miscProps =
			new string[]
			{
				"MeasureUnit",
				"ShowDisabledHandles",
				"Tag"
			};

		static private string[] _boxProps =
			new string[]
			{
				"AnchorPattern",
				"BoundingRect",
				"ShapeRotation",
				"PolyTextLayout",
				"CustomDraw",
				"EnableStyledText",
				"Expandable",
				"Expanded",
				"FillColor",
				"Font",
				"FrameColor",
				"HyperLink",
				"IgnoreLayout",
				"Locked",
				"MnpHandlesMask",
				"Obstacle",
				"PenDashStyle",
				"PenWidth",
				"Picture",
				"PicturePos",
				"Printable",
				"RotationAngle",
				"RotateContents",
				"Selected",
				"SelStyle",
				"ShadowColor",
				"ShadowOffsetX",
				"ShadowOffsetY",
				"Text",
				"TextColor",
				"TextFormat",
				"ToolTip",
				"Transparent",
				"Visible",
				"Weight",
				"AllowIncomingArrows",
				"AllowOutgoingArrows"
			};

		static private string[] _hostProps = 
			new string[]
			{
				"AnchorPattern",
				"BoundingRect",
				"Expanded",
				"Expandable",
				"FillColor",
				"Font",
				"FrameColor",
				"HandlesStyle",
				"HyperLink",
				"Locked",
				"MnpHandlesMask",
				"Obstacle",
				"PenDashStyle",
				"PenWidth",
				"Printable",
				"Selected",
				"ShadowColor",
				"ShadowOffsetX",
				"ShadowOffsetY",
				"ToolTip",
				"Visible",
				"Weight",
				"AllowIncomingArrows",
				"AllowOutgoingArrows"
			};

		static private string[] _tableProps =
			new string[]
			{
				"AnchorPattern",
				"RowAnchorPattern",
				"BoundingRect",
				"Caption",
				"CaptionColor",
				"CaptionFormat",
				"CaptionHeight",
				"CellBorders",
				"CellCustomDraw",
				"ColumnWidth",
				"CurrScrollRow",
				"CustomDraw",
				"EnableStyledText",
				"Expanded",
				"Expandable",
				"FillColor",
				"Font",
				"FrameColor",
				"HyperLink",
				"IgnoreLayout",
				"LinkStyle",
				"Locked",
				"MnpHandlesMask",
				"Obstacle",
				"OffsetHeaderRows",
				"PenDashStyle",
				"PenWidth",
				"Picture",
				"PicturePos",
				"Printable",
				"RowHeight",
				"Scrollable",
				"Selected",
				"SelStyle",
				"Style",
				"ShadowColor",
				"ShadowOffsetX",
				"ShadowOffsetY",
				"TextColor",
				"ToolTip",
				"Visible",
				"Weight",
				"AllowIncomingArrows",
				"AllowOutgoingArrows"
			};

		static private string[] _arrowProps =
			new string[]
			{
				"AllowMoveEnd",
				"AllowMoveStart",
				"ArrowBase",
				"ArrowBaseSize",
				"ArrowHead",
				"ArrowHeadSize",
				"AutoRoute",
				"BoundingRect",
				"CustomDraw",
				"DestAnchor",
				"DrawCrossings",
				"Dynamic",
				"FillColor",
				"Font",
				"FrameColor",
				"HyperLink",
				"IgnoreLayout",
				"IntermArrowHead",
				"IntermHeadSize",
				"Locked",
				"OrgnAnchor",
				"PenColor",
				"PenDashStyle",
				"PenWidth",
				"Printable",
				"PrpStartOrientation",
				"RetainForm",
				"SegmentCount",
				"Selected",
				"SelStyle",
				"ShadowColor",
				"ShadowOffsetX",
				"ShadowOffsetY",
				"SnapToNodeBorder",
				"Style",
				"Text",
				"TextColor",
				"TextStyle",
				"ToolTip",
				"Visible",
				"Weight",
				"PrpHorzFirst" // This has to be after Style, because Style can reset it
			};

		public XmlWriter(FlowChart diagram)
		{
			_diagram = diagram;

			// Create default options
			_options = new XmlWriterOptions();

			// Create the property map, which
			// maps the new property names to the old property names
			_propertyMap = new Hashtable();

			_propertyMap["ExpandBtnPos"] = "ExpandButtonPosition";
			_propertyMap["SelStyle"] = "HandlesStyle";
			_propertyMap["BoxSelStyle"] = "BoxHandlesStyle";
			_propertyMap["TableCellBorders"] = "CellFrameStyle";
			_propertyMap["TableSelStyle"] = "TableHandlesStyle";
			_propertyMap["MnpHandlesMask"] = "EnabledHandles";
			_propertyMap["CellBorders"] = "CellFrameStyle";
			_propertyMap["ArrowSelStyle"] = "ArrowHandlesStyle";
			_propertyMap["TableRowsCount"] = "TableRowCount";
			_propertyMap["TableColumnsCount"] = "TableColumnCount";
			_propertyMap["ShapeRotation"] = "ShapeOrientation";
			_propertyMap["PrpStartOrientation"] = "CascadeOrientation";
			_propertyMap["PrpArrowStartOrnt"] = "ArrowCascadeOrientation";
			_propertyMap["PrpHorzFirst"] = "CascadeStartHorizontal";
			_propertyMap["ArrowsSplittable"] = "AllowSplitArrows";
			_propertyMap["Picture"] = "Image";
			_propertyMap["PicturePos"] = "ImageAlign";
		}

		/// <summary>
		/// Adjust the Writer's options
		/// </summary>
		public XmlWriterOptions Options
		{
			get { return _options; }
			set { _options = value; }
		}


		private void WriteFontElement(
			System.Xml.XmlWriter writer,
			string name, Font val)
		{
			writer.WriteStartElement(name);

			// Write font attributes
			writer.WriteElementString("Name", val.Name);
			writer.WriteElementString("Size", XmlConvert.FromSingle(val.Size));
			writer.WriteElementString("Unit", val.Unit.ToString());
			writer.WriteElementString("Bold", val.Bold.ToString());
			writer.WriteElementString("Italic", val.Italic.ToString());
			writer.WriteElementString("Underline", val.Underline.ToString());
			writer.WriteElementString("Strikeout", val.Strikeout.ToString());

			writer.WriteEndElement();
		}

		private void WriteStringFormatElement(
			System.Xml.XmlWriter writer,
			string name, StringFormat sf)
		{
			writer.WriteStartElement(name);

			writer.WriteAttributeString("Alignment", sf.Alignment.ToString());
			writer.WriteAttributeString("LineAlignment", sf.LineAlignment.ToString());

			writer.WriteEndElement();
		}

		private void WriteRectangleFElement(
			System.Xml.XmlWriter writer,
			string name, RectangleF val)
		{
			writer.WriteStartElement(name);

			writer.WriteAttributeString("Left", XmlConvert.FromSingle(val.Left));
			writer.WriteAttributeString("Top", XmlConvert.FromSingle(val.Top));
			writer.WriteAttributeString("Right", XmlConvert.FromSingle(val.Right));
			writer.WriteAttributeString("Bottom", XmlConvert.FromSingle(val.Bottom));

			writer.WriteEndElement();
		}

		private void WriteRectangleElement(
			System.Xml.XmlWriter writer,
			string name, Rectangle val)
		{
			writer.WriteStartElement(name);

			writer.WriteAttributeString("Left", XmlConvert.FromInt32(val.Left));
			writer.WriteAttributeString("Top", XmlConvert.FromInt32(val.Top));
			writer.WriteAttributeString("Right", XmlConvert.FromInt32(val.Right));
			writer.WriteAttributeString("Bottom", XmlConvert.FromInt32(val.Bottom));

			writer.WriteEndElement();
		}

		private void WriteFloatArrayElement(System.Xml.XmlWriter writer,
			ICollection array, string element, string subElement)
		{
			writer.WriteStartElement(element);
			writer.WriteAttributeString("Count",
				XmlConvert.FromInt32(array.Count));
			foreach(float f in array)
			{
				writer.WriteElementString(
					subElement, XmlConvert.FromSingle(f));
			}
			writer.WriteEndElement();
		}

		private void WriteBrushRefElement(System.Xml.XmlWriter writer,
			FlowChartX.Brush brush, string element, SortedList brushes)
		{
			writer.WriteStartElement(element);
			if(brush != null)
			{
				int bi = brushes.IndexOfValue(brush);

				writer.WriteAttributeString("Id",
					XmlConvert.FromInt32((int)brushes.GetKey(bi)));
			}
			else
			{
				writer.WriteAttributeString("Id", "-1");
			}
			writer.WriteEndElement();
		}

		private void WriteBrushElement(System.Xml.XmlWriter writer,
			FlowChartX.Brush brush, SortedList brushes)
		{
			if(brush == null)
				return;

			int ix = brushes.IndexOfValue(brush);
			if(ix < 0)
				throw new Exception(
					"Brush not found in the brush list!");

			writer.WriteStartElement("Brush");
			writer.WriteAttributeString("Id",
				XmlConvert.FromInt32((int)brushes.GetKey(ix)));

			if(brush.GetType().Equals(typeof(FlowChartX.SolidBrush)))
			{
				FlowChartX.SolidBrush bb =
					(FlowChartX.SolidBrush)brush;

				writer.WriteAttributeString("Type", "Solid");

				writer.WriteElementString("Color",
					XmlConvert.FromColor(bb.Color));
			}
			else if(brush.GetType().Equals(typeof(FlowChartX.HatchBrush)))
			{
				FlowChartX.HatchBrush bb =
					(FlowChartX.HatchBrush)brush;

				writer.WriteAttributeString("Type", "Hatch");

				writer.WriteElementString("ForeColor",
					XmlConvert.FromColor(bb.ForegroundColor));
				writer.WriteElementString("BackColor",
					XmlConvert.FromColor(bb.BackgroundColor));
				writer.WriteElementString("Style",
					bb.HatchStyle.ToString());
			}
			else if(brush.GetType().Equals(typeof(FlowChartX.LinearGradientBrush)))
			{
				FlowChartX.LinearGradientBrush bb =
					(FlowChartX.LinearGradientBrush)brush;

				writer.WriteAttributeString("Type", "Gradient");

				// Write angle
				writer.WriteElementString("Angle",
					XmlConvert.FromSingle(bb.Angle));

				// Write blend pattern
				if(bb.Blend != null)
				{
					writer.WriteStartElement("Blend");

					WriteFloatArrayElement(writer, bb.Blend.Positions,
						"Positions", "Pos");
					WriteFloatArrayElement(writer, bb.Blend.Factors,
						"Factors", "Fac");

					writer.WriteEndElement();
				}

				// Write linear colors
				writer.WriteElementString("Color1",
					XmlConvert.FromColor(bb.LinearColors[0]));
				writer.WriteElementString("Color2",
					XmlConvert.FromColor(bb.LinearColors[1]));

				// Write interpolation colors
				if(bb.InterpolationColor != null)
				{
					writer.WriteStartElement("ColorBlend");

					WriteFloatArrayElement(writer, bb.InterpolationColor.Positions,
						"Positions", "Pos");
					writer.WriteStartElement("Colors");
					writer.WriteAttributeString("Count",
						XmlConvert.FromInt32(bb.InterpolationColor.Colors.Length));
					for(int j = 0; j < bb.InterpolationColor.Colors.Length; j++)
					{
						Color c = (Color)
							bb.InterpolationColor.Colors.GetValue(j);
						writer.WriteElementString("Color",
							XmlConvert.FromColor(c));
					}
					writer.WriteEndElement();

					writer.WriteEndElement();
				}
			}
			else if(brush.GetType().Equals(typeof(FlowChartX.TextureBrush)))
			{
				FlowChartX.TextureBrush bb =
					(FlowChartX.TextureBrush)brush;

				writer.WriteAttributeString("Type", "Texture");

				writer.WriteElementString("WrapMode",
					bb.WrapMode.ToString());
				writer.WriteElementString("Image",
					Version >= 4 ? XmlConvert.FromImageV4(bb.Image) :
						XmlConvert.FromImage(bb.Image));
			}

			writer.WriteEndElement();
		}

		private void WritePenElement(System.Xml.XmlWriter writer,
			FlowChartX.Pen pen, string element, SortedList brushes)
		{
			if(pen == null)
				return;

			writer.WriteStartElement(element);

			if(pen.Brush != null)
			{
				int bi = brushes.IndexOfValue(pen.Brush);

				writer.WriteStartElement("Brush");
				writer.WriteAttributeString("Id",
					XmlConvert.FromInt32((int)brushes.GetKey(bi)));
				writer.WriteEndElement();
			}

			writer.WriteElementString("Color",
				XmlConvert.FromColor(pen.Color));

			if(pen.CompoundArray != null)
			{
				WriteFloatArrayElement(writer, pen.CompoundArray,
					"CompoundArray", "Pos");
			}

			writer.WriteElementString("DashOffset",
				XmlConvert.FromSingle(pen.DashOffset));

			if(pen.DashPattern != null)
			{
				WriteFloatArrayElement(writer, pen.DashPattern,
					"DashPattern", "Pos");
			}

			writer.WriteElementString("DashStyle",
				pen.DashStyle.ToString());

			writer.WriteElementString("LineJoint",
				pen.LineJoin.ToString());

			writer.WriteElementString("MiterLimit",
				XmlConvert.FromSingle(pen.MiterLimit));

			writer.WriteElementString("Width",
				XmlConvert.FromSingle(pen.Width));

			writer.WriteEndElement();
		}

		private void WriteTagElement(System.Xml.XmlWriter writer,
			object obj)
		{
			PropertyDescriptorCollection pdc = 
				TypeDescriptor.GetProperties(obj);
			PropertyDescriptor pd = pdc.Find("Tag", true);
			object tag = pd.GetValue(obj);

			if (tag != null)
			{
				if(SerializeTag != null)
				{
					if (_options.CustomTagSerialization)
					{
						writer.WriteStartElement("Tag");
						SerializeTagArgs args = new SerializeTagArgs(obj, writer, null);
						SerializeTag(this, args);
						writer.WriteEndElement();
					}
					else
					{
						SerializeTagArgs args = new SerializeTagArgs(obj);
						SerializeTag(this, args);

						if (args.Representation != null)
							writer.WriteElementString("Tag",
								args.Representation);
					}
				}
			}
		}

		private void WriteConstraints(
			System.Xml.XmlWriter writer, Node node)
		{
			writer.WriteStartElement("Constraints");
			writer.WriteAttributeString("Count",
				XmlConvert.FromInt32(NodeConstraints.Count));

			writer.WriteStartElement("Constraint");
			writer.WriteAttributeString("Type", "0");
			writer.WriteElementString("Value",
				XmlConvert.FromInt32((int)node.Constraints.MoveDirection));
			writer.WriteEndElement();

			writer.WriteStartElement("Constraint");
			writer.WriteAttributeString("Type", "1");
			writer.WriteElementString("Value",
				XmlConvert.FromSingle(node.Constraints.MinWidth));
			writer.WriteEndElement();

			writer.WriteStartElement("Constraint");
			writer.WriteAttributeString("Type", "2");
			writer.WriteElementString("Value",
				XmlConvert.FromSingle(node.Constraints.MinHeight));
			writer.WriteEndElement();

			writer.WriteStartElement("Constraint");
			writer.WriteAttributeString("Type", "3");
			writer.WriteElementString("Value",
				XmlConvert.FromSingle(node.Constraints.MaxWidth));
			writer.WriteEndElement();

			writer.WriteStartElement("Constraint");
			writer.WriteAttributeString("Type", "4");
			writer.WriteElementString("Value",
				XmlConvert.FromSingle(node.Constraints.MaxHeight));
			writer.WriteEndElement();

			writer.WriteStartElement("Constraint");
			writer.WriteAttributeString("Type", "5");
			writer.WriteElementString("Value",
				XmlConvert.FromBoolean(node.Constraints.KeepInsideParent));
			writer.WriteEndElement();

			writer.WriteStartElement("Constraint");
			writer.WriteAttributeString("Type", "6");
			writer.WriteElementString("Value",
				XmlConvert.FromBoolean(node.Constraints.KeepRatio));
			writer.WriteEndElement();

			writer.WriteEndElement();
		}

		private void WriteAnchorPatternElement(string name,
			System.Xml.XmlWriter writer, AnchorPattern pattern)
		{
			writer.WriteStartElement(name);

			if (pattern == null)
			{
				// Write NoPattern constant
				writer.WriteAttributeString("RegIndex",
					XmlConvert.FromInt32(Constants.NoAnchorPattern));
			}
			else
			{
				// Write the pattern reg index
				writer.WriteAttributeString("RegIndex",
					XmlConvert.FromInt32(pattern.getRegIndex()));

				if (pattern.getRegIndex() == -1)
				{
					// Save the pattern points
					writer.WriteStartElement("Points");
					writer.WriteAttributeString("Count",
						XmlConvert.FromInt32(pattern.Points.Count));

					AnchorPoint point;
					for(int i = 0; i < pattern.Points.Count; i++)
					{
						point = pattern.Points[i];

						writer.WriteStartElement("AnchorPoint");

						// Write points properties
						WriteProperties(writer, point, new string[]
							{ "AllowIncoming", "AllowOutgoing",
								"Color", "Column", "MarkStyle", "ToolTip", "X", "Y" }, null);
						// Write tag
						WriteTagElement(writer, point);

						writer.WriteEndElement();
					}

					writer.WriteEndElement();
				}
			}

			writer.WriteEndElement();
		}

		private void WriteControlProperties(
			System.Xml.XmlWriter writer, System.Windows.Forms.Control control,
			BinaryFormatter fmt)
		{
			if (control == null)
				return;

			writer.WriteStartElement("Properties");

			System.Type type = control.GetType();
			PropertyInfo[] props = type.GetProperties();
			foreach (PropertyInfo prop in props)
			{
				// Will not serialize indexed properties
				ParameterInfo[] iparams = prop.GetIndexParameters();
				if (iparams == null || iparams.Length > 0)
					continue;
				if (!prop.CanRead || !prop.CanWrite)
					continue;

				object val = null;
				try
				{
					val = prop.GetValue(control, null);
				}
				catch
				{
					// Some properties cannot be read
					continue;
				}
				if (val == null)
					continue;
				if (!(val.GetType().IsSerializable))
					continue;

				MemoryStream temp = new MemoryStream();
				fmt.Serialize(temp, val);
				string toWrite = XmlConvert.FromStream(temp);
				writer.WriteElementString(prop.Name, toWrite.Length == 0 ? "-" : toWrite);
			}

			writer.WriteEndElement();
		}

		private void WriteProperties(
			System.Xml.XmlWriter writer, object component,
			string[] propNames, string categoryName)
		{
			PropertyDescriptorCollection pdc = 
				TypeDescriptor.GetProperties(component);
			PropertyDescriptor pd;

			if(categoryName != null)
				writer.WriteStartElement(categoryName);

			string prop = "";
			for (int i = 0; i < propNames.Length; i++)
			{
				prop = propNames[i];

				// Check if the name of this property has changed
				string realProp = prop;
				if (_propertyMap.Contains(realProp))
					realProp = _propertyMap[realProp] as string;

				// The property value
				object val = null;
				string tprop = prop;
				Type type = null;

				pd = pdc.Find(realProp, true);
				if (pd != null)
				{
					val = pd.GetValue(component);
					tprop = prop;
					type = pd.PropertyType;
				}
				else
				{
					// Check if it is not internal or private property
					PropertyInfo info = component.GetType().GetProperty(prop, 
						BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.GetProperty);

					// The property is missing
					if (info == null)
						continue;

					val = info.GetValue(component, null);
					tprop = prop;
					type = info.PropertyType;
				}

				if(type.Equals(typeof(Color)))
				{
					writer.WriteElementString(tprop,
						XmlConvert.FromColor((Color)val));
				}
				else if(type.Equals(typeof(Font)))
				{
					WriteFontElement(writer,
						tprop, (Font)val);
				}
				else if(type.Equals(typeof(StringFormat)))
				{
					WriteStringFormatElement(writer,
						tprop, (StringFormat)val);
				}
				else if(type.Equals(typeof(RectangleF)))
				{
					WriteRectangleFElement(writer,
						tprop, (RectangleF)val);
				}
				else if(type.Equals(typeof(Image)))
				{
					if(_options.WriteImages && val != null)
					{
						writer.WriteElementString(tprop,
							Version >= 4 ? XmlConvert.FromImageV4((Image)val) : 
							XmlConvert.FromImage((Image)val));
					}
				}
				else if(type.Equals(typeof(AnchorPattern)))
				{
					WriteAnchorPatternElement(tprop, writer, (AnchorPattern)val);
				}
				else
				{
					// Special case for FlowChart's tag
					if (prop == "Tag")
					{
						WriteTagElement(writer, component);
					}
					else if (type.Equals(typeof(float)))
					{
						writer.WriteElementString(tprop,
							XmlConvert.FromSingle((float)val));
					}
					else if (type.Equals(typeof(double)))
					{
						writer.WriteElementString(tprop,
							XmlConvert.FromDouble((double)val));
					}
					else if (type.Equals(typeof(Int16)))
					{
						writer.WriteElementString(tprop,
							XmlConvert.FromInt16((Int16)val));
					}
					else if (type.Equals(typeof(Int32)))
					{
						writer.WriteElementString(tprop,
							XmlConvert.FromInt32((Int32)val));
					}
					else if (type.Equals(typeof(Int64)))
					{
						writer.WriteElementString(tprop,
							XmlConvert.FromInt64((Int64)val));
					}
					else if (type.Equals(typeof(ShapeTemplate)))
					{
						// Only shapes with assigned ids are serialized
						writer.WriteElementString(tprop,
							(val as ShapeTemplate).ID);
					}
					else if (val != null)
					{
						writer.WriteElementString(tprop, val.ToString());
					}
				}
			}

			if(categoryName != null)
				writer.WriteEndElement();
		}


		public void Write(Stream stream)
		{
			XmlTextWriter writer = new XmlTextWriter(stream,
				System.Text.Encoding.Default);
			writer.Formatting = Formatting.Indented;

			Write(writer);
		}

		public void Write(string fileName)
		{
			XmlTextWriter writer = new XmlTextWriter(fileName,
				System.Text.Encoding.Default);
			writer.Formatting = Formatting.Indented;

			Write(writer);
			writer.Close();
		}

		private static int Version { get { return 10; } }

		public void Write(System.Xml.XmlWriter writer)
		{
			// Write the XML declaration
			if (_options.WriteXMLDecl)
				writer.WriteStartDocument(true);

			// Write header info
			writer.WriteComment("FlowChart.NET diagram");

			// Write the root element
			writer.WriteStartElement("Diagram");
			writer.WriteAttributeString("Version", Version.ToString());

			// Write brushes
			int id = 0;
			SortedList brushes = new SortedList();
			foreach(Brush b in Brush.Brushes)
			{
				brushes.Add(id, b); // Map brushes to ids
				id++;
			}

			writer.WriteStartElement("Brushes");
			writer.WriteAttributeString("Count",
				XmlConvert.FromInt32(brushes.Count));

			for(int i = 0; i < brushes.Count; i++)
			{
				FlowChartX.Brush b =
					(FlowChartX.Brush)brushes.GetValueList()[i];

				WriteBrushElement(writer, b, brushes);
			}

			// Close the Brushes element
			writer.WriteEndElement();

			// Write the environment information
			writer.WriteStartElement("Environment");

			// Write appearance information
			WriteProperties(writer, _diagram,
				_appearanceProps, "Appearance");

			// Write behaviour information
			WriteProperties(writer, _diagram,
				_behaviourProps, "Behaviour");

			// Write defaults information
			WriteProperties(writer, _diagram,
				_defaultProps, "Defaults");

			// Write default brush and pens
			writer.WriteStartElement("DefaultsGDI");

			WriteBrushRefElement(writer, _diagram.BoxBrush, "BoxBrush", brushes);
			WriteBrushRefElement(writer, _diagram.TableBrush, "TableBrush", brushes);
			WriteBrushRefElement(writer, _diagram.ArrowBrush, "ArrowBrush", brushes);
			WriteBrushRefElement(writer, _diagram.BackBrush, "BackBrush", brushes);
			WriteBrushRefElement(writer, _diagram.ExteriorBrush, "ExteriorBrush", brushes);
			WritePenElement(writer, _diagram.BoxPen, "BoxPen", brushes);
			WritePenElement(writer, _diagram.TablePen, "TablePen", brushes);
			WritePenElement(writer, _diagram.ArrowPen, "ArrowPen", brushes);

			writer.WriteEndElement();

			// Write grid information
			WriteProperties(writer, _diagram,
				_gridProps, "Grid");

			// Write layout information
			WriteProperties(writer, _diagram,
				_layoutProps, "Layout");

			// Write miscellaneous information
			WriteProperties(writer, _diagram,
				_miscProps, "Miscellaneous");

			// Close the environment element
			writer.WriteEndElement();

			// Write the box information
			writer.WriteStartElement("Boxes");
			writer.WriteAttributeString("Count",
				XmlConvert.FromInt32(_diagram.Boxes.Count));

			id = 0;
			SortedList sl = new SortedList();
			foreach(Box b in _diagram.Boxes)
			{
				writer.WriteStartElement("Box");
				writer.WriteAttributeString("Id", XmlConvert.FromInt32(id));
				writer.WriteAttributeString("ZIndex", XmlConvert.FromInt32(b.ZIndex));

				// Write the shape type
				string shape = "Invalid";
				switch(b.Style)
				{
					case BoxStyle.Delay:
						shape = "Delay";
						break;
					case BoxStyle.Ellipse:
						shape = "Ellipse";
						break;
					case BoxStyle.Rectangle:
						shape = "Rectangle";
						break;
					case BoxStyle.Rhombus:
						shape = "Decision";
						break;
					case BoxStyle.RoundedRectangle:
						shape = "RoundRectangle";
						break;
					case BoxStyle.Shape:
						// Only shapes with assigned ids are serialized
						shape = b.Shape.ID;

						if (shape == "")
						{
							// Possibly one of the old predefined shapes
							foreach (ShapeTemplate newShape in ShapeTemplate.Shapes)
							{
								if (b.Shape.Equals(newShape))
								{
									shape = newShape.ID;
									break;
								}
							}

							if (shape == "")
								shape = "Rectangle";
						}
						break;
				}
				writer.WriteElementString("Shape", shape);

				// Write brush index
				WriteBrushRefElement(writer, b.Brush, "Brush", brushes);
				// Write the pen
				WritePenElement(writer, b.Pen, "Pen", brushes);
				// Write the tag
				WriteTagElement(writer, b);

				// Write constraints
				WriteConstraints(writer, b);

				// Write properties
				WriteProperties(writer, b, _boxProps, null);

				writer.WriteEndElement();

				sl.Add(id, b); // map boxes to ids
				id++;
			}

			// Close boxes element
			writer.WriteEndElement();

			// Write the control host information
			writer.WriteStartElement("ControlHosts");
			writer.WriteAttributeString("Count",
				XmlConvert.FromInt32(_diagram.ControlHosts.Count));

			BinaryFormatter fmt = new BinaryFormatter();
			foreach (ControlHost h in _diagram.ControlHosts)
			{
				writer.WriteStartElement("Host");
				writer.WriteAttributeString("Id", XmlConvert.FromInt32(id));
				writer.WriteAttributeString("ZIndex", XmlConvert.FromInt32(h.ZIndex));
				if (h.Control != null)
				{
					System.Type type = h.Control.GetType();
					writer.WriteAttributeString("ControlAssembly", type.Assembly.GetName().Name);
					writer.WriteAttributeString("ControlType", type.FullName);

					// Find the z-index of the contained control
					int controlZIndex = 0;
					for (int i = 0; i < _diagram.Controls.Count; i++)
					{
						if (_diagram.Controls[i] == h.Control)
						{
							controlZIndex = i;
							break;
						}
					}
					writer.WriteAttributeString("ControlZIndex", controlZIndex.ToString());
				}
				else
				{
					writer.WriteAttributeString("ControlAssembly", "");
					writer.WriteAttributeString("ControlType", "");
					writer.WriteAttributeString("ControlZIndex", "0");
				}

				// Write brush index
				WriteBrushRefElement(writer, h.Brush, "Brush", brushes);
				// Write the pen
				WritePenElement(writer, h.Pen, "Pen", brushes);

				// Write control properties
				WriteControlProperties(writer, h.Control, fmt);
				// Write control host properties
				WriteProperties(writer, h, _hostProps, null);
				// Write the tag
				WriteTagElement(writer, h);

				// Write constraints
				WriteConstraints(writer, h);

				writer.WriteEndElement();

				sl.Add(id, h); // map hosts to ids
				id++;
			}

			writer.WriteEndElement();

			// Write table information
			writer.WriteStartElement("Tables");
			writer.WriteAttributeString("Count",
				XmlConvert.FromInt32(_diagram.Tables.Count));

			foreach(Table t in _diagram.Tables)
			{
				writer.WriteStartElement("Table");
				writer.WriteAttributeString("Id",
					XmlConvert.FromInt32(id));
				writer.WriteAttributeString("ZIndex",
					XmlConvert.FromInt32(t.ZIndex));
				writer.WriteAttributeString("Rows",
					XmlConvert.FromInt32(t.RowCount));
				writer.WriteAttributeString("Columns",
					XmlConvert.FromInt32(t.ColumnCount));

				// Write table data
				writer.WriteStartElement("Data");
				for(int r = 0; r < t.RowCount; r++)
				{
					for(int c = 0; c < t.ColumnCount; c++)
					{
						Table.Cell cell = t[c, r];

						writer.WriteStartElement("Cell");

						WriteBrushRefElement(writer, cell.Brush, "Brush",
							brushes);

						// Write the tag
						WriteTagElement(writer, cell);

						WriteProperties(writer, cell, 
							new string[] { "Text", "TextFormat",
								"Picture", "PicturePos", "TextColor",
								"HyperLink", "ToolTip", "RowSpan", "ColumnSpan", }, null);

						writer.WriteEndElement();
					}
				}
				writer.WriteEndElement();

				// Write row data
				writer.WriteStartElement("Rows");
				for(int r = 0; r < t.RowCount; r++)
				{
					writer.WriteStartElement("Row");
					WriteProperties(writer, t.Rows[r],
						new string[] { "Height", "AnchorPattern", "Header", "Expanded" }, null);
					writer.WriteEndElement();
				}
				writer.WriteEndElement();

				// Write column data
				writer.WriteStartElement("Columns");
				for(int c = 0; c < t.ColumnCount; c++)
				{
					writer.WriteStartElement("Column");
					WriteProperties(writer, t.Columns[c],
						new string[] { "ColumnStyle", "Width" }, null);
					writer.WriteEndElement();
				}
				writer.WriteEndElement();

				// Write brush index
				WriteBrushRefElement(writer, t.Brush, "Brush", brushes);
				// Write caption background brush
				WriteBrushRefElement(writer, t.CaptionBackBrush, "CaptionBackBrush", brushes);
				// Write the pen
				WritePenElement(writer, t.Pen, "Pen", brushes);
				// Write the tag
				WriteTagElement(writer, t);

				// Write constraints
				WriteConstraints(writer, t);

				// Write properties
				WriteProperties(writer, t, _tableProps, null);

				writer.WriteEndElement();

				sl.Add(id, t); // map tables to ids
				id++;
			}

			// Close tables element
			writer.WriteEndElement();

			// Write containers in v7
			writer.WriteStartElement("Containers");
			writer.WriteAttributeString("Count", "0");
			writer.WriteEndElement();

			// Write arrows data
			writer.WriteStartElement("Arrows");
			writer.WriteAttributeString("Count",
				XmlConvert.FromInt32(_diagram.Arrows.Count));

			int oi, di;
			int aid = id;
			foreach(Arrow a in _diagram.Arrows)
			{
				writer.WriteStartElement("Arrow");
				writer.WriteAttributeString("Id",
					XmlConvert.FromInt32(aid));
				writer.WriteAttributeString("ZIndex",
					XmlConvert.FromInt32(a.ZIndex));

				if (a.Origin != _diagram.Dummy)
				{
					oi = sl.IndexOfValue(a.Origin);
					if(oi < 0)
						throw new Exception("Unexpected arrow origin index");
					id = (int)sl.GetKey(oi);
				}
				else
				{
					id = -1;
				}

				writer.WriteAttributeString("From", XmlConvert.FromInt32(id));
				if(a.Origin is Table)
					writer.WriteAttributeString("RowFrom",
						XmlConvert.FromInt32(a.OrgnIndex));

				if (a.Destination != _diagram.Dummy)
				{
					di = sl.IndexOfValue(a.Destination);
					if(di < 0)
						throw new Exception("Unexpected arrow destination index");
					id = (int)sl.GetKey(di);
				}
				else
				{
					id = -1;
				}

				writer.WriteAttributeString("To", XmlConvert.FromInt32(id));
				if(a.Destination is Table)
					writer.WriteAttributeString("RowTo",
						XmlConvert.FromInt32(a.DestIndex));

				// Write control points
				writer.WriteStartElement("Data");
				foreach(PointF pt in a.ControlPoints)
				{
					writer.WriteStartElement("Point");

					writer.WriteAttributeString("X", XmlConvert.FromSingle(pt.X));
					writer.WriteAttributeString("Y", XmlConvert.FromSingle(pt.Y));

					writer.WriteEndElement();
				}
				writer.WriteEndElement();

				// Write brush index
				WriteBrushRefElement(writer, a.Brush, "Brush", brushes);
				// Write pen
				WritePenElement(writer, a.Pen, "Pen", brushes);
				// Write head pen
				WritePenElement(writer, a.HeadPen, "HeadPen", brushes);
				// Write the tag
				WriteTagElement(writer, a);

				// Write arrow properties
				WriteProperties(writer, a, _arrowProps, null);

				writer.WriteEndElement();

				sl.Add(aid, a); // map arrows to ids
				aid++;
			}

			// Close arrows element
			writer.WriteEndElement();

			// Write group objects
			writer.WriteStartElement("Groups");
			writer.WriteAttributeString("Count",
				XmlConvert.FromInt32(_diagram.Groups.Count));

			foreach(Group g in _diagram.Groups)
			{
				writer.WriteStartElement("Group");

				oi = sl.IndexOfValue(g.MainObject);
				if(oi < 0)
					throw new Exception("Unexpected group master index");
				id = (int)sl.GetKey(oi);

				// Write group's main object
				writer.WriteStartElement("MainObject");
				writer.WriteAttributeString("Id", XmlConvert.FromInt32(id));
				writer.WriteEndElement();

				// Write group's visibility
				writer.WriteElementString("Visible", g.Visible.ToString());

				// Write AutoDeleteItems flag
				writer.WriteElementString("AutoDeleteItems", g.AutoDeleteItems.ToString());

				// Write Expandable flag
				writer.WriteElementString("Expandable", g.Expandable.ToString());

				// Write FollowMasterRotation flag
				writer.WriteElementString("FollowMasterRotation", g.FollowMasterRotation.ToString());

				// Write group's tag
				WriteTagElement(writer, g);

				// Write attached objects
				writer.WriteStartElement("Attachments");
				writer.WriteAttributeString("Count",
					XmlConvert.FromInt32(g.Attachments.Count));
				foreach(Attachment at in g.Attachments)
				{
					writer.WriteStartElement("Attachment");

					writer.WriteElementString("Data",
						XmlConvert.FromInt32(at.attData));
					oi = sl.IndexOfValue(at.node);
					if(oi < 0)
						throw new Exception("Unexpected group attachee index");
					id = (int)sl.GetKey(oi);
					writer.WriteElementString("Object", XmlConvert.FromInt32(id));
					WriteRectangleFElement(writer, "Percents", at.percents);
					writer.WriteElementString("Type",
						at.type.ToString());

					writer.WriteEndElement();
				}
				writer.WriteEndElement();

				writer.WriteEndElement();
			}

			// Close groups element
			writer.WriteEndElement();


			// Close the diagram element
			writer.WriteEndElement();

			// Finish with this document
			if (_options.WriteXMLDecl)
				writer.WriteEndDocument();

			writer.Flush();
		}

		public event SerializeTagEvent SerializeTag;


		private Hashtable _propertyMap;
	}

	public class SerializeTagArgs : EventArgs
	{
		public SerializeTagArgs(object obj)
		{
			_object = obj;
			_representation = "";
			writer = null;
			reader = null;
		}

		internal SerializeTagArgs(object obj,
			System.Xml.XmlWriter writer, System.Xml.XmlReader reader)
		{
			_object = obj;
			_representation = "";
			this.writer = writer;
			this.reader = reader;
		}

		public object Object
		{
			get { return _object; }
		}

		public System.Xml.XmlWriter XmlWriter
		{
			get { return writer; }
		}

		public System.Xml.XmlReader XmlReader
		{
			get { return reader; }
		}

		public string Representation
		{
			get { return _representation; }
			set { _representation = value; }
		}

		private object _object;
		private string _representation;
		System.Xml.XmlWriter writer;
		System.Xml.XmlReader reader;
	}

	public delegate void SerializeTagEvent(object sender, SerializeTagArgs e);
}