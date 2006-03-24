using System;
using System.Drawing;
using System.Drawing.Printing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Implements an extended print preview form.
	/// </summary>
	internal class FrmPrintPreview : System.Windows.Forms.Form
	{
		private System.Windows.Forms.ImageList ToolbarImages;
		private System.Windows.Forms.MenuItem Menu25;
		private System.Windows.Forms.MenuItem Menu50;
		private System.Windows.Forms.MenuItem Menu10;
		private System.Windows.Forms.MenuItem Menu100;
		private System.Windows.Forms.ContextMenu ZoomMenu;
		private System.Windows.Forms.MenuItem MenuAuto;
		private System.Windows.Forms.MenuItem Menu500;
		private System.Windows.Forms.MenuItem Menu200;
		private System.Windows.Forms.MenuItem Menu150;
		private System.Windows.Forms.MenuItem Menu75;
		private System.Windows.Forms.Panel PrintToolbar;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.ToolBar toolBarMain;
		private System.Windows.Forms.TextBox PageRange;
		private System.Windows.Forms.ToolBarButton toolBarBtnActPrint;
		private System.Windows.Forms.ToolBarButton toolBarBtnActZoomAuto;
		private System.Windows.Forms.ToolBarButton toolBarButton2;
		private System.Windows.Forms.ToolBarButton toolBarBtnOnePage;
		private System.Windows.Forms.ToolBarButton toolBarBtnTwoPages;
		private System.Windows.Forms.ToolBarButton toolBarBtnThreePages;
		private System.Windows.Forms.ToolBarButton toolBarBtnFourPages;
		private System.Windows.Forms.ToolBarButton toolBarBtnSixPages;
		private System.Windows.Forms.ToolBarButton toolBarButton8;
		private System.Windows.Forms.ToolBarButton toolBarBtnFirstPage;
		private System.Windows.Forms.ToolBarButton toolBarBtnPrevPage;
		private System.Windows.Forms.ToolBarButton toolBarBtnNextPage;
		private System.Windows.Forms.ToolBarButton toolBarBtnLastPage;
		private System.Windows.Forms.ToolBarButton toolBarButton13;
		private System.Windows.Forms.ToolBarButton toolBarBtnOrientation;
		private FlowChart flowChart;
		private string DocumentName;
		private bool Landscape;

        private const int PRINT_IMAGE           = 0;
        private const int ZOOM_IMAGE            = 1;
        private const int ONE_PAGE_IMAGE        = 2;
        private const int TWO_PAGES_IMAGE       = 3;
        private const int THREE_PAGES_IMAGE     = 4;
        private const int FOUR_PAGES_IMAGE      = 5;
        private const int SIX_PAGES_IMAGE       = 6;
        private const int FIRST_PAGE_IMAGE      = 7;
        private const int PREVIOUS_PAGE_IMAGE   = 8;
        private const int NEXT_PAGE_IMAGE       = 9;
        private const int LAST_PAGE_IMAGE       = 10;
        private const int FITALL_IMAGE          = 13;
        private const int PORTARAIT_IMAGE       = 11;
		private const int LANDSCAPE_IMAGE       = 12;
		private System.Windows.Forms.ToolBarButton toolBarButtonDummy;
		private System.Windows.Forms.ContextMenu scaleMenu;
		private System.Windows.Forms.MenuItem miScaleToPage;
		private System.Windows.Forms.MenuItem miOriginalScale;
		private System.Windows.Forms.MenuItem miIncreaseScale;
		private System.Windows.Forms.MenuItem miDecreaseScale;
		private System.Windows.Forms.ToolBarButton toolBarBtnScale;
		private System.Windows.Forms.MenuItem miDoubleScale;
		private System.Windows.Forms.MenuItem miHalveSpace;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.ContextMenu printerMenu;
		private System.Windows.Forms.PrintPreviewControl PreviewArea;

		public FrmPrintPreview(FlowChart fc, string docName)
		{
			InitializeComponent();

            Landscape = false;
            flowChart = fc;
            DocumentName = docName;

            toolBarBtnActPrint.ToolTipText		= "Print";
			toolBarBtnActZoomAuto.ToolTipText	= "Zoom";
			toolBarBtnOnePage.ToolTipText		= "One Page";
			toolBarBtnTwoPages.ToolTipText		= "Two Pages";
			toolBarBtnThreePages.ToolTipText	= "Three Pages";
			toolBarBtnFourPages.ToolTipText		= "Four Pages";
			toolBarBtnSixPages.ToolTipText		= "Six Pages";
			toolBarBtnFirstPage.ToolTipText		= "First Page";
			toolBarBtnPrevPage.ToolTipText		= "Previous Page";
			toolBarBtnNextPage.ToolTipText		= "Next Page";
			toolBarBtnLastPage.ToolTipText		= "Last Page";
			toolBarBtnScale.ToolTipText		= "Fit to Page";
			toolBarBtnOrientation.ToolTipText	= "Landscape";

			fitToPage = false;
			orgScaleFactor = flowChart.PrintOptions.Scale;
			scaleChange = flowChart.PrintOptions.Scale / 5;

			// add the installed printers to the printers menu
			EventHandler printerHandler = new EventHandler(miPrinterMenu_Click);
			foreach (String printerName in PrinterSettings.InstalledPrinters)
				printerMenu.MenuItems.Add(printerName, printerHandler);
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(FrmPrintPreview));
			this.ToolbarImages = new System.Windows.Forms.ImageList(this.components);
			this.Menu25 = new System.Windows.Forms.MenuItem();
			this.Menu50 = new System.Windows.Forms.MenuItem();
			this.Menu10 = new System.Windows.Forms.MenuItem();
			this.Menu100 = new System.Windows.Forms.MenuItem();
			this.ZoomMenu = new System.Windows.Forms.ContextMenu();
			this.MenuAuto = new System.Windows.Forms.MenuItem();
			this.Menu500 = new System.Windows.Forms.MenuItem();
			this.Menu200 = new System.Windows.Forms.MenuItem();
			this.Menu150 = new System.Windows.Forms.MenuItem();
			this.Menu75 = new System.Windows.Forms.MenuItem();
			this.PrintToolbar = new System.Windows.Forms.Panel();
			this.PageRange = new System.Windows.Forms.TextBox();
			this.toolBarMain = new System.Windows.Forms.ToolBar();
			this.toolBarBtnActPrint = new System.Windows.Forms.ToolBarButton();
			this.printerMenu = new System.Windows.Forms.ContextMenu();
			this.toolBarBtnActZoomAuto = new System.Windows.Forms.ToolBarButton();
			this.toolBarButton2 = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnOnePage = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnTwoPages = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnThreePages = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnFourPages = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnSixPages = new System.Windows.Forms.ToolBarButton();
			this.toolBarButton8 = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnFirstPage = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnPrevPage = new System.Windows.Forms.ToolBarButton();
			this.toolBarButtonDummy = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnNextPage = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnLastPage = new System.Windows.Forms.ToolBarButton();
			this.toolBarButton13 = new System.Windows.Forms.ToolBarButton();
			this.toolBarBtnScale = new System.Windows.Forms.ToolBarButton();
			this.scaleMenu = new System.Windows.Forms.ContextMenu();
			this.miScaleToPage = new System.Windows.Forms.MenuItem();
			this.miOriginalScale = new System.Windows.Forms.MenuItem();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.miDoubleScale = new System.Windows.Forms.MenuItem();
			this.miHalveSpace = new System.Windows.Forms.MenuItem();
			this.miIncreaseScale = new System.Windows.Forms.MenuItem();
			this.miDecreaseScale = new System.Windows.Forms.MenuItem();
			this.toolBarBtnOrientation = new System.Windows.Forms.ToolBarButton();
			this.PreviewArea = new System.Windows.Forms.PrintPreviewControl();
			this.PrintToolbar.SuspendLayout();
			this.SuspendLayout();
			// 
			// ToolbarImages
			// 
			this.ToolbarImages.ColorDepth = System.Windows.Forms.ColorDepth.Depth24Bit;
			this.ToolbarImages.ImageSize = new System.Drawing.Size(25, 25);
			this.ToolbarImages.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ToolbarImages.ImageStream")));
			this.ToolbarImages.TransparentColor = System.Drawing.Color.FromArgb(((System.Byte)(212)), ((System.Byte)(208)), ((System.Byte)(200)));
			// 
			// Menu25
			// 
			this.Menu25.Index = 7;
			this.Menu25.Text = "25%";
			this.Menu25.Click += new System.EventHandler(this.Menu25_Click);
			// 
			// Menu50
			// 
			this.Menu50.Index = 6;
			this.Menu50.Text = "50%";
			this.Menu50.Click += new System.EventHandler(this.Menu50_Click);
			// 
			// Menu10
			// 
			this.Menu10.Index = 8;
			this.Menu10.Text = "10%";
			this.Menu10.Click += new System.EventHandler(this.Menu10_Click);
			// 
			// Menu100
			// 
			this.Menu100.Index = 4;
			this.Menu100.Text = "100%";
			this.Menu100.Click += new System.EventHandler(this.Menu100_Click);
			// 
			// ZoomMenu
			// 
			this.ZoomMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.MenuAuto,
																					 this.Menu500,
																					 this.Menu200,
																					 this.Menu150,
																					 this.Menu100,
																					 this.Menu75,
																					 this.Menu50,
																					 this.Menu25,
																					 this.Menu10});
			// 
			// MenuAuto
			// 
			this.MenuAuto.Checked = true;
			this.MenuAuto.Index = 0;
			this.MenuAuto.Text = "Auto";
			this.MenuAuto.Click += new System.EventHandler(this.MenuAuto_Click);
			// 
			// Menu500
			// 
			this.Menu500.Index = 1;
			this.Menu500.Text = "500%";
			this.Menu500.Click += new System.EventHandler(this.Menu500_Click);
			// 
			// Menu200
			// 
			this.Menu200.Index = 2;
			this.Menu200.Text = "200%";
			this.Menu200.Click += new System.EventHandler(this.Menu200_Click);
			// 
			// Menu150
			// 
			this.Menu150.Index = 3;
			this.Menu150.Text = "150%";
			this.Menu150.Click += new System.EventHandler(this.Menu150_Click);
			// 
			// Menu75
			// 
			this.Menu75.Index = 5;
			this.Menu75.Text = "75%";
			this.Menu75.Click += new System.EventHandler(this.Menu75_Click);
			// 
			// PrintToolbar
			// 
			this.PrintToolbar.Controls.AddRange(new System.Windows.Forms.Control[] {
																					   this.PageRange,
																					   this.toolBarMain});
			this.PrintToolbar.Dock = System.Windows.Forms.DockStyle.Top;
			this.PrintToolbar.Name = "PrintToolbar";
			this.PrintToolbar.Size = new System.Drawing.Size(640, 34);
			this.PrintToolbar.TabIndex = 6;
			// 
			// PageRange
			// 
			this.PageRange.BackColor = System.Drawing.SystemColors.Control;
			this.PageRange.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.PageRange.Enabled = false;
			this.PageRange.Location = new System.Drawing.Point(336, 8);
			this.PageRange.Name = "PageRange";
			this.PageRange.Size = new System.Drawing.Size(24, 20);
			this.PageRange.TabIndex = 5;
			this.PageRange.Text = "1";
			this.PageRange.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// toolBarMain
			// 
			this.toolBarMain.Appearance = System.Windows.Forms.ToolBarAppearance.Flat;
			this.toolBarMain.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
																						   this.toolBarBtnActPrint,
																						   this.toolBarBtnActZoomAuto,
																						   this.toolBarButton2,
																						   this.toolBarBtnOnePage,
																						   this.toolBarBtnTwoPages,
																						   this.toolBarBtnThreePages,
																						   this.toolBarBtnFourPages,
																						   this.toolBarBtnSixPages,
																						   this.toolBarButton8,
																						   this.toolBarBtnFirstPage,
																						   this.toolBarBtnPrevPage,
																						   this.toolBarButtonDummy,
																						   this.toolBarBtnNextPage,
																						   this.toolBarBtnLastPage,
																						   this.toolBarButton13,
																						   this.toolBarBtnScale,
																						   this.toolBarBtnOrientation});
			this.toolBarMain.ButtonSize = new System.Drawing.Size(26, 26);
			this.toolBarMain.DropDownArrows = true;
			this.toolBarMain.ImageList = this.ToolbarImages;
			this.toolBarMain.Name = "toolBarMain";
			this.toolBarMain.ShowToolTips = true;
			this.toolBarMain.Size = new System.Drawing.Size(640, 34);
			this.toolBarMain.TabIndex = 0;
			this.toolBarMain.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.toolBarMain_ButtonClick);
			// 
			// toolBarBtnActPrint
			// 
			this.toolBarBtnActPrint.DropDownMenu = this.printerMenu;
			this.toolBarBtnActPrint.ImageIndex = 0;
			this.toolBarBtnActPrint.Style = System.Windows.Forms.ToolBarButtonStyle.DropDownButton;
			// 
			// toolBarBtnActZoomAuto
			// 
			this.toolBarBtnActZoomAuto.DropDownMenu = this.ZoomMenu;
			this.toolBarBtnActZoomAuto.ImageIndex = 1;
			this.toolBarBtnActZoomAuto.Style = System.Windows.Forms.ToolBarButtonStyle.DropDownButton;
			// 
			// toolBarButton2
			// 
			this.toolBarButton2.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
			// 
			// toolBarBtnOnePage
			// 
			this.toolBarBtnOnePage.ImageIndex = 2;
			// 
			// toolBarBtnTwoPages
			// 
			this.toolBarBtnTwoPages.ImageIndex = 3;
			// 
			// toolBarBtnThreePages
			// 
			this.toolBarBtnThreePages.ImageIndex = 4;
			// 
			// toolBarBtnFourPages
			// 
			this.toolBarBtnFourPages.ImageIndex = 5;
			// 
			// toolBarBtnSixPages
			// 
			this.toolBarBtnSixPages.ImageIndex = 6;
			// 
			// toolBarButton8
			// 
			this.toolBarButton8.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
			// 
			// toolBarBtnFirstPage
			// 
			this.toolBarBtnFirstPage.ImageIndex = 7;
			// 
			// toolBarBtnPrevPage
			// 
			this.toolBarBtnPrevPage.ImageIndex = 8;
			// 
			// toolBarButtonDummy
			// 
			this.toolBarButtonDummy.Enabled = false;
			// 
			// toolBarBtnNextPage
			// 
			this.toolBarBtnNextPage.ImageIndex = 9;
			// 
			// toolBarBtnLastPage
			// 
			this.toolBarBtnLastPage.ImageIndex = 10;
			// 
			// toolBarButton13
			// 
			this.toolBarButton13.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
			// 
			// toolBarBtnScale
			// 
			this.toolBarBtnScale.DropDownMenu = this.scaleMenu;
			this.toolBarBtnScale.ImageIndex = 13;
			this.toolBarBtnScale.Style = System.Windows.Forms.ToolBarButtonStyle.DropDownButton;
			// 
			// scaleMenu
			// 
			this.scaleMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.miScaleToPage,
																					  this.miOriginalScale,
																					  this.menuItem1,
																					  this.miDoubleScale,
																					  this.miHalveSpace,
																					  this.miIncreaseScale,
																					  this.miDecreaseScale});
			// 
			// miScaleToPage
			// 
			this.miScaleToPage.Index = 0;
			this.miScaleToPage.Text = "Fit to Page";
			this.miScaleToPage.Click += new System.EventHandler(this.miScaleToPage_Click);
			// 
			// miOriginalScale
			// 
			this.miOriginalScale.Index = 1;
			this.miOriginalScale.Text = "Original Scale";
			this.miOriginalScale.Click += new System.EventHandler(this.miOriginalScale_Click);
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 2;
			this.menuItem1.Text = "-";
			// 
			// miDoubleScale
			// 
			this.miDoubleScale.Index = 3;
			this.miDoubleScale.Text = "Double Scale";
			this.miDoubleScale.Click += new System.EventHandler(this.miDoubleScale_Click);
			// 
			// miHalveSpace
			// 
			this.miHalveSpace.Index = 4;
			this.miHalveSpace.Text = "Halve Scale";
			this.miHalveSpace.Click += new System.EventHandler(this.miHalveSpace_Click);
			// 
			// miIncreaseScale
			// 
			this.miIncreaseScale.Index = 5;
			this.miIncreaseScale.Text = "Increase Scale";
			this.miIncreaseScale.Click += new System.EventHandler(this.miIncreaseScale_Click);
			// 
			// miDecreaseScale
			// 
			this.miDecreaseScale.Index = 6;
			this.miDecreaseScale.Text = "Decrease Scale";
			this.miDecreaseScale.Click += new System.EventHandler(this.miDecreaseScale_Click);
			// 
			// toolBarBtnOrientation
			// 
			this.toolBarBtnOrientation.ImageIndex = 12;
			// 
			// PreviewArea
			// 
			this.PreviewArea.AutoZoom = false;
			this.PreviewArea.Dock = System.Windows.Forms.DockStyle.Fill;
			this.PreviewArea.Location = new System.Drawing.Point(0, 34);
			this.PreviewArea.Name = "PreviewArea";
			this.PreviewArea.Size = new System.Drawing.Size(640, 388);
			this.PreviewArea.TabIndex = 12;
			this.PreviewArea.Zoom = 1;
			// 
			// FrmPrintPreview
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(640, 422);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.PreviewArea,
																		  this.PrintToolbar});
			this.MinimumSize = new System.Drawing.Size(584, 392);
			this.Name = "FrmPrintPreview";
			this.Text = "1";
			this.Load += new System.EventHandler(this.FrmPrintPreview_Load);
			this.Closed += new System.EventHandler(this.FrmPrintPreview_Closed);
			this.PrintToolbar.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

        // ----- Gets or sets the internal version of the document.
        public System.Drawing.Printing.PrintDocument Document
        {
            get { return PreviewArea.Document; }
            set
			{
				PreviewArea.Document = value;
				if (value != null)
				{
					foreach (MenuItem item in printerMenu.MenuItems)
						item.Checked = item.Text == value.PrinterSettings.PrinterName;
				}
			}
        }

        // ----- Gets or sets the anti-alias flag on the preview control.    
        public bool UseAntiAlias
        {
            get { return PreviewArea.UseAntiAlias; }
            set { PreviewArea.UseAntiAlias = value; }
        }

        // Remove the check from all menus.
        private void ZoomDisplay(double zoomFactor, System.Windows.Forms.MenuItem selectedMenu)
        {
            MenuAuto.Checked    = false;
            Menu500.Checked     = false;
            Menu200.Checked     = false;
            Menu150.Checked     = false;
            Menu100.Checked     = false;
            Menu75.Checked      = false;
            Menu50.Checked      = false;
            Menu25.Checked      = false;
            Menu10.Checked      = false;

            // Check the selected menu.
            selectedMenu.Checked = true;

            // Set the auto-zoom as needed.
            PreviewArea.AutoZoom = (zoomFactor == 0.0);

            // Zoom the display.
            if (zoomFactor > 0.0)
                PreviewArea.Zoom = zoomFactor;
        }

        private void Menu10_Click(object sender, System.EventArgs e)
        {
			// Zoom the preview area to 10%.
            ZoomDisplay(0.1, Menu10);
        }

        private void Menu25_Click(object sender, System.EventArgs e)
        {
			// Zoom the preview area to 25%.
            ZoomDisplay(0.25, Menu25);
        }

        private void Menu50_Click(object sender, System.EventArgs e)
        {
            // Zoom the preview area to 50%.
            ZoomDisplay(0.5, Menu50);
        }

        private void Menu75_Click(object sender, System.EventArgs e)
        {
            // Zoom the preview area to 75%.
            ZoomDisplay(0.75, Menu75);
        }

        private void Menu100_Click(object sender, System.EventArgs e)
        {
            // Zoom the preview area to 100%.
            ZoomDisplay(1.0, Menu100);
        }

        private void Menu150_Click(object sender, System.EventArgs e)
        {
            // Zoom the preview area to 150%.
            ZoomDisplay(1.5, Menu150);
        }

        private void Menu200_Click(object sender, System.EventArgs e)
        {
            // Zoom the preview area to 200%.
            ZoomDisplay(2.0, Menu200);
        }

        private void Menu500_Click(object sender, System.EventArgs e)
        {
            // Zoom the preview area to 500%.
            ZoomDisplay(5.0, Menu500);
        }

        private void MenuAuto_Click(object sender, System.EventArgs e)
        {
            // Auto-size the preview area.
            ZoomDisplay(0.0, MenuAuto);
        }

		private void toggleOrientation()
		{
			PrintDocument doc = Document;
			Landscape = !Landscape;
			doc.DefaultPageSettings.Landscape = Landscape;

			if (fitToPage)
			{
				flowChart.PrintOptions.ScaleToPage(doc);
				scaleChange = flowChart.PrintOptions.Scale / 5;
			}

			if (Landscape)
			{
				toolBarBtnOrientation.ToolTipText = previewOptions.PortraitToolTip;
				toolBarBtnOrientation.ImageIndex  = PORTARAIT_IMAGE;
			}
			else
			{
				toolBarBtnOrientation.ToolTipText = previewOptions.LandscapeToolTip;
				toolBarBtnOrientation.ImageIndex  = LANDSCAPE_IMAGE;
			}

			resetPreviewArea();
		}

		private void toggleFitToPage()
		{
			PrintDocument doc = Document;

			if (!fitToPage)
				flowChart.PrintOptions.ScaleToPage(doc);
			else
				flowChart.PrintOptions.Scale = orgScaleFactor;
			scaleChange = flowChart.PrintOptions.Scale / 5;

			fitToPage = !fitToPage;
			toolBarBtnScale.Pushed = fitToPage;

			resetPreviewArea();
		}

		private void toolBarMain_ButtonClick(object sender, System.Windows.Forms.ToolBarButtonClickEventArgs e)
		{
			if (e.Button == toolBarBtnActPrint)
			{
				if (previewOptions.DisplayPrintDialog)
				{
					PrintDialog dlg = new PrintDialog();
					dlg.AllowPrintToFile = false;
					dlg.AllowSomePages = true;
					dlg.Document = PreviewArea.Document;
					dlg.Document.PrinterSettings.MinimumPage = 1;
					dlg.Document.PrinterSettings.MaximumPage = 10000;
					dlg.Document.PrinterSettings.FromPage = 1;
					dlg.Document.PrinterSettings.ToPage = 100;

					if (dlg.ShowDialog(this) == DialogResult.OK)
						PreviewArea.Document.Print();
				}
				else
				{
					PreviewArea.Document.Print();
				}
			}

			if (e.Button == toolBarBtnActZoomAuto)
			{
				ZoomDisplay(0.0, MenuAuto);
			}

			if (e.Button == toolBarBtnOnePage)
			{
				PreviewArea.Columns = 1;
				PreviewArea.Rows = 1;
			}

			if (e.Button == toolBarBtnTwoPages)
			{
				PreviewArea.Columns = 2;
				PreviewArea.Rows = 1;
			}

			if (e.Button == toolBarBtnThreePages)
			{
				PreviewArea.Columns = 3;
				PreviewArea.Rows = 1;
			}

			if (e.Button == toolBarBtnFourPages)
			{
				PreviewArea.Columns = 2;
				PreviewArea.Rows = 2;
			}

			if (e.Button == toolBarBtnSixPages)
			{
				PreviewArea.Columns = 3;
				PreviewArea.Rows = 2;
			}

			if (e.Button == toolBarBtnFirstPage)
			{
				PreviewArea.StartPage = 0;
				PageRange.Text = (PreviewArea.StartPage + 1).ToString();
			}

			if (e.Button == toolBarBtnPrevPage)
			{
				PreviewArea.StartPage -= 1;
				PageRange.Text = (PreviewArea.StartPage + 1).ToString();
			}

			if (e.Button == toolBarBtnNextPage)
			{
				PreviewArea.StartPage += 1;
				PageRange.Text = (PreviewArea.StartPage + 1).ToString();
			}

			if (e.Button == toolBarBtnLastPage)
			{
				PreviewArea.StartPage = int.MaxValue;
				PageRange.Text = (PreviewArea.StartPage + 1).ToString();
			}

			if (e.Button == toolBarBtnScale)
			{
                toggleFitToPage();
			}

			if (e.Button == toolBarBtnOrientation)
			{
				toggleOrientation();
			}
		}

		private void FrmPrintPreview_Load(object sender, System.EventArgs e)
		{
			ZoomDisplay(0.0, MenuAuto);
		}

		private bool fitToPage = false;
		private float orgScaleFactor;

		private void FrmPrintPreview_Closed(object sender, System.EventArgs e)
		{
			if (flowChart.PrintOptions.Scale != orgScaleFactor)
				flowChart.PrintOptions.Scale = orgScaleFactor;
		}

		private void miScaleToPage_Click(object sender, System.EventArgs e)
		{
			PrintDocument doc = Document;

			flowChart.PrintOptions.ScaleToPage(doc);
			scaleChange = flowChart.PrintOptions.Scale / 5;
			fitToPage = true;
			toolBarBtnScale.Pushed = fitToPage;

			resetPreviewArea();
		}

		private void miOriginalScale_Click(object sender, System.EventArgs e)
		{
			PrintDocument doc = Document;

			flowChart.PrintOptions.Scale = orgScaleFactor;
			scaleChange = flowChart.PrintOptions.Scale / 5;
			fitToPage = false;
			toolBarBtnScale.Pushed = fitToPage;

			resetPreviewArea();
		}

		private void miIncreaseScale_Click(object sender, System.EventArgs e)
		{
			PrintDocument doc = Document;

			flowChart.PrintOptions.Scale += scaleChange;
			fitToPage = false;
			toolBarBtnScale.Pushed = fitToPage;

			resetPreviewArea();
		}

		private void miDecreaseScale_Click(object sender, System.EventArgs e)
		{
			if (flowChart.PrintOptions.Scale <= scaleChange)
				return;

			PrintDocument doc = Document;

			flowChart.PrintOptions.Scale -= scaleChange;
			fitToPage = false;
			toolBarBtnScale.Pushed = fitToPage;

			resetPreviewArea();
		}

		/// <summary>
		/// Initialize preview dialog properties using the passed PreviewOptions.
		/// </summary>
		internal void init(PreviewOptions options)
		{
			Icon = options.TitleBarIcon;
			toolBarMain.ImageList = (options.ButtonImages != null) ?
				options.ButtonImages : ToolbarImages;

			toolBarBtnActPrint.ToolTipText = options.PrintToolTip;
			toolBarBtnActZoomAuto.ToolTipText = options.ZoomToolTip;
			toolBarBtnOnePage.ToolTipText = options.OnePageToolTip;
			toolBarBtnTwoPages.ToolTipText = options.TwoPagesToolTip;
			toolBarBtnThreePages.ToolTipText = options.ThreePagesToolTip;
			toolBarBtnFourPages.ToolTipText = options.FourPagesToolTip;
			toolBarBtnSixPages.ToolTipText = options.SixPagesToolTip;
			toolBarBtnFirstPage.ToolTipText = options.FirstPageToolTip;
			toolBarBtnPrevPage.ToolTipText = options.PreviousPageToolTip;
			toolBarBtnNextPage.ToolTipText = options.NextPageToolTip;
			toolBarBtnLastPage.ToolTipText = options.LastPageToolTip;
			toolBarBtnScale.ToolTipText = options.FitToPageToolTip;
			toolBarBtnOrientation.ToolTipText = options.LandscapeToolTip;

			MenuAuto.Text = options.ZoomAutoCmdText;
			miScaleToPage.Text = options.ScaleFitToPageCmdText;
			miOriginalScale.Text = options.OriginalScaleCmdText;
			miDoubleScale.Text = options.DoubleScaleCmdText;
			miHalveSpace.Text = options.HalveScaleCmdText;
			miIncreaseScale.Text = options.IncreaseScaleCmdText;
			miDecreaseScale.Text = options.DecreaseScaleCmdText;

			previewOptions = options;
		}

		private float scaleChange;

		private void miDoubleScale_Click(object sender, System.EventArgs e)
		{
			PrintDocument doc = Document;

			flowChart.PrintOptions.Scale *= 2;
			fitToPage = false;
			toolBarBtnScale.Pushed = fitToPage;

			resetPreviewArea();
		}

		private void miHalveSpace_Click(object sender, System.EventArgs e)
		{
			if (flowChart.PrintOptions.Scale <= 2)
				return;

			PrintDocument doc = Document;

			flowChart.PrintOptions.Scale /= 2;
			fitToPage = false;
			toolBarBtnScale.Pushed = fitToPage;

			resetPreviewArea();
		}

		private void miPrinterMenu_Click(object sender, System.EventArgs e)
		{
			if (sender is MenuItem)
			{
				MenuItem item = sender as MenuItem;
				if (item.Checked)
					return;

				PrintDocument doc = Document;
				Margins originalMargins = doc.DefaultPageSettings.Margins;

				// change the target printer
				doc.PrinterSettings.PrinterName = item.Text;
				doc.DefaultPageSettings =
					(PageSettings)doc.PrinterSettings.DefaultPageSettings.Clone();

				// preserve some settings from the original document
				doc.DefaultPageSettings.Landscape = Landscape;
				doc.DefaultPageSettings.Margins = originalMargins;
				if (fitToPage)
				{
					flowChart.PrintOptions.ScaleToPage(doc);
					scaleChange = flowChart.PrintOptions.Scale / 5;
				}

				resetPreviewArea();
			}
		}

		/// <summary>
		/// Initialize the preview area with new document settings.
		/// </summary>
		private void resetPreviewArea()
		{
			// print the document again
			PrintDocument doc = Document;
			Document = null;
			Document = doc;

			// move to the first page
			PreviewArea.StartPage = 0;
			PageRange.Text = (PreviewArea.StartPage + 1).ToString();
		}

		/// <summary>
		/// Stores a reference to the flowchart's PreviewOptions.
		/// </summary>
		private PreviewOptions previewOptions;
	}
}