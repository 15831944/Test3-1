using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Interface;

namespace StencilEditor
{
  public partial class Form1 : Form
  {
    GraphicStencil graphicStencil = new GraphicStencil();

    public Form1()
    {
      InitializeComponent();
    }

    private void NewDecorationLineToolStripButton_Click(object sender, EventArgs e)
    {
      Line line = new Line(0.0F, 0.0F, 0.0F, 0.0F);
      int index = DecorationAreaListBox.Items.Add(line);
      DecorationAreaListBox.SetSelected(index, true);
    }

    private void DecorationAreaListBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (DecorationAreaListBox.SelectedItem is Line)
      {
        Line line = DecorationAreaListBox.SelectedItem as Line;
        toolStripTextBox1.Text = line.x1.ToString("N2");
        toolStripTextBox2.Text = line.x2.ToString("N2");
        toolStripTextBox3.Text = line.y1.ToString("N2");
        toolStripTextBox4.Text = line.y2.ToString("N2");

        toolStripTextBox1.BackColor = DefaultBackColor;
        toolStripTextBox2.BackColor = DefaultBackColor;
        toolStripTextBox3.BackColor = DefaultBackColor;
        toolStripTextBox4.BackColor = DefaultBackColor;
        toolStripButton1.Enabled = true;
        toolStrip4.Visible = true;
      }
      else
      {
        toolStrip4.Visible = false;
      }
    }

    private void toolStripTextBox_TextChanged(object sender, EventArgs e)
    {
      try
      {
        Line line = DecorationAreaListBox.SelectedItem as Line;
        line.x1 = Single.Parse(toolStripTextBox1.Text);
        line.y1 = Single.Parse(toolStripTextBox2.Text);
        line.x2 = Single.Parse(toolStripTextBox3.Text);
        line.y2 = Single.Parse(toolStripTextBox4.Text);

        toolStripTextBox1.BackColor = DefaultBackColor;
        toolStripButton1.Enabled = true;
      }
      catch
      {
        toolStripTextBox1.BackColor = Color.Yellow;
        toolStripButton1.Enabled = false;
      }
    }

    private void toolStripButton1_Click(object sender, EventArgs e)
    {
      Line line = DecorationAreaListBox.SelectedItem as Line;
      line.x1 = Single.Parse(toolStripTextBox1.Text);
      line.y1 = Single.Parse(toolStripTextBox2.Text);
      line.x2 = Single.Parse(toolStripTextBox3.Text);
      line.y2 = Single.Parse(toolStripTextBox4.Text);

      DecorationAreaListBox.Items[DecorationAreaListBox.SelectedIndex] = line;
      DecorationAreaListBox.Refresh();
    }
  }
}