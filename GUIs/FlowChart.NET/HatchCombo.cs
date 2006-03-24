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

namespace MindFusion.Controls
{
	internal class HatchCombo : ComboBox
	{
		public HatchCombo()
		{
			this.DrawMode = DrawMode.OwnerDrawFixed;
			this.DropDownStyle = ComboBoxStyle.DropDownList;
			this.ItemHeight = 24;

			// Populate the combo box
			Array hatchStyles = Enum.GetValues(typeof(HatchStyle));
			foreach (HatchStyle style in hatchStyles)
				this.Items.Add(style.ToString());

			this.SelectedIndex = 0;
		}

		protected override void OnDrawItem(DrawItemEventArgs e)
		{
			base.OnDrawItem(e);

			e.DrawBackground();

			HatchStyle style = FromString((string)this.Items[e.Index]);
			System.Drawing.Drawing2D.HatchBrush brush =
				new System.Drawing.Drawing2D.HatchBrush(style, Color.Black, e.BackColor);
			System.Drawing.Pen pen =
				new System.Drawing.Pen(Color.Black, 0);

			Rectangle rect = e.Bounds;
			rect.Inflate(-1, -1);
			rect.Width -= 1;
			rect.Height -= 1;

			e.Graphics.RenderingOrigin = new Point(0, 0);
			e.Graphics.FillRectangle(brush, rect);
			e.Graphics.DrawRectangle(pen, rect);

			pen.Dispose();
			brush.Dispose();
		}

		[Browsable(true)]
		[Category("Data")]
		[Description("Gets or sets the currently selected hatch style.")]
		public HatchStyle SelectedStyle
		{
			get { return FromString((string)this.SelectedItem); }
			set { this.SelectedItem = value.ToString(); }
		}

		[Browsable(false)]
		public new System.Windows.Forms.ComboBox.ObjectCollection Items
		{
			get { return base.Items; }
		}


		private HatchStyle FromString(string style)
		{
			return (HatchStyle)Enum.Parse(
				typeof(HatchStyle), style, false);
		}
	}
}

