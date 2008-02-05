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
using System.IO;
using System.Collections;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	public abstract class ElementTemplate : ICloneable, IPersists
	{
		internal abstract float[] initData(RectangleF rc, float rotation);
		internal abstract void updateData(RectangleF rc, float[] data, float rotation);
		internal abstract void updatePath(GraphicsPath path, float[] data);
		internal abstract bool outsideBounds();
		public abstract object Clone();

		public abstract int getClassId();
		public abstract void saveTo(BinaryWriter writer, PersistContext ctx);
		public abstract void loadFrom(BinaryReader reader, PersistContext ctx);
		public abstract void setReference(int refId, IPersists obj);

		public abstract bool SameAs(ElementTemplate obj);
	}

	public class ArcTemplate : ElementTemplate
	{
		public ArcTemplate(float x, float y, float w, float h, float a, float s)
		{
			this.x = x;
			this.y = y;
			this.w = w;
			this.h = h;
			this.a = a;
			this.s = s;
		}

		internal override float[] initData(RectangleF rc, float rotation)
		{
			float[] data = new float[5];
			updateData(rc, data, rotation);
			return data;
		}

		internal override void updateData(RectangleF rc, float[] data, float rotation)
		{
			// Rotate containing rectangle
			PointF rot = new PointF(x, y);
			float rotw = w;
			float roth = h;
			rot = Utilities.getRotatedPt(rot, rotation);
			if(rotation == 90)
			{
				rot.X -= h;
			}
			else if(rotation == 180)
			{
				rot.X -= w;
				rot.Y -= h;
			}
			else if(rotation == 270)
			{
				rot.Y -= w;
			}
			if(rotation == 90 || rotation == 270)
			{
				rotw = h;
				roth = w;
			}
			// Rotate the arc angles
			float rota = a + rotation;

			data[0] = rc.X + rot.X * rc.Width / 100;
			data[1] = rc.Y + rot.Y * rc.Height / 100;
			data[2] = rotw * rc.Width / 100;
			data[3] = roth * rc.Height / 100;
			data[4] = rota;
		}

		internal override void updatePath(GraphicsPath path, float[] data)
		{
			if (data[2] > 0 && data[3] > 0)
				path.AddArc(data[0], data[1], data[2], data[3], data[4], s);
		}

		internal override bool outsideBounds()
		{
			if (x < 0 || y < 0 || x + w > 100 || y + h > 100)
				return true;

			return false;
		}

		public override object Clone()
		{
			return this.MemberwiseClone();
		}

		public override bool SameAs(ElementTemplate obj)
		{
			if(!(obj is ArcTemplate))
				return false;

			ArcTemplate tmpl = (ArcTemplate)obj;

			return (Math.Abs(x - tmpl.x) < 0.0001 &&
				Math.Abs(y - tmpl.y) < 0.0001 &&
				Math.Abs(w - tmpl.w) < 0.0001 &&
				Math.Abs(h - tmpl.h) < 0.0001 &&
				Math.Abs(a - tmpl.a) < 0.0001 &&
				Math.Abs(s - tmpl.s) < 0.0001);
		}

		public RectangleF Bounds
		{
			get { return new RectangleF(x, y, w, h); }
		}

		public float StartAngle
		{
			get { return a; }
		}

		public float SweepAngle
		{
			get { return s; }
		}

		private float x;
		private float y;
		private float w;
		private float h;
		private float a;
		private float s;

		public override int getClassId()
		{
			return 28;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write((double)x);
			writer.Write((double)y);
			writer.Write((double)w);
			writer.Write((double)h);
			writer.Write((double)a);
			writer.Write((double)s);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			x = (float)reader.ReadDouble();
			y = (float)reader.ReadDouble();
			w = (float)reader.ReadDouble();
			h = (float)reader.ReadDouble();
			a = (float)reader.ReadDouble();
			s = (float)reader.ReadDouble();
		}

		public override void setReference(int refId, IPersists obj)
		{
		}
	}

	public class BezierTemplate : ElementTemplate
	{
		public BezierTemplate(float x1, float y1,
			float x2, float y2, float x3, float y3, float x4, float y4)
		{
			pts = new float[] { x1, y1, x2, y2, x3, y3, x4, y4 };
		}

		internal override float[] initData(RectangleF rc, float rotation)
		{
			float[] data = new float[8];
			updateData(rc, data, rotation);
			return data;
		}

		internal override void updateData(RectangleF rc, float[] data, float rotation)
		{
			PointF[] rot = new PointF[pts.Length / 2];
			for(int i = 0; i < rot.Length; i++)
				rot[i] = Utilities.getRotatedPt(new PointF(pts[i * 2], pts[i * 2 + 1]), rotation);

			data[0] = rc.X + rot[0].X * rc.Width / 100;
			data[1] = rc.Y + rot[0].Y * rc.Height / 100;
			data[2] = rc.X + rot[1].X * rc.Width / 100;
			data[3] = rc.Y + rot[1].Y * rc.Height / 100;
			data[4] = rc.X + rot[2].X * rc.Width / 100;
			data[5] = rc.Y + rot[2].Y * rc.Height / 100;
			data[6] = rc.X + rot[3].X * rc.Width / 100;
			data[7] = rc.Y + rot[3].Y * rc.Height / 100;
		}

		internal override void updatePath(GraphicsPath path, float[] data)
		{
			path.AddBezier(data[0], data[1], data[2], data[3],
				data[4], data[5], data[6], data[7]);
		}

		internal override bool outsideBounds()
		{
			for (int i = 0; i < pts.Length; ++i)
				if (pts[i] < 0 || pts[i] > 100)
					return true;

			return false;
		}

		public override object Clone()
		{
			BezierTemplate clone = new BezierTemplate(pts[0], pts[1],
				pts[2], pts[3], pts[4], pts[5], pts[6], pts[7]);
			return clone;
		}

		public override bool SameAs(ElementTemplate obj)
		{
			if(!(obj is BezierTemplate))
				return false;

			BezierTemplate tmpl = (BezierTemplate)obj;

			for(int i = 0; i < pts.Length; i++)
				if(Math.Abs(pts[i] - tmpl.pts[i]) >= 0.0001)
					return false;

			return true;
		}

		public float[] Coordinates
		{
			get { return pts; }
		}

		private float[] pts;

		public override int getClassId()
		{
			return 29;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			ctx.saveFloatArray(pts);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			pts = ctx.loadFloatArray();
		}

		public override void setReference(int refId, IPersists obj)
		{
		}
	}

	public class LineTemplate : ElementTemplate
	{
		public LineTemplate(float x1, float y1, float x2, float y2)
		{
			pts = new float[] { x1, y1, x2, y2 };
		}

		internal override float[] initData(RectangleF rc, float rotation)
		{
			float[] data = new float[4];
			updateData(rc, data, rotation);
			return data;
		}

		internal override void updateData(RectangleF rc, float[] data, float rotation)
		{
			PointF[] rot = new PointF[pts.Length / 2];
			for(int i = 0; i < rot.Length; i++)
				rot[i] = Utilities.getRotatedPt(new PointF(pts[i * 2], pts[i * 2 + 1]), rotation);

			data[0] = rc.X + rot[0].X * rc.Width / 100;
			data[1] = rc.Y + rot[0].Y * rc.Height / 100;
			data[2] = rc.X + rot[1].X * rc.Width / 100;
			data[3] = rc.Y + rot[1].Y * rc.Height / 100;
		}

		internal override void updatePath(GraphicsPath path, float[] data)
		{
			path.AddLine(data[0], data[1], data[2], data[3]);
		}

		internal override bool outsideBounds()
		{
			for (int i = 0; i < pts.Length; ++i)
				if (pts[i] < 0 || pts[i] > 100)
					return true;

			return false;
		}

		public override object Clone()
		{
			LineTemplate clone = new LineTemplate(
				pts[0], pts[1], pts[2], pts[3]);
			return clone;
		}

		public override bool SameAs(ElementTemplate obj)
		{
			if(!(obj is LineTemplate))
				return false;

			LineTemplate tmpl = (LineTemplate)obj;

			for(int i = 0; i < pts.Length; i++)
				if(Math.Abs(pts[i] - tmpl.pts[i]) >= 0.0001)
					return false;

			return true;
		}

		public float[] Coordinates
		{
			get { return pts; }
		}

		private float[] pts;

		public override int getClassId()
		{
			return 30;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			ctx.saveFloatArray(pts);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			pts = ctx.loadFloatArray();
		}

		public override void setReference(int refId, IPersists obj)
		{
		}
	}

	public class ShapeTemplate : ICloneable, IPersists
	{
		internal class PathData : ICloneable, IPersists
		{
			public PathData(int numElements)
			{
				data = new float[numElements][];
			}

			public void initData(int num, float[] elementData)
			{
				data[num] = elementData;
			}

			public void setData(int num, float[] element)
			{
				data[num] = element;
			}

			public float[] getData(int num)
			{
				return data[num];
			}

			public void setBounds(RectangleF bounds)
			{
				this.bounds = bounds;
			}

			public RectangleF getBounds()
			{
				return bounds;
			}

			public object Clone()
			{
				PathData clone = new PathData(data.GetLength(0));
				for (int i = 0; i < data.GetLength(0); ++i)
					clone.data[i] = (float[])data[i].Clone();
				return clone;
			}

			public virtual int getClassId()
			{
				return 27;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				int num = data.GetLength(0);
				writer.Write(num);
				for (int i = 0; i < num; ++i)
					ctx.saveFloatArray(data[i]);
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				int num = reader.ReadInt32();
				data = new float[num][];
				for (int i = 0; i < num; ++i)
					data[i] = ctx.loadFloatArray();
			}

			public virtual void setReference(int refId, IPersists obj)
			{
			}

			private float[][] data;
			private RectangleF bounds;
		}

		public ShapeTemplate(ElementTemplate[] elements, FillMode fillMode) :
			this(elements, fillMode, "")
		{
		}

		public ShapeTemplate(ElementTemplate[] elements, FillMode fillMode, string id)
		{
			this.elements = elements;
			this.decorations = null;
			this.textArea = null;
			this.fillMode = fillMode;
			this.image = null;

			// Add to the map
			if(id != null && id != "")
			{
				defaultShapes[id] = this;
				if (ShapeAdded != null)
					ShapeAdded(this, new EventArgs());
			}

			textOutside = false;
		}

		public ShapeTemplate(byte[] outline) :
			this(outline, "")
		{
		}

		public ShapeTemplate(byte[] outline, string id)
		{
			if (outline == null)
				throw new ArgumentException("outline cannot be null/Nothing.");

			int len = outline.Length;
			if (len < 6)
				throw new ArgumentException("The outline must contain at least 3 points.");

			if (len % 2 != 0)
				throw new ArgumentException("outline must contain even number of coordinates.");

			ElementTemplate[] lines = new ElementTemplate[len / 2];
			for (int l = 0; l < len / 2; ++l)
			{
				float x1 = outline[l * 2 + 0];
				float y1 = outline[l * 2 + 1];
				float x2 = outline[(l * 2 + 2) % len];
				float y2 = outline[(l * 2 + 3) % len];
				lines[l] = new LineTemplate(x1, y1, x2, y2);
			}

			this.elements = lines;
			this.decorations = null;
			this.textArea = null;
			this.fillMode = FillMode.Winding;
			this.image = null;

			// Add to the map
			if (id != null && id != "")
			{
				defaultShapes[id] = this;
				if (ShapeAdded != null)
					ShapeAdded(this, new EventArgs());
			}

			textOutside = false;
		}

		public ShapeTemplate(ElementTemplate[] elements,
			ElementTemplate[] decorations,
			ElementTemplate[] textArea,
			FillMode fillMode) :
			this(elements, decorations, textArea, fillMode, "")
		{
		}

		public ShapeTemplate(ElementTemplate[] elements,
			ElementTemplate[] decorations,
			ElementTemplate[] textArea,
			FillMode fillMode, string id)
		{
			this.elements = elements;
			this.decorations = decorations;
			this.textArea = textArea;
			this.fillMode = fillMode;
			this.image = null;

			// Add to the map
			if(id != null && id != "")
			{
				defaultShapes[id] = this;
				if (ShapeAdded != null)
					ShapeAdded(this, new EventArgs());
			}

			// check whether a part of the text area lays outside the shape bounds
			textOutside = false;
			if (textArea != null)
			{
				for (int i = 0; i < textArea.Length; i++)
				{
					if (textArea[i].outsideBounds())
					{
						textOutside = true;
						break;
					}
				}
			}
		}


		public static event EventHandler ShapeAdded;


		internal PathData initData(RectangleF rc, float rotation)
		{
			int i;
			int elementCount = elements.GetLength(0);
			if (decorations != null)
				elementCount += decorations.Length;
			if (textArea != null)
				elementCount += textArea.Length;

			PathData data = new PathData(elementCount);
			for (i = 0; i < elements.GetLength(0); ++i)
				data.setData(i, elements[i].initData(rc, rotation));
			if (decorations != null)
			{
				for (i = 0; i < decorations.Length; i++)
					data.setData(elements.Length + i, decorations[i].initData(rc, rotation));
			}
			if (textArea != null)
			{
				int offset = elements.Length;
				if (decorations != null)
					offset += decorations.Length;
				for (i = 0; i < textArea.Length; i++)
					data.setData(offset + i, textArea[i].initData(rc, rotation));
			}

			data.setBounds(rc);

			return data;
		}

		internal void updateData(RectangleF rc, PathData data, float rotation)
		{
			int i;

			for (i = 0; i < elements.GetLength(0); ++i)
				elements[i].updateData(rc, data.getData(i), rotation);
			if (decorations != null)
			{
				for (i = 0; i < decorations.Length; i++)
					decorations[i].updateData(rc, data.getData(elements.Length + i), rotation);
			}
			if (textArea != null)
			{
				int offset = elements.Length;
				if (decorations != null)
					offset += decorations.Length;
				for (i = 0; i < textArea.Length; i++)
					textArea[i].updateData(rc, data.getData(offset + i), rotation);
			}

			data.setBounds(rc);
		}

		private void rotatePath(PathData data, GraphicsPath path, float rotationAngle)
		{
			if (rotationAngle != 0)
			{
				Matrix rotation = new Matrix();
				rotation.RotateAt(rotationAngle,
					Utilities.getCenter(data.getBounds()));
				path.Transform(rotation);
				rotation.Dispose();
			}
		}

		internal GraphicsPath getPath(PathData data, float rotationAngle)
		{
			GraphicsPath path = new GraphicsPath(fillMode);
			for (int i = 0; i < elements.GetLength(0); ++i)
				elements[i].updatePath(path, data.getData(i));
			path.CloseFigure();

			rotatePath(data, path, rotationAngle);
			return path;
		}

		internal GraphicsPath getDecorationPath(PathData data, float rotationAngle)
		{
			if (decorations == null)
				return null;

			GraphicsPath path = new GraphicsPath(fillMode);

			// Add decorations as separate figures
			for (int i = 0; i < decorations.Length; i++)
			{
				path.StartFigure();
				decorations[i].updatePath(path, data.getData(elements.Length + i));
			}

			rotatePath(data, path, rotationAngle);
			return path;
		}

		internal PointF[] getTextArea(PathData data, float rotationAngle)
		{
			GraphicsPath path = null;

			if (textArea == null)
			{
				path = getPath(data, rotationAngle);
			}
			else
			{
				path = new GraphicsPath(fillMode);

				int offset = elements.Length;
				if (decorations != null)
					offset += decorations.Length;
				for (int i = 0; i < textArea.Length; ++i)
					textArea[i].updatePath(path, data.getData(offset + i));
				path.CloseFigure();

				rotatePath(data, path, rotationAngle);
			}

			path.Flatten();

			PointF[] result = path.PointCount > 2 ?
				(PointF[])path.PathPoints.Clone() : null;
			path.Dispose();

			return result;
		}

		internal RectangleF getTextRect(PathData data, float rotationAngle)
		{
			GraphicsPath path = null;

			if (textArea == null)
			{
				path = getPath(data, rotationAngle);
			}
			else
			{
				path = new GraphicsPath(fillMode);

				int offset = elements.Length;
				if (decorations != null)
					offset += decorations.Length;
				for (int i = 0; i < textArea.Length; ++i)
					textArea[i].updatePath(path, data.getData(offset + i));
				path.CloseFigure();

				rotatePath(data, path, rotationAngle);
			}

			RectangleF result = path.GetBounds();
			path.Dispose();

			return result;
		}

		internal bool isTextOutside()
		{
			return textOutside;
		}

		public object Clone()
		{
			ElementTemplate[] clonedElements =
				new ElementTemplate[elements.GetLength(0)];
			for (int i = 0; i < elements.GetLength(0); ++i)
				clonedElements[i] = (ElementTemplate)elements[i].Clone();

			ElementTemplate[] clonedDecorations = null;
			if (decorations != null)
			{
				clonedDecorations = 
					new ElementTemplate[decorations.Length];
				for (int i = 0; i < decorations.Length; i++)
					clonedDecorations[i] = (ElementTemplate)decorations[i].Clone();
			}

			ElementTemplate[] clonedTextArea = null;
			if (textArea != null)
			{
				clonedTextArea = 
					new ElementTemplate[textArea.Length];
				for (int i = 0; i < textArea.Length; i++)
					clonedTextArea[i] = (ElementTemplate)textArea[i].Clone();
			}

			return new ShapeTemplate(clonedElements,
				clonedDecorations, clonedTextArea, fillMode);
		}

		public bool Equals(ShapeTemplate obj)
		{
			int i;
			ShapeTemplate tmpl = (ShapeTemplate)obj;

			if(elements.Length != tmpl.elements.Length)
				return false;
			for(i = 0; i < elements.Length; i++)
				if(!elements[i].SameAs(tmpl.elements[i]))
					return false;

			if((decorations != null && tmpl.decorations == null) ||
				(decorations == null && tmpl.decorations != null))
				return false;
			if(decorations != null)
			{
				if(decorations.Length != tmpl.decorations.Length)
					return false;

				for(i = 0; i < decorations.Length; i++)
					if(!decorations[i].SameAs(tmpl.decorations[i]))
						return false;
			}

			if((textArea != null && tmpl.textArea == null) ||
				(textArea == null && tmpl.textArea != null))
				return false;
			if(textArea != null)
			{
				if(textArea.Length != tmpl.textArea.Length)
					return false;

				for(i = 0; i < textArea.Length; i++)
					if(!textArea[i].SameAs(tmpl.textArea[i]))
						return false;
			}

			if(fillMode != tmpl.fillMode)
				return false;

			return true;
		}

		public ElementTemplate[] Outline
		{
			get { return elements; }
		}

		public ElementTemplate[] Decorations
		{
			get { return decorations; }
		}

		public ElementTemplate[] TagArea
		{
			get { return textArea; }
		}

		public Image Image
		{
			get { return image; }
			set { image = value; }
		}

		public RectangleF ImageRectangle
		{
			get { return imageRectangle; }
			set { imageRectangle = value; }
		}

		private ElementTemplate[] elements;
		private ElementTemplate[] decorations;
		private ElementTemplate[] textArea;
		private FillMode fillMode;
		private Image image;
		private RectangleF imageRectangle = new RectangleF(0, 0, 100, 100);

		public string Id
		{
			get
			{
				if (!defaultShapes.ContainsValue(this))
					return "";

				int ix = defaultShapes.IndexOfValue(this);
				if (ix == -1)
					return "";

				return (string)defaultShapes.GetKey(ix);
			}
		}

		internal bool hasTextArea()
		{
			if (textArea == null) return false;
			if (textArea.Length == 0) return false;
			return true;
		}

		/// <summary>
		/// Obtain a read-only collection of all named shapes.
		/// </summary>
		public static ShapeCollection Shapes
		{
			get
			{
				return new ShapeCollection(defaultShapes.Values);
			}
		}

		private static SortedList defaultShapes = new SortedList();
		private static bool defaultShapesDefined = false;

		public static ShapeTemplate FromId(string id)
		{
			if (!defaultShapes.ContainsKey(id))
				return null;

			return (ShapeTemplate)defaultShapes[id];
		}

		static ShapeTemplate()
		{
			lock (defaultShapes)
			{
				// Fill the list with all default shapes.
				// Each shape has associated string id to it
				if (defaultShapes.Count == 0 || !defaultShapesDefined)
				{
					defaultShapesDefined = true;

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100),
							new LineTemplate(100, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						FillMode.Winding, "Rectangle");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(50, 0, 100, 50),
							new LineTemplate(100, 50, 50, 100),
							new LineTemplate(50, 100, 0, 50),
							new LineTemplate(0, 50, 50, 0)
						},
						FillMode.Winding, "Decision");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 50, 0),
							new BezierTemplate(50, 0, 80, 0, 100, 30, 100, 50),
							new BezierTemplate(100, 50, 100, 70, 80, 100, 50, 100),
							new LineTemplate(50, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						FillMode.Winding, "Delay");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100),
							new LineTemplate(100, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 0, 15, 100),
							new LineTemplate(85, 0, 85, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 0, 85, 0),
							new LineTemplate(85, 0, 85, 100),
							new LineTemplate(85, 100, 15, 100),
							new LineTemplate(15, 100, 15, 0)
						},
						FillMode.Winding, "Procedure");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new ArcTemplate(0, 0, 100, 100, 0, 360)
						},
						FillMode.Winding, "Ellipse");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 50, 0),
							new LineTemplate(50, 0, 100, 100),
							new LineTemplate(100, 100, 0, 100)
						},
						FillMode.Winding, "Alternative");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(40, 50, 0, 0),
							new LineTemplate(0, 0, 60, 0),
							new LineTemplate(60, 0, 100, 50),
							new LineTemplate(100, 50, 60, 100),
							new LineTemplate(60, 100, 0, 100),
							new LineTemplate(0, 100, 40, 50)
						},
						FillMode.Winding, "Arrow1");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 25, 0),
							new LineTemplate(25, 0, 25, 40),
							new LineTemplate(25, 40, 60, 40),
							new LineTemplate(60, 40, 60, 0),
							new LineTemplate(60, 0, 100, 50),
							new LineTemplate(100, 50, 60, 100),
							new LineTemplate(60, 100, 60, 60),
							new LineTemplate(60, 60, 25, 60),
							new LineTemplate(25, 60, 25, 100),
							new LineTemplate(25, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						FillMode.Winding, "Arrow2");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 30, 60, 30),
							new LineTemplate(60, 30, 60, 0),
							new LineTemplate(60, 0, 100, 50),
							new LineTemplate(100, 50, 60, 100),
							new LineTemplate(60, 100, 60, 70),
							new LineTemplate(60, 70, 0, 70),
							new LineTemplate(0, 70, 0, 30)
						},
						FillMode.Winding, "Arrow3");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 30, 60, 30),
							new LineTemplate(60, 30, 60, 0),
							new LineTemplate(60, 0, 100, 50),
							new LineTemplate(100, 50, 60, 100),
							new LineTemplate(60, 100, 60, 70),
							new LineTemplate(60, 70, 0, 70),
							new LineTemplate(0, 70, 20, 50),
							new LineTemplate(20, 50, 0, 30)
						},
						FillMode.Winding, "Arrow4");
				
					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(30, 30, 30, 0),
							new LineTemplate(30, 0, 0, 50),
							new LineTemplate(0, 50, 30, 100),
							new LineTemplate(30, 100, 30, 70),
							new LineTemplate(30, 70, 70, 70),
							new LineTemplate(70, 70, 70, 100),
							new LineTemplate(70, 100, 100, 50),
							new LineTemplate(100, 50, 70, 0),
							new LineTemplate(70, 0, 70, 30),
							new LineTemplate(70, 30, 30, 30)
						},
						FillMode.Winding, "Arrow5");
				
					new ShapeTemplate(
						new ElementTemplate[]
						{
							new BezierTemplate(0, 100, 50, 66, 50, 33, 0, 0),
							new LineTemplate(0, 0, 100, 50),
							new LineTemplate(100, 50, 0, 100)
						},
						FillMode.Winding, "Arrow6");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 70, 0),
							new LineTemplate(70, 0, 100, 50),
							new LineTemplate(100, 50, 70, 100),
							new LineTemplate(70, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						FillMode.Winding, "Arrow7");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(40, 0, 40, 35),
							new LineTemplate(40, 35, 0, 0),
							new LineTemplate(0, 0, 0, 100),
							new LineTemplate(0, 100, 40, 65),
							new LineTemplate(40, 65, 40, 100),
							new LineTemplate(40, 100, 100, 50),
							new LineTemplate(100, 50, 40, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(40, 35, 40, 65)
						},
						null,
						FillMode.Winding, "Arrow8");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 25, 0, 25),
							new LineTemplate(0, 25, 0, 100),
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 100, 25),
							new LineTemplate(100, 25, 75, 0),
							new LineTemplate(75, 0, 25, 0),
							new LineTemplate(25, 0, 0, 25)
						},
						FillMode.Winding, "BeginLoop");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(100, 100, 0, 100),
							new LineTemplate(0, 100, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 100, 100)
						},
						FillMode.Winding, "Collate");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new BezierTemplate(0, 80, 6, 94, 30, 100, 50, 100),
							new BezierTemplate(50, 100, 70, 100, 94, 94, 100, 80),
							new LineTemplate(100, 80, 50, 0),
							new LineTemplate(50, 0, 0, 80)
						},
						new ElementTemplate[]
						{
							new BezierTemplate(0, 80, 10, 65, 30, 60, 50, 60),
							new BezierTemplate(50, 60, 70, 60, 90, 65, 100, 80)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 80, 50, 0),
							new LineTemplate(50, 0, 100, 80),
							new BezierTemplate(100, 80, 94, 66, 70, 60, 50, 60),
							new BezierTemplate(50, 60, 30, 60, 6, 66, 0, 80)
						},
						FillMode.Winding, "ConeDown");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new BezierTemplate(0, 80, 6, 94, 30, 100, 50, 100),
							new BezierTemplate(50, 100, 70, 100, 94, 94, 100, 80),
							new LineTemplate(100, 80, 50, 0),
							new LineTemplate(50, 0, 0, 80)
						},
						FillMode.Winding, "ConeUp");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 0, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 15, 100, 15),
							new LineTemplate(0, 85, 100, 85)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 15, 100, 15),
							new LineTemplate(100, 15, 100, 85),
							new LineTemplate(100, 85, 0, 85),
							new LineTemplate(0, 85, 0, 15)
						},
						FillMode.Winding, "CreateRequest");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(40, 40, 40, 0),
							new LineTemplate(40, 0, 60, 0),
							new LineTemplate(60, 0, 60, 40),
							new LineTemplate(60, 40, 100, 40),
							new LineTemplate(100, 40, 100, 60),
							new LineTemplate(100, 60, 60, 60),
							new LineTemplate(60, 60, 60, 100),
							new LineTemplate(60, 100, 40, 100),
							new LineTemplate(40, 100, 40, 60),
							new LineTemplate(40, 60, 0, 60),
							new LineTemplate(0, 60, 0, 40),
							new LineTemplate(0, 40, 40, 40)
						},
						FillMode.Winding, "Cross");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(100, 20, 100, 80),
							new BezierTemplate(100, 80, 100, 95, 70, 100, 50, 100),
							new BezierTemplate(50, 100, 30, 100, 0, 95, 0, 80),
							new LineTemplate(0, 80, 0, 20),
							new BezierTemplate(0, 20, 0, 5, 30, 0, 50, 0),
							new BezierTemplate(50, 0, 70, 0, 100, 5, 100, 20)
						},
						new ElementTemplate[]
						{
							new BezierTemplate(100, 20, 100, 35, 70, 40, 50, 40),
							new BezierTemplate(50, 40, 30, 40, 0, 35, 0, 20)
						},
						new ElementTemplate[]
						{
							new LineTemplate(100, 20, 100, 80),
							new BezierTemplate(100, 80, 100, 95, 70, 100, 50, 100),
							new BezierTemplate(50, 100, 30, 100, 0, 95, 0, 80),
							new LineTemplate(0, 80, 0, 20),
							new BezierTemplate(0, 20, 16, 35, 30, 40, 50, 40),
							new BezierTemplate(50, 40, 70, 40, 100, 35, 100, 20)
						},
						FillMode.Winding, "Cylinder");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 50, 25, 100),
							new LineTemplate(25, 100, 75, 100),
							new LineTemplate(75, 100, 100, 50),
							new LineTemplate(100, 50, 75, 0),
							new LineTemplate(75, 0, 25, 0),
							new LineTemplate(25, 0, 0, 50)
						},
						FillMode.Winding, "Decision2");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 100, 100),
							new BezierTemplate(100, 100, 85, 100, 67, 75, 67, 50),
							new BezierTemplate(67, 50, 67, 25, 85, 0, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new BezierTemplate(0, 0, 15, 0, 33, 25, 33, 50),
							new BezierTemplate(33, 50, 33, 75, 15, 100, 0, 100)
						},
						FillMode.Winding, "DInDelay");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(33, 100, 67, 100),
							new BezierTemplate(67, 100, 80, 100, 100, 75, 100, 50),
							new BezierTemplate(100, 50, 100, 25, 85, 0, 67, 0),
							new LineTemplate(67, 0, 33, 0),
							new BezierTemplate(33, 0, 15, 0, 0, 25, 0, 50),
							new BezierTemplate(0, 50, 0, 75, 15, 100, 33, 100)
						},
						FillMode.Winding, "DOutDelay");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(100, 20, 100, 80),
							new BezierTemplate(100, 80, 100, 95, 70, 100, 50, 100),
							new BezierTemplate(50, 100, 30, 100, 0, 95, 0, 80),
							new LineTemplate(0, 80, 0, 20),
							new BezierTemplate(0, 20, 0, 5, 30, 0, 50, 0),
							new BezierTemplate(50, 0, 70, 0, 100, 5, 100, 20)
						},
						new ElementTemplate[]
						{
							new BezierTemplate(100, 20, 100, 35, 70, 40, 50, 40),
							new BezierTemplate(50, 40, 30, 40, 0, 35, 0, 20),
							new BezierTemplate(100, 30, 100, 45, 70, 50, 50, 50),
							new BezierTemplate(50, 50, 30, 50, 0, 45, 0, 30)
						},
						new ElementTemplate[]
						{
							new LineTemplate(100, 30, 100, 80),
							new BezierTemplate(100, 80, 100, 95, 70, 100, 50, 100),
							new BezierTemplate(50, 100, 30, 100, 0, 95, 0, 80),
							new LineTemplate(0, 80, 0, 30),
							new BezierTemplate(0, 30, 16, 45, 30, 50, 50, 50),
							new BezierTemplate(50, 50, 70, 50, 100, 45, 100, 30)
						},
						FillMode.Winding, "DiskStorage");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(10, 100, 90, 100),
							new BezierTemplate(90, 100, 95, 100, 100, 95, 100, 90),
							new LineTemplate(100, 90, 100, 10),
							new BezierTemplate(100, 10, 100, 5, 95, 0, 90, 0),
							new LineTemplate(90, 0, 10, 0),
							new BezierTemplate(10, 0, 5, 0, 0, 5, 0, 10),
							new LineTemplate(0, 10, 0, 90),
							new BezierTemplate(0, 90, 0, 95, 5, 100, 10, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 15, 100, 15)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 15, 100, 15),
							new LineTemplate(100, 15, 100, 100),
							new LineTemplate(100, 100, 0, 100),
							new LineTemplate(0, 100, 0, 15)
						},
						FillMode.Winding, "DividedEvent");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 0, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 15, 100, 15)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 15, 100, 15),
							new LineTemplate(100, 15, 100, 100),
							new LineTemplate(100, 100, 0, 100),
							new LineTemplate(0, 100, 0, 15)
						},
						FillMode.Winding, "DividedProcess");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 90, 0, 0),
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 100, 90),
							new BezierTemplate(100, 90, 83, 80, 66, 80, 50, 90),
							new BezierTemplate(50, 90, 33, 100, 16, 100, 0, 90)
						},
						FillMode.Winding, "Document");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(100, 75, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 0, 75),
							new LineTemplate(0, 75, 25, 100),
							new LineTemplate(25, 100, 75, 100),
							new LineTemplate(75, 100, 100, 75)
						},
						FillMode.Winding, "EndLoop");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new BezierTemplate(50, 20, 50, 5, 65, 0, 75, 0),
							new BezierTemplate(75, 0, 85, 0, 100, 10, 100, 25),
							new BezierTemplate(100, 25, 100, 50, 67, 75, 50, 100),
							new BezierTemplate(50, 100, 33, 75, 0, 50, 0, 25),
							new BezierTemplate(0, 25, 0, 10, 15, 0, 25, 0),
							new BezierTemplate(25, 0, 35, 0, 50, 5, 50, 20)
						},
						FillMode.Winding, "Heart");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 0, 100),
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 100, 0),
							new LineTemplate(100, 0, 0, 40),
							new LineTemplate(0, 40, 0, 100)
						},
						FillMode.Winding, "Input");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 0, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 15, 100, 15),
							new LineTemplate(15, 0, 15, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 15, 100, 15),
							new LineTemplate(100, 15, 100, 100),
							new LineTemplate(100, 100, 15, 100),
							new LineTemplate(15, 100, 15, 15)
						},
						FillMode.Winding, "InternalStorage");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 80, 50),
							new LineTemplate(80, 50, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 0, 100)
						},
						FillMode.Winding, "MessageFromUser");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(100, 100, 20, 100),
							new LineTemplate(20, 100, 0, 50),
							new LineTemplate(0, 50, 20, 0),
							new LineTemplate(20, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100)
						},
						FillMode.Winding, "MessageToUser");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 92, 0, 20),
							new LineTemplate(0, 20, 10, 20),
							new LineTemplate(10, 20, 10, 10),
							new LineTemplate(10, 10, 20, 10),
							new LineTemplate(20, 10, 20, 0),
							new LineTemplate(20, 0, 100, 0),
							new LineTemplate(100, 0, 100, 72),
							new BezierTemplate(100, 72, 95, 69, 90, 69, 90, 69),
							new LineTemplate(90, 69, 90, 82),
							new BezierTemplate(90, 82, 85, 79, 80, 79, 80, 79),
							new LineTemplate(80, 79, 80, 92),
							new BezierTemplate(80, 92, 66, 84, 53, 84, 39, 92),
							new BezierTemplate(39, 92, 26, 100, 13, 100, 0, 92)
						},
						new ElementTemplate[]
						{
							new LineTemplate(10, 20, 80, 20),
							new LineTemplate(80, 20, 80, 79),
							new LineTemplate(20, 10, 90, 10),
							new LineTemplate(90, 10, 90, 69)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 92, 0, 20),
							new LineTemplate(0, 20, 80, 20),
							new LineTemplate(80, 20, 80, 20),
							new LineTemplate(80, 20, 80, 92),
							new BezierTemplate(80, 92, 66, 84, 53, 84, 39, 92),
							new BezierTemplate(39, 92, 26, 100, 13, 100, 0, 92)
						},
						FillMode.Winding, "MultiDocument");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 50, 100),
							new LineTemplate(50, 100, 100, 0),
							new LineTemplate(100, 0, 0, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(10, 20, 90, 20)
						},
						new ElementTemplate[]
						{
							new LineTemplate(50, 100, 10, 20),
							new LineTemplate(10, 20, 90, 20),
							new LineTemplate(90, 20, 50, 100)
						},
						FillMode.Winding, "OfflineStorage");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 70, 0, 0),
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 100, 70),
							new LineTemplate(100, 70, 50, 100),
							new LineTemplate(50, 100, 0, 70)
						},
						FillMode.Winding, "OffpageReference");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 80, 50),
							new LineTemplate(80, 50, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 0, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 0, 15, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(20, 0, 20, 100),
							new LineTemplate(20, 100, 100, 100),
							new LineTemplate(100, 100, 80, 50),
							new LineTemplate(80, 50, 100, 0),
							new LineTemplate(100, 0, 20, 0)
						},
						FillMode.Winding, "PrimitiveFromCall");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(100, 100, 20, 100),
							new LineTemplate(20, 100, 0, 50),
							new LineTemplate(0, 50, 20, 0),
							new LineTemplate(20, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(85, 0, 85, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(80, 0, 80, 100),
							new LineTemplate(80, 100, 20, 100),
							new LineTemplate(20, 100, 0, 50),
							new LineTemplate(0, 50, 20, 0),
							new LineTemplate(20, 0, 80, 0)
						},
						FillMode.Winding, "PrimitiveToCall");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100),
							new LineTemplate(100, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 0, 15, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100),
							new LineTemplate(100, 100, 15, 100),
							new LineTemplate(15, 100, 15, 0)
						},
						FillMode.Winding, "Process");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(50, 0, 100, 20),
							new LineTemplate(100, 20, 100, 80),
							new LineTemplate(100, 80, 50, 100),
							new LineTemplate(50, 100, 50, 100),
							new LineTemplate(50, 100, 0, 80),
							new LineTemplate(0, 80, 0, 20),
							new LineTemplate(0, 20, 50, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 20, 50, 40),
							new LineTemplate(50, 40, 50, 100),
							new LineTemplate(50, 40, 100, 20)
						},
						null,
						FillMode.Winding, "Prysm");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(50, 100, 100, 80),
							new LineTemplate(100, 80, 50, 0),
							new LineTemplate(50, 0, 0, 80),
							new LineTemplate(0, 80, 50, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 80, 50, 60),
							new LineTemplate(50, 0, 50, 60),
							new LineTemplate(100, 80, 50, 60),
						},
						null,
						FillMode.Winding, "PyramidDown");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(50, 100, 100, 80),
							new LineTemplate(100, 80, 50, 0),
							new LineTemplate(50, 0, 0, 80),
							new LineTemplate(0, 80, 50, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(50, 0, 50, 100)
						},
						null,
						FillMode.Winding, "PyramidUp");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 0, 100),
							new LineTemplate(0, 100, 0, 30),
							new LineTemplate(0, 30, 30, 0),
							new LineTemplate(30, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100),
							new LineTemplate(100, 100, 0, 100)
						},
						FillMode.Winding, "PunchedCard");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 25, 0),
							new LineTemplate(25, 0, 100, 0),
							new LineTemplate(100, 0, 75, 100),
							new LineTemplate(75, 100, 0, 100)
						},
						FillMode.Winding, "Save");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(50, 0, 100, 50),
							new LineTemplate(100, 50, 50, 100),
							new LineTemplate(50, 100, 0, 50),
							new LineTemplate(0, 50, 50, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 50, 100, 50)
						},
						null,
						FillMode.Winding, "Sort");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new ArcTemplate(0, 0, 100, 100, 0, 360)
						},
						new ElementTemplate[]
						{
							new BezierTemplate(50, 0, 25, 25, 25, 75, 50, 100)
						},
						new ElementTemplate[]
						{
							new BezierTemplate(50, 100, 25, 75, 25, 25, 50, 0),
							new ArcTemplate(50, 0, 50, 100, 270, 180)
						},
						FillMode.Winding, "Sphere");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new BezierTemplate(15, 100, 5, 100, 0, 75, 0, 50),
							new BezierTemplate(0, 50, 0, 25, 5, 0, 15, 0),
							new LineTemplate(15, 0, 85, 0),
							new BezierTemplate(85, 0, 95, 0, 100, 25, 100, 50),
							new BezierTemplate(100, 50, 100, 75, 95, 100, 85, 100),
							new LineTemplate(85, 100, 15, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 0, 15, 100),
							new LineTemplate(85, 0, 85, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 0, 80, 0),
							new LineTemplate(80, 0, 80, 100),
							new LineTemplate(80, 100, 15, 100),
							new LineTemplate(15, 100, 15, 0)
						},
						FillMode.Winding, "Start");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 67, 100),
							new BezierTemplate(67, 100, 85, 100, 100, 75, 100, 50),
							new BezierTemplate(100, 50, 100, 25, 85, 0, 67, 0),
							new LineTemplate(67, 0, 0, 0),
							new BezierTemplate(0, 0, 15, 0, 33, 25, 33, 50),
							new BezierTemplate(33, 50, 33, 75, 15, 100, 0, 100)
						},
						FillMode.Winding, "DDelay");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(20, 100, 80, 100),
							new BezierTemplate(80, 100, 90, 100, 100, 75, 100, 50),
							new BezierTemplate(100, 50, 100, 25, 90, 0, 80, 0),
							new LineTemplate(80, 0, 20, 0),
							new BezierTemplate(20, 0, 10, 0, 0, 25, 0, 50),
							new BezierTemplate(0, 50, 0, 75, 10, 100, 20, 100)
						},
						FillMode.Winding, "Terminator");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 20, 100),
							new LineTemplate(20, 100, 80, 100),
							new LineTemplate(80, 100, 100, 0)
						},
						FillMode.Winding, "ManualOperation");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new BezierTemplate(40, 100, 15, 100, 0, 70, 0, 50),
							new BezierTemplate(0, 50, 0, 30, 15, 0, 40, 0),
							new LineTemplate(40, 0, 85, 0),
							new BezierTemplate(85, 0, 100, 20, 100, 30, 100, 50),
							new BezierTemplate(100, 50, 100, 70, 100, 80, 85, 100),
							new LineTemplate(85, 100, 40, 100)
						},
						FillMode.Winding, "Display");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 80, 100),
							new LineTemplate(80, 100, 80, 90),
							new LineTemplate(80, 90, 90, 90),
							new LineTemplate(90, 90, 90, 80),
							new LineTemplate(90, 80, 100, 80),
							new LineTemplate(100, 80, 100, 0),
							new LineTemplate(100, 0, 20, 0),
							new LineTemplate(20, 0, 20, 10),
							new LineTemplate(20, 10, 10, 10),
							new LineTemplate(10, 10, 10, 20),
							new LineTemplate(10, 20, 0, 20),
							new LineTemplate(0, 20, 0, 100)
						},
						new ElementTemplate[]
						{
							new LineTemplate(10, 20, 80, 20),
							new LineTemplate(80, 20, 80, 90),
							new LineTemplate(20, 10, 90, 10),
							new LineTemplate(90, 10, 90, 80)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 80, 100),
							new LineTemplate(80, 100, 80, 20),
							new LineTemplate(80, 20, 0, 20),
							new LineTemplate(0, 20, 0, 100)
						},
						FillMode.Winding, "MultiProc");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 90, 0, 0),
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 100, 90),
							new BezierTemplate(100, 90, 83, 80, 66, 80, 50, 90),
							new BezierTemplate(50, 90, 33, 100, 16, 100, 0, 90)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 0, 15, 96)
						},
						new ElementTemplate[]
						{
							new LineTemplate(16, 94, 16, 0),
							new LineTemplate(16, 0, 100, 0),
							new LineTemplate(100, 0, 100, 90),
							new BezierTemplate(100, 90, 83, 80, 66, 80, 50, 90),
							new BezierTemplate(50, 90, 33, 100, 24, 100, 16, 94)
						},
						FillMode.Winding, "LinedDocument");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(100, 20, 100, 80),
							new BezierTemplate(100, 80, 100, 95, 70, 100, 50, 100),
							new BezierTemplate(50, 100, 30, 100, 0, 95, 0, 80),
							new LineTemplate(0, 80, 0, 20),
							new BezierTemplate(0, 20, 0, 5, 30, 0, 50, 0),
							new BezierTemplate(50, 0, 70, 0, 100, 5, 100, 20)
						},
						new ElementTemplate[]
						{
							new BezierTemplate(100, 20, 100, 35, 70, 40, 50, 40),
							new BezierTemplate(50, 40, 30, 40, 0, 35, 0, 20),
							new BezierTemplate(100, 30, 100, 45, 70, 50, 50, 50),
							new BezierTemplate(50, 50, 30, 50, 0, 45, 0, 30),
							new BezierTemplate(100, 40, 100, 55, 70, 60, 50, 60),
							new BezierTemplate(50, 60, 30, 60, 0, 55, 0, 40)
						},
						new ElementTemplate[]
						{
							new LineTemplate(100, 40, 100, 80),
							new BezierTemplate(100, 80, 100, 95, 70, 100, 50, 100),
							new BezierTemplate(50, 100, 30, 100, 0, 95, 0, 80),
							new LineTemplate(0, 80, 0, 40),
							new BezierTemplate(0, 40, 16, 55, 30, 60, 50, 60),
							new BezierTemplate(50, 60, 70, 60, 100, 55, 100, 40)
						},
						FillMode.Winding, "Database");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(15, 100, 85, 100),
							new LineTemplate(85, 100, 100, 50),
							new LineTemplate(100, 50, 85, 0),
							new LineTemplate(85, 0, 15, 0),
							new LineTemplate(15, 0, 0, 50),
							new LineTemplate(0, 50, 15, 100)
						},
						FillMode.Winding, "DataTransmition");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new BezierTemplate(67, 0, 58, 2, 61, 7, 60, 19),
							new BezierTemplate(60, 19, 49, 11, 51, 8, 43, 14),
							new BezierTemplate(43, 14, 37, 19, 40, 25, 41, 33),
							new BezierTemplate(41, 33, 35, 24, 27, 13, 13, 21),
							new BezierTemplate(13, 21, 10, 25, 7, 29, 15, 44),
							new BezierTemplate(15, 44, 5, 47, 0, 50, 0, 63),
							new BezierTemplate(0, 63, 3, 74, 17, 72, 27, 71),
							new BezierTemplate(27, 71, 21, 77, 14, 93, 27, 100),
							new BezierTemplate(27, 100, 38, 100, 52, 93, 55, 78),
							new BezierTemplate(55, 78, 70, 93, 81, 87, 88, 80),
							new BezierTemplate(88, 80, 91, 69, 87, 63, 79, 57),
							new BezierTemplate(79, 57, 90, 52, 100, 43, 100, 24),
							new BezierTemplate(100, 24, 96, 7, 77, 0, 67, 0)
						},
						FillMode.Winding, "Cloud");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(60, 90, 100, 100),
							new LineTemplate(100, 100, 100, 0),
							new LineTemplate(100, 0, 0, 0),
							new LineTemplate(0, 0, 0, 90),
							new LineTemplate(0, 90, 60, 90)
						},
						FillMode.Winding, "TransmittalTape");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100),
							new LineTemplate(100, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(5, 5, 95, 5),
							new LineTemplate(95, 5, 95, 95),
							new LineTemplate(95, 95, 5, 95),
							new LineTemplate(5, 95, 5, 5)
						},
						new ElementTemplate[]
						{
							new LineTemplate(5, 5, 95, 5),
							new LineTemplate(95, 5, 95, 95),
							new LineTemplate(95, 95, 5, 95),
							new LineTemplate(5, 95, 5, 5)
						},
						FillMode.Winding, "FramedRectangle");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 90, 0, 10),
							new BezierTemplate(0, 10, 16, 20, 33, 20, 50, 10),
							new BezierTemplate(50, 10, 66, 0, 83, 0, 100, 10),
							new LineTemplate(100, 10, 100, 90),
							new BezierTemplate(100, 90, 83, 80, 66, 80, 50, 90),
							new BezierTemplate(50, 90, 33, 100, 16, 100, 0, 90)
						},
						FillMode.Winding, "Microform");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 80, 75),
							new LineTemplate(80, 75, 100, 85),
							new LineTemplate(100, 85, 100, 15),
							new LineTemplate(100, 15, 80, 25),
							new LineTemplate(80, 25, 0, 0),
							new LineTemplate(0, 0, 0, 100)
						},
						FillMode.Winding, "MicroformRecording");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 50, 74),
							new LineTemplate(50, 74, 100, 100),
							new LineTemplate(100, 100, 100, 0),
							new LineTemplate(100, 0, 50, 25),
							new LineTemplate(50, 25, 0, 0),
							new LineTemplate(0, 0, 0, 100)
						},
						FillMode.Winding, "MicroformProcessing");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 10, 40, 50),
							new LineTemplate(40, 50, 0, 90),
							new LineTemplate(0, 90, 10, 100),
							new LineTemplate(10, 100, 50, 60),
							new LineTemplate(50, 60, 90, 100),
							new LineTemplate(90, 100, 100, 90),
							new LineTemplate(100, 90, 60, 50),
							new LineTemplate(60, 50, 100, 10),
							new LineTemplate(100, 10, 90, 0),
							new LineTemplate(90, 0, 50, 40),
							new LineTemplate(50, 40, 10, 0),
							new LineTemplate(10, 0, 0, 10)
						},
						FillMode.Winding, "ConnectedIssues");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 100, 100),
							new LineTemplate(100, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(10, 0, 0, 10),
							new LineTemplate(90, 0, 100, 10),
							new LineTemplate(90, 100, 100, 90),
							new LineTemplate(0, 90, 10, 100)
						},
						null,
						FillMode.Winding, "ExternalOrganization");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(50, 0, 100, 50),
							new LineTemplate(100, 50, 50, 100),
							new LineTemplate(50, 100, 0, 50),
							new LineTemplate(0, 50, 50, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(40, 10, 60, 10),
							new LineTemplate(40, 90, 60, 90),
							new LineTemplate(10, 40, 10, 60),
							new LineTemplate(90, 40, 90, 60)
						},
						null,
						FillMode.Winding, "ExternalProcess");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 0, 100),
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 100, 30),
							new LineTemplate(100, 30, 70, 0),
							new LineTemplate(70, 0, 0, 0)
						},
						new ElementTemplate[]
						{
							new LineTemplate(70, 0, 70, 30),
							new LineTemplate(70, 30, 100, 30)
						},
						new ElementTemplate[]
						{
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 100, 30),
							new LineTemplate(100, 30, 70, 30),
							new LineTemplate(70, 30, 70, 0),
							new LineTemplate(70, 0, 0, 0),
							new LineTemplate(0, 0, 0, 100)
						},
						FillMode.Winding, "File");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(50, 0, 0, 50),
							new LineTemplate(0, 50, 15, 100),
							new LineTemplate(15, 100, 85, 100),
							new LineTemplate(85, 100, 100, 50),
							new LineTemplate(100, 50, 50, 0)
						},
						FillMode.Winding, "Pentagon");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(28, 100, 0, 70),
							new LineTemplate(0, 70, 10, 25),
							new LineTemplate(10, 25, 50, 0),
							new LineTemplate(50, 0, 90, 25),
							new LineTemplate(90, 25, 100, 70),
							new LineTemplate(100, 70, 72, 100),
							new LineTemplate(72, 100, 28, 100)
						},
						FillMode.Winding, "Heptagon");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(33, 100, 0, 67),
							new LineTemplate(0, 67, 0, 33),
							new LineTemplate(0, 33, 33, 0),
							new LineTemplate(33, 0, 67, 0),
							new LineTemplate(67, 0, 100, 33),
							new LineTemplate(100, 33, 100, 67),
							new LineTemplate(100, 67, 67, 100),
							new LineTemplate(67, 100, 33, 100)
						},
						FillMode.Winding, "Octagon");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(30, 100, 100, 100),
							new LineTemplate(100, 100, 70, 0),
							new LineTemplate(70, 0, 0, 0),
							new LineTemplate(0, 0, 30, 100)
						},
						FillMode.Winding, "RSave");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(50, 0, 50, 0),
							new LineTemplate(50, 0, 40, 33),
							new LineTemplate(40, 33, 0, 30),
							new LineTemplate( 0, 30, 30, 50),
							new LineTemplate(30, 50, 15, 100),
							new LineTemplate(15, 100, 50, 60),
							new LineTemplate(50, 60, 85, 100),
							new LineTemplate(85, 100, 67, 50),
							new LineTemplate(67, 50, 100, 30),
							new LineTemplate(100, 30, 60, 33),
							new LineTemplate(60, 33, 50, 0)
						},
						FillMode.Winding, "Star");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(30, 0, 30, 0),
							new LineTemplate(30, 0, 15, 40),
							new LineTemplate(15, 40, 45, 40),
							new LineTemplate(45, 40, 30, 100),
							new LineTemplate(30, 100, 65, 30),
							new LineTemplate(65, 30, 33, 27),
							new LineTemplate(33, 27, 56, 0),
							new LineTemplate(56, 0, 30, 0)
						},
						FillMode.Winding, "Lightning");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 30, 50),
							new LineTemplate(30, 50, 0, 100),
							new LineTemplate(0, 100, 100, 50),
							new LineTemplate(100, 50, 0, 0)
						},
						FillMode.Winding, "Arrow9");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 30, 50),
							new LineTemplate(30, 50, 0, 100),
							new LineTemplate(0, 100, 100, 100),
							new LineTemplate(100, 100, 70, 50),
							new LineTemplate(70, 50, 100, 0),
							new LineTemplate(100, 0, 0, 0)
						},
						FillMode.Winding, "Hourglass");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(70, 100, 70, 100),
							new LineTemplate(70, 100, 0, 100),
							new LineTemplate(0, 100, 0, 30),
							new LineTemplate(0, 30, 70, 30),
							new LineTemplate(70, 30, 0, 30),
							new LineTemplate(0, 30, 30, 0),
							new LineTemplate(30, 0, 100, 0),
							new LineTemplate(100, 0, 70, 30),
							new LineTemplate(70, 30, 100, 0),
							new LineTemplate(100, 0, 100, 70),
							new LineTemplate(100, 70, 70, 100),
							new LineTemplate(70, 100, 70, 30),
							new LineTemplate(70, 30, 70, 100)
						},
						null,
						new ElementTemplate[]
						{
							new LineTemplate(70, 100, 0, 100),
							new LineTemplate(0, 100, 0, 30),
							new LineTemplate(0, 30, 70, 30),
							new LineTemplate(70, 30, 70, 100)
						},
						FillMode.Winding, "Cube");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 0, 100),
							new LineTemplate(0, 100, 100, 50),
							new LineTemplate(100, 50, 100, 100),
							new LineTemplate(100, 100, 100, 0),
							new LineTemplate(100, 0, 100, 50),
							new LineTemplate(100, 50, 0, 0)
						},
						FillMode.Winding, "Interrupt");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 0, 100),
							new LineTemplate(0, 100, 80, 50),
							new LineTemplate(80, 50, 80, 20),
							new LineTemplate(80, 20, 100, 0),
							new LineTemplate(100, 0, 80, 20),
							new LineTemplate(80, 20, 80, 80),
							new LineTemplate(80, 80, 60, 100),
							new LineTemplate(60, 100, 80, 80),
							new LineTemplate(80, 80, 80, 50),
							new LineTemplate(80, 50, 0, 0)
						},
						FillMode.Winding, "Interrupt2");

					float y = (float)Math.Sqrt(3) * 25 + 50;
					new ShapeTemplate(
						new ElementTemplate[]
						{
							new ArcTemplate(0, 0, 100, 100, 90, 330),
							new LineTemplate(75, y, 100, y),
							new LineTemplate(100, y, 100, 100),
							new LineTemplate(100, 100, 50, 100)
						},
						FillMode.Winding, "Tape");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new ArcTemplate(40, 0, 20, 20, 0, 360),
							new LineTemplate(50, 20, 70, 20),
							new ArcTemplate(65, 20, 10, 10, 270, 90),
							new LineTemplate(75, 25, 75, 60),
							new LineTemplate(75, 60, 68, 60),
							new LineTemplate(68, 60, 68, 30),
							new LineTemplate(68, 30, 64, 30),
							new LineTemplate(64, 30, 64, 100),
							new LineTemplate(64, 100, 52, 100),
							new LineTemplate(52, 100, 52, 65),
							new LineTemplate(52, 65, 48, 65),
							new LineTemplate(48, 65, 48, 100),
							new LineTemplate(48, 100, 36, 100),
							new LineTemplate(36, 100, 36, 30),
							new LineTemplate(36, 30, 32, 30),
							new LineTemplate(32, 30, 32, 60),
							new LineTemplate(32, 60, 25, 60),
							new LineTemplate(25, 60, 25, 25),
							new ArcTemplate(25, 20, 10, 10, 180, 90),
							new LineTemplate(30, 20, 50, 20)
						},
						FillMode.Winding, "Actor");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(10, 0, 90, 0),
							new ArcTemplate(80, 0, 20, 20, -90, 90),
							new LineTemplate(100, 10, 100, 90),
							new ArcTemplate(80, 80, 20, 20, 0, 90),
							new LineTemplate(90, 100, 10, 100),
							new ArcTemplate(0, 80, 20, 20, 90, 90),
							new LineTemplate(0, 90, 0, 10),
							new ArcTemplate(0, 0, 20, 20, 180, 90)
						},
						FillMode.Winding, "RoundRect");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(10, 0, 90, 0),
							new ArcTemplate(80, 0, 20, 100, -90, 180),
							new LineTemplate(90, 100, 10, 100),
							new ArcTemplate(0, 0, 20, 100, 90, 180)
						},
						new ElementTemplate[]
						{
							new ArcTemplate(0, 0, 20, 100, -90, 180)
						},
						new ElementTemplate[]
						{
							new LineTemplate(10, 0, 90, 0),
							new ArcTemplate(80, 0, 20, 100, -90, 180),
							new LineTemplate(90, 100, 10, 100),
							new BezierTemplate(10, 100, 35, 66, 35, 33, 10, 0)
						},
						FillMode.Winding, "DirectAccessStorage");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 100, 0),
							new LineTemplate(100, 0, 50, 100),
							new LineTemplate(50, 100, 0, 0)
						},
						FillMode.Winding, "Merge");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 50, 0),
							new LineTemplate(50, 0, 100, 50),
							new LineTemplate(100, 50, 50, 100),
							new LineTemplate(50, 100, 0, 100),
							new LineTemplate(0, 100, 0, 0)
						},
						FillMode.Winding, "OffpageConnection");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new ArcTemplate(0, 0, 100, 100, 0, 360)
						},
						new ElementTemplate[]
						{
							new LineTemplate(50, 0, 50, 100),
							new LineTemplate(0, 50, 100, 50)
						},
						null, FillMode.Winding, "Or");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new ArcTemplate(-100, 0, 200, 100, -90, 180),
							new BezierTemplate(0, 100, 55, 75, 55, 25, 0, 0)
						},
						FillMode.Winding, "Gate");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new LineTemplate(0, 0, 75, 0),
							new LineTemplate(75, 0, 100, 50),
							new LineTemplate(100, 50, 75, 100),
							new LineTemplate(75, 100, 0, 100),
							new LineTemplate(0, 100, 25, 50),
							new LineTemplate(25, 50, 0, 0)
						},
						FillMode.Winding, "IsoProcess");

					new ShapeTemplate(
						new ElementTemplate[]
						{
							new ArcTemplate(0, 0, 100, 100, 0, 360)
						},
						new ElementTemplate[]
						{
							new LineTemplate(15, 15, 85, 85),
							new LineTemplate(85, 15, 15, 85)
						},
						null, FillMode.Winding, "Junction");
				}

				Cylinder = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100, 10, 100, 90),
					new ArcTemplate(0, 80, 100, 20, 0, 180),
					new LineTemplate(0, 90, 0, 10),
					new ArcTemplate(0, 0, 100, 20, 180, 180),
					new ArcTemplate(0, 0, 100, 20, 0, 180),
					new ArcTemplate(0, 0, 100, 20, 180, 180) },
					FillMode.Winding);


				// Flowcharting
				Start = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  30,   0,  70),
					new LineTemplate(  0,  70,  20, 100),
					new LineTemplate( 20, 100,  30, 100),
					new LineTemplate( 30, 100,  30,   0),
					new LineTemplate( 30,   0,  30, 100),
					new LineTemplate( 30, 100,  70, 100),
					new LineTemplate( 70, 100,  70,   0),
					new LineTemplate( 70,   0,  70, 100),
					new LineTemplate( 70, 100,  80, 100),
					new LineTemplate( 80, 100, 100,  70),
					new LineTemplate(100,  70, 100,  30),
					new LineTemplate(100,  30,  80,   0),
					new LineTemplate( 80,   0,  20,   0),
					new LineTemplate( 20,   0,   0,  30) },
					FillMode.Winding);

				Procedure = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,   0,   0, 100),
					new LineTemplate(  0, 100,  15, 100),
					new LineTemplate( 15, 100,  15,   0),
					new LineTemplate( 15,   0,  15, 100),
					new LineTemplate( 15, 100,  85, 100),
					new LineTemplate( 85, 100,  85,   0),
					new LineTemplate( 85,   0,  85, 100),
					new LineTemplate( 85, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0),
					new LineTemplate(100,   0,   0,   0) },
					FillMode.Winding);

				CreateRequest = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100,   0,   0,   0),
					new LineTemplate(  0,   0,   0,  15),
					new LineTemplate(  0,  15, 100,  15),
					new LineTemplate(100,  15,   0,  15),
					new LineTemplate(  0,  15,   0,  85),
					new LineTemplate(  0,  85, 100,  85),
					new LineTemplate(100,  85,   0,  85),
					new LineTemplate(  0,  85,   0, 100),
					new LineTemplate(  0, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0) },
					FillMode.Winding);

				Alternative = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100,  50,   0),
					new LineTemplate( 50,   0, 100, 100),
					new LineTemplate(100, 100,   0, 100) },
					FillMode.Winding);

				Decision = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  50,  25,   0),
					new LineTemplate( 25,   0,  75,   0),
					new LineTemplate( 75,   0, 100,  50),
					new LineTemplate(100,  50,  75, 100),
					new LineTemplate( 75, 100,  25, 100),
					new LineTemplate( 25, 100,   0,  50) },
					FillMode.Winding);

				MessageFromUser = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,   0, 100,   0),
					new LineTemplate(100,   0,  65,  50),
					new LineTemplate( 65,  50, 100, 100),
					new LineTemplate(100, 100,   0, 100),
					new LineTemplate(  0, 100,   0,   0) },
					FillMode.Winding);

				PrimitiveFromCall = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100,  20, 100),
					new LineTemplate( 20, 100,  20,   0),
					new LineTemplate( 20,   0,  20, 100),
					new LineTemplate( 20, 100, 100, 100),
					new LineTemplate(100, 100,  65,  50),
					new LineTemplate( 65,  50, 100,   0),
					new LineTemplate(100,   0,   0,   0),
					new LineTemplate(  0,   0,   0, 100) },
					FillMode.Winding);

				MessageToUser = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100,   0,  35,   0),
					new LineTemplate( 35,   0,   0,  50),
					new LineTemplate(  0,  50,  35, 100),
					new LineTemplate( 35, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0) },
					FillMode.Winding);

				PrimitiveToCall = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100,   0,  80,   0),
					new LineTemplate( 80,   0,  80, 100),
					new LineTemplate( 80, 100,  80,   0),
					new LineTemplate( 80,   0,  35,   0),
					new LineTemplate( 35,   0,   0,  50),
					new LineTemplate(  0,  50,  35, 100),
					new LineTemplate( 35, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0) },
					FillMode.Winding);

				Save = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100,  35,   0),
					new LineTemplate( 35,   0, 100,   0),
					new LineTemplate(100,   0,  65, 100),
					new LineTemplate( 65, 100,   0, 100) },
					FillMode.Winding);

				OffpageReference = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  50,   0,   0),
					new LineTemplate(  0,   0, 100,   0),
					new LineTemplate(100,   0, 100,  50),
					new LineTemplate(100,  50,  50, 100),
					new LineTemplate( 50, 100,   0,  50) },
					FillMode.Winding);

				Document = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  90,   0,   0),
					new LineTemplate(  0,   0, 100,   0),
					new LineTemplate(100,   0, 100,  90),
					new BezierTemplate(100,  90,  83,  80,  67,  80,  50,  90),
					new BezierTemplate( 50,  90,  33, 100,  17, 100,   0,  90) },
					FillMode.Winding);

				DocumentReversed = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  90,   0,   0),
					new LineTemplate(  0,   0, 100,   0),
					new LineTemplate(100,   0, 100,  90),
					new BezierTemplate(100,  90,  83, 100,  66, 100,  50,  90),
					new BezierTemplate( 50,  90,  33,  80,  16,  80,   0,  90) },
					FillMode.Winding);

				MultiDocument = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  92,   0,  20),
					new LineTemplate(  0,  20,  10,  20),
					new LineTemplate( 10,  20,  80,  20),
					new LineTemplate( 80,  20,  80,  79),
					new BezierTemplate( 80,  79,  80,  79,  86,  79,  90,  82),
					new LineTemplate( 90,  82,  90,  10),
					new LineTemplate( 90,  10,  20,  10),
					new LineTemplate( 20,  10,  90,  10),
					new LineTemplate( 90,  10,  90,  69),
					new BezierTemplate( 90,  69,  90,  69,  96,  69, 100,  72),
					new LineTemplate(100,  72, 100,   0),
					new LineTemplate(100,   0,  20,   0),
					new LineTemplate( 20,   0,  20,  10),
					new LineTemplate( 20,  10,  10,  10),
					new LineTemplate( 10,  10,  10,  20),
					new LineTemplate( 10,  20,  80,  20),
					new LineTemplate( 80,  20,  80,  92),
					new BezierTemplate( 80,  92,  66,  84,  53,  84,  39,  92),
					new BezierTemplate( 39,  92,  26, 100,  13, 100,   0,  92) },
					FillMode.Winding);

				DividedProcess = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0),
					new LineTemplate(100,   0,   0,   0),
					new LineTemplate(  0,   0,   0,  25),
					new LineTemplate(  0,  25, 100,  25),
					new LineTemplate(100,  25,   0,  25),
					new LineTemplate(  0,  25,   0, 100) },
					FillMode.Winding);

				DividedEvent = new ShapeTemplate(new ElementTemplate[] {
					new BezierTemplate(  0,  90,   0,  95,   5, 100,  10, 100),
					new LineTemplate( 10, 100,  90, 100),
					new BezierTemplate( 90, 100,  95, 100, 100,  95, 100,  90),
					new LineTemplate(100,  90, 100,  10),
					new BezierTemplate(100,  10, 100,   5,  95,   0,  90,   0),
					new LineTemplate( 90,   0,  10,   0),
					new BezierTemplate( 10,   0,   5,   0,   0,   5,   0,  10),
					new LineTemplate(  0,  10,   0,  25),
					new LineTemplate(  0,  25, 100,  25),
					new LineTemplate(100,  25,   0,  25),
					new LineTemplate(  0,  25,   0,  90) },
					FillMode.Winding);

				ManualOperation = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,   0, 100,   0),
					new LineTemplate(100,   0,  75, 100),
					new LineTemplate( 75, 100,  25, 100),
					new LineTemplate( 25, 100,   0,   0) },
					FillMode.Winding);

				Display = new ShapeTemplate(new ElementTemplate[] {
					new BezierTemplate(  0,  50,   2,  67,  11,  90,  40, 100),
					new LineTemplate( 40, 100,  80, 100),
					new BezierTemplate( 80, 100,  90,  90, 100,  67, 100,  50),
					new BezierTemplate(100,  50, 100,  33,  90,  10,  80,   0),
					new LineTemplate( 80,   0,  40,   0),
					new BezierTemplate( 40,   0,  11,  10,   2,  33,   0,  50),
					},
					FillMode.Winding);

				MultiDocProc = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100,  80, 100),
					new LineTemplate( 80, 100,  80,  20),
					new LineTemplate( 80,  20,  10,  20),
					new LineTemplate( 10,  20,  80,  20),
					new LineTemplate( 80,  20,  80,  90),
					new LineTemplate( 80,  90,  90,  90),
					new LineTemplate( 90,  90,  90,  10),
					new LineTemplate( 90,  10,  20,  10),
					new LineTemplate( 20,  10,  90,  10),
					new LineTemplate( 90,  10,  90,  80),
					new LineTemplate( 90,  80, 100,  80),
					new LineTemplate(100,  80, 100,   0),
					new LineTemplate(100,   0,  20,   0),
					new LineTemplate( 20,   0,  20,  10),
					new LineTemplate( 20,  10,  10,  10),
					new LineTemplate( 10,  10,  10,  20),
					new LineTemplate( 10,  20,   0,  20),
					new LineTemplate(  0,  20,   0, 100) },
					FillMode.Winding);

				Tape = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate( 50, 100,  85, 100),
					new LineTemplate( 85, 100,  85,  90),
					new LineTemplate( 85,  90,  70,  90),
					new BezierTemplate( 70,  90,  85,  90, 100,  75, 100,  50),
					new BezierTemplate(100,  50, 100,  25,  75,   0,  50,   0),
					new BezierTemplate( 50,   0,  25,   0,   0,  25,   0,  50),
					new BezierTemplate(  0,  50,   0,  75,  25, 100,  50, 100) },
					FillMode.Winding);

				ManualInput = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0),
					new LineTemplate(100,   0,   0,  25),
					new LineTemplate(  0,  25,   0, 100),
					},
					FillMode.Winding);

				InternalStorage = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100,  20, 100),
					new LineTemplate( 20, 100,  20,   0),
					new LineTemplate( 20,   0,  20, 100),
					new LineTemplate( 20, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0),
					new LineTemplate(100,   0,   0,   0),
					new LineTemplate(  0,   0,   0,  20),
					new LineTemplate(  0,  20, 100,  20),
					new LineTemplate(100,  20,   0,  20),
					new LineTemplate(  0,  20,   0, 100) },
					FillMode.Winding);

				Card = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0),
					new LineTemplate(100,   0,  25,   0),
					new LineTemplate( 25,   0,   0,  25),
					new LineTemplate(  0,  25,   0, 100) },
					FillMode.Winding);

				DirectData = new ShapeTemplate(new ElementTemplate[] {
					new BezierTemplate( 15, 100,   0, 100,   0,  66,   0,  50),
					new BezierTemplate(  0,  50,   0,  33,   0,   0,  15,   0),
					new LineTemplate( 15,   0,  85,   0),
					new BezierTemplate( 85,   0,  70,   0,  70,  33,  70,  50),
					new BezierTemplate( 70,  50,  70,  66,  70, 100,  85, 100),
					new BezierTemplate( 85, 100, 100, 100, 100,  66, 100,  50),
					new BezierTemplate(100,  50, 100,  33, 100,   0,  85,   0),
					new BezierTemplate( 85,   0,  70,   0,  70,  33,  70,  50),
					new BezierTemplate( 70,  50,  70,  66,  70, 100,  85, 100),
					new LineTemplate( 85, 100,  15, 100) },
					FillMode.Winding);

				PaperTape = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  90,   0,  10),
					new BezierTemplate(  0,  10,  16,  20,  33,  20,  50,  10),
					new BezierTemplate( 50,  10,  66,   0,  83,   0, 100,  10),
					new LineTemplate(100,  10, 100,  90),
					new BezierTemplate(100,  90,  83,  80,  66,  80,  50,  90),
					new BezierTemplate( 50,  90,  33, 100,  16, 100,   0,  90) },
					FillMode.Winding);

				PaperTapeReversed = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  90,   0,  10),
					new BezierTemplate(  0,  10,  16,   0,  33,   0,  50,  10),
					new BezierTemplate( 50,  10,  66,  20,  83,  20, 100,  10),
					new LineTemplate(100,  10, 100,  90),
					new BezierTemplate(100,  90,  83, 100,  66, 100,  50,  90),
					new BezierTemplate( 50,  90,  33,  80,  16,  80,   0,  90) },
					FillMode.Winding);

				LoopLimit = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100,   0,  25),
					new LineTemplate(  0,  25,  25,   0),
					new LineTemplate( 25,   0,  75,   0),
					new LineTemplate( 75,   0, 100,  25),
					new LineTemplate(100,  25, 100, 100),
					new LineTemplate(100, 100,   0, 100) },
					FillMode.Winding);


				// Arrows
				Arrow1Left = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  50,  35,   0),
					new LineTemplate( 35,   0, 100,   0),
					new LineTemplate(100,   0,  65,  50),
					new LineTemplate( 65,  50, 100, 100),
					new LineTemplate(100, 100,  35, 100),
					new LineTemplate( 35, 100,   0,  50) },
					FillMode.Winding);

				Arrow1Top = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate( 50,   0, 100,  35),
					new LineTemplate(100,  35, 100, 100),
					new LineTemplate(100, 100,  50,  65),
					new LineTemplate( 50,  65,   0, 100),
					new LineTemplate(  0, 100,   0,  35),
					new LineTemplate(  0,  35,  50,   0) },
					FillMode.Winding);

				Arrow1Right = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100,  50,  65, 100),
					new LineTemplate( 65, 100,   0, 100),
					new LineTemplate(  0, 100,  35,  50),
					new LineTemplate( 35,  50,   0,   0),
					new LineTemplate(  0,   0,  65,   0),
					new LineTemplate( 65,   0, 100,  50) },
					FillMode.Winding);

				Arrow1Bottom = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate( 50, 100,   0,  65),
					new LineTemplate(  0,  65,   0,   0),
					new LineTemplate(  0,   0,  50,  35),
					new LineTemplate( 50,  35, 100,   0),
					new LineTemplate(100,   0, 100,  65),
					new LineTemplate(100,  65,  50, 100) },
					FillMode.Winding);

				Arrow2Left = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate( 80,  60,  35,  60),
					new LineTemplate( 35,  60,  35, 100),
					new LineTemplate( 35, 100,   0,  50),
					new LineTemplate(  0,  50,  35,   0),
					new LineTemplate( 35,   0,  35,  40),
					new LineTemplate( 35,  40,  80,  40),
					new LineTemplate( 80,  40,  80,   0),
					new LineTemplate( 80,   0, 100,   0),
					new LineTemplate(100,   0, 100, 100),
					new LineTemplate(100, 100,  80, 100),
					new LineTemplate( 80, 100,  80,  60) },
					FillMode.Winding);

				Arrow2Top = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate( 40,  80,  40,  35),
					new LineTemplate( 40,  35,   0,  35),
					new LineTemplate(  0,  35,  50,   0),
					new LineTemplate( 50,   0, 100,  35),
					new LineTemplate(100,  35,  60,  35),
					new LineTemplate( 60,  35,  60,  80),
					new LineTemplate( 60,  80, 100,  80),
					new LineTemplate(100,  80, 100, 100),
					new LineTemplate(100, 100,   0, 100),
					new LineTemplate(  0, 100,   0,  80),
					new LineTemplate(  0,  80,  40,  80),
					},
					FillMode.Winding);

				Arrow2Right = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate( 20,  40,  65,  40),
					new LineTemplate( 65,  40,  65,   0),
					new LineTemplate( 65,   0, 100,  50),
					new LineTemplate(100,  50,  65, 100),
					new LineTemplate( 65, 100,  65,  60),
					new LineTemplate( 65,  60,  20,  60),
					new LineTemplate( 20,  60,  20, 100),
					new LineTemplate( 20, 100,   0, 100),
					new LineTemplate(  0, 100,   0,   0),
					new LineTemplate(  0,   0,  20,   0),
					new LineTemplate( 20,   0,  20,  40) },
					FillMode.Winding);

				Arrow2Bottom = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate( 60,  20,  60,  65),
					new LineTemplate( 60,  65, 100,  65),
					new LineTemplate(100,  65,  50, 100),
					new LineTemplate( 50, 100,   0,  65),
					new LineTemplate(  0,  65,  40,  65),
					new LineTemplate( 40,  65,  40,  20),
					new LineTemplate( 40,  20,   0,  20),
					new LineTemplate(  0,  20,   0,   0),
					new LineTemplate(  0,   0, 100,   0),
					new LineTemplate(100,   0, 100,  20),
					new LineTemplate(100,  20,  60,  20) },
					FillMode.Winding);

				Arrow3Left = new ShapeTemplate(new ElementTemplate[] {
					new BezierTemplate(100,   0,  80,  30,  80,  70, 100, 100),
					new LineTemplate(100, 100,   0,  50),
					new LineTemplate(  0,  50, 100,   0) },
					FillMode.Winding);

				Arrow3Top = new ShapeTemplate(new ElementTemplate[] {
					new BezierTemplate(100, 100,  70,  80,  30,  80,   0, 100),
					new LineTemplate(  0, 100,  50,   0),
					new LineTemplate( 50,   0, 100, 100) },
					FillMode.Winding);

				Arrow3Right = new ShapeTemplate(new ElementTemplate[] {
					new BezierTemplate(  0, 100,  20,  70,  20,  30,   0,   0),
					new LineTemplate(  0,   0, 100,  50),
					new LineTemplate(100,  50,   0, 100) },
					FillMode.Winding);

				Arrow3Bottom = new ShapeTemplate(new ElementTemplate[] {
					new BezierTemplate(  0,   0,  30,  20,  70,  20, 100,   0),
					new LineTemplate(100,   0,  50, 100),
					new LineTemplate( 50, 100,   0,   0) },
					FillMode.Winding);


				// 3D-Like
				ConeUp = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  80,  50,   0),
					new LineTemplate( 50,   0, 100,  80),
					new BezierTemplate(100,  80, 100,  90,  66, 100,  50, 100),
					new BezierTemplate( 50, 100,  33, 100,   0,  90,   0,  80) },
					FillMode.Winding);

				ConeDown = new ShapeTemplate(new ElementTemplate[] {
					new BezierTemplate(  0,  80,   4,  70,  33,  60,  50,  60),
					new BezierTemplate( 50,  60,  66,  60,  96,  70, 100,  80),
					new BezierTemplate(100,  80,  96,  70,  66,  60,  50,  60),
					new BezierTemplate( 50,  60,  33,  60,   4,  70,   0,  80),
					new LineTemplate(  0,  80,  50,   0),
					new LineTemplate( 50,   0, 100,  80),
					new BezierTemplate(100,  80, 100,  90,  66, 100,  50, 100),
					new BezierTemplate( 50, 100,  33, 100,   0,  90,   0,  80) },
					FillMode.Winding);

				CubeUp = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  80,  50, 100),
					new LineTemplate( 50, 100,  50,  40),
					new LineTemplate( 50,  40,  50, 100),
					new LineTemplate( 50, 100, 100,  80),
					new LineTemplate(100,  80, 100,  20),
					new LineTemplate(100,  20,  50,  40),
					new LineTemplate( 50,  40,   0,  20),
					new LineTemplate(  0,  20,  50,  40),
					new LineTemplate( 50,  40, 100,  20),
					new LineTemplate(100,  20,  50,   0),
					new LineTemplate( 50,   0,   0,  20),
					new LineTemplate(  0,  20,   0,  80) },
					FillMode.Winding);

				CubeDown = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100,  20,  50,   0),
					new LineTemplate( 50,   0,  50,  60),
					new LineTemplate( 50,  60,  50,   0),
					new LineTemplate( 50,   0,   0,  20),
					new LineTemplate(  0,  20,   0,  80),
					new LineTemplate(  0,  80,  50,  60),
					new LineTemplate( 50,  60, 100,  80),
					new LineTemplate(100,  80,  50,  60),
					new LineTemplate( 50,  60,   0,  80),
					new LineTemplate(  0,  80,  50, 100),
					new LineTemplate( 50, 100, 100,  80),
					new LineTemplate(100,  80, 100,  20),
					},
					FillMode.Winding);

				PyramidUp = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  80,  50,   0),
					new LineTemplate( 50,   0, 100,  80),
					new LineTemplate(100,  80,  50, 100),
					new LineTemplate( 50, 100,  50,   0),
					new LineTemplate( 50,   0,  50, 100),
					new LineTemplate( 50, 100,   0,  80) },
					FillMode.Winding);

				PyramidDown = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,  80,  50,   0),
					new LineTemplate( 50,   0, 100,  80),
					new LineTemplate(100,  80,  50, 100),
					new LineTemplate( 50, 100, 100,  80),
					new LineTemplate(100,  80,  50,  60),
					new LineTemplate( 50,  60,   0,  80),
					new LineTemplate(  0,  80,  50,  60),
					new LineTemplate( 50,  60,  50,   0),
					new LineTemplate( 50,   0,  50,  60),
					new LineTemplate( 50,  60, 100,  80),
					new LineTemplate(100,  80,  50, 100),
					new LineTemplate( 50, 100,   0,  80) },
					FillMode.Winding);


				// Miscellaneous
				DeltaLeft = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100,   0,  40,   0),
					new BezierTemplate( 40,   0,  20,   0,   0,  25,   0,  50),
					new BezierTemplate(  0,  50,   0,  75,  20, 100,  40, 100),
					new LineTemplate( 40, 100, 100, 100),
					new LineTemplate(100, 100, 100,   0) },
					FillMode.Winding);

				DeltaTop = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100, 100, 100,  40),
					new BezierTemplate(100,  40, 100,  20,  75,   0,  50,   0),
					new BezierTemplate( 50,   0,  25,   0,   0,  20,   0,  40),
					new LineTemplate(  0,  40,   0, 100),
					new LineTemplate(  0, 100, 100, 100) },
					FillMode.Winding);

				DeltaRight = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100,  60, 100),
					new BezierTemplate( 60, 100,  80, 100, 100,  75, 100,  50),
					new BezierTemplate(100,  50, 100,  25,  80,   0,  60,   0),
					new LineTemplate( 60,   0,   0,   0),
					new LineTemplate(  0,   0,   0, 100) },
					FillMode.Winding);

				DeltaBottom = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,   0,   0,  60),
					new BezierTemplate(  0,  60,   0,  80,  25, 100,  50, 100),
					new BezierTemplate( 50, 100,  75, 100, 100,  80, 100,  60),
					new LineTemplate(100,  60, 100,   0),
					new LineTemplate(100,   0,   0,   0) },
					FillMode.Winding);

				DoubleDeltaLeft = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100,   0,  40,   0),
					new BezierTemplate( 40,   0,  20,   0,   0,  25,   0,  50),
					new BezierTemplate(  0,  50,   0,  75,  20, 100,  40, 100),
					new LineTemplate( 40, 100, 100, 100),
					new BezierTemplate(100, 100,  60,  75,  60,  25, 100,   0) },
					FillMode.Winding);

				DoubleDeltaTop = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100, 100, 100,  40),
					new BezierTemplate(100,  40, 100,  20,  75,   0,  50,   0),
					new BezierTemplate( 50,   0,  25,   0,   0,  20,   0,  40),
					new LineTemplate(  0,  40,   0, 100),
					new BezierTemplate(  0, 100,  25,  60,  75,  60, 100, 100) },
					FillMode.Winding);

				DoubleDeltaRight = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0, 100,  60, 100),
					new BezierTemplate( 60, 100,  80, 100, 100,  75, 100,  50),
					new BezierTemplate(100,  50, 100,  25,  80,   0,  60,   0),
					new LineTemplate( 60,   0,   0,   0),
					new BezierTemplate(  0,   0,  40,  25,  40,  75,   0, 100) },
					FillMode.Winding);

				DoubleDeltaBottom = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,   0,   0,  60),
					new BezierTemplate(  0,  60,   0,  80,  25, 100,  50, 100),
					new BezierTemplate( 50, 100,  75, 100, 100,  80, 100,  60),
					new LineTemplate(100,  60, 100,   0),
					new BezierTemplate(100,   0,  75,  40,  25,  40,   0,   0) },
					FillMode.Winding);

				DeflateDeltaVertical = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(  0,   0,   0, 100),
					new BezierTemplate(  0, 100,  25,  60,  75,  60, 100, 100),
					new LineTemplate(100, 100, 100,   0),
					new BezierTemplate(100,   0,  75,  40,  25,  40,   0,   0) },
					FillMode.Winding);

				DeflateDeltaHorizontal = new ShapeTemplate(new ElementTemplate[] {
					new LineTemplate(100,   0,   0,   0),
					new BezierTemplate(  0,   0,  40,  25,  40,  75,   0, 100),
					new LineTemplate(  0, 100, 100, 100),
					new BezierTemplate(100, 100,  60,  75,  60,  25, 100,   0) },
					FillMode.Winding);
			}
		}

		public static ShapeTemplate GetPredefined(ulong index)
		{
			switch(index)
			{
				case 0:
					return Cylinder;
				case 1:
					return Start;
				case 2:
					return Procedure;
				case 3:
					return CreateRequest;
				case 4:
					return Alternative;
				case 5:
					return Decision;
				case 6:
					return MessageFromUser;
				case 7:
					return PrimitiveFromCall;
				case 8:
					return MessageToUser;
				case 9:
					return PrimitiveToCall;
				case 10:
					return Save;
				case 11:
					return OffpageReference;
				case 12:
					return Document;
				case 13:
					return DocumentReversed;
				case 14:
					return MultiDocument;
				case 15:
					return DividedProcess;
				case 16:
					return DividedEvent;
				case 17:
					return ManualOperation;
				case 18:
					return Display;
				case 19:
					return MultiDocProc;
				case 20:
					return Tape;
				case 21:
					return ManualInput;
				case 22:
					return InternalStorage;
				case 23:
					return Card;
				case 24:
					return DirectData;
				case 25:
					return PaperTape;
				case 26:
					return PaperTapeReversed;
				case 27:
					return LoopLimit;
				case 28:
					return Arrow1Left;
				case 29:
					return Arrow1Top;
				case 30:
					return Arrow1Right;
				case 31:
					return Arrow1Bottom;
				case 32:
					return Arrow2Left;
				case 33:
					return Arrow2Top;
				case 34:
					return Arrow2Right;
				case 35:
					return Arrow2Bottom;
				case 36:
					return Arrow3Left;
				case 37:
					return Arrow3Top;
				case 38:
					return Arrow3Right;
				case 39:
					return Arrow3Bottom;
				case 40:
					return ConeUp;
				case 41:
					return ConeDown;
				case 42:
					return CubeUp;
				case 43:
					return CubeDown;
				case 44:
					return PyramidUp;
				case 45:
					return PyramidDown;
				case 46:
					return DeltaLeft;
				case 47:
					return DeltaTop;
				case 48:
					return DeltaRight;
				case 49:
					return DeltaBottom;
				case 50:
					return DoubleDeltaLeft;
				case 51:
					return DoubleDeltaTop;
				case 52:
					return DoubleDeltaRight;
				case 53:
					return DoubleDeltaBottom;
				case 54:
					return DeflateDeltaVertical;
				case 55:
					return DeflateDeltaHorizontal;
				default:
					return null;
			}
		}

		public static ShapeTemplate Cylinder;

		public static ShapeTemplate Start;
		public static ShapeTemplate Procedure;
		public static ShapeTemplate CreateRequest;
		public static ShapeTemplate Alternative;
		public static ShapeTemplate Decision;
		public static ShapeTemplate MessageFromUser;
		public static ShapeTemplate PrimitiveFromCall;
		public static ShapeTemplate MessageToUser;
		public static ShapeTemplate PrimitiveToCall;
		public static ShapeTemplate Save;
		public static ShapeTemplate OffpageReference;
		public static ShapeTemplate Document;
		public static ShapeTemplate DocumentReversed;
		public static ShapeTemplate MultiDocument;
		public static ShapeTemplate DividedProcess;
		public static ShapeTemplate DividedEvent;
		public static ShapeTemplate ManualOperation;
		public static ShapeTemplate Display;
		public static ShapeTemplate MultiDocProc;
		public static ShapeTemplate Tape;
		public static ShapeTemplate ManualInput;
		public static ShapeTemplate InternalStorage;
		public static ShapeTemplate Card;
		public static ShapeTemplate DirectData;
		public static ShapeTemplate PaperTape;
		public static ShapeTemplate PaperTapeReversed;
		public static ShapeTemplate LoopLimit;

		public static ShapeTemplate Arrow1Left;
		public static ShapeTemplate Arrow1Top;
		public static ShapeTemplate Arrow1Right;
		public static ShapeTemplate Arrow1Bottom;
		public static ShapeTemplate Arrow2Left;
		public static ShapeTemplate Arrow2Top;
		public static ShapeTemplate Arrow2Right;
		public static ShapeTemplate Arrow2Bottom;
		public static ShapeTemplate Arrow3Left;
		public static ShapeTemplate Arrow3Top;
		public static ShapeTemplate Arrow3Right;
		public static ShapeTemplate Arrow3Bottom;

		public static ShapeTemplate ConeUp;
		public static ShapeTemplate ConeDown;
		public static ShapeTemplate CubeUp;
		public static ShapeTemplate CubeDown;
		public static ShapeTemplate PyramidUp;
		public static ShapeTemplate PyramidDown;

		public static ShapeTemplate DeltaLeft;
		public static ShapeTemplate DeltaTop;
		public static ShapeTemplate DeltaRight;
		public static ShapeTemplate DeltaBottom;
		public static ShapeTemplate DoubleDeltaLeft;
		public static ShapeTemplate DoubleDeltaTop;
		public static ShapeTemplate DoubleDeltaRight;
		public static ShapeTemplate DoubleDeltaBottom;
		public static ShapeTemplate DeflateDeltaVertical;
		public static ShapeTemplate DeflateDeltaHorizontal;



		public virtual int getClassId()
		{
			return 26;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			int num = 0;

			// save outline
			num = elements.GetLength(0);
			writer.Write(num);
			for (int i = 0; i < num; ++i)
				ctx.saveObject(elements[i]);

			// new in save format 6

			// save decorations
			if (decorations != null)
			{
				num = decorations.GetLength(0);
				writer.Write(num);
				for (int i = 0; i < num; ++i)
					ctx.saveObject(decorations[i]);
			}
			else
			{
				writer.Write((int)0);
			}

			// save text region
			if (textArea != null)
			{
				num = textArea.GetLength(0);
				writer.Write(num);
				for (int i = 0; i < num; ++i)
					ctx.saveObject(textArea[i]);
			}
			else
			{
				writer.Write((int)0);
			}
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int num = reader.ReadInt32();
			elements = new ElementTemplate[num];
			for (int i = 0; i < num; ++i)
				elements[i] = (ElementTemplate)ctx.loadObject();

			decorations = null;
			textArea = null;
			if (ctx.FileVersion > 5)
			{
				// new in save format 6

				// load decorations
				num = reader.ReadInt32();
				if (num > 0)
				{
					decorations = new ElementTemplate[num];
					for (int i = 0; i < num; ++i)
						decorations[i] = (ElementTemplate)ctx.loadObject();
				}

				// load text region
				num = reader.ReadInt32();
				if (num > 0)
				{
					textArea = new ElementTemplate[num];
					for (int i = 0; i < num; ++i)
						textArea[i] = (ElementTemplate)ctx.loadObject();
				}
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
		}

		public override string ToString()
		{
			return Id;
		}

		private bool textOutside;
	}
}