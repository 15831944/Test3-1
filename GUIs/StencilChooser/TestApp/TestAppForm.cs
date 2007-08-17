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
    public TestAppForm(Dictionary<String, Bitmap> modelThumbnails, Dictionary<String, Bitmap> graphicThumbnails, 
                       Dictionary<String, ModelStencil> modelStencils, Dictionary<String, GraphicStencil> graphicStencils)
    {
      InitializeComponent(modelThumbnails, graphicThumbnails, modelStencils, graphicStencils);
    }
  }
}