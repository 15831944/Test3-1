using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using MindFusion.FlowChartX;
using System.Drawing.Drawing2D;
using SysCAD.ThingEditor.ThingEditor;

using SysCAD.Interface;

namespace SysCAD.ThingEditor
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class ThingEditorForm : System.Windows.Forms.Form
  {
        private System.Windows.Forms.Splitter splitter1;
    private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel5;
        private System.Windows.Forms.Splitter splitter3;
        private System.Windows.Forms.Panel panel6;
        private System.Windows.Forms.MainMenu mainMenu1;
    private System.Windows.Forms.MenuItem menuItem1;
    private System.Windows.Forms.MenuItem menuItem3;
        private System.Windows.Forms.MenuItem menuItem7;
        private System.Windows.Forms.MenuItem menuItem8;
        private System.Windows.Forms.MenuItem menuItem9;
        private System.Windows.Forms.MenuItem menuItem10;
        private System.Windows.Forms.MenuItem menuItem11;
        private System.Windows.Forms.MenuItem menuItem12;
        private System.Windows.Forms.MenuItem menuItem13;
    private MindFusion.FlowChartX.FlowChart flowChart1;
    private SysCAD.ThingEditor.ThingEditorCtrl thingEditor1;
    private MenuItem menuItem6;
    private IContainer components;

    public GraphicThing graphicThing;

    public ThingEditorForm(GraphicThing graphicThing)
		{
      this.graphicThing = graphicThing;
			//jj
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
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

    protected override void OnClosing(CancelEventArgs e)
    {
      // Write information back into the CraphicItem...
      thingEditor1.SetGraphicThing();

      base.OnClosing(e);
    }

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ThingEditorForm));
      this.splitter1 = new System.Windows.Forms.Splitter();
      this.panel2 = new System.Windows.Forms.Panel();
      this.panel6 = new System.Windows.Forms.Panel();
      this.flowChart1 = new MindFusion.FlowChartX.FlowChart();
      this.splitter3 = new System.Windows.Forms.Splitter();
      this.panel5 = new System.Windows.Forms.Panel();
      this.thingEditor1 = new SysCAD.ThingEditor.ThingEditorCtrl(graphicThing);
      this.mainMenu1 = new System.Windows.Forms.MainMenu(this.components);
      this.menuItem1 = new System.Windows.Forms.MenuItem();
      this.menuItem3 = new System.Windows.Forms.MenuItem();
      this.menuItem6 = new System.Windows.Forms.MenuItem();
      this.menuItem10 = new System.Windows.Forms.MenuItem();
      this.menuItem9 = new System.Windows.Forms.MenuItem();
      this.menuItem8 = new System.Windows.Forms.MenuItem();
      this.menuItem11 = new System.Windows.Forms.MenuItem();
      this.menuItem12 = new System.Windows.Forms.MenuItem();
      this.menuItem13 = new System.Windows.Forms.MenuItem();
      this.menuItem7 = new System.Windows.Forms.MenuItem();
      this.panel2.SuspendLayout();
      this.panel6.SuspendLayout();
      this.panel5.SuspendLayout();
      this.SuspendLayout();
      // 
      // splitter1
      // 
      this.splitter1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.splitter1.Location = new System.Drawing.Point(0, 397);
      this.splitter1.Name = "splitter1";
      this.splitter1.Size = new System.Drawing.Size(832, 6);
      this.splitter1.TabIndex = 1;
      this.splitter1.TabStop = false;
      // 
      // panel2
      // 
      this.panel2.Controls.Add(this.panel6);
      this.panel2.Controls.Add(this.splitter3);
      this.panel2.Controls.Add(this.panel5);
      this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel2.Location = new System.Drawing.Point(0, 0);
      this.panel2.Name = "panel2";
      this.panel2.Size = new System.Drawing.Size(832, 397);
      this.panel2.TabIndex = 2;
      // 
      // panel6
      // 
      this.panel6.Controls.Add(this.flowChart1);
      this.panel6.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel6.Location = new System.Drawing.Point(502, 0);
      this.panel6.Name = "panel6";
      this.panel6.Size = new System.Drawing.Size(330, 397);
      this.panel6.TabIndex = 2;
      // 
      // flowChart1
      // 
      this.flowChart1.ActiveMnpColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
      this.flowChart1.BoxStyle = MindFusion.FlowChartX.BoxStyle.Shape;
      this.flowChart1.CurrentCursor = System.Windows.Forms.Cursors.Default;
      this.flowChart1.Cursor = System.Windows.Forms.Cursors.Default;
      this.flowChart1.DefaultControlType = typeof(System.Windows.Forms.Button);
      this.flowChart1.DocExtents = ((System.Drawing.RectangleF)(resources.GetObject("flowChart1.DocExtents")));
      this.flowChart1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowChart1.Font = new System.Drawing.Font("Microsoft Sans Serif", 4F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World, ((byte)(0)));
      this.flowChart1.InplaceEditFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.flowChart1.Location = new System.Drawing.Point(0, 0);
      this.flowChart1.Name = "flowChart1";
      this.flowChart1.ShowToolTips = false;
      this.flowChart1.Size = new System.Drawing.Size(330, 397);
      this.flowChart1.TabIndex = 0;
      this.flowChart1.TableFrameColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
      this.flowChart1.TableHandlesStyle = MindFusion.FlowChartX.HandlesStyle.HatchFrame;
      // 
      // splitter3
      // 
      this.splitter3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.splitter3.Location = new System.Drawing.Point(496, 0);
      this.splitter3.Name = "splitter3";
      this.splitter3.Size = new System.Drawing.Size(6, 397);
      this.splitter3.TabIndex = 1;
      this.splitter3.TabStop = false;
      // 
      // panel5
      // 
      this.panel5.Controls.Add(this.thingEditor1);
      this.panel5.Dock = System.Windows.Forms.DockStyle.Left;
      this.panel5.Location = new System.Drawing.Point(0, 0);
      this.panel5.Name = "panel5";
      this.panel5.Size = new System.Drawing.Size(496, 397);
      this.panel5.TabIndex = 0;
      // 
      // thingEditor1
      // 
      this.thingEditor1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.thingEditor1.GridColor = System.Drawing.Color.Black;
      this.thingEditor1.GridSize = 16;
      this.thingEditor1.GridStyle = SysCAD.ThingEditor.ThingEditorCtrl.EGridStyle.GrdLine;
      this.thingEditor1.Location = new System.Drawing.Point(0, 0);
      this.thingEditor1.Name = "thingEditor1";
      this.thingEditor1.PenColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(220)))), ((int)(((byte)(255)))));
      //this.thingEditor1.ShowScrollbars = true;
      this.thingEditor1.Size = new System.Drawing.Size(496, 397);
      this.thingEditor1.TabIndex = 0;
      this.thingEditor1.Text = "thingEditor1";
      this.thingEditor1.ShapeChanged += new SysCAD.ThingEditor.ShapeChangeEventHandler(this.thingEditor1_ShapeChanged);
      // 
      // mainMenu1
      // 
      this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.menuItem1});
      // 
      // menuItem1
      // 
      this.menuItem1.Index = 0;
      this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.menuItem3,
            this.menuItem6,
            this.menuItem10,
            this.menuItem9,
            this.menuItem8,
            this.menuItem11,
            this.menuItem12,
            this.menuItem13,
            this.menuItem7});
      this.menuItem1.Text = "&File";
      // 
      // menuItem3
      // 
      this.menuItem3.Index = 0;
      this.menuItem3.Text = "&New";
      this.menuItem3.Click += new System.EventHandler(this.menuItem3_Click);
      // 
      // menuItem6
      // 
      this.menuItem6.Index = 1;
      this.menuItem6.Text = "-";
      // 
      // menuItem10
      // 
      this.menuItem10.Index = 2;
      this.menuItem10.Text = "Background Color";
      this.menuItem10.Click += new System.EventHandler(this.menuItem10_Click);
      // 
      // menuItem9
      // 
      this.menuItem9.Index = 3;
      this.menuItem9.Text = "Grid Color";
      this.menuItem9.Click += new System.EventHandler(this.menuItem9_Click);
      // 
      // menuItem8
      // 
      this.menuItem8.Index = 4;
      this.menuItem8.Text = "Pen Color";
      this.menuItem8.Click += new System.EventHandler(this.menuItem8_Click);
      // 
      // menuItem11
      // 
      this.menuItem11.Index = 5;
      this.menuItem11.Text = "-";
      // 
      // menuItem12
      // 
      this.menuItem12.Index = 6;
      this.menuItem12.Text = "Set Grid Size";
      this.menuItem12.Click += new System.EventHandler(this.menuItem12_Click);
      // 
      // menuItem13
      // 
      this.menuItem13.Index = 7;
      this.menuItem13.Text = "-";
      // 
      // menuItem7
      // 
      this.menuItem7.Index = 8;
      this.menuItem7.Text = "Exit";
      this.menuItem7.Click += new System.EventHandler(this.menuItem7_Click);
      // 
      // Form1
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(832, 403);
      this.Controls.Add(this.panel2);
      this.Controls.Add(this.splitter1);
      this.Menu = this.mainMenu1;
      this.Name = "Form1";
      this.Text = "Thing Editor";
      this.panel2.ResumeLayout(false);
      this.panel6.ResumeLayout(false);
      this.panel5.ResumeLayout(false);
      this.ResumeLayout(false);

		}
		#endregion

        private void thingEditor1_ShapeChanged(object sender, System.EventArgs e)
        {
            flowChart1.ClearAll();
            

            flowChart1.DefaultShape = thingEditor1.CreateShapeTemplate();

            Box b = flowChart1.CreateBox(0, 0, 70, 70);
            b.PolyTextLayout = true;
            b.Text = graphicThing.Text;
        }

        private void menuItem3_Click(object sender, System.EventArgs e)
        {
          thingEditor1.DeleteAllDecorations(sender);
        }

        private void menuItem7_Click(object sender, System.EventArgs e)
        {
          // Write information back into the CraphicItem...
          thingEditor1.SetGraphicThing();

            Close();
        }

        private void menuItem2_Click(object sender, System.EventArgs e)
        {
            FrmHelp fh = new FrmHelp();
            fh.ShowDialog(this);
        }

        private void menuItem10_Click(object sender, System.EventArgs e)
        {
            ColorDialog colorDialog = new ColorDialog();
            if (colorDialog.ShowDialog() == DialogResult.OK)
                thingEditor1.BackColor = colorDialog.Color;
            colorDialog.Dispose();
        }

        private void menuItem9_Click(object sender, System.EventArgs e)
        {
            ColorDialog colorDialog = new ColorDialog();
            if (colorDialog.ShowDialog() == DialogResult.OK)
                thingEditor1.GridColor = colorDialog.Color;
            colorDialog.Dispose();
        }

        private void menuItem8_Click(object sender, System.EventArgs e)
        {
            ColorDialog colorDialog = new ColorDialog();
            if (colorDialog.ShowDialog() == DialogResult.OK)
                thingEditor1.PenColor = colorDialog.Color;
            colorDialog.Dispose();
        }

        private void menuItem12_Click(object sender, System.EventArgs e)
        {
            FrmGridSize frmGridSize = new FrmGridSize();
            frmGridSize.GridSizeUpDown.Value = thingEditor1.GridSize;
            if (frmGridSize.ShowDialog()== DialogResult.Cancel)
                thingEditor1.GridSize = (int)frmGridSize.GridSizeUpDown.Value;
        }


	}
}
