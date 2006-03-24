using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Collections;
//using sharpPDF;
//using sharpPDF.Enumerators;
/*
namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Summary description for PDFBuilder.
	/// </summary>
	public class PDFBuilder
	{
		static private FlowChart flowChart;
		static private string  documentName;
		private const int left = 50;
		private const int top = 50;
		private const int pdfPageHeight = 700;
		private const int pdfPageWidth = 520;

		public PDFBuilder(FlowChart fc, string docName)
		{
			flowChart = fc;
			documentName = docName;
		}

		static private void setTransforms(Graphics g)
		{
			g.PageUnit = flowChart.MeasureUnit;
			g.PageScale = (float)flowChart.ZoomFactor / 100F;
		}

		static private Point DocToClient(PointF docPt)
		{
			Graphics g = flowChart.CreateGraphics();
			setTransforms(g);
			Point clientPt = Utilities.docToDevice(g, docPt);
			g.Dispose();
			return clientPt;
		}

		static private void createPDFFitAll(string path, bool createBookmark)
		{
			Cursor saveCursor = flowChart.Cursor;
			flowChart.Cursor = Cursors.WaitCursor;
			Bitmap bmp = flowChart.CreateBmpFromChart();
			pdfDocument pdfDoc = new pdfDocument(documentName,"", createBookmark);
			pdfPage pdfpage = pdfDoc.addPage();
			pdfpage.addImage(bmp, left, top, pdfPageHeight, pdfPageWidth);
			pdfBookmarkNode pdfBookNode = new pdfBookmarkNode(documentName, pdfpage, false, pdfDestinationFactory.createPdfDestinationFit());
			pdfDoc.addBookmark(pdfBookNode);
			pdfDoc.createPDF(path);
			bmp.Dispose();
			flowChart.Cursor = saveCursor;
		}

		static private Bitmap CreateBmpFromChart(int left, int top, int width, int height)
		{
			int bmpWidth  = DocToClient(new Point((int)flowChart.DocExtents.Width, (int)flowChart.DocExtents.Height)).X;
			int bmpHeight = DocToClient(new Point((int)flowChart.DocExtents.Width, (int)flowChart.DocExtents.Height)).Y;
			//int bmpWidth  = (int)flowChart.DocExtents.Width;
			//int bmpHeight = (int)flowChart.DocExtents.Height;

			//Bitmap   bmp  = new Bitmap(bmpWidth, bmpHeight);
			Bitmap   bmp  = new Bitmap(width, height);
			if (bmp != null)
			{
				Graphics g = Graphics.FromImage(bmp);
				//g.ResetTransform();
				g.TranslateTransform(-left, -top);
				g.PageUnit = flowChart.MeasureUnit;
				g.PageScale = (float)flowChart.ZoomFactor / 100F;
			
				Rectangle rc = new Rectangle(left, top, width, height);

				flowChart.PrintOptions.EnableBackground = true;
				if (!flowChart.PrintOptions.EnableBackground)
				{
					System.Drawing.Brush sb = new System.Drawing.SolidBrush(Color.White);
					g.FillRectangle(sb, rc);
				}
				//flowChart.drawFlowChart(bmp, rc, false, false);
				flowChart.printFlowChart(g, rc);
				g.Dispose();
			}
			return bmp;
		}

        static private void createPDF(string path, bool createBookmark)
        {
            bool bookMarkCreated     = false;
            pdfBookmarkNode mainBook = null;
			int currPageTop	     = 0;
			int currPageLeft     = 0;
			int pageCount	     = 0;

			int docExtentsWidth  = DocToClient(new Point((int)flowChart.DocExtents.Width, (int)flowChart.DocExtents.Height)).X;
			int docExtentsHeight = DocToClient(new Point((int)flowChart.DocExtents.Width, (int)flowChart.DocExtents.Height)).Y;

			int pageCountHoriz  = docExtentsWidth  / pdfPageWidth + 1; // Calculate number horiz  pages 
            int pageCountVert   = docExtentsHeight / pdfPageHeight + 1;// Calculate number vert  pages

			pdfDocument pdfDoc = new pdfDocument(documentName,"", createBookmark);
			Cursor saveCursor = flowChart.Cursor;
            for(int i = 0; i < pageCountVert; ++i)
            {
				currPageLeft = 0;
				for(int j = 0; j < pageCountHoriz; ++j)
				{
					flowChart.Cursor = Cursors.WaitCursor;
					pdfPage pdfpage = pdfDoc.addPage();
					Bitmap bmp = CreateBmpFromChart(currPageLeft, currPageTop, pdfPageWidth, pdfPageHeight);

					pdfpage.addImage(bmp, left, top, pdfPageHeight, pdfPageWidth);

					if (createBookmark)
					{
						if (!bookMarkCreated)
						{
							pdfBookmarkNode Book = new pdfBookmarkNode(documentName, pdfpage, false, pdfDestinationFactory.createPdfDestinationFit());
							pdfDoc.addBookmark(Book);
							mainBook = Book;
						}
						bookMarkCreated = true;
						pageCount += 1;
						pdfBookmarkNode childBook = new pdfBookmarkNode("Page -" + pageCount.ToString() +"-", pdfpage, false, pdfDestinationFactory.createPdfDestinationFit());
						mainBook.addChildNode(childBook);
					}
					//bmp.Dispose();
					currPageLeft += pdfPageWidth;
				}
                currPageTop += pdfPageHeight;
            }
            pdfDoc.createPDF(path); 
			flowChart.Cursor = saveCursor;
        }

		static public void createPDF(string path, bool createBookmark, bool fitAll)
		{
			if (fitAll)
				createPDFFitAll(path, createBookmark);
			else
				createPDF(path, createBookmark);
		}
	}
}
*/