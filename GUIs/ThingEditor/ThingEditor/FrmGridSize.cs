using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;


namespace SysCAD.ThingEditor.ThingEditor
{
    public delegate void GridValueChanged(object sender, EventArgs e);
	/// <summary>
	/// Summary description for FrmGridSize.
	/// </summary>
	public class FrmGridSize : System.Windows.Forms.Form
	{
        public System.Windows.Forms.NumericUpDown GridSizeUpDown;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public FrmGridSize()
		{
			InitializeComponent();
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
            this.GridSizeUpDown = new System.Windows.Forms.NumericUpDown();
            ((System.ComponentModel.ISupportInitialize)(this.GridSizeUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // GridSizeUpDown
            // 
            this.GridSizeUpDown.Location = new System.Drawing.Point(16, 8);
            this.GridSizeUpDown.Maximum = new System.Decimal(new int[] {
                                                                            50,
                                                                            0,
                                                                            0,
                                                                            0});
            this.GridSizeUpDown.Name = "GridSizeUpDown";
            this.GridSizeUpDown.Size = new System.Drawing.Size(136, 20);
            this.GridSizeUpDown.TabIndex = 0;
            this.GridSizeUpDown.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.FrmGridSize_KeyPress);
            this.GridSizeUpDown.ValueChanged += new System.EventHandler(this.GridSizeUpDown_ValueChanged);
            // 
            // FrmGridSize
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(168, 42);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.GridSizeUpDown});
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FrmGridSize";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Grid Size";
            this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.FrmGridSize_KeyPress);
            this.Load += new System.EventHandler(this.FrmGridSize_Load);
            ((System.ComponentModel.ISupportInitialize)(this.GridSizeUpDown)).EndInit();
            this.ResumeLayout(false);

        }
		#endregion

        private void GridSizeUpDown_ValueChanged(object sender, System.EventArgs e)
        {

        }

        private void FrmGridSize_Load(object sender, System.EventArgs e)
        {
        
        }

        private void button1_Click(object sender, System.EventArgs e)
        {
            Close();
        }

        private void FrmGridSize_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)27)
                Close();
        }
	}
}
