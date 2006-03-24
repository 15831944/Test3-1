// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Provides properties that control the appearance of the print preview dialog.
	/// </summary>
	public sealed class PreviewOptions
	{
		public PreviewOptions()
		{
			buttonImages = null;

			printToolTip = "Print";
			zoomToolTip = "Zoom";
			onePageToolTip = "One Page";
			twoPagesToolTip = "Two Pages";
			threePagesToolTip = "Three Pages";
			fourPagesToolTip = "Four Pages";
			sixPagesToolTip = "Six Pages";
			firstPageToolTip = "First Page";
			previousPageToolTip = "Previous Page";
			nextPageToolTip = "Next Page";
			lastPageToolTip = "Last Page";
			fitToPageToolTip = "Fit to Page";
			landscapeToolTip = "Landscape";
			portraitToolTip = "Portrait";

			zoomAutoCmdText = "Auto";
			scaleFitToPageCmdText = "Fit to Page";
			originalScaleCmdText = "Original Scale";
			doubleScaleCmdText = "Double Scale";
			halveScaleCmdText = "Halve Scale";
			increaseScaleCmdText = "Increase Scale";
			decreaseScaleCmdText = "Decrease Scale";

			titleBarIcon = null;
			displayPrintDialog = false;
		}

		/// <summary>
		/// Gets or sets the list of images to display in print preview toolbar buttons.
		/// </summary>
		[DefaultValue(null)]
		[Description("Gets or sets the list of images to display in print preview toolbar buttons.")]
		public ImageList ButtonImages
		{
			get { return buttonImages; }
			set { buttonImages = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Print button.
		/// </summary>
		[DefaultValue("Print")]
		public string PrintToolTip
		{
			get { return printToolTip; }
			set { printToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Zoom button.
		/// </summary>
		[DefaultValue("Zoom")]
		[Description("Gets or sets the ToolTip text displayed for the Zoom button.")]
		public string ZoomToolTip
		{
			get { return zoomToolTip; }
			set { zoomToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the One Page button.
		/// </summary>
		[DefaultValue("One Page")]
		[Description("Gets or sets the ToolTip text displayed for the One Page button.")]
		public string OnePageToolTip
		{
			get { return onePageToolTip; }
			set { onePageToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Two Pages button.
		/// </summary>
		[DefaultValue("Two Pages")]
		[Description("Gets or sets the ToolTip text displayed for the Two Pages button.")]
		public string TwoPagesToolTip
		{
			get { return twoPagesToolTip; }
			set { twoPagesToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Three Pages button.
		/// </summary>
		[DefaultValue("Three Pages")]
		[Description("Gets or sets the ToolTip text displayed for the Three Pages button.")]
		public string ThreePagesToolTip
		{
			get { return threePagesToolTip; }
			set { threePagesToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Four Pages button.
		/// </summary>
		[DefaultValue("Four Pages")]
		[Description("Gets or sets the ToolTip text displayed for the Four Pages button.")]
		public string FourPagesToolTip
		{
			get { return fourPagesToolTip; }
			set { fourPagesToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Six Pages button.
		/// </summary>
		[DefaultValue("Six Pages")]
		[Description("Gets or sets the ToolTip text displayed for the Six Pages button.")]
		public string SixPagesToolTip
		{
			get { return sixPagesToolTip; }
			set { sixPagesToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the First Page button.
		/// </summary>
		[DefaultValue("First Page")]
		[Description("Gets or sets the ToolTip text displayed for the First Page button.")]
		public string FirstPageToolTip
		{
			get { return firstPageToolTip; }
			set { firstPageToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Previous Page button.
		/// </summary>
		[DefaultValue("Previous Page")]
		[Description("Gets or sets the ToolTip text displayed for the Previous Page button.")]
		public string PreviousPageToolTip
		{
			get { return previousPageToolTip; }
			set { previousPageToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Next Page button.
		/// </summary>
		[DefaultValue("Next Page")]
		[Description("Gets or sets the ToolTip text displayed for the Next Page button.")]
		public string NextPageToolTip
		{
			get { return nextPageToolTip; }
			set { nextPageToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Last Page button.
		/// </summary>
		[DefaultValue("Last Page")]
		[Description("Gets or sets the ToolTip text displayed for the Last Page button.")]
		public string LastPageToolTip
		{
			get { return lastPageToolTip; }
			set { lastPageToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Fit to Page button.
		/// </summary>
		[DefaultValue("Fit to Page")]
		[Description("Gets or sets the ToolTip text displayed for the Fit to Page button.")]
		public string FitToPageToolTip
		{
			get { return fitToPageToolTip; }
			set { fitToPageToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Landscape button.
		/// </summary>
		[DefaultValue("Landscape")]
		[Description("Gets or sets the ToolTip text displayed for the Landscape button.")]
		public string LandscapeToolTip
		{
			get { return landscapeToolTip; }
			set { landscapeToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the ToolTip text displayed for the Portrait button.
		/// </summary>
		[DefaultValue("Portrait")]
		[Description("Gets or sets the ToolTip text displayed for the Portrait button.")]
		public string PortraitToolTip
		{
			get { return portraitToolTip; }
			set { portraitToolTip = value; }
		}

		/// <summary>
		/// Gets or sets the Label of the Zoom menu Auto item.
		/// </summary>
		[DefaultValue("Auto")]
		[Description("Gets or sets the Label of the Zoom menu Auto item.")]
		public string ZoomAutoCmdText
		{
			get { return zoomAutoCmdText; }
			set { zoomAutoCmdText = value; }
		}

		/// <summary>
		/// Gets or sets the Label of the Scale menu Fit to Page item.
		/// </summary>
		[DefaultValue("Fit to Page")]
		[Description("Gets or sets the Label of the Scale menu Fit to Page item.")]
		public string ScaleFitToPageCmdText
		{
			get { return scaleFitToPageCmdText; }
			set { scaleFitToPageCmdText = value; }
		}

		/// <summary>
		/// Gets or sets the Label of the Scale menu Original Scale item.
		/// </summary>
		[DefaultValue("Original Scale")]
		[Description("Gets or sets the Label of the Scale menu Original Scale item.")]
		public string OriginalScaleCmdText
		{
			get { return originalScaleCmdText; }
			set { originalScaleCmdText = value; }
		}

		/// <summary>
		/// Gets or sets the Label of the Scale menu Double Scale item.
		/// </summary>
		[DefaultValue("Double Scale")]
		[Description("Gets or sets the Label of the Scale menu Double Scale item.")]
		public string DoubleScaleCmdText
		{
			get { return doubleScaleCmdText; }
			set { doubleScaleCmdText = value; }
		}

		/// <summary>
		/// Gets or sets the Label of the Scale menu Halve Scale item.
		/// </summary>
		[DefaultValue("Halve Scale")]
		[Description("Gets or sets the Label of the Scale menu Halve Scale item.")]
		public string HalveScaleCmdText
		{
			get { return halveScaleCmdText; }
			set { halveScaleCmdText = value; }
		}

		/// <summary>
		/// Gets or sets the Label of the Scale menu Increase Scale item.
		/// </summary>
		[DefaultValue("Increase Scale")]
		[Description("Gets or sets the Label of the Scale menu Increase Scale item.")]
		public string IncreaseScaleCmdText
		{
			get { return increaseScaleCmdText; }
			set { increaseScaleCmdText = value; }
		}

		/// <summary>
		/// Gets or sets the Label of the Scale menu Decrease Scale item.
		/// </summary>
		[DefaultValue("Decrease Scale")]
		[Description("Gets or sets the Label of the Scale menu Decrease Scale item.")]
		public string DecreaseScaleCmdText
		{
			get { return decreaseScaleCmdText; }
			set { decreaseScaleCmdText = value; }
		}

		/// <summary>
		/// Gets or sets the icon displayed in the titlebar of the preview window.
		/// </summary>
		[DefaultValue(null)]
		[Description("Gets or sets the icon displayed in the titlebar of the preview window.")]
		public Icon TitleBarIcon
		{
			get { return titleBarIcon; }
			set { titleBarIcon = value; }
		}

		/// <summary>
		/// Specifies whether a print setup dialog is displayed when
		/// the print button of the preview form is clicked.
		/// </summary>
		[DefaultValue(false)]
		[Description("Specifies whether to display the printer setup dialog when a user clicks the print button.")]
		public bool DisplayPrintDialog
		{
			get { return displayPrintDialog; }
			set { displayPrintDialog = value; }
		}

		/// <summary>
		/// A list of images to display in print preview toolbar buttons.
		/// </summary>
		private ImageList buttonImages;

		/// <summary>
		/// ToolTip text displayed for the Print button.
		/// </summary>
		private string printToolTip;

		/// <summary>
		/// ToolTip text displayed for the Zoom button.
		/// </summary>
		private string zoomToolTip;

		/// <summary>
		/// ToolTip text displayed for the One Page button.
		/// </summary>
		private string onePageToolTip;

		/// <summary>
		/// ToolTip text displayed for the Two Pages button.
		/// </summary>
		private string twoPagesToolTip;

		/// <summary>
		/// ToolTip text displayed for the Three Pages button.
		/// </summary>
		private string threePagesToolTip;

		/// <summary>
		/// ToolTip text displayed for the Four Pages button.
		/// </summary>
		private string fourPagesToolTip;

		/// <summary>
		/// ToolTip text displayed for the Six Pages button.
		/// </summary>
		private string sixPagesToolTip;

		/// <summary>
		/// ToolTip text displayed for the First Page button.
		/// </summary>
		private string firstPageToolTip;

		/// <summary>
		/// ToolTip text displayed for the Previous Page button.
		/// </summary>
		private string previousPageToolTip;

		/// <summary>
		/// ToolTip text displayed for the Next Page button.
		/// </summary>
		private string nextPageToolTip;

		/// <summary>
		/// ToolTip text displayed for the Last Page button.
		/// </summary>
		private string lastPageToolTip;

		/// <summary>
		/// ToolTip text displayed for the Fit to Page button.
		/// </summary>
		private string fitToPageToolTip;

		/// <summary>
		/// ToolTip text displayed for the Landscape button.
		/// </summary>
		private string landscapeToolTip;

		/// <summary>
		/// ToolTip text displayed for the Portrait button.
		/// </summary>
		private string portraitToolTip;

		/// <summary>
		/// Label of the Zoom menu Auto item.
		/// </summary>
		private string zoomAutoCmdText;

		/// <summary>
		/// Label of the Scale menu Fit to Page item.
		/// </summary>
		private string scaleFitToPageCmdText;

		/// <summary>
		/// Label of the Scale menu Original Scale item.
		/// </summary>
		private string originalScaleCmdText;

		/// <summary>
		/// Label of the Scale menu Double Scale item.
		/// </summary>
		private string doubleScaleCmdText;

		/// <summary>
		/// Label of the Scale menu Halve Scale item.
		/// </summary>
		private string halveScaleCmdText;

		/// <summary>
		/// Label of the Scale menu Increase Scale item.
		/// </summary>
		private string increaseScaleCmdText;

		/// <summary>
		/// Label of the Scale menu Decrease Scale item.
		/// </summary>
		private string decreaseScaleCmdText;

		/// <summary>
		/// An Icon displayed in the preview window titlebar.
		/// </summary>
		private Icon titleBarIcon;

		/// <summary>
		/// Specifies whether a print setup dialog is displayed when
		/// the print button of the preview form is clicked.
		/// </summary>
		private bool displayPrintDialog;
	}
}