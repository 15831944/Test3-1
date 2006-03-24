// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Reflection;
using MindFusion.FlowChartX;
using MindFusion.FlowChartX.Commands;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Generalized alignment enum.
	/// </summary>
	public enum Alignment
	{
		Near,
		Far
	}

	/// <summary>
	/// Ruler unit enum.
	/// </summary>
	public enum RulerUnit
	{
		Point,
		Pixel,
		Millimeter,
		Inch,
		Percent
	}

	/// <summary>
	/// The powerful ruler control.
	/// </summary>
	[Designer(typeof(MindFusion.FlowChartX.Design.RulerDesigner))]
	[DefaultProperty("Document")]
	public class Ruler : System.Windows.Forms.Control
	{
		/// <summary>
		/// Represents either horizontal or vertical scale in a ruler control.
		/// </summary>
		private sealed new class Scale : System.Windows.Forms.Control
		{
			/// <summary>
			/// Scales are instantiated only by the ruler.
			/// </summary>
			public Scale(Ruler parent, Orientation orientation)
			{
				SetStyle(ControlStyles.AllPaintingInWmPaint, true);
				SetStyle(ControlStyles.DoubleBuffer, true);
				SetStyle(ControlStyles.UserPaint, true);
				SetStyle(ControlStyles.ResizeRedraw, true);

				_parent = parent;
				_orientation = orientation;
				_alignment = Alignment.Near;

				_unit = RulerUnit.Millimeter;
				_projectionColor = Color.LightSteelBlue;
				_enableGuides = true;
				_guideColor = SystemColors.ControlText;
				_cursorColor = Color.Red;

				_aligning = false;
				_alignedNodes = new Hashtable();

				_speeder = new StringFormat();
				_speeder.SetMeasurableCharacterRanges(new CharacterRange[]
					{
						new CharacterRange(0, 5)
					});
			}


			/// <summary>
			/// Gets the parent ruler this scale belongs to.
			/// </summary>
			public Ruler Ruler
			{
				get { return _parent; }
			}

			/// <summary>
			/// Gets the orientation of the scale.
			/// </summary>
			public Orientation Orientation
			{
				get { return _orientation; }
			}

			/// <summary>
			/// Gets or sets the alignment of the scale.
			/// </summary>
			public Alignment Alignment
			{
				get { return _alignment; }
				set
				{
					if (_alignment == value)
						return;

					_alignment = value;
				}
			}

			/// <summary>
			/// Gets or sets the measure unit used by the scale.
			/// </summary>
			public RulerUnit Unit
			{
				get { return _unit; }
				set
				{
					if (_unit == value)
						return;

					_unit = value;
				}
			}

			/// <summary>
			/// Gets or sets the color used to paint
			/// the projection of the active node.
			/// </summary>
			public Color ProjectionColor
			{
				get { return _projectionColor; }
				set
				{
					if (_projectionColor.Equals(value))
						return;

					_projectionColor = value;
				}
			}

			/// <summary>
			/// Gets or sets a value indicating whether
			/// alignment guides are active.
			/// </summary>
			public bool EnableGuides
			{
				get { return _enableGuides; }
				set { _enableGuides = value; }
			}

			/// <summary>
			/// Gets or sets the color used to paint guides.
			/// </summary>
			public Color GuideColor
			{
				get { return _guideColor; }
				set
				{
					if (_guideColor.Equals(value))
						return;

					_guideColor = value;
				}
			}

			/// <summary>
			/// Gets or sets the color used to paint cursor projections.
			/// </summary>
			public Color CursorColor
			{
				get { return _cursorColor; }
				set
				{
					if (_cursorColor.Equals(value))
						return;

					_cursorColor = value;
				}
			}


			protected override void OnPaint(PaintEventArgs e)
			{
				Draw(e.Graphics);
			}

			protected override void OnPaintBackground(PaintEventArgs e)
			{
			}

			protected override void OnMouseDown(MouseEventArgs e)
			{
				base.OnMouseDown(e);

				if (!_enableGuides)
					return;

				if (e.Button != MouseButtons.Left)
					return;

				if (_parent.Document == null)
					return;

				FlowChart doc = _parent.Document;

				// Record undo operation
				_command = null;
				if (doc.UndoManager.UndoEnabled)
				{
					_command = new CompositeCmd(doc, "Align");
					foreach (ChartObject obj in doc.Objects)
						_command.AddSubCmd(new ModifyItemCmd(obj));
				}

				// Begin align action
				Capture = true;
				_aligning = true;
				_alignedNodes.Clear();
				_ptInitial = doc.ClientToDoc(new Point(e.X, e.Y));
				_ptCurrent = _ptInitial;
				_align = AlignStart.Undetermined;
			}

			float getThresh()
			{
				float div = 5f;

				if (_parent.Document == null)
					return 1f / div;

				switch (_parent.Document.MeasureUnit)
				{
					case GraphicsUnit.Millimeter:
						return 1.0f / div;
					case GraphicsUnit.Inch:
						return 1.0f/12 / div;
					case GraphicsUnit.Point:
						return 72.0f/12 / div;
					case GraphicsUnit.Pixel:
						return 3.0f / div;
					case GraphicsUnit.Document:
						return 300.0f/12 / div;
					case GraphicsUnit.Display:
						return 75.0f/12 / div;
				}

				return 1f / div;
			}

			protected override void OnMouseMove(MouseEventArgs e)
			{
				base.OnMouseMove(e);

				if (_aligning)
				{
					FlowChart doc = _parent.Document;
					PointF ptDoc = doc.ClientToDoc(new Point(e.X, e.Y));

					// Determine the alignment
					if (_align == AlignStart.Undetermined)
					{
						if (_orientation == Orientation.Horizontal)
						{
							if (ptDoc.X < _ptInitial.X)
								_align = AlignStart.Decrease;
							else if (ptDoc.X > _ptInitial.X)
								_align = AlignStart.Increase;
						}
						else
						{
							if (ptDoc.Y < _ptInitial.Y)
								_align = AlignStart.Decrease;
							else if (ptDoc.Y > _ptInitial.Y)
								_align = AlignStart.Increase;
						}

						if (_align == AlignStart.Undetermined)
							return;
					}

					// Check which objects are collected
					foreach (ChartObject obj in doc.Objects)
					{
						if (!(obj is Node))
							continue;

						if (!obj.Visible)
							continue;

						if (obj.Locked)
							continue;

						// If there are selected objects, align only them
						if (doc.Selection.Objects.Count > 0 && !obj.Selected)
							continue;

						Node node = obj as Node;
						RectangleF bounds = node.BoundingRect;

						if (_orientation == Orientation.Horizontal)
						{
							if (_align == AlignStart.Increase)
							{
								// Left-side align
								if (bounds.Left >= Math.Min(ptDoc.X, _ptCurrent.X) - getThresh() &&
									bounds.Left <= Math.Max(ptDoc.X, _ptCurrent.X) + getThresh() &&
									!_alignedNodes.Contains(node))
								{
									_alignedNodes.Add(node, Alignment.Near);
								}
							}
							else
							{
								// Right-side align
								if (bounds.Right >= Math.Min(ptDoc.X, _ptCurrent.X) - getThresh() &&
									bounds.Right <= Math.Max(ptDoc.X, _ptCurrent.X) + getThresh() &&
									!_alignedNodes.Contains(node))
								{
									_alignedNodes.Add(node, Alignment.Far);
								}
							}
						}
						else
						{
							if (_align == AlignStart.Increase)
							{
								// Top-side align
								if (bounds.Top >= Math.Min(ptDoc.Y, _ptCurrent.Y) - getThresh() &&
									bounds.Top <= Math.Max(ptDoc.Y, _ptCurrent.Y) + getThresh() &&
									!_alignedNodes.Contains(node))
								{
									_alignedNodes.Add(node, Alignment.Near);
								}
							}
							else
							{
								// Bottom-side align
								if (bounds.Bottom >= Math.Min(ptDoc.Y, _ptCurrent.Y) - getThresh() &&
									bounds.Bottom <= Math.Max(ptDoc.Y, _ptCurrent.Y) + getThresh() &&
									!_alignedNodes.Contains(node))
								{
									_alignedNodes.Add(node, Alignment.Far);
								}
							}
						}
					}

					// Update dragged nodes
					foreach (Node node in _alignedNodes.Keys)
					{
						Alignment alignment = (Alignment)_alignedNodes[node];
						RectangleF bounds = node.BoundingRect;

						if (_orientation == Orientation.Horizontal)
						{
							if (alignment == Alignment.Near)
							{
								// Left-side align
								bounds.X = ptDoc.X;
							}
							else
							{
								// Right-side align
								bounds.X = ptDoc.X - bounds.Width;
							}
						}
						else
						{
							if (alignment == Alignment.Near)
							{
								// Top-side align
								bounds.Y = ptDoc.Y;
							}
							else
							{
								// Bottom-side align
								bounds.Y = ptDoc.Y - bounds.Height;
							}
						}

						// Set bounding rect
						node.SetRect(bounds, false);
					}

					doc.Invalidate();

					_ptCurrent = ptDoc;
				}
			}

			protected override void OnMouseUp(MouseEventArgs e)
			{
				base.OnMouseUp(e);

				// Record undo operation
				if (_command != null)
				{
					_command.Execute();
					_command = null;
				}

				if (_aligning)
				{
					Capture = false;
					_aligning = false;
					_parent.Document.Invalidate();

					foreach (Node node in _alignedNodes.Keys)
						node.SetRect(node.BoundingRect, true);
				}
			}


			internal void OnDocumentPaint(Graphics g)
			{
				if (!_aligning)
					return;

				FlowChart doc = _parent.Document;
				Point point = doc.PointToClient(Cursor.Position);
				Rectangle rect = doc.ClientRectangle;

				System.Drawing.Pen pen =
					new System.Drawing.Pen(_guideColor);

				if (_orientation == Orientation.Horizontal)
					g.DrawLine(pen, point.X, rect.Top, point.X, rect.Bottom);
				else
					g.DrawLine(pen, rect.Left, point.Y, rect.Right, point.Y);

				pen.Dispose();
			}

			/// <summary>
			/// Draws the scale in the specified target.
			/// </summary>
			private void Draw(Graphics g)
			{
				Rectangle rect = this.ClientRectangle;
				System.Drawing.Brush brush =
					new System.Drawing.SolidBrush(BackColor);
				System.Drawing.Pen pen =
					new System.Drawing.Pen(ForeColor);

				rect.Width -= 1;
				rect.Height -= 1;

				g.FillRectangle(brush, rect);
				g.DrawRectangle(pen, rect);

				// Draw the active node' projection
				DrawProjection(g);

				// Draw the division marks
				DrawMarks(g);

				// Draw cursor projection
				if (!this.DesignMode)
					DrawCursorProjection(g);

				pen.Dispose();
				brush.Dispose();
			}

			/// <summary>
			/// Draws the projection of the active box (if any).
			/// </summary>
			private void DrawProjection(Graphics g)
			{
				FlowChart doc = _parent.Document;

				if (doc == null)
					return;

				if (doc.ActiveObject == null)
					return;

				if (!(doc.ActiveObject is Node))
					return;

				Node node = doc.ActiveObject as Node;
				RectangleF bounds = node.BoundingRect;
				Rectangle client = this.ClientRectangle;

				// Translate node's coordinates
				Point lt = doc.DocToClient(new PointF(bounds.Left, bounds.Top));
				Point rb = doc.DocToClient(new PointF(bounds.Right, bounds.Bottom));

				System.Drawing.Brush brush =
					new System.Drawing.SolidBrush(_projectionColor);

				if (_orientation == Orientation.Horizontal)
				{
					Rectangle rect = client;
					rect.Y += 1;
					rect.Height -= 2;
					rect.X = lt.X;
					rect.Width = rb.X - lt.X + 1;

					if (rect.X < client.X)
					{
						rect.Width -= (client.X - rect.X);
						rect.X = client.X;
					}

					if (rect.X + rect.Width > client.Right)
					{
						rect.Width = client.Right - rect.X - 1;
					}

					g.FillRectangle(brush, rect);
				}
				else
				{
					Rectangle rect = client;
					rect.X += 1;
					rect.Width -= 2;
					rect.Y = lt.Y;
					rect.Height = rb.Y - lt.Y + 1;

					if (rect.Y < client.Y)
					{
						rect.Height -= (client.Y - rect.Y);
						rect.Y = client.Y;
					}

					if (rect.Y + rect.Height > client.Bottom)
					{
						rect.Height = client.Bottom - rect.Y - 1;
					}

					g.FillRectangle(brush, rect);
				}

				brush.Dispose();
			}

			/// <summary>
			/// Draws the division marks and numbers.
			/// </summary>
			private void DrawMarks(Graphics g)
			{
				FlowChart doc = _parent.Document;

				if (doc == null)
					return;

				Rectangle rect = this.ClientRectangle;
				float pixelsPerUnit = CalcPixelsPerUnit();
				int units = CalcUnitInterval(pixelsPerUnit);

				Point doc0InDevice = Point.Empty;
				
				if (_unit != RulerUnit.Percent)
					doc0InDevice = doc.DocToClient(new PointF(0, 0));
				else
					doc0InDevice = doc.DocToClient(doc.DocExtents.Location);

				System.Drawing.Pen pen = new System.Drawing.Pen(ForeColor);
				System.Drawing.Brush brush = new System.Drawing.SolidBrush(ForeColor);

				float fontHeight = Font.GetHeight(g) + 2;

				if (_orientation == Orientation.Horizontal)
				{
					float x = (float)doc0InDevice.X;

					int steps = 0;
					if (x <= rect.Left)
					{
						while (x < rect.Left)
						{
							x += units * pixelsPerUnit;
							steps++;
						}
						x -= units * pixelsPerUnit;
						steps--;
					}

					else if (x > rect.Left)
					{
						while (x > rect.Left)
						{
							x -= units * pixelsPerUnit;
							steps--;
						}
					}

					while (x < rect.Right)
					{
						int num = steps * units;

						g.DrawLine(pen, x, rect.Top + 1, x, rect.Top + rect.Height - 2);
						g.DrawString(num.ToString(), Font, brush, x + 1, 1);

						// Draw sub-marks
						int sub = 5;
						if (units * pixelsPerUnit > 100)
							sub = 20;
						else if (units * pixelsPerUnit > 40)
							sub = 10;

						float dist = units * pixelsPerUnit / sub;
						for (int i = 1; i < sub; i++)
						{
							float f = (sub % 2 == 0 && i == sub / 2) ? 3 : 0;
							g.DrawLine(pen,
								x + i * dist, rect.Top + fontHeight - f,
								x + i * dist, rect.Bottom - 1);
						}


						x += units * pixelsPerUnit;
						steps++;
					}
				}
				else
				{
					float y = (float)doc0InDevice.Y;

					int steps = 0;
					if (y <= rect.Top)
					{
						while (y < rect.Top)
						{
							y += units * pixelsPerUnit;
							steps++;
						}
						y -= units * pixelsPerUnit;
						steps--;
					}

					else if (y > rect.Top)
					{
						while (y > rect.Top)
						{
							y -= units * pixelsPerUnit;
							steps--;
						}
					}

					while (y < rect.Bottom + pixelsPerUnit * units)
					{
						int num = steps * units;

						g.DrawLine(pen, rect.Left + 1, y, rect.Left + rect.Width - 2, y);
						GraphicsState state = g.Save();
						g.TranslateTransform(-1, -(y + 1), MatrixOrder.Append);
						g.RotateTransform(-90, MatrixOrder.Append);
						g.TranslateTransform(1, y + 1, MatrixOrder.Append);
						g.DrawString(num.ToString(), Font, brush, 1, y + 1);
						g.Restore(state);

						// Draw sub-marks
						int sub = 5;
						if (units * pixelsPerUnit > 100)
							sub = 20;
						else if (units * pixelsPerUnit > 40)
							sub = 10;

						float dist = units * pixelsPerUnit / sub;
						for (int i = 1; i < sub; i++)
						{
							float f = (sub % 2 == 0 && i % 5 == 0) ? 3 : 0;
							g.DrawLine(pen,
								rect.Left + fontHeight - f, y + i * dist,
								rect.Right - 1, y + i * dist);
						}


						y += units * pixelsPerUnit;
						steps++;
					}
				}

				brush.Dispose();
				pen.Dispose();
			}

			private void DrawCursorProjection(Graphics g)
			{
				if (_parent.Document == null)
					return;

				Rectangle rect = this.ClientRectangle;
				Point point = _parent.Document.PointToClient(Cursor.Position);

				if (!_parent.Document.ClientRectangle.Contains(point))
					return;

				System.Drawing.Pen pen =
					new System.Drawing.Pen(_cursorColor, 0);

				if (_orientation == Orientation.Horizontal)
					g.DrawLine(pen, point.X, rect.Top, point.X, rect.Bottom);
				else
					g.DrawLine(pen, rect.Left, point.Y, rect.Right, point.Y);

				pen.Dispose();
			}

			/// <summary>
			/// Gets setup graphics.
			/// </summary>
			private Graphics GetGraphics()
			{
				Graphics g = CreateGraphics();

				// Set page unit
				switch (_unit)
				{

					case RulerUnit.Inch:
						g.PageUnit = GraphicsUnit.Inch;
						break;

					case RulerUnit.Millimeter:
						g.PageUnit = GraphicsUnit.Millimeter;
						break;

					case RulerUnit.Pixel:
						g.PageUnit = GraphicsUnit.Pixel;
						break;

					case RulerUnit.Point:
						g.PageUnit = GraphicsUnit.Point;
						break;

					case RulerUnit.Percent:
						g.PageUnit = GraphicsUnit.Pixel;
						break;

				}

				// Obtain the current zoom factor from the ruler document
				if (_parent.Document != null)
					g.PageScale = _parent.Document.ZoomFactor / 100.0f;

				return g;
			}

			/// <summary>
			/// Computes a value representing how many pixels
			/// are needed to display single unit of the current
			/// units of measure of the scale (i.e. when the scale
			/// unit is inches, returns how many pixels equal
			/// 1 Inch).
			/// </summary>
			/// <returns></returns>
			private float CalcPixelsPerUnit()
			{
				Graphics g = GetGraphics();

				// Transform
				PointF[] points = new PointF[]
					{
						new PointF(0, 0),
						new PointF(1, 0)
					};
				
				g.TransformPoints(CoordinateSpace.Device,
					CoordinateSpace.World, points);

				g.Dispose();

				if (_unit != RulerUnit.Percent && _parent.Document != null)
				{
					return points[1].X - points[0].X;
				}
				else
				{
					// Calculate the total FlowChart extents, expressed
					// in scale units (i.e. if the scale unit is Pixel and
					// the FlowChart unit is Millimeter, then transform
					// FlowChart extents from millimeters to pixels
					float totalUnits = 0;
					FlowChart doc = _parent.Document;
					RectangleF extents = doc.DocExtents;

					Point pt0 = doc.DocToClient(
						new PointF(0, 0));
					Point pt1 = doc.DocToClient(
						new PointF(doc.DocExtents.Width, doc.DocExtents.Height));

					if (_orientation == Orientation.Horizontal)
						totalUnits = pt1.X - pt0.X;
					else
						totalUnits = pt1.Y - pt0.Y;

					// Ignore scale factor
					totalUnits = totalUnits * 100 / doc.ZoomFactor;

					return (points[1].X - points[0].X) * totalUnits / 100;
				}
			}

			/// <summary>
			/// Calculates the interval (in units) between
			/// two consecutively displayed numbers on the scale,
			/// ensuring perspicuity.
			/// </summary>
			private int CalcUnitInterval(float pixelsPerUnit)
			{
				// Calculate relative label width
				float width = 0.1f;
				string text = "10000";

				Graphics g = CreateGraphics();
				Region[] r = g.MeasureCharacterRanges(text, Font, _rspeeder, _speeder);

				width = r[0].GetBounds(g).Width;

                r[0].Dispose();
				g.Dispose();

				// Calculate the interval now
				int[] intervals = new int[]
					{
						1, 2, 5, 10, 20, 50, 100, 200,
						500, 1000, 2000, 5000, 10000
					};

				for (int i = 0; i < intervals.Length; i++)
				{
					if (intervals[i] * pixelsPerUnit > width)
						return intervals[i];
				}

				return intervals[intervals.Length - 1];
			}


			private Ruler _parent;
			private Orientation _orientation;
			private Alignment _alignment;

			private RulerUnit _unit;
			private Color _projectionColor;
			private bool _enableGuides;
			private Color _guideColor;
			private Color _cursorColor;

			private enum AlignStart
			{
				Decrease, Increase, Undetermined
			}

			internal bool IsAligning { get { return _aligning; } }

			private bool _aligning;
			private Hashtable _alignedNodes;
			private PointF _ptInitial;
			private PointF _ptCurrent;
			private AlignStart _align;
			private CompositeCmd _command = null;

			// The following are used merely for character measurement
			private StringFormat _speeder;
			private RectangleF _rspeeder = new RectangleF(0, 0, 1000, 1000);
		}


		/// <summary>
		/// Creates new ruler object.
		/// </summary>
		public Ruler()
		{
			InitializeComponent();

			this.ControlAdded += new ControlEventHandler(this.OnControlAdded);
			this.ControlRemoved += new ControlEventHandler(this.OnControlRemoved);

			_horizontalScale = new Scale(this, Orientation.Horizontal);
			_horizontalScale.Dock = DockStyle.Fill;
			_horizontalScale.MouseUp += new MouseEventHandler(OnMouseUp);
			_panelHorizontal.Controls.Add(_horizontalScale);

			_verticalScale = new Scale(this, Orientation.Vertical);
			_verticalScale.Dock = DockStyle.Fill;
			_verticalScale.MouseUp += new MouseEventHandler(OnMouseUp);
			_panelVertical.Controls.Add(_verticalScale);

			_contextMenu = null;

			_document = null;
			_prevDocParent = null;
			_prevDocDock = DockStyle.None;
			_prevDocBounds = Rectangle.Empty;

			_handlerPaint = new PaintEventHandler(OnDocumentPaint);
			_handlerBoxActivated = new BoxEvent(OnDocumentBoxActivated);
			_handlerBoxDeactivated = new BoxEvent(OnDocumentBoxDeactivated);
			_handlerBoxDeleted = new BoxEvent(OnDocumentBoxDeleted);
			_handlerTableActivated = new TableEvent(OnDocumentTableActivated);
			_handlerTableDeactivated = new TableEvent(OnDocumentTableDeactivated);
			_handlerTableDeleted = new TableEvent(OnDocumentTableDeleted);
			_handlerHostActivated = new ControlHostEvent(OnDocumentHostActivated);
			_handlerHostDeactivated = new ControlHostEvent(OnDocumentHostDeactivated);
			_handlerHostDeleted = new ControlHostEvent(OnDocumentHostDeleted);
			_handlerScrollChanged = new EventHandler(OnDocumentScrollChanged);
			_handlerZoomFactorChanged = new EventHandler(OnDocumentZoomFactorChanged);
			_handlerDocExtentsChanged = new EventHandler(OnDocumentDocExtentsChanged);
			_handlerMeasureUnitChanged = new EventHandler(OnDocumentMeasureUnitChanged);
			_handlerDocMouseEnter = new EventHandler(OnDocumentMouseEnter);
			_handlerDocMouseMove = new MouseEventHandler(OnDocumentMouseMove);
			_handlerDocMouseLeave = new EventHandler(OnDocumentMouseLeave);

			_timer = new Timer();
			_timer.Interval = 400;
			_timer.Tick += new EventHandler(OnTimer);
			_timer.Start();
		}


		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				_timer.Dispose();
			}

			base.Dispose(disposing);
		}


		#region Component Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Ruler));
			this._panelVertical = new System.Windows.Forms.Panel();
			this._panelTop = new System.Windows.Forms.Panel();
			this._panelHorizontal = new System.Windows.Forms.Panel();
			this._label = new System.Windows.Forms.Label();
			this._panelTop.SuspendLayout();
			this.SuspendLayout();
			// 
			// _panelVertical
			// 
			this._panelVertical.Dock = System.Windows.Forms.DockStyle.Left;
			this._panelVertical.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.4F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this._panelVertical.Location = new System.Drawing.Point(0, 18);
			this._panelVertical.Name = "_panelVertical";
			this._panelVertical.Size = new System.Drawing.Size(18, 326);
			this._panelVertical.TabIndex = 2;
			// 
			// _panelTop
			// 
			this._panelTop.Controls.Add(this._panelHorizontal);
			this._panelTop.Controls.Add(this._label);
			this._panelTop.Dock = System.Windows.Forms.DockStyle.Top;
			this._panelTop.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.4F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this._panelTop.Location = new System.Drawing.Point(0, 0);
			this._panelTop.Name = "_panelTop";
			this._panelTop.Size = new System.Drawing.Size(384, 18);
			this._panelTop.TabIndex = 0;
			// 
			// _panelHorizontal
			// 
			this._panelHorizontal.Dock = System.Windows.Forms.DockStyle.Fill;
			this._panelHorizontal.Location = new System.Drawing.Point(18, 0);
			this._panelHorizontal.Name = "_panelHorizontal";
			this._panelHorizontal.Size = new System.Drawing.Size(366, 18);
			this._panelHorizontal.TabIndex = 1;
			// 
			// _label
			// 
			this._label.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._label.Dock = System.Windows.Forms.DockStyle.Left;
			this._label.Image = ((System.Drawing.Image)(resources.GetObject("_label.Image")));
			this._label.Location = new System.Drawing.Point(0, 0);
			this._label.Name = "_label";
			this._label.Size = new System.Drawing.Size(18, 18);
			this._label.TabIndex = 0;
			this._label.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			this._label.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnMouseUp);
			// 
			// Ruler
			// 
			this.Controls.Add(this._panelVertical);
			this.Controls.Add(this._panelTop);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.4F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Size = new System.Drawing.Size(300, 200);
			this._panelTop.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion


		/// <summary>
		/// Gets or sets the FlowChart document this
		/// ruler is associated with.
		/// </summary>
		[Browsable(false)]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public FlowChart Document
		{
			get { return _document; }
			set
			{
				// Unset the currently contained document (if any)
				if (_document != null)
				{
					// Restore the document parent at the same time
					// removing the control from the child list.
					// If the old parent
					if (_prevDocParent == null)
						_document.Parent = this.Parent;
					else
						_document.Parent = _prevDocParent;
					_prevDocParent = null;
				}

				// Set the document if needed
				if (value != null)
				{
					// Remember the document parent, to restore
					// it when the document is unset
					_prevDocParent = value.Parent;

					// Set the document parent to be the Ruler.
					// This will cause the OnControlAdded handler
					// to be invoked,
					value.Parent = this;
				}
			}
		}

		/// <summary>
		/// Sets the document from the OnControlAdded event handler.
		/// </summary>
		private void Set(FlowChart document)
		{
			_document = document;

			// Remember the document's bound and dock
			_prevDocBounds = _document.Bounds;
			_prevDocDock = _document.Dock;

			// Dock the FlowChart to the ruler
			_document.Dock = DockStyle.Fill;

			// Set the required event handlers
			_document.Paint += _handlerPaint;
			_document.BoxActivated += _handlerBoxActivated;
			_document.BoxDeactivated += _handlerBoxDeactivated;
			_document.BoxDeleted += _handlerBoxDeleted;
			_document.TableActivated += _handlerTableActivated;
			_document.TableDeactivated += _handlerTableDeactivated;
			_document.TableDeleted += _handlerTableDeleted;
			_document.ControlHostActivated += _handlerHostActivated;
			_document.ControlHostDeactivated += _handlerHostDeactivated;
			_document.ControlHostDeleted += _handlerHostDeleted;
			_document.ScrollChanged += _handlerScrollChanged;
			_document.ZoomFactorChanged += _handlerZoomFactorChanged;
			_document.DocExtentsChanged += _handlerDocExtentsChanged;
			_document.MeasureUnitChanged += _handlerMeasureUnitChanged;
			_document.MouseEnter += _handlerDocMouseEnter;
			_document.MouseMove += _handlerDocMouseMove;
			_document.MouseLeave += _handlerDocMouseLeave;
		}

		/// <summary>
		/// Unsets the document, (i.e. set it to null)
		/// from the OnControlAdded event handler.
		/// </summary>
		private void Unset()
		{
			// Restore document bounds and dock as they were
			// recorder when the document has been set
			_document.Bounds = _prevDocBounds;
			_document.Dock = _prevDocDock;

			// Remove the associated event handlers
			_document.Paint -= _handlerPaint;
			_document.BoxActivated -= _handlerBoxActivated;
			_document.BoxDeactivated -= _handlerBoxDeactivated;
			_document.BoxDeleted -= _handlerBoxDeleted;
			_document.TableActivated -= _handlerTableActivated;
			_document.TableDeactivated -= _handlerTableDeactivated;
			_document.TableDeleted -= _handlerTableDeleted;
			_document.ControlHostActivated -= _handlerHostActivated;
			_document.ControlHostDeactivated -= _handlerHostDeactivated;
			_document.ControlHostDeleted -= _handlerHostDeleted;
			_document.ScrollChanged -= _handlerScrollChanged;
			_document.ZoomFactorChanged -= _handlerZoomFactorChanged;
			_document.DocExtentsChanged -= _handlerDocExtentsChanged;
			_document.MeasureUnitChanged -= _handlerMeasureUnitChanged;
			_document.MouseEnter -= _handlerDocMouseEnter;
			_document.MouseMove -= _handlerDocMouseMove;
			_document.MouseLeave -= _handlerDocMouseLeave;

			_document = null;
		}

		/// <summary>
		/// Hide the property from the public.
		/// It is not meant to be used with the Ruler.
		/// </summary>
		[EditorBrowsable(EditorBrowsableState.Never)]
		public new Control.ControlCollection Controls
		{
			get { return base.Controls; }
		}

		/// <summary>
		/// Gets or sets the alignment of the horizontal scale of the ruler.
		/// </summary>
		[Category("Layout")]
		[Description("Gets or sets the alignment of the horizontal scale.")]
		[DefaultValue(typeof(Alignment), "Near")]
		public Alignment HorizontalScaleAlignment
		{
			get { return _horizontalScale.Alignment; }
			set
			{
				if (value != Alignment.Near && value != Alignment.Far)
					value = Alignment.Near;

				_horizontalScale.Alignment = value;
				if (_horizontalScale.Alignment == Alignment.Near)
					_panelTop.Dock = DockStyle.Top;
				else
					_panelTop.Dock = DockStyle.Bottom;
			}
		}

		/// <summary>
		/// Gets or sets the alignment of the vertical scale of the ruler.
		/// </summary>
		[Category("Layout")]
		[Description("Gets or sets the alignment of the vertical scale.")]
		[DefaultValue(typeof(Alignment), "Near")]
		public Alignment VerticalScaleAlignment
		{
			get { return _verticalScale.Alignment; }
			set
			{
				if (value != Alignment.Near && value != Alignment.Far)
					value = Alignment.Near;

				_verticalScale.Alignment = value;
				if (_verticalScale.Alignment == Alignment.Near)
					_panelVertical.Dock = DockStyle.Left;
				else
					_panelVertical.Dock = DockStyle.Right;

				if (_verticalScale.Alignment == Alignment.Near)
					_label.Dock = DockStyle.Left;
				else
					_label.Dock = DockStyle.Right;
			}
		}

		/// <summary>
		/// Specifies whether the vertical ruler scale is visible.
		/// </summary>
		[Category("Appearance")]
		[Description("Specifies whether the vertical scale is visible.")]
		[DefaultValue(true)]
		public bool VerticalScaleVisible
		{
			get { return _panelVertical.Visible; }
			set
			{
				_panelVertical.Visible = value;
				_label.Visible = value;
			}
		}

		/// <summary>
		/// Specifies whether the horizontal ruler scale is visible.
		/// </summary>
		[Category("Appearance")]
		[Description("Specifies whether the horizontal scale is visible.")]
		[DefaultValue(true)]
		public bool HorizontalScaleVisible
		{
			get { return _panelTop.Visible; }
			set { _panelTop.Visible = value; }
		}

		/// <summary>
		/// Specifies the context menu associated with the ruler.
		/// We supply our own implementation of the context menu to
		/// avoid popping it up when the right button is pressed over
		/// the document area.
		/// </summary>
		[Category("Behavior")]
		[Description("Gets or sets the context menu associated with this ruler.")]
		[DefaultValue(null)]
		public override ContextMenu ContextMenu
		{
			get { return _contextMenu; }
			set { _contextMenu = value; }
		}

		/// <summary>
		/// Gets or sets the text displayed in the ruler.
		/// </summary>
		[Category("Appearance")]
		[Description("Gets or sets the text displayed in the ruler.")]
		[Browsable(true)]
		[DefaultValue("")]
		public new string Text
		{
			get { return _label.Text; }
			set { _label.Text = value; }
		}

		/// <summary>
		/// Gets or sets the measure unit used by the ruler.
		/// </summary>
		[Category("Appearance")]
		[Description("Specifies the measure units displayed by the ruler.")]
		[DefaultValue(typeof(RulerUnit), "Millimeter")]
		public RulerUnit Unit
		{
			get { return _horizontalScale.Unit; }
			set
			{
				_horizontalScale.Unit = value;
				_verticalScale.Unit = value;

				RefreshScales();
			}
		}

		/// <summary>
		/// Gets or sets the color used to paint
		/// the projection of the active node.
		/// </summary>
		[Category("Appearance")]
		[Description("Specifies the color used to paint the " +
			 "projections of the active node on the ruler scales.")]
		[DefaultValue(typeof(Color), "LightSteelBlue")]
		public Color ProjectionColor
		{
			get { return _horizontalScale.ProjectionColor; }
			set
			{
				_horizontalScale.ProjectionColor = value;
				_verticalScale.ProjectionColor = value;

				RefreshScales();
			}
		}

		/// <summary>
		/// Gets or sets a value indicating whether
		/// alignment guides are active.
		/// </summary>
		[Category("Behavior")]
		[Description("Specifies whether alignment guides are enabled.")]
		[DefaultValue(true)]
		public bool EnableGuides
		{
			get { return _horizontalScale.EnableGuides; }
			set
			{
				_horizontalScale.EnableGuides = value;
				_verticalScale.EnableGuides = value;
			}
		}

		/// <summary>
		/// Gets or sets the color used to paint guides.
		/// </summary>
		[Category("Appearance")]
		[Description("Gets or sets the color of alignment guides.")]
		public Color GuideColor
		{
			get { return _horizontalScale.GuideColor; }
			set
			{
				_horizontalScale.GuideColor = value;
				_verticalScale.GuideColor = value;
			}
		}

		private bool ShouldSerializeGuideColor()
		{
			return !_horizontalScale.GuideColor.Equals(SystemColors.ControlText);
		}

		public new Color ForeColor
		{
			get { return base.ForeColor; }
			set
			{
				base.ForeColor = value;
				_label.ForeColor = value;
			}
		}

		/// <summary>
		/// Gets or sets the color used to paint cursor projections.
		/// </summary>
		[Category("Appearance")]
		[Description("Gets or sets the color of mouse pointer's projections.")]
		[DefaultValue(typeof(Color), "255, 0, 0")]
		public Color PointerColor
		{
			get { return _horizontalScale.CursorColor; }
			set
			{
				_horizontalScale.CursorColor = value;
				_verticalScale.CursorColor = value;
			}
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);

			// VS' undo/redo is buggy and removes the scale controls from the ruler
			bool fixScales = false;
			if (!Controls.Contains(_panelVertical))
			{
				Controls.Add(_panelVertical);
				fixScales = true;
			}
			if (!Controls.Contains(_panelTop))
			{
				Controls.Add(_panelTop);
				fixScales = true;
			}

			if (_document != null && fixScales)
				_document.BringToFront();

			if (fixScales)
			{
				PerformLayout();
				Refresh();
			}
		}

		protected override void OnBackgroundImageChanged(EventArgs e)
		{
			base.OnBackgroundImageChanged(e);

			_label.Image = this.BackgroundImage;
		}

		protected override void OnFontChanged(EventArgs e)
		{
			base.OnFontChanged(e);

			_label.Font = this.Font;
			if (_horizontalScale != null)
				_horizontalScale.Font = this.Font;
			if (_verticalScale != null)
				_verticalScale.Font = this.Font;
		}


		private void OnMouseUp(object sender, MouseEventArgs e)
		{
			if (_contextMenu != null && e.Button == MouseButtons.Right)
				_contextMenu.Show(sender as Control, new Point(e.X, e.Y));
		}

		private void OnTimer(object sender, EventArgs e)
		{
			RefreshScales();
		}

		private void OnControlAdded(object sender, ControlEventArgs e)
		{
			// Control has been added to the ruler
			// (most probably from the design-time generated code).
			
			// If the control is a FlowChart, set it as a document
			if (e.Control is FlowChart)
			{
				Set(e.Control as FlowChart);
				e.Control.BringToFront();

				this.ResumeLayout();
			}
		}

		private void OnControlRemoved(object sender, ControlEventArgs e)
		{
			if (e.Control is FlowChart)
			{
				Unset();
				this.PerformLayout();
			}
		}


		private void OnDocumentPaint(object sender, PaintEventArgs e)
		{
			_horizontalScale.OnDocumentPaint(e.Graphics);
			_verticalScale.OnDocumentPaint(e.Graphics);
		}

		private void OnDocumentBoxActivated(object sender, BoxEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentBoxDeactivated(object sender, BoxEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentBoxDeleted(object sender, BoxEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentTableActivated(object sender, TableEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentTableDeactivated(object sender, TableEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentTableDeleted(object sender, TableEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentHostActivated(object sender, ControlHostEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentHostDeactivated(object sender, ControlHostEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentHostDeleted(object sender, ControlHostEventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentScrollChanged(object sender, EventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentZoomFactorChanged(object sender, EventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentDocExtentsChanged(object sender, EventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentMeasureUnitChanged(object sender, EventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentMouseEnter(object sender, EventArgs e)
		{
			RefreshScales();
		}

		private void OnDocumentMouseMove(object sender, MouseEventArgs e)
		{
			// TODO: efficiency
			//RefreshScales(false);
			RefreshScales();
		}

		private void OnDocumentMouseLeave(object sender, EventArgs e)
		{
			RefreshScales();
		}


		/// <summary>
		/// Refreshes the scales of the ruler either
		/// unconditionally (if always is set to true)
		/// or only if the document is being modified
		/// by the user (if set to false).
		/// </summary>
		private void RefreshScales(bool always)
		{
			if (always)
			{
				_horizontalScale.Invalidate();
				_verticalScale.Invalidate();
			}
			else
			{
				if (_document != null)
				{
					if (_document.IsModifying)
					{
						_horizontalScale.Invalidate();
						_verticalScale.Invalidate();
					}
				}
			}
		}

		/// <summary>
		/// Refreshes the scales of the ruler unconditionally.
		/// </summary>
		private void RefreshScales()
		{
			RefreshScales(true);
		}


		private System.Windows.Forms.Panel _panelTop;
		private System.Windows.Forms.Panel _panelHorizontal;
		private System.Windows.Forms.Panel _panelVertical;
		private System.Windows.Forms.Label _label;
		private System.Windows.Forms.ContextMenu _contextMenu;

		private FlowChart _document;
		private DockStyle _prevDocDock;
		private Control _prevDocParent;
		private Rectangle _prevDocBounds;

		/// <summary>
		/// This timer is intended to perform update operations
		/// on regular intervals of time, thus fixing repaint
		/// issues when diagram is modified programmatically.
		/// </summary>
		private Timer _timer;

		private PaintEventHandler _handlerPaint;
		private BoxEvent _handlerBoxActivated;
		private BoxEvent _handlerBoxDeactivated;
		private BoxEvent _handlerBoxDeleted;
		private TableEvent _handlerTableActivated;
		private TableEvent _handlerTableDeactivated;
		private TableEvent _handlerTableDeleted;
		private ControlHostEvent _handlerHostActivated;
		private ControlHostEvent _handlerHostDeactivated;
		private ControlHostEvent _handlerHostDeleted;
		private EventHandler _handlerScrollChanged;
		private EventHandler _handlerZoomFactorChanged;
		private EventHandler _handlerDocExtentsChanged;
		private EventHandler _handlerMeasureUnitChanged;
		private EventHandler _handlerDocMouseEnter;
		private MouseEventHandler _handlerDocMouseMove;
		private EventHandler _handlerDocMouseLeave;

		private Ruler.Scale _horizontalScale;
		private Ruler.Scale _verticalScale;
	}
}
