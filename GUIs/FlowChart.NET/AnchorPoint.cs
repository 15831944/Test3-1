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

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	public sealed class AnchorPoint : ICloneable, IPersists
	{
		public AnchorPoint(short x, short y)
		{
			this.x = x;
			this.y = y;
			this.allowIncoming = true;
			this.allowOutgoing = true;
			this.markStyle = MarkStyle.X;
			this.color = Color.Black;
			this.col = -1;
			this.tag = 0;
		}

		public AnchorPoint(short x, short y,
			bool allowIncoming, bool allowOutgoing)
		{
			this.x = x;
			this.y = y;
			this.allowIncoming = allowIncoming;
			this.allowOutgoing = allowOutgoing;
			this.markStyle = MarkStyle.X;
			this.color = Color.Black;
			this.col = -1;
			this.tag = 0;
		}

		public AnchorPoint(short x, short y,
			bool allowIncoming, bool allowOutgoing,
			MarkStyle markStyle)
		{
			this.x = x;
			this.y = y;
			this.allowIncoming = allowIncoming;
			this.allowOutgoing = allowOutgoing;
			this.markStyle = markStyle;
			this.color = Color.Black;
			this.col = -1;
			this.tag = 0;
		}

		public AnchorPoint(short x, short y,
			bool allowIncoming, bool allowOutgoing,
			MarkStyle markStyle, Color color)
		{
			this.x = x;
			this.y = y;
			this.allowIncoming = allowIncoming;
			this.allowOutgoing = allowOutgoing;
			this.markStyle = markStyle;
			this.color = color;
			this.col = -1;
			this.tag = 0;
		}

		public AnchorPoint(short x, short y,
			bool allowIncoming, bool allowOutgoing,
			Color color, int col)
		{
			this.x = x;
			this.y = y;
			this.allowIncoming = allowIncoming;
			this.allowOutgoing = allowOutgoing;
			this.markStyle = MarkStyle.X;
			this.color = Color.Black;
			this.col = col;
			this.tag = 0;
		}

		public AnchorPoint(short x, short y,
			bool allowIncoming, bool allowOutgoing,
			Color color, int col, object tag)
		{
			this.x = x;
			this.y = y;
			this.allowIncoming = allowIncoming;
			this.allowOutgoing = allowOutgoing;
			this.markStyle = MarkStyle.X;
			this.color = Color.Black;
			this.col = col;
			this.tag = tag;
		}

		public int getClassId()
		{
			return 36;
		}

		public void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(x);
			writer.Write(y);
			writer.Write(allowIncoming);
			writer.Write(allowOutgoing);
			writer.Write((int)markStyle);
			ctx.saveColor(color);

			// new in file format 6
			writer.Write(col);

			// new in file format 7
			bool tagSerializable = false;
			if (tag != null)
				tagSerializable = tag.GetType().IsSerializable;
			writer.Write(tagSerializable);

			if (tagSerializable)
				ctx.saveTag(tag);

			// new in file format 23
			writer.Write(toolTip);
		}

		public void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			x = reader.ReadInt16();
			y = reader.ReadInt16();
			allowIncoming = reader.ReadBoolean();
			allowOutgoing = reader.ReadBoolean();
			markStyle = (MarkStyle)reader.ReadInt32();
			color = ctx.loadColor();

			if (ctx.FileVersion > 5)
			{
				// new in file format 6
				col = reader.ReadInt32();

				if (ctx.FileVersion > 6)
				{
					// new in file format 7
					tag = null;
					bool tagAvailable = reader.ReadBoolean();
					if (tagAvailable)
						tag = ctx.loadTag();

					if (ctx.FileVersion > 22)
					{
						// new in file format 23
						toolTip = reader.ReadString();
					}
				}
			}
		}

		public void setReference(int refId, IPersists obj)
		{
		}

    public PointF AnchorToDoc(RectangleF nodeRect, float angle)
    {
      float cX = nodeRect.X + nodeRect.Width * x / 100;
      float cY = nodeRect.Y + nodeRect.Height * y / 100;

      PointF point = new PointF(cX, cY);

      if (angle != 0)
      {
        point = Utilities.rotatePointAt(point,
          Utilities.getCenter(nodeRect), angle);
        cX = point.X;
        cY = point.Y;
      }

      return point;
    }
    
    internal void draw(Graphics g, RectangleF nodeRect, float angle,
			MarkDrawArgs customDrawArgs, float markSize)
		{
			float cX = nodeRect.X + nodeRect.Width * x / 100;
			float cY = nodeRect.Y + nodeRect.Height * y / 100;
			float d = markSize / 2;

			if (angle != 0)
			{
				PointF point = new PointF(cX, cY);
				point = Utilities.rotatePointAt(point,
					Utilities.getCenter(nodeRect), angle);
				cX = point.X;
				cY = point.Y;
			}

			switch (markStyle)
			{
				case MarkStyle.Cross:
					{
						System.Drawing.Pen pen = new System.Drawing.Pen(color, 0);
						g.DrawLine(pen, cX - d, cY, cX + d, cY);
						g.DrawLine(pen, cX, cY - d, cX, cY + d);
						pen.Dispose();
						
					}
					break;
				case MarkStyle.X:
					{
						System.Drawing.Pen pen = new System.Drawing.Pen(color, 0);
						g.DrawLine(pen, cX - d, cY - d, cX + d, cY + d);
						g.DrawLine(pen, cX + d, cY - d, cX - d, cY + d);
						pen.Dispose();
						
					}
					break;
				case MarkStyle.Circle:
					{
						System.Drawing.Pen pen = new System.Drawing.Pen(color, 0);
						g.DrawEllipse(pen, cX - d, cY - d, 2*d, 2*d);
						pen.Dispose();
					}
					break;
				case MarkStyle.Rectangle:
					{
						System.Drawing.Pen pen = new System.Drawing.Pen(color, 0);
						g.DrawRectangle(pen, cX - d, cY - d, 2*d, 2*d);
						pen.Dispose();
					}
					break;
				case MarkStyle.Custom:
					{
						customDrawArgs.location = new PointF(cX, cY);
						customDrawArgs.Node.drawMark(customDrawArgs);
					}
					break;
			}
		}

		internal PointF getPos(RectangleF nodeRect)
		{
			return new PointF(
				nodeRect.X + nodeRect.Width * x / 100,
				nodeRect.Y + nodeRect.Height * y / 100);
		}

		public short X
		{
			get { return x; }
			set { x = value; }
		}

		public short Y
		{
			get { return y; }
			set { y = value; }
		}

		public int Column
		{
			get { return col; }
			set { col = value; }
		}

		public bool AllowIncoming
		{
			get { return allowIncoming; }
			set { allowIncoming = value; }
		}

		public bool AllowOutgoing
		{
			get { return allowOutgoing; }
			set { allowOutgoing = value; }
		}

		public MarkStyle MarkStyle
		{
			get { return markStyle; }
			set { markStyle = value; }
		}

		public Color Color
		{
			get { return color; }
			set { color = value; }
		}

		private short x, y;
		private bool allowIncoming;
		private bool allowOutgoing;
		private MarkStyle markStyle;
		private Color color;
		private int col;

		public object Clone()
		{
			return this.MemberwiseClone();
		}

		/// <summary>
		/// User-defined data
		/// </summary>
		public object Tag
		{
			get
			{
				return tag;
			}
			set
			{
				tag = value;
			}
		}

		private object tag;

		public string ToolTip
		{
			get { return toolTip; }
			set
			{
				if (toolTip != value)
				{
					if (value == null)
						toolTip = "";
					else
						toolTip = value;
				}
			}
		}

		private string toolTip = "";
	}
}