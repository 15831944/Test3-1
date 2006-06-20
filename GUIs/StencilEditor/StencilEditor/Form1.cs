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
        decorationAreaLineX1ToolStripTextBox.Text = line.x1.ToString("N2");
        decorationAreaLineY1ToolStripTextBox.Text = line.x2.ToString("N2");
        decorationAreaLineX2ToolStripTextBox.Text = line.y1.ToString("N2");
        decorationAreaLineY2ToolStripTextBox.Text = line.y2.ToString("N2");

        decorationAreaLineX1ToolStripTextBox.BackColor = DefaultBackColor;
        decorationAreaLineY1ToolStripTextBox.BackColor = DefaultBackColor;
        decorationAreaLineX2ToolStripTextBox.BackColor = DefaultBackColor;
        decorationAreaLineY2ToolStripTextBox.BackColor = DefaultBackColor;
        toolStripButton1.Enabled = true;
        decorationAreaLineToolStrip.Visible = true;
      }
      else
      {
        decorationAreaLineToolStrip.Visible = false;
      }
    }

    private void decorationAreaLineToolStripTextBox_TextChanged(object sender, EventArgs e)
    {
      try
      {
        Line line = DecorationAreaListBox.SelectedItem as Line;
        line.x1 = Single.Parse(decorationAreaLineX1ToolStripTextBox.Text);
        line.y1 = Single.Parse(decorationAreaLineY1ToolStripTextBox.Text);
        line.x2 = Single.Parse(decorationAreaLineX2ToolStripTextBox.Text);
        line.y2 = Single.Parse(decorationAreaLineY2ToolStripTextBox.Text);

        decorationAreaLineX1ToolStripTextBox.BackColor = DefaultBackColor;
        toolStripButton1.Enabled = true;
      }
      catch
      {
        decorationAreaLineX1ToolStripTextBox.BackColor = Color.Yellow;
        toolStripButton1.Enabled = false;
      }
    }

    private void toolStripButton1_Click(object sender, EventArgs e)
    {
      Line line = DecorationAreaListBox.SelectedItem as Line;
      line.x1 = Single.Parse(decorationAreaLineX1ToolStripTextBox.Text);
      line.y1 = Single.Parse(decorationAreaLineY1ToolStripTextBox.Text);
      line.x2 = Single.Parse(decorationAreaLineX2ToolStripTextBox.Text);
      line.y2 = Single.Parse(decorationAreaLineY2ToolStripTextBox.Text);

      DecorationAreaListBox.Items[DecorationAreaListBox.SelectedIndex] = line;
      DecorationAreaListBox.Refresh();
    }
  }
}