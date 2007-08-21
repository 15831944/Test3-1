using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SysCAD.Protocol;

namespace TestApp
{
  public partial class TestAppForm : Form
  {
    public TestAppForm(Dictionary<String, Bitmap> modelUnselectedThumbnails, Dictionary<String, Bitmap> modelSelectedThumbnails,
      Dictionary<String, Bitmap> graphicUnselectedThumbnails, Dictionary<String, Bitmap> graphicSelectedThumbnails,
      Dictionary<String, ModelStencil> modelStencils, Dictionary<String, GraphicStencil> graphicStencils)
    {
      InitializeComponent(modelUnselectedThumbnails, modelSelectedThumbnails,
        graphicUnselectedThumbnails, graphicSelectedThumbnails,
        modelStencils, graphicStencils);
    }
  }
}