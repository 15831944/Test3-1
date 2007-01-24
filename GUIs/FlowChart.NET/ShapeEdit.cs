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
using System.Drawing.Design;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace MindFusion.FlowChartX.Design
{
	/// <summary>
	/// MindFusion.FlowChartX.Pen editing form.
	/// </summary>
	internal class ShapeEdit : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.ListBox _list;
		private System.ComponentModel.Container components = null;

		public ShapeEdit(IWindowsFormsEditorService service)
		{
			_service = service;

			InitializeComponent();

			// Populate the list box with the available shapes
			foreach (ShapeTemplate shape in ShapeTemplate.Shapes)
				_list.Items.Add(shape.Id);
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this._list = new System.Windows.Forms.ListBox();
			this.SuspendLayout();
			// 
			// _list
			// 
			this._list.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this._list.Dock = System.Windows.Forms.DockStyle.Fill;
			this._list.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
			this._list.Location = new System.Drawing.Point(0, 0);
			this._list.Name = "_list";
			this._list.Size = new System.Drawing.Size(152, 130);
			this._list.TabIndex = 0;
			this._list.MeasureItem += new System.Windows.Forms.MeasureItemEventHandler(this.OnMeasureItem);
			this._list.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.OnDrawItem);
			this._list.SelectedIndexChanged += new System.EventHandler(this.OnSelected);
			// 
			// ShapeEdit
			// 
			this.Controls.Add(this._list);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Name = "ShapeEdit";
			this.Size = new System.Drawing.Size(152, 130);
			this.ResumeLayout(false);

		}
		#endregion

		private void OnMeasureItem(object sender, System.Windows.Forms.MeasureItemEventArgs e)
		{
			e.ItemHeight = 26;
		}

		private void OnDrawItem(object sender, System.Windows.Forms.DrawItemEventArgs e)
		{
			e.DrawBackground();
			e.DrawFocusRectangle();

			Rectangle rect = e.Bounds;
			rect.X += 1;
			rect.Y += 1;
			rect.Height -= 2;
			rect.Width = rect.Height;

			System.Drawing.Brush brush =
				new System.Drawing.SolidBrush(Color.White);
			System.Drawing.Brush fill =
				new System.Drawing.SolidBrush(Color.LightSteelBlue);
			System.Drawing.Pen pen =
				new System.Drawing.Pen(Color.Black, 0);

			e.Graphics.FillRectangle(brush, rect);
			e.Graphics.DrawRectangle(pen, rect);

			System.Drawing.Drawing2D.SmoothingMode mode =
				e.Graphics.SmoothingMode;
			e.Graphics.SmoothingMode =
				System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

			rect.Inflate(-2, -2);
			string shapeId = _list.Items[e.Index].ToString();
			ShapeTemplate shape = ShapeTemplate.FromId(shapeId);
			RectangleF rectf = new RectangleF(
				(float)rect.X, (float)rect.Y,
				(float)rect.Width, (float)rect.Height);
			MindFusion.FlowChartX.ShapeTemplate.PathData data =
				shape.initData(rectf, 0);

			System.Drawing.Drawing2D.GraphicsPath path =
				shape.getPath(data, 0);
			e.Graphics.FillPath(fill, path);
			e.Graphics.DrawPath(pen, path);
			path.Dispose();

			path = shape.getDecorationPath(data, 0);
			if (path != null)
			{
				e.Graphics.DrawPath(pen, path);
				path.Dispose();
			}

			e.Graphics.SmoothingMode = mode;

			pen.Dispose();
			fill.Dispose();
			brush.Dispose();

			// Draw the text;
			rectf.X = rectf.Right + 6;
			rectf.Width = (float)e.Bounds.Width - rectf.X;

			StringFormat format = new StringFormat();
			format.Alignment = StringAlignment.Near;
			format.LineAlignment = StringAlignment.Center;
			System.Drawing.Brush textBrush =
				new System.Drawing.SolidBrush(Color.Black);
			e.Graphics.DrawString(shapeId, Font, textBrush, rectf, format);
			textBrush.Dispose();
			format.Dispose();
		}

		private void OnSelected(object sender, System.EventArgs e)
		{
			if (_list.SelectedIndex != -1)
                _selected = ShapeTemplate.FromId(
					_list.SelectedItem.ToString());

			_service.CloseDropDown();
		}


		[Browsable(false)]
		public MindFusion.FlowChartX.ShapeTemplate Shape
		{
			get { return _selected; }
			set
			{
				// Set the selected index to the appropriate shape
				for (int i = 0; i < ShapeTemplate.Shapes.Count; i++)
				{
					if (ShapeTemplate.Shapes[i] == value)
					{
						_selected = ShapeTemplate.Shapes[i];
						_list.SelectedItem = _list.Items[i];
						return;
					}
				}

				_list.SelectedIndex = -1;
			}
		}

		private IWindowsFormsEditorService _service = null;
		private ShapeTemplate _selected = null;
	}
}
