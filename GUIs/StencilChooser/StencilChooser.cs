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
    Dictionary<String, ModelStencil> modelStencils;

    public StencilChooser()
    {
      InitializeComponent();
    }

    public void PopulateTree(Dictionary<String, Bitmap> modelThumbnails,
                       Dictionary<String, ModelStencil> modelStencils)
    {
      this.modelThumbnails = modelThumbnails;
      this.modelStencils = modelStencils;

      ImageList imageList = new ImageList();
      imageList.ImageSize = new System.Drawing.Size(16, 16);
      foreach (String key in modelThumbnails.Keys)
      {
        imageList.Images.Add("Model: " + key, modelThumbnails[key]);
      }

      stencilChooserTreeView.ImageList = imageList;

      foreach (String key in modelStencils.Keys)
      {
        String group = modelStencils[key].GroupName;
        if ((group == null) || (group.Length == 0))
          group = "!!None!!";

        TreeNode groupNode;
        if (stencilChooserTreeView.Nodes.ContainsKey("Group: " + group))
        {
          groupNode = stencilChooserTreeView.Nodes["Group: " + group];
        }
        else
        {
          groupNode = stencilChooserTreeView.Nodes.Add("Group: " + group, group);
        }

        TreeNode modelNode = groupNode.Nodes.Add("Model: " + key, key, "Model: " + key, "Model: " + key);
      }
    }

    private void stencilChooserTreeView_ItemDrag(object sender, ItemDragEventArgs e)
    {
      ModelStencil modelStencil;
      if (modelStencils.TryGetValue(((TreeNode)e.Item).Text, out modelStencil))
      {
        this.DoDragDrop(modelStencil, DragDropEffects.Move);
      }
    }
  }
}
