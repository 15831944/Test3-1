using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using SysCAD.Protocol;

namespace SysCAD
{
  public partial class StencilChooser : UserControl
  {
    Dictionary<String, Bitmap> modelThumbnails;
    Dictionary<String, Bitmap> graphicThumbnails;
    Dictionary<String, ModelStencil> modelStencils;
    Dictionary<String, GraphicStencil> graphicStencils;

    ImageList doubleWideImageList;

    public StencilChooser(Dictionary<String, Bitmap> modelThumbnails, Dictionary<String, Bitmap> graphicThumbnails,
                       Dictionary<String, ModelStencil> modelStencils, Dictionary<String, GraphicStencil> graphicStencils)
    {
      this.modelThumbnails = modelThumbnails;
      this.graphicThumbnails = graphicThumbnails;
      this.modelStencils = modelStencils;
      this.graphicStencils = graphicStencils;

      InitializeComponent();

      ImageList imageList = new ImageList();
      imageList.ImageSize = new Size(16, 16);
      foreach (String key in modelThumbnails.Keys)
      {
        imageList.Images.Add("Model: " + key, modelThumbnails[key]);
      }
      foreach (String key in graphicThumbnails.Keys)
      {
        imageList.Images.Add("Graphic: " + key, graphicThumbnails[key]);
      }

      listView.ImageList = imageList;

      foreach (String key in modelStencils.Keys)
      {
        String group = modelStencils[key].GroupName;
        if (group.Length == 0)
          group = "None";

        TreeNode groupNode;
        if (listView.Nodes.ContainsKey("Group: " + group))
        {
          groupNode = listView.Nodes["Group: " + group];
        }
        else
        {
          groupNode = listView.Nodes.Add("Group: " + group, group);
        }

        groupNode.Nodes.Add("Model: " + key, key, "Model: " + key, "Model: " + key);
      }

      foreach (String key in graphicStencils.Keys)
      {
        String group = graphicStencils[key].groupName;

        TreeNode groupNode;
        if (listView.Nodes.ContainsKey("Group: " + group))
        {
          groupNode = listView.Nodes["Group: " + group];

          foreach (TreeNode modelNode in groupNode.Nodes)
          {
            modelNode.Nodes.Add("Model: " + modelNode.Text + "; Graphic: " + key, key, "Graphic: " + key, "Graphic: " + key);
          }
        }
      }

      doubleWideImageList = new ImageList();
      doubleWideImageList.ImageSize = new Size(32, 16);
      listBox1.SmallImageList = doubleWideImageList;
    }

    private void listView_AfterSelect(object sender, TreeViewEventArgs e)
    {
      label1.Text = "";
      pictureBox2.Visible = false;
      pictureBox1.Visible = false;

      String key = e.Node.ImageKey;

      if (key.Contains("Graphic: "))
      {
        key = key.Remove(0, 9);

        if (graphicThumbnails.ContainsKey(key))
        {
          pictureBox2.Image = graphicThumbnails[key];
          pictureBox2.Visible = true;
        }

        label1.Text = " : " + key;

        key = e.Node.Parent.ImageKey;
      }
      else if (key.Contains("Model: "))
      {
        key = key.Remove(0, 7);

        if (graphicThumbnails.ContainsKey(key))
        {
          pictureBox2.Image = graphicThumbnails[key];
          pictureBox2.Visible = true;
        }

        label1.Text = " : " + key;

        key = e.Node.ImageKey;
      }

      if (key.Contains("Model: "))
      {
        key = key.Remove(0, 7);

        if (modelThumbnails.ContainsKey(key))
        {
          pictureBox1.Image = modelThumbnails[key];
          pictureBox1.Visible = true;
        }

        label1.Text = key + label1.Text;
      }
    }

    private void pictureBox1_Click(object sender, EventArgs e)
    {
      panel1_Click(sender, e);
    }

    private void pictureBox2_Click(object sender, EventArgs e)
    {
      panel1_Click(sender, e);
    }

    private void label1_Click(object sender, EventArgs e)
    {
      panel1_Click(sender, e);
    }

    private void panel1_Click(object sender, EventArgs e)
    {
      if (listBox1.Items.ContainsKey(label1.Text))
        listBox1.Items.RemoveByKey(label1.Text);

      if (!doubleWideImageList.Images.ContainsKey(label1.Text))
      {
        Bitmap doubleWideBitmap = new Bitmap(32, 16);
        Graphics doubleWideGraphics = Graphics.FromImage(doubleWideBitmap);
        doubleWideGraphics.DrawImage(pictureBox1.Image, 0, 0);
        doubleWideGraphics.DrawImage(pictureBox2.Image, 16, 0);
        doubleWideImageList.Images.Add(label1.Text, doubleWideBitmap);
      }

      ListViewItem item = listBox1.Items.Insert(0, label1.Text, label1.Text, label1.Text);
    }
  }
}