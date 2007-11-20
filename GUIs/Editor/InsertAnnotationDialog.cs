
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Protocol;
using System.Text.RegularExpressions;

namespace SysCAD.Editor
{

  internal partial class InsertAnnotationDialog : Form
  {

    private double defaultHeight;

    private double defaultWidth;

    private State state;

    private String thingTag;
    private String thingXaml;

    public InsertAnnotationDialog(State state)
    {
      this.state = state;

      InitializeComponent();

      foreach (EditorNode item in state.Items)
      {
        tagListBox.Items.Add(item);
      }
    }

    private void annotationListBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      ThingStencil thingStencil = annotationListBox.SelectedItem as ThingStencil;
      thingXaml = thingStencil.Xaml;
      thingXaml = Regex.Replace(thingXaml, @"\[\[TAG\]\]", (tagListBox.SelectedItem as EditorNode).Tag);

      defaultWidth = thingStencil.DefaultSize.Width;
      defaultHeight = thingStencil.DefaultSize.Height;

      okButton.Enabled = true;
    }

    public double DefaultHeight
    {
      get { return defaultHeight; }
      set { defaultHeight = value; }
    }

    public double DefaultWidth
    {
      get { return defaultWidth; }
      set { defaultWidth = value; }
    }

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

  }
}
