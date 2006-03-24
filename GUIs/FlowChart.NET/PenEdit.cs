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
using System.Data;
using System.Windows.Forms;

namespace MindFusion.FlowChartX.Design
{
	/// <summary>
	/// MindFusion.FlowChartX.Pen editing form.
	/// </summary>
	internal class PenEdit : System.Windows.Forms.UserControl
	{
		private MindFusion.Controls.DashEdit _dash;
		private MindFusion.Controls.DashEdit _compound;
		private System.Windows.Forms.Label _lblDash;
		private System.Windows.Forms.Label _lblCompound;
		private System.Windows.Forms.Label _lblStyle;
		private System.Windows.Forms.ComboBox _styleCombo;
		private System.Windows.Forms.Label _lblWidth;
		private System.Windows.Forms.TextBox _width;
		private System.Windows.Forms.Label _lblColor;
		private System.Windows.Forms.TextBox _color;
		private MindFusion.Controls.PenView _preview;
		private System.ComponentModel.Container components = null;

		public PenEdit()
		{
			InitializeComponent();

			_styleCombo.SelectedIndex = 0;
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if (_temp != null)
			{
				_temp.Dispose();
				_temp = null;
			}

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
			this._dash = new MindFusion.Controls.DashEdit();
			this._compound = new MindFusion.Controls.DashEdit();
			this._lblDash = new System.Windows.Forms.Label();
			this._lblCompound = new System.Windows.Forms.Label();
			this._styleCombo = new System.Windows.Forms.ComboBox();
			this._lblStyle = new System.Windows.Forms.Label();
			this._lblWidth = new System.Windows.Forms.Label();
			this._width = new System.Windows.Forms.TextBox();
			this._lblColor = new System.Windows.Forms.Label();
			this._color = new System.Windows.Forms.TextBox();
			this._preview = new MindFusion.Controls.PenView();
			this.SuspendLayout();
			// 
			// _dash
			// 
			this._dash.Location = new System.Drawing.Point(4, 60);
			this._dash.Name = "_dash";
			this._dash.Size = new System.Drawing.Size(108, 8);
			this._dash.TabIndex = 0;
			this._dash.DashChanged += new System.EventHandler(this.OnDashChanged);
			// 
			// _compound
			// 
			this._compound.ForeColor = System.Drawing.SystemColors.WindowText;
			this._compound.Location = new System.Drawing.Point(4, 84);
			this._compound.Name = "_compound";
			this._compound.Size = new System.Drawing.Size(108, 8);
			this._compound.TabIndex = 1;
			this._compound.DashChanged += new System.EventHandler(this.OnDashChanged);
			// 
			// _lblDash
			// 
			this._lblDash.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblDash.Location = new System.Drawing.Point(8, 48);
			this._lblDash.Name = "_lblDash";
			this._lblDash.Size = new System.Drawing.Size(100, 12);
			this._lblDash.TabIndex = 2;
			this._lblDash.Text = "Dash pattern:";
			// 
			// _lblCompound
			// 
			this._lblCompound.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblCompound.Location = new System.Drawing.Point(8, 72);
			this._lblCompound.Name = "_lblCompound";
			this._lblCompound.Size = new System.Drawing.Size(100, 12);
			this._lblCompound.TabIndex = 2;
			this._lblCompound.Text = "Compound array:";
			// 
			// _styleCombo
			// 
			this._styleCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this._styleCombo.Items.AddRange(new object[] {
															 "Solid",
															 "Dash",
															 "Dot",
															 "DashDot",
															 "DashDotDot",
															 "Custom"});
			this._styleCombo.Location = new System.Drawing.Point(40, 4);
			this._styleCombo.Name = "_styleCombo";
			this._styleCombo.Size = new System.Drawing.Size(68, 20);
			this._styleCombo.TabIndex = 3;
			this._styleCombo.SelectedIndexChanged += new System.EventHandler(this.OnStyleChanged);
			// 
			// _lblStyle
			// 
			this._lblStyle.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblStyle.Location = new System.Drawing.Point(8, 8);
			this._lblStyle.Name = "_lblStyle";
			this._lblStyle.Size = new System.Drawing.Size(28, 12);
			this._lblStyle.TabIndex = 4;
			this._lblStyle.Text = "Style:";
			// 
			// _lblWidth
			// 
			this._lblWidth.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblWidth.Location = new System.Drawing.Point(116, 48);
			this._lblWidth.Name = "_lblWidth";
			this._lblWidth.Size = new System.Drawing.Size(32, 12);
			this._lblWidth.TabIndex = 4;
			this._lblWidth.Text = "Width:";
			// 
			// _width
			// 
			this._width.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._width.Location = new System.Drawing.Point(116, 64);
			this._width.Name = "_width";
			this._width.Size = new System.Drawing.Size(32, 17);
			this._width.TabIndex = 5;
			this._width.Text = "0";
			this._width.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			this._width.Leave += new System.EventHandler(this.OnWidthChanged);
			// 
			// _lblColor
			// 
			this._lblColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this._lblColor.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblColor.Location = new System.Drawing.Point(8, 30);
			this._lblColor.Name = "_lblColor";
			this._lblColor.Size = new System.Drawing.Size(28, 12);
			this._lblColor.TabIndex = 4;
			this._lblColor.Text = "Color:";
			// 
			// _color
			// 
			this._color.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._color.Location = new System.Drawing.Point(40, 28);
			this._color.Name = "_color";
			this._color.Size = new System.Drawing.Size(68, 17);
			this._color.TabIndex = 6;
			this._color.Text = "0, 0, 0";
			this._color.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			this._color.Leave += new System.EventHandler(this.OnColorChanged);
			// 
			// _preview
			// 
			this._preview.Location = new System.Drawing.Point(116, 4);
			this._preview.Name = "_preview";
			this._preview.Pen = null;
			this._preview.Size = new System.Drawing.Size(32, 20);
			this._preview.TabIndex = 7;
			// 
			// PenEdit
			// 
			this.Controls.Add(this._preview);
			this.Controls.Add(this._color);
			this.Controls.Add(this._width);
			this.Controls.Add(this._lblStyle);
			this.Controls.Add(this._styleCombo);
			this.Controls.Add(this._lblDash);
			this.Controls.Add(this._compound);
			this.Controls.Add(this._dash);
			this.Controls.Add(this._lblCompound);
			this.Controls.Add(this._lblWidth);
			this.Controls.Add(this._lblColor);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Name = "PenEdit";
			this.Size = new System.Drawing.Size(152, 96);
			this.ResumeLayout(false);

		}
		#endregion


		[Browsable(false)]
		public MindFusion.FlowChartX.Pen Pen
		{
			get
			{
				if (_pen == null)
					return null;

				MindFusion.FlowChartX.Pen pen =
					_pen.Clone() as MindFusion.FlowChartX.Pen;

				pen.DashStyle = (DashStyle)_styleCombo.SelectedIndex;
				if (pen.DashStyle == DashStyle.Custom)
				{
					_dash.GetDashPattern(pen);
				}
				else
				{
					pen.DashPattern = null;
				}

				pen.CompoundArray = _compound.GetCompoundArray();

				pen.Width = float.Parse(_width.Text);

				pen.Color = TextToColor(_color.Text);

				return pen;
			}
			set
			{
				_pen = value;

				if (_pen.DashStyle == DashStyle.Custom)
				{
					_dash.Enabled = true;
					_dash.SetDashPattern(_pen);
				}
				else
				{
					_dash.Enabled = false;
				}
				_styleCombo.SelectedIndex = (int)_pen.DashStyle;

				_width.Text = _pen.Width.ToString();

				if (_pen.CompoundArray != null)
					_compound.SetCompoundArray(_pen.CompoundArray);

				_color.Text = ColorToText(_pen.Color);

				UpdatePreview();
			}
		}


		private void OnWidthChanged(object sender, System.EventArgs e)
		{
			try
			{
				float.Parse(_width.Text);
			}
			catch
			{
				_width.Text = "0";
			}

			UpdatePreview();
		}

		private void OnColorChanged(object sender, System.EventArgs e)
		{
			TextBox text = sender as TextBox;

			text.Text = ColorToText(TextToColor(text.Text));

			UpdatePreview();
		}

		private void OnDashChanged(object sender, System.EventArgs e)
		{
			UpdatePreview();
		}

		private void OnStyleChanged(object sender, System.EventArgs e)
		{
			_dash.Enabled = (_styleCombo.SelectedIndex == (int)DashStyle.Custom);

			UpdatePreview();
		}


		private void UpdatePreview()
		{
			MindFusion.FlowChartX.Pen pen = Pen;
			if (pen == null)
				return;

			if (_temp != null)
				_temp.Dispose();

			_temp = pen.CreateGDIPen(RectangleF.Empty);
			_preview.Pen = _temp;
		}

		private string ColorToText(Color color)
		{
			string result = "";

			if (color.A != 255)
				result += color.A.ToString() + ", ";
			result += color.R.ToString() + ", ";
			result += color.G.ToString() + ", ";
			result += color.B.ToString();

			return result;
		}

		private Color TextToColor(string text)
		{
			string[] channels = text.Split(new char[] { ',' });

			if (channels.Length != 3 && channels.Length != 4)
				return Color.White;

			int a = 255, r, g, b;
			try
			{
				int i = 0;
				if (channels.Length == 4)
					a = int.Parse(channels[i++]);

				r = int.Parse(channels[i++]);
				g = int.Parse(channels[i++]);
				b = int.Parse(channels[i++]);

				if (a > 255)
					a = 255;
				if (a < 0)
					a = 0;
				if (r > 255)
					r = 255;
				if (r < 0)
					r = 0;
				if (g > 255)
					g = 255;
				if (g < 0)
					g = 0;
				if (b > 255)
					b = 255;
				if (b < 0)
					b = 0;
			}
			catch
			{
				a = r = g = b = 255;
			}

			return Color.FromArgb(a, r, g, b);
		}


		private System.Drawing.Pen _temp = null;
		private MindFusion.FlowChartX.Pen _pen = null;
	}
}
