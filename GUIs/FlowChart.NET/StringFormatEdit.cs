using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace MindFusion.FlowChartX.Design
{
	internal class StringFormatEdit : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.RadioButton _lt;
		private System.Windows.Forms.RadioButton _rt;
		private System.Windows.Forms.RadioButton _l;
		private System.Windows.Forms.RadioButton _c;
		private System.Windows.Forms.RadioButton _r;
		private System.Windows.Forms.RadioButton _lb;
		private System.Windows.Forms.RadioButton _rb;
		private System.Windows.Forms.RadioButton _ct;
		private System.Windows.Forms.RadioButton _cb;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public StringFormatEdit(IWindowsFormsEditorService service)
		{
			_service = service;

			// This call is required by the Windows.Forms Form Designer.
			_initializing = true;
			InitializeComponent();
			_initializing = false;
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
			this._lt = new System.Windows.Forms.RadioButton();
			this._ct = new System.Windows.Forms.RadioButton();
			this._rt = new System.Windows.Forms.RadioButton();
			this._l = new System.Windows.Forms.RadioButton();
			this._c = new System.Windows.Forms.RadioButton();
			this._r = new System.Windows.Forms.RadioButton();
			this._lb = new System.Windows.Forms.RadioButton();
			this._cb = new System.Windows.Forms.RadioButton();
			this._rb = new System.Windows.Forms.RadioButton();
			this.SuspendLayout();
			// 
			// _lt
			// 
			this._lt.Appearance = System.Windows.Forms.Appearance.Button;
			this._lt.Location = new System.Drawing.Point(0, 0);
			this._lt.Name = "_lt";
			this._lt.Size = new System.Drawing.Size(20, 20);
			this._lt.TabIndex = 0;
			this._lt.Click += new System.EventHandler(this.OnButtonClick);
			this._lt.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._lt.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// _ct
			// 
			this._ct.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this._ct.Appearance = System.Windows.Forms.Appearance.Button;
			this._ct.Location = new System.Drawing.Point(20, 0);
			this._ct.Name = "_ct";
			this._ct.Size = new System.Drawing.Size(20, 20);
			this._ct.TabIndex = 0;
			this._ct.Click += new System.EventHandler(this.OnButtonClick);
			this._ct.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._ct.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// _rt
			// 
			this._rt.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._rt.Appearance = System.Windows.Forms.Appearance.Button;
			this._rt.Location = new System.Drawing.Point(40, 0);
			this._rt.Name = "_rt";
			this._rt.Size = new System.Drawing.Size(20, 20);
			this._rt.TabIndex = 0;
			this._rt.Click += new System.EventHandler(this.OnButtonClick);
			this._rt.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._rt.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// _l
			// 
			this._l.Appearance = System.Windows.Forms.Appearance.Button;
			this._l.Location = new System.Drawing.Point(0, 20);
			this._l.Name = "_l";
			this._l.Size = new System.Drawing.Size(20, 20);
			this._l.TabIndex = 0;
			this._l.Click += new System.EventHandler(this.OnButtonClick);
			this._l.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._l.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// _c
			// 
			this._c.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this._c.Appearance = System.Windows.Forms.Appearance.Button;
			this._c.Location = new System.Drawing.Point(20, 20);
			this._c.Name = "_c";
			this._c.Size = new System.Drawing.Size(20, 20);
			this._c.TabIndex = 0;
			this._c.Click += new System.EventHandler(this.OnButtonClick);
			this._c.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._c.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// _r
			// 
			this._r.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._r.Appearance = System.Windows.Forms.Appearance.Button;
			this._r.Location = new System.Drawing.Point(40, 20);
			this._r.Name = "_r";
			this._r.Size = new System.Drawing.Size(20, 20);
			this._r.TabIndex = 0;
			this._r.Click += new System.EventHandler(this.OnButtonClick);
			this._r.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._r.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// _lb
			// 
			this._lb.Appearance = System.Windows.Forms.Appearance.Button;
			this._lb.Location = new System.Drawing.Point(0, 40);
			this._lb.Name = "_lb";
			this._lb.Size = new System.Drawing.Size(20, 20);
			this._lb.TabIndex = 0;
			this._lb.Click += new System.EventHandler(this.OnButtonClick);
			this._lb.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._lb.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// _cb
			// 
			this._cb.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this._cb.Appearance = System.Windows.Forms.Appearance.Button;
			this._cb.Location = new System.Drawing.Point(20, 40);
			this._cb.Name = "_cb";
			this._cb.Size = new System.Drawing.Size(20, 20);
			this._cb.TabIndex = 0;
			this._cb.Click += new System.EventHandler(this.OnButtonClick);
			this._cb.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._cb.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// _rb
			// 
			this._rb.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._rb.Appearance = System.Windows.Forms.Appearance.Button;
			this._rb.Location = new System.Drawing.Point(40, 40);
			this._rb.Name = "_rb";
			this._rb.Size = new System.Drawing.Size(20, 20);
			this._rb.TabIndex = 0;
			this._rb.Click += new System.EventHandler(this.OnButtonClick);
			this._rb.Paint += new System.Windows.Forms.PaintEventHandler(this.OnButtonPaint);
			this._rb.CheckedChanged += new System.EventHandler(this.OnChanged);
			// 
			// StringFormatEdit
			// 
			this.Controls.Add(this._lt);
			this.Controls.Add(this._ct);
			this.Controls.Add(this._rt);
			this.Controls.Add(this._l);
			this.Controls.Add(this._c);
			this.Controls.Add(this._r);
			this.Controls.Add(this._lb);
			this.Controls.Add(this._cb);
			this.Controls.Add(this._rb);
			this.Name = "StringFormatEdit";
			this.Size = new System.Drawing.Size(60, 60);
			this.ResumeLayout(false);

		}
		#endregion

		private void OnButtonPaint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			RadioButton s = sender as RadioButton;
			if (s.Checked)
			{
				System.Drawing.Brush brush =
					new System.Drawing.SolidBrush(SystemColors.Highlight);
				e.Graphics.FillRectangle(brush, s.ClientRectangle);
				brush.Dispose();
			}
		}

		private void OnButtonClick(object sender, System.EventArgs e)
		{
		}

		private void OnChanged(object sender, System.EventArgs e)
		{
			if (!_initializing)
				_service.CloseDropDown();
		}


		[Browsable(false)]
		public StringFormat Format
		{
			get
			{
				StringFormat format = new StringFormat();

				if (_lt.Checked || _l.Checked || _lb.Checked)
					format.Alignment = StringAlignment.Near;
				else if (_ct.Checked || _c.Checked || _cb.Checked)
					format.Alignment = StringAlignment.Center;
				else
					format.Alignment = StringAlignment.Far;

				if (_lt.Checked || _ct.Checked || _rt.Checked)
					format.LineAlignment = StringAlignment.Near;
				else if (_l.Checked || _c.Checked || _r.Checked)
					format.LineAlignment = StringAlignment.Center;
				else
					format.LineAlignment = StringAlignment.Far;

				format.FormatFlags = StringFormatFlags.NoFontFallback;

				return format;
			}
			set
			{
				_initializing = true;

				switch (value.Alignment)
				{

					case StringAlignment.Near:
					switch (value.LineAlignment)
					{

						case StringAlignment.Near:
							_lt.Checked = true;
							break;

						case StringAlignment.Center:
							_l.Checked = true;
							break;

						case StringAlignment.Far:
							_lb.Checked = true;
							break;

					}
						break;

					case StringAlignment.Center:
					switch (value.LineAlignment)
					{

						case StringAlignment.Near:
							_ct.Checked = true;
							break;

						case StringAlignment.Center:
							_c.Checked = true;
							break;

						case StringAlignment.Far:
							_cb.Checked = true;
							break;

					}
						break;

					case StringAlignment.Far:
					switch (value.LineAlignment)
					{

						case StringAlignment.Near:
							_rt.Checked = true;
							break;

						case StringAlignment.Center:
							_r.Checked = true;
							break;

						case StringAlignment.Far:
							_rb.Checked = true;
							break;

					}
						break;

				}

				_initializing = false;
			}
		}


		private IWindowsFormsEditorService _service = null;
		private bool _initializing = true;
	}
}
