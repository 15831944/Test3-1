// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace MindFusion.FlowChartX
{
	public class ShapeToolBar : System.Windows.Forms.ToolBar
	{
		private FlowChart document;

		private Color shapeFillColor;
		private ImageList images;
		private bool showText;
		private bool showImages;
		private bool applyToSelection;

		private bool allowDrag;
		private Size defaultBoxSize;
		private Size imageSize;


		public ShapeToolBar()
		{
			document				= null;

			shapeFillColor			= Color.FromArgb(150, 180, 210);
			images					= new ImageList();
			showImages				= true;
			showText				= false;
			applyToSelection		= true;

			allowDrag				= false;
			defaultBoxSize			= new Size(20, 20);
			imageSize				= new Size(16, 16);

			ButtonClick  += new ToolBarButtonClickEventHandler(OnButtonClick);

			shapes = null;
		}

		~ShapeToolBar()
		{
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
				Document = null;

			base.Dispose (disposing);
		}


		// Event handlers
		private DragEventHandler documentDragOver = null;
		private DragEventHandler documentDragDrop = null;
		private EventHandler shapeTemplateChanged = null;
		private EventHandler boxStyleChanged = null;
		private EventHandler shapeAdded = null;


		public MindFusion.FlowChartX.FlowChart Document
		{
			get
			{ 
				return document;
			}
			set
			{
				if (document != null)
				{
					document.DragOver -= documentDragOver;
					document.DragDrop -= documentDragDrop;
					document.ShapeTemplateChanged -= shapeTemplateChanged;
					document.BoxStyleChanged -= boxStyleChanged;
					ShapeTemplate.ShapeAdded -= shapeAdded;
				}

				document = value;

				if (document != null)
				{
					rebuild();

                    documentDragOver = new DragEventHandler(OnDocumentDragOver);
                    documentDragDrop = new DragEventHandler(OnDocumentDragDrop);
					shapeTemplateChanged = new EventHandler(OnShapeTemplateChanged);
					boxStyleChanged = new EventHandler(OnBoxStyleChanged);
					shapeAdded = new EventHandler(OnShapeAdded);

					document.DragOver += documentDragOver;
					document.DragDrop += documentDragDrop;
					document.ShapeTemplateChanged += shapeTemplateChanged;
					document.BoxStyleChanged += boxStyleChanged;
					ShapeTemplate.ShapeAdded += shapeAdded;
				}
				else
				{
					base.Buttons.Clear();
					base.ImageList =  null;
				}
			}
		}

		[Browsable(false)]
		public new System.Windows.Forms.ToolBar.ToolBarButtonCollection Buttons
		{
			get { return new ToolBarButtonCollection(this); }
		}

        [Browsable(false)]
        public new System.Windows.Forms.ImageList ImageList
        {
            get { return null; }
            set { }
        }

        [Category("Appearance")]
		[DefaultValue(false)]
        public bool ShowText
		{
			get { return showText; }
			set
			{
				if (showText == value)
					return;

				showText = value;
				ToggleButtonText();
			}
		}

        [Category("Appearance")]
		[DefaultValue(true)]
		public bool ShowImages
		{
			get { return showImages; }
			set
			{
				if (showImages == value)
					return;

				showImages = value;
                ToggleImageList();
			}
		}

		[Browsable(false)]
		public new bool AllowDrop
		{
			get { return false; }
			set { }
		}

        [Category("Behavior")]
		[DefaultValue(false)]
		public bool AllowDrag
		{
			get { return allowDrag; }
			set { allowDrag = value; }
		}

        [Category("Misc")]
		[DefaultValue(typeof(Size), "20, 20")]
		public Size DefaultBoxSize
		{
			get { return defaultBoxSize; }
			set { defaultBoxSize = value; }
		}

        [Category("Behavior")]
		[DefaultValue(true)]
		public bool ApplyToSelection
		{
			get
			{
				return applyToSelection;
			}
			set
			{
				applyToSelection = value;
			}
		}

        [Category("Appearance")]
		[DefaultValue(typeof(Size), "16, 16")]
		public Size ButtonImageSize
		{
			get { return imageSize; }
			set
			{
				if (imageSize.Equals(value))
					return;

				imageSize = value;

				// Recreate the image list
				CreateImages();
				ToggleImageList();
			}
		}

        [Category("Behavior")]
		[DefaultValue(typeof(Color), "150, 180, 210")]
		public Color ShapeFillColor
		{
			get { return shapeFillColor; }
			set
			{
				shapeFillColor = value;

				// Recreate the image list
				CreateImages();
				ToggleImageList();
			}
		}


		protected override void OnMouseDown(System.Windows.Forms.MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Right && allowDrag)
			{
				foreach (ToolBarButton button in base.Buttons)
				{
					Rectangle rect = button.Rectangle;
					if (e.X > rect.Left && e.X < rect.Right &&
						e.Y > rect.Top && e.Y < rect.Bottom)
					{
						DoDragDrop(button.Tag, DragDropEffects.Copy);
						return;
					}
				}
			}

			base.OnMouseDown(e);
		}
		
		private void OnDocumentDragOver(object sender, System.Windows.Forms.DragEventArgs e)
        {
			if (e.Data.GetDataPresent(typeof(MindFusion.FlowChartX.ShapeTemplate)))
				e.Effect = DragDropEffects.Copy;
			else
				e.Effect = DragDropEffects.None;
        }

        private void OnDocumentDragDrop(object sender, System.Windows.Forms.DragEventArgs e)
        {
			if (!e.Data.GetDataPresent(typeof(ShapeTemplate)))
				return;
			if (document == null)
				return;

			ShapeTemplate template = (ShapeTemplate)e.Data.GetData(typeof(ShapeTemplate));

            Point ptClient = document.PointToClient(new Point(e.X, e.Y));
			PointF ptDoc = document.ClientToDoc(ptClient);
			Box box = document.CreateBox(ptDoc.X, ptDoc.Y,
				defaultBoxSize.Width, defaultBoxSize.Height);
			box.Style = BoxStyle.Shape;
			box.Shape = template;
        }

		private void OnShapeTemplateChanged(object sender, System.EventArgs e)
		{
			UpdateDefaultButton();
		}

		private void OnBoxStyleChanged(object sender, System.EventArgs e)
		{
			UpdateDefaultButton();
		}

		private void OnShapeAdded(object sender, System.EventArgs e)
		{
			rebuild();
		}

		private void OnButtonClick(object sender, System.Windows.Forms.ToolBarButtonClickEventArgs e)
		{
			SuspendLayout();
			// Reset all buttons
			foreach (ToolBarButton button in base.Buttons)
			{
				if (button == e.Button)
					button.Pushed = true;
				else if (button.Pushed)
					button.Pushed = false;
			}
			ResumeLayout();

			Document.BoxStyle = BoxStyle.Shape;
			Document.DefaultShape = (ShapeTemplate)e.Button.Tag;

			if (applyToSelection)
			{
				foreach (Box box in document.Selection.Boxes)
				{
					box.Style = BoxStyle.Shape;
					box.Shape = (ShapeTemplate)e.Button.Tag;
				}
			}
		}


		/// <summary>
		/// Updates the toolbar buttons depending on the current document.
		/// </summary>
		private void CreateButtons()
		{
			base.Buttons.Clear();

			if (document == null)
				return;

			int index = 0;

			if (shapes == null || shapes.Length == 0)
			{
				// show all shapes
				foreach (ShapeTemplate template in ShapeTemplate.Shapes)
					createButton(template, index++);
			}
			else
			{
				// show specified shapes
				foreach (string id in shapes)
					createButton(ShapeTemplate.FromId(id), index++);
			}
		}

		/// <summary>
		/// creates a new button
		/// </summary>
		private void createButton(ShapeTemplate template, int index)
		{
			if (template == null)
				return;

			ToolBarButton button = null;
			if (showText)
				button = new ToolBarButton(template.Id);
			else
				button = new ToolBarButton("");
			button.Tag = template;
			button.ImageIndex = index;
			button.ToolTipText = template.Id;
			base.Buttons.Add(button);

			// Set as default
			if (document.DefaultShape == template &&
				document.BoxStyle == BoxStyle.Shape)
				button.Pushed = true;
		}

		/// <summary>
		/// Updates the default button.
		/// </summary>
		private void UpdateDefaultButton()
		{
			foreach (ToolBarButton button in base.Buttons)
			{
				if (document.DefaultShape == (ShapeTemplate)button.Tag &&
					document.BoxStyle == BoxStyle.Shape)
					button.Pushed = true;
				else if (button.Pushed)
					button.Pushed = false;
			}
		}

		/// <summary>
		/// Creates the image list with shape images using
		/// the current control settings.
		/// </summary>
		private void CreateImages()
		{
			this.ImageList = null;

			// Create the image list
			images = new ImageList();
			images.ImageSize = imageSize;
			images.TransparentColor = SystemColors.Control;

			// Create a temporary hidden flowchart object
			FlowChart chart = new FlowChart();
			chart.MeasureUnit = GraphicsUnit.Pixel;
			chart.DrawBox += this.DrawBox;

			System.Drawing.Brush back =
				new System.Drawing.SolidBrush(SystemColors.Control);

			// Populate the image list
			if (shapes == null || shapes.Length == 0)
			{
				// show all shapes
				foreach (ShapeTemplate template in ShapeTemplate.Shapes)
					createImage(template, back, chart);
			}
			else
			{
				// show specified shapes
				foreach (string id in shapes)
					createImage(ShapeTemplate.FromId(id), back, chart);
			}

			back.Dispose();
			chart.Dispose();
		}

		/// <summary>
		/// create an image for the given shape
		/// </summary>
		private void createImage(ShapeTemplate template,
			System.Drawing.Brush brush, FlowChart chart)
		{
			if (template == null)
				return;

			Box b = chart.CreateBox(1, 1, imageSize.Width - 4, imageSize.Height - 4);
			b.Style = BoxStyle.Shape;
			b.Shape = template;
			b.PenWidth = 0;
			b.ShadowColor = Color.FromArgb(40, Color.Black);
			b.ShadowOffsetX = 1;
			b.ShadowOffsetY = 1;
			b.FillColor  = ShapeFillColor;
			b.CustomDraw = CustomDraw.Additional;

			Bitmap image = new Bitmap(imageSize.Width, imageSize.Height);
			Graphics g = Graphics.FromImage(image);
			g.SmoothingMode = SmoothingMode.AntiAlias;
			g.FillRectangle(brush, -1, -1, imageSize.Width + 1, imageSize.Height + 1);

			b.Draw(g, true);
			b.Draw(g, false);

			images.Images.Add(image);

			g.Dispose();
		}

		/// <summary>
		/// Toggles button texts.
		/// </summary>
		private void ToggleButtonText()
		{
			SuspendLayout();
			foreach (ToolBarButton button in base.Buttons)
			{
				if (showText)
					button.Text = ((ShapeTemplate)button.Tag).Id;
				else
					button.Text = "";
			}
			ResumeLayout();
		}

		/// <summary>
		/// Toggles button images.
		/// </summary>
		private void ToggleImageList()
		{
			if (showImages)
				base.ImageList = images;
			else
				base.ImageList = null;
		}

		/// <summary>
		/// recreates the toolbar buttons
		/// </summary>
		private void rebuild()
		{
			CreateImages();
			ToggleImageList();
			CreateButtons();
		}

		/// <summary>
		/// get or sets the collection of shape identifiers be used with the toolbar
		/// </summary>
		[Description("Specifies what node shapes should be displayed in the toolbar.")]
		[Editor("StringArrayEditor", "UITypeEditor")]
		[TypeConverter("ArrayConverter")]
		public string[] Shapes
		{
			get { return shapes; }
			set
			{
				shapes = value;
				rebuild();
			}
		}

		public event BoxCustomDraw DrawBox;

		/// <summary>
		/// a collection of identifiers of the shapes to be used with the toolbar
		/// </summary>
		private string[] shapes;

		public void RecreateImages()
		{
			rebuild();
		}
	}
}