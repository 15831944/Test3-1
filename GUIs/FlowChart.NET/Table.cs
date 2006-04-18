// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;
using System.Windows.Forms;
using System.IO;
using System.Resources;
using MindFusion.FlowChartX.Commands;
using MindFusion.FlowChartX.Visitors;
using MindFusion.FlowChartX.Text;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Tables are nodes that contain multiple cells arranged in rows and columns.
	/// A table can be related to other nodes either by its rows or as an integral node.
	/// </summary>
	public class Table : Node
	{
		/// <summary>
		/// Cell instances represent table's cells. Cells can display text and images
		/// and allow setting their colors, text font and style, and image position.
		/// </summary>
		public class Cell : InplaceEditable, IPersists
		{
			public Cell(Table table)
			{
				this.table = table;

				text = "";
				hyperLink = "";
				textFormat = new StringFormat();
				textFormat.Alignment = StringAlignment.Near;
				textFormat.LineAlignment = StringAlignment.Center;
				textColor = Color.Empty;

				toolTip = "";
				picture = null;
				picturePos = ImageAlign.Fit;
				brush = null;

				txOptions = new Text.LayoutOptions();
				txLayout = new Text.Layout();

				columnSpan = 1;
				rowSpan = 1;
				tag = null;
			}

			public Cell Clone()
			{
				Cell cell = new Cell(table);

				cell.text = this.text;
				cell.hyperLink = this.hyperLink;
				cell.textFormat = this.textFormat;
				cell.textColor = this.textColor;

				cell.toolTip = this.toolTip;
				cell.picture = this.picture;
				cell.picturePos = this.picturePos;

				cell.brush = this.brush;
				if (cell.brush != null)
					cell.brush.AddRef();

				cell.columnSpan = this.columnSpan;
				cell.rowSpan = this.rowSpan;
				cell.tag = this.tag;

				cell.txOptions = new Text.LayoutOptions();
				cell.txLayout = new Text.Layout();
				cell.updateText();

				return cell;
			}

			internal void freeResources()
			{
				if (brush != null)
				{
					brush.Release();
					brush = null;
				}
			}

			internal Cell()
			{
				columnSpan = 1;
				rowSpan = 1;
			}

			internal Table table;

			public string Text
			{
				get
				{
					return text;
				}
				set
				{
					if (text != value)
					{
						if (value == null)
							text = "";
						else
							text = value;

						updateText();
						layoutText();

						table.fcParent.setDirty();
						table.fcParent.invalidate(table.getRepaintRect(false));
					}
				}
			}

			private string text;

			public string HyperLink
			{
				get 
				{
					return hyperLink;
				}
				set
				{
					if (hyperLink != value)
					{
						if (value == null)
							hyperLink = "";
						else
							hyperLink = value;

						table.fcParent.setDirty();
					}
				}
			}

			private string hyperLink;

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
					if (tag == null || !tag.Equals(value))
					{
						tag = value;
						table.Parent.setDirty();
					}
				}
			}

			private object tag;

			public StringFormat TextFormat
			{
				get
				{
					return textFormat;
				}
				set
				{
					if (!textFormat.Equals(value))
					{
						textFormat = new StringFormat(value);

						layoutText();

						table.fcParent.setDirty();
						table.fcParent.invalidate(table.getRepaintRect(false));
					}
				}
			}

			private StringFormat textFormat;

			public Color TextColor
			{
				get
				{
					return textColor;
				}
				set
				{
					textColor = value;
					table.fcParent.setDirty();
					table.fcParent.invalidate(table.getRepaintRect(false));
				}
			}

			private Color textColor;

			/// <summary>
			/// Updates the internal MindFusion.FlowChartX.Text.Text
			/// obejct when styled characteristics of
			/// a cell's text have changed.
			/// </summary>
			internal void updateText()
			{
				if (table.EnableStyledText)
				{
					Graphics graphics = table.fcParent.CreateGraphics();
					table.fcParent.setTransforms(graphics);

					// Create and setup styled text
					ftext = new Text.StyledText();
					ftext.Setup(text, graphics, table.Font);

					graphics.Dispose();
				}
				else
				{
					ftext = null;
				}
			}

			/// <summary>
			/// Performs the actual text layouting so that
			/// the internal MindFusion.FlowChartX.Text.Layout object
			/// is prepared for subsequent drawing operations.
			/// Returns whether text fits entirely whithin the area.
			/// </summary>
			internal bool layoutText()
			{
				// Check if styled text is switched on for this table
				if (!table.EnableStyledText || ftext == null)
					return false;

				// Ensure that the layout options are adequate
				txOptions.LineDistance = 0;
				txOptions.Alignment = textFormat.Alignment;
				txOptions.LineAlignment = textFormat.LineAlignment;

				RectangleF cellRect = table.getSpannedCellRect(this);
				RectangleF rect = docToLocal(cellRect, cellRect);
				RectangleF rcCell = Utilities.normalizeRect(rect);
				rcCell.Inflate(-table.Pen.Width / 2, -table.Pen.Width / 2);

				float pixel = Constants.getPixel(table.fcParent.MeasureUnit);
				rcCell.Width -= 2 * pixel;
				if (table.CellFrameStyle == CellFrameStyle.System3D)
					rcCell.Inflate(-pixel * 2, -pixel * 2);
				else if (table.pen.Width == 0)
					rcCell.Inflate(-pixel, -pixel);

				// Perform the layout
				return txLayout.LayoutInRectangle(ftext, rcCell, txOptions);
			}

			/// <summary>
			/// Gets the bounding rectangle of the specified cell,
			/// expressed in coordinates relative to the top-left
			/// point of the cell.
			/// </summary>
			private RectangleF docToLocal(RectangleF rect, RectangleF cellRect)
			{
				RectangleF result = rect;

				result.X -= cellRect.X;
				result.Y -= cellRect.Y;

				return result;
			}

			private Text.Text ftext;
			private Text.Layout txLayout;
			private Text.LayoutOptions txOptions;
			private static StringFormat nearFormat = new StringFormat();

			internal void draw(Graphics g, System.Drawing.Pen pen,
				System.Drawing.Brush textBrush, RectangleF rect, Font font,
				CellFrameStyle borders, int row, int column, RectangleF imgRect)
			{
				if (table.CellCustomDraw == CustomDraw.Full)
				{
					// call the custom draw function
					table.Parent.drawCell(g, table, column, row, rect);
				}
				else
				{
					// Paint cell with its own brush if specified
					if (brush != null && table.fcParent.RenderOptions.EnableInterior)
					{
						System.Drawing.Brush cellBrush =
							brush.CreateGDIBrush(rect);
						g.FillRectangle(cellBrush, rect);
						cellBrush.Dispose();
					}

					// Draw cell picture if available
					if (picture != null)
						Utilities.drawPicture(g, picture, imgRect, picturePos);

					// additional custom draw type
					if (table.CellCustomDraw == CustomDraw.Additional)
						table.Parent.drawCell(g, table, column, row, rect);

					// Paint the border
					switch (borders)
					{
						case CellFrameStyle.System3D:
							Rectangle rcDev = Utilities.docToDevice(g, rect);
							if (column == 0)
							{
								rcDev.X += 2;
								rcDev.Width -= 2;
							}
							ControlPaint.DrawBorder3D(g, rcDev, Border3DStyle.Sunken);
							break;

						case CellFrameStyle.Simple:
							g.DrawRectangle(pen, rect.X, rect.Y, rect.Width, rect.Height);
							break;
					}

					RectangleF textRect = rect;

					// DrawString crashes with a too small PageScale
					if (g.PageScale > 0.01 && text.Length > 0 &&
						table.fcParent.RenderOptions.EnableText)
					{
						System.Drawing.Brush b = textBrush;
						if (textColor.A != 0)
							b = new System.Drawing.SolidBrush(textColor);

						if (table.EnableStyledText)
						{
							txLayout.Draw(textRect.Left, textRect.Top,
								new RenderTextCallback(DrawText),
								new DrawTextHint(g, font, b, nearFormat,
								Utilities.getTextEllipsis(textFormat)));
						}
						else
						{
							textRect.Inflate(-pen.Width / 2, -pen.Width / 2);
							g.DrawString(text, font, b, textRect, textFormat);
						}

						if (textColor.A != 0)
							b.Dispose();
					}

					// additional2 custom draw type
					if (table.CellCustomDraw == CustomDraw.Additional2)
						table.Parent.drawCell(g, table, column, row, rect);
				}
			}

			/// <summary>
			/// Styled text rendering callback.
			/// </summary>
			private void DrawText(string text, RectangleF dest, DrawTextHint hint)
			{
				// Render formatted text
				hint.Graphics.DrawString(text, hint.Font, hint.Brush,
					dest, hint.Format);
			}

			public string PlainText
			{
				get
				{
					if (ftext == null)
						return text;

					return ftext.PlainText;
				}
			}

			public string ToolTip
			{
				get
				{
					return toolTip;
				}
				set
				{
					if (toolTip != value)
					{
						if (value == null)
							toolTip = "";
						else
							toolTip = value;

						table.fcParent.setDirty();
					}
				}
			}

			private string toolTip;

			public Image Picture
			{
				get { return picture; }
				set
				{
					picture = value;
					table.fcParent.setDirty();
					table.fcParent.invalidate(table.getRepaintRect(false));
				}
			}

			private Image picture;

			public ImageAlign PicturePos
			{
				get { return picturePos; }
				set
				{
					picturePos = value;
					table.fcParent.setDirty();
					table.fcParent.invalidate(table.getRepaintRect(false));
				}
			}

			private ImageAlign picturePos;

			/// <summary>
			/// The brush used to fill the cell's interior.
			/// </summary>
			public MindFusion.FlowChartX.Brush Brush
			{
				get
				{
					return brush;
				}
				set
				{
					if (brush != null)
						brush.Release();
					brush = value;
					if (brush != null)
						brush.AddRef();

					table.fcParent.setDirty();
					table.fcParent.invalidate(table.getRepaintRect(false));
				}
			}

			private MindFusion.FlowChartX.Brush brush;

			public virtual int getClassId()
			{
				return 20;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				writer.Write(text);
				ctx.saveStringFormat(textFormat);
				ctx.saveReference(this, table, 1);

				// new in save format 6
				writer.Write((int)picturePos);
				ctx.saveImage(picture);

				// new in save format 14
				ctx.saveColor(textColor);
				ctx.saveReference(this, brush, 2);

				// new in save format 18
				writer.Write(toolTip);

				// new in save format 19
				writer.Write(hyperLink);

				// new in save format 22
				writer.Write(columnSpan);
				writer.Write(rowSpan);

				bool tagSerializable = false;
				if (tag != null)
					tagSerializable = tag.GetType().IsSerializable;
				writer.Write(tagSerializable);

				if (tagSerializable)
					ctx.saveTag(tag);
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				toolTip = "";
				hyperLink = "";

				text = reader.ReadString();
				textFormat = ctx.loadStringFormat();
				ctx.loadReference(this);

				if (ctx.FileVersion > 5)
				{
					// new in save format 6
					picturePos = (ImageAlign)reader.ReadInt32();
					picture = ctx.loadImage();

					if (ctx.FileVersion > 13)
					{
						// new in save format 14
						textColor = ctx.loadColor();
						ctx.loadReference(this);	// brush

						if (ctx.FileVersion > 17)
						{
							// new in save format 18
							toolTip = reader.ReadString();

							if (ctx.FileVersion > 18)
							{
								// new in save format 19
								hyperLink = reader.ReadString();

								if (ctx.FileVersion > 21)
								{
									// new in save format 22
									columnSpan = reader.ReadInt32();
									rowSpan = reader.ReadInt32();

									tag = null;
									bool tagAvailable = reader.ReadBoolean();
									if (tagAvailable)
										tag = ctx.loadTag();
								}
							}
						}
					}
				}

				txOptions = new Text.LayoutOptions();
				txLayout = new Text.Layout();
			}

			public virtual void setReference(int refId, IPersists obj)
			{
				switch(refId)
				{
					case 1:
						table = (Table)obj;
						break;
					case 2:
						if (brush != null)
							brush.Release();
						brush = (Brush)obj;
						if (brush != null)
							brush.AddRef();
						break;
					default:
						throw new Exception("Unrecognized reference in Cell.setReference(...)");
				}
			}

			internal override string getTextToEdit()
			{
				return Text;
			}


			internal override void setEditedText(string newText)
			{
				Text = newText;
			}

			internal override RectangleF getEditRect()
			{
				return table.getSpannedCellRect(this);
			}


			public int ColumnSpan
			{
				get { return columnSpan; }
				set
				{
					if (value < 1)
						return;

					if (columnSpan == value)
						return;

					columnSpan = value;
					layoutText();
					table.fcParent.setDirty();
					table.fcParent.invalidate(table.getRepaintRect(false));
					table.resetCoveredCells();
					table.setHasSpanningCells();
				}
			}

			public int RowSpan
			{
				get { return rowSpan; }
				set
				{
					if (value < 1)
						return;

					if (rowSpan == value)
						return;

					rowSpan = value;
					layoutText();
					table.fcParent.setDirty();
					table.fcParent.invalidate(table.getRepaintRect(false));
					table.resetCoveredCells();
					table.setHasSpanningCells();
				}
			}

			private int columnSpan;
			private int rowSpan;
		}

		/// <summary>
		/// Collection of cells.
		/// </summary>
		public class CellCollection : CollectionBase, IPersists
		{
			public CellCollection()
			{
			}

			public CellCollection Clone()
			{
				CellCollection cc = new CellCollection();
				foreach (Cell c in this)
					cc.Add(c.Clone());
				return cc;
			}

			public void Add(Cell r)
			{
				InnerList.Add(r);
			}

			public void Insert(int i, Cell r)
			{
				InnerList.Insert(i, r);
			}

			public Cell GetAt(int i)
			{
				if(i >= Count)
					return null;

				return (Cell)InnerList[i];
			}

			public void Remove(Cell r)
			{
				InnerList.Remove(r);
			}

			public Cell this[int index]
			{
				get
				{
					return (Cell)InnerList[index];
				}
				set
				{
					InnerList[index] = value;
				}
			}

			public virtual int getClassId()
			{
				return 21;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				writer.Write(InnerList.Count);
				for (int i = 0; i < InnerList.Count; ++i)
				{
					ctx.saveReference(this, (Cell)InnerList[i], i);
				}
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				int count = reader.ReadInt32();
				InnerList.Capacity = count;
				for (int i = 0; i < count; ++i)
				{
					InnerList.Add(null);
					ctx.loadReference(this);
				}
			}

			public virtual void setReference(int refId, IPersists obj)
			{
				InnerList[refId] = obj;
			}
		}

		public class Row : IPersists
		{
			internal Row() {}

			public Row(Table table)
			{
				height = 22;
				incomingArrows = new ArrowCollection();
				outgoingArrows = new ArrowCollection();
				relatedOneToMany = new TableCollection();
				relatedManyToOne= new TableCollection();
				this.table = table;
				anchorPattern = null;

				header = false;
				expanded = true;
			}

			public Row Clone()
			{
				Row row = new Row(table);

				row.height = this.height;
				row.anchorPattern = this.anchorPattern;
				row.incomingArrows = this.incomingArrows.Clone();
				row.outgoingArrows = this.outgoingArrows.Clone();
				row.relatedOneToMany = this.relatedOneToMany.Clone();
				row.relatedManyToOne = this.relatedManyToOne.Clone();

				row.header = this.header;
				row.expanded = this.expanded;

				return row;
			}

			internal Table table;

			private float height;
			private ArrowCollection incomingArrows;
			private ArrowCollection outgoingArrows;
			private TableCollection relatedOneToMany;
			private TableCollection relatedManyToOne;
			private bool header;
			private bool expanded;

			public float Height
			{
				get
				{
					return height;
				}
				set
				{
					height = value;
				}
			}

			public bool Header
			{
				get { return header; }
				set
				{
					header = value;
					if (value)
						table.hasHeaderRows = true;
					else
						table.checkForHeaderRows();
				}
			}

			internal void setHeader(bool value)
			{
				header = value;
			}

			public bool Expanded
			{
				get { return expanded; }
				set
				{
					if (expanded != value)
					{
						int index = table.Rows.IndexOf(this);
						ExpandHeaderRowCmd cmd =
							new ExpandHeaderRowCmd(table, index);
						setExpanded(value, index);
						table.Parent.UndoManager.executeCommand(cmd);

						table.Parent.setDirty();
						table.Parent.invalidate(table.getRepaintRect(true));
					}
				}
			}

			internal void setExpanded(bool value, int index)
			{
				expanded = value;
				table.updateArrowsPos(index);
			}

			internal bool IsExpanded
			{
				get { return !header || expanded; }
			}

			public ArrowCollection IncomingArrows
			{
				get { return incomingArrows; }
			}

			public ArrowCollection OutgoingArrows
			{
				get { return outgoingArrows; }
			}

			public TableCollection RelatedOneToMany
			{
				get
				{
					relatedOneToMany.Clear();
					foreach (Arrow arrow in incomingArrows)
					{
						ChartObject obj = arrow.Origin;
						if (obj.getType() == ItemType.Table)
							relatedOneToMany.Add((Table)obj);
					}
					return relatedOneToMany;
				}
			}

			public TableCollection RelatedManyToOne
			{
				get
				{
					relatedManyToOne.Clear();
					foreach (Arrow arrow in outgoingArrows)
					{
						ChartObject obj = arrow.Origin;
						if (obj.getType() == ItemType.Table)
							relatedManyToOne.Add((Table)obj);
					}
					return relatedManyToOne;
				}
			}

			public AnchorPattern AnchorPattern
			{
				get { return anchorPattern; }
				set
				{
					if (value == null || value.isRegistered)
						anchorPattern = value;
					else
						anchorPattern = (AnchorPattern)value.Clone();
				}
			}

			private AnchorPattern anchorPattern;

			public virtual int getClassId()
			{
				return 22;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				writer.Write((double)height);
				ctx.saveReference(this, table, 1);
				ctx.saveReference(this, incomingArrows, 2);
				ctx.saveReference(this, outgoingArrows, 3);
				ctx.saveReference(this, relatedOneToMany, 4);
				ctx.saveReference(this, relatedManyToOne, 5);

				// new in save format 6
				ctx.saveAnchorPattern(anchorPattern);

				// new in save format 22
				writer.Write(header);
				writer.Write(expanded);
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				height = (float)reader.ReadDouble();
				ctx.loadReference(this);
				ctx.loadReference(this);
				ctx.loadReference(this);
				ctx.loadReference(this);
				ctx.loadReference(this);

				if (ctx.FileVersion > 5)
				{
					// new in save format 6
					anchorPattern = ctx.loadAnchorPattern();

					if (ctx.FileVersion > 21)
					{
						// new in save format 22
						header = reader.ReadBoolean();
						expanded = reader.ReadBoolean();
					}
				}
			}

			public virtual void setReference(int refId, IPersists obj)
			{
				switch (refId)
				{
				case 1:
					table = (Table)obj;
					break;
				case 2:
					incomingArrows = (ArrowCollection)obj;
					break;
				case 3:
					outgoingArrows = (ArrowCollection)obj;
					break;
				case 4:
					relatedOneToMany = (TableCollection)obj;
					break;
				case 5:
					relatedManyToOne = (TableCollection)obj;
					break;
				}
			}
		}

		/// <summary>
		/// Collection of rows.
		/// </summary>
		public class RowCollection : CollectionBase, IPersists
		{
			public RowCollection()
			{
			}

			public RowCollection Clone()
			{
				RowCollection rc = new RowCollection();
				foreach (Row r in this)
					rc.Add(r.Clone());
				return rc;
			}

			public void Add(Row r)
			{
				List.Add(r);
			}

			public void Insert(int i, Row r)
			{
				List.Insert(i, r);
			}

			public Row GetAt(int i)
			{
				if(i >= Count)
					return null;

				return (Row)List[i];
			}

			public void Remove(Row r)
			{
				List.Remove(r);
			}

			public Row this[int index]
			{
				get
				{
					return (Row)List[index];
				}
				set
				{
					List[index] = value;
				}
			}

			public int IndexOf(Row row)
			{
				return List.IndexOf(row);
			}

			public virtual int getClassId()
			{
				return 23;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				writer.Write(List.Count);
				for (int i = 0; i < List.Count; ++i)
				{
					ctx.saveReference(this, (Row)List[i], i);
				}
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				int count = reader.ReadInt32();
				InnerList.Capacity = count;
				for (int i = 0; i < count; ++i)
				{
					InnerList.Add(null);
					ctx.loadReference(this);
				}
			}

			public virtual void setReference(int refId, IPersists obj)
			{
				InnerList[refId] = obj;
			}
		}

		public class Column : IPersists
		{
			internal Column() {}

			public Column(Table table)
			{
				width = 50;
				columnStyle = ColumnStyle.FixedWidth;
				this.table = table;
			}

			public Column Clone()
			{
				Column col = new Column(table);

				col.width = this.width;
				col.columnStyle = this.columnStyle;

				return col;
			}

			internal Table table;

			internal float width;
			private ColumnStyle columnStyle;

			public float Width
			{
				get
				{
					return width;
				}
				set
				{
					if (width != value)
					{
						width = value;
						table.calcColWidths();
						table.fcParent.setDirty();
						table.fcParent.invalidate(table.getRepaintRect(false));
					}
				}
			}

			public ColumnStyle ColumnStyle
			{
				get
				{
					return columnStyle;
				}
				set
				{
					if (columnStyle != value)
					{
						columnStyle = value;
						table.calcColWidths();
						table.fcParent.setDirty();
						table.fcParent.invalidate(table.getRepaintRect(false));
					}
				}
			}

			public virtual int getClassId()
			{
				return 24;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				ctx.saveReference(this, table, 1);
				writer.Write((double)width);
				writer.Write((int)columnStyle);
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				ctx.loadReference(this);
				width = (float)reader.ReadDouble();
				columnStyle = (ColumnStyle)reader.ReadInt32();
			}

			public virtual void setReference(int refId, IPersists obj)
			{
				table = (Table)obj;
			}
		}

		/// <summary>
		/// Collection of columns.
		/// </summary>
		public class ColumnCollection : CollectionBase, IPersists
		{
			public ColumnCollection()
			{
			}

			public ColumnCollection Clone()
			{
				ColumnCollection cc = new ColumnCollection();
				foreach (Column c in this)
					cc.Add(c.Clone());
				return cc;
			}

			public void Add(Column r)
			{
				List.Add(r);
			}

			public void Insert(int i, Column r)
			{
				List.Insert(i, r);
			}

			public Column GetAt(int i)
			{
				if(i >= Count)
					return null;

				return (Column)List[i];
			}

			public void Remove(Column r)
			{
				List.Remove(r);
			}

			public Column this[int index]
			{
				get
				{
					return (Column)List[index];
				}
				set
				{
					List[index] = value;
				}
			}

			public virtual int getClassId()
			{
				return 25;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				writer.Write(List.Count);
				for (int i = 0; i < List.Count; ++i)
				{
					ctx.saveReference(this, (Column)List[i], i);
				}
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				int count = reader.ReadInt32();
				InnerList.Capacity = count;
				for (int i = 0; i < count; ++i)
				{
					InnerList.Add(null);
					ctx.loadReference(this);
				}
			}

			public virtual void setReference(int refId, IPersists obj)
			{
				InnerList[refId] = obj;
			}
		}

		public Table(FlowChart parent) : base(parent)
		{
			captionBackBrush = null;
			hasSpanningCells = false;
			coveredCells = null;
			maxRowSpan = 1;

			pen = (Pen)fcParent.TablePen.Clone();
			brush = parent.TableBrush;
			brush.AddRef();

			this.cells = null;
			this.rowsList = null;
			this.colsList = null;
			columnWidth = parent.TableColWidth;
			rowHeight = parent.TableRowHeight;
			redimTable(parent.TableColumnCount, parent.TableRowCount);

			caption = parent.TableCaption;
			captionColor = Color.FromArgb(0, 0, 0);
			fillColor = parent.TableFillColor;
			frameColor = parent.TableFrameColor;
			captionFormat = new StringFormat();
			captionFormat.Alignment = StringAlignment.Center;
			captionFormat.LineAlignment = StringAlignment.Center;
			captionHeight = parent.TableCaptionHeight;

			this.picture = null;

			cellBorders = parent.CellFrameStyle;
			setScrollable(parent.TablesScrollable);
			currScrollRow = 0;

			setExpandable(parent.TablesExpandable);
			setExpanded(true);

			textColor = parent.TextColor;
			picturePos = ImageAlign.Fit;

			selStyle = parent.TableHandlesStyle;
			style = parent.TableStyle;

			rowAnchorPattern = null;

			customDraw = parent.TableCustomDraw;
			cellCustomDraw = parent.CellCustomDraw;

			linkStyle = parent.TableLinkStyle;

			lastAnchorForTable = false;
			lastFoundAnchor = new PointF(0, 0);

			useStyledText = fcParent.EnableStyledText;
			txOptions = new Text.LayoutOptions();
			txLayout = new Text.Layout();
			updateText();
			layoutText();

			hasHeaderRows = false;
			offsetHeaderRows = false;

			if (imageExpand == null)
			{
				lock (typeof(Table))
				{
					if (imageExpand == null)
					{
						ResourceManager rm = new ResourceManager(
							"MindFusion.FlowChartX.Icons", typeof(ChartObject).Assembly);
						imageExpand = rm.GetObject("mnp_rowexpand") as Image;
						imageCollapse = rm.GetObject("mnp_rowcollapse") as Image;
					}
				}
			}
		}

		public Table(Table prototype) : base(prototype)
		{
			captionBackBrush = prototype.captionBackBrush;
			if (captionBackBrush != null)
				captionBackBrush.AddRef();

			hasSpanningCells = prototype.hasSpanningCells;
			coveredCells = null;
			maxRowSpan = prototype.maxRowSpan;

			this.cells = null;
			this.rowsList = null;
			this.colsList = null;
			columnWidth = prototype.columnWidth;
			rowHeight = prototype.rowHeight;
			redimTable(prototype.ColumnCount, prototype.RowCount);

			caption = prototype.caption;
			captionColor = prototype.captionColor;
			captionFormat = (StringFormat)prototype.captionFormat.Clone();
			captionHeight = prototype.captionHeight;

			picture = prototype.picture;

			cellBorders = prototype.cellBorders;
			setScrollable(prototype.scrollable);
			currScrollRow = prototype.currScrollRow;

			setExpandable(prototype.Expandable);
			setExpanded(prototype.Expanded);

			textColor = prototype.textColor;
			picturePos = prototype.picturePos;

			selStyle = prototype.selStyle;
			style = prototype.style;

			RowAnchorPattern = prototype.RowAnchorPattern;

			customDraw = prototype.CustomDraw;
			cellCustomDraw = prototype.CellCustomDraw;

			linkStyle = prototype.linkStyle;

			for (int c = 0; c < colsList.Count; ++c)
			{
				colsList[c].ColumnStyle = prototype.colsList[c].ColumnStyle;
				colsList[c].Width = prototype.colsList[c].Width;
			}

			for (int r = 0; r < rowsList.Count; ++r)
			{
				rowsList[r].AnchorPattern = prototype.rowsList[r].AnchorPattern;
				rowsList[r].Height = prototype.rowsList[r].Height;
				rowsList[r].setHeader(prototype.rowsList[r].Header);
			}

			for (int c = 0; c < colsList.Count; ++c)
			{
				for (int r = 0; r < rowsList.Count; ++r)
				{
					this[c, r].ToolTip = prototype[c, r].ToolTip;
					this[c, r].TextFormat = prototype[c, r].TextFormat;
					this[c, r].Text = prototype[c, r].Text;
					this[c, r].PicturePos = prototype[c, r].PicturePos;
					this[c, r].Picture = prototype[c, r].Picture;
					this[c, r].ColumnSpan = prototype[c, r].ColumnSpan;
					this[c, r].RowSpan = prototype[c, r].RowSpan;
				}
			}

			hasHeaderRows = prototype.hasHeaderRows;
			offsetHeaderRows = prototype.offsetHeaderRows;

			useStyledText = prototype.EnableStyledText;
			txOptions = new Text.LayoutOptions();
			txLayout = new Text.Layout();

			updateText();
			layoutText();
			updateCellText();
			layoutCellText();
		}

		internal override void onRemove()
		{
			base.onRemove();

			// fire a TableDeleted event
			if (constructed)
				fcParent.fireTableDeleted(this);
		}

		internal override void freeResources()
		{
			base.freeResources();

			// cleanup
			if (picture != null)
			{
				//picture.Dispose();
				picture = null;
				System.GC.Collect();
			}

			if (cells != null)
			{
				foreach (Cell cell in cells)
					cell.freeResources();
				cells.Clear();
			}
			cells = null;

			if (rowsList != null) rowsList.Clear();
			rowsList = null;

			if (colsList != null) colsList.Clear();
			colsList = null;

			resetCoveredCells();

			if (captionBackBrush != null)
				captionBackBrush.Release();
			captionBackBrush = null;
		}

		private CellCollection cells;
		private RowCollection rowsList;
		private ColumnCollection colsList;

		public RowCollection Rows
		{
			get { return rowsList; }
		}

		public ColumnCollection Columns
		{
			get { return colsList; }
		}


		public int CurrScrollRow
		{
			get { return currScrollRow; }
			set
			{
				if (currScrollRow != value)
				{
					ScrollTableCmd cmd = new ScrollTableCmd(this);
					setCurrScrollRow(value);
					fcParent.UndoManager.executeCommand(cmd);

					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(true));
				}
			}
		}

		internal void setCurrScrollRow(int row)
		{
			if (row < 0) return;
			if (row >= rowsList.Count) return;

			currScrollRow = row;
			updateArrowsPos(0);
		}

		private int currScrollRow;


		private Scroller scroller = null;

		public bool Scrollable
		{
			get { return scrollable; }
			set
			{
				if (scrollable != value)
				{
					setScrollable(value);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private bool scrollable;

		private void setScrollable(bool scr)
		{
			if (scroller != null)
			{
				removeManipulator(scroller);
				scroller = null;
			}

			scrollable = scr;
			if (scrollable)
				addManipulator(scroller = new Scroller(this));

			layoutText();
		}

		internal override void expand()
		{
			// Expand on rows
			if (!isExpanded())
			{
				if (fcParent.ExpandOnIncoming)
				{
					foreach (Row row in rowsList)
						foreach (Arrow a in row.IncomingArrows)
							a.expand(true);
				}
				else
				{
					foreach (Row row in rowsList)
						foreach (Arrow a in row.OutgoingArrows)
							a.expand(false);
				}
			}

			base.expand();
		}

		internal override void collapse()
		{
			// Collapse on rows
			if (isExpanded())
			{
				if (fcParent.ExpandOnIncoming)
				{

					foreach (Row row in rowsList)
						foreach (Arrow a in row.IncomingArrows)
							a.collapse(true);
				}
				else
				{
					foreach (Row row in rowsList)
						foreach (Arrow a in row.OutgoingArrows)
							a.collapse(false);
				}
			}

			base.collapse();
		}

		private Image picture;

		private string caption;
		private Color captionColor;
		private StringFormat captionFormat;
		private float captionHeight;

		private bool useStyledText;
		private Text.Text fcaption;
		private Text.Layout txLayout;
		private Text.LayoutOptions txOptions;
		private static StringFormat nearFormat = new StringFormat();

		public string CaptionPlainText
		{
			get
			{
				if (fcaption == null)
					return caption;

				return fcaption.PlainText;
			}
		}

		public override System.Drawing.Font Font
		{
			get { return base.Font; }
			set
			{
				base.Font = value;

				updateText();
				layoutText();
				updateCellText();
				layoutCellText();
			}
		}

		public bool EnableStyledText
		{
			get { return useStyledText; }
			set
			{
				if (useStyledText != value)
				{
					useStyledText = value;

					updateText();
					layoutText();
					updateCellText();
					layoutCellText();

					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		/// <summary>
		/// Updates the internal MindFusion.FlowChartX.Text.Text
		/// obejct when styled characteristics of
		/// a table's text have changed.
		/// </summary>
		private void updateText()
		{
			if (useStyledText)
			{
				Graphics graphics = fcParent.CreateGraphics();
				fcParent.setTransforms(graphics);

				// Create and setup styled text
				fcaption = new Text.StyledText();
				fcaption.Setup(caption, graphics, Font);

				graphics.Dispose();
			}
			else
			{
				// Text won't be used
				fcaption = null;
			}
		}

		/// <summary>
		/// Performs the actual text layouting so that
		/// the internal MindFusion.FlowChartX.Text.Layout object
		/// is prepared for subsequent drawing operations.
		/// Returns whether text fits entirely whithin the area.
		/// </summary>
		private bool layoutText()
		{
			// Check if styled text is switched on for this table
			if (!useStyledText)
				return false;

			// Ensure that the layout options are adequate
			txOptions.LineDistance = 0;
			txOptions.Alignment = captionFormat.Alignment;
			txOptions.LineAlignment = captionFormat.LineAlignment;

			RectangleF rcCaption = getCapTextRect();

			// Perform the layout
			return txLayout.LayoutInRectangle(fcaption, docToLocal(rcCaption), txOptions);
		}

		/// <summary>
		/// Transforms the specified rectangle from document
		/// coordinates to coordinates relative to the left-top
		/// point of the table.
		/// </summary>
		private RectangleF docToLocal(RectangleF rect)
		{
			RectangleF result = rect;
			RectangleF nrect = Utilities.normalizeRect(this.rect);

			result.X -= nrect.X;
			result.Y -= nrect.Y;

			return result;
		}

		/// <summary>
		/// Performs updating of the text contained within table cells.
		/// </summary>
		private void updateCellText()
		{
			if (cells == null)
				return;

			foreach (Cell cell in cells)
				cell.updateText();
		}

		/// <summary>
		/// Performs layouting of the texs contained within table cells.
		/// </summary>
		private void layoutCellText()
		{
			if (cells == null)
				return;

			foreach (Cell cell in cells)
				cell.layoutText();
		}

		/// <summary>
		/// Styled text rendering callback.
		/// </summary>
		private void DrawText(string text, RectangleF dest, DrawTextHint hint)
		{
			// Render formatted text
			hint.Graphics.DrawString(text, hint.Font, hint.Brush,
				dest, hint.Format);
		}

		private CellFrameStyle cellBorders;
		private int rowsCount;
		private float rowHeight;
		private int columnsCount;
		private float columnWidth;

		/// <summary>
		/// Image painted inside the table.
		/// </summary>
		public Image Picture
		{
			get
			{
				return picture;
			}
			set
			{
				picture = value;
				fcParent.setDirty();
				fcParent.invalidate(getRepaintRect(false));
			}
		}


		/// <summary>
		/// Table caption text.
		/// </summary>
		public string Caption
		{
			get
			{
				return caption;
			}
			set
			{
				string newText = value;
				if (newText == null)
					newText = "";
				if (fcParent.IsTrialVersion && ZIndex > 50)
					newText = "FlowChart.NET\nDemo Version";

				if (!caption.Equals(newText))
				{
					caption = newText;

					updateText();
					layoutText();
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		/// <summary>
		/// Color of the table caption.
		/// </summary>
		public Color CaptionColor
		{
			get
			{
				return captionColor;
			}
			set
			{
				if (!captionColor.Equals(value))
				{
					captionColor = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		/// <summary>
		/// The caption text position and alignment.
		/// </summary>
		[Editor(typeof(MindFusion.FlowChartX.Design.StringFormatEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public StringFormat CaptionFormat
		{
			get
			{
				return captionFormat;
			}
			set
			{
				if (!captionFormat.Equals(value))
				{
					captionFormat = new StringFormat(value);

					layoutText();
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		/// <summary>
		/// Height of the table caption.
		/// </summary>
		public float CaptionHeight
		{
			get
			{
				return captionHeight;
			}
			set
			{
				if (captionHeight != value)
				{
					captionHeight = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(true));
				}
			}
		}


		/// <summary>
		/// Number of the table columns.
		/// </summary>
		public int ColumnCount
		{
			get
			{
				return columnsCount;
			}
			set
			{
				if (value < 0) return;

				int oldRows = rowsCount;
				int oldCols = columnsCount;

				if (oldCols != value)
				{
					bool insert = oldCols < value;
					redimTable(value, oldRows, true, insert ? oldCols : value, oldRows,
						insert, Math.Abs(oldCols - value), 0, true);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(true));
				}
			}
		}

		/// <summary>
		/// Number of the table rows.
		/// </summary>
		public int RowCount
		{
			get
			{
				return rowsCount;
			}
			set
			{
				if (value < 0) return;

				int oldRows = rowsCount;
				int oldCols = columnsCount;

				if (oldRows != value)
				{
					bool insert = oldRows < value;
					redimTable(oldCols, value, true, oldCols, insert ? oldRows : value,
						insert, 0, Math.Abs(oldRows - value), true);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(true));
				}
			}
		}

		public CellFrameStyle CellFrameStyle
		{
			get
			{
				return cellBorders;
			}
			set
			{
				if (cellBorders != value)
				{
					cellBorders = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		public float ColumnWidth
		{
			get
			{
				return columnWidth;
			}
			set
			{
				columnWidth = value;
			}
		}

		public float RowHeight
		{
			get
			{
				return rowHeight;
			}
			set
			{
				rowHeight = value;
			}
		}

		public Cell this[int col, int row]
		{
			get
			{
				return cells[row * colsList.Count + col];
			}
		}


/*		public void Destroy()
		{
			cells.Clear();
			rowsList.Clear();
			colsList.Clear();
		}
*/
		/// <summary>
		/// Adds relation to a table field.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="rel">Desired relation type.</param>
		/// <param name="table">Related table object.</param>
		/// <param name="relRow">Related table row index.</param>
		/// <returns></returns>
		public Arrow AddRelation(int row, Relationship rel, Table table, int relRow)
		{
			if (this.Rows == null) return null;
			if (row < 0 || row >= this.Rows.Count) return null;

			switch (rel)
			{
			case Relationship.OneToMany:
				return fcParent.CreateArrow(table, relRow, this, row);
			case Relationship.ManyToOne:
				return fcParent.CreateArrow(this, row, table, relRow);
			}

			return null;
		}

		/// <summary>
		/// Breaks a relationship.
		/// </summary>
		/// <param name="row">The row index.</param>
		/// <param name="rel">The type of the relation to break.</param>
		/// <param name="table">Related table object.</param>
		public void BreakRelation(int row, Relationship rel, Table table)
		{
			if (this.Rows == null) return;
			if (row < 0 || row >= this.Rows.Count) return;

			fcParent.invalidate(getRepaintRect(true));
			fcParent.setDirty();

			this.removeRelation(row, rel, table);
		}

		/// <summary>
		/// Makes the table the same size as its picture.
		/// </summary>
		public void FitSizeToPicture()
		{
			fcParent.invalidate(getRepaintRect(true));

			if (picture != null)
			{
				int w = picture.Width;
				int h = picture.Height;

				Graphics g = fcParent.CreateGraphics();
				fcParent.setTransforms(g);
				RectangleF sizeDev = RectangleF.FromLTRB(0, 0,
					(float)w / picture.HorizontalResolution * g.DpiX * g.PageScale,
					(float)h / picture.VerticalResolution * g.DpiY * g.PageScale);
				RectangleF newRect = Utilities.deviceToDoc(g, sizeDev);

				newRect.X = rect.X;
				newRect.Y = rect.Y;
				rect = newRect;
				g.Dispose();

				calcColWidths();

				visitArrows(
					new MethodCallVisitor(new VisitOperation(AV_UpdateOutgoing)),
					new MethodCallVisitor(new VisitOperation(AV_UpdateIncoming)));

				if (groupAttached != null)
					groupAttached.updateObjects(new InteractionState(this, -1, Action.Modify));
			}

			fcParent.invalidate(getRepaintRect(true));
			fcParent.setDirty();
		}

		internal override bool manipulatorEnacted(PointF pt)
		{
			if (base.manipulatorEnacted(pt))
				return true;

			if (hasHeaderRows)
			{
				RectangleF rc = Utilities.normalizeRect(rect);

				// Check if the click was on a header row
				for (int i = CurrentRow; i < rowsList.Count; i++)
				{
					RectangleF rowRect = getRowRect(i);
					if (rowRect.Top >= rc.Bottom)
						return false;

					Row row = rowsList[i];
					if (!row.Header)
						continue;

					RectangleF iconRect = rowRect;
					iconRect.Width = HeaderRowIconWidth;

					float iconWidth = Constants.getPixel(fcParent.MeasureUnit) * 9;
					float iconHeight = Constants.getPixel(fcParent.MeasureUnit) * 9;

					iconRect.X = iconRect.X + iconRect.Width / 2 - iconWidth / 2;
					iconRect.Y = iconRect.Y + iconRect.Height / 2 - iconHeight / 2;
					iconRect.Width = iconWidth;
					iconRect.Height = iconHeight;

					if (iconRect.Contains(pt))
					{
						row.Expanded = !row.Expanded;
						fcParent.fireTableSectionEvent(this, i, row.Expanded);
						return true;
					}
				}
			}

			return false;
		}

		internal override bool ptInManipulator(PointF pt)
		{
			if (base.ptInManipulator(pt))
				return true;

			if (hasHeaderRows)
			{
				RectangleF rc = Utilities.normalizeRect(rect);

				// Check if the click was on a header row
				for (int i = CurrentRow; i < rowsList.Count; i++)
				{
					RectangleF rowRect = getRowRect(i);
					if (rowRect.Top >= rc.Bottom)
						return false;

					Row row = rowsList[i];
					if (!row.Header)
						continue;

					RectangleF iconRect = rowRect;
					iconRect.Width = HeaderRowIconWidth;

					float iconWidth = Constants.getPixel(fcParent.MeasureUnit) * 9;
					float iconHeight = Constants.getPixel(fcParent.MeasureUnit) * 9;

					iconRect.X = iconRect.X + iconRect.Width / 2 - iconWidth / 2;
					iconRect.Y = iconRect.Y + iconRect.Height / 2 - iconHeight / 2;
					iconRect.Width = iconWidth;
					iconRect.Height = iconHeight;

					if (iconRect.Contains(pt))
						return true;
				}
			}

			return false;
		}

		/// <summary>
		/// Adds a row to the table.
		/// </summary>
		public int AddRow()
		{
			InsertRow(Rows.Count);
			return Rows.Count - 1;
		}

		/// <summary>
		/// Inserts a row in the table.
		/// </summary>
		/// <param name="row">Insert position.</param>
		public void InsertRow(int row)
		{
			int oldRows = rowsList.Count;
			int oldCols = colsList.Count;

			if (row > oldRows || row < 0) return;

			redimTable(oldCols, oldRows + 1,
				true, oldCols, row, true, 0, 1, true);
			fcParent.setDirty();
			fcParent.invalidate(getRepaintRect(true));
		}

		/// <summary>
		/// Deletes a row from the table.
		/// </summary>
		/// <param name="row">The index of the row to delete.</param>
		public void DeleteRow(int row)
		{
			int oldRows = rowsList.Count;
			int oldCols = colsList.Count;

			if (oldRows == 0 || row >= oldRows || row < 0) return;

			redimTable(oldCols, oldRows - 1,
				true, oldCols, row, false, 0, 1, true);
			fcParent.setDirty();
			fcParent.invalidate(getRepaintRect(true));
		}

		/// <summary>
		/// Adds a column to the table.
		/// </summary>
		public int AddColumn()
		{
			InsertColumn(colsList.Count);
			return Columns.Count - 1;
		}

		/// <summary>
		/// Inserts a column in the table.
		/// </summary>
		/// <param name="col">Insert position.</param>
		public void InsertColumn(int col)
		{
			int oldRows = rowsList.Count;
			int oldCols = colsList.Count;

			if (col > oldCols || col < 0) return;

			redimTable(oldCols + 1, oldRows,
				true, col, oldRows, true, 1, 0, true);
			fcParent.setDirty();
			fcParent.invalidate(getRepaintRect(true));
		}

		/// <summary>
		/// Deletes a column from the table.
		/// </summary>
		/// <param name="col">The index of the column to delete.</param>
		public void DeleteColumn(int col)
		{
			int oldRows = rowsList.Count;
			int oldCols = colsList.Count;

			if (oldCols == 0 || col >= oldCols || col < 0) return;

			redimTable(oldCols - 1, oldRows,
				true, col, oldRows, false, 1, 0, true);
			fcParent.setDirty();
			fcParent.invalidate(getRepaintRect(true));
		}

		public void RedimTable(int cols, int rows)
		{
			redimTable(cols, rows, false, 0, 0, true, 1, 1, true);
		}

		internal void redimTable(int cols, int rows)
		{
			redimTable(cols, rows, false, 0, 0, true, 1, 1, false);
		}

		internal void redimTable(int cols, int rows, bool copy)
		{
			redimTable(cols, rows, copy, 0, 0, true, 1, 1, true);
		}

		internal void redimTable(int cols, int rows, bool copy,
			int cpCol)
		{
			redimTable(cols, rows, copy, cpCol, 0, true, 1, 1, true);
		}

		internal void redimTable(int cols, int rows, bool copy,
			int cpCol, int cpRow)
		{
			redimTable(cols, rows, copy, cpCol, cpRow, true, 1, 1, true);
		}

		internal void redimTable(int cols, int rows, bool copy,
			int cpCol, int cpRow, bool cpInserted)
		{
			redimTable(cols, rows, copy, cpCol, cpRow, cpInserted, 1, 1, true);
		}

		internal void redimTable(int cols, int rows, bool copy,
			int cpCol, int cpRow, bool cpInserted,
			int colsAffected)
		{
			redimTable(cols, rows, copy, cpCol, cpRow, cpInserted,
				colsAffected, 1, true);
		}

		internal void redimTable(int cols, int rows, bool copy,
			int cpCol, int cpRow, bool cpInserted,
			int colsAffected, int rowsAffected, bool undo)
		{
			resetCoveredCells();
			hasHeaderRows = false;

			if (undo) fcParent.UndoManager.onRedimTable(this);

			// remove arrows attached to deleted rows
			if (!cpInserted && (rowsAffected != 0) && (rowsList != null))
			{
				ArrayList arrowsToRemove = new ArrayList();
				for (int i = cpRow; i < cpRow + rowsAffected && i < this.rowsCount; ++i)
				{
					Row r = (Row)this.rowsList[i];
					for (int k = 0; k < r.IncomingArrows.Count; ++k)
						arrowsToRemove.Add(r.IncomingArrows[k]);
					for (int k = 0; k < r.OutgoingArrows.Count; ++k)
						arrowsToRemove.Add(r.OutgoingArrows[k]);
				}
				for (int j = 0; j < arrowsToRemove.Count; ++j)
					fcParent.DeleteObject((ChartObject)arrowsToRemove[j]);
				arrowsToRemove.Clear();
			}

			if (undo) fcParent.UndoManager.onRedimArrowsDeleted(this);

			CellCollection oldData = cells;
			RowCollection oldRowsDsc = rowsList;
			ColumnCollection oldColsDsc = colsList;
			int oldCols = this.columnsCount;
			int oldRows = this.rowsCount;
			bool copyC, copyR;
			copyC = copyR = copy;

			// resize the table
			this.columnsCount = cols;
			this.rowsCount = rows;

			// resize the columns array
			colsList = new ColumnCollection();
			if (this.columnsCount > 0)
			{
				for (int c = 0; c < this.columnsCount; ++c)
				{
					Column col = new Column(this);
					col.width = columnWidth;
					colsList.Add(col);
				}
			}
			else
			{
				copyC = false;
			}

			// resize the rows array
			rowsList = new RowCollection();
			if (this.rowsCount > 0)
			{
				for (int rr = 0; rr < this.rowsCount; ++rr)
				{
					Row row = new Row(this);
					row.Height = rowHeight;
					rowsList.Add(row);
				}
			}
			else
			{
				copyR = false;
			}

			// resize the cells array
			cells = new CellCollection();
			if (this.columnsCount > 0 && this.rowsCount > 0)
			{
				for (int i = 0; i < this.columnsCount * this.rowsCount; ++i)
					cells.Add(new Cell(this));
			}
			else
			{
				copy = false;
			}

			// copy data from the old cells array to the new one
			if (copy && (cells != null) && (oldData != null))
			{
				for (int rr = 0; rr < this.rowsCount; ++rr)
				{
					for (int c = 0; c < this.columnsCount; ++c)
					{
						int cr = rr;
						int cc = c;
						bool cp = true;
						if (cpInserted)
						{
							if (c >= cpCol && c < cpCol + colsAffected) cp = false;
							if (rr >= cpRow && rr < cpRow + rowsAffected) cp = false;
							if (c >= cpCol + colsAffected) cc -= colsAffected;
							if (rr >= cpRow + rowsAffected) cr -= rowsAffected;
						}
						else
						{
							if (c >= cpCol) cc += colsAffected;
							if (rr >= cpRow) cr += rowsAffected;
						}
						if (cp)
						{
							cells[rr * this.columnsCount + c] = oldData[cr * oldCols + cc];
							oldData[cr * oldCols + cc] = null;
						}
						else
							cells[rr * this.columnsCount + c] = new Cell(this);
					}
				}
			}

			if (oldData != null)
			{
				for (int oc = 0; oc < oldData.Count; ++oc)
				{
					if (oldData[oc] != null)
						oldData[oc].freeResources();
				}
			}

			// copy data from the old rows array to the new one
			if (copyR && (rowsList != null) && (oldRowsDsc != null))
			{
				for (int rr = 0; rr < this.rowsCount; ++rr)
				{
					int cr = rr;
					bool cp = true;
					if (cpInserted)
					{
						if (rr >= cpRow && rr < cpRow + rowsAffected) cp = false;
						if (rr >= cpRow + rowsAffected) cr -= rowsAffected;
					}
					else
					{
						if (rr >= cpRow) cr += rowsAffected;
					}
					if (cp)
						rowsList[rr] = oldRowsDsc[cr];
				}
			}

			// copy data from the old columns array to the new one
			if (copyC && (colsList != null) && (oldColsDsc != null))
			{
				for (int c = 0; c < this.columnsCount; ++c)
				{
					int cc = c;
					bool cp = true;
					if (cpInserted)
					{
						if (c >= cpCol && c < cpCol + colsAffected) cp = false;
						if (c >= cpCol + colsAffected) cc -= colsAffected;
					}
					else
					{
						if (c >= cpCol) cc += colsAffected;
					}
					if (cp)
						colsList[c] = oldColsDsc[cc];
				}
			}

			if (rowsAffected > 0)
			{
				updateLinksIndices();
				updateArrowsPos(cpRow);
			}

			if (copy && rowsAffected != 0)
				checkForHeaderRows();

			if (undo) fcParent.UndoManager.onCompleteRedim();
		}

		protected override void onRectUpdate()
		{
			calcColWidths();

			layoutText();
			layoutCellText();
		}


		// ************ mouse pointers ************

		internal override Cursor getCannotDropCursor()
		{
			return fcParent.CurCannotCreate;
		}

		internal override Cursor getCanDropCursor()
		{
			return fcParent.CurPointer;
		}


		/// <summary>
		/// The box Text is painted in this color
		/// </summary>
		public Color TextColor
		{
			get
			{
				return textColor;
			}
			set
			{
				textColor = value;
			}
		}

		private Color textColor;

		private void drawCells(Graphics g, System.Drawing.Pen pen,
			System.Drawing.Brush brText, RectangleF rc)
		{
			// DrawBorder3D does not honor clipping, so...
			CellFrameStyle cb = cellBorders;
			if (fcParent.NowPrinting && cb == CellFrameStyle.System3D)
				cb = CellFrameStyle.Simple;

			// draw the cells
			if (cells != null)
			{
				bool[,] coveredCells =
					hasSpanningCells ? getCoveredCells() : null;
				RectangleF cellsRect = RectangleF.FromLTRB(rc.Left,
					rc.Top + captionHeight, rc.Right, rc.Bottom);

				int rowFrom = CurrentRow - maxRowSpan + 1;
				if (rowFrom < 0)
					rowFrom = 0;

				float h = rc.Top + captionHeight;
				for (int r = rowFrom; r < rowsCount; r++)
				{
					RectangleF cellRect = rc;
					cellRect.Y = h;
					if (cellRect.Top >= rc.Bottom) break;

					cellRect.Height = ((Row)rowsList[r]).Height;

					if (cellRect.Bottom >= rc.Bottom) break;

					// If it is a hidden row below a collapsed header row, skip it
					if (isRowCollapsed(r))
						continue;

					if (rowFrom >= currScrollRow)
						h += ((Row)rowsList[r]).Height;

					for (int c = 0; c < columnsCount; ++c)
					{
						RectangleF imgRect = RectangleF.Empty;
						cellRect = getSpannedCellRect(r, c, true, ref imgRect);

						if (cellRect.Height == 0 ||
							cellRect.Width == 0 ||
							cellRect.Bottom <= cellsRect.Top)
							continue;

						Cell cell = (Cell)cells[r * columnsCount + c];

						if (!hasSpanningCells)
						{
							cell.draw(g, pen, brText, cellRect, Font, cb, r, c, imgRect);
						}
						else
						{
							// If the cell is covered by a spanned cell
							// and is not a span cell itself, do not draw it
							if (!coveredCells[c,r] ||
								(cell.RowSpan != 1 || cell.ColumnSpan != 1))
							{
								if (cell.RowSpan != 1 || cell.ColumnSpan != 1)
								{
									RectangleF cellVisibleRect =
										RectangleF.Intersect(cellRect, cellsRect);

									// Clip with the visible rect, and draw in the
									// absolute rect
									Region oldClip = g.Clip;
									Region newClip = new Region(cellVisibleRect);
									newClip.Intersect(oldClip);
									g.Clip = newClip;

									cell.draw(g, pen, brText, cellRect, Font, cb, r, c, imgRect);

									g.Clip = oldClip;
									newClip.Dispose();
								}
								else
								{
									cell.draw(g, pen, brText, cellRect, Font, cb, r, c, imgRect);
								}
							}
						}

						cellRect.X = cellRect.Right;
						if (cellRect.Left >= rc.Right) break;
					}
				}
			}
		}

		private RectangleF getCapTextRect()
		{
			RectangleF rect = Utilities.normalizeRect(this.rect);
			rect.Height = captionHeight;
			if (scrollable)
			{
				float scrollerWth = Constants.getScrollerWdt(fcParent.MeasureUnit);
				rect.Width -= scrollerWth;
			}

			return rect;
		}

		public override void Draw(Graphics g, bool shadow)
		{
			if (invisible) return;
			if (fcParent.NowPrinting && !Printable) return;

			System.Drawing.Brush br = null;
			System.Drawing.Pen pen = null;
			Region oldClipReg = g.Clip;
			Region newClipReg = null;

			// select appropriate brush and pen
			if (shadow)
			{
				br = new System.Drawing.SolidBrush(ShadowColor);
				pen = this.Pen.CreateGDIPen();
				pen.Brush = br;
			}
			else
			{
				if (fcParent.RenderOptions.EnableInterior)
					br = this.Brush.CreateGDIBrush(rect);
				else
					br = new System.Drawing.SolidBrush(Color.FromArgb(0, 255, 255, 255));
				pen = this.Pen.CreateGDIPen();
			}

			System.Drawing.Brush brCaption = new System.Drawing.SolidBrush(captionColor);
			System.Drawing.Brush brText = new System.Drawing.SolidBrush(textColor);

			// draw with offset - used for drawing shadows
			RectangleF tableRect = Utilities.normalizeRect(rect);
			if (shadow)
				tableRect.Offset(ShadowOffsetX, ShadowOffsetY);
			RectangleF rc = tableRect;
			float minSize = 1.5f * Constants.getMinObjSize(fcParent.MeasureUnit);

			if (customDraw == CustomDraw.Full)
			{
				// call the custom draw function
				fcParent.drawTable(g, this, shadow, rc);
				if (!shadow && rc.Right - rc.Left > minSize)
					drawCells(g, pen, brText, rc);
			}
			else if (customDraw == CustomDraw.ShadowOnly && shadow)
			{
				newClipReg = getRgnInRect(rc);
				if (newClipReg != null)
					g.SetClip(newClipReg, CombineMode.Intersect);

				fcParent.drawTable(g, this, true, rc);
			}
			else
			{
				GraphicsPath roundRect = null;
				if (style == TableStyle.RoundedRectangle)
				{
					roundRect = Utilities.getRoundRect(rc.X, rc.Y, rc.Width, rc.Height,
						Constants.getRoundRectArc(fcParent.MeasureUnit, fcParent.RoundRectFactor));
				}

				// fill the table shape
				if (style == TableStyle.Rectangle)
					g.FillRectangle(br, rc);
				else
					g.FillPath(br, roundRect);

				if (!shadow && rc.Right - rc.Left > minSize)
				{
					// set clipping region if painting pictures or irregular shapes
					if (style == TableStyle.RoundedRectangle ||
						(picture != null && fcParent.RenderOptions.EnableImages) ||
						customDraw == CustomDraw.Additional)
					{
						newClipReg = getRegion();
						if (newClipReg != null)
							g.SetClip(newClipReg, CombineMode.Intersect);
					}

					// fill with color the caption area
					if (captionBackBrush != null && captionHeight > 0 &&
						fcParent.RenderOptions.EnableInterior)
					{
						RectangleF captionRect = rc;
						captionRect.Height = captionHeight + pen.Width;
						captionRect.Inflate(-pen.Width, -pen.Width);

						System.Drawing.Brush cbb =
							captionBackBrush.CreateGDIBrush(captionRect);
						g.FillRectangle(cbb, captionRect);
						cbb.Dispose();
					}

					// draw the table picture
					if (picture != null && fcParent.RenderOptions.EnableImages)
						Utilities.drawPicture(g, picture, rc, picturePos);

					// additional custom draw type
					if (customDraw == CustomDraw.Additional)
						fcParent.drawTable(g, this, false, rc);

					// DrawString crashes with a too small PageScale
					if (g.PageScale > 0.01 && caption.Length > 0 &&
						fcParent.RenderOptions.EnableText)
					{
						RectangleF rcf = getCapTextRect();
						if (useStyledText)
						{
							txLayout.Draw(rcf.Left, rcf.Top,
								new RenderTextCallback(DrawText),
								new DrawTextHint(g, Font, brCaption, nearFormat,
								Utilities.getTextEllipsis(captionFormat)));
						}
						else
						{
							g.DrawString(caption, Font, brCaption, rcf, captionFormat);
						}
					}

					// draw the caption divider line
					if (rc.Top + captionHeight < rc.Bottom)
						g.DrawLine(pen, new PointF(rc.Left, rc.Top + captionHeight),
							new PointF(rc.Right, rc.Top + captionHeight));

					// draw all visible cells
					if (hasHeaderRows)
					{
						rc.X += HeaderRowOffset;
						rc.Width -= HeaderRowOffset;
					}
					drawCells(g, pen, brText, rc);

					// draw header icons
					if (hasHeaderRows)
					{
						for (int i = CurrentRow; i < rowsList.Count; i++)
						{
							RectangleF rowRect = getRowRect(i);

							if (rowRect.Height == 0)
								continue;

							if (rowRect.Bottom > rc.Bottom)
								break;

							Row row = rowsList[i];
							if (!row.Header)
								continue;

							RectangleF iconRect = rowRect;
							iconRect.Width = HeaderRowIconWidth;

							Image image = null;
							if (row.IsExpanded)
								image = imageCollapse;
							else
								image = imageExpand;
							Utilities.drawPicture(g, image, iconRect, ImageAlign.Center);
						}
					}
				}

				// reset clipping region
				if (newClipReg != null)
				{
					g.SetClip(oldClipReg, CombineMode.Replace);
					newClipReg.Dispose();
					newClipReg = null;
				}

				// paint the table frame
				if (style == TableStyle.Rectangle)
					g.DrawRectangle(pen, tableRect.Left, tableRect.Top,
						tableRect.Width, tableRect.Height);
				else
					g.DrawPath(pen, roundRect);

				if (roundRect != null)
					roundRect.Dispose();
			}

			// draw the scroller
			if (!shadow) drawManipulators(g, true);

			// reset clipping region
			if (newClipReg != null)
			{
				g.SetClip(oldClipReg, CombineMode.Replace);
				newClipReg.Dispose();
			}

			// additional2 custom draw type
			if (customDraw == CustomDraw.Additional2)
				fcParent.drawTable(g, this, shadow, rc);

			// draw the anchor points and non-clipped manipulators
			if (!shadow)
			{
				drawManipulators(g, false);
				if (fcParent.RenderOptions.EnableAnchors)
					drawAnchors(g);
			}

			// cleanup
			brText.Dispose();
			brCaption.Dispose();
			pen.Dispose();
			br.Dispose();
		}

		internal override void drawSelHandles(Graphics g, Color color)
		{
			if (selStyle != HandlesStyle.Custom)
			{
				RectangleF rc = Utilities.normalizeRect(rect);

				if (style == TableStyle.RoundedRectangle &&
					selStyle == HandlesStyle.DashFrame)
				{
					System.Drawing.Pen pen = new System.Drawing.Pen(color, 0);
					pen.DashStyle = DashStyle.Dash;
				
					GraphicsPath gp = Utilities.getRoundRect(
						rc.X, rc.Y, rc.Width, rc.Height, Constants.getRoundRectArc(fcParent.MeasureUnit, fcParent.RoundRectFactor));
					g.DrawPath(pen, gp);

					gp.Dispose();
					pen.Dispose();

					return;
				}

				Utilities.drawSelHandles(g, color,
					fcParent.DisabledMnpColor, rc, 0, enabledHandles & ~Handles.Rotate,
					fcParent.ShowDisabledHandles, selStyle, fcParent.SelHandleSize);
			}
			else
			{
				// let applications custom draw the selection handles
				fcParent.fireDrawSelHandles(g, this);
			}
		}

		internal override void drawAnchors(Graphics g)
		{
			base.drawAnchors(g);

			for (int r = CurrentRow; r < rowsCount; r++)
			{
				Row row = rowsList[r];
				AnchorPattern pattern = row.AnchorPattern;

				if (pattern == null)
					pattern = this.RowAnchorPattern;

				if (pattern != null)
				{
					int i = 0;
					foreach (AnchorPoint point in pattern.Points)
					{
						RectangleF rect = RectangleF.Empty;

						if (point.Column == -1)
							rect = getRowRect(r);
						else
							rect = getSpannedCellRect(r, point.Column);

						if (rect.Bottom > this.rect.Bottom)
							break;

						MarkDrawArgs args = null;
						if (point.MarkStyle == MarkStyle.Custom)
							args = new MarkDrawArgs(g, new PointF(0, 0),
								this, pattern, i);
						i++;
						point.draw(g, rect, 0, args,
							Constants.getMarkSize(fcParent.MeasureUnit));
					}
				}
			}
		}

		public HandlesStyle HandlesStyle
		{
			get	{ return selStyle; }

			set
			{
				if (value != selStyle)
				{
					selStyle = value;
					fcParent.invalidate(getRepaintRect(false));
					fcParent.setDirty();
				}
			}
		}

		private HandlesStyle selStyle;

		// ************ placement ************

		internal override bool containsPoint(PointF pt)
		{
			return Utilities.pointInRect(pt, rect);
		}

		public override bool HitTestHandle(PointF pt, ref int handle)
		{
			if (selStyle != HandlesStyle.Custom)
			{
				RectangleF rc = Utilities.normalizeRect(rect);

				if (ptInManipulator(pt)) return false;

				return Utilities.pointInHandle(pt,
					ref handle, rc, 0, enabledHandles & ~Handles.Rotate, captionHeight,
					selStyle, fcParent.SelHandleSize, fcParent.MeasureUnit);
			}
			else
			{
				// let the application do hit testing
				return fcParent.hitTestSelHandles(this, pt, ref handle);
			}
		}

		internal override int getHandleIdx(PointF pt)
		{
			int handle = -1;

			RectangleF rc = Utilities.normalizeRect(rect);
			if (Utilities.pointInHandle(pt, ref handle, rc, 0, Handles.All,
				0, selStyle, fcParent.SelHandleSize, fcParent.MeasureUnit))
				return handle;

			return -1;
		}

		internal override bool canHaveArrows(bool outgoing)
		{
			if (outgoing && !AllowOutgoingArrows)
				return false;
			if (!outgoing && !AllowIncomingArrows)
				return false;

			if (linkStyle == TableLinkStyle.Rows)
				return (RowCount > 0 && (fcParent.AllowUnanchoredArrows ||
					allowsRowAnchorDir(outgoing)));

			if (linkStyle == TableLinkStyle.Both && RowCount > 0)
				return fcParent.AllowUnanchoredArrows ||
					base.canHaveArrows(outgoing) || allowsRowAnchorDir(outgoing);

			return base.canHaveArrows(outgoing);
		}

		internal int rowFromPt(PointF pt)
		{
			if (linkStyle == TableLinkStyle.Table) return -1;

			if (lastAnchorForTable && linkStyle == TableLinkStyle.Both &&
				lastFoundAnchor == pt && anchorPattern != null)
				return -1;

			if (rowsList == null)
			{
				if (linkStyle == TableLinkStyle.Rows) return 0;
				return -1;
			}

			float h = Math.Min(rect.Top, rect.Bottom) + captionHeight;

			for (int i = CurrentRow; i < rowsList.Count; ++i)
			{
				if (isRowCollapsed(i))
					continue;

				if (pt.Y >= h && pt.Y < h + rowsList[i].Height)
					return i;
				h += rowsList[i].Height;
			}

			if (linkStyle == TableLinkStyle.Rows) return 0;
			return -1;
		}

		internal override Link createLink(Arrow arrow, PointF pt, bool incoming)
		{
			return new TableLink(this, arrow, incoming, rowFromPt(pt));
		}

		internal Link createLink(Arrow arrow, bool incm, int row)
		{
			return new TableLink(this, arrow, incm, row);
		}

		internal override bool alreadyLinked(Node node)
		{
			if (base.alreadyLinked(node)) return true;

			if (rowsList != null)
			{
				foreach (Row row in rowsList)
				{
					foreach (Arrow arrow in row.OutgoingArrows)
					{
						if (arrow.getDestLink().getNode() == node)
							return true;
					}
				}
			}

			return false;
		}

		public override ItemType getType()
		{
			return ItemType.Table;
		}

		internal override RectangleF getRepaintRect(bool includeConnected)
		{
			RectangleF repaintRect = base.getRepaintRect(includeConnected);

			if (includeConnected)
			{
				InvalidAreaUpdater updtr = new InvalidAreaUpdater(repaintRect, true);
				visitArrows(updtr);
				repaintRect = updtr.getInvalidRect();
			}

			return repaintRect;
		}

		private void calcColWidths()
		{
			RectangleF rcNorm = Utilities.normalizeRect(rect);
			float tblWidth = rcNorm.Right - rcNorm.Left;
			if (colsList == null) return;

			tblWidth -= 2*PenWidth;
			float autoWdth, auto = 0;
			float sum = 0;
			for (int c = 0; c < columnsCount; ++c)
			{
				if (((Column)colsList[c]).ColumnStyle == ColumnStyle.FixedWidth)
					sum += ((Column)colsList[c]).Width;
				else
					auto++;
			}

			if (auto > 0)
			{
				autoWdth = (tblWidth - sum) / auto;
				if (autoWdth <= 0) autoWdth = 1;
				for (int c = 0; c < columnsCount; ++c)
					if (((Column)colsList[c]).ColumnStyle == ColumnStyle.AutoWidth)
						((Column)colsList[c]).width = autoWdth;
			}
		}

		internal override void visitArrows(CollectionVisitor visitor)
		{
			base.visitArrows(visitor);

			if (rowsList != null)
			{
				for (int i = 0; i < rowsCount; ++i)
				{
					foreach(Arrow a in ((Row)rowsList[i]).OutgoingArrows)
						visitor.accept(a);
					foreach(Arrow a in ((Row)rowsList[i]).IncomingArrows)
						visitor.accept(a);
				}
			}
		}

		internal override void visitArrows(CollectionVisitor outgVstr, CollectionVisitor incmVstr)
		{
			base.visitArrows(outgVstr, incmVstr);

			if (rowsList != null)
			{
				for (int i = 0; i < rowsCount; ++i)
				{
					foreach(Arrow a in ((Row)rowsList[i]).OutgoingArrows)
						outgVstr.accept(a);
					foreach(Arrow a in ((Row)rowsList[i]).IncomingArrows)
						incmVstr.accept(a);
				}
			}
		}

		internal override void visitArrows(
			CollectionVisitor outgVstr,
			CollectionVisitor incmVstr,
			ArrowCollection excluded)
		{
			if (excluded == null)
			{
				visitArrows(outgVstr, incmVstr);
				return;
			}

			base.visitArrows(outgVstr, incmVstr, excluded);

			if (rowsList != null)
			{
				for (int i = 0; i < rowsCount; ++i)
				{
					foreach(Arrow a in ((Row)rowsList[i]).OutgoingArrows)
						if (!excluded.Contains(a))
							outgVstr.accept(a);
					foreach(Arrow a in ((Row)rowsList[i]).IncomingArrows)
						if (!excluded.Contains(a))
							incmVstr.accept(a);
				}
			}
		}


		/// <summary>
		/// Returns the number of the first header row before the
		/// specified row. If the specified row is a header row
		/// itself, returns 'row'. If there is no header row
		/// before this one, the return value is -1.
		/// </summary>
		private int getParentRow(int row)
		{
			if (row >= rowsCount)
				row = rowsCount - 1;

			if (row < 0)
				return -1;

			if (hasHeaderRows)
			{
				for (int i = row; i >= 0; i--)
				{
					if (rowsList[i].Header)
						return i;
				}
			}

			return -1;
		}

		/// <summary>
		/// Checks whether the specified row is collapsed.
		/// Collapsed are all rows whose header row is collapsed.
		/// </summary>
		internal bool isRowCollapsed(int row)
		{
			if (!hasHeaderRows)
				return false;

			bool collapsed = false;

			int parentRow = getParentRow(row);
			if (parentRow != -1 && parentRow != row)
				collapsed = !rowsList[parentRow].IsExpanded;

			return collapsed;
		}


		internal RectangleF getExpandedRowRect(int row)
		{
			RectangleF rc = Utilities.normalizeRect(rect);

			if (rowsList == null || row >= rowsCount) return rc;

			if (row < CurrentRow)
			{
				rc.Height = CaptionHeight;
				return rc;
			}

			float h = rc.Top + captionHeight;
			for (int i = CurrentRow; i < row; ++i)
			{
				if (isRowCollapsed(i))
					continue;

				h += ((Row)rowsList[i]).Height;
			}
			rc.Y = h;
			rc.Height = ((Row)rowsList[row]).Height;

			return rc;
		}

		internal RectangleF getRowRect(int row)
		{
			int parentRow = getParentRow(row);

			if (parentRow == -1 || parentRow == row)
				return getExpandedRowRect(row);

			if (rowsList[parentRow].IsExpanded)
				return getExpandedRowRect(row);

			return getRowRect(parentRow);
		}

		internal RectangleF getColumnRect(int col)
		{
			RectangleF rc = Utilities.normalizeRect(rect);

			if (colsList == null || col >= columnsCount) return rc;

			float w = rc.Left;
			if (hasHeaderRows)
				w += HeaderRowOffset;

			for (int i = 0; i < col; ++i)
				w += ((Column)colsList[i]).Width;
			rc.X = w;
			rc.Width = ((Column)colsList[col]).Width;

			return rc;
		}

		private RectangleF getCellRect(int row, int col)
		{
			RectangleF rcCol = getColumnRect(col);
			RectangleF rcRow = getRowRect(row);

			RectangleF rcCell = new RectangleF(
				rcCol.Left, rcRow.Top, rcCol.Width, rcRow.Height);

			RectangleF rc = Utilities.normalizeRect(rect);
			if (rcCell.Right >= rc.Right || col == columnsCount - 1)
				rcCell.Width = rc.Right - rcCell.Left;

			return rcCell;
		}

		internal RectangleF getSpannedCellRect(Cell cell)
		{
			int i = 0;
			for (i = 0; i < cells.Count; ++i)
				if (cells[i] == cell) break;

			int row = i / colsList.Count;
			int col = i % colsList.Count;

			return getSpannedCellRect(row, col);
		}

		internal RectangleF getSpannedCellRect(int row, int col)
		{
			return getSpannedCellRect(row, col, false, ref dummyImgRect);
		}

		static RectangleF dummyImgRect = RectangleF.Empty;

		internal RectangleF getSpannedCellRect(int row, int col,
			bool getImgRect, ref RectangleF imgRect)
		{
			if (cells == null)
				return RectangleF.Empty;

			Cell cell = this[col, row];

			if (cell.RowSpan == 1 && cell.ColumnSpan == 1)
			{
				RectangleF cellRect = getCellRect(row, col);
				if (getImgRect)
				{
					imgRect = cellRect;
					imgRect.Width = colsList[col].Width;
				}
				return cellRect;
			}

			RectangleF rc = Utilities.normalizeRect(rect);
			if (hasHeaderRows)
			{
				rc.X += HeaderRowOffset;
				rc.Width -= HeaderRowOffset;
			}

			float h = rc.Top + captionHeight;

			// Calc y and height
			float y = 0;
			float height = 0;

			int rowFrom = row;
			if (isRowCollapsed(rowFrom))
				rowFrom = getParentRow(rowFrom);

			int topRow = CurrentRow;
			if (rowFrom < topRow)
			{
				float aboveHeight = 0;
				for (int i = rowFrom; i < topRow; i++)
				{
					if (isRowCollapsed(i))
						continue;

					aboveHeight += (rowsList[i] as Row).Height;
				}

				y = h - aboveHeight;
			}
			else
			{
				float belowHeight = 0;
				for (int i = topRow; i < rowFrom; i++)
				{
					if (isRowCollapsed(i))
						continue;

					belowHeight += (rowsList[i] as Row).Height;
				}

				y = h + belowHeight;
			}

			int rowSpan = cell.RowSpan;
			if (row + rowSpan > rowsCount)
				rowSpan = RowCount - row;

			int rowTo = rowFrom + rowSpan - 1;
			for (int i = rowFrom; i <= rowTo; i++)
			{
				if (isRowCollapsed(i))
					continue;

				height += rowsList[i].Height;
			}

			// Calculate x and width
			int colSpan = cell.ColumnSpan;
			if (col + colSpan > columnsCount)
				colSpan = columnsCount - col;

			RectangleF rcColFrom = getColumnRect(col);
			RectangleF rcColTo = getColumnRect(col + colSpan - 1);

			float x = rcColFrom.Left;
			float width = rcColTo.Right - rcColFrom.Left;
			float imgWidth = width;
			if (x + width > rc.Right)
				width = rc.Right - x;

			if (col + colSpan - 1 == columnsCount - 1)
				width = rc.Right - x;

			RectangleF res = new RectangleF(x, y, width, height);
			if (getImgRect)
			{
				imgRect = res;
				imgRect.Width = imgWidth;
			}

			return res;
		}

		internal override Region getRegion()
		{
			RectangleF rc = Utilities.normalizeRect(rect);

			if (style == TableStyle.RoundedRectangle)
				return Utilities.createRoundRectRgn(rc, Constants.getRoundRectArc(fcParent.MeasureUnit, fcParent.RoundRectFactor));

			return new Region(rc);
		}

		private Region getRgnInRect(RectangleF rc)
		{
			switch (style)
			{
				case TableStyle.Rectangle:
					return new Region(rc);
				case TableStyle.RoundedRectangle:
					return Utilities.createRoundRectRgn(rc, Constants.getRoundRectArc(fcParent.MeasureUnit, fcParent.RoundRectFactor));
			}

			return null;
		}

		internal override PointF getIntersection(PointF ptOrg, PointF ptEnd)
		{
			if (ptOrg == ptEnd) return ptOrg;

			PointF pt = getCenter();
			if (rect.Width == 0 || rect.Height == 0)
				return pt;

			RectangleF rcTable = Utilities.normalizeRect(rect);

			if (Utilities.pointInRect(ptOrg, rcTable) &&
				Utilities.pointInRect(ptEnd, rcTable)) return pt;

			if (style == TableStyle.Rectangle)
			{
				Utilities.getRectIntr(rcTable, ptOrg, ptEnd, ref pt);
			}
			else
			{
				GraphicsPath path = Utilities.getRoundRect(
					rcTable.X, rcTable.Y, rcTable.Width, rcTable.Height,
					Constants.getRoundRectArc(fcParent.MeasureUnit, fcParent.RoundRectFactor));
				path.Flatten();
				Utilities.getPathIntersection(path, ptOrg, ptEnd, ref pt);
				path.Dispose();
			}

			return pt;
		}

		internal PointF getRowIntersection(int nRow, PointF org, PointF end)
		{
			RectangleF rc = Utilities.normalizeRect(rect);

			PointF pt = getRowCenter(nRow);
			if (org.X > end.X)
				pt.X = rc.Left;
			else
				pt.X = rc.Right;
			return pt;
		}

		internal PointF getRowCenter(int row)
		{
			RectangleF rc = getRowRect(row);
			PointF pt = new PointF( (rc.Left + rc.Right) / 2, (rc.Top + rc.Bottom) / 2 );

			RectangleF rcTable = Utilities.normalizeRect(rect);
			if (pt.Y >= rcTable.Bottom)
				pt.Y = rcTable.Bottom - 1;
			return pt;
		}

		internal void addIncomingArrow(int row, Arrow arrow)
		{
			if (rowsList == null || row >= rowsCount) return;

			ArrowCollection arrows = ((Row)rowsList[row]).IncomingArrows;
			if (!arrows.Contains(arrow))
				arrows.Add(arrow);
		}

		internal void addOutgoingArrow(int row, Arrow arrow)
		{
			if (rowsList == null || row >= rowsCount) return;

			ArrowCollection arrows = ((Row)rowsList[row]).OutgoingArrows;
			if (!arrows.Contains(arrow))
				arrows.Add(arrow);
		}

		internal void removeIncomingArrow(int row, Arrow arrow)
		{
			if (rowsList == null || row >= rowsCount) return;

			foreach(Arrow a in ((Row)rowsList[row]).IncomingArrows)
				if(a == arrow)
				{
					((Row)rowsList[row]).IncomingArrows.Remove(a);
					return;
				}
		}

		internal void removeOutgoingArrow(int row, Arrow arrow)
		{
			if (rowsList == null || row >= rowsCount) return;

			foreach(Arrow a in ((Row)rowsList[row]).OutgoingArrows)
				if(a == arrow)
				{
					((Row)rowsList[row]).OutgoingArrows.Remove(a);
					return;
				}
		}


		internal override void visitHierarchy(CollectionVisitor visitor)
		{
			base.visitHierarchy(visitor);
			
			if (rowsList != null)
			{
				for (int i = 0; i < rowsCount; ++i)
				{
					foreach(Arrow a in ((Row)rowsList[i]).OutgoingArrows)
						a.visitHierarchy(visitor);
					foreach(Arrow a in ((Row)rowsList[i]).IncomingArrows)
						a.visitHierarchy(visitor);
				}
			}
		}

		private void removeRelation(int row, Relationship rel, Table table)
		{
			ArrowCollection arrows = null;
			TableCollection	tables = null;

			switch (rel)
			{
			case Relationship.OneToMany:
				arrows = IncomingArrows;
				if (row != -1)
				{
					arrows = ((Row)rowsList[row]).IncomingArrows;
					tables = ((Row)rowsList[row]).RelatedOneToMany;
				}
				break;
			case Relationship.ManyToOne:
				arrows = OutgoingArrows;
				if (row != -1)
				{
					arrows = ((Row)rowsList[row]).OutgoingArrows;
					tables = ((Row)rowsList[row]).RelatedManyToOne;
				}
				break;
			}
			if (tables.Count == 0 && row != -1) return;

			Link link = null;
			ChartObject obj;
			foreach(Arrow a in arrows)
			{
				switch (rel)
				{
					case Relationship.OneToMany:
						link = a.getOrgnLink();
						break;
					case Relationship.ManyToOne:
						link = a.getDestLink();
						break;
				}
				obj = link.getNode();
				if (obj == table)
				{
					fcParent.DeleteObject(a);
					return;
				}
			}
		}

		internal void updateLinksIndices()
		{
			if (rowsList == null) return;
			for (int r = 0; r < rowsCount; ++r)
			{
				foreach(Arrow a in ((Row)rowsList[r]).IncomingArrows)
					((TableLink)a.getDestLink()).updateRow(r);
				foreach(Arrow a in ((Row)rowsList[r]).OutgoingArrows)
					((TableLink)a.getOrgnLink()).updateRow(r);
			}
		}

		private void updateArrowsPos(int fr)
		{
			if (rowsList == null) return;
			for (int r = fr; r < rowsCount; ++r)
			{
				foreach (Arrow a in ((Row)rowsList[r]).IncomingArrows)
				{
					a.updatePosFromOrgAndDest(false);
					a.doRoute();
				}
				foreach (Arrow a in ((Row)rowsList[r]).OutgoingArrows)
				{
					a.updatePosFromOrgAndDest(false);
					a.doRoute();
				}
			}
		}

		internal bool cellFromPt(PointF pt, ref int row, ref int col)
		{
			if (rowsList == null || colsList == null) return false;

			int tempRow = 0, tempCol = 0;
			if (!nonSpannedCellFromPt(pt, ref tempRow, ref tempCol))
				return false;

			if (hasSpanningCells && getCoveredCells()[tempCol,tempRow])
			{
				// Check if there is a spanned cell over this one
				for (int r = tempRow; r >= 0; r--)
				{
					for (int c = tempCol; c >= 0; c--)
					{
						if (c == tempCol && r == tempRow)
							continue;

						Cell cell = this[c, r];

						if (cell.RowSpan + r > tempRow && cell.ColumnSpan + c > tempCol)
						{
							row = r;
							col = c;

							return true;
						}
					}
				}
			}

			row = tempRow;
			col = tempCol;

			return true;
		}

		internal bool nonSpannedCellFromPt(PointF pt, ref int row, ref int col)
		{
			if (rowsList == null || colsList == null) return false;

			float h = Math.Min(rect.Top, rect.Bottom) + captionHeight;
			float w = Math.Min(rect.Left, rect.Right);

			if (hasHeaderRows)
				w += HeaderRowOffset;

			if (pt.Y < h) return false;

			row = 0;
			col = columnsCount - 1;

			Row r;
			int i;
			for (i = CurrentRow; i < rowsCount; ++i)
			{
				if (isRowCollapsed(i))
					continue;

				r = (Row)rowsList[i];
				if (pt.Y >= h && pt.Y < h + r.Height)
				{
					row = i;
					break;
				}

				h += r.Height;
			}
			if (i == rowsCount) return false;

			Column c;
			for (i = 0; i < columnsCount; ++i)
			{
				c = (Column)colsList[i];
				if (i < columnsCount - 1)
				{
					if (pt.X >= w && pt.X < w + c.Width)
					{
						col = i;
						break;
					}
				}
				else
				{
					RectangleF rc = Utilities.normalizeRect(rect);
					if (pt.X >= w && pt.X < rc.Right)
					{
						col = i;
						break;
					}
				}
				w += c.Width;
			}
			if (i == columnsCount) return false;

			return true;
		}


		internal bool canScrollUp()
		{
			if (!hasHeaderRows)
			{
				if (currScrollRow > 0)
					return true;
			}
			else
			{
				for (int i = currScrollRow - 1; i >= 0; i--)
					if (!isRowCollapsed(i))
						return true;
			}

			return false;
		}

		internal bool canScrollDown()
		{
			if (!hasHeaderRows)
			{
				if (currScrollRow < rowsCount - 1)
					return true;
			}
			else
			{
				for (int i = currScrollRow + 1; i < rowsCount; i++)
					if (!isRowCollapsed(i))
						return true;
			}

			return false;
		}

		internal void scrollDown()
		{
			if (!hasHeaderRows)
			{
				setCurrScrollRow(currScrollRow + 1);
			}
			else
			{
				int nextRow = currScrollRow + 1;

				while (isRowCollapsed(nextRow))
				{
					nextRow++;
					if (nextRow >= rowsCount)
						return;
				}

				setCurrScrollRow(nextRow);
			}

			fcParent.setDirty();
			fcParent.invalidate(getRepaintRect(true));
		}

		internal void scrollUp()
		{
			if (!hasHeaderRows)
			{
				setCurrScrollRow(currScrollRow - 1);
			}
			else
			{
				int prevRow = currScrollRow - 1;

				while (isRowCollapsed(prevRow))
				{
					prevRow--;
					if (prevRow < 0)
						return;
				}

				setCurrScrollRow(prevRow);
			}

			fcParent.setDirty();
			fcParent.invalidate(getRepaintRect(true));
		}

		/// <summary>
		/// Defines the placement of the box picture
		/// </summary>
		public ImageAlign PicturePos
		{
			get
			{
				return picturePos;
			}
			set
			{
				if (picturePos != value)
				{
					picturePos = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private ImageAlign picturePos;

		public override int getClassId()
		{
			return 4;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			ctx.saveReference(this, cells, 1);
			ctx.saveReference(this, rowsList, 2);
			ctx.saveReference(this, colsList, 3);
			writer.Write(currScrollRow);
			writer.Write(scrollable);
			ctx.saveColor(fillColor);
			ctx.saveColor(frameColor);
			writer.Write(caption);
			ctx.saveColor(captionColor);
			ctx.saveStringFormat(captionFormat);

			writer.Write((double)captionHeight);
			writer.Write((int)cellBorders);
			writer.Write(rowsCount);
			writer.Write((double)rowHeight);
			writer.Write(columnsCount);
			writer.Write((double)columnWidth);
			ctx.saveColor(textColor);
			writer.Write((int)picturePos);

			ctx.saveImage(picture);

			ctx.saveObject(pen);
			ctx.saveReference(this, brush, 4);

			writer.Write((int)selStyle);

			// new in save format 6
			ctx.saveAnchorPattern(rowAnchorPattern);
			writer.Write((long)enabledHandles);
			writer.Write((int)Style);

			// new in file format 8
			writer.Write((int)linkStyle);

			// new in file format 9
			ctx.saveAnchorPattern(anchorPattern);

			ctx.saveReference(this, incomingArrows, 5);
			ctx.saveReference(this, outgoingArrows, 6);

			// new in save format 14
			writer.Write(Expanded);
			writer.Write(Expandable);
			writer.Write(useStyledText);

			// new in save format 19
			writer.Write((int)customDraw);
			writer.Write((int)cellCustomDraw);

			// new in save format 22
			writer.Write(offsetHeaderRows);

			// new in save format 25
			ctx.saveReference(this, captionBackBrush, 7);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);
			hasHeaderRows = false;

			hasSpanningCells = false;
			ctx.loadReference(this);	// load cells
			ctx.loadReference(this);	// load rows
			ctx.loadReference(this);	// load columns
			currScrollRow = reader.ReadInt32();
			scrollable = reader.ReadBoolean();
			setScrollable(scrollable);
			fillColor = ctx.loadColor();
			frameColor = ctx.loadColor();
			caption = reader.ReadString();
			captionColor = ctx.loadColor();
			captionFormat = ctx.loadStringFormat();

			captionHeight = (float)reader.ReadDouble();
			cellBorders = (CellFrameStyle)reader.ReadInt32();
			rowsCount = reader.ReadInt32();
			rowHeight = (float)reader.ReadDouble();
			columnsCount = reader.ReadInt32();
			columnWidth = (float)reader.ReadDouble();
			textColor = ctx.loadColor();
			picturePos = (ImageAlign)reader.ReadInt32();

			picture = ctx.loadImage();

			pen.Width = PenWidth;
			pen.Color = frameColor;
			pen.DashStyle = PenDashStyle;
			brush.Release();
			brush = new SolidBrush(fillColor);
			brush.AddRef();

			if (ctx.FileVersion > 3)
			{
				pen = (Pen)ctx.loadObject();
				ctx.loadReference(this);

				selStyle = (HandlesStyle)reader.ReadInt32();

				if (ctx.FileVersion > 5)
				{
					// new in save format 6
					rowAnchorPattern = ctx.loadAnchorPattern();
					enabledHandles = (Handles)reader.ReadInt64();
					style = (TableStyle)reader.ReadInt32();

					if (ctx.FileVersion > 7)
					{
						// new in file format 8
						linkStyle = (TableLinkStyle)reader.ReadInt32();

						if (ctx.FileVersion > 8)
						{
							// new in save format 9
							anchorPattern = ctx.loadAnchorPattern();

							// Load table arrows
							ctx.loadReference(this);
							ctx.loadReference(this);

							if (ctx.FileVersion > 13)
							{
								// new in save format 14
								setExpanded(reader.ReadBoolean());
								setExpandable(reader.ReadBoolean());
								useStyledText = reader.ReadBoolean();

								if (ctx.FileVersion > 18)
								{
									// new in save format 19
									customDraw = (CustomDraw)reader.ReadInt32();
									cellCustomDraw = (CustomDraw)reader.ReadInt32();

									if (ctx.FileVersion > 21)
									{
										// new in save format 22
										offsetHeaderRows = reader.ReadBoolean();

										if (ctx.FileVersion > 24)
										{
											// new in save format 25
											ctx.loadReference(this);	// captionBackBrush
										}
									}
								}
							}
						}
					}
				}
			}

			resetCoveredCells();
			updateText();
			layoutText();
		}

		public override void setReference(int refId, IPersists obj)
		{
			base.setReference(refId, obj);

			switch (refId)
			{
			case 1:
				cells = (CellCollection)obj;
				break;
			case 2:
				rowsList = (RowCollection)obj;
				break;
			case 3:
				colsList = (ColumnCollection)obj;
				break;
			case 4:
				brush.Release();
				brush = (Brush)obj;
				brush.AddRef();
				break;
			case 5:
				incomingArrows = (ArrowCollection)obj;
				break;
			case 6:
				outgoingArrows = (ArrowCollection)obj;
				break;
			case 7:
				if (captionBackBrush != null)
					captionBackBrush.Release();
				captionBackBrush = (Brush)obj;
				if (captionBackBrush != null)
					captionBackBrush.AddRef();
				break;
			}
		}

		internal override void onLoad()
		{
			if (cells != null)
			{
				foreach (Cell cell in cells)
					if (cell.ColumnSpan > 1 || cell.RowSpan > 1)
						hasSpanningCells = true;
			}
			resetCoveredCells();

			if (rowsList != null)
			{
				foreach (Row row in rowsList)
					if (row.Header)
						hasHeaderRows = true;
			}

			// Update cell text
			updateCellText();
			layoutCellText();
		}

		internal override string getTextToEdit()
		{
			return Caption;
		}


		internal override void setEditedText(string newText)
		{
			Caption = newText;
		}


		internal override RectangleF getEditRect()
		{
			RectangleF cptRect = BoundingRect;
			cptRect.Height = CaptionHeight;
			return cptRect;
		}


		internal override void getAllOutgoingArrows(ArrowCollection ac)
		{
			base.getAllOutgoingArrows(ac);

			if (rowsList == null) return;
			foreach (Row r in rowsList)
				foreach (Arrow a in r.OutgoingArrows)
				{
					if (!ac.Contains(a))
						ac.Add(a);
				}
		}

		internal override void getAllIncomingArrows(ArrowCollection ac)
		{
			base.getAllIncomingArrows(ac);

			if (rowsList == null) return;
			foreach (Row r in rowsList)
				foreach (Arrow a in r.IncomingArrows)
				{
					if (!ac.Contains(a))
						ac.Add(a);
				}
		}

		public TableStyle Style
		{
			get
			{
				return style;
			}
			set
			{
				if (style != value)
				{
					style = value;
					fcParent.invalidate(getRepaintRect(false));
					fcParent.setDirty();
				}
			}
		}

		private TableStyle style;

		public AnchorPattern RowAnchorPattern
		{
			get { return rowAnchorPattern; }
			set
			{
				if (value == null || value.isRegistered)
					rowAnchorPattern = value;
				else
					rowAnchorPattern = (AnchorPattern)value.Clone();
			}
		}

		private AnchorPattern rowAnchorPattern;

		internal AnchorPattern getRowAnchorPattern(int row)
		{
			AnchorPattern ap = null;
			if (row >= 0 && row < RowCount)
				ap = Rows[row].AnchorPattern;
			if (ap == null) ap = rowAnchorPattern;
			return ap;
		}

		private bool allowsRowAnchorDir(bool outgoing)
		{
			for (int row = 0; row < rowsCount; ++row)
			{
				AnchorPattern ap = Rows[row].AnchorPattern;
				if (ap != null && ap.allowArrowsDir(outgoing, this))
					return true;
			}
			
			if (rowAnchorPattern != null &&
				rowAnchorPattern.allowArrowsDir(outgoing, this))
				return true;

			return false;
		}

		// invoked when an arrow is drawn from / to a table.
		// finds the anchor point nearest to arrow's end point.
		// tables need special handling to combine row's and table's anchor patterns
		internal override PointF getNearestAnchor(PointF pt,
			Arrow arrow, bool incoming, ref int anchorIdx)
		{
			PointF nearestPt = pt;
			anchorIdx = -1;

			// check if at that position, an arrow would link to a row
			lastAnchorForTable = false;
			int row = rowFromPt(pt);
			if(row == -1)
			{
				// it seems not, so use Table's anchor pattern, not Row's one
				PointF result = base.getNearestAnchor(pt, arrow, incoming, ref anchorIdx);
				lastAnchorForTable = true;
				lastFoundAnchor = result;
				return result;
			}

			// link to a row, get the relevant anchor pattern for this row
			AnchorPattern rowap = getRowAnchorPattern(row);
			bool foundAp = false;

			// get row extents
			RectangleF rowRect = getRowRect(row);
			float nearestDist = 100 * Math.Max(rowRect.Width, rowRect.Height);

			if (rowap != null)
			{			
				// look for appropriate anchor points, in or out
				for (int i = 0; i < rowap.Points.Count; i++)
				{
					AnchorPoint ap = rowap.Points[i];
					if (incoming && !ap.AllowIncoming) continue;
					if (!incoming && !ap.AllowOutgoing) continue;
					if (!fcParent.validateAnchor(arrow, !incoming, this, i)) continue;
					foundAp = true;

					// how close an anchor point is to the point passed as parameter
					RectangleF testRect =
						ap.Column == -1 ? rowRect : getSpannedCellRect(row, ap.Column);
					PointF pos = ap.getPos(testRect);
					float dx = pos.X - pt.X;
					float dy = pos.Y - pt.Y;
					float ptDist = (float)Math.Sqrt(dx*dx + dy*dy);
					if (ptDist < nearestDist)
					{
						// dis closer than the others
						nearestDist = ptDist;
						nearestPt = pos;
						anchorIdx = i;
					}
				}

				if (foundAp) return nearestPt;
			}

			// haven't found suitable anchor point for this row
			if (anchorPattern != null && linkStyle != TableLinkStyle.Rows)
			{
				PointF result = base.getNearestAnchor(pt, arrow, incoming, ref anchorIdx);
				lastAnchorForTable = true;
				lastFoundAnchor = result;
				return result;
			}

			if (!foundAp)
			{
				nearestDist = float.MaxValue;

				for (int r = 0; r < RowCount; ++r)
				{
					if (r == row) continue;
					rowap = getRowAnchorPattern(r);
					if (rowap == null) continue;

					// get row extents
					rowRect = getRowRect(r);

					// look for anchor points
					for (int i = 0; i < rowap.Points.Count; i++)
					{
						AnchorPoint ap = rowap.Points[i];
						if (incoming && !ap.AllowIncoming) continue;
						if (!incoming && !ap.AllowOutgoing) continue;
						if (!fcParent.validateAnchor(arrow, !incoming, this, i)) continue;

						RectangleF testRect =
							ap.Column == -1 ? rowRect : getSpannedCellRect(r, ap.Column);
						PointF pos = ap.getPos(testRect);
						float dx = pos.X - pt.X;
						float dy = pos.Y - pt.Y;
						float ptDist = (float)Math.Sqrt(dx*dx + dy*dy);
						if (ptDist < nearestDist)
						{
							nearestDist = ptDist;
							nearestPt = pos;
							anchorIdx = i;
						}
					}

				}
			}

			return nearestPt;
		}

		internal PointF getNearestAnchor(int row,
			PointF pt, Arrow arrow, bool incoming, ref int anchorIdx)
		{
			lastAnchorForTable = false;
			PointF nearestPt = pt;
			anchorIdx = -1;

			if (row == -1)
				return base.getNearestAnchor(pt, arrow, incoming, ref anchorIdx);

			// link to a row, get the relevant anchor pattern for this row
			AnchorPattern rowap = getRowAnchorPattern(row);

			// get row extents
			RectangleF rowRect = getRowRect(row);
			float nearestDist = 100 * Math.Max(rowRect.Width, rowRect.Height);

			if (rowap != null)
			{			
				// look for appropriate anchor points, in or out
				for (int i = 0; i < rowap.Points.Count; i++)
				{
					AnchorPoint ap = rowap.Points[i];
					if (incoming && !ap.AllowIncoming) continue;
					if (!incoming && !ap.AllowOutgoing) continue;
					if (!fcParent.validateAnchor(arrow, !incoming, this, i)) continue;

					// how close an anchor point is to the point passed as parameter
					RectangleF testRect =
						ap.Column == -1 ? rowRect : getSpannedCellRect(row, ap.Column);
					PointF pos = ap.getPos(testRect);
					float dx = pos.X - pt.X;
					float dy = pos.Y - pt.Y;
					float ptDist = (float)Math.Sqrt(dx*dx + dy*dy);
					if (ptDist < nearestDist)
					{
						// dis closer than the others
						nearestDist = ptDist;
						nearestPt = pos;
						anchorIdx = i;
					}
				}
			}

			return nearestPt;
		}


		private bool lastAnchorForTable;
		private PointF lastFoundAnchor;

		internal override bool canModify(int handle)
		{
			return (((long)enabledHandles) & (1 << handle)) != 0;
		}

		internal override string getToolTip(PointF point)
		{
			int r = 0, c = 0;
			Cell cell;
			if (cellFromPt(point, ref r, ref c))
			{
				cell = this[c, r];
				if (cell.ToolTip != "")
					return cell.ToolTip;
			}

			return base.getToolTip(point);
		}

		private TableLinkStyle linkStyle;
		public TableLinkStyle LinkStyle
		{
			get
			{
				return linkStyle;
			}
			set
			{
				if (linkStyle != value)
				{
					linkStyle = value;

					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(true));
				}
			}
		}

		/// <summary>
		/// Specifies what kind of custom painting to perform on tables.
		/// </summary>
		public CustomDraw CustomDraw
		{
			get
			{
				return customDraw;
			}
			set
			{
				if (customDraw != value)
				{
					customDraw = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private CustomDraw customDraw;

		/// <summary>
		/// Specifies what kind of custom painting to perform on table cells.
		/// </summary>
		public CustomDraw CellCustomDraw
		{
			get
			{
				return cellCustomDraw;
			}
			set
			{
				if (cellCustomDraw != value)
				{
					cellCustomDraw = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private CustomDraw cellCustomDraw;

		#region undo and redo

		internal override ItemProperties createProperties()
		{
			return new TableProperties();
		}

		internal override void saveProperties(ItemProperties props)
		{
			base.saveProperties(props);

			TableProperties tprops = (TableProperties)props;

			tprops.caption = caption;
			tprops.captionColor = captionColor;
			tprops.captionFormat = captionFormat;
			tprops.captionHeight = captionHeight;
			tprops.cellBorders = cellBorders;
			tprops.columnWidth = columnWidth;
			tprops.currScrollRow = currScrollRow;
			tprops.linkStyle = linkStyle;
			tprops.picture = picture;
			tprops.picturePos = picturePos;
			tprops.rowHeight = rowHeight;
			tprops.scrollable = scrollable;
			tprops.selStyle = selStyle;
			tprops.style = style;
			tprops.textColor = textColor;
			tprops.enableStyledText = EnableStyledText;
			tprops.customDraw = customDraw;
			tprops.cellCustomDraw = cellCustomDraw;

			if (tprops.captionBackBrush != null)
				tprops.captionBackBrush.Release();
			tprops.captionBackBrush = captionBackBrush;
			if (tprops.captionBackBrush != null)
				tprops.captionBackBrush.AddRef();
		}

		internal override void restoreProperties(ItemProperties props)
		{
			TableProperties tprops = (TableProperties)props;

			caption = tprops.caption;
			captionColor = tprops.captionColor;
			captionFormat = tprops.captionFormat;
			captionHeight = tprops.captionHeight;
			cellBorders = tprops.cellBorders;
			columnWidth = tprops.columnWidth;
			currScrollRow = tprops.currScrollRow;
			linkStyle = tprops.linkStyle;
			picture = tprops.picture;
			picturePos = tprops.picturePos;
			rowHeight = tprops.rowHeight;
			scrollable = tprops.scrollable;
			selStyle = tprops.selStyle;
			style = tprops.style;
			textColor = tprops.textColor;
			EnableStyledText = tprops.enableStyledText;
			customDraw = tprops.customDraw;
			cellCustomDraw = tprops.cellCustomDraw;

			if (captionBackBrush != tprops.captionBackBrush)
			{
				if (captionBackBrush != null)
					captionBackBrush.Release();
				captionBackBrush = tprops.captionBackBrush;
				if (captionBackBrush != null)
					captionBackBrush.AddRef();
			}

			base.restoreProperties(props);
		}

		internal void saveStructure(TableStructure str)
		{
			str.rows = rowsList.Clone();
			str.cols = colsList.Clone();
			str.cells = cells.Clone();

			str.rowsCount = rowsCount;
			str.columnsCount = columnsCount;

			str.hasHeaderRows = hasHeaderRows;
		}

		internal void restoreStructure(TableStructure str)
		{
			resetCoveredCells();

			cells = str.cells.Clone();
			colsList = str.cols.Clone();
			rowsList = str.rows.Clone();

			rowsCount = str.rowsCount;
			columnsCount = str.columnsCount;

			hasHeaderRows = str.hasHeaderRows;

			layoutText();
			layoutCellText();
		}

		#endregion // undo and redo

		private float HeaderRowOffset
		{
			get
			{
				if (!offsetHeaderRows)
					return 0;

				return HeaderRowIconWidth;
			}
		}

		private float HeaderRowIconWidth
		{
			get
			{
				return Constants.getTableHeaderOffset(fcParent.MeasureUnit);
			}
		}

		/// <summary>
		/// Gets the current scroll row.
		/// If the row is hidden though, returns the next visible row.
		/// </summary>
		internal int CurrentRow
		{
			get
			{
				if (!isRowCollapsed(currScrollRow))
					return currScrollRow;

				return getParentRow(currScrollRow);
			}
		}


		public bool OffsetHeaderRows
		{
			get { return offsetHeaderRows; }
			set
			{
				if (offsetHeaderRows == value)
					return;

				offsetHeaderRows = value;

				fcParent.setDirty();
				fcParent.invalidate(getRepaintRect(false));
			}
		}


		private bool offsetHeaderRows;


		private static Image imageExpand = null;
		private static Image imageCollapse = null;

		internal bool[,] getCoveredCells()
		{
			if (coveredCells != null)
			{
				if (coveredCells.GetLength(0) != columnsCount ||
					coveredCells.GetLength(1) != rowsCount)
					coveredCells = null;
			}

			if (coveredCells == null)
			{
				coveredCells = new bool[columnsCount, rowsCount];
				maxRowSpan = 1;

				// Initialize the cell span cover map
				for (int i = 0; i < rowsCount; i++)
				{
					for (int j = 0; j < columnsCount; j++)
						coveredCells[j, i] = false;
				}

				for (int i = 0; i < rowsCount; i++)
				{
					for (int j = 0; j < columnsCount; j++)
					{
						Cell cell = this[j, i];
						if (cell == null) continue;

						if (cell.RowSpan > maxRowSpan)
							maxRowSpan = cell.RowSpan;

						int rowTo = i + cell.RowSpan - 1;
						if (rowTo >= rowsCount)
							rowTo = rowsCount - 1;

						int colTo = j + cell.ColumnSpan - 1;
						if (colTo >= columnsCount)
							colTo = columnsCount - 1;

						for (int k = i; k <= rowTo; k++)
						{
							for (int l = j; l <= colTo; l++)
							{
								if (k == i && l == j)
									continue;

								Cell cell2 = this[l, k];

								if (cell2.RowSpan != 1 || cell2.ColumnSpan != 1)
									continue;

								coveredCells[l,k] = true;
							}
						}
					}
				}
			}

			return coveredCells;
		}

		internal void resetCoveredCells()
		{
			coveredCells = null;
		}

		internal void setHasSpanningCells()
		{
			hasSpanningCells = true;
		}

		internal bool getHasSpanningCells()
		{
			return hasSpanningCells;
		}

		internal int getMaxRowSpan()
		{
			return maxRowSpan;
		}

		// keeps track on which cells are covered by other spanning cells
		private bool hasSpanningCells;
		private bool[,] coveredCells;
		private int maxRowSpan;
		private bool hasHeaderRows;

		private MindFusion.FlowChartX.Brush captionBackBrush;

		/// <summary>
		/// A brush used to fill the caption area.
		/// </summary>
		[Editor(typeof(MindFusion.FlowChartX.Design.BrushEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Brush CaptionBackBrush
		{
			get
			{
				return captionBackBrush;
			}
			set
			{
				if (captionBackBrush != null)
					captionBackBrush.Release();
				captionBackBrush = value;
				if (captionBackBrush != null)
					captionBackBrush.AddRef();

				fcParent.setDirty();
				fcParent.invalidate(getRepaintRect(false));
			}
		}

		internal void checkForHeaderRows()
		{
			hasHeaderRows = false;

			for (int r = 0; r < rowsCount; ++r)
			{
				if (rowsList[r].Header)
				{
					hasHeaderRows = true;
					return;
				}
			}
		}
	}
}