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
using System.Drawing.Printing;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	public sealed class PrintOptions
	{
		public PrintOptions(FlowChart document)
		{
			documentName = " - Untitled - ";
			headerFormat = "%D , Page %P";
			scale = 100.0f;
			enableShadows = true;
			enableImages = true;
			enableInterior = true;
			enableAnchors = false;
			enableBackground = true;
			enableBackgroundImage = true;
			enableText = true;
			paintControls = false;

			this.document = document;

			pageWidth = pageHeight = 0;

			// init the margins to the PrintDocument defaults of 1 inch
			margins = new Margins(100, 100, 100, 100);
		}

		[DefaultValue(" - Untitled - ")]
		[Description("This string is displayed in the print queue status window and in page headers.")]
		public string DocumentName
		{
			get { return documentName; }
			set { documentName = value; }
		}

		[DefaultValue("%D , Page %P")]
		[Description("Specifies the format of page headers.")]
		public string HeaderFormat
		{
			get { return headerFormat; }
			set { headerFormat = value; }
		}

		[DefaultValue(100.0f)]
		[Description("Specifies the scale of the printed diagram as percent of its on-screen representation.")]
		public float Scale
		{
			get { return scale; }
			set { scale = value; }
		}

		[DefaultValue(true)]
		[Description("Enables or disables printing the shadows of diagram items.")]
		public bool EnableShadows
		{
			get { return enableShadows; }
			set { enableShadows = value; }
		}

		[DefaultValue(true)]
		[Description("Enables or disables printing the images assigned to diagram nodes.")]
		public bool EnableImages
		{
			get { return enableImages; }
			set { enableImages = value; }
		}

		[DefaultValue(true)]
		[Description("Enables or disables filling the interior of nodes when printing.")]
		public bool EnableInterior
		{
			get { return enableInterior; }
			set { enableInterior = value; }
		}

		[DefaultValue(false)]
		[Description("Enables or disables printing the anchor points of nodes.")]
		public bool EnableAnchors
		{
			get { return enableAnchors; }
			set { enableAnchors = value; }
		}

		[DefaultValue(false)]
		[Description("Enables or disables filling the printed pages with flowchart's BackColor.")]
		public bool EnableBackground
		{
			get { return enableBackground; }
			set { enableBackground = value; }
		}

		[DefaultValue(false)]
		[Description("Enables or disables printing the flowchart's background image.")]
		public bool EnableBackgroundImage
		{
			get { return enableBackgroundImage; }
			set { enableBackgroundImage = value; }
		}

		[DefaultValue(true)]
		[Description("Enables or disables printing the text assigned to diagram items.")]
		public bool EnableText
		{
			get { return enableText; }
			set { enableText = value; }
		}

		[DefaultValue(true)]
		[Description("Enables or disables the execution of custom painting code for ControlHost nodes when printing.")]
		public bool PaintControls
		{
			get { return paintControls; }
			set { paintControls = value; }
		}

		/// <summary>
		/// Gets or sets the page margins.
		/// </summary>
		[DefaultValue(typeof(Margins), "100, 100, 100, 100")]
		[Description("The margins for printed pages, measured in hundredths of an inch.")]
		public Margins Margins
		{
			get { return margins; }
			set { margins = value; }
		}

		/// <summary>
		/// Sets the scale to a value that makes the whole diagram fit into a single page.
		/// </summary>
		public void ScaleToPage()
		{
			PrintDocument doc = new PrintDocument();
			ScaleToPage(doc);
		}

		private void recalcPageDims(PrintDocument doc)
		{
			// page margins are specified in hundreths of inch
			// the closest GraphicsUnit is the Document: 1/300 inch
			Graphics g = null;
			try
			{
				g = doc.PrinterSettings.CreateMeasurementGraphics();
			}
			catch (InvalidPrinterException)
			{
				g = document.CreateGraphics();
			}

			g.ResetTransform();
			g.PageUnit = GraphicsUnit.Document;

			// get margins rectangle in 1/300 inch coordinates
			Margins m = doc.DefaultPageSettings.Margins;
			RectangleF marginsRect = doc.DefaultPageSettings.Bounds;
			marginsRect.Width -= m.Right + m.Left;
			marginsRect.Height -= m.Bottom + m.Top;
			marginsRect.Width *= 3;
			marginsRect.Height *= 3;

			// get margins rectangle in device coordinates
			RectangleF marginsRectDev = Utilities.transformRect(g,
				marginsRect, CoordinateSpace.Device, CoordinateSpace.World);

			// set transformation to get margins in document coordinates
			g.PageUnit = document.MeasureUnit;
			g.PageScale = 1.0f;

			// get margins rectangle in document coordinates
			RectangleF marginsRectDoc = Utilities.transformRect(g,
				marginsRectDev, CoordinateSpace.World, CoordinateSpace.Device);
			
			// finally we have page width and height in document coordinates
			pageWidth = marginsRectDoc.Width;
			pageHeight = marginsRectDoc.Height;
			
			g.Dispose();
		}


		public void ScaleToPage(PrintDocument doc)
		{
			// get page width and height in document coordinates
			recalcPageDims(doc);
			if (pageWidth == 0 || pageHeight == 0) return;

			// get document rectangle in document coordinates
			RectangleF docRect = document.getContentRect(true, true);

			// do not scale empty documents
			if (docRect.Width <= 0 || docRect.Height <= 0) return;

			// items should not touch the border line
			docRect.Width += Constants.getMillimeter(document.MeasureUnit);
			docRect.Height += Constants.getMillimeter(document.MeasureUnit);

			// calculate print scale
			float docWidth = docRect.Width;
			float docHeight = docRect.Height;

			if (docWidth > 0 && docHeight > 0 && pageWidth > 0 && pageHeight > 0)
			{
				float rx = pageWidth / docWidth;
				float ry = pageHeight / docHeight;
				float ratio = Math.Min(rx, ry);

				Scale = 100 * ratio;
			}
		}

		private string documentName;
		private string headerFormat;
		private float scale;
		private bool enableShadows;
		private bool enableImages;
		private bool enableInterior;
		private bool enableAnchors;
		private bool enableBackground;
		private bool enableBackgroundImage;
		private bool enableText;
		private bool paintControls;

		private float pageWidth;
		private float pageHeight;

		/// <summary>
		/// Defines the page margins.
		/// </summary>
		private Margins margins;

		private FlowChart document;
	}
}