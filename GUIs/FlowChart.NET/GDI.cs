// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Collections;
using System.IO;
using System.Reflection;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Generally used to generate construction code
	/// during design-time serialization.
	/// </summary>
	internal class BrushConverter : TypeConverter
	{
		public override bool CanConvertTo(
			ITypeDescriptorContext context, Type destinationType)
		{
			if (destinationType == typeof(InstanceDescriptor)) 
				return true;

			return base.CanConvertTo(context, destinationType);
		}

		public override object ConvertTo(ITypeDescriptorContext context,
			System.Globalization.CultureInfo culture, object value, Type destinationType)
		{
			if (destinationType == typeof(InstanceDescriptor))
			{
				Brush brush = null;

				// Depending on the type of 'value'
				// generate the appropriate constuction code
				if (value is SolidBrush)
				{
					brush = value as SolidBrush;
				}

				else if (value is LinearGradientBrush)
				{
					brush = value as LinearGradientBrush;
				}

				else if (value is HatchBrush)
				{
					brush = value as HatchBrush;
				}

				else if (value is TextureBrush)
				{
					brush = value as TextureBrush;
				}

				if (brush != null)
				{
					ConstructorInfo ctor =
						brush.GetType().GetConstructor(new Type[] { typeof(string) });
					if (ctor != null)
						return new InstanceDescriptor(ctor,
							new object[] { brush.Store() });
				}
			}

			return base.ConvertTo(context, culture, value, destinationType);
		}
	}

	/// <summary>
	/// Generally used to generate construction code
	/// during design-time serialization.
	/// </summary>
	internal class PenConverter : TypeConverter
	{
		public override bool CanConvertTo(
			ITypeDescriptorContext context, Type destinationType)
		{
			if (destinationType == typeof(InstanceDescriptor)) 
				return true;

			return base.CanConvertTo(context, destinationType);
		}

		public override object ConvertTo(ITypeDescriptorContext context,
			System.Globalization.CultureInfo culture, object value, Type destinationType)
		{
			if (destinationType == typeof(InstanceDescriptor))
			{
				Pen pen = null;

				// Depending on the type of 'value'
				// generate the appropriate constuction code
				if (value is Pen)
				{
					pen = value as Pen;
				}

				if (pen != null)
				{
					ConstructorInfo ctor =
						pen.GetType().GetConstructor(new Type[] { typeof(string) });
					if (ctor != null)
						return new InstanceDescriptor(ctor,
							new object[] { pen.Store() });
				}
			}

			return base.ConvertTo(context, culture, value, destinationType);
		}
	}

	/// <summary>
	/// Defines a wrapper for the System.Drawing.Brush class.
	/// </summary>
	[TypeConverter(typeof(BrushConverter))]
	public abstract class Brush : IPersists, ICloneable
	{
		/// <summary>
		/// Creates and returns an instance of a
		/// System.Drawing.Brush -derived class, corresponding
		/// to the wrapper.
		/// </summary>
		public abstract System.Drawing.Brush CreateGDIBrush(RectangleF rc);
		public virtual System.Drawing.Brush CreateGDIBrush(RectangleF rc, float dx, float dy)
		{
			return CreateGDIBrush(rc);
		}

		/// <summary>
		/// Creates exact copy of the brush.
		/// </summary>
		/// <returns></returns>
		public virtual object Clone() { return null; }

		public virtual int getClassId() { return -1; }
		public virtual void saveTo(BinaryWriter writer, PersistContext ctx) { writer.Write(_refCount); }
		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			reader.ReadInt32();
			_refCount = 0;
		}
		public virtual void setReference(int refId, IPersists obj) {}

		internal static void freeUnusedBrushes()
		{
			for (int i = 0; i < _brushes.Count; )
			{
				if ((_brushes[i] as Brush)._refCount == 0)
					_brushes.RemoveAt(i);
				else
					i++;
			}
		}

		internal static void StoreBrushes(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(_brushes.Count);
			foreach(Brush br in _brushes)
				ctx.saveObject(br);
		}

		internal static void RestoreBrushes(IPersistObjFactory objFactory,
			BinaryReader reader, PersistContext ctx)
		{
			Brush brush;
			int count = reader.ReadInt32();

			for(int i = 0; i < count; i++)
			{
				brush = (Brush)ctx.loadObject();
				_brushes.Add(brush);
			}
		}


		/// <summary>
		/// When overriden in a derived class,
		/// serializes the brush into a string.
		/// </summary>
		internal abstract string Store();

		/// <summary>
		/// When overriden in a derived class,
		/// deserializes the brush from a string.
		/// </summary>
		internal abstract void Restore(string image);

		/// <summary>
		/// compares this brush with another one
		/// </summary>
		internal abstract bool equals(Brush brush);

		/// <summary>
		/// Serializes the brush to a string object.
		/// </summary>
		public static string Serialize(Brush brush)
		{
			string code = "invalid";

			if (brush is SolidBrush)
				code = "solid";

			else if (brush is HatchBrush)
				code = "hatch";

			else if (brush is LinearGradientBrush)
				code = "gradient";

			else if (brush is TextureBrush)
				code = "texture";

			return code + ";" + brush.Store();
		}

		/// <summary>
		/// Creates and returns a new brush,
		/// given its string representation.
		/// </summary>
		public static Brush Deserialize(string image)
		{
			Brush brush = null;

			int semicolon = image.IndexOf(";");
			if (semicolon == -1)
				return null;

			string code = image.Substring(0, semicolon);
			string representation = image.Substring(semicolon + 1);

			switch (code)
			{

				case "solid":
					brush = new SolidBrush(Color.Black);
					break;

				case "hatch":
					brush = new HatchBrush(HatchStyle.Cross, Color.Black);
					break;

				case "gradient":
					brush = new LinearGradientBrush(Color.Black, Color.Black);
					break;

				case "texture":
					brush = new TextureBrush((Image)null);
					break;

			}

			if (brush == null)
				return null;

			brush.Restore(representation);

			return brush;
		}

		/// <summary>
		/// Increases the reference count of the brush.
		/// </summary>
		public void AddRef()
		{
			if(!_brushes.Contains(this))
				_brushes.Add(this);
			_refCount++;
		}

		/// <summary>
		/// Decreases the reference count of the brush by 1,
		/// eventually removing the brush from the internal collection
		/// if the count reaches 0.
		/// </summary>
		public void Release()
		{
			if(_refCount == 0)
				throw new Exception("Releasing an object with zero reference count!");

			_refCount--;
			if(_refCount == 0)
				_brushes.Remove(this);
		}

	
		/// <summary>
		/// Gets a collection with all brushes.
		/// </summary>
		internal static ArrayList Brushes
		{
			get { return _brushes; }
		}

		private int _refCount = 0;

		static private ArrayList _brushes = new ArrayList();
	}


	/// <summary>
	/// Defines a wrapper for the System.Drawing.Drawing2D.HatchBrush class.
	/// </summary>
	public class HatchBrush : Brush
	{
		/// <summary>
		/// Constructs a hatch brush given its string representation.
		/// </summary>
		public HatchBrush(string representation)
		{
			Restore(representation);
		}

		/// <summary>
		/// Constructs a new HatchBrush with the specified style and color.
		/// </summary>
		public HatchBrush(HatchStyle hatchStyle, Color foreColor)
		{
			_hatchStyle = hatchStyle;
			_foreColor = foreColor;
			_backColor = Color.Black;
		}

		/// <summary>
		/// Constructs a new HatchBrush with the specified style,
		/// foreground color and background color.
		/// </summary>
		public HatchBrush(HatchStyle hatchStyle, Color foreColor, Color backColor)
		{
			_hatchStyle = hatchStyle;
			_foreColor = foreColor;
			_backColor = backColor;
		}

		public override System.Drawing.Brush CreateGDIBrush(RectangleF rc)
		{
			System.Drawing.Drawing2D.HatchBrush brush =
				new System.Drawing.Drawing2D.HatchBrush(
				_hatchStyle, _foreColor, _backColor);

			return brush;
		}

		public override object Clone()
		{
			return new HatchBrush(_hatchStyle, _foreColor, _backColor);
		}

		internal override bool equals(Brush obj)
		{
			if (obj == null)
				return false;

			if (obj.GetType() != typeof(HatchBrush))
				return false;

			HatchBrush brush = obj as HatchBrush;

			// Hatch style
			if (this._hatchStyle != brush._hatchStyle)
				return false;

			// Fore color
			if (this._foreColor.ToArgb() != brush._foreColor.ToArgb())
				return false;

			// Back color
			if (this._backColor.ToArgb() != brush._backColor.ToArgb())
				return false;

			return true;
		}

		public override string ToString()
		{
			return "Hatch";
		}

		internal override string Store()
		{
			string representation = "";

			representation += Xml.XmlConvert.FromInt32((int)_hatchStyle) + ";";
			representation += Xml.XmlConvert.FromColor(_foreColor) + ";";
			representation += Xml.XmlConvert.FromColor(_backColor);

			return representation;
		}

		internal override void Restore(string image)
		{
			string[] parts = image.Split(new char[] { ';' });

			_hatchStyle = (HatchStyle)Xml.XmlConvert.ToInt32(parts[0]);
			_foreColor = Xml.XmlConvert.ToColor(parts[1]);
			_backColor = Xml.XmlConvert.ToColor(parts[2]);
		}


		// Implement IPersist
		public override int getClassId()
		{
			return 32;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			writer.Write((int)_hatchStyle);
			ctx.saveColor(_foreColor);
			ctx.saveColor(_backColor);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);

			_hatchStyle = (HatchStyle)reader.ReadInt32();
			_foreColor = ctx.loadColor();
			_backColor = ctx.loadColor();
		}

		public override void setReference(int refId, IPersists obj)
		{
		}


		public HatchStyle HatchStyle
		{
			get { return _hatchStyle; }
			set { _hatchStyle = value; }
		}

		public Color ForegroundColor
		{
			get { return _foreColor; }
			set { _foreColor = value; }
		}

		public Color BackgroundColor
		{
			get { return _backColor; }
			set { _backColor = value; }
		}


		private HatchStyle _hatchStyle;
		private Color _foreColor;
		private Color _backColor;
	}


	/// <summary>
	/// Defines a wrapper for the System.Drawing.Drawing2D.LinearGradientBrush class.
	/// </summary>
	public class LinearGradientBrush : Brush
	{
		/// <summary>
		/// Constructs a linear gradient brush given its string representation.
		/// </summary>
		public LinearGradientBrush(string representation) : this(Color.Empty, Color.Empty)
		{
			Restore(representation);
		}

		public LinearGradientBrush(Color color1, Color color2)
		{
			_blend = null;
			_linearColors = new Color[2] { color1, color2 };
			_interpolationColors = null;
			_angle = 0.0f;
		}

		public LinearGradientBrush(Color color1, Color color2, float angle)
		{
			_blend = null;
			_linearColors = new Color[2] { color1, color2 };
			_interpolationColors = null;
			_angle = angle;
		}

		public override System.Drawing.Brush CreateGDIBrush(RectangleF rc)
		{
			RectangleF rect = Utilities.normalizeRect(rc);
			if(rect.Width == 0 || rect.Height == 0)
				rect = new RectangleF(0.0f, 0.0f, 1.0f, 1.0f);

			System.Drawing.Drawing2D.LinearGradientBrush brush = new
				System.Drawing.Drawing2D.LinearGradientBrush(
				rect, _linearColors[0], _linearColors[1], _angle);
			brush.WrapMode = WrapMode.TileFlipXY;

			if(_blend != null)
				brush.Blend = _blend;
			if(_interpolationColors != null)
				brush.InterpolationColors = _interpolationColors;

			return brush;
		}

		public override System.Drawing.Brush CreateGDIBrush(RectangleF rc, float dx, float dy)
		{
			System.Drawing.Drawing2D.LinearGradientBrush brush =
				CreateGDIBrush(rc) as System.Drawing.Drawing2D.LinearGradientBrush;

			//brush.TranslateTransform(dx, dy);
			return brush;
		}

		public override object Clone()
		{
			LinearGradientBrush brush = new LinearGradientBrush(
				_linearColors[0], _linearColors[1], _angle);

			if(_blend != null)
			{
				brush._blend = new Blend();
				brush._blend.Factors = (float[])_blend.Factors.Clone();
				brush._blend.Positions = (float[])_blend.Positions.Clone();
			}
			if(_interpolationColors != null)
			{
				brush._interpolationColors = new ColorBlend();
				brush._interpolationColors.Positions =
					(float[])_interpolationColors.Positions.Clone();
				brush._interpolationColors.Colors =
					new Color[_interpolationColors.Colors.Length];
				for(int i = 0; i < _interpolationColors.Colors.Length; i++)
					brush._interpolationColors.Colors[i] = _interpolationColors.Colors[i];
			}

			return brush;
		}

		internal override bool equals(Brush obj)
		{
			if (obj == null)
				return false;

			if (obj.GetType() != typeof(LinearGradientBrush))
				return false;

			LinearGradientBrush brush = obj as LinearGradientBrush;

			// Check angle
			if (this._angle != brush._angle)
				return false;

			// Check linear colors
			for (int i = 0; i < this._linearColors.Length; i++)
				if (this._linearColors[i].ToArgb() != brush._linearColors[i].ToArgb())
					return false;

			// Check blend
			if (this._blend == null && brush._blend != null ||
				this._blend != null && brush._blend == null)
				return false;

			if (this._blend != null)
			{
				if (this._blend.Factors.Length != brush._blend.Factors.Length ||
					this._blend.Positions.Length != brush._blend.Positions.Length)
					return false;

				for (int i = 0; i < this._blend.Factors.Length; i++)
					if (this._blend.Factors[i] != brush._blend.Factors[i])
						return false;

				for (int i = 0; i < this._blend.Positions.Length; i++)
					if (this._blend.Positions[i] != brush._blend.Positions[i])
						return false;
			}

			// Check interpolation colors
			if (this._interpolationColors == null &&
				brush._interpolationColors != null ||
				this._interpolationColors != null &&
				brush._interpolationColors == null)
				return false;

			if (this._interpolationColors != null)
			{
				if (this._interpolationColors.Colors.Length !=
					brush._interpolationColors.Colors.Length ||
					this._interpolationColors.Positions.Length !=
					brush._interpolationColors.Positions.Length)
					return false;

				for (int i = 0; i < this._interpolationColors.Colors.Length; i++)
					if (this._interpolationColors.Colors[i].ToArgb() !=
						brush._interpolationColors.Colors[i].ToArgb())
						return false;

				for (int i = 0; i < this._interpolationColors.Positions.Length; i++)
					if (this._interpolationColors.Positions[i] !=
						brush._interpolationColors.Positions[i])
						return false;
			}

			return true;
		}

		public override string ToString()
		{
			return "Gradient";
		}

		internal override string Store()
		{
			string representation = "";

			representation += Xml.XmlConvert.FromSingle(_angle) + ";";
			representation += Xml.XmlConvert.FromColor(_linearColors[0]) + ";";
			representation += Xml.XmlConvert.FromColor(_linearColors[1]);

			return representation;
		}

		internal override void Restore(string image)
		{
			string[] parts = image.Split(new char[] { ';' });

			_angle = Xml.XmlConvert.ToSingle(parts[0]);
			_linearColors[0] = Xml.XmlConvert.ToColor(parts[1]);
			_linearColors[1] = Xml.XmlConvert.ToColor(parts[2]);
		}


		// Implement IPersist
		public override int getClassId()
		{
			return 33;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			if(_blend != null)
			{
				writer.Write((int)1);
				ctx.saveFloatArray(_blend.Factors);
				ctx.saveFloatArray(_blend.Positions);
			}
			else
			{
				writer.Write((int)0);
			}

			ctx.saveColor(_linearColors[0]);
			ctx.saveColor(_linearColors[1]);
			
			if(_interpolationColors != null)
			{
				writer.Write((int)1);
				ctx.saveColorArray(_interpolationColors.Colors);
				ctx.saveFloatArray(_interpolationColors.Positions);
			}
			else
			{
				writer.Write((int)0);
			}

			writer.Write((double)_angle);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);

			int b = reader.ReadInt32();
			if(b == 1)
			{
				_blend = new Blend();
				_blend.Factors = ctx.loadFloatArray();
				_blend.Positions = ctx.loadFloatArray();
			}
			else
			{
				_blend = null;
			}

			_linearColors[0] = ctx.loadColor();
			_linearColors[1] = ctx.loadColor();

			b = reader.ReadInt32();
			if(b == 1)
			{
				_interpolationColors = new ColorBlend();
				_interpolationColors.Colors = ctx.loadColorArray();
				_interpolationColors.Positions = ctx.loadFloatArray();
			}
			else
			{
				_interpolationColors = null;
			}

			_angle = (float)reader.ReadDouble();
		}

		public override void setReference(int refId, IPersists obj)
		{
		}


		public Blend Blend
		{
			get { return _blend; }
			set { _blend = value; }
		}

		public Color[] LinearColors
		{
			get { return _linearColors; }
			set
			{
				if (value == null)
					throw new ArgumentException("LinearColors cannot be null.", "value");

				if (value.Length != 2)
					throw new ArgumentException("The number of colors in the LinearColors array must always be 2.", "value");

				_linearColors = value;
			}
		}

		public ColorBlend InterpolationColor
		{
			get { return _interpolationColors; }
			set { _interpolationColors = value; }
		}

		public float Angle
		{
			get { return _angle; }
			set { _angle = value; }
		}


		private Blend _blend;
		private Color[] _linearColors;
		private ColorBlend _interpolationColors;
		private float _angle;
	}

	/// <summary>
	/// Defines a wrapper for the System.Drawing.SolidBrush class.
	/// </summary>
	public class SolidBrush : Brush
	{
		/// <summary>
		/// Constructs a solid brush given its string representation.
		/// </summary>
		public SolidBrush(string representation)
		{
			Restore(representation);
		}

		public SolidBrush(Color color)
		{
			_color = color;
		}

		public override System.Drawing.Brush CreateGDIBrush(RectangleF rc)
		{
			return new System.Drawing.SolidBrush(_color);
		}

		public override object Clone()
		{
			return new SolidBrush(_color);
		}

		internal override bool equals(Brush obj)
		{
			if (obj == null)
				return false;

			if (obj.GetType() != typeof(SolidBrush))
				return false;

			SolidBrush brush = obj as SolidBrush;

			// Color
			if (this._color.ToArgb() != brush._color.ToArgb())
				return false;

			return true;
		}

		public override string ToString()
		{
			return "Solid";
		}

		internal override string Store()
		{
			string representation = "";

			representation += Xml.XmlConvert.FromColor(_color);

			return representation;
		}

		internal override void Restore(string image)
		{
			_color = Xml.XmlConvert.ToColor(image);
		}


		// Implement IPersist
		public override int getClassId()
		{
			return 34;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			ctx.saveColor(_color);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);

			_color = ctx.loadColor();
		}

		public override void setReference(int refId, IPersists obj)
		{
		}


		public Color Color
		{
			get { return _color; }
			set { _color = value; }
		}

		private Color _color;
	}

	/// <summary>
	/// Defines a wrapper for the System.Drawing.TextureBrush class.
	/// </summary>
	public class TextureBrush : Brush
	{
		/// <summary>
		/// Constructs a texture brush given its string representation.
		/// </summary>
		public TextureBrush(string representation)
		{
			Restore(representation);
		}

		public TextureBrush(Image image)
		{
			_image = image;
			_wrapMode = WrapMode.Tile;
		}

		public TextureBrush(Image image, WrapMode wrapMode)
		{
			_image = image;
			_wrapMode = wrapMode;
		}

		public override System.Drawing.Brush CreateGDIBrush(RectangleF rc)
		{
			return new System.Drawing.TextureBrush(_image, _wrapMode);
		}

		public override System.Drawing.Brush CreateGDIBrush(RectangleF rc, float dx, float dy)
		{
			RectangleF rct = new RectangleF(0, 0, _image.Width, _image.Height);
			System.Drawing.TextureBrush brush =
				new System.Drawing.TextureBrush(_image, _wrapMode, rct);

			brush.TranslateTransform(dx, dy);
			brush.ScaleTransform(24.0f/80, 24.0f/80, MatrixOrder.Append);
			return brush;
		}

		public override object Clone()
		{
			return new TextureBrush((Image)_image.Clone(), _wrapMode);
		}

		internal override bool equals(Brush obj)
		{
			if (obj == null)
				return false;

			if (obj.GetType() != typeof(TextureBrush))
				return false;

			TextureBrush brush = obj as TextureBrush;

			// Wrap mode
			if (this._wrapMode != brush._wrapMode)
				return false;

			// Images
			// Note: Since we can hardly compare image equality,
			// return false unless both images are null.
			if (this._image != null || brush._image == null)
				return false;

			return true;
		}

		public override string ToString()
		{
			return "Texture";
		}

		internal override string Store()
		{
			// TODO
			return "";
		}

		internal override void Restore(string image)
		{
			// TODO
		}


		// Implement IPersist
		public override int getClassId()
		{
			return 35;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			ctx.saveImage(_image);
			writer.Write((int)_wrapMode);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);

			_image = ctx.loadImage();
			_wrapMode = (WrapMode)reader.ReadInt32();
		}

		public override void setReference(int refId, IPersists obj)
		{
		}


		public Image Image
		{
			get { return _image; }
			set { _image = value; }
		}

		public WrapMode WrapMode
		{
			get { return _wrapMode; }
			set { _wrapMode = value; }
		}


		private Image _image;
		private WrapMode _wrapMode;
	}

	/// <summary>
	/// Defines a wrapper for the System.Drawing.Pen class.
	/// </summary>
	[TypeConverter(typeof(PenConverter))]
	public class Pen : IPersists, ICloneable
	{
		/// <summary>
		/// Constructs a pen given its string representation.
		/// </summary>
		public Pen(string representation)
		{
			Restore(representation);
		}

		public Pen(MindFusion.FlowChartX.Brush brush)
		{
			Brush = brush;
			_color = Color.Black;
			_compoundArray = null;
			_dashStyle = DashStyle.Solid;
			_dashPattern = null;
			_dashOffset = 0.0f;
			_width = 0.0f;
			_lineJoin = LineJoin.Miter;
			_miterLimit = 10.0f;
		}

		public Pen(Color color)
		{
			Brush = null;
			_color = color;
			_compoundArray = null;
			_dashStyle = DashStyle.Solid;
			_dashPattern = null;
			_dashOffset = 0.0f;
			_width = 0.0f;
			_lineJoin = LineJoin.Miter;
			_miterLimit = 10.0f;
		}

		public Pen(MindFusion.FlowChartX.Brush brush, float width)
		{
			Brush = brush;
			_color = Color.Black;
			_compoundArray = null;
			_dashStyle = DashStyle.Solid;
			_dashPattern = null;
			_dashOffset = 0.0f;
			_width = width;
			_lineJoin = LineJoin.Miter;
			_miterLimit = 10.0f;
		}

		public Pen(Color color, float width)
		{
			Brush = null;
			_color = color;
			_compoundArray = null;
			_dashStyle = DashStyle.Solid;
			_dashPattern = null;
			_dashOffset = 0.0f;
			_width = width;
			_lineJoin = LineJoin.Miter;
			_miterLimit = 10.0f;
		}

		~Pen()
		{
			Brush = null;
		}

		public System.Drawing.Pen CreateGDIPen()
		{
			return CreateGDIPen(new RectangleF(0.0f, 0.0f, 1.0f, 1.0f));
		}

		public System.Drawing.Pen CreateGDIPen(RectangleF rc)
		{
			System.Drawing.Pen pen = new System.Drawing.Pen(_color, _width);
			if(_brush != null)
				pen.Brush = _brush.CreateGDIBrush(rc);
			if(_compoundArray != null)
				pen.CompoundArray = _compoundArray;
			pen.DashStyle = _dashStyle;
			if(_dashPattern != null)
				pen.DashPattern = _dashPattern;
			pen.DashOffset = _dashOffset;
			pen.LineJoin = _lineJoin;
			pen.MiterLimit = _miterLimit;

			return pen;
		}

		public virtual object Clone()
		{
			Pen pen = new Pen(_color, _width);

			if(_brush != null)
				pen.Brush = _brush;
			if(_compoundArray != null)
				pen._compoundArray = (float[])_compoundArray.Clone();
			pen._dashStyle = _dashStyle;
			if(_dashPattern != null)
				pen._dashPattern = (float[])_dashPattern.Clone();
			pen._dashOffset = _dashOffset;
			pen._lineJoin = _lineJoin;
			pen._miterLimit = _miterLimit;

			return pen;
		}

		internal bool equals(Pen obj)
		{
			if (obj == null)
				return false;

			if (obj.GetType() != typeof(Pen))
				return false;

			Pen pen = obj as Pen;

			// Brushes
			if (this._brush == null)
			{
				if (pen._brush != null)
					return false;
			}
			else
			{
				if (!this._brush.equals(pen._brush))
					return false;
			}

			// Colors
			if (this._color.ToArgb() != pen._color.ToArgb())
				return false;

			// Compound array
			if (this._compoundArray != null && pen._compoundArray == null ||
				this._compoundArray == null && pen._compoundArray != null)
				return false;

			if (this._compoundArray != null)
			{
				if (this._compoundArray.Length != pen._compoundArray.Length)
					return false;

				for (int i = 0; i < this._compoundArray.Length; i++)
					if (this._compoundArray[i] != pen._compoundArray[i])
						return false;
			}

			// Dash offset
			if (this._dashOffset != pen._dashOffset)
				return false;

			// Dash patterns
			if (this._dashPattern != null && pen._dashPattern == null ||
				this._dashPattern == null && pen._dashPattern != null)
				return false;

			if (this._dashPattern != null)
			{
				if (this._dashPattern.Length != pen._dashPattern.Length)
					return false;

				for (int i = 0; i < this._dashPattern.Length; i++)
					if (this._dashPattern[i] != pen._dashPattern[i])
						return false;
			}

			// Dash style
			if (this._dashStyle != pen._dashStyle)
				return false;

			// Line join
			if (this._lineJoin != pen._lineJoin)
				return false;

			// Miter limit
			if (this._miterLimit != pen._miterLimit)
				return false;

			// Pen width
			if (this._width != pen._width)
				return false;

			return true;
		}

		public override string ToString()
		{
			switch (_dashStyle)
			{

				case DashStyle.Custom:
					return "Custom";

				case DashStyle.Dash:
					return "Dash";

				case DashStyle.DashDot:
					return "DashDot";

				case DashStyle.DashDotDot:
					return "DashDotDot";

				case DashStyle.Dot:
					return "Dot";

				case DashStyle.Solid:
					return "Solid";

			}

			return "";
		}

		internal virtual string Store()
		{
			string representation = "";

			// style
			representation += Xml.XmlConvert.FromInt32((int)_dashStyle) + ";";

			// color
			representation += Xml.XmlConvert.FromColor(_color) + ";";

			// width
			representation += Xml.XmlConvert.FromSingle(_width) + ";";

			// dash pattern
			if (_dashPattern != null)
			{
				representation += Xml.XmlConvert.FromInt32(_dashPattern.Length) + ";";

				foreach (float f in _dashPattern)
					representation += Xml.XmlConvert.FromSingle(f) + ";";
			}
			else
			{
				representation += "0;";
			}

			// compound
			if (_compoundArray != null)
			{
				representation += Xml.XmlConvert.FromInt32(_compoundArray.Length) + ";";

				foreach (float f in _compoundArray)
					representation += Xml.XmlConvert.FromSingle(f) + ";";
			}
			else
			{
				representation += "0;";
			}

			return representation;
		}

		internal virtual void Restore(string image)
		{
			string[] parts = image.Split(new char[] { ';' });
			int i = 0;

			// style
			_dashStyle = (DashStyle)Xml.XmlConvert.ToInt32(parts[i++]);

			// color
			_color = Xml.XmlConvert.ToColor(parts[i++]);

			// width
			_width = Xml.XmlConvert.ToSingle(parts[i++]);

			// dash pattern
			int len = Xml.XmlConvert.ToInt32(parts[i++]);
			if (len > 0)
			{
				_dashPattern = new float[len];
				for (int j = 0; j < len; j++)
					_dashPattern[j] = Xml.XmlConvert.ToSingle(parts[i++]);
			}
			else
			{
				_dashPattern = null;
			}

			// compound array
			len = Xml.XmlConvert.ToInt32(parts[i++]);
			if (len > 0)
			{
				_compoundArray = new float[len];
				for (int j = 0; j < len; j++)
					_compoundArray[j] = Xml.XmlConvert.ToSingle(parts[i++]);
			}
			else
			{
				_compoundArray = null;
			}
		}

		/// <summary>
		/// Serializes a pen to a string object.
		/// </summary>
		public static string Serialize(Pen pen)
		{
			return "pen;" + pen.Store();
		}

		/// <summary>
		/// Creates and returns a new pen,
		/// given its string representation.
		/// </summary>
		public static Pen Deserialize(string image)
		{
			int semicolon = image.IndexOf(";");
			if (semicolon == -1)
				return null;

			string code = image.Substring(0, semicolon);
			string representation = image.Substring(semicolon + 1);

			if (code != "pen")
				return null;

			Pen pen = new Pen(Color.Black);
			pen.Restore(representation);

			return pen;
		}


		public virtual int getClassId()
		{
			return 31;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			ctx.saveReference(this, _brush, 1);
			ctx.saveColor(_color);
			ctx.saveFloatArray(_compoundArray);
			writer.Write((int)_dashStyle);
			ctx.saveFloatArray(_dashPattern);
			writer.Write((double)_dashOffset);
			writer.Write((double)_width);
			writer.Write((int)_lineJoin);
			writer.Write((double)_miterLimit);
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			ctx.loadReference(this);
			_color = ctx.loadColor();
			_compoundArray = ctx.loadFloatArray();
			_dashStyle = (DashStyle)reader.ReadInt32();
			_dashPattern = ctx.loadFloatArray();
			_dashOffset = (float)reader.ReadDouble();
			_width = (float)reader.ReadDouble();
			_lineJoin = (LineJoin)reader.ReadInt32();
			_miterLimit = (float)reader.ReadDouble();
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			switch(refId)
			{
				case 1:
					_brush = (Brush)obj;
					if (_brush != null)
						_brush.AddRef();
					break;
				default:
					throw new Exception("Unrecognized reference in Pen.setReference(...)");
			}
		}


		public MindFusion.FlowChartX.Brush Brush
		{
			get { return _brush; }
			set
			{
				if(_brush != null)
					_brush.Release();
				_brush = value;
				if(_brush != null)
					_brush.AddRef();
			}
		}

		public Color Color
		{
			get { return _color; }
			set { _color = value; }
		}

		public float[] CompoundArray
		{
			get { return _compoundArray; }
			set { _compoundArray = value; }
		}

		public DashStyle DashStyle
		{
			get { return _dashStyle; }
			set { _dashStyle = value; }
		}

		public float[] DashPattern
		{
			get { return _dashPattern; }
			set { _dashPattern = value; }
		}

		public float DashOffset
		{
			get { return _dashOffset; }
			set { _dashOffset = value; }
		}

		public float Width
		{
			get { return _width; }
			set { _width = value; }
		}

		public LineJoin LineJoin
		{
			get { return _lineJoin; }
			set { _lineJoin = value; }
		}

		public float MiterLimit
		{
			get { return _miterLimit; }
			set { _miterLimit = value; }
		}

		private MindFusion.FlowChartX.Brush _brush = null;
		private Color _color;
		private float[] _compoundArray;
		private DashStyle _dashStyle;
		private float[] _dashPattern;
		private float _dashOffset;
		private float _width;
		private LineJoin _lineJoin;
		private float _miterLimit;
	}
}
