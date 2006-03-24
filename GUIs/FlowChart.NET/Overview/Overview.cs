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

namespace MindFusion.FlowChartX
{
	public class Overview : System.Windows.Forms.Control
	{
		// The viewd document
		private FlowChart document;

		// Visualization options
		private MindFusion.FlowChartX.PrintOptions options;
		private SmoothingMode smoothMode;
		private float scaleFactor;
		private Color marginalColor;

		// Fitting
		private bool fitToAll;
		private bool wereScrollersVisible;
		private float nofitScaleFactor;
		private int nofitScrollX;
		private int nofitScrollY;

		// Scrolling
		HScrollBar hScroller = null;
		VScrollBar vScroller = null;
		private int scrollX = 0;
		private int scrollY = 0;

		// Viewport rectangle
		Rectangle rcView; // in device/client coordinates
		RectangleF rcDoc; // in document coordinates
		float cx = 0;
		float cy = 0;

		// Drag tool
		private IDragTool drag = null;

		// Auto scroll
		private Timer scrollTimer = null;
		private bool autoScroll;
		private int autoScrDX;
		private int autoScrDY;


		public Overview()
		{
			// Set some styles
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			SetStyle(ControlStyles.DoubleBuffer, true);
			SetStyle(ControlStyles.ResizeRedraw, true);
			SetStyle(ControlStyles.UserPaint, true);

			document		= null;
			options			= new MindFusion.FlowChartX.PrintOptions(null);
			smoothMode		= SmoothingMode.Default;
			scaleFactor		= 30;
			fitToAll		= false;
			marginalColor	= Color.FromArgb(35, Color.Black);

			// Initialize rendering options
			options.EnableShadows			= true;
			options.EnableImages			= true;
			options.EnableInterior			= true;
			options.EnableBackground		= true;
			options.EnableBackgroundImage	= true;
			options.EnableText				= true;
			options.PaintControls			= true;

			// Auto scroll
			autoScroll = true;
			autoScrDX = autoScrDY = 0;
		}

		~Overview()
		{
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
				Document = null;

			base.Dispose(disposing);
		}


		/// <summary>
		/// Causes the control to recalculate all internals
		/// and repaint its client area.
		/// </summary>
		public new void Update()
		{
			Fit();
			LayoutScrollbars();
			SetScrollbars();
			CalcCenteringOffsets();
			CalcViewport();
			Invalidate();
		}


		bool iAmScrolling = false;
		internal Rectangle Viewport
		{
			get { return rcView; }
			set
			{
				if (document == null)
					return;

				rcView = value;

				RectangleF rcDiagram = document.DocExtents;
				Rectangle bounds = DocToScreen(rcDiagram);

				if (rcView.Right > bounds.Right)
					rcView.X = bounds.Right - rcView.Width;
				if (rcView.Bottom > bounds.Bottom)
					rcView.Y = bounds.Bottom - rcView.Height;
				if (rcView.Left < bounds.Left)
					rcView.X = bounds.Left;
				if (rcView.Top < bounds.Top)
					rcView.Y = bounds.Top;
				if (rcView.Width > bounds.Width)
					rcView.Width = bounds.Width;
				if (rcView.Height > bounds.Height)
					rcView.Height = bounds.Height;

				rcDoc = ScreenToDoc(rcView);
				iAmScrolling = true;
				document.ScrollTo(rcDoc.X, rcDoc.Y);
				iAmScrolling = false;
			}
		}


		private PaintEventHandler documentPaint = null;
		private EventHandler documentResize = null;
		private EventHandler scrollChanged = null;
		private EventHandler zoomFactorChanged = null;
		private EventHandler docExtentsChanged = null;

		public MindFusion.FlowChartX.FlowChart Document
		{
			get { return document; }
			set
			{
				if (document != null)
				{
					document.Paint				-= documentPaint;
					document.Resize				-= documentResize;
					document.ScrollChanged		-= scrollChanged;
					document.ZoomFactorChanged	-= zoomFactorChanged;
					document.DocExtentsChanged	-= docExtentsChanged;
				}

				document = value;
				if (document != null)
				{
					documentPaint		= new PaintEventHandler(OnDocumentPaint);
					documentResize		= new EventHandler(OnDocumentResize);
					scrollChanged		= new EventHandler(OnScrollChanged);
					zoomFactorChanged	= new EventHandler(OnZoomFactorChanged);
					docExtentsChanged	= new EventHandler(OnDocExtentsChanged);

					document.Paint				+= documentPaint;
					document.Resize				+= documentResize;
					document.ScrollChanged		+= scrollChanged;
					document.ZoomFactorChanged	+= zoomFactorChanged;
					document.DocExtentsChanged	+= docExtentsChanged;

					SetScrollbars();
					CalcCenteringOffsets();
					CalcViewport();
				}
				else
				{
					rcView = Rectangle.Empty;
					rcDoc = RectangleF.Empty;
				}
				Invalidate();
			}
		}

		[Category("Behavior")]
		[DefaultValue(false)]
		public bool FitAll
		{
			get { return fitToAll; }
			set
			{
				if (fitToAll == value)
					return;

				fitToAll = value;
				if (fitToAll)
				{
					wereScrollersVisible = ShowScrollbars;
					nofitScaleFactor = scaleFactor;
					nofitScrollX = scrollX;
					nofitScrollY = scrollY;

					scrollX = 0;
					scrollY = 0;
					ShowScrollbars = false;

					// Calculate the new scale factor
					Fit();
				}
				else
				{
					scaleFactor = nofitScaleFactor;
					scrollX = nofitScrollX;
					scrollY = nofitScrollY;
					ShowScrollbars = wereScrollersVisible;
				}

				CalcCenteringOffsets();
				CalcViewport();
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(30)]
		public int ScaleFactor
		{
			get { return (int)scaleFactor; }
			set
			{
				int newScale = value;
				newScale = value;
				if (newScale > 100)
					newScale = 100;
				if (newScale < 10)
					newScale = 10;

				// Scale factor cannot be set when whole-fitting
				if (fitToAll)
				{
					// Remember the scale factor so
					// that when the user turns fit whole off
					// the factor is the one that has been set here
					nofitScaleFactor = newScale;
				}
				else
				{
					scaleFactor = newScale;
					SetScrollbars();
					CalcCenteringOffsets();
					CalcViewport();
					Invalidate();
				}
			}
		}

		[Category("Appearance")]
		[DefaultValue(typeof(SmoothingMode), "Default")]
		public SmoothingMode AntiAlias
		{
			get { return smoothMode; }
			set
			{
				smoothMode = value;
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(true)]
		public bool DisplayShadows
		{
			get { return options.EnableShadows; }
			set
			{
				options.EnableShadows = value;
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(true)]
		public bool DisplayImages
		{
			get { return options.EnableImages; }
			set
			{
				options.EnableImages = value;
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(true)]
		public bool DisplayInterior
		{
			get { return options.EnableInterior; }
			set
			{
				options.EnableInterior = value;
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(true)]
		public bool DisplayBackground
		{
			get { return options.EnableBackground; }
			set
			{
				options.EnableBackground = value;
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(true)]
		public bool DisplayBackgroundImage
		{
			get { return options.EnableBackgroundImage; }
			set
			{
				options.EnableBackgroundImage = value;
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(true)]
		public bool DisplayText
		{
			get { return options.EnableText; }
			set
			{
				options.EnableText = value;
				Invalidate();
			}
		}

		[Category("Appearance")]
		public override Color BackColor
		{
			get { return base.BackColor; }
			set
			{
				base.BackColor = value;
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(false)]
		public bool ShowScrollbars
		{
			get
			{
				return (hScroller != null && vScroller != null); 
			}
			set
			{
				if (value == ShowScrollbars)
					return;

				// Cannot show scrollbars while whole-fitting
				if (fitToAll && value)
					return;

				if (hScroller != null) 
					Controls.Remove(hScroller);
				if (vScroller != null) 
					Controls.Remove(vScroller);

				hScroller = null;
				vScroller = null;

				if (value)
				{
					hScroller = new HScrollBar();
					hScroller.Scroll += new ScrollEventHandler(OnHScroll);
					Controls.Add(hScroller);

					vScroller = new VScrollBar();
					vScroller.Scroll += new ScrollEventHandler(OnVScroll);
					Controls.Add(vScroller);

					LayoutScrollbars();
					SetScrollbars();
					hScroller.Value = scrollX;
					vScroller.Value = scrollY;
				}

				CalcViewport();
				CalcCenteringOffsets();
				Invalidate();
			}
		}

		[Category("Appearance")]
		[DefaultValue(typeof(Color), "35, 0, 0, 0")]
		public Color DimColor
		{
			get { return marginalColor; }
			set
			{
				if (marginalColor.Equals(value))
					return;

				marginalColor = value;
				Invalidate();
			}
		}


		private void ScrollTo(int x, int y)
		{
			Rectangle rc = ClientRectangle;
			if (hScroller != null && vScroller != null)
			{
				rc.Width -= vScroller.Width;
				rc.Height -= hScroller.Height;
			}

			// Calculate the visible overview area in document coordinates
			RectangleF rcDoc = ScreenToDoc(rc);

			int htotal = (int)(document.DocExtents.Width);
			int vtotal = (int)(document.DocExtents.Height);
			int hvisible = (int)rcDoc.Width;
			int vvisible = (int)rcDoc.Height;

			if (x >= 0 && (x <= htotal - hvisible + 1 || x < scrollX))
			{
				scrollX = x;
				if (hScroller != null)
					hScroller.Value = x;
			}
			if (y >= 0 && (y <= vtotal - vvisible + 1 || y < scrollY))
			{
				scrollY = y;
				if (vScroller != null)
					vScroller.Value = y;
			}

			Invalidate();
		}


		protected override void OnMouseDown(System.Windows.Forms.MouseEventArgs e)
		{
			base.OnMouseDown(e);

			// Start dragging ?
			if (e.Button == MouseButtons.Left)
			{
				this.Capture = true;
				if (rcView.Contains(e.X, e.Y))
				{
					drag = new ViewDrag(this, e.X, e.Y);
				}
				else
				{
					// Move the viewport to the click points
					Rectangle movedView = rcView;
					movedView.X = e.X - movedView.Width / 2;
					movedView.Y = e.Y - movedView.Height / 2;
					Viewport = movedView;

					// Start dragging
					drag = new ViewDrag(this, e.X, e.Y);
				}
			}
			else if (e.Button == MouseButtons.Right)
			{
				this.Capture = false;
				if (drag != null)
				{
					drag.Cancel();
					drag = null;
				}
			}
		}

		protected override void OnMouseUp(System.Windows.Forms.MouseEventArgs e)
		{
			base.OnMouseUp(e);

			// Complete dragging
			if (drag != null)
			{
				this.Capture = false;
				drag.Complete(e.X,	e.Y);
				drag = null;
			}

			// Stop autoscrolling
			if (scrollTimer != null)
			{
				scrollTimer.Enabled = false;
				scrollTimer = null;
			}
		}

		protected override void OnMouseWheel(MouseEventArgs e)
		{
			ScaleFactor += Math.Min(1, e.Delta / 100);

			base.OnMouseWheel(e);
		}

		protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
		{
			base.OnMouseMove(e);

			// Set cursor
			Cursor newCursor = Cursors.Default;
			if (drag != null)
			{
				newCursor = drag.Cursor;
			}
			else
			{
				// Check if the mouse is by any chance over the rectangle
				if (rcView.Contains(e.X, e.Y))
					newCursor = Cursors.Hand;
			}
			Cursor = newCursor;

			// Update drag operation
			if (drag != null)
				drag.Update(e.X, e.Y);

			// Check auto scroll
			if (autoScroll)
				CheckAutoScroll(e);
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint (e);

			Graphics g = e.Graphics;
			GraphicsContainer state = g.BeginContainer();

			// fill the background
			System.Drawing.SolidBrush back = new System.Drawing.SolidBrush(BackColor);
			g.FillRectangle(back, ClientRectangle);
			back.Dispose();

			if (document != null)
			{
				SetTransforms(g);

				// set clipping rectangle
				g.SetClip(document.DocExtents);

				// draw contents
				DrawContents(g);

				// reset clipping rectangle
				g.ResetClip();

				// Turn off the smooth mode
				g.SmoothingMode = SmoothingMode.None;

				g.SetClip(document.DocExtents);
				g.SetClip(rcDoc, CombineMode.Exclude);
				System.Drawing.Brush marginal =
					new System.Drawing.SolidBrush(marginalColor);
				g.FillRectangle(marginal, document.DocExtents);
				marginal.Dispose();
				g.ResetClip();

				// Draw the view rectangle
				System.Drawing.Pen pen = new System.Drawing.Pen(Color.Black, 0);
				g.DrawRectangle(pen, rcDoc.X, rcDoc.Y, rcDoc.Width, rcDoc.Height);
				pen.Dispose();
			}

			g.EndContainer(state);

			// Draw the small rect around the scrollbar connection point
			if (ShowScrollbars)
			{
				System.Drawing.Brush back2 =
					new System.Drawing.SolidBrush(SystemColors.Control);
				Rectangle rc = new Rectangle(vScroller.Left, hScroller.Top,
					vScroller.Width, hScroller.Height);

				g.FillRectangle(back2, rc);

				back2.Dispose();
			}
		}

		protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs pevent)
		{
			// All painting is carried by the control itself
		}

		protected override void OnResize(System.EventArgs e)
		{
			if (Width > 0 && Height > 0)
			{
				// Update scrollers
				Fit();
				LayoutScrollbars();
				SetScrollbars();
				CalcCenteringOffsets();
				CalcViewport();
			}

			base.OnResize(e);
		}

		private void OnHScroll(object sender, ScrollEventArgs e)
		{
			scrollX = e.NewValue;
			CalcViewport();
			Invalidate();
		}

		private void OnVScroll(object sender, ScrollEventArgs e)
		{
			scrollY = e.NewValue;
			CalcViewport();
			Invalidate();
		}


		private void OnDocumentPaint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			Invalidate();
		}

		private void OnDocumentResize(object Sender, System.EventArgs e)
		{
			CalcViewport();
			Invalidate();
		}

		private void OnScrollChanged(object Sender, System.EventArgs e)
		{
			if (iAmScrolling)
				return;
			CalcViewport();
			Invalidate();
		}

		private void OnZoomFactorChanged(object Sender, System.EventArgs e)
		{
			Fit();
			SetScrollbars();
			CalcCenteringOffsets();
			CalcViewport();
		}

		private void OnDocExtentsChanged(object Sender, System.EventArgs e)
		{
			Fit();
			SetScrollbars();
			CalcCenteringOffsets();
			CalcViewport();
		}


		private void OnAutoScrollTimer(Object obj, EventArgs args)
		{
			if (fitToAll)
				return;

			Rectangle rc = ClientRectangle;
			Rectangle rcDoc = DocToScreen(document.DocExtents);

			int scrX = scrollX;
			int scrY = scrollY;
			if (rc.Width < rcDoc.Width)
				scrX = scrollX + autoScrDX;
			if (rc.Height < rcDoc.Height)
				scrY = scrollY + autoScrDY;
			ScrollTo(scrX, scrY);
		}

		private int GetAutoScroll() { return 4; }

		private void CheckAutoScroll(MouseEventArgs args)
		{
			Point ptDev = new Point(args.X, args.Y);

			if (!Utilities.pointInRect(ptDev, ClientRectangle))
			{
				autoScrDX = autoScrDY = 0;
				if (ptDev.X < ClientRectangle.Left) autoScrDX = -GetAutoScroll();
				if (ptDev.Y < ClientRectangle.Top) autoScrDY = -GetAutoScroll();
				if (ptDev.X > ClientRectangle.Right) autoScrDX = GetAutoScroll();
				if (ptDev.Y > ClientRectangle.Bottom) autoScrDY = GetAutoScroll();

				OnAutoScrollTimer(this, null);
				if (scrollTimer == null)
				{
					scrollTimer = new Timer();
					scrollTimer.Interval = 100;
					scrollTimer.Tick += new EventHandler(OnAutoScrollTimer);
					scrollTimer.Enabled = true;
				}
			}
			else
			{
				if (scrollTimer != null)
				{
					scrollTimer.Enabled = false;
					Invalidate();
				}
				scrollTimer = null;
			}
		}


		private void Fit()
		{
			if (!fitToAll)
				return;
			if (document == null)
				return;

			Rectangle rc = ClientRectangle;
			rc.Width -= 1;
			rc.Height -= 1;
			if (rc.Width <= 0 || rc.Height <= 0)
				return;

			scaleFactor = 100;
			RectangleF expected = DocToScreen(document.DocExtents);

			float hratio = expected.Width / rc.Width;
			float vratio = expected.Height / rc.Height;
			float ratio = Math.Max(hratio, vratio);

			scaleFactor = Math.Min(100, 100 / ratio);
		}

		private void LayoutScrollbars()
		{
			if (!ShowScrollbars)
				return;

			Rectangle rc = ClientRectangle;
			if (hScroller != null)
			{
				hScroller.Location = new Point(rc.Left, rc.Bottom - hScroller.Height);
				hScroller.Width = rc.Width - vScroller.Width;
			}
			if (vScroller != null)
			{
				vScroller.Location = new Point(rc.Right - vScroller.Width, rc.Top);
				vScroller.Height = rc.Height - hScroller.Height;
			}
		}

		/// <summary>
		/// Updates scrollbars' ranges and positions.
		/// </summary>
		private void SetScrollbars()
		{
			if (document == null)
				return;
			if (!ShowScrollbars)
				return;

			Rectangle rc = ClientRectangle;
			rc.Width -= vScroller.Width;
			rc.Height -= hScroller.Height;

			// Calculate the visible overview area in document coordinates
			RectangleF rcDoc = ScreenToDoc(rc);


			// Set the scrollers
			int htotal = (int)(document.DocExtents.Width);
			int vtotal = (int)(document.DocExtents.Height);
			int hvisible = (int)rcDoc.Width;
			int vvisible = (int)rcDoc.Height;

			if (hvisible <= 0 || vvisible <= 0)
				return;

			if (hvisible > htotal + 1)
			{
				scrollX = hScroller.Value = 0;
				hScroller.Enabled = false;
			}
			if (vvisible > vtotal + 1)
			{
				scrollY = vScroller.Value = 0;
				vScroller.Enabled = false;
			}

			if (hvisible <= htotal + 1)
			{
				hScroller.Enabled = true;
				hScroller.Maximum = htotal;
				hScroller.SmallChange = hvisible / 10;
				hScroller.LargeChange = hvisible;
				if (hScroller.Value > htotal - hvisible + 1)
					scrollX = hScroller.Value = Math.Max(0, htotal - hvisible + 1);
			}
			if (vvisible <= vtotal + 1)
			{
				vScroller.Enabled = true;
				vScroller.Maximum = vtotal;
				vScroller.SmallChange = vvisible / 10;
				vScroller.LargeChange = vvisible;
				if (vScroller.Value > vtotal - vvisible + 1)
					scrollY = vScroller.Value = Math.Max(0, vtotal - vvisible + 1);
			}
		}

		/// <summary>
		/// Renders the overviewed document.
		/// </summary>
		private void DrawContents(Graphics g)
		{
			document.SetRenderOptions(options);

			// fill the background
			if (options.EnableBackground)
			{
				if (document.BkgrImagePos >= ImageAlign.Document)
				{
					// presume graphics is already set up
					Point pt = DocToScreen(g, new PointF(0, 0));
					g.RenderingOrigin = pt;

					System.Drawing.Brush fillBrush = Document.BackBrush.CreateGDIBrush(
						Document.DocExtents, -Document.DocExtents.Left, -Document.DocExtents.Top);
					g.FillRectangle(fillBrush, Document.DocExtents);
					fillBrush.Dispose();
				}
				else
				{
					System.Drawing.SolidBrush back =
						new System.Drawing.SolidBrush(Document.BackColor);
					g.FillRectangle(back, document.DocExtents);
					back.Dispose();
				}
			}

			g.SmoothingMode = smoothMode;

			// Draw background image
			if (document.BackgroundImage != null)
				if (document.BkgrImagePos >= ImageAlign.Document)
					if (options.EnableBackgroundImage)
						DrawPicture(g, document.BackgroundImage, document.DocExtents,
							(ImageAlign)(document.BkgrImagePos - ImageAlign.Document));

			// Pass for single level shadowing
			if (options.EnableShadows &&
				document.ShadowsStyle == ShadowsStyle.OneLevel)
			{
				foreach (ChartObject obj in document.Objects)
					obj.Draw(g, true);
			}

			// Pass for objects
			foreach (ChartObject obj in document.Objects)
			{
				if (options.EnableBackground &&
					Document.ShadowsStyle == ShadowsStyle.ZOrder)
					obj.Draw(g, true);

				obj.Draw(g, false);
			}

			document.ResetRenderOptions();
		}

		private void DrawPicture(Graphics g, Image pict, RectangleF rect, ImageAlign pos)
		{
			float xoff = 0, yoff = 0;
			int xc = 0, yc = 0;

			// get image logical size in document coordinates
			float docZoom = document.ZoomFactor / 100.0f;
			float picw = (float)pict.Width / pict.HorizontalResolution * g.DpiX * docZoom;
			float pich = (float)pict.Height / pict.VerticalResolution * g.DpiY * docZoom;
			PointF ptLT = document.ClientToDoc(new Point(0, 0));
			PointF ptRB = document.ClientToDoc(new Point((int)picw, (int)pich));
			picw = ptRB.X - ptLT.X;
			pich = ptRB.Y - ptLT.Y;

			switch (pos)
			{
				case ImageAlign.TopLeft:
					xoff = rect.Left;
					yoff = rect.Top;
					xc = yc = 1;
					break;
				case ImageAlign.BottomLeft:
					xoff = rect.Left;
					yoff = rect.Bottom - pich;
					xc = yc = 1;
					break;
				case ImageAlign.TopRight:
					xoff = rect.Right - picw;
					yoff = rect.Top;
					xc = yc = 1;
					break;
				case ImageAlign.BottomRight:
					xoff = rect.Right - picw;
					yoff = rect.Bottom - pich;
					xc = yc = 1;
					break;
				case ImageAlign.Center:
					xoff = (rect.Right + rect.Left - picw) / 2;
					yoff = (rect.Bottom + rect.Top - pich) / 2;
					xc = yc = 1;
					break;
				case ImageAlign.Fit:
				{
					xc = yc = 1;
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
					xc = yc = 1;
				}
					break;
				case ImageAlign.Tile:
				{
					xoff = rect.Left; yoff = rect.Top;
					xc = (int)((rect.Right - rect.Left) / picw) + 1;
					yc = (int)((rect.Bottom - rect.Top) / pich) + 1;
				}
					break;
			}

			if (pos != ImageAlign.Tile)
			{
				for (int x = 0; x < xc; x++)
					for (int y = 0; y < yc; y++)
						g.DrawImage(pict, xoff+x*picw, yoff+y*pich, picw, pich);
			}
			else
			{
				PointF ptLT2 = document.ClientToDoc(new Point(0, 0));
				PointF ptRB2 = document.ClientToDoc(new Point(
					(int)(1 + (float)Math.Ceiling((double)pict.Size.Width /
					pict.HorizontalResolution * g.DpiX * docZoom)), 
					(int)(1 + (float)Math.Ceiling((double)pict.Size.Height /
					pict.VerticalResolution * g.DpiY * docZoom))));
				float picw2 = ptRB2.X - ptLT.X;
				float pich2 = ptRB2.Y - ptLT.Y;

				for (int x = 0; x < xc; x++)
					for (int y = 0; y < yc; y++)
						g.DrawImage(pict, xoff+x*picw, yoff+y*pich, picw2, pich2);
			}
		}

		/// <summary>
		/// Calculates the current document viewport.
		/// </summary>
		private void CalcViewport()
		{
			if (document == null)
				return;

			Rectangle rc = document.ClientRectangle;
			PointF ptDocLT = document.ClientToDoc(new Point(rc.Left, rc.Top));
			PointF ptDocRB = document.ClientToDoc(new Point(rc.Right, rc.Bottom));

			// Make sure the doc viewport does not exceed the document content
			if (ptDocRB.X > document.DocExtents.Right)
				ptDocRB.X = document.DocExtents.Right;
			if (ptDocRB.Y > document.DocExtents.Bottom)
				ptDocRB.Y = document.DocExtents.Bottom;

			// Viewport in document coordinates
			rcDoc = RectangleF.FromLTRB(document.ScrollX,
				document.ScrollY, ptDocRB.X, ptDocRB.Y);
			// Viewport in client coordinates
			rcView = DocToScreen(rcDoc);
		}

		/// <summary>
		/// Calculates the offsets which would center the overview
		/// within the client area if the client area is bigger
		/// than the diagram.
		/// </summary>
		private void CalcCenteringOffsets()
		{
			if (document == null)
				return;

			Rectangle rcClient = ClientRectangle;
			if (ShowScrollbars)
			{
				rcClient.Width -= vScroller.Width;
				rcClient.Height -= hScroller.Height;
			}
			cx = cy = 0;

			Rectangle rcDiagram = DocToScreen(document.DocExtents);
			Point pt = Point.Empty;
			pt.X = (rcClient.Width - rcDiagram.Width) / 2;
			pt.Y = (rcClient.Height - rcDiagram.Height) / 2;

			RectangleF rcNew = ScreenToDoc(new Rectangle(pt, Size.Empty));
			if (rcDiagram.Width < rcClient.Width)
				cx = rcNew.X - document.DocExtents.Left;
			if (rcDiagram.Height < rcClient.Height)
				cy = rcNew.Y - document.DocExtents.Top;
		}

		/// <summary>
		/// Setup graphics transformations.
		/// </summary>
		private void SetTransforms(Graphics g)
		{
			g.ResetTransform();

//			g.ScaleTransform((float)document.ZoomFactor / 100 * scaleFactor / 100,
//				(float)document.ZoomFactor / 100 * scaleFactor / 100);
			g.TranslateTransform(cx, cy);
			g.TranslateTransform(-scrollX, -scrollY);
			g.TranslateTransform(-document.DocExtents.X, -document.DocExtents.Y);
			g.PageUnit = document.MeasureUnit;
			g.PageScale = document.ZoomFactor / 100 * scaleFactor / 100;
		}

		/// <summary>
		/// Converts overview's document's coordinates to screen coordinates.
		/// </summary>
		internal Rectangle DocToScreen(RectangleF rc)
		{
			Graphics g = CreateGraphics();
			SetTransforms(g);

			PointF[] points = new PointF[2]
				{
					new PointF(rc.Left, rc.Top), new PointF(rc.Right, rc.Bottom)
				};
			g.TransformPoints(CoordinateSpace.Device, CoordinateSpace.World, points);

			g.Dispose();

			return Rectangle.FromLTRB((int)points[0].X, (int)points[0].Y,
				(int)points[1].X, (int)points[1].Y);
		}

		/// <summary>
		/// Converts overview's document's coordinates to screen coordinates.
		/// </summary>
		internal Point DocToScreen(Graphics g, PointF pt)
		{
			PointF[] points = new PointF[1] { pt };
			g.TransformPoints(CoordinateSpace.Device, CoordinateSpace.World, points);
			return new Point((int)points[0].X, (int)points[0].Y);
		}

		/// <summary>
		/// Converts screen coordinates to overview's document's coordinates.
		/// </summary>
		internal RectangleF ScreenToDoc(Rectangle rc)
		{
			Graphics g = CreateGraphics();
			SetTransforms(g);

			PointF[] points = new PointF[2]
				{
					new PointF(rc.Left, rc.Top), new PointF(rc.Right, rc.Bottom)
				};
			g.TransformPoints(CoordinateSpace.World, CoordinateSpace.Device, points);

			g.Dispose();

			return RectangleF.FromLTRB(points[0].X, points[0].Y,
				points[1].X, points[1].Y);
		}
	}


	/// <summary>
	/// Base for all drag tools.
	/// </summary>
	internal interface IDragTool
	{
		/// <summary>
		/// Invoked to notify the drag tool of any mouse changes.
		/// </summary>
		void Update(int x, int y);

		/// <summary>
		/// Invoked when the drag operation is about to be completed.
		/// </summary>
		void Complete(int x, int y);

		/// <summary>
		/// Causes the drag operation to be cancelled.
		/// </summary>
		void Cancel();

		/// <summary>
		/// Gets the mouse cursor to be displayed while the dragging.
		/// </summary>
		Cursor Cursor { get; }
	}


	/// <summary>
	/// Handles the dragging of the viewport rectangle within the control.
	/// </summary>
	internal class ViewDrag : IDragTool
	{
		public ViewDrag(Overview parent, int mouseX, int mouseY)
		{
			this.parent = parent;
			this.x = this.initialX = mouseX;
			this.y = this.initialY = mouseY;
		}

		public virtual void Update(int x, int y)
		{
			int dx = x - this.x;
			int dy = y - this.y;

			// Adjust viewport
			Rectangle rcView = parent.Viewport;
			rcView.Offset(dx, dy);
			parent.Viewport = rcView;

			this.x = x;
			this.y = y;
		}

		public virtual void Complete(int x, int y)
		{
			Update(x, y);
		}

		public virtual void Cancel()
		{
			Update(initialX, initialY);
		}


		public virtual Cursor Cursor
		{
			get { return Cursors.Hand; }
		}


		private int initialX;
		private int initialY;
		private int x;
		private int y;
		private Overview parent;
	}
}
