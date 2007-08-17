using SysCAD.Protocol;
using System.Collections.Generic;
using System;
using System.Drawing;
namespace TestApp
{
  partial class TestAppForm
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent(Dictionary<String, Bitmap> modelThumbnails, Dictionary<String, Bitmap> graphicThumbnails,
                       Dictionary<String, ModelStencil> modelStencils, Dictionary<String, GraphicStencil> graphicStencils)
    {
      this.stencilChooser = new SysCAD.StencilChooser(modelThumbnails, graphicThumbnails, modelStencils, graphicStencils);
      this.SuspendLayout();
      // 
      // stencilListView
      // 
      this.stencilChooser.Dock = System.Windows.Forms.DockStyle.Fill;
      this.stencilChooser.Location = new System.Drawing.Point(0, 0);
      this.stencilChooser.Name = "stencilListView";
      this.stencilChooser.TabIndex = 0;
      // 
      // TestApp
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(200, 600);
      this.Controls.Add(this.stencilChooser);
      this.Name = "TestApp";
      this.Text = "TestApp";
      this.ResumeLayout(false);

    }

    #endregion

    private SysCAD.StencilChooser stencilChooser;
  }
}

