using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace TestApp
{
  public partial class TestAppForm : Form
  {
    public TestAppForm(Dictionary<String, Bitmap> thumbnails16, Dictionary<String, Bitmap> thumbnails32)
    {
      InitializeComponent();

      ImageList imageList16 = new ImageList();
      imageList16.ImageSize = new Size(16, 16);
      //imageList16.TransparentColor = Color.FromArgb(170, 170, 200);
      foreach (String key in thumbnails16.Keys)
      {
        imageList16.Images.Add(key, thumbnails16[key]);
      }

      ImageList imageList32 = new ImageList();
      imageList32.ImageSize = new Size(32, 32);
      //imageList32.TransparentColor = Color.FromArgb(170,170, 200);
      foreach (String key in thumbnails32.Keys)
      {
        imageList32.Images.Add(key, thumbnails32[key]);
      }

      stencilChooser.listView.SmallImageList = imageList16;
      stencilChooser.listView.LargeImageList = imageList32;
      //stencilChooser.listView.StateImageList = imageList;

      stencilChooser.listView.View = View.List;

      foreach (String key in thumbnails16.Keys)
      {
        stencilChooser.listView.Items.Add(new ListViewItem(key, imageList16.Images.IndexOfKey(key)));
      }
    }
  }
}