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
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Collections;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;


namespace MindFusion.FlowChartX.Xml
{
	using FlowChartX = MindFusion.FlowChartX;

	/// <summary>
	/// An exception which is thrown when
	/// the format of the xml file does not
	/// correspond to the format expected.
	/// </summary>
	public class InvalidFormatException : Exception
	{
		public InvalidFormatException() :
			this("Unspecified error")
		{
		}

		public InvalidFormatException(string message) :
			base(message)
		{
		}
	}

	/// <summary>
	/// Reads the FlowChart diagram from a XML file.
	/// </summary>
	[ComVisible(true)]
	public class XmlReader
	{
		private FlowChart _diagram;

		public XmlReader(FlowChart diagram)
		{
			_diagram = diagram;

			// Create the property map, which
			// maps the old property names to the new property names
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
		}

		public void Read(Stream stream)
		{
			XmlTextReader reader = new XmlTextReader(stream);

			reader.WhitespaceHandling = WhitespaceHandling.None;
			Read(reader);
			reader.Close();
		}

		public void Read(string fileName)
		{
			Stream stream = File.OpenRead(fileName);

			Read(stream);

			stream.Close();
		}

		private void ReadMandatory(System.Xml.XmlReader reader,
			XmlNodeType expected, string exMessage)
		{
			if(!reader.Read())
				throw new InvalidFormatException(exMessage);
			if(reader.NodeType != expected)
				throw new InvalidFormatException(exMessage);
		}

		private void ReadMandatory(System.Xml.XmlReader reader,
			string expected, string exMessage)
		{
			if(!reader.Read())
				throw new InvalidFormatException(exMessage);
			if(reader.Name != expected)
				throw new InvalidFormatException(exMessage);
		}

		private Font ReadFontElement(System.Xml.XmlReader reader)
		{
			string tag;
			string name = "Arial";
			float size = 8f;
			GraphicsUnit unit = GraphicsUnit.Pixel;
			bool bold = false;
			bool italic = false;
			bool underline = false;
			bool strikeout = false;

			int depth = reader.Depth;
			while(true)
			{
				reader.Read();
				if(reader.Depth == depth)
					break;

				tag = reader.Name;

				// Read the value
				reader.Read();

				switch(tag)
				{
					case "Name":
						name = reader.Value;
						break;
					case "Size":
						size = XmlConvert.ToSingle(reader.Value);
						break;
					case "Unit":
						unit = (GraphicsUnit)XmlConvert.ToEnum(
							typeof(GraphicsUnit), reader.Value);
						break;
					case "Bold":
						bold = XmlConvert.ToBoolean(reader.Value.ToLower());
						break;
					case "Italic":
						italic = XmlConvert.ToBoolean(reader.Value.ToLower());
						break;
					case "Underline":
						underline = XmlConvert.ToBoolean(reader.Value.ToLower());
						break;
					case "Strikeout":
						strikeout = XmlConvert.ToBoolean(reader.Value.ToLower());
						break;
				}

				// Read the closing element
				reader.Read();
			}

			return new Font(name, size,
				(bold ? FontStyle.Bold : 0) |
				(italic ? FontStyle.Italic : 0) |
				(underline ? FontStyle.Underline : 0) |
				(strikeout ? FontStyle.Strikeout : 0),
				unit);
		}

		private float[] ReadFloatArrayElement(System.Xml.XmlReader reader)
		{
			float[] col = null;

			// Get the count
			int count = XmlConvert.ToInt32(reader.GetAttribute("Count"));
			col = new float[count];

			// Read the array's elements
			int depth = reader.Depth;
			for(int i = 0; i < count; i++)
			{
				// Read the starting element
				reader.Read();

				// Read the value
				reader.Read();

				col[i] = XmlConvert.ToSingle(reader.Value);

				// Read the ending element
				reader.Read();
			}

			// Read the closing element
			reader.Read();

			return col;
		}

		private Color[] ReadColorArrayElement(System.Xml.XmlReader reader)
		{
			Color[] col = null;

			// Get the count
			int count = XmlConvert.ToInt32(reader.GetAttribute("Count"));
			col = new Color[count];

			// Read the array's elements
			int depth = reader.Depth;
			for(int i = 0; i < count; i++)
			{
				// Read the starting element
				reader.Read();

				// Read the value
				reader.Read();

				col[i] = XmlConvert.ToColor(reader.Value);

				// Read the ending element
				reader.Read();
			}

			// Read the closing element
			reader.Read();

			return col;
		}

		private FlowChartX.Pen ReadPenElement(System.Xml.XmlReader reader,
			SortedList brushes)
		{
			int d;
			FlowChartX.Pen result = new FlowChartX.Pen(Color.Black);

			if(reader.IsEmptyElement)
				return result;

			d = reader.Depth;

			while(true)
			{
				// Read the starting tag
				reader.Read();
				if(reader.Depth == d)
					break;

				switch(reader.Name)
				{
					case "Brush":
						result.Brush = (FlowChartX.Brush)
							brushes[XmlConvert.ToInt32(reader.GetAttribute("Id"))];
						break;
					case "Color":
						// Read the value
						reader.Read();

						result.Color = XmlConvert.ToColor(reader.Value);

						// Read the closing element
						reader.Read();
						break;
					case "CompoundArray":
						result.CompoundArray = ReadFloatArrayElement(reader);
						break;
					case "DashOffset":
						// Read the value
						reader.Read();

						result.DashOffset = XmlConvert.ToSingle(reader.Value);

						// Read the closing element
						reader.Read();
						break;
					case "DashPattern":
						result.DashPattern = ReadFloatArrayElement(reader);
						break;
					case "DashStyle":
						// Read the value
						reader.Read();

						result.DashStyle = (DashStyle)XmlConvert.ToEnum(
							typeof(DashStyle), reader.Value);

						// Read the closing element
						reader.Read();
						break;
					case "LineJoint":
						// Read the value
						reader.Read();

						result.LineJoin = (LineJoin)XmlConvert.ToEnum(
							typeof(LineJoin), reader.Value);

						// Read the closing element
						reader.Read();
						break;
					case "MiterLimit":
						// Read the value
						reader.Read();

						result.MiterLimit = XmlConvert.ToSingle(reader.Value);

						// Read the closing element
						reader.Read();
						break;
					case "Width":
						// Read the value
						reader.Read();

						result.Width = XmlConvert.ToSingle(reader.Value);

						// Read the closing element
						reader.Read();
						break;
				}
			}

			return result;
		}

		private AnchorPattern ReadAnchorPatternElement(
			System.Xml.XmlReader reader)
		{
			int regIndex = XmlConvert.ToInt32(reader.GetAttribute("RegIndex"));
			AnchorPattern pattern = null;

			if (regIndex == Constants.NoAnchorPattern)
				pattern = null;

			if (regIndex != -1)
				pattern = AnchorPattern.getRegisteredPattern(regIndex);

			// Load the pattern
			if (regIndex == -1)
			{
				AnchorPointCollection points = new AnchorPointCollection();

				reader.Read();
				int count = XmlConvert.ToInt32(reader.GetAttribute("Count"));

				for (int i = 0; i < count; i++)
				{
					AnchorPoint point = new AnchorPoint(0, 0);

					reader.Read();
					ReadProperties(reader, point, reader.Depth + 1);

					points.Add(point);
				}

				// Points closing tag
				if (count > 0)
					reader.Read();

				// Anchor pattern closing tag
				reader.Read();

				pattern = new AnchorPattern(points);
			}

			return pattern;
		}

		private void ReadControlProperties(
			System.Xml.XmlReader reader, System.Windows.Forms.Control control,
			BinaryFormatter fmt)
		{
			reader.Read(); // Properties tag

			if (reader.IsEmptyElement)
				return;

			System.Type type = control.GetType();
			int depth = reader.Depth;
			string propName;
			while (true)
			{
				reader.Read(); // Property name
				propName = reader.Name;
				if (reader.Depth == depth)
					break;

				reader.Read(); // Property value
				string val = reader.Value;

				reader.Read(); // Closing tag

				try
				{
					PropertyInfo info = (PropertyInfo)type.GetProperty(propName);
					Stream stream = XmlConvert.ToStream(val);
					object v = fmt.Deserialize(stream);
					info.SetValue(control, v, null);
				}
				catch
				{
				}
			}
		}

		string[] _constraints = new string[]
			{
				"MoveDirection",
				"MinWidth",
				"MinHeight",
				"MaxWidth",
				"MaxHeight"
			};

		private void ReadNodeConstraints(
			System.Xml.XmlReader reader, Node node)
		{
			int count = XmlConvert.ToInt32(reader.GetAttribute("Count"));
			NodeConstraints c = node.Constraints;

			for (int i = 0; i < count; i++)
			{
				// Read <constraint>
				reader.Read();

				int type = XmlConvert.ToInt32(reader.GetAttribute("Type"));
				string prop = _constraints[type];
				PropertyInfo pd = c.GetType().GetProperty(prop);

				// Read <value>
				reader.Read();

				// Read value
				reader.Read();
				if (pd.PropertyType.Equals(typeof(DirectionConstraint)))
					pd.SetValue(c, XmlConvert.ToEnum(pd.PropertyType, reader.Value), null);
				else if (pd.PropertyType.Equals(typeof(Single)))
					pd.SetValue(c, XmlConvert.ToSingle(reader.Value), null);

				// Read </value>
				reader.Read();

				// Read </constraint>
				reader.Read();
			}

			// Read </constraints>
			reader.Read();
		}

		private bool ReadProperties(System.Xml.XmlReader reader,
			object component, int depth)
		{
			Type componentType = component.GetType();

			while (reader.Read())
			{
				if (reader.Depth != depth)
					return true;

				string propertyName = reader.Name;

				if (_propertyMap.Contains(propertyName))
					propertyName = _propertyMap[propertyName] as string;

				PropertyInfo info = componentType.GetProperty(propertyName, 
					BindingFlags.Public | BindingFlags.NonPublic |
					BindingFlags.Instance | BindingFlags.SetProperty);

				if (info != null && info.CanWrite)
				{
					Type type = info.PropertyType;

					if (type.Equals(typeof(Color)))
					{
						if(reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						info.SetValue(component,
							XmlConvert.ToColor(reader.Value), null);

						// Read the closing tag
						reader.Read();
					}

					else if (type.Equals(typeof(Font)))
					{
						if(reader.IsEmptyElement)
							continue;

						Font f = ReadFontElement(reader);
						info.SetValue(component, f, null);
					}

					else if (type.Equals(typeof(StringFormat)))
					{
						string sal = reader.GetAttribute("Alignment");
						string lal = reader.GetAttribute("LineAlignment");
						StringAlignment sa = (StringAlignment)XmlConvert.ToEnum(
							typeof(StringAlignment), sal);
						StringAlignment la = (StringAlignment)XmlConvert.ToEnum(
							typeof(StringAlignment), lal);
						StringFormat sf = new StringFormat();
						sf.Alignment = sa;
						sf.LineAlignment = la;
						info.SetValue(component, sf, null);
					}

					else if (type.IsEnum)
					{
						if(reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						object o = XmlConvert.ToEnum(type, reader.Value);
						if (o != null)
							info.SetValue(component, o, null);

						// Read the closing element
						reader.Read();
					}

					else if (type.Equals(typeof(Image)))
					{
						if(reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						Image img = _version >= 4 ? XmlConvert.ToImageV4(reader.Value) :
							XmlConvert.ToImage(reader.Value);
						info.SetValue(component, img, null);

						// Read the closing tag
						reader.Read();
					}

					else if (type.Equals(typeof(bool)))
					{
						if(reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						bool b = XmlConvert.ToBoolean(reader.Value.ToLower());
						info.SetValue(component, b, null);

						// Read the closing tag
						reader.Read();
					}

					else if (type.Equals(typeof(Single)))
					{
						if(reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						Single s = XmlConvert.ToSingle(reader.Value);
						info.SetValue(component, s, null);

						// Read the closing tag
						reader.Read();
					}

					else if (type.Equals(typeof(string)))
					{
						if(reader.IsEmptyElement)
						{
							info.SetValue(component, "", null);
							continue;
						}

						// Read the value
						reader.Read();

						info.SetValue(component, reader.Value, null);

						// Read the closing tag
						if(reader.NodeType != XmlNodeType.EndElement)
							reader.Read();
					}

					else if (type.Equals(typeof(Int16)))
					{
						if(reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						Int16 val = XmlConvert.ToInt16(reader.Value);
						info.SetValue(component, val, null);

						// Read the closing tag
						reader.Read();
					}

					else if (type.Equals(typeof(Int32)))
					{
						if(reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						Int32 i = XmlConvert.ToInt32(reader.Value);
						info.SetValue(component, i, null);

						// Read the closing tag
						reader.Read();
					}

					else if (type.Equals(typeof(Int64)))
					{
						if(reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						Int64 i = XmlConvert.ToInt64(reader.Value);
						info.SetValue(component, i, null);

						// Read the closing tag
						reader.Read();
					}

					else if (type.Equals(typeof(RectangleF)))
					{
						float l, t, r, b;
						l = XmlConvert.ToSingle(reader.GetAttribute("Left"));
						t = XmlConvert.ToSingle(reader.GetAttribute("Top"));
						r = XmlConvert.ToSingle(reader.GetAttribute("Right"));
						b = XmlConvert.ToSingle(reader.GetAttribute("Bottom"));
						info.SetValue(component, new RectangleF(l, t, r - l, b - t), null);
					}

					else if (type.Equals(typeof(AnchorPattern)))
					{
						AnchorPattern pattern = ReadAnchorPatternElement(reader);
						if (pattern != null)
							info.SetValue(component, pattern, null);
					}

					else if (type.Equals(typeof(NodeConstraints)))
					{
						ReadNodeConstraints(reader, component as Node);
					}

					else
					{
						if (reader.IsEmptyElement)
							continue;

						// Read the value
						reader.Read();

						if (info.Name == "Tag")
						{
							// Read the tag
							if (DeserializeTag != null)
							{
								if (component.GetType().Equals(typeof(Box)) ||
									component.GetType().Equals(typeof(Table)) ||
									component.GetType().Equals(typeof(ControlHost)) ||
									component.GetType().Equals(typeof(Arrow)) ||
									component.GetType().Equals(typeof(AnchorPoint)) ||
									component.GetType().Equals(typeof(FlowChart)) ||
									component.GetType().Equals(typeof(Table.Cell)))
								{
									SerializeTagArgs args = new SerializeTagArgs(component);
									args.Representation = reader.Value;
									DeserializeTag(this, args);
								}
							}
						}

						// Read the closing tag
						reader.Read();
					}
				}
				else
				{
					if (!reader.IsEmptyElement)
					{
						int d = reader.Depth;
						while (true)
						{
							reader.Read();
							if(d == reader.Depth)
								break;
						}
					}
				}
			}

			return false;
		}

		private int _version;

		public void Read(System.Xml.XmlReader reader)
		{
			// Disable auto-routing for a while
			_diagram.DontRouteForAwhile = true;

			_diagram.ClearAll();

			// Read while <Diagram> reached
			while (reader.Read())
				if (reader.Name == "Diagram")
					break;

			if (reader.Name != "Diagram")
				throw new InvalidFormatException("Invalid FlowChart document");

			// Check version
			_version = 1;
			if (reader.HasAttributes)
				_version = XmlConvert.ToInt32(reader.GetAttribute("Version"));

			// Read the brushes
			SortedList brushes = new SortedList();
			Brush brush;
			int count;
			int i;
			int id = 0;
			string type;
			int d;

			ReadMandatory(reader, "Brushes",
				"Brushes element missing or invalid");
			count = XmlConvert.ToInt32(
				reader.GetAttribute("Count"));
			for(i = 0; i < count; i++)
			{
				ReadMandatory(reader, "Brush",
					"Unrecognized brush token");

				id = XmlConvert.ToInt32(
					reader.GetAttribute("Id"));
				type = reader.GetAttribute("Type");

				brush = null;

				switch(type)
				{
					case "Solid":
					{
						d = reader.Depth;
						string name;
						while(true)
						{
							// Read element
							reader.Read();
							if(reader.Depth == d)
								break;

							name = reader.Name;
							// Read the value
							reader.Read();

							if(name == "Color")
							{
								Color c = XmlConvert.ToColor(reader.Value);
								brush = new FlowChartX.SolidBrush(c);
							}

							// Read the closing element
							reader.Read();
						}
					}
						break;
					case "Gradient":
					{
						d = reader.Depth;
						float angle = 0;
						Blend blend = null;
						Color c1 = Color.Black, c2 = Color.White;
						ColorBlend cblend = null;
						while(true)
						{
							// Read element
							reader.Read();
							if(reader.Depth == d)
								break;

							switch (reader.Name)
							{
								case "Angle":
									// Read the value
									reader.Read();
									angle = XmlConvert.ToSingle(reader.Value);
									break;
								case "Blend":
								{
									// Read positions
									reader.Read();
									float[] pos = ReadFloatArrayElement(reader);
									// Read factors
									reader.Read();
									float[] fac = ReadFloatArrayElement(reader);

									if(pos.Length != fac.Length)
										throw new InvalidFormatException(
											"Factors and positions in a gradient brush " +
											"have different lengths");

									blend = new Blend();
									blend.Positions = pos;
									blend.Factors = fac;
								}
									break;
								case "Color1":
									// Read the value
									reader.Read();
									c1 = XmlConvert.ToColor(reader.Value);
									break;
								case "Color2":
									// Read the value
									reader.Read();
									c2 = XmlConvert.ToColor(reader.Value);
									break;
								case "ColorBlend":
								{
									// Read positions
									reader.Read();
									float[] pos = ReadFloatArrayElement(reader);
									// Read colors
									reader.Read();
									Color[] colors = ReadColorArrayElement(reader);

									cblend = new ColorBlend();
									cblend.Positions = pos;
									cblend.Colors = colors;
								}
									break;
							}

							// Read the closing element
							reader.Read();
						}

						brush = new FlowChartX.LinearGradientBrush(c1, c2);
						((LinearGradientBrush)brush).Angle = angle;
						((LinearGradientBrush)brush).Blend = blend;
						((LinearGradientBrush)brush).InterpolationColor = cblend;
					}
						break;
					case "Texture":
					{
						d = reader.Depth;
						string name;
						WrapMode wm = WrapMode.Tile;
						Image img = null;
						while(true)
						{
							// Read element
							reader.Read();
							if(reader.Depth == d)
								break;

							name = reader.Name;
							// Read the value
							reader.Read();

							switch(name)
							{
								case "WrapMode":
									wm = (WrapMode)XmlConvert.ToEnum(
										typeof(WrapMode), reader.Value);
									break;
								case "Image":
									img = _version >= 4 ? XmlConvert.ToImageV4(reader.Value) :
										XmlConvert.ToImage(reader.Value);
									break;
							}

							// Read the closing element
							reader.Read();

							if (img != null)
								brush = new FlowChartX.TextureBrush(img, wm);
						}
					}
						break;
					case "Hatch":
					{
						d = reader.Depth;
						string name;
						Color fore = Color.Black, back = Color.White;
						HatchStyle style = HatchStyle.ForwardDiagonal;
						while(true)
						{
							// Read element
							reader.Read();
							if(reader.Depth == d)
								break;

							name = reader.Name;
							// Read the value
							reader.Read();

							switch(name)
							{
								case "ForeColor":
									fore = XmlConvert.ToColor(reader.Value);
									break;
								case "BackColor":
									back = XmlConvert.ToColor(reader.Value);
									break;
								case "Style":
									style = (HatchStyle)XmlConvert.ToEnum(
										typeof(HatchStyle), reader.Value);
									break;
							}

							// Read the closing element
							reader.Read();

							brush = new FlowChartX.HatchBrush(
								style, fore, back);
						}
					}
						break;
				}

				if(!brushes.Contains(id) && brush != null)
					brushes.Add(id, brush);
			}

			// Read the brushes closing element
			if(count > 0)
				reader.Read();

			ReadMandatory(reader, "Environment",
				"Environment element missing or invalid");

			// Read all appearance properties
			ReadMandatory(reader, "Appearance",
				"Appearance element missing or invalid");
			if(!ReadProperties(reader, _diagram, reader.Depth + 1))
				throw new InvalidFormatException("Unexpected " +
					"EOF in the Appearance section");

			// Read all bahaviour properties
			ReadMandatory(reader, "Behaviour",
				"Behaviour element missing or invalid");
			if(!ReadProperties(reader, _diagram, reader.Depth + 1))
				throw new InvalidFormatException("Unexpected " +
					"EOF in the Behaviour section");

			// Read all default properties
			ReadMandatory(reader, "Defaults",
				"Defaults element missing or invalid");
			if(!ReadProperties(reader, _diagram, reader.Depth + 1))
				throw new InvalidFormatException("Unexpected " +
					"EOF in the Defaults section");

			// Read default brushes and pens
			ReadMandatory(reader, "DefaultsGDI",
				"DefaultsGDI element missing or invalid");
			d = reader.Depth;
			while(true)
			{
				// Read starting tag
				reader.Read();
				if(reader.Depth == d)
					break;

				switch (reader.Name)
				{

					case "BoxBrush":
						id = XmlConvert.ToInt32(
							reader.GetAttribute("Id"));
						_diagram.BoxBrush = (FlowChartX.Brush)brushes[id];
						break;

					case "TableBrush":
						id = XmlConvert.ToInt32(
							reader.GetAttribute("Id"));
						_diagram.TableBrush = (FlowChartX.Brush)brushes[id];
						break;

					case "ArrowBrush":
						id = XmlConvert.ToInt32(
							reader.GetAttribute("Id"));
						_diagram.ArrowBrush = (FlowChartX.Brush)brushes[id];
						break;

					case "BackBrush":
						id = XmlConvert.ToInt32(
							reader.GetAttribute("Id"));
						_diagram.BackBrush = (FlowChartX.Brush)brushes[id];
						break;

					case "ExteriorBrush":
						id = XmlConvert.ToInt32(
							reader.GetAttribute("Id"));
						if (id == -1)
							_diagram.ExteriorBrush = null;
						else
							_diagram.ExteriorBrush = (FlowChartX.Brush)brushes[id];
						break;

					case "BoxPen":
						_diagram.BoxPen.Brush = null; // force release
						_diagram.BoxPen = (FlowChartX.Pen)
							ReadPenElement(reader, brushes);
						break;

					case "TablePen":
						_diagram.TablePen.Brush = null; // force release
						_diagram.TablePen = (FlowChartX.Pen)
							ReadPenElement(reader, brushes);
						break;

					case "ArrowPen":
						_diagram.ArrowPen.Brush = null; // force release
						_diagram.ArrowPen = (FlowChartX.Pen)
							ReadPenElement(reader, brushes);
						break;

				}
			}

			// Read all grid properties
			ReadMandatory(reader, "Grid",
				"Grid element missing or invalid");
			if(!ReadProperties(reader, _diagram, reader.Depth + 1))
				throw new InvalidFormatException("Unexpected " +
					"EOF in the Grid section");

			// Read all layout properties
			ReadMandatory(reader, "Layout",
				"Layout element missing or invalid");
			if(!ReadProperties(reader, _diagram, reader.Depth + 1))
				throw new InvalidFormatException("Unexpected " +
					"EOF in the Layout section");

			// Read all miscellaneous properties
			ReadMandatory(reader, "Miscellaneous",
				"Miscellaneous element missing or invalid");
			if(!ReadProperties(reader, _diagram, reader.Depth + 1))
				throw new InvalidFormatException("Unexpected " +
					"EOF in the Miscellaneous section");

			// Read the Environment closing element
			reader.Read();


			// Proceed to diagram objects //
			Box b;
			ControlHost h;
			Arrow a;
			Table t;
			Group g;
			object from, to;
			PointCollection points = new PointCollection(0);
			int idFrom, idTo, idArrow;
			int rowFrom, rowTo;
			int row, col;
			SortedList objects = new SortedList();
			SortedList zOrder = new SortedList();
			SortedList controlZOrder = new SortedList();
			int zIndex;
			id = 0;

			// Read boxes info
			ReadMandatory(reader, "Boxes",
				"Boxes element missing or invalid");
			count = XmlConvert.ToInt32(reader.GetAttribute("Count"));
			int brushId = -1;
			FlowChartX.Pen pen = null;
			FlowChartX.Pen headPen = null;
			for(i = 0; i < count; i++)
			{
				// Read the starting element
				ReadMandatory(reader, "Box",
					"Unrecognized box token");

				id = XmlConvert.ToInt32(reader.GetAttribute("Id"));
				zIndex = XmlConvert.ToInt32(reader.GetAttribute("ZIndex"));
				b = _diagram.CreateBox(0, 0, 1, 1);
				objects.Add(id, b);
				zOrder.Add(zIndex, b);

				// Read the shape
				string shape;
				reader.Read();
				if(!reader.IsEmptyElement)
				{
					reader.Read();
					shape = reader.Value;
					reader.Read();
				}
				else
				{
					shape = "Rectangle";
				}

				BoxStyle style = BoxStyle.Rectangle;
				switch(shape)
				{
					case "RoundRectangle":
						style = BoxStyle.RoundedRectangle;
						break;
					default:
						// Assume it is a complex shape
						style = BoxStyle.Shape;
						break;
				}
				b.Style = style;
				if(style == BoxStyle.Shape)
					b.Shape = ShapeTemplate.FromId(shape);

				// Read brush
				reader.Read();
				if (reader.GetAttribute("Id") != null)
					brushId = XmlConvert.ToInt32(reader.GetAttribute("Id"));
				else
					brushId = -1;
				// Read the pen
				reader.Read();
				pen = ReadPenElement(reader, brushes);

				if(!ReadProperties(reader, b, reader.Depth))
					throw new InvalidFormatException(
						"Unexpected EOF while parsing box info. Box: " +
						i.ToString());

				// Set brush and pen
				if(brushId != -1)
					b.Brush = (FlowChartX.Brush)brushes[brushId];
				if(pen != null)
					b.Pen = pen;
			}

			// Read boxes closing element
			if(count > 0)
				reader.Read();

			// Read control hosts
			if (_version >= 3)
			{
				string assemblyName;
				string typeName;
				ReadMandatory(reader, "ControlHosts",
					"ControlHosts element missing or invalid");
				count = XmlConvert.ToInt32(reader.GetAttribute("Count"));

				for (i = 0; i < count; i++)
				{
					// Read the host element
					ReadMandatory(reader, "Host",
						"Host element missing or invalid");

					id = XmlConvert.ToInt32(reader.GetAttribute("Id"));
					zIndex = XmlConvert.ToInt32(reader.GetAttribute("ZIndex"));
					assemblyName = reader.GetAttribute("ControlAssembly");
					typeName = reader.GetAttribute("ControlType");
					int controlZIndex = XmlConvert.ToInt32(reader.GetAttribute("ControlZIndex"));

					// Create the control host and add it to the diagram
					h = _diagram.CreateControlHost(0, 0, 1, 1);
					objects.Add(id, h);
					zOrder.Add(zIndex, h);
					controlZOrder.Add(controlZIndex, h.Control);

					// Read brush
					reader.Read();
					if(reader.GetAttribute("Id") != null)
						brushId = XmlConvert.ToInt32(reader.GetAttribute("Id"));
					else
						brushId = -1;
					// Read the pen
					reader.Read();
					pen = ReadPenElement(reader, brushes);

					if (typeName != "" && assemblyName != "")
					{
						System.Type controlType = Utilities.getLoadedType(typeName, assemblyName);
						if (controlType == null)
							throw new FileLoadException("Cannot load hosted control of type " + typeName);
						ConstructorInfo ctorInfo = controlType.GetConstructor(System.Type.EmptyTypes);
						// Instantiate
						h.Control = (System.Windows.Forms.Control)ctorInfo.Invoke(null);

						// Read control properties
						BinaryFormatter fmt = new BinaryFormatter();
						fmt.Binder = new DeserializationHack();
						ReadControlProperties(reader, h.Control, fmt);
					}
					else
					{
						h.Control = null;
					}

					// Read properties
					if (!ReadProperties(reader, h, reader.Depth))
						throw new InvalidFormatException(
							"Unexpected EOF while parsing control host info. ControlHost: " +
							i.ToString());

					// Set brush and pen
					if (brushId != -1)
						h.Brush = (FlowChartX.Brush)brushes[brushId];
					if (pen != null)
						h.Pen = pen;
				}

				// Update the z-indices of the contained controls
				foreach (System.Windows.Forms.Control control in controlZOrder.Values)
					control.BringToFront();

				// Read control hosts closing element
				if(count > 0)
					reader.Read();
			}

			// Read tables info
			ReadMandatory(reader, "Tables",
				"Tables element missing or invalid");
			count = XmlConvert.ToInt32(reader.GetAttribute("Count"));
			for(i = 0; i < count; i++)
			{
				// Read the table element
				ReadMandatory(reader, "Table",
					"Unrecognized table token");

				id = XmlConvert.ToInt32(reader.GetAttribute("Id"));
				zIndex = XmlConvert.ToInt32(reader.GetAttribute("ZIndex"));
				t = _diagram.CreateTable(0, 0, 1, 1);
				objects.Add(id, t);
				zOrder.Add(zIndex, t);

				t.RowCount = XmlConvert.ToInt32(reader.GetAttribute("Rows"));
				t.ColumnCount = XmlConvert.ToInt32(reader.GetAttribute("Columns"));

				// Read cell data
				ReadMandatory(reader, "Data",
					"Data element missing or invalid");
				d = reader.Depth;
				row = 0;
				col = 0;
				if(!reader.IsEmptyElement)
				{
					while(true)
					{
						reader.Read();
						if(d == reader.Depth)
							break;

						// Read brush in V5
						if (_version >= 5)
						{
							reader.Read();
							int bid = -1;
							if (reader.GetAttribute("Id") != null)
								bid = XmlConvert.ToInt32(reader.GetAttribute("Id"));
							if (bid != -1)
								t[col, row].Brush = (FlowChartX.Brush)brushes[bid];
							else
								t[col, row].Brush = null;

							ReadProperties(reader, t[col, row], reader.Depth);
						}
						else
						{
							ReadProperties(reader, t[col, row], reader.Depth + 1);
						}

						col++;
						if(col >= t.ColumnCount)
						{
							col = 0;
							row++;
						}
					}
				}

				// Read row data
				ReadMandatory(reader, "Rows",
					"Rows element missing or invalid");
				d = reader.Depth;
				row = 0;
				if(!reader.IsEmptyElement)
				{
					while(true)
					{
						reader.Read();
						if(d == reader.Depth)
							break;

						ReadProperties(reader, t.Rows[row], reader.Depth + 1);
						row++;
					}
				}

				// Read column data
				ReadMandatory(reader, "Columns",
					"Columns element missing or invalid");
				d = reader.Depth;
				col = 0;
				if(!reader.IsEmptyElement)
				{
					while(true)
					{
						reader.Read();
						if(d == reader.Depth)
							break;

						ReadProperties(reader, t.Columns[col], reader.Depth + 1);
						col++;
					}
				}

				// Read brush
				reader.Read();
				if (reader.GetAttribute("Id") != null)
					brushId = XmlConvert.ToInt32(reader.GetAttribute("Id"));
				else
					brushId = -1;

				// Read the caption brush
				int captionBrushId = -1;
				if (_version >= 10)
				{
					reader.Read();
					if (reader.GetAttribute("Id") != null)
						captionBrushId = XmlConvert.ToInt32(reader.GetAttribute("Id"));
					else
						captionBrushId = -1;
				}

				// Read the pen
				reader.Read();
				pen = ReadPenElement(reader, brushes);

				// Read table properties
				if(!ReadProperties(reader, t, reader.Depth))
					throw new InvalidFormatException(
						"Unexpected EOF while parsing table info. Table: " +
						i.ToString());

				// Set brush and pen
				if (brushId != -1)
					t.Brush = (FlowChartX.Brush)brushes[brushId];
				if (captionBrushId != -1)
					t.CaptionBackBrush = (FlowChartX.Brush)brushes[captionBrushId];
				if (pen != null)
					t.Pen = pen;
			}

			// Read tables closing element
			if(count > 0)
				reader.Read();

			if (_version >= 7)
			{
				ReadMandatory(reader, "Containers",
					"Containers element missing or invalid");

				int ccount = XmlConvert.ToInt32(reader.GetAttribute("Count"));

				if (ccount > 0)
				{
					int cdepth = reader.Depth;
					while (true)
					{
						reader.Read();
						if (reader.Depth == cdepth)
							break;
					}
				}
			}

			// Read arrows info
			ReadMandatory(reader, "Arrows",
				"Arrows element missing or invalid");
			count = XmlConvert.ToInt32(reader.GetAttribute("Count"));
			for(i = 0; i < count; i++)
			{
				// Read the arrow element
				ReadMandatory(reader, "Arrow",
					"Unrecognized arrow token");

				// Read the origin and destination indices
				idFrom = XmlConvert.ToInt32(reader.GetAttribute("From"));
				idTo = XmlConvert.ToInt32(reader.GetAttribute("To"));
				idArrow = XmlConvert.ToInt32(reader.GetAttribute("Id"));
				zIndex = XmlConvert.ToInt32(reader.GetAttribute("ZIndex"));

				try
				{
					rowFrom = XmlConvert.ToInt32(reader.GetAttribute("RowFrom"));
				}
				catch
				{
					rowFrom = -1;
				}
				try
				{
					rowTo = XmlConvert.ToInt32(reader.GetAttribute("RowTo"));
				}
				catch
				{
					rowTo = -1;
				}

				if (idTo == -1 || idFrom == -1)
				{
					if (idTo == -1 && idFrom != -1)
					{
						from = objects[idFrom];
						a = _diagram.CreateArrow((Node)from, PointF.Empty);
					}
					else if (idTo != -1 && idFrom == -1)
					{
						to = objects[idTo];
						a = _diagram.CreateArrow(PointF.Empty, (Node)to);
					}
					else
					{
						a = _diagram.CreateArrow(PointF.Empty, PointF.Empty);
					}
				}
				else
				{
					from = objects[idFrom];
					to = objects[idTo];
					if(rowFrom == -1 && rowTo == -1)
					{
						a = _diagram.CreateArrow((Node)from, (Node)to);
					}
					else if(rowFrom != -1 && rowTo == -1)
					{
						a = _diagram.CreateArrow((Table)from, rowFrom, (Node)to);
					}
					else if(rowFrom == -1 && rowTo != -1)
					{
						a = _diagram.CreateArrow((Node)from, (Table)to, rowTo);
					}
					else
					{
						a = _diagram.CreateArrow((Table)from, rowFrom,
							(Table)to, rowTo);
					}
				}

				// Read the control points
				ReadMandatory(reader, "Data",
					"Data element missing or invalid");
				d = reader.Depth;
				float x, y;
				points.Clear();
				while(true)
				{
					// Read the point
					reader.Read();

					if(reader.Depth == d)
						break;

					x = XmlConvert.ToSingle(reader.GetAttribute("X"));
					y = XmlConvert.ToSingle(reader.GetAttribute("Y"));

					points.Add(new PointF(x, y));
				}

				// Read brush
				reader.Read();
				if(reader.GetAttribute("Id") != null)
					brushId = XmlConvert.ToInt32(reader.GetAttribute("Id"));
				else
					brushId = -1;
				// Read the pen
				reader.Read();
				pen = ReadPenElement(reader, brushes);
				// Read head pen
				if (_version > 1)
				{
					reader.Read();
					headPen = ReadPenElement(reader, brushes);
				}

				// Read the properties
				if(!ReadProperties(reader, a, reader.Depth))
					throw new InvalidFormatException(
						"Unexpected EOF while parsing arrow info. Arrow: " +
						i.ToString());

				// Set again segment count, because
				// if the arrow is routed, settings
				// segment count through SegmentCount property
				// won't have any effect
				if (a.Style == ArrowStyle.Bezier)
					a.InternalSegmentCount = (short)((points.Count - 1) / 3);
				else
					a.InternalSegmentCount = (short)(points.Count - 1);

				// Set the control points
				for(int p = 0; p < points.Count; p++)
					a.ControlPoints[p] = points[p];
				a.UpdateFromPoints();

				// Set brush and pen
				if(brushId != -1)
					a.Brush = (FlowChartX.Brush)brushes[brushId];
				if(pen != null)
					a.Pen = pen;
				if (headPen != null)
					a.HeadPen = headPen;

				objects.Add(idArrow, a);
				zOrder.Add(zIndex, a);
			}

			// Read arrows closing element
			if(count > 0)
				reader.Read();


			// Adjust z-order
			for(i = 0; i < zOrder.Count; i++)
			{
				ChartObject obj = (ChartObject)zOrder.GetByIndex(i);
				obj.ZIndex = (int)zOrder.GetKey(i);
			}


			// Read groups
			ReadMandatory(reader, "Groups",
				"Groups element missing or invalid");
			count = XmlConvert.ToInt32(reader.GetAttribute("Count"));
			for(i = 0; i < count; i++)
			{
				// Read the group element
				ReadMandatory(reader, "Group",
					"Unrecognized group token");

				// Read the main object
				reader.Read();
				id = XmlConvert.ToInt32(reader.GetAttribute("Id"));

				g = _diagram.CreateGroup((ChartObject)objects[id]);

				// Read group's visibility
				reader.Read();
				reader.Read();
				g.Visible = XmlConvert.ToBoolean(reader.Value.ToLower());
				reader.Read();

				// Read AutoDeleteItems flag
				if (_version >= 7)
				{
					reader.Read();
					reader.Read();
					g.AutoDeleteItems = XmlConvert.ToBoolean(reader.Value.ToLower());
					reader.Read();
				}

				// Read Expandable flag
				if (_version >= 8)
				{
					reader.Read();
					reader.Read();
					g.Expandable = XmlConvert.ToBoolean(reader.Value.ToLower());
					reader.Read();
				}

				// Read FollowMasterRotation flag
				if (_version >= 9)
				{
					reader.Read();
					reader.Read();
					g.FollowMasterRotation = XmlConvert.ToBoolean(reader.Value.ToLower());
					reader.Read();
				}

				reader.Read(); // read Tag or Attachments
				if (reader.Name == "Tag")
				{
					// Read the value
					reader.Read();

					if(DeserializeTag != null)
					{
						SerializeTagArgs args = new SerializeTagArgs(g);
						args.Representation = reader.Value;
						DeserializeTag(this, args);
					}

					// Read the closing Tag element
					reader.Read();

					// Read the Attachments
					reader.Read();
				}

				// Process attachments
				int acount = XmlConvert.ToInt32(reader.GetAttribute("Count"));
				int ai;
				int adata;
				Node node;
				int al, at, ar, ab;
				AttachTo atype;
				for(ai = 0; ai < acount; ai++)
				{
					// Read attachment element
					reader.Read();

					// Read data
					reader.Read();
					reader.Read();
					adata = XmlConvert.ToInt32(reader.Value);
					reader.Read();

					// Read object
					reader.Read();
					reader.Read();
					node = (Node)objects[XmlConvert.ToInt32(reader.Value)];
					reader.Read();

					// Read percents
					reader.Read();
					al = XmlConvert.ToInt32(reader.GetAttribute("Left"));
					at = XmlConvert.ToInt32(reader.GetAttribute("Top"));
					ar = XmlConvert.ToInt32(reader.GetAttribute("Right"));
					ab = XmlConvert.ToInt32(reader.GetAttribute("Bottom"));

					// Read type
					reader.Read();
					reader.Read();
					atype = (AttachTo)XmlConvert.ToEnum(
						typeof(AttachTo), reader.Value);
					reader.Read();

					switch(atype)
					{
						case AttachTo.ArrowPoint:
							g.AttachToArrowPoint(node, adata);
							break;
						case AttachTo.ArrowSegment:
							g.AttachToArrowSegment(node, adata);
							break;
						case AttachTo.FixedCorner:
							g.AttachToCorner(node, adata);
							break;
						case AttachTo.Proportional:
							g.AttachProportional(node, al, at, ar, ab);
							break;
						case AttachTo.LongestHSegment:
							g.AttachToLongestHSegment(node);
							break;
						case AttachTo.SideMiddle:
							g.AttachToSideMiddle(node, adata);
							break;
					}

					// Read attachment closing element
					reader.Read();
				}

				// Read attachments' closing element
				if(acount > 0)
					reader.Read();

				// Read the group closing element
				reader.Read();
			}

			// Read groups closing element
			reader.Read();

			// Read diagram closing element
			reader.Read();

			foreach (ChartObject obj in _diagram.Objects)
				obj.onLoad();

			// Note: If exception is thrown this
			// flag will remain raised
			_diagram.DontRouteForAwhile = false;
		}

		public event SerializeTagEvent DeserializeTag;


		private Hashtable _propertyMap;
	}
}