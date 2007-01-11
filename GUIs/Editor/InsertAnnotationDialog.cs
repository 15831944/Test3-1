using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Interface;
using System.Text.RegularExpressions;

namespace SysCAD.Editor
{
  public partial class InsertAnnotationDialog : Form
  {
    private State state;

    private String thingTag;
    private String thingXaml;

    private float defaultWidth;
    private float defaultHeight;

    public String ThingTag
    {
      get { return thingTag; }
      set { thingTag = value; }
    }

    public String ThingXaml
    {
      get { return thingXaml; }
      set { thingXaml = value; }
    }

    public float DefaultWidth
    {
      get { return defaultWidth; }
      set { defaultWidth = value; }
    }

    public float DefaultHeight
    {
      get { return defaultHeight; }
      set { defaultHeight = value; }
    }


    public InsertAnnotationDialog(State state)
    {
      this.state = state;

      InitializeComponent();

      foreach (Item item in state.Items)
      {
        tagListBox.Items.Add(item);
      }
    }

    private void tagListBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      okButton.Enabled = false;

      String model = (tagListBox.SelectedItem as Item).GraphicItem.Model;

      annotationListBox.Items.Clear();

      foreach (ThingStencil thingStencil in state.Config.ThingStencils.Values)
      {
        if (thingStencil.Model == model)
        {
          annotationListBox.Items.Add(thingStencil);
          annotationListBox.Enabled = true;
        }
      }
    }

    private void annotationListBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      ThingStencil thingStencil = annotationListBox.SelectedItem as ThingStencil;
      thingXaml = thingStencil.Xaml;
      thingXaml = Regex.Replace(thingXaml, @"\[\[TAG\]\]", (tagListBox.SelectedItem as Item).Tag);

      defaultWidth = thingStencil.defaultSize.Width;
      defaultHeight = thingStencil.defaultSize.Height;

      okButton.Enabled = true;
    }

  }
}