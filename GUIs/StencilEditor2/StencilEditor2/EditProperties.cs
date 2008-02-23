using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using SysCAD.Protocol;

namespace StencilEditor2
{
  public partial class EditProperties : Form
  {
    public String group;
    public String tag;
    public ArrayList anchors;

    public EditProperties()
    {
      InitializeComponent();

      comboBox1.SelectedIndex = 0;
    }

    private void textBox1_TextChanged(object sender, EventArgs e)
    {
      group = textBox1.Text;
    }

    private void textBox2_TextChanged(object sender, EventArgs e)
    {
      tag = textBox2.Text;
    }

    private void textBox3_TextChanged(object sender, EventArgs e)
    {
      button1.Enabled = ((textBox3.Text.Length > 0) && (!listView1.Items.ContainsKey(textBox3.Text)));
      button2.Enabled = ((textBox3.Text.Length > 0) && (listView1.Items.ContainsKey(textBox3.Text)));
      comboBox1.Enabled = textBox3.Text.Length > 0;

      listView1.SelectedItems.Clear();

      if ((textBox3.Text.Length > 0) && (listView1.Items.ContainsKey(textBox3.Text)))
        listView1.Items.Find(textBox3.Text, false)[0].Selected = true;
    }

    private void EditProperties_Shown(object sender, EventArgs e)
    {
      textBox1.Text = group;
      textBox2.Text = tag;

      foreach (Anchor anchor in anchors)
      {
        ListViewItem item = listView1.Items.Add(anchor.Tag, anchor.Tag, 0);
        item.Group = listView1.Groups[anchor.Look];
      }

    }

    private void listView1_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (listView1.SelectedItems.Count > 0)
      {
        textBox3.Text = listView1.SelectedItems[0].Text;
        comboBox1.SelectedIndex = comboBox1.FindStringExact(listView1.SelectedItems[0].Group.Header);
      }

      //textBox3.Focus();
    }

    private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (listView1.SelectedItems.Count > 0)
      {
        listView1.SelectedItems[0].Group = listView1.Groups[comboBox1.SelectedIndex];
      }
    }

    private void button2_Click(object sender, EventArgs e)
    {
      listView1.SelectedItems[0].Remove();
      textBox3.Text = "";
    }

    private void button1_Click(object sender, EventArgs e)
    {
      ListViewItem item = listView1.Items.Add(textBox3.Text, textBox3.Text, 0);
      item.Group = listView1.Groups[comboBox1.SelectedIndex];
    }
  }
}
