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
    Dictionary<String, Bitmap> modelUnselectedThumbnails;
    Dictionary<String, Bitmap> modelSelectedThumbnails;
    Dictionary<String, Bitmap> graphicUnselectedThumbnails;
    Dictionary<String, Bitmap> graphicSelectedThumbnails;
    Dictionary<String, ModelStencil> modelStencils;
    Dictionary<String, GraphicStencil> graphicStencils;

    ImageList doubleWideImageList;
    ImageList modelImageList;
    ImageList graphicImageList;

    public StencilChooser(Dictionary<String, Bitmap> modelUnselectedThumbnails, Dictionary<String, Bitmap> modelSelectedThumbnails,
      Dictionary<String, Bitmap> graphicUnselectedThumbnails, Dictionary<String, Bitmap> graphicSelectedThumbnails,
      Dictionary<String, ModelStencil> modelStencils, Dictionary<String, GraphicStencil> graphicStencils)
    {
      this.modelUnselectedThumbnails = modelUnselectedThumbnails;
      this.modelSelectedThumbnails = modelSelectedThumbnails;
      this.graphicUnselectedThumbnails = graphicUnselectedThumbnails;
      this.graphicSelectedThumbnails = graphicSelectedThumbnails;
      this.modelStencils = modelStencils;
      this.graphicStencils = graphicStencils;

      InitializeComponent();

      modelImageList = new ImageList();
      modelImageList.ImageSize = new Size(16, 16);
      modelImageList.Images.Add("Empty", new Bitmap(16, 16)); // Used when no imagekey listed.
      foreach (String key in modelUnselectedThumbnails.Keys)
      {
        modelImageList.Images.Add("Unselected: " + key, modelUnselectedThumbnails[key]);
        modelImageList.Images.Add("Selected: " + key, modelSelectedThumbnails[key]);
      }
      modelTreeView.ImageList = modelImageList;

      graphicImageList = new ImageList();
      graphicImageList.ImageSize = new Size(16, 16);
      graphicImageList.Images.Add("Empty", new Bitmap(16, 16)); // Used when no imagekey listed.
      foreach (String key in graphicUnselectedThumbnails.Keys)
      {
        graphicImageList.Images.Add("Unselected: " + key, graphicUnselectedThumbnails[key]);
        graphicImageList.Images.Add("Selected: " + key, graphicSelectedThumbnails[key]);
      }
      graphicTreeView.ImageList = graphicImageList;

      foreach (String key in modelStencils.Keys)
      {
        String group = modelStencils[key].GroupName;
        if (group == String.Empty)
          group = "None";

        TreeNode groupNode;
        if (modelTreeView.Nodes.ContainsKey("Group: " + group))
        {
          groupNode = modelTreeView.Nodes["Group: " + group];
        }
        else
        {
          groupNode = modelTreeView.Nodes.Add("Group: " + group, group);
        }

        groupNode.Nodes.Add(key, key, "Unselected: " + key, "Selected: " + key);
      }

      foreach (String key in graphicStencils.Keys)
      {
        String group = modelStencils[key].GroupName;
        if (group == String.Empty)
          group = "None";

        {
          TreeNode groupNode;
          if (graphicTreeView.Nodes.ContainsKey("Group: " + group))
          {
            groupNode = graphicTreeView.Nodes["Group: " + group];
          }
          else
          {
            groupNode = graphicTreeView.Nodes.Add("Group: " + group, group);
          }

          groupNode.Nodes.Add(key, key, "Unselected: " + key, "Selected: " + key);
        }
      }

      doubleWideImageList = new ImageList();
      doubleWideImageList.ImageSize = new Size(32, 16);
      mruListBox.SmallImageList = doubleWideImageList;
    }

    private void SetCurrent(String label, Image graphicImage, Image modelImage)
    {
      currentLabel.Text = label;
      currentGraphiclPictureBox.Visible = true;
      currentGraphiclPictureBox.Image = graphicImage;
      currentModelPictureBox.Visible = true;
      currentModelPictureBox.Image = modelImage;
    }

    private void UnsetCurrent()
    {
      currentLabel.Text = "";
      currentGraphiclPictureBox.Visible = false;
      currentModelPictureBox.Visible = false;
    }

    private void modelTreeView_BeforeSelect(object sender, TreeViewCancelEventArgs e)
    {
      if (e.Node.ImageKey == String.Empty)
      {
        if (e.Action == TreeViewAction.ByMouse)
        {
          if (e.Node.IsExpanded)
            e.Node.Collapse();
          else
            e.Node.Expand();
        }
        e.Node.EnsureVisible();
        e.Cancel = true;
      }
    }

    private void graphicTreeView_BeforeSelect(object sender, TreeViewCancelEventArgs e)
    {
      if (e.Node.ImageKey == String.Empty)
      {
        if (e.Action == TreeViewAction.ByMouse)
        {
          if (e.Node.IsExpanded)
            e.Node.Collapse();
          else
            e.Node.Expand();
        }
        e.Node.EnsureVisible();
        e.Cancel = true;
      }
    }

    private string GetKey(TreeNode node)
    {
      String key = node.ImageKey; // For some reason Key isn't available, using ImageKey instead.
      if (key.StartsWith("Unselected: "))
        return key.Remove(0, 12);
      return key;
    }

    private void modelTreeView_AfterSelect(object sender, TreeViewEventArgs e)
    {
      if (e.Node.IsSelected)
      {
        graphicTreeView.Enabled = true;

        String key = GetKey(e.Node);

        String group = modelStencils[key].GroupName;
        if (group == String.Empty)
          group = "None";

        TreeNode groupNode = graphicTreeView.Nodes["Group: " + group];
        groupNode.Expand();
        groupNode.EnsureVisible();
        if (groupNode.Nodes.ContainsKey(key))
          graphicTreeView.SelectedNode = groupNode.Nodes[key];
        // ToDo: Scroll through mru and select most recently used graphic for this model instead.
      }
      else
      {
        graphicTreeView.Enabled = false;
      }
    }

    private void graphicTreeView_AfterSelect(object sender, TreeViewEventArgs e)
    {
      String key = GetKey(e.Node);

      TreeNode selectedModelNode = modelTreeView.SelectedNode;
      TreeNode selectedGraphicNode = graphicTreeView.SelectedNode;
      if ((selectedModelNode.ImageKey.StartsWith("Unselected: ")) &&
        (selectedGraphicNode.ImageKey.StartsWith("Unselected: ")))
      {
        SetCurrent(GetKey(selectedModelNode) + " | " + GetKey(selectedGraphicNode),
          graphicImageList.Images[selectedGraphicNode.ImageKey],
          modelImageList.Images[selectedModelNode.ImageKey]);
      }
      else
      {
        UnsetCurrent();
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
      if (mruListBox.Items.ContainsKey(currentLabel.Text))
        mruListBox.Items.RemoveByKey(currentLabel.Text);

      if (!doubleWideImageList.Images.ContainsKey(currentLabel.Text))
      {
        Bitmap doubleWideBitmap = new Bitmap(32, 16);
        Graphics doubleWideGraphics = Graphics.FromImage(doubleWideBitmap);
        doubleWideGraphics.DrawImage(currentModelPictureBox.Image, 0, 0);
        doubleWideGraphics.DrawImage(currentGraphiclPictureBox.Image, 16, 0);
        doubleWideImageList.Images.Add(currentLabel.Text, doubleWideBitmap);
      }

      ListViewItem item = mruListBox.Items.Insert(0, currentLabel.Text, currentLabel.Text, currentLabel.Text);

      while (mruListBox.Items.Count > 10)
      {
        mruListBox.Items.RemoveAt(mruListBox.Items.Count-1);
      }
    }

    private void mruListBox_Resize(object sender, EventArgs e)
    {
      itemColumnHeader.Width = mruListBox.Width - 4;
    }
  }
}