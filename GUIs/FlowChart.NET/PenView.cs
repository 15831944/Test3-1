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
using System.Windows.Forms;

namespace MindFusion.Controls
{
	internal class PenView : System.Windows.Forms.Control
	{
		public PenView()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			SetStyle(ControlStyles.DoubleBuffer, true);
			SetStyle(ControlStyles.ResizeRedraw, true);
			SetStyle(ControlStyles.UserPaint, true);
		}


		protected override void Dispose(bool disposing)
		{
			base.Dispose(disposing);
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			System.Drawing.Pen pen = 
				new System.Drawing.Pen(this.ForeColor, 0);

			Rectangle rect = this.ClientRectangle;
			rect.Width -= 1;
			rect.Height -= 1;

			try
			{
				if (_pen != null)
					e.Graphics.DrawLine(_pen,
						rect.Left, rect.Top + rect.Height / 2,
						rect.Right, rect.Top + rect.Height / 2);
			}
			catch
			{
				GC.Collect();
			}
			e.Graphics.DrawRectangle(pen, rect);

            pen.Dispose();
		}


		[Browsable(false)]
		public System.Drawing.Pen Pen
		{
			get { return _pen; }
			set
			{
				_pen = value;
				Invalidate();
			}
		}

		[Browsable(false)]
		public new string Text
		{
			get { return base.Text; }
			set { base.Text = value; }
		}


		private System.Drawing.Pen _pen = null;
	}
}
