using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SysCAD.Protocol;

namespace SysCAD.Editor
{
  public partial class GraphicStencilChangeForm : Form
  {
    private Dictionary<String, Bitmap> graphicUnselectedThumbnails;
    private Dictionary<String, Bitmap> graphicSelectedThumbnails;
    private String value;

    public String Value
    {
      get { return this.value; }
    }

    ImageList graphicImageList;

    public GraphicStencilChangeForm(Dictionary<String, Bitmap> graphicUnselectedThumbnails,
      Dictionary<String, Bitmap> graphicSelectedThumbnails,
      Dictionary<String, ModelStencil> modelStencils,
      Dictionary<String, GraphicStencil> graphicStencils,
      String value)
    {
      this.graphicUnselectedThumbnails = graphicUnselectedThumbnails;
      this.graphicSelectedThumbnails = graphicSelectedThumbnails;
      this.value = value;

      InitializeComponent();

      graphicImageList = new ImageList();
      graphicImageList.ImageSize = new System.Drawing.Size(16, 16);
      graphicImageList.Images.Add("Empty", new Bitmap(16, 16)); // Used when no imagekey listed.
      foreach (String key in graphicUnselectedThumbnails.Keys)
      {
        graphicImageList.Images.Add("Unselected: " + key, graphicUnselectedThumbnails[key]);
        graphicImageList.Images.Add("Selected: " + key, graphicSelectedThumbnails[key]);
      }
      graphicTreeView.ImageList = graphicImageList;

      foreach (String key in graphicStencils.Keys)
      {
        String group = modelStencils[key].GroupName;
        if (String.IsNullOrEmpty(group))
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
    }
 
    private void graphicTreeView_BeforeSelect(object sender, TreeViewCancelEventArgs e)
    {
      if (String.IsNullOrEmpty(e.Node.ImageKey))
      {
        if (e.Action == TreeViewAction.ByMouse)
        {
          if (e.Node.IsExpanded)
            e.Node.Collapse();
          else
            e.Node.Expand();
        }
        graphicTreeView.SelectedNode = null;
        e.Node.EnsureVisible();
        e.Cancel = true;
        okButton.Enabled = false;
      }
    }

    private static string GetKey(TreeNode node)
    {
      String key = node.ImageKey; // For some reason Key isn't available, using ImageKey instead.
      if (key.StartsWith("Unselected: "))
        return key.Remove(0, 12);
      return key;
    }

    private void graphicTreeView_AfterSelect(object sender, TreeViewEventArgs e)
    {
      value = GetKey(e.Node);
      okButton.Enabled = true;
    }
  }
}