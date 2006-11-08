using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace SysCAD.ThingEditor.ThingEditor
{
	/// <summary>
	/// Summary description for FrmHelp.
	/// </summary>
	public class FrmHelp : System.Windows.Forms.Form
	{
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox tb;
        private System.Windows.Forms.Button btnClose;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public FrmHelp()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
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

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.panel1 = new System.Windows.Forms.Panel();
			this.tb = new System.Windows.Forms.TextBox();
			this.btnClose = new System.Windows.Forms.Button();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel1
			// 
			this.panel1.Controls.AddRange(new System.Windows.Forms.Control[] {
																				 this.tb});
			this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(600, 264);
			this.panel1.TabIndex = 0;
			// 
			// tb
			// 
			this.tb.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tb.Multiline = true;
			this.tb.Name = "tb";
			this.tb.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tb.Size = new System.Drawing.Size(600, 264);
			this.tb.TabIndex = 0;
			this.tb.Text = "";
			// 
			// btnClose
			// 
			this.btnClose.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnClose.Location = new System.Drawing.Point(256, 272);
			this.btnClose.Name = "btnClose";
			this.btnClose.TabIndex = 1;
			this.btnClose.Text = "Close";
			// 
			// FrmHelp
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.btnClose;
			this.ClientSize = new System.Drawing.Size(600, 301);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.btnClose,
																		  this.panel1});
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FrmHelp";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Help";
			this.Load += new System.EventHandler(this.FrmHelp_Load);
			this.panel1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void FrmHelp_Load(object sender, System.EventArgs e)
		{
			tb.Text =
				"Use ThingEditor to create ShapeTemplate definitions, including outline, decorations and text-area.\r\n" +
				"Right click on a line or on a control point to get context menus that allow adding or removing segments.\r\n" +
				"Move existing control points by dragging them with the mouse\r\n\r\n"+
				"Test the shape on the right-hand part of the form, which contains FlowChart.NET instance and a node with the shape defined on the left side";
		}
	}
}
